#include "CandidateWindowTrainingDeclarations.h"
#include "CompositeTrainingDeclarations.h"
#include "FinalVerification.h"
#include <QJsonArray>
#include <QStringList>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason="candidate_window_declaration_field") {
    if(!value)throw std::invalid_argument(reason);
}
void Keys(const QJsonObject& o,const char* fields) {
    auto keys=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);keys.sort();Require(o.keys()==keys);
}
QJsonObject Object(const QJsonValue& v){Require(v.isObject());return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray());return v.toArray();}
QString Text(const std::string& v){return QString::fromStdString(v);}
std::string Text(const QJsonValue& v){Require(v.isString());return v.toString().toStdString();}
double Number(const QJsonValue& v){Require(v.isDouble() && std::isfinite(v.toDouble()));return v.toDouble();}
std::uint64_t Count(const QJsonValue& v) {
    const double n=Number(v);Require(n>=0. && n<=double(CandidateWindowMaximumExactCount) && std::floor(n)==n);return std::uint64_t(n);
}
double Count(std::uint64_t n){Require(n<=CandidateWindowMaximumExactCount);return double(n);}
std::size_t Index(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<std::size_t>::max());return std::size_t(n);}
unsigned Version(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<unsigned>::max());return unsigned(n);}
QJsonObject Window(const CompositeInputWindow& w) {
    Require(std::isfinite(w.beginOffset) && std::isfinite(w.endOffset) && w.beginOffset>=0. && w.endOffset>w.beginOffset);
    return {{"begin_offset",w.beginOffset},{"end_offset",w.endOffset}};
}
CompositeInputWindow Window(const QJsonValue& value) {
    const auto w=Object(value);Keys(w,"begin_offset end_offset");CompositeInputWindow result{Number(w["begin_offset"]),Number(w["end_offset"])};
    Require(Window(result)==w);return result;
}
void PolicyTags(const CandidateWindowPolicy& p) {
    Require(p.schemaVersion==1 && p.policyId=="reference-anchored-candidate-window-v1" &&
        p.seedRule=="immutable_reference_full_before" && p.coordinateMode=="relative_native_frame_interval" &&
        p.proposalsPerLogicalRequest==1,"candidate_window_unknown_policy_or_strategy");
}
}
QJsonObject EncodeCandidateWindowPolicy(const CandidateWindowPolicy& p) {
    PolicyTags(p);Require(!p.inputs.empty() && p.maximumFeatureTerms>0 && p.maximumFeatureTerms<=CandidateWindowMaximumExactCount &&
        p.inputs.size()<=p.maximumFeatureTerms/3,"candidate_window_invalid_feature_budget");
    QJsonArray inputs;
    for(const auto& i:p.inputs)inputs.append(QJsonObject{
        {"delay_frames_per_segment",Count(i.delayFramesPerSegment)},
        {"base_left_guard_frames",Count(i.baseLeftGuardFrames)},{"base_right_guard_frames",Count(i.baseRightGuardFrames)},
        {"length_guard_frames",Count(i.lengthGuardFrames)},{"synapse_count_guard_frames",Count(i.synapseCountGuardFrames)},
        {"resistance_change_guard_frames",Count(i.resistanceChangeGuardFrames)},{"capacitance_change_guard_frames",Count(i.capacitanceChangeGuardFrames)},
        {"hard_first_frame",Count(i.hardFirstFrame)},{"hard_last_frame",Count(i.hardLastFrame)},{"maximum_window_frames",Count(i.maximumWindowFrames)}});
    return {{"schema_version",Count(p.schemaVersion)},{"policy_id",Text(p.policyId)},{"seed_rule",Text(p.seedRule)},
        {"coordinate_mode",Text(p.coordinateMode)},{"proposals_per_logical_request",Count(p.proposalsPerLogicalRequest)},
        {"maximum_feature_terms",Count(p.maximumFeatureTerms)},{"inputs",inputs}};
}
CandidateWindowPolicy DecodeCandidateWindowPolicy(const QJsonObject& o) {
    Keys(o,"schema_version policy_id seed_rule coordinate_mode proposals_per_logical_request maximum_feature_terms inputs");
    CandidateWindowPolicy p;p.schemaVersion=Version(o["schema_version"]);p.policyId=Text(o["policy_id"]);p.seedRule=Text(o["seed_rule"]);
    p.coordinateMode=Text(o["coordinate_mode"]);p.proposalsPerLogicalRequest=Count(o["proposals_per_logical_request"]);
    p.maximumFeatureTerms=Count(o["maximum_feature_terms"]);const auto inputs=Array(o["inputs"]);
    Require(p.maximumFeatureTerms>0 && std::uint64_t(inputs.size())<=p.maximumFeatureTerms/3,"candidate_window_invalid_feature_budget");
    for(const auto& value:inputs) {
        const auto i=Object(value);Keys(i,"delay_frames_per_segment base_left_guard_frames base_right_guard_frames length_guard_frames synapse_count_guard_frames resistance_change_guard_frames capacitance_change_guard_frames hard_first_frame hard_last_frame maximum_window_frames");
        p.inputs.push_back({Count(i["delay_frames_per_segment"]),Count(i["base_left_guard_frames"]),Count(i["base_right_guard_frames"]),
            Count(i["length_guard_frames"]),Count(i["synapse_count_guard_frames"]),Count(i["resistance_change_guard_frames"]),
            Count(i["capacitance_change_guard_frames"]),Count(i["hard_first_frame"]),Count(i["hard_last_frame"]),Count(i["maximum_window_frames"])});
    }
    Require(EncodeCandidateWindowPolicy(p)==o);return p;
}
QJsonObject EncodeCandidateWindowMeasurementContract(const CandidateWindowMeasurementContract& c) {
    Require(c.schemaVersion==2 && c.methodId==DeclaredWindowBasinResult::MethodId(),"candidate_window_measurement_contract_version");
    ValidateCandidateWindowPolicyDeclaration(c.candidateWindowPolicy,c.referenceInputWindows.size(),c.contextPolicy.maximumSamples);
    QJsonArray windows;for(const auto& w:c.referenceInputWindows)windows.append(Window(w));
    return {{"schema_version",Count(c.schemaVersion)},{"method_id",Text(c.methodId)},
        {"context_policy",EncodeCompositeContextPolicy(c.contextPolicy)},{"geometry_policy",EncodeCompositeGeometryPolicy(c.geometryPolicy)},
        {"reference_input_windows",windows},{"candidate_window_policy",EncodeCandidateWindowPolicy(c.candidateWindowPolicy)}};
}
CandidateWindowMeasurementContract DecodeCandidateWindowMeasurementContract(const QJsonObject& o) {
    Keys(o,"schema_version method_id context_policy geometry_policy reference_input_windows candidate_window_policy");
    CandidateWindowMeasurementContract c;c.schemaVersion=Version(o["schema_version"]);c.methodId=Text(o["method_id"]);
    c.contextPolicy=DecodeCompositeContextPolicy(Object(o["context_policy"]));c.geometryPolicy=DecodeCompositeGeometryPolicy(Object(o["geometry_policy"]));
    c.candidateWindowPolicy=DecodeCandidateWindowPolicy(Object(o["candidate_window_policy"]));
    const auto windows=Array(o["reference_input_windows"]);Require(std::size_t(windows.size())==c.candidateWindowPolicy.inputs.size());
    for(const auto& w:windows)c.referenceInputWindows.push_back(Window(w));
    Require(EncodeCandidateWindowMeasurementContract(c)==o);return c;
}
QJsonObject EncodeCandidateWindowBasinSpecification(const CandidateWindowBasinSpecification& s) {
    Require(s.schemaVersion==2 && s.methodId==DeclaredWindowBasinResult::MethodId() && !s.groupId.empty() && !s.inputId.empty());
    QJsonValue reference(QJsonValue::Null),candidate(QJsonValue::Null);QString mode;
    if(s.coordinateMode==CandidateWindowCoordinateMode::ReferenceTime) {
        Require(s.referenceTime && !s.candidateFrame);reference=Window(*s.referenceTime);mode="reference_time";
    } else {
        Require(s.coordinateMode==CandidateWindowCoordinateMode::CandidateFrame && s.candidateFrame && !s.referenceTime &&
            s.candidateFrame->firstFrame<s.candidateFrame->lastFrame);
        candidate=QJsonObject{{"first_frame",Count(s.candidateFrame->firstFrame)},{"last_frame",Count(s.candidateFrame->lastFrame)}};mode="candidate_frame";
    }
    return {{"schema_version",Count(s.schemaVersion)},{"method_id",Text(s.methodId)},{"group_id",Text(s.groupId)},{"input_id",Text(s.inputId)},
        {"assigned_input",Count(s.assignedInput)},{"channel",Count(s.channel)},{"coordinate_mode",mode},{"reference_time",reference},{"candidate_frame",candidate}};
}
CandidateWindowBasinSpecification DecodeCandidateWindowBasinSpecification(const QJsonObject& o) {
    Keys(o,"schema_version method_id group_id input_id assigned_input channel coordinate_mode reference_time candidate_frame");
    CandidateWindowBasinSpecification s;s.schemaVersion=Version(o["schema_version"]);s.methodId=Text(o["method_id"]);
    s.groupId=Text(o["group_id"]);s.inputId=Text(o["input_id"]);s.assignedInput=Index(o["assigned_input"]);s.channel=Index(o["channel"]);
    const auto mode=Text(o["coordinate_mode"]);
    if(mode=="reference_time") {s.coordinateMode=CandidateWindowCoordinateMode::ReferenceTime;s.referenceTime=Window(o["reference_time"]);Require(o["candidate_frame"].isNull());}
    else {Require(mode=="candidate_frame" && o["reference_time"].isNull());s.coordinateMode=CandidateWindowCoordinateMode::CandidateFrame;
        const auto c=Object(o["candidate_frame"]);Keys(c,"first_frame last_frame");s.candidateFrame=CandidateFrameWindow{Count(c["first_frame"]),Count(c["last_frame"])};}
    Require(EncodeCandidateWindowBasinSpecification(s)==o);return s;
}
}
