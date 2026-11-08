#include "CompositeTrainingDeclarations.h"
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
}
