#include "CandidateRecoveryEvidenceJson.h"
#include "BoundedTraining.h"
#include "TrainingEvidenceJsonValues.h"

namespace NMSDK::TimeLearner {
QJsonObject EncodeCandidateRecoveryEvidence(const CandidateRecoveryEvidence& e)
{
    using namespace EvidenceJsonDetail;
    if((e.validity!=PeakValidity::Missing && e.validity!=PeakValidity::Ambiguous) ||
       (e.statusAfter!=TrainingStatus::Searching && e.statusAfter!=TrainingStatus::InvalidObservation &&
        e.statusAfter!=TrainingStatus::SearchUnresolved))
        throw std::invalid_argument("invalid_candidate_recovery_enum");
    const char* disposition=nullptr;
    switch(e.disposition)
    {
    case CandidateRecoveryDisposition::CursorAdvanced:disposition="cursor_advanced";break;
    case CandidateRecoveryDisposition::InvalidBudgetStop:disposition="invalid_budget_stop";break;
    case CandidateRecoveryDisposition::RestoredBestStop:disposition="restored_best_stop";break;
    }
    if(!disposition)throw std::invalid_argument("unknown_candidate_recovery_disposition");
    return {{"action_serial",Count(e.actionSerial)},{"input_index",Count(e.inputIndex)},
        {"candidate_input_index",Count(e.candidateInputIndex)},{"sweep",Count(e.sweep)},
        {"model_epoch",Count(e.modelEpoch)},{"invalid_before",Count(e.invalidBefore)},
        {"invalid_after",Count(e.invalidAfter)},{"run_id",Text(e.runId)},{"trial_id",Text(e.trialId)},
        {"base_model_revision",Text(e.baseModelRevision)},{"effective_model_revision",Text(e.effectiveModelRevision)},
        {"phase",e.anchor?"anchor":"input"},{"validity",PeakValidityName(e.validity)},
        {"full_parameters",Parameters(e.fullParameters)},
        {"capacitance",e.capacitance?QJsonValue(CapacitanceParameters(*e.capacitance)):QJsonValue(QJsonValue::Null)},
        {"disposition",disposition},{"status_after",TrainingStatusName(e.statusAfter)},
        {"reason_after",Text(e.reasonAfter)}};
}
}
