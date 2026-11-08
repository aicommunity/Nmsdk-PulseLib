#include "MatchedOmissionEvidenceJson.h"
#include "ConfiguredNativeTraining.h"
#include "ModelIdentity.h"
#include "ProbeEvidenceJson.h"
#include "ResponseDomainEvidenceJson.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QStringList>
#include <algorithm>
#include <cmath>
#include <limits>
#include <locale>
#include <set>
#include <sstream>

namespace NMSDK::TimeLearner {
namespace {
constexpr const char* Encoding="length-framed-utf8-binary64-v1";
constexpr const char* TopFields="schema_version kind method complete_group diagnostic_only training_certificate_eligible scientific_verdict specification policy source time_budget traces assignment attestation";
constexpr const char* SpecFields="schema_version run_id group_id sample_id protocol_revision pattern_hash input_id base_model_revision effective_model_revision reset_binding model_instance_binding model_epoch assigned_input channel routing stimulus_ids physical_channel_paths stimulus_profile duration minimum_amplitude";
constexpr const char* PolicyFields="schema_version comparison_radius_ticks maximum_samples maximum_total_samples maximum_native_probes time_comparison_tolerance baseline_absolute repeatability_absolute repeatability_relative influence_absolute influence_relative";
constexpr const char* TraceFields="role omitted_input presented_ordinals planned_presented_isi applied_presented_isi presented_pattern_sha256 relative_path bytes sha256 native_evidence binding";
constexpr const char* BindingFields="group_id source_pattern_hash reset_binding model_instance_binding current_base_revision native_completed full_mask_restored physical_channel_paths";
void Require(bool condition,const char* reason)
{if(!condition)throw std::invalid_argument(reason);}
void Keys(const QJsonObject& object,const char* fields)
{
    auto expected=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);expected.sort();
    Require(object.keys()==expected,"matched_codec_unknown_or_missing_fields");
}
QJsonObject Object(const QJsonValue& value)
{Require(value.isObject(),"matched_codec_expected_object");return value.toObject();}
QJsonArray Array(const QJsonValue& value)
{Require(value.isArray(),"matched_codec_expected_array");return value.toArray();}
QString Text(const std::string& value){return QString::fromStdString(value);}
std::string Text(const QJsonValue& value)
{Require(value.isString(),"matched_codec_expected_string");return value.toString().toStdString();}
double Number(double value)
{Require(std::isfinite(value),"matched_codec_nonfinite_number");return value;}
double Number(const QJsonValue& value)
{Require(value.isDouble(),"matched_codec_expected_number");return Number(value.toDouble());}
double Count(std::size_t value)
{Require(value<=9007199254740991ULL,"matched_codec_count_not_exact");return double(value);}
std::size_t Count(const QJsonValue& value)
{
    const auto number=Number(value);
    Require(number>=0. && number<=9007199254740991. && number<=double(std::numeric_limits<std::size_t>::max()) &&
        std::floor(number)==number,"matched_codec_invalid_count");return std::size_t(number);
}
unsigned Version(const QJsonValue& value)
{const auto number=Count(value);Require(number<=std::numeric_limits<unsigned>::max(),"matched_codec_invalid_version");return unsigned(number);}
bool Boolean(const QJsonValue& value)
{Require(value.isBool(),"matched_codec_expected_boolean");return value.toBool();}
QJsonValue OptionalIndex(const std::optional<std::size_t>& index)
{return index?QJsonValue(Count(*index)):QJsonValue(QJsonValue::Null);}
QJsonArray Strings(const std::vector<std::string>& values)
{QJsonArray result;for(const auto& value:values)result.append(Text(value));return result;}
std::vector<std::string> Strings(const QJsonValue& value)
{std::vector<std::string> result;for(const auto& item:Array(value))result.push_back(Text(item));return result;}
QJsonArray Numbers(const std::vector<double>& values)
{QJsonArray result;for(const auto value:values)result.append(Number(value));return result;}
std::vector<double> Numbers(const QJsonValue& value)
{std::vector<double> result;for(const auto& item:Array(value))result.push_back(Number(item));return result;}
QJsonArray Indices(const std::vector<std::size_t>& values)
{QJsonArray result;for(const auto value:values)result.append(Count(value));return result;}
std::vector<std::size_t> Indices(const QJsonValue& value)
{std::vector<std::size_t> result;for(const auto& item:Array(value))result.push_back(Count(item));return result;}
std::vector<std::size_t> FullMask(std::size_t count)
{std::vector<std::size_t> result;for(std::size_t i=0;i<count;++i)result.push_back(i);return result;}
QString Hash(const std::vector<double>& values)
{return NmsdkTools::CanonicalModelDataSha256(Numbers(values));}
QString BytesHash(const QByteArray& bytes)
{return QString::fromLatin1(QCryptographicHash::hash(bytes,QCryptographicHash::Sha256).toHex());}
void SafePath(const QString& path)
{
    Require(!path.isEmpty() && !path.startsWith('/') && !path.contains('\\') && !path.contains(':'),
        "matched_codec_unsafe_trace_path");
    for(const auto& part:path.split('/'))Require(!part.isEmpty() && part!="." && part!="..","matched_codec_unsafe_trace_path");
}
QJsonObject Profile(const StimulusProfile& p)
{return {{"generator_class",Text(p.generatorClass)},{"amplitude",Number(p.amplitude)},{"pulse_length",Number(p.pulseLength)},
    {"frequency_hz",Number(p.frequencyHz)},{"frequency_deviation",Number(p.frequencyDeviation)},
    {"use_transit_signal",p.useTransitSignal},{"use_pattern_output",p.usePatternOutput}};}
StimulusProfile Profile(const QJsonValue& value)
{
    const auto p=Object(value);Keys(p,"generator_class amplitude pulse_length frequency_hz frequency_deviation use_transit_signal use_pattern_output");
    return {Text(p["generator_class"]),Number(p["amplitude"]),Number(p["pulse_length"]),Number(p["frequency_hz"]),
        Number(p["frequency_deviation"]),Boolean(p["use_transit_signal"]),Boolean(p["use_pattern_output"])};
}
QString Routing(ProbeRouting routing)
{
    Require(routing==ProbeRouting::ClassicBurst || routing==ProbeRouting::BranchSoloBurst,"matched_codec_unknown_routing");
    return routing==ProbeRouting::ClassicBurst?"classic_burst":"branch_solo_burst";
}
ProbeRouting Routing(const QJsonValue& value)
{
    const auto text=Text(value);Require(text=="classic_burst" || text=="branch_solo_burst","matched_codec_unknown_routing");
    return text=="classic_burst"?ProbeRouting::ClassicBurst:ProbeRouting::BranchSoloBurst;
}
QJsonObject Specification(const MatchedOmissionSpecification& s)
{return {{"schema_version",int(s.schemaVersion)},{"run_id",Text(s.runId)},{"group_id",Text(s.groupId)},
    {"sample_id",Text(s.sampleId)},{"protocol_revision",Text(s.protocolRevision)},{"pattern_hash",Text(s.patternHash)},
    {"input_id",Text(s.inputId)},{"base_model_revision",Text(s.baseModelRevision)},
    {"effective_model_revision",Text(s.effectiveModelRevision)},{"reset_binding",Text(s.resetBinding)},
    {"model_instance_binding",Text(s.modelInstanceBinding)},{"model_epoch",Count(s.modelEpoch)},
    {"assigned_input",Count(s.assignedInput)},{"channel",Count(s.channel)},{"routing",Routing(s.routing)},
    {"stimulus_ids",Strings(s.stimulusIds)},{"physical_channel_paths",Strings(s.physicalChannelPaths)},
    {"stimulus_profile",Profile(s.stimulusProfile)},{"duration",Number(s.duration)},{"minimum_amplitude",Number(s.minimumAmplitude)}};}
MatchedOmissionSpecification Specification(const QJsonValue& value)
{
    const auto o=Object(value);Keys(o,SpecFields);MatchedOmissionSpecification s;s.schemaVersion=Version(o["schema_version"]);
    s.runId=Text(o["run_id"]);s.groupId=Text(o["group_id"]);s.sampleId=Text(o["sample_id"]);
    s.protocolRevision=Text(o["protocol_revision"]);s.patternHash=Text(o["pattern_hash"]);s.inputId=Text(o["input_id"]);
    s.baseModelRevision=Text(o["base_model_revision"]);s.effectiveModelRevision=Text(o["effective_model_revision"]);
    s.resetBinding=Text(o["reset_binding"]);s.modelInstanceBinding=Text(o["model_instance_binding"]);
    s.modelEpoch=Count(o["model_epoch"]);s.assignedInput=Count(o["assigned_input"]);s.channel=Count(o["channel"]);
    s.routing=Routing(o["routing"]);s.stimulusIds=Strings(o["stimulus_ids"]);s.physicalChannelPaths=Strings(o["physical_channel_paths"]);
    s.stimulusProfile=Profile(o["stimulus_profile"]);s.duration=Number(o["duration"]);s.minimumAmplitude=Number(o["minimum_amplitude"]);return s;
}
QJsonObject Policy(const MatchedOmissionPolicy& p)
{return {{"schema_version",int(p.schemaVersion)},{"comparison_radius_ticks",Count(p.comparisonRadiusTicks)},
    {"maximum_samples",Count(p.maximumSamples)},{"maximum_total_samples",Count(p.maximumTotalSamples)},
    {"maximum_native_probes",Count(p.maximumNativeProbes)},{"time_comparison_tolerance",Number(p.timeComparisonTolerance)},
    {"baseline_absolute",Number(p.baselineAbsolute)},{"repeatability_absolute",Number(p.repeatabilityAbsolute)},
    {"repeatability_relative",Number(p.repeatabilityRelative)},{"influence_absolute",Number(p.influenceAbsolute)},
    {"influence_relative",Number(p.influenceRelative)}};}
MatchedOmissionPolicy Policy(const QJsonValue& value)
{
    const auto o=Object(value);Keys(o,PolicyFields);MatchedOmissionPolicy p;p.schemaVersion=Version(o["schema_version"]);
    p.comparisonRadiusTicks=Count(o["comparison_radius_ticks"]);p.maximumSamples=Count(o["maximum_samples"]);
    p.maximumTotalSamples=Count(o["maximum_total_samples"]);p.maximumNativeProbes=Count(o["maximum_native_probes"]);
    p.timeComparisonTolerance=Number(o["time_comparison_tolerance"]);p.baselineAbsolute=Number(o["baseline_absolute"]);
    p.repeatabilityAbsolute=Number(o["repeatability_absolute"]);p.repeatabilityRelative=Number(o["repeatability_relative"]);
    p.influenceAbsolute=Number(o["influence_absolute"]);p.influenceRelative=Number(o["influence_relative"]);return p;
}
QJsonObject Maximum(const ResponseMaximum& m)
{return {{"first_sample",Count(m.firstSample)},{"last_sample",Count(m.lastSample)},{"amplitude",Number(m.amplitude)},
    {"raw_potential",Number(m.rawPotential)},{"time",Number(m.time)},{"plateau_end",Number(m.plateauEnd)}};}
QJsonObject Assignment(const MatchedOmissionResult& result)
{
    QJsonArray maxima;
    for(const auto& m:result.maxima)
    {
        auto object=Maximum(m.maximum);QJsonArray influence;
        for(const auto state:m.influenceByInput){const auto text=OmissionInfluenceName(state);
            Require(std::string(text)!="unknown" || state==OmissionInfluence::Unknown,"matched_codec_unknown_influence");influence.append(text);}
        object["comparison_first_sample"]=Count(m.comparisonFirstSample);object["comparison_last_sample"]=Count(m.comparisonLastSample);
        object["supported_inputs"]=Indices(m.supportedInputs);object["influence_by_input"]=influence;
        object["unknown"]=m.unknown;object["precedes_assigned_stimulus"]=m.precedesAssignedStimulus;object["clipped"]=m.clipped;
        maxima.append(object);
    }
    return {{"validity",MatchedOmissionValidityName(result.validity)},{"reasons",Strings(result.reasons)},
        {"maxima",maxima},{"selected_full_maximum",OptionalIndex(result.selectedFullMaximum)},
        {"measured_maximum",result.measuredMaximum?QJsonValue(Maximum(*result.measuredMaximum)):QJsonValue(QJsonValue::Null)}};
}
bool AllowedOutcome(MatchedOmissionValidity validity)
{return validity==MatchedOmissionValidity::Valid || validity==MatchedOmissionValidity::Missing ||
    validity==MatchedOmissionValidity::Ambiguous || validity==MatchedOmissionValidity::Clipped || validity==MatchedOmissionValidity::NonRepeatable;}
QJsonObject Attestation()
{return {{"native_execution_verified",false},{"physical_paths_independently_verified",false},
    {"reset_instance_ownership_independently_verified",false}};}
QJsonObject TimeBudget(const NativeMatchedOmissionResult& result)
{return {{"maximum_model_seconds",Number(result.maximumModelSeconds)},{"maximum_wall_seconds",Number(result.maximumWallSeconds)},
    {"elapsed_model_seconds",QJsonValue(QJsonValue::Null)},{"elapsed_wall_seconds",QJsonValue(QJsonValue::Null)},{"enforcement_verified",false}};}
QJsonObject Binding(const MatchedOmissionTrace& trace)
{return {{"group_id",Text(trace.groupId)},{"source_pattern_hash",Text(trace.sourcePatternHash)},
    {"reset_binding",Text(trace.resetBinding)},{"model_instance_binding",Text(trace.modelInstanceBinding)},
    {"current_base_revision",Text(trace.currentBaseRevision)},{"native_completed",trace.nativeCompleted},
    {"full_mask_restored",trace.fullMaskRestored},{"physical_channel_paths",Strings(trace.physicalChannelPaths)}};}
void SetBinding(MatchedOmissionTrace& trace,const QJsonValue& value)
{
    const auto o=Object(value);Keys(o,BindingFields);trace.groupId=Text(o["group_id"]);trace.sourcePatternHash=Text(o["source_pattern_hash"]);
    trace.resetBinding=Text(o["reset_binding"]);trace.modelInstanceBinding=Text(o["model_instance_binding"]);
    trace.currentBaseRevision=Text(o["current_base_revision"]);trace.nativeCompleted=Boolean(o["native_completed"]);
    trace.fullMaskRestored=Boolean(o["full_mask_restored"]);trace.physicalChannelPaths=Strings(o["physical_channel_paths"]);
}
bool SamePrepared(const PreparedVerificationProbe& a,const PreparedVerificationProbe& b)
{return a.routing==b.routing && a.inputId==b.inputId && a.protocolRevision==b.protocolRevision &&
    a.baseModelRevision==b.baseModelRevision && a.effectiveModelRevision==b.effectiveModelRevision &&
    a.modelEpoch==b.modelEpoch && a.selectedInputIndex==b.selectedInputIndex;}
bool SameObservation(const ResponseDomainObservation& a,const ResponseDomainObservation& b)
{
    if(a.runId!=b.runId || a.trialId!=b.trialId || a.sampleId!=b.sampleId || a.patternHash!=b.patternHash ||
       a.modelRevision!=b.modelRevision || a.protocolRevision!=b.protocolRevision || a.begin!=b.begin || a.end!=b.end ||
       a.maximumSampleGap!=b.maximumSampleGap || a.minimumAmplitude!=b.minimumAmplitude || a.complete!=b.complete ||
       a.events.size()!=b.events.size() || a.channels.size()!=b.channels.size())return false;
    for(std::size_t i=0;i<a.events.size();++i)if(a.events[i].id!=b.events[i].id || a.events[i].time!=b.events[i].time)return false;
    for(std::size_t channel=0;channel<a.channels.size();++channel)
    {
        const auto& x=a.channels[channel];const auto& y=b.channels[channel];
        if(x.channel!=y.channel || x.baseline!=y.baseline || x.samples.size()!=y.samples.size())return false;
        for(std::size_t i=0;i<x.samples.size();++i)if(x.samples[i].time!=y.samples[i].time || x.samples[i].rawPotential!=y.samples[i].rawPotential)return false;
    }
    return true;
}
void RequireClosedNative(const NativeProbeResult& native)
{
    Require(native.status==NativeProbeStatus::Completed && native.reasons.empty() && !native.requiresFreshVerification &&
        native.requestedSpecification && native.prepared && native.observation,"matched_codec_native_not_completed");
    const auto& actual=native.observation->specification;auto expected=*native.requestedSpecification;
    Require(actual.schemaVersion==6 && expected.schemaVersion==6 && native.observation->schemaVersion==6 &&
        native.observation->valid && native.observation->complete && native.observation->reasons.empty() &&
        expected.begin==0. && expected.baseline.empty() && expected.modelRevision.empty() &&
        !expected.recordSpikes && expected.windows.empty() && !expected.characterization && expected.stimulusSubset &&
        expected.waveAssignment==WaveAssignmentPolicy::WindowOnly && expected.peakSelection==PeakSelectionPolicy::IsolatedWindow,
        "matched_codec_invalid_native_observation");
    expected.end=Number(actual.begin+expected.end);expected.begin=actual.begin;
    expected.baseline=actual.baseline;expected.modelRevision=actual.modelRevision;
    Require(EncodeNativeProbeSpecification(expected)==EncodeNativeProbeSpecification(actual),"matched_codec_requested_specification_changed");
    // Typed encoder input must obey the same closed packet schema as import.
    (void)DecodeNativeProbeEvidence(EncodeNativeProbeEvidence(native));
}
NativeResponseDomainCapture ReplayTrace(const MatchedOmissionTrace& trace,const NativeProbeResult& native)
{
    RequireClosedNative(native);
    Require(trace.nativeCompleted && trace.fullMaskRestored && SamePrepared(trace.prepared,*native.prepared),
        "matched_codec_unverified_trace_binding");
    NativeResponseDomainCapture capture;capture.Arm(*native.prepared,native.observation->specification,trace.physicalChannelPaths);
    const auto& o=trace.observation;Require(!o.channels.empty(),"matched_codec_missing_channels");
    const auto count=trace.stimulusSamples.size();std::size_t event=0;
    for(const auto& channel:o.channels)Require(channel.samples.size()==count,"matched_codec_trace_shape_changed");
    for(std::size_t i=0;i<count;++i)
    {
        NativeProbeSample sample;sample.time=o.channels.front().samples[i].time;sample.stimulus=trace.stimulusSamples[i];
        sample.effectiveModelRevision=native.prepared->effectiveModelRevision;
        for(const auto& channel:o.channels){Require(channel.samples[i].time==sample.time,"matched_codec_channel_clock_changed");sample.potentials.push_back(channel.samples[i].rawPotential);}
        while(event<o.events.size() && !Detail::After(o.events[event].time,sample.time))
        {sample.newStimuli.push_back({o.events[event].id,o.events[event].time});++event;}
        capture.Append(sample);
    }
    Require(event==o.events.size(),"matched_codec_unobserved_event");capture.CloseNative(native);
    Require(SameObservation(o,capture.Observation()),"matched_codec_trace_native_metadata_changed");return capture;
}
}

QByteArray EncodeMatchedOmissionPrimaryTrace(const MatchedOmissionTrace& trace,const NativeProbeResult& native)
{const auto capture=ReplayTrace(trace,native);return EncodeNativeResponseDomainTrace(capture);}

// The composite serializer and decoder below share the exact same native
// replay/selector path. No imported assignment or ownership attestation is used
// as an input to a measurement decision.
namespace {
struct CheckedComposite {
    std::vector<const MatchedOmissionTrace*> traces;
    MatchedOmissionResult assignment;
};
CheckedComposite CheckComposite(const NativeMatchedOmissionResult& result)
{
    const auto& s=result.specification;const auto count=result.sourceIsi.size();
    Require(count>=2 && count<=std::numeric_limits<std::size_t>::max()-2 && s.stimulusIds.size()==count &&
        result.packets.size()==count+2 && result.appliedPresentedIsi.size()==count+2 &&
        result.witness.omitEachInput.size()==count && result.reason=="omission_group_closed",
        "matched_codec_incomplete_group");
    Require(Number(result.maximumModelSeconds)>0. && Number(result.maximumWallSeconds)>0.,"matched_codec_invalid_time_budget");
    Require(result.appliedSourceIsi==result.sourceIsi && Hash(result.sourceIsi)==Text(s.patternHash),"matched_codec_original_isi_changed");
    const auto preflight=ValidateMatchedOmissionPolicy(result.policy,s.stimulusProfile,count);
    Require(preflight.validity==MatchedOmissionValidity::Valid,"matched_codec_invalid_policy");
    const auto full=FullMask(count);(void)PlanStimulusSubset(result.sourceIsi,full);
    for(std::size_t i=0;i<count;++i)
        Require(s.stimulusIds[i]=="pulse"+std::to_string(i),"matched_codec_original_stimulus_identity_changed");
    CheckedComposite checked;checked.traces.push_back(&result.witness.fullBefore);
    for(std::size_t i=0;i<count;++i)
    {
        auto mask=full;mask.erase(mask.begin()+std::ptrdiff_t(i));const MatchedOmissionTrace* found=nullptr;
        for(const auto& trace:result.witness.omitEachInput)if(trace.presentedOrdinals==mask)
        {Require(!found,"matched_codec_duplicate_omission");found=&trace;}
        Require(found,"matched_codec_missing_omission");checked.traces.push_back(found);
    }
    checked.traces.push_back(&result.witness.fullAfter);
    for(std::size_t i=0;i<checked.traces.size();++i)
    {
        const auto& trace=*checked.traces[i];const auto& native=result.packets[i];RequireClosedNative(native);
        auto mask=full;if(i>0 && i<=count)mask.erase(mask.begin()+std::ptrdiff_t(i-1));
        const auto presented=PlanStimulusSubset(result.sourceIsi,mask).presentedIsi;const auto hash=Hash(presented);
        const auto& q=*native.requestedSpecification;const auto& actual=native.observation->specification;
        Require(trace.presentedOrdinals==mask && result.appliedPresentedIsi[i]==presented && actual.stimulusSubset &&
            actual.stimulusSubset->physicalInputCount==count && actual.stimulusSubset->sourceOrdinals==mask &&
            actual.stimulusSubset->sourceTrainingPatternHash==s.patternHash &&
            actual.patternHash==hash.toStdString() && q.patternHash==hash.toStdString() &&
            actual.maximumSamples<=result.policy.maximumSamples && q.end==s.duration &&
            actual.stimulusProfile==s.stimulusProfile && trace.stimulusProfile==s.stimulusProfile,
            "matched_codec_presented_isi_or_native_declaration_changed");
        if(i)Require(actual.begin>result.packets[i-1].observation->specification.end,"matched_codec_group_protocol_times_overlap");
    }
    const double span=result.packets.back().observation->specification.end-
                      result.packets.front().observation->specification.begin;
    Require(Number(span)>0. && span<=result.maximumModelSeconds,"matched_codec_observed_model_span_exceeds_declaration");
    checked.assignment=SelectMatchedOmissionMaximum(s,result.policy,result.witness);
    Require(AllowedOutcome(checked.assignment.validity),"matched_codec_unusable_complete_group");
    if(result.assignment)Require(Assignment(*result.assignment)==Assignment(checked.assignment),"matched_codec_stale_assignment");
    return checked;
}
QJsonObject Source(const NativeMatchedOmissionResult& result)
{return {{"planned_isi",Numbers(result.sourceIsi)},{"applied_isi",Numbers(result.appliedSourceIsi)},
    {"planned_sha256",Hash(result.sourceIsi)},{"applied_sha256",Hash(result.appliedSourceIsi)},{"canonical_encoding",Encoding}};}

double CsvNumber(const std::string& token)
{
    std::istringstream stream(token);stream.imbue(std::locale::classic());double value=0.;stream>>std::noskipws>>value;
    Require(bool(stream) && stream.eof(),"matched_codec_invalid_csv_number");return Number(value);
}
MatchedOmissionTrace ImportTrace(const NativeProbeResult& native,const std::vector<std::string>& paths,
                                const QByteArray& bytes)
{
    RequireClosedNative(native);const auto& actual=native.observation->specification;
    Require(bytes.endsWith('\n') && !paths.empty() && paths.size()==actual.baseline.size(),"matched_codec_invalid_csv_shape");
    NativeResponseDomainCapture capture;capture.Arm(*native.prepared,actual,paths);
    std::istringstream stream(bytes.toStdString());stream.imbue(std::locale::classic());std::string line;
    std::ostringstream expectedHeader;expectedHeader<<"time,stimulus";
    for(std::size_t channel=0;channel<paths.size();++channel)expectedHeader<<",soma"<<channel;
    Require(bool(std::getline(stream,line)) && line==expectedHeader.str(),"matched_codec_invalid_csv_header");
    std::size_t event=0,frames=0;
    while(std::getline(stream,line))
    {
        Require(!line.empty() && frames<native.observation->sampleCount && frames<actual.maximumSamples,
                "matched_codec_csv_sample_budget");
        std::vector<double> values;std::size_t begin=0;
        for(std::size_t column=0;column<paths.size()+2;++column)
        {
            const auto comma=line.find(',',begin);
            Require(column==paths.size()+1 ? comma==std::string::npos : comma!=std::string::npos,
                    "matched_codec_csv_columns_changed");
            values.push_back(CsvNumber(line.substr(begin,comma==std::string::npos?comma:comma-begin)));
            if(comma!=std::string::npos)begin=comma+1;
        }
        NativeProbeSample sample;sample.time=values[0];sample.stimulus=values[1];
        sample.effectiveModelRevision=native.prepared->effectiveModelRevision;
        sample.potentials.assign(values.begin()+2,values.end());
        while(event<native.observation->stimuli.size() && !Detail::After(native.observation->stimuli[event].time,sample.time))
        {sample.newStimuli.push_back(native.observation->stimuli[event]);++event;}
        capture.Append(sample);++frames;
    }
    Require(stream.eof() && frames==native.observation->sampleCount && event==native.observation->stimuli.size(),
            "matched_codec_incomplete_csv");capture.CloseNative(native);
    Require(EncodeNativeResponseDomainTrace(capture)==bytes,"matched_codec_noncanonical_csv");
    MatchedOmissionTrace trace;trace.prepared=capture.Context();trace.observation=capture.Observation();
    trace.physicalChannelPaths=capture.PhysicalChannelPaths();trace.stimulusSamples=capture.StimulusSamples();
    trace.stimulusProfile=*actual.stimulusProfile;return trace;
}
}

QJsonObject EncodeMatchedOmissionEvidence(const NativeMatchedOmissionResult& result,
    const std::vector<QString>& paths,const MatchedOmissionPrimary& primary)
{
    const auto checked=CheckComposite(result);
    Require(paths.size()==checked.traces.size() && primary.size()==paths.size(),"matched_codec_primary_inventory_changed");
    QJsonArray records;std::set<QString> seen;
    for(std::size_t i=0;i<paths.size();++i)
    {
        SafePath(paths[i]);Require(seen.insert(paths[i]).second,"matched_codec_reused_trace_path");
        const auto found=primary.find(paths[i]);Require(found!=primary.end(),"matched_codec_missing_primary");
        const auto& trace=*checked.traces[i];const auto bytes=EncodeMatchedOmissionPrimaryTrace(trace,result.packets[i]);
        Require(found->second==bytes,"matched_codec_primary_does_not_match_native_trace");
        const auto presented=PlanStimulusSubset(result.sourceIsi,trace.presentedOrdinals).presentedIsi;
        records.append(QJsonObject{{"role",i==0?"full_before":i+1==paths.size()?"full_after":"omit"},
            {"omitted_input",i==0 || i+1==paths.size()?QJsonValue(QJsonValue::Null):QJsonValue(Count(i-1))},
            {"presented_ordinals",Indices(trace.presentedOrdinals)},{"planned_presented_isi",Numbers(presented)},
            {"applied_presented_isi",Numbers(result.appliedPresentedIsi[i])},{"presented_pattern_sha256",Hash(presented)},
            {"relative_path",paths[i]},{"bytes",Count(std::size_t(bytes.size()))},{"sha256",BytesHash(bytes)},
            {"native_evidence",EncodeNativeProbeEvidence(result.packets[i])},{"binding",Binding(trace)}});
    }
    return {{"schema_version",1},{"kind","timelearner_matched_omission_evidence"},{"method",MatchedOmissionResult::MethodId()},
        {"complete_group",true},{"diagnostic_only",true},{"training_certificate_eligible",false},
        {"scientific_verdict",QJsonValue(QJsonValue::Null)},{"specification",Specification(result.specification)},
        {"policy",Policy(result.policy)},{"source",Source(result)},{"time_budget",TimeBudget(result)},
        {"traces",records},{"assignment",Assignment(checked.assignment)},{"attestation",Attestation()}};
}

NativeMatchedOmissionResult DecodeMatchedOmissionEvidence(const QJsonObject& object,const MatchedOmissionPrimary& primary)
{
    Keys(object,TopFields);
    Require(Version(object["schema_version"])==1 && object["kind"]==QJsonValue("timelearner_matched_omission_evidence") &&
        object["method"]==QJsonValue(MatchedOmissionResult::MethodId()) && Boolean(object["complete_group"]) &&
        Boolean(object["diagnostic_only"]) && !Boolean(object["training_certificate_eligible"]) && object["scientific_verdict"].isNull() &&
        Object(object["attestation"])==Attestation(),"matched_codec_unknown_schema_or_attestation");
    NativeMatchedOmissionResult result;result.specification=Specification(object["specification"]);result.policy=Policy(object["policy"]);
    const auto source=Object(object["source"]);Keys(source,"planned_isi applied_isi planned_sha256 applied_sha256 canonical_encoding");
    result.sourceIsi=Numbers(source["planned_isi"]);result.appliedSourceIsi=Numbers(source["applied_isi"]);
    const auto count=result.sourceIsi.size();
    Require(count>=2 && count<=std::numeric_limits<std::size_t>::max()-2 && result.specification.stimulusIds.size()==count &&
        result.appliedSourceIsi==result.sourceIsi && Hash(result.sourceIsi)==Text(result.specification.patternHash) &&
        source==Source(result),"matched_codec_source_hash_or_inventory_changed");
    const auto preflight=ValidateMatchedOmissionPolicy(result.policy,result.specification.stimulusProfile,count);
    Require(preflight.validity==MatchedOmissionValidity::Valid,"matched_codec_invalid_policy");
    const auto budget=Object(object["time_budget"]);
    Keys(budget,"maximum_model_seconds maximum_wall_seconds elapsed_model_seconds elapsed_wall_seconds enforcement_verified");
    result.maximumModelSeconds=Number(budget["maximum_model_seconds"]);result.maximumWallSeconds=Number(budget["maximum_wall_seconds"]);
    Require(result.maximumModelSeconds>0. && result.maximumWallSeconds>0. && budget==TimeBudget(result),
            "matched_codec_invented_time_budget_attestation");
    const auto records=Array(object["traces"]);
    Require(std::size_t(records.size())==count+2 && primary.size()==count+2,"matched_codec_incomplete_primary_inventory");
    std::vector<QString> paths;std::set<QString> seen;const auto full=FullMask(count);std::size_t totalSamples=0;
    for(std::size_t i=0;i<count+2;++i)
    {
        const auto record=Object(records[int(i)]);Keys(record,TraceFields);
        const bool first=i==0,last=i==count+1;
        Require(Text(record["role"])==(first?"full_before":last?"full_after":"omit") &&
            (first || last ? record["omitted_input"].isNull() : Count(record["omitted_input"])==i-1),
            "matched_codec_probe_role_changed");
        auto mask=full;if(!first && !last)mask.erase(mask.begin()+std::ptrdiff_t(i-1));
        Require(Indices(record["presented_ordinals"])==mask,"matched_codec_mask_inventory_changed");
        const auto planned=PlanStimulusSubset(result.sourceIsi,mask).presentedIsi;
        const auto applied=Numbers(record["applied_presented_isi"]);
        Require(Numbers(record["planned_presented_isi"])==planned && applied==planned &&
            record["presented_pattern_sha256"]==QJsonValue(Hash(planned)),"matched_codec_presented_isi_hash_changed");
        const auto path=QString::fromStdString(Text(record["relative_path"]));SafePath(path);
        Require(seen.insert(path).second,"matched_codec_reused_trace_path");const auto found=primary.find(path);
        Require(found!=primary.end() && Count(record["bytes"])==std::size_t(found->second.size()) &&
            record["sha256"]==QJsonValue(BytesHash(found->second)),"matched_codec_primary_hash_changed");
        const auto native=DecodeNativeProbeEvidence(Object(record["native_evidence"]));RequireClosedNative(native);
        Require(native.observation->sampleCount<=result.policy.maximumSamples &&
            native.observation->specification.maximumSamples<=result.policy.maximumSamples &&
            totalSamples<=result.policy.maximumTotalSamples &&
            native.observation->sampleCount<=result.policy.maximumTotalSamples-totalSamples,
            "matched_codec_native_sample_budget_exceeded");
        totalSamples+=native.observation->sampleCount;
        const auto& subset=native.observation->specification.stimulusSubset;
        Require(subset && subset->physicalInputCount==count && subset->sourceOrdinals==mask &&
            subset->sourceTrainingPatternHash==result.specification.patternHash,
            "matched_codec_native_subset_inventory_changed");
        const auto binding=Object(record["binding"]);Keys(binding,BindingFields);
        const auto physicalPaths=Strings(binding["physical_channel_paths"]);
        Require(physicalPaths==result.specification.physicalChannelPaths,"matched_codec_channel_paths_changed");
        auto trace=ImportTrace(native,physicalPaths,found->second);SetBinding(trace,binding);trace.presentedOrdinals=mask;
        if(first)result.witness.fullBefore=std::move(trace);
        else if(last)result.witness.fullAfter=std::move(trace);
        else result.witness.omitEachInput.push_back(std::move(trace));
        result.packets.push_back(native);result.appliedPresentedIsi.push_back(applied);paths.push_back(path);
    }
    result.reason="omission_group_closed";const auto checked=CheckComposite(result);
    Require(Object(object["assignment"])==Assignment(checked.assignment),"matched_codec_assignment_not_reproduced");
    result.assignment=checked.assignment;
    // Enforce every closed field and optional/null convention after replay.
    Require(EncodeMatchedOmissionEvidence(result,paths,primary)==object,"matched_codec_noncanonical_evidence");return result;
}

NativeMatchedOmissionResult DecodeMatchedOmissionEvidence(const QByteArray& document,const MatchedOmissionPrimary& primary)
{return DecodeMatchedOmissionEvidence(ParseStrictNativeJsonObject(document.toStdString(),128*1024*1024),primary);}
}
