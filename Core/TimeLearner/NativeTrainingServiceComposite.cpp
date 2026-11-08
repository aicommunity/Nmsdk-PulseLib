#include "NativeTrainingService.h"
#include "ProbeEvidenceJson.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}
bool ActiveComposite(NativeTrainingState s){return s==NativeTrainingState::CapturingReferences || s==NativeTrainingState::Searching;}
struct CompositeBudget : std::runtime_error {using std::runtime_error::runtime_error;};
struct Dispatch {std::size_t& depth;Dispatch(std::size_t& d):depth(d){++depth;}~Dispatch(){--depth;}};
CompositeAttemptLimits Limits(const NativeTrainingPlan& p){const auto& c=*p.composite;
    return {c.maximumGroups,c.maximumNativeProbes,c.maximumTotalSamples,p.maximumModelSeconds,p.maximumWallSeconds};}
void Validate(const NativeTrainingPlan& p) {
    ValidateVerificationProtocol(p.protocol);Require(p.protocol.schemaVersion==4 && p.composite,"native_composite_plan_required");
    const auto& c=*p.composite;const auto& method=*p.protocol.compositeMeasurement;const auto n=p.protocol.inputs.size();
    Require(c.schemaVersion==1 && c.methodId==method.methodId && c.contextPolicy==method.contextPolicy &&
        c.geometryPolicy==method.geometryPolicy && p.inputWindows.size()==n && p.maximumReferenceProbes>=n &&
        p.probe.specification.schemaVersion==3 && !p.probe.specification.recordSpikes &&
        p.probe.specification.begin==0. && p.probe.specification.maximumSamples<=c.contextPolicy.maximumSamples &&
        p.probe.specification.patternHash==p.protocol.patternHash && p.probe.specification.sampleId==p.protocol.sampleId &&
        p.probe.specification.stimulusProfile==p.protocol.stimulusProfile &&
        p.probe.specification.expectedStimulusIds==p.protocol.stimulusIds && p.probe.routing==p.protocol.routing &&
        p.probe.protocolRevision==p.protocol.revision,"native_composite_plan_protocol_mismatch");
    for(auto v:{c.maximumGroups,c.maximumNativeProbes,c.maximumTotalSamples,c.maximumTotalPrimaryBytes})
        Require(v>0 && v<=9007199254740991ULL,"invalid_native_composite_attempt_limit");
    Require(c.maximumNativeProbes>=n+2 && c.maximumTotalSamples>=c.contextPolicy.maximumTotalSamples &&
        std::isfinite(p.maximumModelSeconds) && p.maximumModelSeconds>0. && std::isfinite(p.maximumWallSeconds) && p.maximumWallSeconds>0. &&
        std::isfinite(c.maximumGroupModelSeconds) && c.maximumGroupModelSeconds>0. && c.maximumGroupModelSeconds<=p.maximumModelSeconds &&
        std::isfinite(c.maximumGroupWallSeconds) && c.maximumGroupWallSeconds>0. && c.maximumGroupWallSeconds<=p.maximumWallSeconds,
        "invalid_native_composite_time_or_probe_limits");
    for(std::size_t i=0;i<n;++i) {
        const auto& w=p.inputWindows[i];const auto& m=method.inputWindows[i];
        Require(w.inputId==p.protocol.inputs[i].id && w.stimulusId==p.protocol.inputs[i].stimulusId &&
            w.channel==(p.protocol.routing==ProbeRouting::ClassicBurst?i:0u) && w.begin==m.beginOffset && w.end==m.endOffset &&
            w.end<=p.probe.specification.end,"native_composite_window_mapping_mismatch");
    }
}
std::size_t PrimaryReservation(const NativeTrainingPlan& p) {
    const auto n=p.protocol.inputs.size(),channels=p.protocol.routing==ProbeRouting::ClassicBurst?n:1;
    // Classic-locale binary64 with precision17 fits in 32 bytes per number.
    // Reserve the declared upper bound BEFORE executing a group. Partial and
    // failed groups therefore cannot evade the retained-primary byte budget.
    const auto max=std::numeric_limits<std::size_t>::max();
    Require(channels<=(max-67)/33,"composite_primary_reservation_overflow");
    const auto row=33*(channels+2)+1;Require(channels<=(max-16)/32,"composite_primary_reservation_overflow");
    const auto header=16+32*channels;
    Require(n<=max-2 && n+2<=max/header,"composite_primary_reservation_overflow");const auto headers=(n+2)*header;
    Require(p.composite->contextPolicy.maximumTotalSamples<=(max-headers)/row,"composite_primary_reservation_overflow");
    return headers+p.composite->contextPolicy.maximumTotalSamples*row;
}
}

void NativeTrainingService::CompositeRefreshCapacitance(const NativeTrainingAdapter& a,
    const std::optional<TrainingCapacitance>& requested,bool match) {
    const auto witness=a.readCapacitanceWitness?a.readCapacitanceWitness():std::optional<TrainingCapacitanceWitness>{};
    const auto actual=witness?std::optional<TrainingCapacitance>(witness->parameters):std::nullopt;
    ValidateTrainingCapacitance(actual);
    Require(!match || TrainingCapacitanceReadbackMatches(requested,actual),"native_composite_capacitance_readback_mismatch");
    if(witness)Require(witness->modelRevision==a.readModelRevision(),"native_composite_capacitance_revision_mismatch");
    compositeCapacitance=actual;
}
void NativeTrainingService::StartComposite(NativeTrainingPlan value,const NativeTrainingAdapter& a,double m,double w) {
    Require(state==NativeTrainingState::Idle,"native_training_requires_reset_before_restart");Validate(value);
    Require(a.createComposite && a.readParameters && a.readModelRevision && a.apply && a.cancel && a.trainingRequested,
        "incomplete_native_composite_owner_adapter");
    Require(a.trainingRequested(),"native_composite_training_request_not_active");
    Require(std::isfinite(m) && std::isfinite(w),"invalid_native_training_plan");if(a.validateExecution)a.validateExecution();
    const auto revision=a.readModelRevision();Require(!revision.empty(),"missing_native_training_model_revision");
    if(references)Require(references->Protocol()==value.protocol && revision==referenceBinding,"native_training_reference_binding_changed");
    if(incompleteCaptureConstraint)Require(revision==incompleteCaptureConstraint->first && retainedEpoch==incompleteCaptureConstraint->second,
        "incomplete_reference_capture_requires_original_model_or_explicit_cold_restart");
    value.search.initialModelRevision=revision;if(references || incompleteCaptureConstraint)value.search.initialModelEpoch=retainedEpoch;
    ValidateBoundedTrainingInputs(value.protocol,value.search,a.readParameters());
    CompositeRefreshCapacitance(a,{},false);ValidateBoundedTrainingCapacitance(value.search,compositeCapacitance);
    Require(!value.recordCapacitanceHistory || compositeCapacitance,"native_composite_capacitance_history_requires_owned_C0");
    Require(!compositeCapacitance || (a.readCapacitanceWitness && a.applyWithCapacitance && a.assessResistanceDomainWithCapacitance),
        "native_composite_owned_capacitance_requires_boundary_adapter");
    Require(value.search.normalization!=Normalization::TipResistance || a.assessResistanceDomain,"native_n1_requires_effective_domain_provider");
    // Construction happens while the actual owner's service is still Idle.
    // Warm restart keeps the original source family; it never recaptures it.
    if(composite)composite->RestartOwned(Limits(value),m,w,revision);
    else {
        Require(!references,"native_composite_imported_family_requires_owned_adoption");
        composite=a.createComposite(value.protocol,Limits(value),m,w);Require(bool(composite),"missing_native_composite_authority");
        compositeSourceBinding=composite->SourceBinding();
    }
    std::set<std::string> retained(compositeReferenceGroups.begin(),compositeReferenceGroups.end());
    if(references)std::erase_if(compositeRecords,[&](const auto& r){return !retained.count(r.groupId);});
    else {compositeRecords.clear();compositeReferenceGroups.clear();compositeReferenceMeasurements.clear();}
    compositePrimaryBytes=0;for(const auto& r:compositeRecords)compositePrimaryBytes+=CompositeRecordPrimaryBytes(r);
    Require(compositePrimaryBytes<=value.composite->maximumTotalPrimaryBytes,"native_composite_retained_reference_byte_budget");
    plan=std::move(value);startModel=lastModel=m;startWall=lastWall=w;
    retainedEpoch=plan.search.initialModelEpoch;referenceProbes=invalidReferences=0;
    observedPackets.clear();finalPackets.clear();compositeFinalGroups.clear();compositeRequest.reset();
    if(!references){referencePackets.clear();referenceEvidence.clear();}
    certificate.reset();search.reset();activeAction.reset();waiting=false;compositeHarvested=true;reason.clear();outcome=TrainingStatus::Searching;
    ++attempt;++compositeGeneration;plan.search.runId+="/session/"+std::to_string(attempt);
    state=NativeTrainingState::CapturingReferences;incompleteCaptureConstraint.reset();
    if(references)CompositeBeginSearch(a);
}
void NativeTrainingService::CompositeBeginSearch(const NativeTrainingAdapter& a) {
    Require(references && a.readModelRevision()==referenceBinding,"reference_capture_model_changed");
    plan.search.initialModelRevision=referenceBinding;
    search=std::make_unique<BoundedTrainingSession>(*references,plan.search,a.readParameters(),
        plan.search.normalization==Normalization::TipResistance,compositeCapacitance);
    state=NativeTrainingState::Searching;SyncOutcome();
}
void NativeTrainingService::QueueComposite(const NativeTrainingAdapter& a,const VerificationRequest& request,
    CompositeMeasurementPurpose purpose,double m,double w) {
    const auto& p=*plan.composite;const auto n=plan.protocol.inputs.size();
    if(composite->IssuedGroups()>=p.maximumGroups || composite->IssuedNativeProbes()>p.maximumNativeProbes ||
        n+2>p.maximumNativeProbes-composite->IssuedNativeProbes() || composite->RetainedSamples()>p.maximumTotalSamples ||
        p.contextPolicy.maximumTotalSamples>p.maximumTotalSamples-composite->RetainedSamples())throw CompositeBudget("native_composite_attempt_budget_exhausted");
    if(compositePrimaryBytes>p.maximumTotalPrimaryBytes || PrimaryReservation(plan)>p.maximumTotalPrimaryBytes-compositePrimaryBytes)
        throw CompositeBudget("native_composite_primary_byte_budget_exhausted");
    CompositeRefreshCapacitance(a,search?search->Capacitance():compositeCapacitance,true);
    compositeRequest=request;compositeHarvested=false;compositeGroupWitnesses.clear();compositeGroupTrialWitnessIds.clear();
    const auto generation=compositeGeneration;auto prototype=plan.probe;
    const auto serial=activeAction?activeAction->serial:0;
    auto prepared=[this,generation,serial](const PreparedCompositeMeasurement& p){
        if(generation!=compositeGeneration || !ActiveComposite(state))throw CompositeTrainingStopped();
        if(search)search->ArmComposite(serial,p);
    };
    auto before=[this,a,generation](const NativeProbeRequest& raw){
        Require(generation==compositeGeneration && ActiveComposite(state),"native_composite_attempt_replaced");
        CompositeRefreshCapacitance(a,search?search->Capacitance():compositeCapacitance,true);
        if(!compositeCapacitance)return;const auto witness=a.readCapacitanceWitness();
        Require(witness && witness->modelRevision==raw.baseModelRevision,"native_composite_raw_C0_context_mismatch");
        const auto id=NativeTrainingCapacitanceWitnessId(*witness);compositeGroupWitnesses.emplace(id,*witness);
        Require(compositeGroupTrialWitnessIds.emplace(raw.specification.trialId,id).second,"native_composite_raw_C0_trial_replayed");
    };
    auto result=[this,a,generation](const NativeProbeResult& raw){
        if(generation==compositeGeneration)RawCompositeResult(a,raw);
    };
    waiting=true;
    composite->StartMeasurement(request,purpose,std::move(prototype),
        std::min(p.maximumGroupModelSeconds,plan.maximumModelSeconds-(m-startModel)),
        std::min(p.maximumGroupWallSeconds,plan.maximumWallSeconds-(w-startWall)),m,w,prepared,before,result);
}
void NativeTrainingService::RawCompositeResult(const NativeTrainingAdapter& a,const NativeProbeResult& packet) {
    // Retain before the user sink; policy changes/failures cannot erase it.
    observedPackets.push_back(packet);
    if(compositeCapacitance && packet.requestedSpecification) {
        const auto found=compositeGroupTrialWitnessIds.find(packet.requestedSpecification->trialId);
        Require(found!=compositeGroupTrialWitnessIds.end(),"native_composite_unwitnessed_raw_trial");
        const auto next=a.readCapacitanceWitness();Require(next && NativeTrainingCapacitanceWitnessId(*next)==found->second,
            "native_composite_C0_changed_during_raw_trial");
    }
    if(plan.observationSink)plan.observationSink(packet);
}
void NativeTrainingService::ArchiveComposite(const std::shared_ptr<NativeCompositeTrainingAttempt>& actor) {
    if(!actor || !actor->Group() || actor->SourceBinding()!=compositeSourceBinding)return;
    auto record=actor->Record();record.attemptId=plan.search.runId;record.policy=*plan.composite;record.capacitanceWitnesses=compositeGroupWitnesses;
    for(std::size_t i=0;i<record.issuedNativeProbes;++i) {
        const auto id=record.groupId+"/native/"+std::to_string(i);const auto found=compositeGroupTrialWitnessIds.find(id);
        if(found!=compositeGroupTrialWitnessIds.end())record.nativeTrialWitnessIds.emplace(*found);
    }
    std::set<std::string> required;for(const auto& [trial,id]:record.nativeTrialWitnessIds)required.insert(id);
    std::erase_if(record.capacitanceWitnesses,[&](const auto& entry){return !required.count(entry.first);});
    auto found=std::find_if(compositeRecords.begin(),compositeRecords.end(),[&](const auto& r){return r.groupId==record.groupId;});
    if(found==compositeRecords.end())compositeRecords.push_back(std::move(record));
    else {record.consumed=found->consumed;record.trainingRequestedAtConsume=found->trainingRequestedAtConsume;*found=std::move(record);}
    compositePrimaryBytes=0;for(const auto& r:compositeRecords)compositePrimaryBytes+=CompositeRecordPrimaryBytes(r);
    compositeNativeProbes=actor->IssuedNativeProbes();compositeGroups=actor->IssuedGroups();compositeSampledFrames=actor->RetainedSamples();
    compositeHarvested=true;
}
void NativeTrainingService::ClosedComposite(const NativeTrainingAdapter& a,const std::shared_ptr<NativeCompositeTrainingAttempt>& actor) {
    if(!waiting || actor->Running())return;ArchiveComposite(actor);waiting=false;
    Require(bool(compositeRequest),"missing_native_composite_logical_request");
    auto& record=compositeRecords.back();const auto generation=compositeGeneration;
    if(plan.beforeCompositeConsume)plan.beforeCompositeConsume(record);
    if(generation!=compositeGeneration || !ActiveComposite(state) || composite!=actor)return;
    record.trainingRequestedAtConsume=a.trainingRequested();
    if(!*record.trainingRequestedAtConsume){StopComposite(a,TrainingStatus::ExternalStop,"external_stop");return;}
    if(actor->Group()->maximumModelSeconds>0. && (record.endModel-record.startModel>=actor->Group()->maximumModelSeconds ||
        NativeTrainingWallTime()-record.startWall>=actor->Group()->maximumWallSeconds)) {
        StopComposite(a,TrainingStatus::BudgetExhausted,"native_composite_group_time_budget_exhausted");return;
    }
    if(!actor->Measurement()) {
        if(actor->TimeBudgetExhausted() || record.groupState==NativeMatchedOmissionState::BudgetExhausted){StopComposite(a,TrainingStatus::BudgetExhausted,actor->RejectionReason());return;}
        if(state==NativeTrainingState::CapturingReferences) {
            if(++invalidReferences>plan.maximumInvalidReferenceProbes)Finish(TrainingStatus::InvalidObservation,actor->RejectionReason());
        }
        else {search->RejectComposite(actor->RejectionReason().empty()?"native_composite_measurement_rejected":actor->RejectionReason());SyncOutcome();}
    }
    else if(state==NativeTrainingState::CapturingReferences) {
        const auto& measured=*actor->Measurement();const auto& logical=*compositeRequest;
        measured.prepared.Require(plan.protocol,logical.runId,logical.trialId,logical.inputIndex,
            logical.baseModelRevision,logical.epoch,CompositeMeasurementPurpose::Reference,
            measured.prepared.identity->ownerScope,actor->SourceOrigin(),true);
        measured.RequirePrepared(measured.prepared);
        compositeReferenceMeasurements.push_back(measured);compositeReferenceGroups.push_back(record.groupId);
        try {
            if(compositeReferenceMeasurements.size()==plan.protocol.inputs.size())
                references=CapturedReferences::CaptureComposite(plan.protocol,compositeReferenceMeasurements);
        }
        catch(...) {compositeReferenceMeasurements.pop_back();compositeReferenceGroups.pop_back();throw;}
        record.consumed=true;
        const auto& packets=record.group.packets;referencePackets.insert(referencePackets.end(),packets.begin(),packets.end());
        if(references){referenceBinding=plan.search.initialModelRevision;CompositeBeginSearch(a);}
    }
    else {
        const auto wasAccepted=search->AcceptedCompositeObservations();const bool final=activeAction && activeAction->finalVerification;
        search->ConsumeComposite(*actor->Measurement(),a.readModelRevision());
        record.consumed=search->AcceptedCompositeObservations()>wasAccepted;
        if(final && record.consumed) {
            if(compositeFinalGroups.empty())compositeFinalGroups.resize(plan.protocol.inputs.size());
            compositeFinalGroups.at(compositeRequest->inputIndex)=record.groupId;
            finalPackets.insert(finalPackets.end(),record.group.packets.begin(),record.group.packets.end());
        }
        activeAction.reset();SyncOutcome();
    }
    compositeRequest.reset();if(plan.compositeSink)plan.compositeSink(record);
}
void NativeTrainingService::BeforeComposite(const NativeTrainingAdapter& a,double m,double w) {
    if(!ActiveComposite(state))return;
    const auto enteredGeneration=compositeGeneration;
    try {
        if(!a.trainingRequested()){StopComposite(a,TrainingStatus::ExternalStop,"external_stop");return;}
        CheckBudget(a,m,w);if(!ActiveComposite(state))return;if(a.validateExecution)a.validateExecution();
        if(waiting) {
            const auto actor=composite;const auto generation=compositeGeneration;{Dispatch scope(compositeDispatchDepth);actor->BeforeTick(m,w);}
            if(generation!=compositeGeneration || !ActiveComposite(state)){if(actor->Running())actor->Cancel("native_composite_control_replaced");ArchiveComposite(actor);return;}
            ClosedComposite(a,actor);return;
        }
        if(state==NativeTrainingState::CapturingReferences) {
            if(referenceProbes>=plan.maximumReferenceProbes){StopComposite(a,TrainingStatus::BudgetExhausted,"reference_probe_budget_exhausted");return;}
            const auto input=compositeReferenceMeasurements.size();++referenceProbes;const auto run=plan.search.runId+"/capture/"+std::to_string(attempt);
            VerificationRequest request{run,run+"/"+std::to_string(referenceProbes),plan.protocol.inputs[input].id,
                plan.protocol.sampleId,plan.protocol.patternHash,plan.protocol.revision,plan.search.initialModelRevision,
                plan.protocol.routing,plan.search.initialModelEpoch,input};
            QueueComposite(a,request,CompositeMeasurementPurpose::Reference,m,w);return;
        }
        activeAction=search->NextAction();SyncOutcome();if(!activeAction || !ActiveComposite(state))return;
        const auto action=*activeAction;Require(a.readModelRevision()==action.baseModelRevision,"native_training_model_changed_before_action");
        if(action.kind==TrainingActionKind::ResistanceDomain) {
            CompositeRefreshCapacitance(a,search->Capacitance(),true);
            const auto domain=compositeCapacitance?a.assessResistanceDomainWithCapacitance(action.parameters,action.inputIndex,action.capacitance):
                a.assessResistanceDomain(action.parameters,action.inputIndex);
            CompositeRefreshCapacitance(a,search->Capacitance(),true);search->AssessedDomain(action.serial,domain,a.readModelRevision());
            activeAction.reset();SyncOutcome();return;
        }
        if(action.kind==TrainingActionKind::Edit) {
            CompositeRefreshCapacitance(a,search->Capacitance(),true);
            const auto changed=compositeCapacitance?a.applyWithCapacitance(action.parameters,action.capacitance):a.apply(action.parameters);
            if(changed.status==NativeEditStatus::Applied) {
                CompositeRefreshCapacitance(a,action.capacitance,true);finalPackets.clear();compositeFinalGroups.clear();
                const auto actual=a.readModelRevision();composite->RefreshOwnedAfterEdit(actual);
                search->Applied(action.serial,changed.effective,actual,compositeCapacitance);
                if(search->Status()!=TrainingStatus::InvalidModel)referenceBinding=actual;
            }
            else if(changed.status==NativeEditStatus::Rejected && !changed.modelMayHaveChanged)
                search->DeclineEdit(action.serial,changed.reason,a.readModelRevision());
            else {StopComposite(a,TrainingStatus::InvalidModel,"native_training_live_edit_failed:"+changed.reason);return;}
            activeAction.reset();SyncOutcome();return;
        }
        VerificationRequest request{action.runId,action.trialId,plan.protocol.inputs[action.inputIndex].id,plan.protocol.sampleId,
            plan.protocol.patternHash,plan.protocol.revision,action.baseModelRevision,plan.protocol.routing,action.modelEpoch,action.inputIndex};
        QueueComposite(a,request,action.finalVerification?CompositeMeasurementPurpose::Final:CompositeMeasurementPurpose::Search,m,w);
    }
    catch(const CompositeTrainingBudgetExhausted& e){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::BudgetExhausted,e.what());}
    catch(const CompositeBudget& e){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::BudgetExhausted,e.what());}
    catch(const CompositeTrainingStopped&){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::ExternalStop,"external_stop");}
    catch(const std::exception& e){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::InvalidModel,e.what());}
}
void NativeTrainingService::AfterComposite(const NativeTrainingAdapter& a,double m,double w) {
    if(!ActiveComposite(state) || !waiting)return;
    const auto enteredGeneration=compositeGeneration;
    try {
        if(!a.trainingRequested()){StopComposite(a,TrainingStatus::ExternalStop,"external_stop");return;}
        CheckBudget(a,m,w);if(!ActiveComposite(state))return;
        const auto actor=composite;const auto generation=compositeGeneration;{Dispatch scope(compositeDispatchDepth);actor->AfterTick(m,w);}
        if(generation!=compositeGeneration || !ActiveComposite(state)){if(actor->Running())actor->Cancel("native_composite_control_replaced");ArchiveComposite(actor);return;}
        CheckBudget(a,m,NativeTrainingWallTime());if(!ActiveComposite(state))return;ClosedComposite(a,actor);
    }
    catch(const CompositeTrainingBudgetExhausted& e){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::BudgetExhausted,e.what());}
    catch(const CompositeTrainingStopped&){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::ExternalStop,"external_stop");}
    catch(const std::exception& e){if(enteredGeneration==compositeGeneration)StopComposite(a,TrainingStatus::InvalidObservation,e.what());}
}
void NativeTrainingService::StopComposite(const NativeTrainingAdapter& a,TrainingStatus terminal,const std::string& diagnostic) {
    InterruptComposite(a,diagnostic);if(search){if(terminal==TrainingStatus::BudgetExhausted)search->ExhaustBudget();else search->ExternalStop();}
    Finish(terminal,diagnostic);
}
void NativeTrainingService::InterruptComposite(const NativeTrainingAdapter& a,const std::string& diagnostic) {
    if(!composite)return;const auto actor=composite;
    if(compositeDispatchDepth){actor->RevokeLive();++compositeGeneration;return;}
    NativeMatchedOmissionAdapter authority;authority.cancel=a.cancel;authority.takeResult=a.takeResult;
    actor->SuspendOwned(authority,diagnostic);ArchiveComposite(actor);waiting=false;compositeRequest.reset();++compositeGeneration;
}
}
