#include "NativeTrainingService.h"
#include "NativeMeasuredTrainingBoundary.h"
#include "ProbeEvidenceJson.h"
#include <QJsonDocument>
#include <algorithm>
#include <limits>

namespace NMSDK::TimeLearner {
namespace {
void RequireTerminal(bool value,const char* reason){if(!value)throw std::logic_error(reason);}
QJsonArray TerminalParameters(const std::vector<InputParameters>& values)
{
    QJsonArray result;for(const auto& p:values)result.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});
    return result;
}
}
void NativeTrainingService::FinalizeMeasuredBoundary(const NativeTrainingAdapter& adapter)
{
    if(state!=NativeTrainingState::Finalizing)return;
    const auto retained=measuredBoundary;const auto transcript=search;
    const auto session=attempt,generation=compositeGeneration;
    const auto begin=NativeTrainingWallTime();
    const auto authority=std::make_shared<const int>(0);terminalSelectionAuthority=authority;
    const auto current=[&] {
        return state==NativeTrainingState::Finalizing && attempt==session && compositeGeneration==generation &&
            search==transcript && measuredBoundary==retained && terminalSelectionAuthority==authority;
    };
    QJsonObject event{{"kind","timelearner_native_terminal_selection_event"},{"schema_version",2},
        {"stopped_outcome",TrainingStatusName(outcome)},{"stopped_reason",QString::fromStdString(reason)},
        {"session_attempt",double(session)},{"portable_export_supported",true},
        {"fresh_all_input_verification",false},{"certificate",QJsonValue(QJsonValue::Null)}};
    const auto publish=[&](const char* status,const std::string& diagnostic) {
        if(!current())return;
        event["status"]=status;event["reason"]=QString::fromStdString(diagnostic);
        event["native_transactions"]=double(terminalSelectionTransactions);
        event["wall_seconds"]=NativeTrainingWallTime()-begin;
        event["actual_model_epoch"]=double(ModelEpoch());
        event["historical_measurements_rebound_to_actual_epoch"]=false;
        event["sha256"]=NmsdkTools::CanonicalModelDataSha256(event);
        terminalJournal.push_back(event);terminalSelectionEvent=event;
        terminalSelectionAuthority.reset();state=NativeTrainingState::Finished;
    };
    try
    {
        RequireTerminal(terminalJournal.size()<NativeTerminalSelectionPolicy::MaximumJournalEntries,
            "native_terminal_journal_budget_exhausted");
        terminalJournal.reserve(terminalJournal.size()+1);
        RequireTerminal(plan.terminalSelection.has_value() && retained && transcript && !certificate &&
            outcome==TrainingStatus::BudgetExhausted,"terminal_selection_invalid_finished_scope");
        if(adapter.trainingRequested && !adapter.trainingRequested()) {ExternalStop(adapter);return;}
        if(!current())return;
        if(adapter.validateExecution)adapter.validateExecution();
        if(!current())return;
        RequireTerminal(adapter.probeState()!=NativeProbeStatus::Queued && adapter.probeState()!=NativeProbeStatus::Running,
            "terminal_selection_probe_not_closed");
        RequireTerminal(!composite || !composite->Running(),"terminal_selection_composite_not_closed");
        RequireTerminal(!composite || compositeHarvested,"terminal_selection_composite_not_harvested");
        composite.reset();
        const auto& selection=retained->Selection();
        RequireTerminal(transcript->SelectedMeasuredBoundary() &&
            transcript->SelectedMeasuredBoundary()->serial==selection.serial &&
            transcript->SelectedMeasuredBoundary()->candidate.trialId==selection.candidate.trialId,
            "terminal_selection_snapshot_not_current_controller_choice");
        const auto sourceEpoch=transcript->ModelEpoch();
        const auto sourceRevision=transcript->ModelRevision();const auto sourceParameters=transcript->Parameters();
        event["search_terminal_epoch"]=double(sourceEpoch);event["search_terminal_revision"]=QString::fromStdString(sourceRevision);
        event["search_terminal_parameters"]=TerminalParameters(sourceParameters);
        event["selected"]=QJsonObject{{"serial",double(selection.serial)},{"sweep",double(selection.sweep)},
            {"input_index",double(selection.candidate.inputIndex)},
            {"input_position",double(selection.inputPosition)},{"trial_id",QString::fromStdString(selection.candidate.trialId)},
            {"anchor_trial_id",QString::fromStdString(selection.candidate.anchorTrialId)},
            {"model_epoch",double(selection.candidate.modelEpoch)},{"parameters",TerminalParameters(selection.candidate.fullParameters)},
            {"neuron_identity",retained->NeuronIdentity()},{"dataset_identity",retained->DatasetIdentity()},
            {"capacitance_state",retained->CapacitanceState()},
            {"storage_fingerprint",QString::fromStdString(retained->StorageFingerprint())}};
        event["edit_budget"]=QJsonObject{{"search_applied_edits",double(transcript->AppliedEdits())},
            {"maximum_edits",double(plan.search.maximumEdits)},
            {"reserved_transactions",double(NativeTerminalSelectionPolicy::MaximumTransactions)},
            {"maximum_finalization_wall_seconds",plan.terminalSelection->maximumWallSeconds}};
        // Plain probe packets are small mathematical evidence, not raw sample
        // journals. Retain them for a later warm attempt's transition replay.
        // Composite primary stays in its existing retained family archive.
        QJsonArray selectedPackets;
        if(plan.protocol.schemaVersion==3) {
            for(const auto& id:{selection.candidate.trialId,selection.candidate.anchorTrialId}) {
                const auto found=std::find_if(observedPackets.begin(),observedPackets.end(),[&](const auto& packet) {
                    return packet.requestedSpecification && packet.requestedSpecification->trialId==id;
                });
                RequireTerminal(found!=observedPackets.end(),"terminal_selection_measurement_packet_missing");
                selectedPackets.append(EncodeNativeProbeEvidence(*found));
            }
        }
        event["selected_probe_packets"]=selectedPackets;
        const auto actualRevision=adapter.readFullModelRevision();if(!current())return;
        const auto actualParameters=adapter.readParameters();if(!current())return;
        RequireTerminal(actualRevision==sourceRevision && actualParameters==sourceParameters,
            "terminal_selection_search_and_actual_model_differ");
        RequireTerminal(bool(adapter.captureTerminalSource),"terminal_selection_source_capture_missing");
        std::shared_ptr<const NativeTerminalSourceBoundary> source;
        try {source=adapter.captureTerminalSource();}
        catch(const NativeBoundaryRetentionBudgetExhausted&) {
            publish("budget_exhausted","terminal_source_snapshot_storage_budget_exhausted");return;
        }
        if(!current())return;
        RequireTerminal(source && source->graph &&
            source->graph->NeuronIdentity().value("sha256")==QJsonValue(QString::fromStdString(sourceRevision)),
            "terminal_source_snapshot_revision_mismatch");
        RequireTerminal(adapter.readFullModelRevision()==sourceRevision && adapter.readParameters()==sourceParameters,
            "terminal_source_capture_changed_model");
        if(!current())return;
        terminalSourceBoundary=std::move(source);
        event["source_neuron_identity"]=terminalSourceBoundary->graph->NeuronIdentity();
        event["source_dataset_identity"]=terminalSourceBoundary->graph->DatasetIdentity();
        event["source_capacitance_state"]=terminalSourceBoundary->capacitanceState;
        const auto used=transcript->AppliedEdits();const auto limit=plan.search.maximumEdits;
        if(used>limit || limit-used<NativeTerminalSelectionPolicy::MaximumTransactions) {
            publish("budget_exhausted","terminal_selection_requires_commit_and_rollback_edit_reservation");return;
        }
        RequireTerminal(sourceEpoch<=9007199254740991ULL-NativeTerminalSelectionPolicy::MaximumTransactions,
            "terminal_selection_epoch_exhausted");
        const auto deadline=begin+plan.terminalSelection->maximumWallSeconds;
        const auto scope=[&] {
            RequireTerminal(current(),"terminal_selection_authority_revoked");
            RequireTerminal(!adapter.trainingRequested || adapter.trainingRequested(),"terminal_selection_external_stop");
            RequireTerminal(current(),"terminal_selection_authority_revoked");
        };
        const NativeTerminalSelectionPermit permit(scope,[&] {
            RequireTerminal(NativeTrainingWallTime()<deadline,"terminal_selection_finalization_time_budget_exhausted");
        });
        auto selectedCapacitance=std::make_unique<std::optional<TrainingCapacitance>>(selection.candidate.capacitance);
        const auto changed=adapter.restoreMeasuredBoundary(*retained,permit);
        if(!current())return;
        RequireTerminal(changed.transactions<=NativeTerminalSelectionPolicy::MaximumTransactions,
            "terminal_selection_transaction_accounting_invalid");
        terminalSelectionTransactions=changed.transactions;
        if(changed.transactions)terminalPhysicalEpoch=sourceEpoch+changed.transactions;
        event["rollback_attempted"]=changed.rollbackAttempted;event["rollback_succeeded"]=changed.rollbackSucceeded;
        const auto afterRevision=adapter.readFullModelRevision();if(!current())return;
        const auto afterParameters=adapter.readParameters();if(!current())return;
        event["actual_model_revision"]=QString::fromStdString(afterRevision);
        event["actual_parameters"]=TerminalParameters(afterParameters);
        // Preserve the source ledger unchanged. The selected shorter graph can
        // legitimately omit channels that only existed in its later source.
        auto actualCapacitance=adapter.readCapacitanceWitness?adapter.readCapacitanceWitness():
            std::optional<TrainingCapacitanceWitness>{};
        if(!current())return;
        RequireTerminal(!capacitanceHistory || actualCapacitance.has_value(),
            "terminal_selection_actual_capacitance_witness_missing");
        if(actualCapacitance)RequireTerminal(actualCapacitance->modelRevision==afterRevision,
            "terminal_selection_actual_capacitance_revision_mismatch");
        certificate.reset();
        if(changed.model.status==NativeModelBuildStatus::Adopted) {
            RequireTerminal(afterRevision==selection.candidate.modelRevision && afterParameters==selection.candidate.fullParameters,
                "terminal_selection_committed_model_mismatch");
            if(actualCapacitance)RequireTerminal(QJsonDocument::fromJson(QByteArray::fromStdString(
                actualCapacitance->nativeStateJson)).object()==retained->CapacitanceState(),
                "terminal_selection_actual_capacitance_state_mismatch");
            terminalCapacitanceBoundary=std::move(actualCapacitance);
            compositeCapacitance.swap(selectedCapacitance);referenceBinding=afterRevision;
            publish(changed.transactions?"restored":"already_current","");return;
        }
        RequireTerminal(!changed.model.modelMayHaveChanged && afterRevision==sourceRevision && afterParameters==sourceParameters,
            "terminal_selection_failed_without_verified_rollback");
        terminalCapacitanceBoundary=std::move(actualCapacitance);
        referenceBinding=afterRevision;
        publish(changed.rollbackSucceeded?"rolled_back":
            (changed.model.reason=="terminal_selection_finalization_time_budget_exhausted"?"budget_exhausted":"rejected"),changed.model.reason);
    }
    catch(const std::exception& error)
    {
        if(!current())return;
        certificate.reset();outcome=TrainingStatus::InvalidModel;
        reason="native_terminal_selection_failed:"+std::string(error.what());publish("failed",error.what());
    }
}
}
