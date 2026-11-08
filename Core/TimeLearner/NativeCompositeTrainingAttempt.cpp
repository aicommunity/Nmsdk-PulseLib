#include "NativeCompositeTrainingAttempt.h"
#include "../NNeuronTimeLearner.h"
#include "../NNeuronTimeLearnerBranch.h"
#include "../NPulseNeuron.h"
#include "ConfiguredNativeTraining.h"
#include "MatchedOmissionEvidenceJson.h"
#include "ModelArchive.h"
#include "ProbeEvidenceJson.h"
#include "NativeTrainingBindings.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QString Text(const std::string& value){return QString::fromStdString(value);}
std::string Hash(const QJsonValue& value){return NmsdkTools::CanonicalModelDataSha256(value).toStdString();}
QJsonArray Strings(const std::vector<std::string>& values){QJsonArray result;for(const auto& v:values)result.append(Text(v));return result;}
QJsonObject ProtocolBinding(const VerificationProtocol& p) {
    const auto& c=*p.compositeMeasurement;const auto& m=c.contextPolicy;const auto& g=c.geometryPolicy;
    QJsonArray windows,inputs;for(const auto& w:c.inputWindows)windows.append(QJsonArray{w.beginOffset,w.endOffset});
    for(const auto& i:p.inputs)inputs.append(QJsonArray{Text(i.id),Text(i.stimulusId)});
    const auto& s=*p.stimulusProfile;
    return {{"schema_version",4},{"method",Text(c.methodId)},{"contract_schema",int(c.schemaVersion)},
        {"routing",int(p.routing)},{"revision",Text(p.revision)},{"sample",Text(p.sampleId)},{"pattern",Text(p.patternHash)},
        {"origin",Text(p.originStimulusId)},{"inputs",inputs},{"stimuli",Strings(p.stimulusIds)},
        {"sample_gap",p.maximumSampleGap},{"minimum_amplitude",p.minimumAmplitude},{"windows",windows},
        {"source",QJsonArray{Text(s.generatorClass),s.amplitude,s.pulseLength,s.frequencyHz,s.frequencyDeviation,s.useTransitSignal,s.usePatternOutput}},
        {"context_policy",QJsonArray{int(m.schemaVersion),double(m.comparisonRadiusTicks),double(m.maximumSamples),
            double(m.maximumTotalSamples),double(m.maximumNativeProbes),m.timeComparisonTolerance,m.baselineAbsolute,
            m.repeatabilityAbsolute,m.repeatabilityRelative,m.influenceAbsolute,m.influenceRelative}},
        {"geometry_policy",QJsonArray{int(g.schemaVersion),double(g.maximumInputs),double(g.maximumSamples),double(g.maximumTotalSamples),g.repeatabilityAbsolute}}};
}
std::atomic<std::size_t> AttemptGeneration{0};
std::size_t NextAttemptGeneration() {
    auto current=AttemptGeneration.load();
    while(current!=std::numeric_limits<std::size_t>::max())
        if(AttemptGeneration.compare_exchange_weak(current,current+1))return current+1;
    throw std::invalid_argument("composite_attempt_generation_overflow");
}
}
template<class Owner> void NativeCompositeTrainingAttempt::Bind(Owner& owner) {
    Require(owner.NativeTrainingOwned && bool(owner.NativeModelDeclaration),"composite_train_requires_native_owned_model");
    Require(owner.IsReady() && owner.IsInit() && owner.NativeEditFailure.empty(),"composite_train_requires_ready_native_owner");
    Require(!owner.NativeRecognition,"composite_train_recognition_requires_explicit_solo_import_or_cold_model");
    Require(owner.NativeTrainingControl.State()!=NativeTrainingState::CapturingReferences &&
        owner.NativeTrainingControl.State()!=NativeTrainingState::Searching,"composite_train_owner_service_already_active");
    ownerLife=owner.ObserveLifecycle();
    initialEpoch=owner.NativeTrainingControl.ModelEpoch();
    bindings=[&owner](){return owner.NativeProbeModelBindings();};
    declaration=[&owner](){return owner.NativeModelDeclaration?EncodeNativeModelSpec(*owner.NativeModelDeclaration):QJsonObject{};};
    actualModelTime=[&owner](){Require(bool(owner.Environment),"composite_train_missing_native_environment");
        return owner.Environment->GetTime().GetDoubleTime();};
    capacitanceWitness=[&owner](){return owner.ReadNativeTrainingCapacitanceWitness();};
    adapter.queue=[&owner](auto request){owner.NativeProbeControl.Queue(std::move(request));};
    adapter.probeState=[&owner](){return owner.NativeProbeControl.Status();};
    adapter.takeResult=[&owner](){return owner.NativeProbeControl.TakeResult();};
    adapter.cancel=[&owner](const auto& reason){owner.NativeProbeControl.Cancel(reason);};
    adapter.requested=[this,&owner](){return !cancelled && ownerLife.IsCurrent() && owner.IsNeedToTrain.GetData();};
    adapter.readModelRevision=[this](){
        Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");
        const auto b=bindings();const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(bool(source),"composite_train_missing_source");
        return revisionObserver.Read(b.neuron,{{&source->Output,"burst"}}).toStdString();
    };
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearner& o,VerificationProtocol p,
    CompositeAttemptLimits l,double m,double w){Bind(o);Initialize(std::move(p),l,m,w);}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& o,VerificationProtocol p,
    CompositeAttemptLimits l,double m,double w){Bind(o);Initialize(std::move(p),l,m,w);}
NativeCompositeTrainingAttempt::~NativeCompositeTrainingAttempt(){
    try {if(Running())group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},"composite_train_attempt_destroyed");}catch(...){}
    lease.reset();
}
void NativeCompositeTrainingAttempt::Initialize(VerificationProtocol p,CompositeAttemptLimits l,double m,double w) {
    ValidateVerificationProtocol(p);ValidateCompositeMeasurementContract(p);limits=l;
    Require(adapter.requested(),"composite_train_requires_actual_training_request");
    Require(l.maximumGroups && l.maximumNativeProbes && l.maximumTotalSamples && std::isfinite(m) && std::isfinite(w) &&
        std::isfinite(l.maximumModelSeconds) && l.maximumModelSeconds>0. && std::isfinite(l.maximumWallSeconds) &&
        l.maximumWallSeconds>0.,"invalid_composite_attempt_limits");
    Require(m==actualModelTime(),"composite_train_clock_not_actual_native_time");
    Require(l.maximumGroups<=9007199254740991ULL && l.maximumNativeProbes<=9007199254740991ULL &&
        l.maximumTotalSamples<=9007199254740991ULL && initialEpoch<=9007199254740991ULL,"inexact_composite_attempt_limits");
    protocol=std::make_shared<const VerificationProtocol>(std::move(p));startModel=lastModel=m;startWall=lastWall=w;
    initialDeclaration=declaration();const auto b=bindings();
    Require(b.owner && b.neuron && b.dataset && b.inputCount==int(protocol->inputs.size()) &&
        b.branch==(protocol->routing==ProbeRouting::BranchSoloBurst),"composite_train_owner_protocol_mismatch");
    Require(adapter.probeState()!=NativeProbeStatus::Queued && adapter.probeState()!=NativeProbeStatus::Running,
        "composite_train_start_during_native_probe");
    const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(source),"composite_train_missing_source");
    const StimulusProfile actualSource{source->GetCompClassName(),source->Amplitude.GetData(),source->PulseLength.GetData(),
        double(source->GetTimeStep()),source->FrequencyDeviation.GetData(),source->UseTransitSignal.GetData(),source->UsePatternOutput.GetData()};
    Require(protocol->stimulusProfile==actualSource,"composite_train_actual_source_profile_mismatch");
    Require(b.dataset->MatrixData.GetRows()==b.inputCount && b.dataset->MatrixData.GetCols()==1,
        "composite_train_initial_source_dimensions");
    QJsonArray appliedIsi;for(int i=0;i<b.inputCount;++i)appliedIsi.append(b.dataset->MatrixData(i,0));
    Require(Hash(appliedIsi)==protocol->patternHash,"composite_train_initial_source_pattern_mismatch");
    const auto identity=NmsdkTools::IdentifyModel(b.neuron,{{&source->Output,"burst"}});
    Require(identity["valid"].toBool(),"composite_train_invalid_initial_identity");initialRevision=identity["sha256"].toString().toStdString();
    for(const char* key:{"neuron_class","dataset_class","generator_class"})
        if(initialDeclaration[key].isString())imageRoots.push_back(initialDeclaration[key].toString().toStdString());
    const auto images=NmsdkTools::CaptureModelImageDefinitions(b.neuron,{{&source->Output,"burst"}},imageRoots);
    storageFingerprint=NmsdkTools::ModelImageDefinitionsFingerprint(images).toStdString();
    const auto c=capacitanceWitness();if(c){initialCapacitanceOrigin=c->parameters.origin;initialCapacitanceMaximumRelativeChange=c->parameters.maximumRelativeChange;}
    sourceBinding=QJsonObject{{"kind","composite_train_initial_source_v1"},{"initial_model",identity},{"initial_epoch",double(initialEpoch)},
        {"model_spec",initialDeclaration},{"storage_images_sha256",Text(storageFingerprint)},
        {"storage_images",images},{"applied_source_isi",appliedIsi},
        {"pattern_hash",Text(protocol->patternHash)},{"optional_C0_origin",Text(initialCapacitanceOrigin)},
        {"optional_C0_witness",c?QJsonValue(EncodeNativeTrainingCapacitanceWitness(*c)):QJsonValue(QJsonValue::Null)},
        {"actual_source_profile",QJsonArray{Text(actualSource.generatorClass),actualSource.amplitude,actualSource.pulseLength,
            actualSource.frequencyHz,actualSource.frequencyDeviation,actualSource.useTransitSignal,actualSource.usePatternOutput}}};
    sourceOrigin=Hash(sourceBinding);
    generation=NextAttemptGeneration();
    // Process-local ownership identity; unlike semantic proof content this is
    // intentionally not a portable model identity or a physics attestation.
    ownerScope=Hash(QJsonArray{Text(sourceOrigin),QString::number(qulonglong(generation))});
    lease=std::make_shared<CompositeTrainingDetail::LiveLease>();
    lease->requireCurrent=[this](){
        Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
        if(!adapter.requested())throw CompositeTrainingStopped();
    };
    CheckBoundary();
}
void NativeCompositeTrainingAttempt::CheckClock(double m,double w) {
    Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
    if(!adapter.requested())throw CompositeTrainingStopped();
    Require(m==actualModelTime(),"composite_train_clock_not_actual_native_time");
    Require(std::isfinite(m) && std::isfinite(w) && m>=lastModel && w>=lastWall,"composite_attempt_nonmonotonic_clock");
    lastModel=m;lastWall=w;
    Require(m-startModel<limits.maximumModelSeconds && w-startWall<limits.maximumWallSeconds,"composite_attempt_time_budget_exhausted");
}
std::string NativeCompositeTrainingAttempt::CheckBoundary(QJsonObject* retained,std::optional<TrainingCapacitance>* parameters) {
    Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");
    if(!adapter.requested())throw CompositeTrainingStopped();
    Require(declaration()==initialDeclaration,"composite_train_source_owner_changed");
    const auto b=bindings();const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(source),"composite_train_missing_source");
    const auto images=NmsdkTools::CaptureModelImageDefinitions(b.neuron,{{&source->Output,"burst"}},imageRoots);
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(images).toStdString()==storageFingerprint,"composite_train_storage_physics_changed");
    const auto current=capacitanceWitness();
    if(current)Require(current->parameters.origin==initialCapacitanceOrigin &&
        current->parameters.maximumRelativeChange==initialCapacitanceMaximumRelativeChange,"composite_train_C0_origin_or_policy_changed");
    else Require(initialCapacitanceOrigin.empty(),"composite_train_C0_state_lost");
    const auto identity=NmsdkTools::IdentifyModel(b.neuron,{{&source->Output,"burst"}});
    Require(identity["valid"].toBool() && identity["sha256"].toString().toStdString()==adapter.readModelRevision(),
        "composite_train_candidate_full_identity_mismatch");
    const QJsonObject binding{{"kind","composite_train_candidate_binding_v1"},{"source_origin",Text(sourceOrigin)},
        {"model_identity",identity},{"storage_images_sha256",Text(storageFingerprint)},
        {"optional_C0_witness",current?QJsonValue(EncodeNativeTrainingCapacitanceWitness(*current)):QJsonValue(QJsonValue::Null)}};
    if(retained)*retained=binding;
    if(parameters)*parameters=current?std::optional<TrainingCapacitance>(current->parameters):std::nullopt;
    return Hash(binding);
}
void NativeCompositeTrainingAttempt::StartMeasurement(const VerificationRequest& logical,CompositeMeasurementPurpose purpose,
    NativeProbeRequest prototype,double modelLimit,double wallLimit,double modelTime,double wallTime,
    std::function<void(const PreparedCompositeMeasurement&)> sink) {
    Require(!Running(),"composite_train_group_already_running");
    if(group && !closed)CloseGroup();
    measurement.reset();selection.reset();prepared.reset();rejection.clear();
    CheckClock(modelTime,wallTime);
    Require(issuedGroups<limits.maximumGroups && IssuedNativeProbes()<=limits.maximumNativeProbes &&
        protocol->inputs.size()+2<=limits.maximumNativeProbes-IssuedNativeProbes(),"composite_attempt_native_or_group_budget_exhausted");
    Require(logical.inputIndex<protocol->inputs.size() && logical.inputId==protocol->inputs[logical.inputIndex].id &&
        logical.routing==protocol->routing && logical.protocolRevision==protocol->revision && logical.sampleId==protocol->sampleId &&
        logical.patternHash==protocol->patternHash && !logical.runId.empty() && !logical.trialId.empty() &&
        logicalTrials.insert(logical.trialId).second && logical.baseModelRevision==adapter.readModelRevision(),
        "composite_train_logical_request_mismatch_or_replay");
    Require((purpose==CompositeMeasurementPurpose::Reference || purpose==CompositeMeasurementPurpose::Search ||
        purpose==CompositeMeasurementPurpose::Final) && logical.epoch<=9007199254740991ULL,
        "composite_train_invalid_purpose_or_epoch");
    Require(purpose!=CompositeMeasurementPurpose::Reference || (logical.baseModelRevision==initialRevision && logical.epoch==initialEpoch),
        "composite_train_reference_requires_original_capture_model");
    const auto& contract=*protocol->compositeMeasurement;const auto window=contract.inputWindows.at(logical.inputIndex);
    Require(std::isfinite(modelLimit) && modelLimit>0. && modelLimit<=limits.maximumModelSeconds-(modelTime-startModel) &&
        std::isfinite(wallLimit) && wallLimit>0. && wallLimit<=limits.maximumWallSeconds-(wallTime-startWall),"composite_train_group_time_budget");
    Require(prototype.specification.schemaVersion==3 && prototype.specification.end>=window.endOffset &&
        prototype.specification.stimulusProfile==protocol->stimulusProfile && prototype.specification.minimumAmplitude==protocol->minimumAmplitude &&
        prototype.specification.maximumSampleGap==protocol->maximumSampleGap && prototype.specification.patternHash==protocol->patternHash &&
        prototype.specification.expectedStimulusIds==protocol->stimulusIds,"composite_train_prototype_protocol_mismatch");
    for(const auto& declared:contract.inputWindows)
        Require(declared.endOffset<=prototype.specification.end,"composite_train_declared_window_exceeds_duration");
    const auto charge=contract.contextPolicy.maximumTotalSamples;
    Require(totalSamples<=limits.maximumTotalSamples && charge<=limits.maximumTotalSamples-totalSamples,"composite_attempt_sample_budget_exhausted");
    closed=false;activeSamples=0;groupStartWall=wallTime;groupWallLimit=wallLimit;
    auto id=std::make_shared<CompositeTrainingDetail::Identity>();id->protocol=protocol;id->lease=lease;
    QJsonObject candidate;id->ownerScope=ownerScope;id->sourceOrigin=sourceOrigin;id->candidateWitnessId=CheckBoundary(&candidate,&id->capacitance);
    id->candidateBindingJson=QJsonDocument(candidate).toJson(QJsonDocument::Compact).toStdString();id->generation=generation;
    id->runId=logical.runId;id->trialId=logical.trialId;id->groupId=logical.trialId+"/composite/"+std::to_string(++issuedGroups);
    id->baseRevision=logical.baseModelRevision;id->modelEpoch=logical.epoch;id->inputIndex=logical.inputIndex;
    id->channel=protocol->routing==ProbeRouting::ClassicBurst?logical.inputIndex:0;id->purpose=purpose;activeIdentity=id;
    id->requireLiveModel=[this,weak=std::weak_ptr<const CompositeTrainingDetail::Identity>(id)](bool measurementClosed){
        const auto identity=weak.lock();Require(bool(identity),"expired_composite_measurement_identity");
        if(measurementClosed) {
            CheckClock(actualModelTime(),NativeTrainingWallTime());
            Require(adapter.readModelRevision()==identity->baseRevision && CheckBoundary()==identity->candidateWitnessId,
                "composite_live_model_or_candidate_witness_changed");
            CheckClock(actualModelTime(),NativeTrainingWallTime());
        }
        else Require(Running() && !closed && activeIdentity.get()==identity.get() &&
            adapter.readModelRevision()==identity->effectiveRevision,"composite_preparation_not_current_live_group");
    };
    prototype.routing=logical.routing;prototype.activeInput=int(logical.inputIndex);prototype.inputId=logical.inputId;
    prototype.protocolRevision=logical.protocolRevision;prototype.baseModelRevision=logical.baseModelRevision;prototype.modelEpoch=logical.epoch;
    prototype.specification.runId=logical.runId;prototype.specification.sampleId=logical.sampleId;
    prototype.readModelRevision=adapter.readModelRevision;
    prototype.readFullModelRevision=[this](){
        Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");const auto b=bindings();
        const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(bool(source),"composite_train_missing_source");const auto full=NmsdkTools::IdentifyModel(b.neuron,{{&source->Output,"burst"}});
        Require(full["valid"].toBool(),"composite_train_invalid_full_identity");return full["sha256"].toString().toStdString();
    };
    const auto caller=prototype.onPrepared;
    prototype.onPrepared=[this,id,sink,caller](const auto& p,const auto& s){
        if(!prepared){
            Require(p.baseModelRevision==id->baseRevision && p.modelEpoch==id->modelEpoch &&
                p.inputId==protocol->inputs[id->inputIndex].id && p.protocolRevision==protocol->revision &&
                s.stimulusProfile==protocol->stimulusProfile,"composite_train_preparation_changed");
            id->effectiveRevision=p.effectiveModelRevision;prepared=PreparedCompositeMeasurement(id);if(sink)sink(*prepared);
        }
        if(caller)caller(p,s);
    };
    const auto sampleSink=prototype.onSample;
    prototype.onSample=[this,sampleSink](const NativeProbeSample& sample){
        Require(activeSamples<limits.maximumTotalSamples-totalSamples,"composite_attempt_sample_budget_exhausted");
        ++activeSamples;if(sampleSink)sampleSink(sample);
    };
    NativeMatchedOmissionPlan plan;plan.fullBurst=std::move(prototype);plan.groupId=id->groupId;
    plan.policy=contract.contextPolicy;plan.channel=id->channel;plan.maximumModelSeconds=modelLimit;plan.maximumWallSeconds=wallLimit;
    const auto b=bindings();for(const auto& soma:b.somas)plan.physicalChannelPaths.push_back(soma->GetLongName(b.owner));
    group=std::make_unique<NativeMatchedOmissionGroup>();group->Start(std::move(plan),b,modelTime,wallTime);
}
bool NativeCompositeTrainingAttempt::Running() const {return group && group->State()==NativeMatchedOmissionState::Running;}
std::size_t NativeCompositeTrainingAttempt::IssuedNativeProbes() const {return completedNativeProbes+(group && !closed?group->IssuedNativeProbes():0);}
void NativeCompositeTrainingAttempt::BeforeTick(double m,double w) {
    if(!Running())return;
    try {CheckClock(m,w);group->BeforeTick(adapter,m,w);}
    catch(const std::exception& e){group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    if(!Running())CloseGroup();
}
void NativeCompositeTrainingAttempt::AfterTick(double m,double w) {
    if(!Running())return;
    try {CheckClock(m,w);group->AfterTick(adapter,m,w);}
    catch(const std::exception& e){group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    if(!Running())CloseGroup();
}
void NativeCompositeTrainingAttempt::CloseGroup() {
    if(closed || !group)return;closed=true;completedNativeProbes+=group->IssuedNativeProbes();
    const auto& result=group->Result();
    try {
        // Includes samples from a cancelled/current partial raw probe, even
        // when its closed trace never reached the group's witness inventory.
        Require(activeSamples<=limits.maximumTotalSamples-totalSamples,"composite_attempt_sample_budget_exhausted");
        totalSamples+=activeSamples;
        Require(group->State()==NativeMatchedOmissionState::Completed && prepared,"composite_train_group_not_completed");
        Require(CheckBoundary()==activeIdentity->candidateWitnessId && adapter.readModelRevision()==activeIdentity->baseRevision,
            "composite_train_candidate_boundary_changed");
        NativeDeclaredWindowBasinSpecification spec;spec.groupId=activeIdentity->groupId;
        spec.inputId=protocol->inputs[activeIdentity->inputIndex].id;spec.assignedInput=activeIdentity->inputIndex;spec.channel=activeIdentity->channel;
        const auto& window=protocol->compositeMeasurement->inputWindows[activeIdentity->inputIndex];
        spec.windowBeginOffset=window.beginOffset;spec.windowEndOffset=window.endOffset;
        selection=SelectNativeDeclaredWindowBasinMaximum(spec,protocol->compositeMeasurement->geometryPolicy,result);
        const auto& selected=*selection;
        if(selected.status!=NativeDeclaredWindowBasinStatus::Completed || !selected.geometry)
            throw std::invalid_argument(selected.reasons.empty()?"composite_train_native_assignment_rejected":selected.reasons.front());
        if(selected.geometry->validity!=DeclaredWindowBasinValidity::Valid || !selected.geometry->measuredMaximum)
            throw std::invalid_argument(std::string("composite_train_assignment_")+DeclaredWindowBasinValidityName(selected.geometry->validity));
        const auto& peak=*selected.geometry->measuredMaximum;const auto& full=result.witness.fullBefore.observation;
        const auto origin=std::find(protocol->stimulusIds.begin(),protocol->stimulusIds.end(),protocol->originStimulusId)-protocol->stimulusIds.begin();
        const double originTime=full.events.at(origin).time;
        QJsonArray primary;
        std::vector<const MatchedOmissionTrace*> traces{&result.witness.fullBefore};
        for(const auto& c:result.witness.omitEachInput)traces.push_back(&c);traces.push_back(&result.witness.fullAfter);
        for(std::size_t i=0;i<traces.size();++i){
            const auto bytes=EncodeMatchedOmissionPrimaryTrace(*traces[i],result.packets[i]);
            primary.append(QJsonObject{{"native",EncodeNativeProbeEvidence(result.packets[i])},
                {"csv_sha256",QString::fromLatin1(QCryptographicHash::hash(bytes,QCryptographicHash::Sha256).toHex())}});
        }
        const auto proof=Hash(QJsonObject{{"kind","live-composite-training-measurement-v1"},{"method_binding",ProtocolBinding(*protocol)},
            {"source_origin",Text(sourceOrigin)},{"candidate_witness",Text(activeIdentity->candidateWitnessId)},
            {"live_attempt_binding",Text(ownerScope)},
            {"attempt_limits",QJsonArray{double(limits.maximumGroups),double(limits.maximumNativeProbes),
                double(limits.maximumTotalSamples),limits.maximumModelSeconds,limits.maximumWallSeconds}},
            {"group_limits",QJsonArray{result.maximumModelSeconds,result.maximumWallSeconds}},
            {"logical_trial",Text(activeIdentity->trialId)},{"group_id",Text(activeIdentity->groupId)},
            {"purpose",int(activeIdentity->purpose)},{"primary",primary},{"raw",peak.rawPotential},{"time",peak.time},{"plateau_end",peak.plateauEnd}});
        // Native wall budget includes replay/selection/hash work. This is a
        // live steady-clock check, not an elapsed-time claim decoded from old
        // group declarations. Supplied hook clocks must use this same clock.
        const double now=NativeTrainingWallTime();CheckClock(lastModel,now);
        Require(now-groupStartWall<groupWallLimit,"composite_train_group_wall_budget_exhausted");
        measurement=ValidatedCompositeMeasurement(*prepared,peak.rawPotential,peak.time-originTime,peak.plateauEnd-originTime,proof);
    }
    catch(const std::exception& e){measurement.reset();rejection=e.what();if(!result.reason.empty())rejection+=";"+result.reason;}
}
void NativeCompositeTrainingAttempt::Cancel(const std::string& reason) {
    if(Running())group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},reason);
    CloseGroup();cancelled=true;lease.reset();measurement.reset();
    if(rejection.empty())rejection=reason;
}
}
