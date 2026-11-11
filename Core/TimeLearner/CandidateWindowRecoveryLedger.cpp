#include "CandidateWindowRecoveryLedger.h"
#include "CandidateRecoveryEvidenceJson.h"
#include "CandidateWindowMeasurementEvidence.h"
#include "CandidateWindowTrainingDeclarations.h"
#include "ConfiguredNativeTraining.h"
#include "NativeCompositeTrainingEvidence.h"
#include "ProbeEvidenceJson.h"
#include "TrainingEvidenceJsonValues.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& value){Require(value.isObject(),"recovery_ledger_object");return value.toObject();}
void Keys(const QJsonObject& value,const char* fields) {
    const auto expected=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts),actual=value.keys();
    Require(std::set<QString>(expected.begin(),expected.end())==std::set<QString>(actual.begin(),actual.end()),"recovery_ledger_fields");
}
struct Previous {
    bool seen=false;
    std::size_t serial=0,invalid=0,probes=0,edits=0,accepted=0,epoch=0;
    QJsonObject runtime;
};
void Bind(const std::vector<NativeCandidateWindowRecoveryDelivery>& deliveries,const DecodedNativeCompositeArchive& archive) {
    Require(archive.Document()["schema_version"]==QJsonValue(2) && archive.Protocol().schemaVersion==5,
        "recovery_ledger_requires_replayed_archive2");
    const auto& records=archive.Records();std::map<std::string,Previous> history;std::set<std::string> groups,events;
    std::size_t lastPosition=0;bool first=true;
    for(const auto& delivery:deliveries) {
        const auto wire=EncodeCandidateWindowRecoveryEvidence(delivery.event);
        const auto& event=delivery.event;const auto& t=event.transition;
        Require(delivery.runtimePlan["schema_version"]==QJsonValue(3),"recovery_ledger_requires_runtime3");
        const auto runtime=DecodeCandidateWindowRuntimePlan(delivery.runtimePlan,archive.Protocol());
        Require(runtime.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow &&
            runtime.search.maximumInvalidObservations>0 && runtime.search.runId==t.runId && runtime.composite,
            "recovery_ledger_runtime_policy_or_attempt");
        const auto found=std::find_if(records.begin(),records.end(),[&](const auto& r){return r.groupId==event.groupId;});
        Require(found!=records.end(),"recovery_ledger_group_missing");const auto& r=*found;
        const auto position=std::size_t(found-records.begin());
        Require((first || position>lastPosition) && groups.insert(event.groupId).second &&
            events.insert(wire["event_id"].toString().toStdString()).second,"recovery_ledger_replay_or_order");
        first=false;lastPosition=position;
        Require(r.purpose==CompositeMeasurementPurpose::Search && !r.consumed && r.trainingRequestedAtConsume==true &&
            r.semanticProofId.empty() && r.attemptId==t.runId && r.logical.runId==t.runId &&
            r.logical.trialId==t.trialId && t.trialId==t.runId+"/search/"+std::to_string(t.actionSerial) &&
            r.logical.inputIndex==t.inputIndex && r.logical.epoch==t.modelEpoch && r.logical.baseModelRevision==t.baseModelRevision &&
            r.group.specification.effectiveModelRevision==t.effectiveModelRevision,"recovery_ledger_delivery_context");
        const auto binding=ParseStrictNativeJsonObject(r.candidateBindingJson,128u*1024u*1024u);
        Require(event.sourceOrigin==archive.SourceOrigin() && event.sourceOrigin==r.sourceOrigin &&
            event.ownerScope==archive.OwnerFamilyBinding() && event.ownerScope==r.ownerFamilyBinding &&
            event.candidateWitnessId==NmsdkTools::CanonicalModelDataSha256(binding).toStdString() &&
            event.rejectedContentId==CandidateWindowRejectedGroupContentId(archive.Protocol(),r),"recovery_ledger_primary_binding");
        const auto physical=Object(binding["physical_readback"]);
        const auto c=t.capacitance?QJsonValue(EvidenceJsonDetail::CapacitanceParameters(*t.capacitance)):QJsonValue(QJsonValue::Null);
        Require(physical["parameters"]==QJsonValue(EvidenceJsonDetail::Parameters(t.fullParameters)) && physical["capacitance"]==c,
            "recovery_ledger_effective_physics");
        Require(r.selection && r.selection->geometry &&
            ((t.validity==PeakValidity::Missing && r.selection->geometry->validity==DeclaredWindowBasinValidity::Missing) ||
             (t.validity==PeakValidity::Ambiguous && r.selection->geometry->validity==DeclaredWindowBasinValidity::Ambiguous)),
            "recovery_ledger_validity");
        const auto n=archive.Protocol().inputs.size();const auto& search=runtime.search;
        Require(n>1 && t.fullParameters.size()==n && search.bounds.size()==n && t.candidateInputIndex<n-1 &&
            t.inputIndex==(t.anchor?n-1:t.candidateInputIndex),"recovery_ledger_input_assignment");
        for(std::size_t i=0;i<n;++i) {
            const auto& p=t.fullParameters[i];const auto& b=search.bounds[i];
            Require(p.length>=b.minimumLength && p.length<=b.maximumLength && p.synapses>=b.minimumSynapses &&
                p.synapses<=b.maximumSynapses && p.resistance>=b.minimumResistance && p.resistance<=b.maximumResistance,
                "recovery_ledger_parameter_bounds");
        }
        const auto& limits=r.attemptLimits;const auto& policy=*runtime.composite;
        Require(limits.maximumGroups==policy.maximumGroups && limits.maximumNativeProbes==policy.maximumNativeProbes &&
            limits.maximumTotalSamples==policy.maximumTotalSamples && limits.maximumModelSeconds==runtime.maximumModelSeconds &&
            limits.maximumWallSeconds==runtime.maximumWallSeconds && r.policy.maximumTotalPrimaryBytes==policy.maximumTotalPrimaryBytes &&
            r.maximumGroupModelSeconds<=policy.maximumGroupModelSeconds && r.maximumGroupWallSeconds<=policy.maximumGroupWallSeconds,
            "recovery_ledger_runtime_limits");
        auto& previous=history[t.runId];
        Require(!previous.seen || (previous.runtime==delivery.runtimePlan && t.actionSerial>previous.serial &&
            t.invalidBefore>=previous.invalid && event.issuedProbes>=previous.probes && event.appliedEdits>=previous.edits &&
            event.acceptedCompositeObservations>=previous.accepted && t.modelEpoch>=previous.epoch),"recovery_ledger_attempt_history");
        Require(t.modelEpoch>=search.initialModelEpoch && event.issuedProbes<=search.maximumProbes && event.appliedEdits<=search.maximumEdits &&
            t.invalidBefore<=search.maximumInvalidObservations,"recovery_ledger_common_budget");
        if(t.disposition==CandidateRecoveryDisposition::InvalidBudgetStop)
            Require(t.invalidBefore==search.maximumInvalidObservations,"recovery_ledger_invalid_budget_stop");
        else Require(t.invalidBefore<search.maximumInvalidObservations,"recovery_ledger_advance_without_budget");
        if(t.disposition==CandidateRecoveryDisposition::RestoredBestStop)
            Require(t.reasonAfter=="restored_best_candidate_has_rejected_window","recovery_ledger_restored_best_reason");
        const auto accepted=std::count_if(records.begin(),found,[&](const auto& q){return q.attemptId==t.runId &&
            q.purpose==CompositeMeasurementPurpose::Search && q.consumed;});
        Require(event.acceptedCompositeObservations==std::size_t(accepted),"recovery_ledger_accepted_counter");
        previous={true,t.actionSerial,t.invalidAfter,event.issuedProbes,event.appliedEdits,event.acceptedCompositeObservations,
            t.modelEpoch,delivery.runtimePlan};
    }
}
}
QJsonObject EncodeCandidateWindowRecoveryLedger(const std::vector<NativeCandidateWindowRecoveryDelivery>& deliveries,
    const DecodedNativeCompositeArchive& archive,bool intact) {
    Require(intact,"recovery_ledger_producer_log_damaged");Bind(deliveries,archive);QJsonArray entries;
    for(const auto& delivery:deliveries)entries.append(QJsonObject{{"event",EncodeCandidateWindowRecoveryEvidence(delivery.event)},
        {"runtime_plan",delivery.runtimePlan}});
    QJsonObject result{{"kind","timelearner_candidate_window_recovery_ledger"},{"schema_version",1},
        {"source_origin",QString::fromStdString(archive.SourceOrigin())},{"owner_family_binding",QString::fromStdString(archive.OwnerFamilyBinding())},
        {"measurement_archive_document_id",archive.Document()["artifact_document_sha256"]},{"producer_log_intact",true},
        {"independent_delivery_completeness_verified",false},{"deliveries",entries}};
    result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
}
std::vector<NativeCandidateWindowRecoveryDelivery> DecodeCandidateWindowRecoveryLedger(const QJsonObject& wire,
    const DecodedNativeCompositeArchive& archive) {
    Keys(wire,"kind schema_version source_origin owner_family_binding measurement_archive_document_id producer_log_intact independent_delivery_completeness_verified deliveries sha256");
    Require(wire["kind"]==QJsonValue("timelearner_candidate_window_recovery_ledger") && wire["schema_version"]==QJsonValue(1) &&
        wire["producer_log_intact"]==QJsonValue(true) && wire["independent_delivery_completeness_verified"]==QJsonValue(false) &&
        wire["deliveries"].isArray(),"recovery_ledger_schema_or_claim");
    Require(std::size_t(wire["deliveries"].toArray().size())<=archive.Records().size(),"recovery_ledger_inventory_bound");
    std::vector<NativeCandidateWindowRecoveryDelivery> result;
    for(const auto& value:wire["deliveries"].toArray()) {
        const auto row=Object(value);Keys(row,"event runtime_plan");
        result.push_back({DecodeCandidateWindowRecoveryEvidence(Object(row["event"])),Object(row["runtime_plan"])});
    }
    Require(EncodeCandidateWindowRecoveryLedger(result,archive,true)==wire,"recovery_ledger_digest_or_readback");return result;
}
}
