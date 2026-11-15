#include "CandidateRecoveryEvidenceJson.h"
#include "BoundedTraining.h"
#include "TrainingEvidenceJsonValues.h"
#include "ModelIdentity.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void RequireRecovery(bool value,const char* reason) {
    if(!value)throw std::invalid_argument(reason);
}
void RecoveryKeys(const QJsonObject& value,const char* fields) {
    const auto expected=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts),actual=value.keys();
    RequireRecovery(std::set<QString>(expected.begin(),expected.end())==std::set<QString>(actual.begin(),actual.end()),
        "candidate_window_recovery_fields");
}
std::size_t RecoveryCount(const QJsonValue& value) {
    RequireRecovery(value.isDouble(),"candidate_window_recovery_integer");const auto n=value.toDouble();
    RequireRecovery(std::isfinite(n) && n>=0. && n<=9007199254740991. &&
        n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n,"candidate_window_recovery_integer");
    return std::size_t(n);
}
std::string RecoveryText(const QJsonValue& value) {
    RequireRecovery(value.isString(),"candidate_window_recovery_text");return value.toString().toStdString();
}
QJsonObject RecoveryObject(const QJsonValue& value) {
    RequireRecovery(value.isObject(),"candidate_window_recovery_object");return value.toObject();
}
double RecoveryNumber(const QJsonValue& value) {
    RequireRecovery(value.isDouble() && std::isfinite(value.toDouble()),"candidate_window_recovery_number");
    return value.toDouble();
}
}
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
CandidateWindowRecoveryEvidence DecodeCandidateWindowRecoveryEvidence(const QJsonObject& document)
{
    RecoveryKeys(document,"kind schema_version transition group_id rejected_content_id source_origin owner_scope candidate_witness_id issued_probes applied_edits accepted_composite_observations event_id");
    RequireRecovery(document["kind"]==QJsonValue("timelearner_candidate_window_recovery_event") &&
        RecoveryCount(document["schema_version"])==1,"candidate_window_recovery_version");
    CandidateWindowRecoveryEvidence result;auto& e=result.transition;const auto t=RecoveryObject(document["transition"]);
    RecoveryKeys(t,"action_serial input_index candidate_input_index sweep model_epoch invalid_before invalid_after run_id trial_id base_model_revision effective_model_revision phase validity full_parameters capacitance disposition status_after reason_after");
    e.actionSerial=RecoveryCount(t["action_serial"]);e.inputIndex=RecoveryCount(t["input_index"]);
    e.candidateInputIndex=RecoveryCount(t["candidate_input_index"]);e.sweep=RecoveryCount(t["sweep"]);
    e.modelEpoch=RecoveryCount(t["model_epoch"]);e.invalidBefore=RecoveryCount(t["invalid_before"]);e.invalidAfter=RecoveryCount(t["invalid_after"]);
    e.runId=RecoveryText(t["run_id"]);e.trialId=RecoveryText(t["trial_id"]);
    e.baseModelRevision=RecoveryText(t["base_model_revision"]);e.effectiveModelRevision=RecoveryText(t["effective_model_revision"]);
    const auto phase=RecoveryText(t["phase"]),validity=RecoveryText(t["validity"]);
    RequireRecovery(phase=="anchor" || phase=="input","candidate_window_recovery_phase");e.anchor=phase=="anchor";
    RequireRecovery(validity=="missing" || validity=="ambiguous","candidate_window_recovery_validity");
    e.validity=validity=="missing"?PeakValidity::Missing:PeakValidity::Ambiguous;
    RequireRecovery(t["full_parameters"].isArray(),"candidate_window_recovery_parameters_array");
    for(const auto& item:t["full_parameters"].toArray()) {
        const auto p=RecoveryObject(item);RecoveryKeys(p,"length synapses resistance");
        const auto l=RecoveryCount(p["length"]),n=RecoveryCount(p["synapses"]);
        RequireRecovery(l<=std::size_t(std::numeric_limits<int>::max()) && n<=128,"candidate_window_recovery_parameter_range");
        e.fullParameters.push_back({int(l),int(n),RecoveryNumber(p["resistance"])});
    }
    if(!t["capacitance"].isNull()) {
        const auto c=RecoveryObject(t["capacitance"]);RecoveryKeys(c,"origin maximum_relative_change membrane_scales");
        TrainingCapacitance values;values.origin=RecoveryText(c["origin"]);values.maximumRelativeChange=RecoveryNumber(c["maximum_relative_change"]);
        const auto scales=RecoveryObject(c["membrane_scales"]);
        for(auto it=scales.begin();it!=scales.end();++it)values.membraneScales.emplace(it.key().toStdString(),RecoveryNumber(it.value()));
        e.capacitance=std::move(values);ValidateTrainingCapacitance(e.capacitance);
    }
    const auto disposition=RecoveryText(t["disposition"]),status=RecoveryText(t["status_after"]);
    if(disposition=="cursor_advanced")e.disposition=CandidateRecoveryDisposition::CursorAdvanced;
    else if(disposition=="invalid_budget_stop")e.disposition=CandidateRecoveryDisposition::InvalidBudgetStop;
    else if(disposition=="restored_best_stop")e.disposition=CandidateRecoveryDisposition::RestoredBestStop;
    else throw std::invalid_argument("candidate_window_recovery_disposition");
    if(status=="searching")e.statusAfter=TrainingStatus::Searching;
    else if(status=="invalid_observation")e.statusAfter=TrainingStatus::InvalidObservation;
    else if(status=="search_unresolved")e.statusAfter=TrainingStatus::SearchUnresolved;
    else throw std::invalid_argument("candidate_window_recovery_status");
    e.reasonAfter=RecoveryText(t["reason_after"]);result.groupId=RecoveryText(document["group_id"]);
    result.rejectedContentId=RecoveryText(document["rejected_content_id"]);result.sourceOrigin=RecoveryText(document["source_origin"]);
    result.ownerScope=RecoveryText(document["owner_scope"]);result.candidateWitnessId=RecoveryText(document["candidate_witness_id"]);
    result.issuedProbes=RecoveryCount(document["issued_probes"]);result.appliedEdits=RecoveryCount(document["applied_edits"]);
    result.acceptedCompositeObservations=RecoveryCount(document["accepted_composite_observations"]);
    // Encoding performs the shared range/outcome checks and recomputes the
    // complete canonical hash. No unknown/coerced/lost field is normalized away.
    RequireRecovery(EncodeCandidateWindowRecoveryEvidence(result)==document,"candidate_window_recovery_digest_or_readback");
    return result;
}
}
