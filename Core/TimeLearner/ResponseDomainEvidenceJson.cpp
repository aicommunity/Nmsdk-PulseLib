#include "ResponseDomainEvidenceJson.h"
#include "ProbeEvidenceJson.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QStringList>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool condition,const char* reason){if(!condition)throw std::invalid_argument(reason);}
QString Text(const std::string& value){return QString::fromStdString(value);}
double Number(double value){Require(std::isfinite(value),"nonfinite_response_domain_json");return value;}
double Count(std::size_t value){Require(value<=9007199254740991ULL,"response_domain_count_not_exact");return double(value);}
QJsonValue Index(const std::optional<std::size_t>& value){return value?QJsonValue(Count(*value)):QJsonValue(QJsonValue::Null);}
QJsonValue Optional(const std::optional<double>& value){return value?QJsonValue(Number(*value)):QJsonValue(QJsonValue::Null);}
QJsonArray Strings(const std::vector<std::string>& values){QJsonArray a;for(const auto& v:values)a.append(Text(v));return a;}
QJsonArray Indices(const std::vector<std::size_t>& values){QJsonArray a;for(auto v:values)a.append(Count(v));return a;}
QJsonObject Maximum(const ResponseMaximum& m){return {{"first_sample",Count(m.firstSample)},{"last_sample",Count(m.lastSample)},
    {"amplitude",Number(m.amplitude)},{"raw",Number(m.rawPotential)},{"time",Number(m.time)},{"plateau_end",Number(m.plateauEnd)}};}
const char* FeatureStatus(ResponseFeatureValidity v){switch(v){
    case ResponseFeatureValidity::Valid:return "valid";
    case ResponseFeatureValidity::InsufficientSamples:return "insufficient_samples";
    case ResponseFeatureValidity::InvalidSpecification:return "invalid_specification";
    case ResponseFeatureValidity::NonfiniteSample:return "nonfinite_sample";
    case ResponseFeatureValidity::NonmonotonicTime:return "nonmonotonic_time";
    }throw std::invalid_argument("unknown_response_feature_validity");}
}
QByteArray EncodeNativeResponseDomainTrace(const NativeResponseDomainCapture& c)
{
    c.ClosedNativeEvidence();const auto& o=c.Observation();
    Require(!o.channels.empty() && o.channels.size()==c.PhysicalChannelPaths().size(),"response_domain_trace_channels_invalid");
    const auto samples=c.StimulusSamples().size();std::ostringstream stream;stream.imbue(std::locale::classic());
    stream<<std::setprecision(17)<<"time,stimulus";
    for(const auto& channel:o.channels){Require(channel.samples.size()==samples,"response_domain_trace_shape_invalid");stream<<",soma"<<channel.channel;}
    stream<<'\n';
    for(std::size_t i=0;i<samples;++i){
        stream<<Number(o.channels.front().samples[i].time)<<','<<Number(c.StimulusSamples()[i]);
        for(const auto& channel:o.channels){
            Require(channel.samples[i].time==o.channels.front().samples[i].time,"response_domain_channel_clock_changed");
            stream<<','<<Number(channel.samples[i].rawPotential);
        }
        stream<<'\n';
    }
    Require(bool(stream),"response_domain_trace_encoding_failed");return QByteArray::fromStdString(stream.str());
}
QJsonObject EncodeNativeResponseDomainEvidence(const NativeResponseDomainCapture& c,const ResponseDomainSpec& spec,
    const QString& relativeTracePath,const QByteArray& trace)
{
    Require(!relativeTracePath.isEmpty() && !relativeTracePath.startsWith('/') && !relativeTracePath.contains('\\') &&
            !relativeTracePath.contains(':') && !relativeTracePath.split('/').contains("..") &&
            !relativeTracePath.split('/').contains(".") && !relativeTracePath.split('/').contains(""),"unsafe_response_domain_trace_path");
    Require(trace==EncodeNativeResponseDomainTrace(c),"response_domain_trace_does_not_match_capture");
    const auto& o=c.Observation();const auto result=AnalyzeDeclaredResponseDomains(spec,o);
    QJsonArray domains,mapping,channels,associations,header;header.append("time");header.append("stimulus");
    for(const auto& d:spec.domains)domains.append(QJsonObject{{"channel",Count(d.channel)},{"stimulus_id",Text(d.stimulusId)},
        {"begin",Number(d.begin)},{"end",Number(d.end)}});
    for(std::size_t i=0;i<o.channels.size();++i){const auto& ch=o.channels[i];const auto column="soma"+QString::number(qulonglong(ch.channel));
        header.append(column);mapping.append(QJsonObject{{"channel",Count(ch.channel)},{"physical_path",Text(c.PhysicalChannelPaths()[i])},
            {"csv_column",column},{"baseline",Number(ch.baseline)}});}
    for(const auto& ch:result.channels){const auto& f=ch.features;QJsonArray maxima;
        for(const auto& m:f.positiveMaxima)maxima.append(Maximum(m));
        channels.append(QJsonObject{{"channel",Count(ch.channel)},{"first_source_sample",Count(ch.firstSourceSample)},
            {"feature_validity",FeatureStatus(f.validity)},{"positive_maxima",maxima},
            {"global_maximum",f.globalMaximum?QJsonValue(Maximum(*f.globalMaximum)):QJsonValue(QJsonValue::Null)},
            {"largest_interior_maximum_count",Count(f.largestInteriorMaximumCount)},
            {"global_maximum_clipped",f.globalMaximumClipped},{"boundary_response_clipped",f.boundaryResponseClipped}});}
    for(const auto& a:result.associations)associations.append(QJsonObject{{"channel",Count(a.channel)},{"stimulus_id",Text(a.stimulusId)},
        {"status",DomainAssociationStatusName(a.status)},{"reason",Text(a.reason)},{"candidate_maxima",Indices(a.candidateMaxima)},
        {"largest_measured_maximum",Index(a.largestMeasuredMaximum)},{"selected_maximum",Index(a.selectedMaximum)},
        {"actual_stimulus_time",Optional(a.actualStimulusTime)},{"maximum_offset_low",Optional(a.maximumOffsetLow)},
        {"maximum_offset_high",Optional(a.maximumOffsetHigh)},{"competing_stimulus_ids",Strings(a.competingStimulusIds)}});
    return {{"kind","timelearner_native_response_domain_evidence"},{"schema_version",1},{"method",ResponseDomainResult::MethodId()},
        {"diagnostic_only",true},{"training_certificate_eligible",false},{"scientific_verdict",QJsonValue(QJsonValue::Null)},
        {"native_evidence",EncodeNativeProbeEvidence(c.ClosedNativeEvidence())},
        {"domain_spec",QJsonObject{{"schema_version",Count(spec.schemaVersion)},{"model_revision",Text(spec.modelRevision)},
            {"protocol_revision",Text(spec.protocolRevision)},{"pattern_hash",Text(spec.patternHash)},
            {"stimulus_ids",Strings(spec.stimulusIds)},{"channel_ids",Indices(spec.channelIds)},{"domains",domains}}},
        {"capture",QJsonObject{{"run_id",Text(o.runId)},{"trial_id",Text(o.trialId)},{"sample_id",Text(o.sampleId)},
            {"complete",o.complete},{"sample_count",Count(c.StimulusSamples().size())},{"begin",Number(o.begin)},{"end",Number(o.end)},
            {"maximum_sample_gap",Number(o.maximumSampleGap)},{"minimum_amplitude",Number(o.minimumAmplitude)},{"channel_mapping",mapping}}},
        {"trace",QJsonObject{{"format","native-probe-csv-v1"},{"path",relativeTracePath},{"bytes",Count(std::size_t(trace.size()))},
            {"sha256",QString::fromLatin1(QCryptographicHash::hash(trace,QCryptographicHash::Sha256).toHex())},
            {"header",header},{"time_frame","absolute_model_time"}}},
        {"analysis",QJsonObject{{"channels",channels},{"associations",associations},{"reasons",Strings(result.reasons)}}}};
}
}
