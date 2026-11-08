#include "CompositeTrainingDeclarations.h"
#include "ConfiguredNativeTraining.h"
#include <QJsonArray>
#include <QStringList>
#include <cmath>
#include <limits>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v){if(!v)throw std::invalid_argument("invalid_composite_declaration");}
void Keys(const QJsonObject& o,const char* fields){auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);names.sort();Require(o.keys()==names);}
double Number(const QJsonValue& v){Require(v.isDouble() && std::isfinite(v.toDouble()));return v.toDouble();}
double Number(double v){Require(std::isfinite(v));return v;}
std::size_t Count(const QJsonValue& v){const auto n=Number(v);Require(n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n);return std::size_t(n);}
double Count(std::size_t n){Require(n<=9007199254740991ULL);return double(n);}
unsigned Version(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<unsigned>::max());return unsigned(n);}
QJsonObject Object(const QJsonValue& v){Require(v.isObject());return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray());return v.toArray();}
QString Text(const std::string& v){return QString::fromStdString(v);}
std::string Text(const QJsonValue& v){Require(v.isString());return v.toString().toStdString();}
QJsonArray Strings(const std::vector<std::string>& v){QJsonArray a;for(const auto& s:v)a.append(Text(s));return a;}
}
QJsonObject EncodeCompositeContextPolicy(const MatchedOmissionPolicy& p) {
    return {{"schema_version",Count(p.schemaVersion)},{"comparison_radius_ticks",Count(p.comparisonRadiusTicks)},
        {"maximum_samples",Count(p.maximumSamples)},{"maximum_total_samples",Count(p.maximumTotalSamples)},
        {"maximum_native_probes",Count(p.maximumNativeProbes)},{"time_comparison_tolerance",Number(p.timeComparisonTolerance)},
        {"baseline_absolute",Number(p.baselineAbsolute)},{"repeatability_absolute",Number(p.repeatabilityAbsolute)},
        {"repeatability_relative",Number(p.repeatabilityRelative)},{"influence_absolute",Number(p.influenceAbsolute)},
        {"influence_relative",Number(p.influenceRelative)}};
}
MatchedOmissionPolicy DecodeCompositeContextPolicy(const QJsonObject& o) {
    Keys(o,"schema_version comparison_radius_ticks maximum_samples maximum_total_samples maximum_native_probes time_comparison_tolerance baseline_absolute repeatability_absolute repeatability_relative influence_absolute influence_relative");
    MatchedOmissionPolicy p;p.schemaVersion=Version(o["schema_version"]);p.comparisonRadiusTicks=Count(o["comparison_radius_ticks"]);
    p.maximumSamples=Count(o["maximum_samples"]);p.maximumTotalSamples=Count(o["maximum_total_samples"]);p.maximumNativeProbes=Count(o["maximum_native_probes"]);
    p.timeComparisonTolerance=Number(o["time_comparison_tolerance"]);p.baselineAbsolute=Number(o["baseline_absolute"]);
    p.repeatabilityAbsolute=Number(o["repeatability_absolute"]);p.repeatabilityRelative=Number(o["repeatability_relative"]);
    p.influenceAbsolute=Number(o["influence_absolute"]);p.influenceRelative=Number(o["influence_relative"]);return p;
}
QJsonObject EncodeCompositeGeometryPolicy(const DeclaredWindowBasinPolicy& p) {
    return {{"schema_version",Count(p.schemaVersion)},{"maximum_inputs",Count(p.maximumInputs)},
        {"maximum_samples",Count(p.maximumSamples)},{"maximum_total_samples",Count(p.maximumTotalSamples)},
        {"repeatability_absolute",Number(p.repeatabilityAbsolute)}};
}
DeclaredWindowBasinPolicy DecodeCompositeGeometryPolicy(const QJsonObject& o) {
    Keys(o,"schema_version maximum_inputs maximum_samples maximum_total_samples repeatability_absolute");
    DeclaredWindowBasinPolicy p;p.schemaVersion=Version(o["schema_version"]);p.maximumInputs=Count(o["maximum_inputs"]);
    p.maximumSamples=Count(o["maximum_samples"]);p.maximumTotalSamples=Count(o["maximum_total_samples"]);p.repeatabilityAbsolute=Number(o["repeatability_absolute"]);return p;
}
QJsonObject EncodeCompositeMeasurementContract(const CompositeMeasurementContract& c) {
    QJsonArray windows;for(const auto& w:c.inputWindows)windows.append(QJsonObject{{"begin_offset",Number(w.beginOffset)},{"end_offset",Number(w.endOffset)}});
    return {{"schema_version",Count(c.schemaVersion)},{"method_id",Text(c.methodId)},
        {"context_policy",EncodeCompositeContextPolicy(c.contextPolicy)},{"geometry_policy",EncodeCompositeGeometryPolicy(c.geometryPolicy)},{"input_windows",windows}};
}
CompositeMeasurementContract DecodeCompositeMeasurementContract(const QJsonObject& o) {
    Keys(o,"schema_version method_id context_policy geometry_policy input_windows");CompositeMeasurementContract c;
    c.schemaVersion=Version(o["schema_version"]);c.methodId=Text(o["method_id"]);c.contextPolicy=DecodeCompositeContextPolicy(Object(o["context_policy"]));
    c.geometryPolicy=DecodeCompositeGeometryPolicy(Object(o["geometry_policy"]));for(const auto& v:Array(o["input_windows"])) {
        const auto w=Object(v);Keys(w,"begin_offset end_offset");c.inputWindows.push_back({Number(w["begin_offset"]),Number(w["end_offset"])});
    }return c;
}
QJsonObject EncodeNativeCompositeTrainingPolicy(const NativeCompositeTrainingPolicy& p) {
    return {{"schema_version",Count(p.schemaVersion)},{"method_id",Text(p.methodId)},
        {"context_policy",EncodeCompositeContextPolicy(p.contextPolicy)},{"geometry_policy",EncodeCompositeGeometryPolicy(p.geometryPolicy)},
        {"attempt_limits",QJsonObject{{"maximum_groups",Count(p.maximumGroups)},{"maximum_native_probes",Count(p.maximumNativeProbes)},
            {"maximum_total_samples",Count(p.maximumTotalSamples)}}},
        {"maximum_group_model_seconds",Number(p.maximumGroupModelSeconds)},{"maximum_group_wall_seconds",Number(p.maximumGroupWallSeconds)},
        {"maximum_total_primary_bytes",Count(p.maximumTotalPrimaryBytes)}};
}
NativeCompositeTrainingPolicy DecodeNativeCompositeTrainingPolicy(const QJsonObject& o) {
    Keys(o,"schema_version method_id context_policy geometry_policy attempt_limits maximum_group_model_seconds maximum_group_wall_seconds maximum_total_primary_bytes");
    NativeCompositeTrainingPolicy p;p.schemaVersion=Version(o["schema_version"]);p.methodId=Text(o["method_id"]);
    p.contextPolicy=DecodeCompositeContextPolicy(Object(o["context_policy"]));p.geometryPolicy=DecodeCompositeGeometryPolicy(Object(o["geometry_policy"]));
    const auto limits=Object(o["attempt_limits"]);Keys(limits,"maximum_groups maximum_native_probes maximum_total_samples");
    p.maximumGroups=Count(limits["maximum_groups"]);p.maximumNativeProbes=Count(limits["maximum_native_probes"]);p.maximumTotalSamples=Count(limits["maximum_total_samples"]);
    p.maximumGroupModelSeconds=Number(o["maximum_group_model_seconds"]);p.maximumGroupWallSeconds=Number(o["maximum_group_wall_seconds"]);
    p.maximumTotalPrimaryBytes=Count(o["maximum_total_primary_bytes"]);return p;
}
QJsonObject CompositeTrainingProtocolBinding(const VerificationProtocol& p) {
    ValidateVerificationProtocol(p);ValidateCompositeMeasurementContract(p);
    const auto& c=*p.compositeMeasurement;const auto& m=c.contextPolicy;const auto& g=c.geometryPolicy;
    QJsonArray windows,inputs;for(const auto& w:c.inputWindows)windows.append(QJsonArray{w.beginOffset,w.endOffset});
    for(const auto& i:p.inputs)inputs.append(QJsonArray{Text(i.id),Text(i.stimulusId)});const auto& s=*p.stimulusProfile;
    return {{"schema_version",4},{"method",Text(c.methodId)},{"contract_schema",int(c.schemaVersion)},
        {"routing",int(p.routing)},{"revision",Text(p.revision)},{"sample",Text(p.sampleId)},{"pattern",Text(p.patternHash)},
        {"origin",Text(p.originStimulusId)},{"inputs",inputs},{"stimuli",Strings(p.stimulusIds)},
        {"sample_gap",p.maximumSampleGap},{"minimum_amplitude",p.minimumAmplitude},{"windows",windows},
        {"source",QJsonArray{Text(s.generatorClass),s.amplitude,s.pulseLength,s.frequencyHz,s.frequencyDeviation,s.useTransitSignal,s.usePatternOutput}},
        {"context_policy",QJsonArray{int(m.schemaVersion),double(m.comparisonRadiusTicks),double(m.maximumSamples),
            double(m.maximumTotalSamples),double(m.maximumNativeProbes),m.timeComparisonTolerance,m.baselineAbsolute,
            m.repeatabilityAbsolute,m.repeatabilityRelative,m.influenceAbsolute,m.influenceRelative}},
        {"geometry_policy",QJsonArray{int(g.schemaVersion),double(g.maximumInputs),double(g.maximumSamples),double(g.maximumTotalSamples),g.repeatabilityAbsolute}}};
}
QJsonObject EncodeCompositeRuntimePlan(const NativeTrainingPlan& p) {
    ValidateCompositeMeasurementContract(p.protocol);Require(bool(p.composite));const auto& c=p.search;
    Require(c.normalization==Normalization::SynapseCount || c.normalization==Normalization::TipResistance);
    Require(c.invalidMeasurementRecovery==InvalidMeasurementRecovery::RetrySameCandidate);
    QJsonArray bounds;for(const auto& b:c.bounds)bounds.append(QJsonObject{{"minimum_length",b.minimumLength},{"maximum_length",b.maximumLength},
        {"minimum_synapses",b.minimumSynapses},{"maximum_synapses",b.maximumSynapses},{"minimum_resistance",Number(b.minimumResistance)},{"maximum_resistance",Number(b.maximumResistance)}});
    QJsonObject search{{"normalization",c.normalization==Normalization::SynapseCount?"N0":"N1"},
        {"maximum_probes",Count(c.maximumProbes)},{"maximum_edits",Count(c.maximumEdits)},
        {"maximum_candidates_per_input",Count(c.maximumCandidatesPerInput)},{"maximum_sweeps",Count(c.maximumSweeps)},
        {"maximum_invalid_observations",Count(c.maximumInvalidObservations)},{"resistance_subdivision_depth",Count(c.resistanceSubdivisionDepth)},
        {"n0_candidate_repeats",Count(c.n0CandidateRepeats)},{"bounds",bounds},
        {"tolerances",QJsonObject{{"amplitude_absolute",Number(c.tolerances.amplitudeAbsolute)},
            {"amplitude_relative",Number(c.tolerances.amplitudeRelative)},{"sync",Number(c.tolerances.sync)}}},
        {"capacitance_search",c.capacitanceSearch?QJsonValue(EncodeCapacitanceSearchPolicy(*c.capacitanceSearch)):QJsonValue(QJsonValue::Null)},
        {"invalid_measurement_recovery","retry_same_candidate"}};
    return {{"schema_version",1},{"run_id",Text(c.runId)},{"initial_model_revision",Text(c.initialModelRevision)},
        {"initial_model_epoch",Count(c.initialModelEpoch)},{"search",search},
        {"maximum_reference_probes",Count(p.maximumReferenceProbes)},{"maximum_invalid_reference_probes",Count(p.maximumInvalidReferenceProbes)},
        {"maximum_model_seconds",Number(p.maximumModelSeconds)},{"maximum_wall_seconds",Number(p.maximumWallSeconds)},
        {"record_capacitance_history",p.recordCapacitanceHistory},{"composite_measurement",EncodeNativeCompositeTrainingPolicy(*p.composite)}};
}
NativeTrainingPlan DecodeCompositeRuntimePlan(const QJsonObject& o,const VerificationProtocol& protocol) {
    Keys(o,"schema_version run_id initial_model_revision initial_model_epoch search maximum_reference_probes maximum_invalid_reference_probes maximum_model_seconds maximum_wall_seconds record_capacitance_history composite_measurement");
    Require(Version(o["schema_version"])==1 && o["record_capacitance_history"].isBool());
    NativeTrainingPlan p;p.protocol=protocol;p.composite=DecodeNativeCompositeTrainingPolicy(Object(o["composite_measurement"]));
    auto& c=p.search;c.runId=Text(o["run_id"]);c.initialModelRevision=Text(o["initial_model_revision"]);c.initialModelEpoch=Count(o["initial_model_epoch"]);
    const auto search=Object(o["search"]);Keys(search,"normalization maximum_probes maximum_edits maximum_candidates_per_input maximum_sweeps maximum_invalid_observations resistance_subdivision_depth n0_candidate_repeats bounds tolerances capacitance_search invalid_measurement_recovery");
    const auto norm=Text(search["normalization"]);Require(norm=="N0" || norm=="N1");c.normalization=norm=="N0"?Normalization::SynapseCount:Normalization::TipResistance;
    c.maximumProbes=Count(search["maximum_probes"]);c.maximumEdits=Count(search["maximum_edits"]);c.maximumCandidatesPerInput=Count(search["maximum_candidates_per_input"]);
    c.maximumSweeps=Count(search["maximum_sweeps"]);c.maximumInvalidObservations=Count(search["maximum_invalid_observations"]);
    const auto depth=Count(search["resistance_subdivision_depth"]);Require(depth<=20);c.resistanceSubdivisionDepth=unsigned(depth);c.n0CandidateRepeats=Count(search["n0_candidate_repeats"]);
    for(const auto& v:Array(search["bounds"])) {const auto b=Object(v);Keys(b,"minimum_length maximum_length minimum_synapses maximum_synapses minimum_resistance maximum_resistance");
        const auto integer=[](const QJsonValue& value){const auto n=Count(value);Require(n<=std::numeric_limits<int>::max());return int(n);};
        c.bounds.push_back({integer(b["minimum_length"]),integer(b["maximum_length"]),integer(b["minimum_synapses"]),integer(b["maximum_synapses"]),Number(b["minimum_resistance"]),Number(b["maximum_resistance"])});
    }
    const auto tolerances=Object(search["tolerances"]);Keys(tolerances,"amplitude_absolute amplitude_relative sync");
    c.tolerances={Number(tolerances["amplitude_absolute"]),Number(tolerances["amplitude_relative"]),Number(tolerances["sync"])};
    if(!search["capacitance_search"].isNull())c.capacitanceSearch=DecodeCapacitanceSearchPolicy(Object(search["capacitance_search"]));
    Require(Text(search["invalid_measurement_recovery"])=="retry_same_candidate");
    p.maximumReferenceProbes=Count(o["maximum_reference_probes"]);p.maximumInvalidReferenceProbes=Count(o["maximum_invalid_reference_probes"]);
    p.maximumModelSeconds=Number(o["maximum_model_seconds"]);p.maximumWallSeconds=Number(o["maximum_wall_seconds"]);p.recordCapacitanceHistory=o["record_capacitance_history"].toBool();
    Require(EncodeCompositeRuntimePlan(p)==o);return p;
}

}
