#include "CandidateRecoveryEvidenceJson.h"
#include "BoundedTraining.h"
#include "TrainingEvidenceJsonValues.h"
#include "ModelIdentity.h"
#include <algorithm>

namespace NMSDK::TimeLearner {
QJsonObject EncodeCandidateWindowRecoveryEvidence(const CandidateWindowRecoveryEvidence& event)
{
    using namespace EvidenceJsonDetail;
    const auto& e=event.transition;
    const auto hash=[](const std::string& value){return value.size()==64 &&
        std::all_of(value.begin(),value.end(),[](char c){return (c>='0' && c<='9') || (c>='a' && c<='f');});};
    if(event.groupId.empty() || event.ownerScope.empty() || !hash(event.rejectedContentId) ||
        !hash(event.sourceOrigin) || !hash(event.candidateWitnessId) ||
        e.runId.empty() || e.trialId.empty() || e.baseModelRevision.empty() || e.effectiveModelRevision.empty() ||
        !e.actionSerial || !event.issuedProbes || event.acceptedCompositeObservations>event.issuedProbes ||
        e.inputIndex>=e.fullParameters.size() || e.candidateInputIndex>=e.fullParameters.size() ||
        e.invalidBefore>=9007199254740991ULL || e.invalidAfter!=e.invalidBefore+1)
        throw std::invalid_argument("invalid_candidate_window_recovery_event");
    if((e.disposition==CandidateRecoveryDisposition::CursorAdvanced && e.statusAfter!=TrainingStatus::Searching) ||
        (e.disposition==CandidateRecoveryDisposition::InvalidBudgetStop && e.statusAfter!=TrainingStatus::InvalidObservation) ||
        (e.disposition==CandidateRecoveryDisposition::RestoredBestStop && e.statusAfter!=TrainingStatus::SearchUnresolved))
        throw std::invalid_argument("inconsistent_candidate_window_recovery_outcome");
    for(const auto& p:e.fullParameters)if(p.length<1 || p.synapses<1 || p.synapses>128 ||
        !std::isfinite(p.resistance) || p.resistance<=0.)
        throw std::invalid_argument("invalid_candidate_window_recovery_parameters");
    QJsonObject document{{"kind","timelearner_candidate_window_recovery_event"},{"schema_version",1},
        {"transition",EncodeCandidateRecoveryEvidence(e)},
        {"group_id",Text(event.groupId)},{"rejected_content_id",Text(event.rejectedContentId)},
        {"source_origin",Text(event.sourceOrigin)},{"owner_scope",Text(event.ownerScope)},
        {"candidate_witness_id",Text(event.candidateWitnessId)},
        {"issued_probes",Count(event.issuedProbes)},{"applied_edits",Count(event.appliedEdits)},
        {"accepted_composite_observations",Count(event.acceptedCompositeObservations)}};
    document["event_id"]=NmsdkTools::CanonicalModelDataSha256(document);
    return document;
}
}
