#include "NativeTrainingService.h"
#include "NativeTrainingProof.h"
#include <cmath>
#include <chrono>
#include <stdexcept>

namespace NMSDK::TimeLearner {
double NativeTrainingWallTime()
{return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();}
namespace {
bool Active(NativeTrainingState state)
{return state==NativeTrainingState::CapturingReferences || state==NativeTrainingState::Searching;}
void Require(bool condition,const char* reason)
{if(!condition)throw std::invalid_argument(reason);}
}

void NativeTrainingService::Start(NativeTrainingPlan value,const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    Require(state==NativeTrainingState::Idle,"native_training_requires_reset_before_restart");
    Require(bool(a.readParameters) && bool(a.readModelRevision) && bool(a.apply) && bool(a.queue) &&
        bool(a.probeState) && bool(a.takeResult) && bool(a.cancel),"incomplete_native_training_adapter");
    Require(value.search.normalization!=Normalization::TipResistance || bool(a.assessResistanceDomain),
        "native_n1_requires_effective_domain_provider");
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
        plan.search.normalization==Normalization::TipResistance);
    state=NativeTrainingState::Searching;SyncOutcome();
}

void NativeTrainingService::BeforeTick(const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
    if(!Active(state))return;
    try
    {
        if(a.trainingRequested && !a.trainingRequested()){ExternalStop(a);return;}
        CheckBudget(a,modelTime,wallTime);if(!Active(state) || waiting)return;
        if(state==NativeTrainingState::CapturingReferences)
        {
            if(referenceProbes>=plan.maximumReferenceProbes)
            {Finish(TrainingStatus::BudgetExhausted,"reference_probe_budget_exhausted");return;}
            Require(a.readModelRevision()==plan.search.initialModelRevision,"reference_capture_model_changed");
            const auto input=referenceEvidence.size();++referenceProbes;
            a.queue(Request(input,plan.search.runId+"/capture/"+std::to_string(attempt),
                plan.search.runId+"/capture/"+std::to_string(attempt)+"/"+std::to_string(referenceProbes),
                plan.search.initialModelRevision,plan.search.initialModelEpoch));waiting=true;return;
        }
        activeAction=search->NextAction();SyncOutcome();if(!activeAction || !Active(state))return;
        if(activeAction->kind==TrainingActionKind::ResistanceDomain)
        {
            Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_domain");
            const auto domain=a.assessResistanceDomain(activeAction->parameters,activeAction->inputIndex);
            search->AssessedDomain(activeAction->serial,domain,a.readModelRevision());
            activeAction.reset();SyncOutcome();return;
        }
        if(activeAction->kind==TrainingActionKind::Edit)
        {
            Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_edit");
            const auto edited=a.apply(activeAction->parameters);
            if(edited.status==NativeEditStatus::Applied)
            {
                finalPackets.clear();
                const auto actual=a.readModelRevision();
                search->Applied(activeAction->serial,edited.effective,actual);
                if(search->Status()!=TrainingStatus::InvalidModel)referenceBinding=actual;
            }
            else if(edited.status==NativeEditStatus::Rejected && !edited.modelMayHaveChanged)
                search->DeclineEdit(activeAction->serial,edited.reason,a.readModelRevision());
            else
            {Finish(TrainingStatus::InvalidModel,"native_training_live_edit_failed:"+edited.reason);return;}
            activeAction.reset();SyncOutcome();return; // One boundary edit per native tick.
        }
        Require(a.readModelRevision()==activeAction->baseModelRevision,"native_training_model_changed_before_probe");
        a.queue(Request(activeAction->inputIndex,activeAction->runId,activeAction->trialId,
            activeAction->baseModelRevision,activeAction->modelEpoch));waiting=true;
    }
    catch(const std::exception& error){a.cancel("native_training_controller_error");Finish(TrainingStatus::InvalidModel,error.what());RecordCancelled(a);}
    catch(...){a.cancel("native_training_controller_error");Finish(TrainingStatus::InvalidModel,"native_training_controller_exception");RecordCancelled(a);}
}

void NativeTrainingService::AfterTick(const NativeTrainingAdapter& a,double modelTime,double wallTime)
{
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
        Require(result->currentBaseRevision==a.readModelRevision(),"native_training_probe_revision_mismatch");
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
    const auto& rejected=search->Rejected();
    Finish(status,(status==TrainingStatus::InvalidModel || status==TrainingStatus::InvalidObservation) && !rejected.empty()?
        rejected.back().reason:TrainingStatusName(status));
}
void NativeTrainingService::ExternalStop(const NativeTrainingAdapter& a)
{
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
    if(!references && attempt>0 && !plan.search.initialModelRevision.empty() && !incompleteCaptureConstraint)
        incompleteCaptureConstraint.emplace(plan.search.initialModelRevision,plan.search.initialModelEpoch);
    state=NativeTrainingState::Idle;outcome=TrainingStatus::Searching;reason.clear();waiting=false;
    search.reset();activeAction.reset();if(!references){referenceEvidence.clear();referencePackets.clear();}
    referenceProbes=invalidReferences=0;
}
void NativeTrainingService::InvalidateTarget()
{ResetControl();certificate.reset();references.reset();referenceEvidence.clear();referencePackets.clear();finalPackets.clear();observedPackets.clear();incompleteCaptureConstraint.reset();referenceBinding.clear();retainedEpoch=0;plan={};}
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
}
const char* NativeTrainingStateName(NativeTrainingState state)
{switch(state){case NativeTrainingState::Idle:return "idle";case NativeTrainingState::CapturingReferences:return "capturing_references";
 case NativeTrainingState::Searching:return "searching";case NativeTrainingState::Finished:return "finished";}return "unknown";}
}
