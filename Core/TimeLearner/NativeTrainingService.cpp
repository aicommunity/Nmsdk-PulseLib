#include "NativeTrainingService.h"
#include "NativeTrainingProof.h"
#include "ProbeEvidenceJson.h"
#include <QJsonDocument>
#include <cmath>
#include <chrono>
#include <stdexcept>
#include <set>

namespace NMSDK::TimeLearner {
double NativeTrainingWallTime()
{return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();}
namespace {
bool Active(NativeTrainingState state)
{return state==NativeTrainingState::CapturingReferences || state==NativeTrainingState::Searching;}
void Require(bool condition,const char* reason)
{if(!condition)throw std::invalid_argument(reason);}
void PreservedBaseline(const TrainingCapacitanceWitness& previous,const TrainingCapacitanceWitness& next)
{
    Require(previous.parameters.origin==next.parameters.origin &&
        previous.parameters.maximumRelativeChange==next.parameters.maximumRelativeChange,
        "native_training_capacitance_origin_or_policy_changed");
    auto before=QJsonDocument::fromJson(QByteArray::fromStdString(previous.nativeStateJson)).object()["baseline"].toObject();
    auto after=QJsonDocument::fromJson(QByteArray::fromStdString(next.nativeStateJson)).object()["baseline"].toObject();
    for(const auto* field:{"channels","channel_classes","channel_physics","channel_origins"})
    {
        const auto old=before[field].toObject(),current=after[field].toObject();
        for(auto it=old.begin();it!=old.end();++it)
            Require(current.contains(it.key()) && current[it.key()]==it.value(),"native_training_capacitance_baseline_history_lost");
        before.remove(field);after.remove(field);
    }
    before.remove("sha256");after.remove("sha256");
    Require(before==after,"native_training_capacitance_baseline_recaptured");
}
}

void NativeTrainingService::Start(NativeTrainingPlan value,const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    if(value.protocol.schemaVersion==4){StartComposite(std::move(value),a,modelTime,wallTime);return;}
    Require(!value.composite,"unexpected_native_composite_policy");
    Require(state==NativeTrainingState::Idle,"native_training_requires_reset_before_restart");
    Require(!value.search.capacitanceSearch || value.recordCapacitanceHistory,
        "native_capacitance_search_requires_recorded_C0_history");
    Require(bool(a.readParameters) && bool(a.readModelRevision) && bool(a.apply) && bool(a.queue) &&
        bool(a.probeState) && bool(a.takeResult) && bool(a.cancel),"incomplete_native_training_adapter");
    Require(value.search.normalization!=Normalization::TipResistance || bool(a.assessResistanceDomain),
        "native_n1_requires_effective_domain_provider");
    const auto capabilities=int(bool(a.readCapacitanceWitness))+int(bool(a.applyWithCapacitance))+
        int(bool(a.assessResistanceDomainWithCapacitance));
    Require(capabilities==0 || capabilities==3,"incomplete_native_training_capacitance_adapter");
    Require(!value.recordCapacitanceHistory || capabilities==3,"native_training_requires_capacitance_history_adapter");
    Require(value.protocol.schemaVersion==3 && value.probe.specification.schemaVersion==3 &&
        value.protocol.inputs.size()==value.inputWindows.size() && !value.protocol.inputs.empty() &&
        value.maximumReferenceProbes>=value.protocol.inputs.size() &&
        std::isfinite(value.maximumModelSeconds) && value.maximumModelSeconds>0. &&
        std::isfinite(value.maximumWallSeconds) && value.maximumWallSeconds>0. &&
        std::isfinite(modelTime) && std::isfinite(wallTime),"invalid_native_training_plan");
    Require(value.search.runId.size()>0 && value.probe.specification.begin==0. &&
        value.probe.routing==value.protocol.routing && value.probe.protocolRevision==value.protocol.revision &&
        value.probe.specification.sampleId==value.protocol.sampleId &&
        value.probe.specification.patternHash==value.protocol.patternHash &&
        value.probe.specification.expectedStimulusIds==value.protocol.stimulusIds &&
        value.probe.specification.stimulusProfile==value.protocol.stimulusProfile &&
        value.probe.specification.minimumAmplitude==value.protocol.minimumAmplitude &&
        value.probe.specification.waveAssignment==value.protocol.waveAssignment &&
        value.probe.specification.peakSelection==value.protocol.peakSelection,
        "native_training_protocol_mismatch");
    if(a.validateExecution)a.validateExecution();
    const auto revision=a.readModelRevision();Require(!revision.empty(),"missing_native_training_model_revision");
    if(incompleteCaptureConstraint)
        Require(revision==incompleteCaptureConstraint->first && retainedEpoch==incompleteCaptureConstraint->second,
            "incomplete_reference_capture_requires_original_model_or_explicit_cold_restart");
    value.search.initialModelRevision=revision;
    ValidateBoundedTrainingInputs(value.protocol,value.search,a.readParameters());
    if(references)
        Require(references->Protocol()==value.protocol && revision==referenceBinding,
            "native_training_reference_binding_changed");
    for(std::size_t i=0;i<value.inputWindows.size();++i)
    {
        const auto& window=value.inputWindows[i];
        Require(window.inputId==value.protocol.inputs[i].id && window.stimulusId==value.protocol.inputs[i].stimulusId &&
            window.channel==(value.protocol.routing==ProbeRouting::ClassicBurst?i:0u),"native_training_window_mapping_mismatch");
    }
    auto initialWitness=value.recordCapacitanceHistory?a.readCapacitanceWitness():std::optional<TrainingCapacitanceWitness>{};
    Require(!value.recordCapacitanceHistory || bool(initialWitness),"native_training_capacitance_history_requires_bound_C0");
    ValidateBoundedTrainingCapacitance(value.search,initialWitness?std::optional<TrainingCapacitance>(initialWitness->parameters):std::nullopt);
    Require((!capacitanceHistory || bool(initialWitness)) &&
        (bool(initialWitness)==bool(capacitanceHistory) || (!references && !incompleteCaptureConstraint)),
        "native_training_capacitance_context_changed_on_restart");
    if(initialWitness)
    {
        Require(initialWitness->modelRevision==revision,"native_training_capacitance_revision_mismatch");
        const auto id=NativeTrainingCapacitanceWitnessId(*initialWitness);
        if(capacitanceHistory)
        {
            const auto& prior=capacitanceHistory->witnesses.at(capacitanceHistory->currentWitnessId);
            PreservedBaseline(prior,*initialWitness);
            Require(prior.parameters==initialWitness->parameters,"native_training_capacitance_changed_on_restart");
        }
        else capacitanceHistory.emplace();
        capacitanceHistory->witnesses.emplace(id,*initialWitness);capacitanceHistory->currentWitnessId=id;
        capacitanceHistory->currentVerified=true;
        if(!references)capacitanceHistory->captureWitnessId=id;
        capacitanceHistory->domainBaseWitnessIds.clear();
        // The new attempt drops old observed/search packets. Keep only the
        // reference witnesses that genuinely survive the same boundary reset.
        std::map<std::string,std::string> keptTrials;
        std::set<std::string> required{capacitanceHistory->captureWitnessId,id};
        if(references)for(const auto& packet:referencePackets)
        {
            Require(bool(packet.requestedSpecification),"missing_restart_capacitance_trial_identity");
            const auto found=capacitanceHistory->trialWitnessIds.find(packet.requestedSpecification->trialId);
            Require(found!=capacitanceHistory->trialWitnessIds.end(),"missing_restart_reference_capacitance_witness");
            keptTrials.emplace(*found);required.insert(found->second);
        }
        std::map<std::string,TrainingCapacitanceWitness> keptWitnesses;
        for(const auto& key:required)keptWitnesses.emplace(key,capacitanceHistory->witnesses.at(key));
        capacitanceHistory->trialWitnessIds=std::move(keptTrials);capacitanceHistory->witnesses=std::move(keptWitnesses);
    }
    plan=std::move(value);plan.search.initialModelRevision=revision;plan.probe.readModelRevision=a.readModelRevision;
    plan.probe.readFullModelRevision=a.readFullModelRevision;
    if(references || incompleteCaptureConstraint)plan.search.initialModelEpoch=retainedEpoch;
    else retainedEpoch=plan.search.initialModelEpoch;
    startModel=lastModel=modelTime;startWall=lastWall=wallTime;
    referenceProbes=invalidReferences=0;if(!references){referenceEvidence.clear();referencePackets.clear();}
    finalPackets.clear();observedPackets.clear();incompleteCaptureConstraint.reset();
    certificate.reset();search.reset();activeAction.reset();
    waiting=false;reason.clear();outcome=TrainingStatus::Searching;++attempt;
    plan.search.runId+="/session/"+std::to_string(attempt);
    state=NativeTrainingState::CapturingReferences;
    if(references)BeginSearch(a);
}

std::string NativeTrainingService::RememberCapacitance(const NativeTrainingAdapter& a,const std::string& revision,
    const std::optional<TrainingCapacitance>& requested)
{
    Require(bool(capacitanceHistory) && bool(a.readCapacitanceWitness),"missing_native_training_capacitance_provider");
    capacitanceHistory->currentVerified=false;
    const auto next=a.readCapacitanceWitness();Require(bool(next),"native_training_capacitance_owner_lost");
    Require(next->modelRevision==revision && a.readModelRevision()==revision,"native_training_capacitance_revision_mismatch");
    Require(TrainingCapacitanceReadbackMatches(requested,next->parameters),"native_training_capacitance_readback_mismatch");
    const auto id=NativeTrainingCapacitanceWitnessId(*next);
    PreservedBaseline(capacitanceHistory->witnesses.at(capacitanceHistory->currentWitnessId),*next);
    capacitanceHistory->witnesses.emplace(id,*next);capacitanceHistory->currentWitnessId=id;
    capacitanceHistory->currentVerified=true;
    return id;
}
void NativeTrainingService::BindTrialCapacitance(const NativeTrainingAdapter& a,const NativeProbeRequest& request)
{
    if(!capacitanceHistory)return;
    const auto current=search?search->Capacitance():std::optional<TrainingCapacitance>(
        capacitanceHistory->witnesses.at(capacitanceHistory->captureWitnessId).parameters);
    const auto id=RememberCapacitance(a,request.baseModelRevision,current);
    Require(capacitanceHistory->trialWitnessIds.emplace(request.specification.trialId,id).second,
        "duplicate_native_training_capacitance_trial");
}
void NativeTrainingService::ValidateTrialCapacitance(const NativeTrainingAdapter& a,const NativeProbeResult& result)
{
    if(!capacitanceHistory)return;
    Require(bool(result.requestedSpecification),"missing_native_training_capacitance_trial");
    const auto trial=capacitanceHistory->trialWitnessIds.find(result.requestedSpecification->trialId);
    Require(trial!=capacitanceHistory->trialWitnessIds.end(),"unwitnessed_native_training_trial");
    const auto prior=capacitanceHistory->witnesses.at(trial->second);
    const auto id=RememberCapacitance(a,result.currentBaseRevision,prior.parameters);
    const auto& next=capacitanceHistory->witnesses.at(id);
    const bool unchanged=next.parameters==prior.parameters &&
        QJsonDocument::fromJson(QByteArray::fromStdString(next.nativeStateJson))==
        QJsonDocument::fromJson(QByteArray::fromStdString(prior.nativeStateJson));
    if(!unchanged)capacitanceHistory->currentVerified=false;
    Require(unchanged,"native_training_capacitance_changed_during_trial");
}
NativeProbeRequest NativeTrainingService::Request(std::size_t input,const std::string& run,const std::string& trial,
                                                 const std::string& revision,std::size_t epoch)
{
    auto result=plan.probe;result.inputId=plan.protocol.inputs.at(input).id;result.activeInput=int(input);
    result.baseModelRevision=revision;result.modelEpoch=epoch;
    auto& s=result.specification;s.runId=run;s.trialId=trial;s.windows={plan.inputWindows.at(input)};
    // The executor supplies fresh absolute times, routing fingerprint and rest
    // baseline for each probe. Never reuse a prepared physical observation.
    s.modelRevision.clear();s.baseline.clear();
    if(activeAction)
    {
        const auto serial=activeAction->serial;
        const auto original=result.onPrepared;
        result.onPrepared=[this,serial,original](const auto& prepared,const auto& specification) {
            search->Arm(serial,prepared);if(original)original(prepared,specification);
        };
    }
    return result;
}

void NativeTrainingService::BeginSearch(const NativeTrainingAdapter& a)
{
    Require(bool(references),"missing_native_training_references");
    const auto actual=a.readModelRevision();Require(actual==referenceBinding,"reference_capture_model_changed");
    plan.search.initialModelRevision=actual;
    search=std::make_unique<BoundedTrainingSession>(*references,plan.search,a.readParameters(),
        plan.search.normalization==Normalization::TipResistance,
        capacitanceHistory?std::optional<TrainingCapacitance>(capacitanceHistory->witnesses.at(
            capacitanceHistory->currentWitnessId).parameters):std::optional<TrainingCapacitance>{});
    state=NativeTrainingState::Searching;SyncOutcome();
}

void NativeTrainingService::BeforeTick(const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    if(plan.protocol.schemaVersion==4){BeforeComposite(a,modelTime,wallTime);return;}
    if(!Active(state))return;
    try
    {
        if(a.trainingRequested && !a.trainingRequested()){ExternalStop(a);return;}
        CheckBudget(a,modelTime,wallTime);if(!Active(state) || waiting)return;
        if(a.validateExecution)a.validateExecution();
        if(state==NativeTrainingState::CapturingReferences)
        {
            if(referenceProbes>=plan.maximumReferenceProbes)
            {Finish(TrainingStatus::BudgetExhausted,"reference_probe_budget_exhausted");return;}
            Require(a.readModelRevision()==plan.search.initialModelRevision,"reference_capture_model_changed");
            const auto input=referenceEvidence.size();++referenceProbes;
            auto request=Request(input,plan.search.runId+"/capture/"+std::to_string(attempt),
                plan.search.runId+"/capture/"+std::to_string(attempt)+"/"+std::to_string(referenceProbes),
                plan.search.initialModelRevision,plan.search.initialModelEpoch);
            BindTrialCapacitance(a,request);a.queue(std::move(request));waiting=true;return;
        }
        activeAction=search->NextAction();SyncOutcome();if(!activeAction || !Active(state))return;
        if(activeAction->kind==TrainingActionKind::ResistanceDomain)
        {
            Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_domain");
            std::string witnessId;
            if(capacitanceHistory)witnessId=RememberCapacitance(a,activeAction->baseModelRevision,search->Capacitance());
            const auto domain=capacitanceHistory?
                a.assessResistanceDomainWithCapacitance(activeAction->parameters,activeAction->inputIndex,activeAction->capacitance):
                a.assessResistanceDomain(activeAction->parameters,activeAction->inputIndex);
            if(capacitanceHistory)RememberCapacitance(a,activeAction->baseModelRevision,search->Capacitance());
            search->AssessedDomain(activeAction->serial,domain,a.readModelRevision());
            if(capacitanceHistory)capacitanceHistory->domainBaseWitnessIds.push_back(witnessId);
            activeAction.reset();SyncOutcome();return;
        }
        if(activeAction->kind==TrainingActionKind::Edit)
        {
            Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_edit");
            if(capacitanceHistory)RememberCapacitance(a,activeAction->baseModelRevision,search->Capacitance());
            if(capacitanceHistory)capacitanceHistory->currentVerified=false;
            const auto edited=capacitanceHistory?a.applyWithCapacitance(activeAction->parameters,activeAction->capacitance):
                a.apply(activeAction->parameters);
            if(edited.status==NativeEditStatus::Applied)
            {
                finalPackets.clear();
                const auto actual=a.readModelRevision();
                std::optional<TrainingCapacitance> effectiveCapacitance;
                if(capacitanceHistory)
                {
                    const auto id=RememberCapacitance(a,actual,activeAction->capacitance);
                    effectiveCapacitance=capacitanceHistory->witnesses.at(id).parameters;
                }
                search->Applied(activeAction->serial,edited.effective,actual,effectiveCapacitance);
                if(search->Status()!=TrainingStatus::InvalidModel)referenceBinding=actual;
            }
            else if(edited.status==NativeEditStatus::Rejected && !edited.modelMayHaveChanged)
            {
                if(capacitanceHistory)RememberCapacitance(a,activeAction->baseModelRevision,search->Capacitance());
                search->DeclineEdit(activeAction->serial,edited.reason,a.readModelRevision());
            }
            else
            {Finish(TrainingStatus::InvalidModel,"native_training_live_edit_failed:"+edited.reason);return;}
            activeAction.reset();SyncOutcome();return; // One boundary edit per native tick.
        }
        Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_probe");
        auto request=Request(activeAction->inputIndex,activeAction->runId,activeAction->trialId,
            activeAction->baseModelRevision,activeAction->modelEpoch);
        BindTrialCapacitance(a,request);a.queue(std::move(request));waiting=true;
    }
    catch(const std::exception& error){a.cancel("native_training_controller_error");Finish(TrainingStatus::InvalidModel,error.what());RecordCancelled(a);}
    catch(...){a.cancel("native_training_controller_error");Finish(TrainingStatus::InvalidModel,"native_training_controller_exception");RecordCancelled(a);}
}

void NativeTrainingService::AfterTick(const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    if(plan.protocol.schemaVersion==4){AfterComposite(a,modelTime,wallTime);return;}
    if(!Active(state))return;
    try
    {
        if(a.trainingRequested && !a.trainingRequested()){ExternalStop(a);return;}
        CheckBudget(a,modelTime,wallTime);if(!Active(state) || !waiting)return;
        const auto status=a.probeState();
        if(status==NativeProbeStatus::Queued || status==NativeProbeStatus::Running)return;
        auto result=a.takeResult();Require(bool(result),"missing_native_training_probe_result");waiting=false;
        observedPackets.push_back(*result);
        if(plan.observationSink)plan.observationSink(*result);
        if(result->status!=NativeProbeStatus::Completed || !result->prepared || !result->observation)
        {Finish(TrainingStatus::InvalidObservation,result->reasons.empty()?"native_training_probe_failed":result->reasons.front());return;}
        // Preserve the actual packet first. An observation sink can change an
        // owner policy after the window closed; it must not cause acceptance
        // of references or a certificate under an unsupported execution scope.
        if(a.validateExecution)a.validateExecution();
        Require(result->currentBaseRevision==a.readModelRevision(),"native_training_probe_revision_mismatch");
        ValidateTrialCapacitance(a,*result);
        if(state==NativeTrainingState::CapturingReferences)
        {
            Require(result->currentBaseRevision==plan.search.initialModelRevision,"reference_capture_model_changed");
            try{ValidateProbeMeasurement(plan.protocol,referenceEvidence.size(),*result->prepared,*result->observation);}
            catch(const std::invalid_argument& error)
            {
                if(++invalidReferences>plan.maximumInvalidReferenceProbes)
                    Finish(TrainingStatus::InvalidObservation,error.what());
                return;
            }
            referenceEvidence.push_back({*result->prepared,*result->observation});
            referencePackets.push_back(*result);
            if(referenceEvidence.size()==plan.protocol.inputs.size())
            {
                references.emplace(CapturedReferences::Capture(plan.protocol,referenceEvidence));
                referenceBinding=plan.search.initialModelRevision;BeginSearch(a);
            }
        }
        else
        {
            Require(bool(activeAction),"missing_native_training_probe_action");
            if(activeAction->finalVerification)
            {
                if(!finalPackets.empty() && finalPackets.back().observation->specification.runId!=result->observation->specification.runId)
                    finalPackets.clear();
                finalPackets.push_back(*result);
            }
            search->Consume(*result->observation,result->currentBaseRevision);activeAction.reset();SyncOutcome();
        }
    }
    catch(const std::exception& error){a.cancel("native_training_observation_error");Finish(TrainingStatus::InvalidObservation,error.what());RecordCancelled(a);}
    catch(...){a.cancel("native_training_observation_error");Finish(TrainingStatus::InvalidObservation,"native_training_observation_exception");RecordCancelled(a);}
}

void NativeTrainingService::CheckBudget(const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    Require(std::isfinite(modelTime) && std::isfinite(wallTime) && modelTime>=lastModel && wallTime>=lastWall,
        "native_training_nonmonotonic_time");lastModel=modelTime;lastWall=wallTime;
    if(modelTime-startModel>=plan.maximumModelSeconds || wallTime-startWall>=plan.maximumWallSeconds)
    {
        if(plan.protocol.schemaVersion==4){StopComposite(a,TrainingStatus::BudgetExhausted,"native_training_time_budget_exhausted");return;}
        a.cancel("native_training_time_budget_exhausted");
        if(search)search->ExhaustBudget();
        Finish(TrainingStatus::BudgetExhausted,"native_training_time_budget_exhausted");
        RecordCancelled(a);
    }
}
void NativeTrainingService::Finish(TrainingStatus result,const std::string& diagnostic)
{
    if(search)retainedEpoch=search->ModelEpoch();
    outcome=result;reason=diagnostic;state=NativeTrainingState::Finished;waiting=false;activeAction.reset();
    if(result!=TrainingStatus::Exact && !(result==TrainingStatus::SearchUnresolved && search && search->Certificate()))
        certificate.reset();
}
void NativeTrainingService::SyncOutcome()
{
    if(!search)return;retainedEpoch=search->ModelEpoch();const auto status=search->Status();
    if(status==TrainingStatus::Searching || status==TrainingStatus::Verifying)return;
    certificate=search->Certificate();
    Finish(status,search->Reason().empty()?TrainingStatusName(status):search->Reason());
}
void NativeTrainingService::ExternalStop(const NativeTrainingAdapter& a)
{
    if(plan.protocol.schemaVersion==4){if(Active(state))StopComposite(a,TrainingStatus::ExternalStop,"external_stop");return;}
    if(!Active(state))return;a.cancel("external_stop");if(search)search->ExternalStop();
    Finish(TrainingStatus::ExternalStop,"external_stop");
    RecordCancelled(a);
}
void NativeTrainingService::RecordCancelled(const NativeTrainingAdapter& a)
{
    try{if(auto result=a.takeResult()){observedPackets.push_back(*result);if(plan.observationSink)plan.observationSink(*result);}}
    catch(const std::exception& error){reason+=";cancelled_observation_sink_failed:"+std::string(error.what());}
    catch(...){reason+=";cancelled_observation_sink_failed";}
}
void NativeTrainingService::FailConfiguration(const std::string& diagnostic)
{Finish(TrainingStatus::InvalidModel,diagnostic);}
void NativeTrainingService::ResetControl()
{
    if(composite){composite->RevokeLive();++compositeGeneration;}
    if(!references && attempt>0 && !plan.search.initialModelRevision.empty() && !incompleteCaptureConstraint)
        incompleteCaptureConstraint.emplace(plan.search.initialModelRevision,plan.search.initialModelEpoch);
    state=NativeTrainingState::Idle;outcome=TrainingStatus::Searching;reason.clear();waiting=false;
    search.reset();activeAction.reset();if(!references){referenceEvidence.clear();referencePackets.clear();}
    if(capacitanceHistory)capacitanceHistory->domainBaseWitnessIds.clear();
    referenceProbes=invalidReferences=0;compositeRequest.reset();compositeReferenceMeasurements.clear();
}
void NativeTrainingService::InvalidateTarget()
{ResetControl();certificate.reset();references.reset();referenceEvidence.clear();referencePackets.clear();finalPackets.clear();observedPackets.clear();incompleteCaptureConstraint.reset();referenceBinding.clear();retainedEpoch=0;plan={};capacitanceHistory.reset();
 composite.reset();compositeSourceBinding={};compositeRecords.clear();compositeReferenceGroups.clear();compositeFinalGroups.clear();
 compositeGroupWitnesses.clear();compositeGroupTrialWitnessIds.clear();compositeCapacitance.reset();
 compositeNativeProbes=compositeGroups=compositeSampledFrames=compositePrimaryBytes=0;}
void NativeTrainingService::RestoreBoundary(NativeTrainingPlan value,std::vector<NativeProbeResult> refs,
    std::vector<NativeProbeResult> finals,const std::string& revision,std::size_t epoch,std::size_t sessions)
{
    Require(!Active(state),"native_boundary_import_requires_idle_controller");
    const auto proof=ReplayNativeTrainingProof(value.protocol,value.search.tolerances,refs,finals,revision,epoch,
        value.search.maximumInvalidObservations);
    Require(sessions>0,"invalid_native_boundary_session");
    std::vector<ReferenceEvidence> observations;for(const auto& packet:refs)observations.push_back({*packet.prepared,*packet.observation});
    value.observationSink={};value.probe.onPrepared={};value.probe.onSample={};
    value.probe.readModelRevision={};value.probe.readFullModelRevision={};
    plan=std::move(value);referencePackets=std::move(refs);finalPackets=std::move(finals);referenceEvidence=std::move(observations);
    observedPackets.clear();incompleteCaptureConstraint.reset();
    references=proof.references;certificate=proof.certificate;referenceBinding=revision;retainedEpoch=epoch;attempt=sessions;
    search.reset();activeAction.reset();waiting=false;referenceProbes=invalidReferences=0;
    outcome=certificate->Exact()?TrainingStatus::Exact:TrainingStatus::SearchUnresolved;
    capacitanceHistory.reset();
    reason="restored_completed_training_boundary";state=NativeTrainingState::Finished;
}
void NativeTrainingService::RestoreStoppedBoundary(NativeTrainingPlan value,std::vector<NativeProbeResult> refs,
    std::vector<NativeProbeResult> finals,std::vector<NativeProbeResult> observations,
    const std::string& revision,std::size_t epoch,std::size_t sessions,
    const std::string& captureRevision,std::size_t captureEpoch,bool complete,
    TrainingStatus terminal,const std::string& diagnostic)
{
    Require(!Active(state),"native_boundary_import_requires_idle_controller");
    Require(sessions>0 && !revision.empty() && !diagnostic.empty() &&
        (terminal==TrainingStatus::BudgetExhausted || terminal==TrainingStatus::ExternalStop ||
         terminal==TrainingStatus::InvalidModel || terminal==TrainingStatus::InvalidObservation || terminal==TrainingStatus::SearchUnresolved),
        "invalid_native_stopped_boundary");
    auto captured=ReplayNativeReferencePrefix(value.protocol,refs,captureRevision,captureEpoch,complete);
    std::vector<ReferenceEvidence> accepted;for(const auto& p:refs)accepted.push_back({*p.prepared,*p.observation});
    value.observationSink={};value.probe.onPrepared={};value.probe.onSample={};
    value.probe.readModelRevision={};value.probe.readFullModelRevision={};
    plan=std::move(value);referencePackets=std::move(refs);finalPackets=std::move(finals);
    observedPackets=std::move(observations);referenceEvidence=std::move(accepted);references=std::move(captured);
    certificate.reset();referenceBinding=complete?revision:captureRevision;retainedEpoch=epoch;attempt=sessions;
    incompleteCaptureConstraint.reset();if(!complete)incompleteCaptureConstraint.emplace(captureRevision,captureEpoch);
    search.reset();activeAction.reset();waiting=false;referenceProbes=invalidReferences=0;
    outcome=terminal;reason=diagnostic;state=NativeTrainingState::Finished;
    capacitanceHistory.reset();
}
void ValidateNativeTrainingCapacitanceBoundary(const NativeTrainingCapacitanceHistory& history,
    const NativeTrainingService& service,const std::string& revision,const QJsonObject& actualState)
{
    Require(service.Plan().recordCapacitanceHistory && history.currentVerified && history.domainBaseWitnessIds.empty(),
        "invalid_native_capacitance_boundary_scope");
    const auto current=history.witnesses.find(history.currentWitnessId),capture=history.witnesses.find(history.captureWitnessId);
    Require(current!=history.witnesses.end() && capture!=history.witnesses.end(),"missing_native_capacitance_boundary_witness");
    Require(current->second.modelRevision==revision &&
        QJsonDocument::fromJson(QByteArray::fromStdString(current->second.nativeStateJson)).object()==actualState,
        "native_capacitance_boundary_does_not_match_current_state");
    Require(capture->second.modelRevision==service.CaptureRevision(),"native_capacitance_capture_boundary_mismatch");
    for(const auto& [id,witness]:history.witnesses)
    {
        Require(NativeTrainingCapacitanceWitnessId(witness)==id,"native_capacitance_boundary_id_mismatch");
        PreservedBaseline(witness,current->second);
    }
    std::set<std::string> trials,required{history.currentWitnessId,history.captureWitnessId};
    const auto packet=[&](const NativeProbeResult& p,const std::string& expectedRevision) {
        Require(bool(p.requestedSpecification),"native_capacitance_boundary_packet_missing_identity");
        const auto& trial=p.requestedSpecification->trialId;
        const auto binding=history.trialWitnessIds.find(trial);
        Require(binding!=history.trialWitnessIds.end(),"native_capacitance_boundary_trial_unwitnessed");
        const auto witness=history.witnesses.find(binding->second);
        Require(witness!=history.witnesses.end(),"native_capacitance_boundary_trial_witness_missing");
        if(!expectedRevision.empty())Require(witness->second.modelRevision==expectedRevision,"native_capacitance_boundary_packet_context_mismatch");
        if(p.currentBaseRevision.empty())
            Require(p.status!=NativeProbeStatus::Completed && !p.prepared && !p.observation,
                "native_capacitance_boundary_packet_revision_unknown");
        else Require(witness->second.modelRevision==p.currentBaseRevision,"native_capacitance_boundary_packet_context_mismatch");
        trials.insert(trial);required.insert(binding->second);
    };
    for(const auto& p:service.ReferencePackets())packet(p,service.CaptureRevision());
    for(const auto& p:service.FinalPackets())packet(p,revision);
    if(!service.Certificate())for(const auto& p:service.ObservedPackets())packet(p,{});
    std::set<std::string> declared,stored;
    for(const auto& [trial,id]:history.trialWitnessIds)declared.insert(trial);
    for(const auto& [id,witness]:history.witnesses)stored.insert(id);
    Require(trials==declared && required==stored,"native_capacitance_boundary_inventory_mismatch");
}
void NativeTrainingService::RestoreCapacitanceBoundary(NativeTrainingCapacitanceHistory history,
    const std::string& revision,const QJsonObject& state)
{
    Require(!Active(this->state) && !search,"native_capacitance_restore_requires_completed_boundary");
    ValidateNativeTrainingCapacitanceBoundary(history,*this,revision,state);
    capacitanceHistory=std::move(history);
}
const char* NativeTrainingStateName(NativeTrainingState state)
{switch(state){case NativeTrainingState::Idle:return "idle";case NativeTrainingState::CapturingReferences:return "capturing_references";
 case NativeTrainingState::Searching:return "searching";case NativeTrainingState::Finished:return "finished";}return "unknown";}
}
