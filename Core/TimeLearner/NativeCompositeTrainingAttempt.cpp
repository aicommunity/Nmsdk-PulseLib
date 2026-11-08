#include "NativeCompositeTrainingAttempt.h"
#include "CompositePortableTrainingProof.h"
#include "CompositeTrainingDeclarations.h"
#include "NativeCompositeTrainingEvidence.h"
#include "../NNeuronTimeLearner.h"
#include "../NNeuronTimeLearnerBranch.h"
#include "../NPulseNeuron.h"
#include "ConfiguredNativeTraining.h"
#include "MatchedOmissionEvidenceJson.h"
#include "ModelArchive.h"
#include "ProbeEvidenceJson.h"
#include "NativeTrainingBindings.h"
#include "CandidateWindowProposalReplay.h"
#include "NativeCandidateWindowBindings.h"
#include "CompositeSourceValidation.h"
#include "CandidateWindowMeasurementEvidence.h"
#include "CandidateWindowLogicalContext.h"
#include "CandidateWindowPortableReferences.h"
#include "CompositePrimaryBudget.h"
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
    observeOwner=[&owner](){return owner.ObserveLifecycle();};
    requireOwnerControl=[this,&owner](){
        const auto state=owner.NativeTrainingControl.State();
        if(state==NativeTrainingState::CapturingReferences || state==NativeTrainingState::Searching)
            Require(owner.NativeTrainingControl.composite.get()==this,"another_native_training_service_owns_executor");
    };
    initialEpoch=owner.NativeTrainingControl.ModelEpoch();
    bindings=[&owner](){return owner.NativeProbeModelBindings();};
    declaration=[&owner](){return owner.NativeModelDeclaration?EncodeNativeModelSpec(*owner.NativeModelDeclaration):QJsonObject{};};
    actualModelTime=[&owner](){Require(bool(owner.Environment),"composite_train_missing_native_environment");
        return owner.Environment->GetTime().GetDoubleTime();};
    capacitanceWitness=[&owner](){return owner.ReadNativeTrainingCapacitanceWitness();};
    readWindowParameters=[&owner](){return owner.ReadNativeParameters();};
    adapter.queue=[this,&owner](auto request){
        Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");
        if(beforeNativeQueue)beforeNativeQueue(request);
        Require(!cancelled && ownerLife.IsCurrent(),"composite_train_queue_interrupted");
        ownedRawRun=request.specification.runId;ownedRawTrial=request.specification.trialId;
        // Only the executor's copy retains the actor. Its reusable group
        // prototype must not form actor -> group -> callback -> actor cycles.
        if(auto keep=weak_from_this().lock()) {
            const auto prepared=request.onPrepared;
            const auto sample=request.onSample;
            request.onPrepared=[keep,prepared](const auto& p,const auto& s){if(prepared)prepared(p,s);};
            request.onSample=[keep,sample](const auto& frame){if(sample)sample(frame);};
        }
        owner.NativeProbeControl.Queue(std::move(request));
        ownedRawControlGeneration=owner.NativeProbeControl.ControlGeneration();
    };
    adapter.probeState=[&owner](){return owner.NativeProbeControl.Status();};
    adapter.takeResult=[this,&owner]()->std::optional<NativeProbeResult>{
        if(!ownerLife.IsCurrent() || owner.NativeProbeControl.ControlGeneration()!=ownedRawControlGeneration)return {};
        const auto& result=owner.NativeProbeControl.Result();
        if(!result || !result->requestedSpecification || result->requestedSpecification->runId!=ownedRawRun ||
            result->requestedSpecification->trialId!=ownedRawTrial)return {};
        return owner.NativeProbeControl.TakeResult();
    };
    adapter.cancel=[this,&owner](const auto& reason){if(ownerLife.IsCurrent() &&
        owner.NativeProbeControl.MatchesActiveTrial(ownedRawRun,ownedRawTrial,ownedRawControlGeneration))owner.NativeProbeControl.Cancel(reason);};
    cancelOwnedRaw=[this,&owner](const std::string& reason,bool lifecycleAuthority)->std::optional<NativeProbeResult>{
        // Rdk advances lifecycle before its owner hook. That hook may close
        // its exact still-existing request, but cannot revive a live lease or
        // touch a later request, even if run/trial strings were reused.
        if(!ownerLife.IsAlive() || (!lifecycleAuthority && !ownerLife.IsCurrent()))return {};
        auto& probe=owner.NativeProbeControl;
        if(probe.ControlGeneration()!=ownedRawControlGeneration)return {};
        if(probe.MatchesActiveTrial(ownedRawRun,ownedRawTrial,ownedRawControlGeneration))probe.Cancel(reason);
        if(probe.ControlGeneration()!=ownedRawControlGeneration)return {};
        const auto& result=probe.Result();
        if(!result || !result->requestedSpecification || result->requestedSpecification->runId!=ownedRawRun ||
            result->requestedSpecification->trialId!=ownedRawTrial)return {};
        return probe.TakeResult();
    };
    adapter.onRetainedPacket=[this](const NativeProbeResult& packet){RetainRawPacket(packet);};
    adapter.onNativeResult=[this](const NativeProbeResult& packet){if(afterNativeResult)afterNativeResult(packet);};
    adapter.requested=[this,&owner](){return !cancelled && ownerLife.IsCurrent() && owner.IsNeedToTrain.GetData();};
    adapter.readModelRevision=[this](){
        Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");
        const auto b=bindings();const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(bool(source),"composite_train_missing_source");
        return revisionObserver.Read(b.neuron.Get(),{{&source->Output,"burst"}}).toStdString();
    };
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearner& o,VerificationProtocol p,
    CompositeAttemptLimits l,double m,double w){Bind(o);Initialize(std::move(p),l,m,w);}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& o,VerificationProtocol p,
    CompositeAttemptLimits l,double m,double w){Bind(o);Initialize(std::move(p),l,m,w);}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearner& o,VerificationProtocol p,
    CompositeAttemptLimits l,NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    Bind(o);InitializeCandidateWindow(std::move(p),l,std::move(d),m,w);
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearner& o,const DecodedNativeCompositeArchive& a,
    CompositeAttemptLimits l,NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    Bind(o);AdoptCandidateWindowArchive(a,l,std::move(d),m,w);
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& o,const DecodedNativeCompositeArchive& a,
    CompositeAttemptLimits l,NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    Bind(o);AdoptCandidateWindowArchive(a,l,std::move(d),m,w);
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& o,VerificationProtocol p,
    CompositeAttemptLimits l,NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    Bind(o);InitializeCandidateWindow(std::move(p),l,std::move(d),m,w);
}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearner& o,const CompositePortableTrainingProof& proof,
    CompositeAttemptLimits l,double m,double w){Bind(o);AdoptOwned(proof,l,m,w);}
NativeCompositeTrainingAttempt::NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& o,const CompositePortableTrainingProof& proof,
    CompositeAttemptLimits l,double m,double w){Bind(o);AdoptOwned(proof,l,m,w);}
void NativeCompositeTrainingAttempt::AdoptOwned(const CompositePortableTrainingProof& proof,CompositeAttemptLimits l,double m,double w) {
    const auto& archive=proof.Archive();protocol=std::make_shared<const VerificationProtocol>(archive.Protocol());
    ValidateVerificationProtocol(*protocol);ValidateCompositeMeasurementContract(*protocol);
    sourceBinding=archive.SourceBinding();sourceOrigin=archive.SourceOrigin();ownerScope=archive.OwnerFamilyBinding();
    initialDeclaration=sourceBinding["model_spec"].toObject();initialRevision=sourceBinding["initial_model"].toObject()["sha256"].toString().toStdString();
    Require(proof.ModelEpoch()==initialEpoch,"composite_imported_epoch_changed");
    initialEpoch=std::size_t(sourceBinding["initial_epoch"].toDouble());storageFingerprint=sourceBinding["storage_images_sha256"].toString().toStdString();
    initialCapacitanceOrigin=sourceBinding["optional_C0_origin"].toString().toStdString();
    if(!sourceBinding["optional_C0_witness"].isNull())initialCapacitanceMaximumRelativeChange=
        DecodeNativeTrainingCapacitanceWitness(sourceBinding["optional_C0_witness"].toObject()).parameters.maximumRelativeChange;
    for(const auto& image:sourceBinding["storage_images"].toArray())imageRoots.push_back(image.toObject()["name"].toString().toStdString());
    const auto b=bindings();Require(b.owner && b.neuron && b.dataset && b.inputCount==int(protocol->inputs.size()) &&
        b.branch==(protocol->routing==ProbeRouting::BranchSoloBurst),"composite_imported_owner_protocol_mismatch");
    Require(adapter.probeState()!=NativeProbeStatus::Queued && adapter.probeState()!=NativeProbeStatus::Running,
        "composite_imported_owner_probe_active");
    RestartOwned(l,m,w,proof.ModelRevision());
}
void NativeCompositeTrainingAttempt::AdoptCandidateWindowArchive(const DecodedNativeCompositeArchive& archive,CompositeAttemptLimits l,
    NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    Require(!d.attemptId.empty() && d.attemptId!=archive.CurrentAttemptId() && d.maximumTotalPrimaryBytes>0 &&
        d.maximumTotalPrimaryBytes<=9007199254740991ULL,"candidate_window_restart_explicit_new_attempt_or_primary_budget");
    Require(std::none_of(archive.Records().begin(),archive.Records().end(),[&](const auto& r){return r.attemptId==d.attemptId;}),
        "candidate_window_restart_attempt_namespace_reused");
    auto references=CandidateWindowPortableReferenceFactory::Replay(archive);
    protocol=std::make_shared<const VerificationProtocol>(archive.Protocol());sourceBinding=archive.SourceBinding();
    sourceOrigin=archive.SourceOrigin();ownerScope=archive.OwnerFamilyBinding();windowDeclaration=std::move(d);
    initialDeclaration=sourceBinding["model_spec"].toObject();initialRevision=sourceBinding["initial_model"].toObject()["sha256"].toString().toStdString();
    initialEpoch=std::size_t(sourceBinding["initial_epoch"].toDouble());storageFingerprint=sourceBinding["storage_images_sha256"].toString().toStdString();
    initialCapacitanceOrigin=sourceBinding["optional_C0_origin"].toString().toStdString();
    if(!sourceBinding["optional_C0_witness"].isNull())initialCapacitanceMaximumRelativeChange=
        DecodeNativeTrainingCapacitanceWitness(sourceBinding["optional_C0_witness"].toObject()).parameters.maximumRelativeChange;
    for(const auto& image:sourceBinding["storage_images"].toArray())imageRoots.push_back(image.toObject()["name"].toString().toStdString());
    const auto b=bindings();Require(b.owner && b.neuron && b.dataset && b.inputCount==int(protocol->inputs.size()) &&
        b.branch==(protocol->routing==ProbeRouting::BranchSoloBurst),"candidate_window_restart_owner_protocol");
    Require(adapter.probeState()!=NativeProbeStatus::Queued && adapter.probeState()!=NativeProbeStatus::Running,"candidate_window_restart_probe_active");
    windowRecords=archive.Records();for(const auto& r:windowRecords) {
        const auto bytes=CompositeRecordPrimaryBytes(r);Require(bytes<=windowDeclaration->maximumTotalPrimaryBytes-windowPrimaryBytes,
            "candidate_window_restart_retained_primary_budget");windowPrimaryBytes+=bytes;
    }
    // No InitializeShared/IdentifyModel target capture occurs here. Every P0,
    // seed and raw target is retained from independently replayed ORIGINAL refs.
    initialWindowParameters=references.seeds.front().arithmetic.initialParameters;
    initialWindowCapacitance=references.seeds.front().arithmetic.initialCapacitance;
    windowReferences=std::make_shared<const CandidateWindowReferenceSet>(std::move(references));
    const auto revision=adapter.readModelRevision();RestartOwned(l,m,w,revision);
    QJsonObject actual;CheckBoundary(&actual);
    (void)CandidateWindowProposalReplayFactory::CheckCandidateBinding(*windowReferences,*protocol,actual);
    CheckClock(actualModelTime(),NativeTrainingWallTime());
}
NativeCompositeTrainingAttempt::~NativeCompositeTrainingAttempt(){
    // A destructor never invokes application/controller notification callbacks.
    afterNativeResult={};beforeNativeQueue={};
    try {SuspendOwned("composite_train_attempt_destroyed");}catch(...){}
    lease.reset();
}
void NativeCompositeTrainingAttempt::Initialize(VerificationProtocol p,CompositeAttemptLimits l,double m,double w) {
    ValidateVerificationProtocol(p);ValidateCompositeMeasurementContract(p);InitializeShared(std::move(p),l,m,w);
}
void NativeCompositeTrainingAttempt::InitializeCandidateWindow(VerificationProtocol p,CompositeAttemptLimits l,
    NativeCandidateWindowAttemptDeclaration d,double m,double w) {
    ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    Require(!d.attemptId.empty() && d.maximumTotalPrimaryBytes>0 && d.maximumTotalPrimaryBytes<=9007199254740991ULL,
        "candidate_window_explicit_attempt_or_primary_budget");
    windowDeclaration=std::move(d);InitializeShared(std::move(p),l,m,w);
    CompositeEvidenceDetail::ValidateInitialSource(*protocol,sourceBinding,sourceOrigin);
    initialWindowParameters=readWindowParameters();const auto c=capacitanceWitness();
    initialWindowCapacitance=c?std::optional<TrainingCapacitance>(c->parameters):std::nullopt;
    stagedWindowSeeds.resize(protocol->inputs.size());stagedWindowMeasurements.resize(protocol->inputs.size());
    stagedWindowGroups.resize(protocol->inputs.size());
}
void NativeCompositeTrainingAttempt::InitializeShared(VerificationProtocol p,CompositeAttemptLimits l,double m,double w) {
    limits=l;
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
    const auto identity=NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}});
    Require(identity["valid"].toBool(),"composite_train_invalid_initial_identity");initialRevision=identity["sha256"].toString().toStdString();
    for(const char* key:{"neuron_class","dataset_class","generator_class"})
        if(initialDeclaration[key].isString())imageRoots.push_back(initialDeclaration[key].toString().toStdString());
    const auto c=capacitanceWitness();
    if(c)for(const auto& definition:QJsonDocument::fromJson(QByteArray::fromStdString(c->nativeStateJson)).object()["baseline"].toObject()["storage_images"].toArray())
        imageRoots.push_back(definition.toObject()["name"].toString().toStdString());
    const auto datasetSnapshot=NmsdkTools::ExportDatasetSnapshot(b.dataset.Get());
    for(const auto& cls:datasetSnapshot["required_class_names"].toArray())imageRoots.push_back(cls.toString().toStdString());
    const auto images=NmsdkTools::CaptureModelImageDefinitions(b.neuron.Get(),{{&source->Output,"burst"}},imageRoots);
    storageFingerprint=NmsdkTools::ModelImageDefinitionsFingerprint(images).toStdString();
    if(c){initialCapacitanceOrigin=c->parameters.origin;initialCapacitanceMaximumRelativeChange=c->parameters.maximumRelativeChange;}
    sourceBinding=QJsonObject{{"kind","composite_train_initial_source_v1"},{"schema_version",2},{"initial_model",identity},
        {"initial_native_snapshot",NmsdkTools::ExportPhysicalGraphForEdit(b.neuron.Get(),{{&source->Output,"burst"}})},
        {"initial_dataset",datasetSnapshot},{"initial_epoch",double(initialEpoch)},
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
    liveAttemptBinding=Hash(QJsonArray{Text(ownerScope),QString::number(qulonglong(generation))});
    lease=std::make_shared<CompositeTrainingDetail::LiveLease>();
    lease->requireCurrent=[this](){
        Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
        if(!adapter.requested())throw CompositeTrainingStopped();
    };
    CheckBoundary();
}
void NativeCompositeTrainingAttempt::CheckClock(double m,double w) {
    Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
    requireOwnerControl();
    if(!adapter.requested())throw CompositeTrainingStopped();
    Require(m==actualModelTime(),"composite_train_clock_not_actual_native_time");
    Require(std::isfinite(m) && std::isfinite(w) && m>=lastModel && w>=lastWall,"composite_attempt_nonmonotonic_clock");
    lastModel=m;lastWall=w;
    if(m-startModel>=limits.maximumModelSeconds || w-startWall>=limits.maximumWallSeconds)
        throw CompositeTrainingBudgetExhausted("composite_attempt_time_budget_exhausted");
}
std::string NativeCompositeTrainingAttempt::CheckBoundary(QJsonObject* retained,std::optional<TrainingCapacitance>* parameters) {
    Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");
    requireOwnerControl();
    if(!adapter.requested())throw CompositeTrainingStopped();
    Require(declaration()==initialDeclaration,"composite_train_source_owner_changed");
    const auto b=bindings();const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(source),"composite_train_missing_source");
    // The neuron-only semantic identity declares a boundary port, not the
    // external dataset/generator's numeric parameters. Recheck the restored
    // actual source independently before mint/consume or owned warm restart.
    const auto datasetIdentity=NmsdkTools::IdentifyModel(b.dataset.Get());
    Require(datasetIdentity["valid"].toBool() && datasetIdentity["model"]==sourceBinding["initial_dataset"].toObject()["identity"].toObject()["model"],
        "composite_train_actual_source_boundary_changed");
    const auto images=NmsdkTools::CaptureModelImageDefinitions(b.neuron.Get(),{{&source->Output,"burst"}},imageRoots);
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(images).toStdString()==storageFingerprint,"composite_train_storage_physics_changed");
    const auto current=capacitanceWitness();
    if(current)Require(current->parameters.origin==initialCapacitanceOrigin &&
        current->parameters.maximumRelativeChange==initialCapacitanceMaximumRelativeChange,"composite_train_C0_origin_or_policy_changed");
    else Require(initialCapacitanceOrigin.empty(),"composite_train_C0_state_lost");
    const auto identity=NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}});
    Require(identity["valid"].toBool() && identity["sha256"].toString().toStdString()==adapter.readModelRevision(),
        "composite_train_candidate_full_identity_mismatch");
    QJsonObject binding{{"kind","composite_train_candidate_binding_v1"},{"source_origin",Text(sourceOrigin)},
        {"model_identity",identity},{"storage_images_sha256",Text(storageFingerprint)},
        {"optional_C0_witness",current?QJsonValue(EncodeNativeTrainingCapacitanceWitness(*current)):QJsonValue(QJsonValue::Null)}};
    if(protocol->schemaVersion==5) {
        const auto values=readWindowParameters();std::vector<int> attachments;for(const auto& value:values)attachments.push_back(value.length);
        const auto physical=ReadNativeCandidateWindowPhysical(b,attachments,*protocol,DecodeNativeModelSpec(initialDeclaration),
            current?std::optional<TrainingCapacitance>(current->parameters):std::nullopt);
        Require(physical.parameters==values,"candidate_window_actual_parameter_readback_changed");
        binding["kind"]="composite_train_candidate_binding_v2";binding["schema_version"]=2;
        binding["physical_readback"]=EncodeCandidateWindowPhysicalReadback(physical);
    }
    if(retained)*retained=binding;
    if(parameters)*parameters=current?std::optional<TrainingCapacitance>(current->parameters):std::nullopt;
    return Hash(binding);
}
void NativeCompositeTrainingAttempt::StartMeasurement(const VerificationRequest& logical,CompositeMeasurementPurpose purpose,
    NativeProbeRequest prototype,double modelLimit,double wallLimit,double modelTime,double wallTime,
    std::function<void(const PreparedCompositeMeasurement&)> sink,
    std::function<void(const NativeProbeRequest&)> onQueue,std::function<void(const NativeProbeResult&)> onResult) {
    Require(!Running(),"composite_train_group_already_running");
    if(group && !closed)CloseGroup();
    // A failed preflight is not the previous group's data under a new label.
    group.reset();activeIdentity.reset();rawTrials.clear();
    measurement.reset();selection.reset();prepared.reset();rejection.clear();
    activeWindowSnapshot.reset();activeWindowPhysical.reset();activeWindowProposal.reset();activeWindowSpecification.reset();
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
    const bool windowMode=protocol->schemaVersion==5;
    Require(protocol->schemaVersion==4 || windowMode,"composite_train_explicit_protocol_required");
    const auto& contextPolicy=windowMode?protocol->candidateWindowMeasurement->contextPolicy:protocol->compositeMeasurement->contextPolicy;
    const auto& inputWindows=windowMode?protocol->candidateWindowMeasurement->referenceInputWindows:protocol->compositeMeasurement->inputWindows;
    const auto window=inputWindows.at(logical.inputIndex);
    if(windowMode)Require(windowDeclaration && (purpose==CompositeMeasurementPurpose::Reference || windowReferences),
        "candidate_window_complete_reference_set_required");
    if(windowMode && purpose==CompositeMeasurementPurpose::Reference)Require(!windowReferences && !stagedWindowSeeds.at(logical.inputIndex),
        "candidate_window_reference_already_captured");
    Require(std::isfinite(modelLimit) && modelLimit>0. && modelLimit<=limits.maximumModelSeconds-(modelTime-startModel) &&
        std::isfinite(wallLimit) && wallLimit>0. && wallLimit<=limits.maximumWallSeconds-(wallTime-startWall),"composite_train_group_time_budget");
    Require(prototype.specification.schemaVersion==3 &&
        ((windowMode && purpose!=CompositeMeasurementPurpose::Reference) || prototype.specification.end>=window.endOffset) &&
        prototype.specification.stimulusProfile==protocol->stimulusProfile && prototype.specification.minimumAmplitude==protocol->minimumAmplitude &&
        prototype.specification.maximumSampleGap==protocol->maximumSampleGap && prototype.specification.patternHash==protocol->patternHash &&
        prototype.specification.expectedStimulusIds==protocol->stimulusIds,"composite_train_prototype_protocol_mismatch");
    if(!windowMode || purpose==CompositeMeasurementPurpose::Reference)for(const auto& declared:inputWindows)
        Require(declared.endOffset<=prototype.specification.end,"composite_train_declared_window_exceeds_duration");
    const auto charge=contextPolicy.maximumTotalSamples;
    Require(totalSamples<=limits.maximumTotalSamples && charge<=limits.maximumTotalSamples-totalSamples,"composite_attempt_sample_budget_exhausted");
    if(windowMode) {
        const auto reservation=CompositePrimaryByteReservation(protocol->inputs.size(),protocol->routing==ProbeRouting::ClassicBurst?protocol->inputs.size():1,charge);
        if(windowPrimaryBytes>windowDeclaration->maximumTotalPrimaryBytes || reservation>windowDeclaration->maximumTotalPrimaryBytes-windowPrimaryBytes)
            throw CompositeTrainingBudgetExhausted("candidate_window_primary_byte_budget_exhausted");
    }
    closed=false;budgetExhausted=false;activeSamples=0;groupStartModel=modelTime;groupStartWall=wallTime;groupWallLimit=wallLimit;groupModelLimit=modelLimit;
    groupEndModel=modelTime;groupEndWall=wallTime;this->logical=logical;this->purpose=purpose;rawTrials.clear();
    rawDeliveryCursor=0;interruptedRawPackets.clear();ownedRawRun.clear();ownedRawTrial.clear();
    beforeNativeQueue=std::move(onQueue);afterNativeResult=std::move(onResult);
    auto id=std::make_shared<CompositeTrainingDetail::Identity>();id->protocol=protocol;id->lease=lease;
    QJsonObject candidate;id->ownerScope=ownerScope;id->sourceOrigin=sourceOrigin;id->candidateWitnessId=CheckBoundary(&candidate,&id->capacitance);
    id->candidateBindingJson=QJsonDocument(candidate).toJson(QJsonDocument::Compact).toStdString();id->generation=generation;
    id->runId=logical.runId;id->trialId=logical.trialId;id->groupId=logical.trialId+"/composite/"+
        std::to_string(windowMode?issuedGroups+1:++issuedGroups);
    id->baseRevision=logical.baseModelRevision;id->modelEpoch=logical.epoch;id->inputIndex=logical.inputIndex;
    id->channel=protocol->routing==ProbeRouting::ClassicBurst?logical.inputIndex:0;id->purpose=purpose;activeIdentity=id;
    if(windowMode) {
        ValidateCandidateWindowLogicalContext(*protocol,logical,windowDeclaration->attemptId,id->groupId,purpose);
        const auto parameters=readWindowParameters();std::vector<int> attachments;for(const auto& p:parameters)attachments.push_back(p.length);
        activeWindowPhysical=ReadNativeCandidateWindowPhysical(bindings(),attachments,*protocol,DecodeNativeModelSpec(initialDeclaration),id->capacitance);
        Require(EncodeCandidateWindowPhysicalReadback(*activeWindowPhysical)==candidate["physical_readback"],"candidate_window_snapshot_changed_before_queue");
        CandidateWindowBasinSpecification spec;spec.groupId=id->groupId;spec.inputId=logical.inputId;
        spec.assignedInput=logical.inputIndex;spec.channel=id->channel;
        if(purpose==CompositeMeasurementPurpose::Reference)spec.referenceTime=window;
        else {
            const auto checked=CandidateWindowProposalReplayFactory::CheckCandidateBinding(*windowReferences,*protocol,candidate);
            auto data=CandidateWindowProposalReplayFactory::Compute(*windowReferences,*protocol,checked,logical,windowDeclaration->attemptId,
                id->groupId,prototype.specification.maximumSamples,purpose);
            data=DecodeCandidateWindowProposalData(QJsonDocument(EncodeCandidateWindowProposalData(data)).toJson(QJsonDocument::Compact));
            const auto replay=CandidateWindowProposalReplayFactory::Replay(*windowReferences,*protocol,checked,logical,windowDeclaration->attemptId,
                id->groupId,prototype.specification.maximumSamples,purpose,data);
            activeWindowProposal=PreparedCandidateWindowProposal(replay.Record(),id);
            id->candidateWindowProposal=std::make_shared<const PreparedCandidateWindowProposal>(*activeWindowProposal);
            activeWindowSnapshot=NativeCandidateWindowSnapshot(replay.Record().candidate,*activeWindowPhysical,id->candidateBindingJson,id);
            spec.coordinateMode=CandidateWindowCoordinateMode::CandidateFrame;spec.candidateFrame=replay.Record().computation.window;
        }
        activeWindowSpecification=std::move(spec);
    }
    id->requireLiveModel=[this,weak=std::weak_ptr<const CompositeTrainingDetail::Identity>(id)](bool measurementClosed){
        const auto identity=weak.lock();Require(bool(identity),"expired_composite_measurement_identity");
        if(measurementClosed) {
            CheckClock(actualModelTime(),NativeTrainingWallTime());
            if(protocol->schemaVersion==5) {
                Require(bool(identity->requireNativeInstances),"candidate_window_live_instance_guard_missing");
                identity->requireNativeInstances();
            }
            Require(adapter.readModelRevision()==identity->baseRevision && CheckBoundary()==identity->candidateWitnessId,
                "composite_live_model_or_candidate_witness_changed");
            CheckClock(actualModelTime(),NativeTrainingWallTime());
        }
        else Require(Running() && !closed && activeIdentity.get()==identity.get() &&
            adapter.readModelRevision()==identity->effectiveRevision,"composite_preparation_not_current_live_group");
    };
    if(windowMode && purpose!=CompositeMeasurementPurpose::Reference)
        id->requireCandidateWindowAdmission=[this,weak=std::weak_ptr<const CompositeTrainingDetail::Identity>(id),
            groupModelStart=modelTime,groupWallStart=wallTime,modelBudget=modelLimit,wallBudget=wallLimit](
            const CandidateWindowReferenceSet& expected,bool) {
        Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
        const auto identity=weak.lock();Require(identity && windowReferences && windowDeclaration &&
            identity->candidateWindowProposal && identity->candidateWindowProposal->identity.lock()==identity &&
            expected.Protocol()==*protocol && expected.SourceOrigin()==sourceOrigin && expected.OwnerFamilyBinding()==ownerScope &&
            expected.ReferenceGroupIds()==windowReferences->ReferenceGroupIds() && expected.ReferenceProofIds()==windowReferences->ReferenceProofIds() &&
            expected.InitialModelRevision()==initialRevision && expected.InitialModelEpoch()==initialEpoch &&
            expected.TargetRawPotentials()==windowReferences->TargetRawPotentials(),"candidate_window_live_reference_or_proposal_changed");
        Require(bool(identity->requireNativeInstances),"candidate_window_live_instance_guard_missing");identity->requireNativeInstances();
        const auto checkDeadline=[&](){const auto model=actualModelTime(),wall=NativeTrainingWallTime();CheckClock(model,wall);
            if(model-groupModelStart>=modelBudget || wall-groupWallStart>=wallBudget)
                throw CompositeTrainingBudgetExhausted("candidate_window_group_admission_budget_exhausted");};
        checkDeadline();
        const auto& frozen=identity->candidateWindowProposal->Record();
        const auto checked=CandidateWindowProposalReplayFactory::CheckCandidateBinding(expected,*protocol,
            ParseStrictNativeJsonObject(identity->candidateBindingJson,128u*1024u*1024u));
        const VerificationRequest request{identity->runId,identity->trialId,protocol->inputs.at(identity->inputIndex).id,
            protocol->sampleId,protocol->patternHash,protocol->revision,identity->baseRevision,protocol->routing,identity->modelEpoch,identity->inputIndex};
        (void)CandidateWindowProposalReplayFactory::Replay(expected,*protocol,checked,request,windowDeclaration->attemptId,
            identity->groupId,frozen.candidate.declaredMaximumSamples,identity->purpose,frozen);
        checkDeadline();
    };
    prototype.routing=logical.routing;prototype.activeInput=int(logical.inputIndex);prototype.inputId=logical.inputId;
    prototype.protocolRevision=logical.protocolRevision;prototype.baseModelRevision=logical.baseModelRevision;prototype.modelEpoch=logical.epoch;
    prototype.specification.runId=logical.runId;prototype.specification.sampleId=logical.sampleId;
    prototype.readModelRevision=adapter.readModelRevision;
    prototype.readFullModelRevision=[this](){
        Require(ownerLife.IsCurrent(),"composite_train_owner_lifecycle_changed");const auto b=bindings();
        const auto source=b.dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(bool(source),"composite_train_missing_source");const auto full=NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}});
        Require(full["valid"].toBool(),"composite_train_invalid_full_identity");return full["sha256"].toString().toStdString();
    };
    const auto caller=prototype.onPrepared;
    prototype.onPrepared=[this,id,sink,caller](const auto& p,const auto& s){
        rawTrials.push_back({p,s,{},{}});
        if(protocol->schemaVersion==5) {
            Require(bool(activeWindowPhysical),"candidate_window_actual_snapshot_missing");
            ValidateNativeCandidateWindowPreparedSelector(bindings(),*activeWindowPhysical,*protocol,DecodeNativeModelSpec(initialDeclaration),id->inputIndex,p);
        }
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
        Require(!rawTrials.empty(),"composite_native_sample_without_preparation");
        ++activeSamples;rawTrials.back().samples.push_back(sample);if(sampleSink)sampleSink(sample);
    };
    NativeMatchedOmissionPlan plan;plan.fullBurst=std::move(prototype);plan.groupId=id->groupId;
    plan.policy=contextPolicy;plan.channel=id->channel;plan.maximumModelSeconds=modelLimit;plan.maximumWallSeconds=wallLimit;
    const auto b=bindings();for(const auto& soma:b.somas)plan.physicalChannelPaths.push_back(soma->GetLongName(b.owner));
    if(windowMode) {
        auto next=std::make_unique<NativeMatchedOmissionGroup>();next->Start(std::move(plan),b,modelTime,wallTime);
        id->requireNativeInstances=next->RetainInstanceGuard();group=std::move(next);++issuedGroups;
    } else {group=std::make_unique<NativeMatchedOmissionGroup>();group->Start(std::move(plan),b,modelTime,wallTime);}
}
bool NativeCompositeTrainingAttempt::Running() const {return group && group->State()==NativeMatchedOmissionState::Running;}
std::size_t NativeCompositeTrainingAttempt::IssuedNativeProbes() const {return completedNativeProbes+(group && !closed?group->IssuedNativeProbes():0);}
void NativeCompositeTrainingAttempt::BeforeTick(double m,double w) {
    if(!Running())return;
    try {CheckClock(m,w);group->BeforeTick(adapter,m,w);}
    catch(const CompositeTrainingBudgetExhausted& e){budgetExhausted=true;rejection=e.what();group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    catch(const std::exception& e){group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    if(cancelled)SuspendOwned(rejection.empty()?"native_composite_control_replaced":rejection);
    else if(!Running())CloseGroup();
}
void NativeCompositeTrainingAttempt::AfterTick(double m,double w) {
    if(!Running())return;
    try {CheckClock(m,w);group->AfterTick(adapter,m,w);}
    catch(const CompositeTrainingBudgetExhausted& e){budgetExhausted=true;rejection=e.what();group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    catch(const std::exception& e){group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},e.what());}
    if(cancelled)SuspendOwned(rejection.empty()?"native_composite_control_replaced":rejection);
    else if(!Running())CloseGroup();
}
void NativeCompositeTrainingAttempt::CloseGroup() {
    if(closed || !group)return;closed=true;completedNativeProbes+=group->IssuedNativeProbes();
    const auto& result=group->Result();
    try {
        // Includes samples from a cancelled/current partial raw probe, even
        // when its closed trace never reached the group's witness inventory.
        Require(activeSamples<=limits.maximumTotalSamples-totalSamples,"composite_attempt_sample_budget_exhausted");
        totalSamples+=activeSamples;
        if(protocol->schemaVersion==5) {
            const auto bytes=CompositeRecordPrimaryBytes(Record());
            if(windowPrimaryBytes>windowDeclaration->maximumTotalPrimaryBytes || bytes>windowDeclaration->maximumTotalPrimaryBytes-windowPrimaryBytes)
                throw CompositeTrainingBudgetExhausted("candidate_window_primary_byte_budget_exhausted");
            windowPrimaryBytes+=bytes;
        }
        Require(group->State()==NativeMatchedOmissionState::Completed && prepared,"composite_train_group_not_completed");
        Require(CheckBoundary()==activeIdentity->candidateWitnessId && adapter.readModelRevision()==activeIdentity->baseRevision,
            "composite_train_candidate_boundary_changed");
        if(protocol->schemaVersion==5) {
            Require(activeWindowSpecification && activeWindowPhysical,"candidate_window_prepared_specification_missing");
            if(purpose!=CompositeMeasurementPurpose::Reference) {
                Require(windowReferences && activeWindowProposal && activeWindowSnapshot &&
                    activeWindowProposal->identity.lock()==activeIdentity && activeWindowSnapshot->identity==activeIdentity,
                    "candidate_window_prepared_identity_changed");
                const auto checked=CandidateWindowProposalReplayFactory::CheckCandidateBinding(*windowReferences,*protocol,
                    ParseStrictNativeJsonObject(activeIdentity->candidateBindingJson,128u*1024u*1024u));
                Require(!result.packets.empty() && result.packets.front().requestedSpecification,"candidate_window_actual_requested_specification_missing");
                const auto replay=CandidateWindowProposalReplayFactory::Replay(*windowReferences,*protocol,checked,logical,
                    windowDeclaration->attemptId,activeIdentity->groupId,result.packets.front().requestedSpecification->maximumSamples,purpose,
                    activeWindowProposal->Record());
                Require(replay.Record().computation.window==activeWindowSpecification->candidateFrame,"candidate_window_frozen_interval_changed");
            }
            selection=SelectNativeDeclaredWindowBasinMaximum(*activeWindowSpecification,protocol->candidateWindowMeasurement->geometryPolicy,result);
        } else {
            NativeDeclaredWindowBasinSpecification spec;spec.groupId=activeIdentity->groupId;
            spec.inputId=protocol->inputs[activeIdentity->inputIndex].id;spec.assignedInput=activeIdentity->inputIndex;spec.channel=activeIdentity->channel;
            const auto& window=protocol->compositeMeasurement->inputWindows[activeIdentity->inputIndex];
            spec.windowBeginOffset=window.beginOffset;spec.windowEndOffset=window.endOffset;
            selection=SelectNativeDeclaredWindowBasinMaximum(spec,protocol->compositeMeasurement->geometryPolicy,result);
        }
        const auto& selected=*selection;
        if(selected.status!=NativeDeclaredWindowBasinStatus::Completed || !selected.geometry)
            throw std::invalid_argument(selected.reasons.empty()?"composite_train_native_assignment_rejected":selected.reasons.front());
        if(selected.geometry->validity!=DeclaredWindowBasinValidity::Valid || !selected.geometry->measuredMaximum)
            throw std::invalid_argument(std::string("composite_train_assignment_")+DeclaredWindowBasinValidityName(selected.geometry->validity));
        const auto& peak=*selected.geometry->measuredMaximum;const auto& full=result.witness.fullBefore.observation;
        const auto origin=std::find(protocol->stimulusIds.begin(),protocol->stimulusIds.end(),protocol->originStimulusId)-protocol->stimulusIds.begin();
        const double originTime=full.events.at(origin).time;
        const auto proof=protocol->schemaVersion==5?CandidateWindowTrainingSemanticProofId(*protocol,Record()):
            CompositeTrainingSemanticProofId(*protocol,CompositeSemanticProofContent{sourceOrigin,
                activeIdentity->candidateWitnessId,liveAttemptBinding,limits,result.maximumModelSeconds,result.maximumWallSeconds,
                activeIdentity->trialId,activeIdentity->groupId,activeIdentity->purpose,result,peak});
        // Native wall budget includes replay/selection/hash work. This is a
        // live steady-clock check, not an elapsed-time claim decoded from old
        // group declarations. Supplied hook clocks must use this same clock.
        const double now=NativeTrainingWallTime();CheckClock(lastModel,now);
        if(now-groupStartWall>=groupWallLimit)throw CompositeTrainingBudgetExhausted("composite_train_group_wall_budget_exhausted");
        measurement=ValidatedCompositeMeasurement(*prepared,peak.rawPotential,peak.time-originTime,peak.plateauEnd-originTime,proof);
        if(protocol->schemaVersion==5 && purpose==CompositeMeasurementPurpose::Reference) {
            CandidateWindowArithmeticSeed seed;seed.inputIndex=activeIdentity->inputIndex;
            seed.firstFrame=peak.firstSample;seed.lastFrame=peak.lastSample;seed.referenceFrameCount=full.channels.at(activeIdentity->channel).samples.size();
            seed.initialParameters=initialWindowParameters;seed.initialCapacitance=initialWindowCapacitance;
            seed.grid.frequencyHz=std::uint64_t(protocol->stimulusProfile->frequencyHz);seed.grid.sourceOrigin=sourceOrigin;
            seed.grid.sourcePatternBinding=protocol->patternHash;seed.grid.sourceProfileBinding=Hash(sourceBinding["actual_source_profile"]);
            seed.grid.resetBinding=result.specification.resetBinding;
            stagedWindowSeeds.at(activeIdentity->inputIndex)=CandidateWindowReferenceSeed(std::move(seed),proof,ownerScope);
            stagedWindowMeasurements.at(activeIdentity->inputIndex)=*measurement;
            stagedWindowGroups.at(activeIdentity->inputIndex)=activeIdentity->groupId;
        }
    }
    catch(const CompositeTrainingBudgetExhausted& e){budgetExhausted=true;measurement.reset();rejection=e.what();}
    catch(const std::exception& e){measurement.reset();if(rejection.empty())rejection=e.what();if(!result.reason.empty())rejection+=";"+result.reason;}
    groupEndModel=lastModel;groupEndWall=NativeTrainingWallTime();
    if(protocol->schemaVersion==5)windowRecords.push_back(Record());
}
void NativeCompositeTrainingAttempt::RevokeLive(){cancelled=true;lease.reset();}
void NativeCompositeTrainingAttempt::RetainRawPacket(const NativeProbeResult& packet) {
    if(packet.requestedSpecification)for(auto& trial:rawTrials)
        if(trial.actual.trialId==packet.requestedSpecification->trialId &&
            trial.actual.runId==packet.requestedSpecification->runId){trial.packet=packet;break;}
}
std::vector<NativeProbeResult> NativeCompositeTrainingAttempt::TakeRetainedRawPackets() {
    std::vector<NativeProbeResult> packets;if(!group)return packets;
    const auto& journal=group->Result().packets;
    Require(rawDeliveryCursor<=journal.size(),"composite_raw_delivery_cursor_changed");
    while(rawDeliveryCursor<journal.size()) {
        RetainRawPacket(journal[rawDeliveryCursor]);packets.push_back(journal[rawDeliveryCursor]);++rawDeliveryCursor;
    }
    return packets;
}
void NativeCompositeTrainingAttempt::InterruptOwnedRaw(const std::string& reason,bool lifecycleAuthority) {
    RevokeLive();
    // May run from a preparation/sample callback. Capture the actual executor
    // result now, before Reset clears it, without re-entering a group frame.
    if(cancelOwnedRaw)if(auto packet=cancelOwnedRaw(reason,lifecycleAuthority)) {
        RetainRawPacket(*packet);interruptedRawPackets.push_back(std::move(*packet));
    }
}
void NativeCompositeTrainingAttempt::SuspendOwned(const std::string& reason) {
    InterruptOwnedRaw(reason);
    NativeMatchedOmissionAdapter authority;authority.onRetainedPacket=adapter.onRetainedPacket;
    // Already captured packets are plain historical data, not an owner getter.
    authority.takeResult=[this]()->std::optional<NativeProbeResult>{
        if(interruptedRawPackets.empty())return {};
        auto packet=std::move(interruptedRawPackets.front());interruptedRawPackets.erase(interruptedRawPackets.begin());return packet;
    };
    if(Running())group->Cancel(authority,reason);
    if(group)while(auto packet=authority.takeResult())group->RetainPacket(authority,std::move(*packet));
    CloseGroup();RevokeLive();
    if(rejection.empty())rejection=reason;
}
void NativeCompositeTrainingAttempt::RestartOwned(CompositeAttemptLimits value,double m,double w,const std::string& revision) {
    // Only the owning service can carry this immutable source family across a
    // legal Reset. Default/pool/retarget clear that service-owned family.
    Require(ownerLife.IsAlive() && !Running(),"composite_warm_restart_owner_not_available");
    ownerLife=observeOwner();Require(ownerLife.IsCurrent(),"composite_warm_restart_owner_not_current");
    Require(value.maximumGroups && value.maximumNativeProbes && value.maximumTotalSamples &&
        std::isfinite(value.maximumModelSeconds) && value.maximumModelSeconds>0. &&
        std::isfinite(value.maximumWallSeconds) && value.maximumWallSeconds>0.,"invalid_composite_attempt_limits");
    Require(value.maximumGroups<=9007199254740991ULL && value.maximumNativeProbes<=9007199254740991ULL &&
        value.maximumTotalSamples<=9007199254740991ULL && std::isfinite(m) && std::isfinite(w),"inexact_composite_attempt_limits");
    limits=value;startModel=lastModel=m;startWall=lastWall=w;cancelled=false;
    Require(m==actualModelTime() && adapter.requested() && adapter.readModelRevision()==revision,
        "composite_warm_restart_current_binding_changed");
    // Keep sourceOrigin, sourceBinding, initial revision/epoch, images and C0.
    CheckBoundary();generation=NextAttemptGeneration();
    liveAttemptBinding=Hash(QJsonArray{Text(ownerScope),QString::number(qulonglong(generation))});
    lease=std::make_shared<CompositeTrainingDetail::LiveLease>();
    lease->requireCurrent=[this](){Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
        if(!adapter.requested())throw CompositeTrainingStopped();};
    issuedGroups=completedNativeProbes=totalSamples=activeSamples=0;logicalTrials.clear();
    group.reset();prepared.reset();measurement.reset();selection.reset();activeIdentity.reset();rawTrials.clear();
    closed=false;budgetExhausted=false;rejection.clear();beforeNativeQueue={};afterNativeResult={};
    rawDeliveryCursor=0;interruptedRawPackets.clear();ownedRawRun.clear();ownedRawTrial.clear();
}
NativeCompositeMeasurementRecord NativeCompositeTrainingAttempt::Record() const {
    Require(bool(group) && bool(activeIdentity),"composite_measurement_not_started");
    NativeCompositeMeasurementRecord r;r.logical=logical;r.purpose=purpose;r.sourceOrigin=sourceOrigin;r.groupId=activeIdentity->groupId;
    r.ownerFamilyBinding=ownerScope;r.liveAttemptBinding=liveAttemptBinding;r.candidateBindingJson=activeIdentity->candidateBindingJson;
    r.semanticProofId=measurement?measurement->SemanticProofId():std::string{};r.reason=rejection;
    r.groupState=group->State();r.group=group->Result();r.rawTrials=rawTrials;r.selection=selection;r.attemptLimits=limits;
    r.issuedNativeProbes=group->IssuedNativeProbes();r.sampledFrames=activeSamples;
    r.startModel=groupStartModel;r.startWall=groupStartWall;r.endModel=groupEndModel;r.endWall=groupEndWall;
    r.maximumGroupModelSeconds=groupModelLimit;r.maximumGroupWallSeconds=groupWallLimit;
    if(protocol->schemaVersion==5) {
        r.attemptId=windowDeclaration->attemptId;r.candidateWindowSpecification=activeWindowSpecification;
        if(activeWindowProposal)r.windowProposal=activeWindowProposal->Record();
        r.policy.schemaVersion=2;r.policy.contextPolicy=protocol->candidateWindowMeasurement->contextPolicy;
        r.policy.geometryPolicy=protocol->candidateWindowMeasurement->geometryPolicy;
        r.policy.maximumGroups=limits.maximumGroups;r.policy.maximumNativeProbes=limits.maximumNativeProbes;r.policy.maximumTotalSamples=limits.maximumTotalSamples;
        r.policy.maximumTotalPrimaryBytes=windowDeclaration->maximumTotalPrimaryBytes;
        r.policy.maximumGroupModelSeconds=groupModelLimit;r.policy.maximumGroupWallSeconds=groupWallLimit;
        const auto candidate=ParseStrictNativeJsonObject(activeIdentity->candidateBindingJson,128u*1024u*1024u);
        if(!candidate["optional_C0_witness"].isNull()) {
            const auto witness=DecodeNativeTrainingCapacitanceWitness(candidate["optional_C0_witness"].toObject());
            const auto witnessId=NativeTrainingCapacitanceWitnessId(witness);r.capacitanceWitnesses.emplace(witnessId,witness);
            for(std::size_t i=0;i<r.issuedNativeProbes;++i)r.nativeTrialWitnessIds.emplace(r.groupId+"/native/"+std::to_string(i),witnessId);
            if(!r.issuedNativeProbes)r.capacitanceWitnesses.clear();
        }
    }
    return r;
}
void NativeCompositeTrainingAttempt::CompleteCandidateWindowReferences(const CapturedReferences& captured) {
    CommitCandidateWindowReferences(PrepareCandidateWindowReferences(captured));
}
std::shared_ptr<const CandidateWindowReferenceSet> NativeCompositeTrainingAttempt::PrepareCandidateWindowReferences(const CapturedReferences& captured) {
    Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
    CheckClock(actualModelTime(),NativeTrainingWallTime());
    Require(protocol->schemaVersion==5 && windowDeclaration && !Running() && !windowReferences &&
        captured.Protocol()==*protocol && captured.Inputs().size()==protocol->inputs.size() &&
        captured.CompositeOwnerScope()==ownerScope && captured.CompositeSourceOrigin()==sourceOrigin &&
        captured.InitialModelRevision()==initialRevision && captured.InitialModelEpoch()==initialEpoch &&
        adapter.readModelRevision()==initialRevision,"candidate_window_complete_reference_family_mismatch");
    std::vector<CandidateWindowReferenceSeed> seeds;seeds.reserve(protocol->inputs.size());
    std::vector<ValidatedCompositeMeasurement> liveReferences;liveReferences.reserve(protocol->inputs.size());
    for(std::size_t i=0;i<protocol->inputs.size();++i) {
        Require(stagedWindowSeeds[i] && stagedWindowMeasurements[i] && captured.Inputs()[i].id==protocol->inputs[i].id &&
            captured.Inputs()[i].measurementGroupId==stagedWindowGroups[i] &&
            captured.Inputs()[i].semanticProofId==stagedWindowSeeds[i]->ReferenceProofId(),"candidate_window_incomplete_reference_seed_set");
        seeds.push_back(*stagedWindowSeeds[i]);
        liveReferences.push_back(*stagedWindowMeasurements[i]);
    }
    // CapturedReferences itself is immutable data. Recheck each staged live
    // token here as well: an intervening graph replacement cannot be hidden by
    // restoring the same semantic values before releasing the complete set.
    const auto fresh=CapturedReferences::CaptureComposite(*protocol,liveReferences);
    Require(fresh.Inputs()==captured.Inputs(),"candidate_window_reference_consumption_changed");
    auto ready=std::shared_ptr<const CandidateWindowReferenceSet>(new CandidateWindowReferenceSet(
        std::move(seeds),sourceOrigin,ownerScope,stagedWindowGroups,Hash(EncodeVerificationProtocol(*protocol)),
        QJsonDocument(sourceBinding).toJson(QJsonDocument::Compact).toStdString(),protocol,initialRevision,initialEpoch,[&](){
            std::vector<double> targets;for(const auto& input:fresh.Inputs())targets.push_back(input.rawPotential);return targets;}()));
    QJsonObject candidate;CheckBoundary(&candidate);
    (void)CandidateWindowProposalReplayFactory::CheckCandidateBinding(*ready,*protocol,candidate);
    CheckClock(actualModelTime(),NativeTrainingWallTime());
    return ready;
}
void NativeCompositeTrainingAttempt::CommitCandidateWindowReferences(std::shared_ptr<const CandidateWindowReferenceSet> ready) noexcept {
    // The caller has prepared the complete set in this uninterrupted frame.
    // Shared immutable ownership lets the service preallocate its copies and
    // publish the SAME allN value without another fallible deep copy.
    windowReferences=std::move(ready);
    for(auto& record:windowRecords)if(record.purpose==CompositeMeasurementPurpose::Reference &&
        std::find(windowReferences->ReferenceGroupIds().begin(),windowReferences->ReferenceGroupIds().end(),record.groupId)!=windowReferences->ReferenceGroupIds().end()) {
        record.consumed=true;record.trainingRequestedAtConsume=true;
    }
}
void NativeCompositeTrainingAttempt::RefreshOwnedAfterEdit(const std::string& revision) {
    Require(ownerLife.IsAlive() && !Running(),"composite_owned_edit_not_at_group_boundary");
    RevokeLive();ownerLife=observeOwner();cancelled=false;
    Require(ownerLife.IsCurrent() && adapter.requested() && adapter.readModelRevision()==revision,
        "composite_owned_edit_current_binding_changed");
    CheckBoundary();generation=NextAttemptGeneration();
    liveAttemptBinding=Hash(QJsonArray{Text(ownerScope),QString::number(qulonglong(generation))});
    lease=std::make_shared<CompositeTrainingDetail::LiveLease>();
    lease->requireCurrent=[this](){Require(!cancelled && ownerLife.IsCurrent(),"expired_composite_training_attempt");
        if(!adapter.requested())throw CompositeTrainingStopped();};
    // Attempt clocks/counters and immutable source family are NOT restarted.
}
void NativeCompositeTrainingAttempt::Cancel(const std::string& reason) {
    if(Running())group->Cancel(ownerLife.IsCurrent()?adapter:NativeMatchedOmissionAdapter{},reason);
    CloseGroup();cancelled=true;lease.reset();measurement.reset();
    if(rejection.empty())rejection=reason;
}
}
