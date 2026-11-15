#include "CandidateWindowTrainingDeclarations.h"
#include "CompositeTrainingDeclarations.h"
#include "ConfiguredNativeTraining.h"
#include "ModelIdentity.h"
#include "ProbeEvidenceJson.h"
#include <QJsonArray>
#include <QStringList>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
constexpr std::uint64_t Maximum=CandidateWindowMaximumExactCount;
void Require(bool value,const char* reason="candidate_window_runtime2_field") {
    if(!value)throw std::invalid_argument(reason);
}
void Keys(const QJsonObject& object,const char* fields) {
    auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);names.sort();Require(object.keys()==names);
}
QJsonObject Object(const QJsonValue& value){Require(value.isObject());return value.toObject();}
QJsonArray Array(const QJsonValue& value){Require(value.isArray());return value.toArray();}
std::string Text(const QJsonValue& value){Require(value.isString());return value.toString().toStdString();}
QString Text(const std::string& value){return QString::fromStdString(value);}
double Number(double value){Require(std::isfinite(value));return value;}
double Number(const QJsonValue& value){Require(value.isDouble());return Number(value.toDouble());}
double Count(std::size_t value){Require(value<=Maximum);return double(value);}
std::size_t Count(const QJsonValue& value) {
    const auto number=Number(value);Require(number>=0. && number<=double(Maximum) &&
        number<=double(std::numeric_limits<std::size_t>::max()) && std::floor(number)==number);return std::size_t(number);
}
int Integer(const QJsonValue& value) {
    const auto number=Count(value);Require(number<=std::size_t(std::numeric_limits<int>::max()));return int(number);
}
void ValidateDeclaration(const NativeTrainingPlan& plan) {
    ValidateVerificationProtocol(plan.protocol);ValidateCandidateWindowMeasurementContract(plan.protocol);
    Require(plan.composite && plan.composite->schemaVersion==2,"candidate_window_runtime2_policy_required");
    const auto& common=*plan.composite;const auto& method=*plan.protocol.candidateWindowMeasurement;
    const auto& search=plan.search;const auto n=plan.protocol.inputs.size();
    Require(common.methodId==method.methodId && common.contextPolicy==method.contextPolicy &&
        common.geometryPolicy==method.geometryPolicy,"candidate_window_runtime2_protocol_policy_mismatch");
    Require(!search.runId.empty() && !search.initialModelRevision.empty() && search.bounds.size()==n,
        "candidate_window_runtime2_unbound_search");
    for(auto value:{search.initialModelEpoch,search.maximumProbes,search.maximumEdits,search.maximumCandidatesPerInput,
        search.maximumSweeps,search.maximumInvalidObservations,search.n0CandidateRepeats,
        plan.maximumReferenceProbes,plan.maximumInvalidReferenceProbes})Count(value);
    Require(search.maximumProbes && search.maximumEdits && search.maximumCandidatesPerInput && search.maximumSweeps &&
        search.resistanceSubdivisionDepth<=20 && search.n0CandidateRepeats>=1 && search.n0CandidateRepeats<=128 &&
        (search.normalization==Normalization::SynapseCount || search.normalization==Normalization::TipResistance) &&
        (search.normalization!=Normalization::TipResistance || search.n0CandidateRepeats==1),
        "candidate_window_runtime2_search_limits");
    ValidateInvalidMeasurementRecovery(search.invalidMeasurementRecovery,search.maximumInvalidObservations);
    Require(search.invalidMeasurementRecovery==InvalidMeasurementRecovery::RetrySameCandidate ||
        search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow,
        "candidate_window_runtime2_recovery_unsupported");
    for(auto value:{search.tolerances.amplitudeAbsolute,search.tolerances.amplitudeRelative,search.tolerances.sync})
        Require(Number(value)>=0.,"candidate_window_runtime2_tolerance");
    for(const auto& bound:search.bounds)Require(bound.minimumLength>=1 && bound.maximumLength>=bound.minimumLength &&
        bound.minimumSynapses>=1 && bound.maximumSynapses>=bound.minimumSynapses && bound.maximumSynapses<=128 &&
        Number(bound.minimumResistance)>0. && Number(bound.maximumResistance)>=bound.minimumResistance,
        "candidate_window_runtime2_search_bounds");
    ValidateCapacitanceSearchPolicy(search,plan.protocol.routing);
    for(auto value:{common.maximumGroups,common.maximumNativeProbes,common.maximumTotalSamples,common.maximumTotalPrimaryBytes})
        Require(Count(value)>0.,"candidate_window_runtime2_attempt_limits");
    Require(plan.maximumReferenceProbes>=n && common.maximumNativeProbes>=n+2 &&
        common.maximumTotalSamples>=common.contextPolicy.maximumTotalSamples &&
        Number(plan.maximumModelSeconds)>0. && Number(plan.maximumWallSeconds)>0. &&
        Number(common.maximumGroupModelSeconds)>0. && common.maximumGroupModelSeconds<=plan.maximumModelSeconds &&
        Number(common.maximumGroupWallSeconds)>0. && common.maximumGroupWallSeconds<=plan.maximumWallSeconds,
        "candidate_window_runtime2_budget_relations");
}
QJsonObject Search(const BoundedTrainingConfig& search) {
    QJsonArray bounds;for(const auto& bound:search.bounds)bounds.append(QJsonObject{
        {"minimum_length",bound.minimumLength},{"maximum_length",bound.maximumLength},
        {"minimum_synapses",bound.minimumSynapses},{"maximum_synapses",bound.maximumSynapses},
        {"minimum_resistance",Number(bound.minimumResistance)},{"maximum_resistance",Number(bound.maximumResistance)}});
    return {{"normalization",search.normalization==Normalization::SynapseCount?"N0":"N1"},
        {"maximum_probes",Count(search.maximumProbes)},{"maximum_edits",Count(search.maximumEdits)},
        {"maximum_candidates_per_input",Count(search.maximumCandidatesPerInput)},{"maximum_sweeps",Count(search.maximumSweeps)},
        {"maximum_invalid_observations",Count(search.maximumInvalidObservations)},
        {"resistance_subdivision_depth",Count(search.resistanceSubdivisionDepth)},{"n0_candidate_repeats",Count(search.n0CandidateRepeats)},
        {"bounds",bounds},{"tolerances",QJsonObject{{"amplitude_absolute",Number(search.tolerances.amplitudeAbsolute)},
            {"amplitude_relative",Number(search.tolerances.amplitudeRelative)},{"sync",Number(search.tolerances.sync)}}},
        {"capacitance_search",search.capacitanceSearch?QJsonValue(EncodeCapacitanceSearchPolicy(*search.capacitanceSearch)):QJsonValue(QJsonValue::Null)},
        {"invalid_measurement_recovery",search.invalidMeasurementRecovery==InvalidMeasurementRecovery::RetrySameCandidate?
            "retry_same_candidate":"advance_rejected_candidate_window"}};
}
BoundedTrainingConfig Search(const QJsonObject& object,std::size_t version) {
    Keys(object,"normalization maximum_probes maximum_edits maximum_candidates_per_input maximum_sweeps maximum_invalid_observations resistance_subdivision_depth n0_candidate_repeats bounds tolerances capacitance_search invalid_measurement_recovery");
    BoundedTrainingConfig search;const auto normalization=Text(object["normalization"]);Require(normalization=="N0" || normalization=="N1");
    search.normalization=normalization=="N0"?Normalization::SynapseCount:Normalization::TipResistance;
    search.maximumProbes=Count(object["maximum_probes"]);search.maximumEdits=Count(object["maximum_edits"]);
    search.maximumCandidatesPerInput=Count(object["maximum_candidates_per_input"]);search.maximumSweeps=Count(object["maximum_sweeps"]);
    search.maximumInvalidObservations=Count(object["maximum_invalid_observations"]);
    const auto depth=Count(object["resistance_subdivision_depth"]);Require(depth<=20);search.resistanceSubdivisionDepth=unsigned(depth);
    search.n0CandidateRepeats=Count(object["n0_candidate_repeats"]);
    for(const auto& value:Array(object["bounds"])) {
        const auto bound=Object(value);Keys(bound,"minimum_length maximum_length minimum_synapses maximum_synapses minimum_resistance maximum_resistance");
        search.bounds.push_back({Integer(bound["minimum_length"]),Integer(bound["maximum_length"]),
            Integer(bound["minimum_synapses"]),Integer(bound["maximum_synapses"]),
            Number(bound["minimum_resistance"]),Number(bound["maximum_resistance"])});
    }
    const auto tolerances=Object(object["tolerances"]);Keys(tolerances,"amplitude_absolute amplitude_relative sync");
    search.tolerances={Number(tolerances["amplitude_absolute"]),Number(tolerances["amplitude_relative"]),Number(tolerances["sync"])};
    if(!object["capacitance_search"].isNull())search.capacitanceSearch=DecodeCapacitanceSearchPolicy(Object(object["capacitance_search"]));
    const auto policy=Text(object["invalid_measurement_recovery"]);
    Require((version==2 && policy=="retry_same_candidate") ||
        (version==3 && policy=="advance_rejected_candidate_window"),"candidate_window_runtime_recovery_version_mismatch");
    search.invalidMeasurementRecovery=version==2?InvalidMeasurementRecovery::RetrySameCandidate:
        InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow;
    return search;
}
}
QJsonObject EncodeCandidateWindowRuntimePlan(const NativeTrainingPlan& plan) {
    ValidateDeclaration(plan);auto common=EncodeNativeCompositeTrainingPolicy(*plan.composite);
    common["candidate_window_policy"]=EncodeCandidateWindowPolicy(plan.protocol.candidateWindowMeasurement->candidateWindowPolicy);
    return {{"schema_version",plan.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::RetrySameCandidate?2:3},
        {"protocol_binding",NmsdkTools::CanonicalModelDataSha256(EncodeVerificationProtocol(plan.protocol))},
        {"run_id",Text(plan.search.runId)},{"initial_model_revision",Text(plan.search.initialModelRevision)},
        {"initial_model_epoch",Count(plan.search.initialModelEpoch)},{"search",Search(plan.search)},
        {"maximum_reference_probes",Count(plan.maximumReferenceProbes)},
        {"maximum_invalid_reference_probes",Count(plan.maximumInvalidReferenceProbes)},
        {"maximum_model_seconds",Number(plan.maximumModelSeconds)},{"maximum_wall_seconds",Number(plan.maximumWallSeconds)},
        {"record_capacitance_history",plan.recordCapacitanceHistory},{"composite_measurement",common}};
}
NativeTrainingPlan DecodeCandidateWindowRuntimePlan(const QJsonObject& object,const VerificationProtocol& protocol) {
    Keys(object,"schema_version protocol_binding run_id initial_model_revision initial_model_epoch search maximum_reference_probes maximum_invalid_reference_probes maximum_model_seconds maximum_wall_seconds record_capacitance_history composite_measurement");
    const auto version=Count(object["schema_version"]);
    Require((version==2 || version==3) && object["record_capacitance_history"].isBool());
    ValidateVerificationProtocol(protocol);ValidateCandidateWindowMeasurementContract(protocol);
    Require(object["protocol_binding"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(EncodeVerificationProtocol(protocol))),
        "candidate_window_runtime2_protocol_binding");
    auto common=Object(object["composite_measurement"]);Keys(common,"schema_version method_id context_policy geometry_policy attempt_limits maximum_group_model_seconds maximum_group_wall_seconds maximum_total_primary_bytes candidate_window_policy");
    Require(DecodeCandidateWindowPolicy(Object(common.take("candidate_window_policy")))==protocol.candidateWindowMeasurement->candidateWindowPolicy,
        "candidate_window_runtime2_candidate_policy_mismatch");
    NativeTrainingPlan plan;plan.protocol=protocol;plan.composite=DecodeNativeCompositeTrainingPolicy(common);plan.search=Search(Object(object["search"]),version);
    plan.search.runId=Text(object["run_id"]);plan.search.initialModelRevision=Text(object["initial_model_revision"]);
    plan.search.initialModelEpoch=Count(object["initial_model_epoch"]);plan.maximumReferenceProbes=Count(object["maximum_reference_probes"]);
    plan.maximumInvalidReferenceProbes=Count(object["maximum_invalid_reference_probes"]);
    plan.maximumModelSeconds=Number(object["maximum_model_seconds"]);plan.maximumWallSeconds=Number(object["maximum_wall_seconds"]);
    plan.recordCapacitanceHistory=object["record_capacitance_history"].toBool();Require(EncodeCandidateWindowRuntimePlan(plan)==object);return plan;
}
}
