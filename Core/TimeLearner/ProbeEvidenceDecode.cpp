#include "ProbeEvidenceJson.h"
#include <QJsonArray>
#include <cmath>
#include <limits>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool condition){if(!condition)throw std::invalid_argument("invalid_native_probe_evidence_field");}
QJsonObject Object(const QJsonValue& v){Require(v.isObject());return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray());return v.toArray();}
std::string Text(const QJsonValue& v){Require(v.isString());return v.toString().toStdString();}
bool Boolean(const QJsonValue& v){Require(v.isBool());return v.toBool();}
double Number(const QJsonValue& v){Require(v.isDouble() && std::isfinite(v.toDouble()));return v.toDouble();}
std::size_t Count(const QJsonValue& v)
{const auto n=Number(v);Require(n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n);return std::size_t(n);}
unsigned Version(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<unsigned>::max());return unsigned(n);}
std::optional<double> Maybe(const QJsonValue& v){return v.isNull()?std::nullopt:std::optional<double>(Number(v));}
std::vector<std::string> Strings(const QJsonValue& v)
{std::vector<std::string> r;for(const auto& value:Array(v))r.push_back(Text(value));return r;}
PeakValidity Validity(const QJsonValue& v)
{
    const auto text=Text(v);
    for(auto p:{PeakValidity::Valid,PeakValidity::Missing,PeakValidity::Ambiguous,PeakValidity::Clipped,PeakValidity::Incomplete})
        if(text==PeakValidityName(p))return p;
    throw std::invalid_argument("unknown_native_peak_validity");
}
ProbeRouting Routing(const QJsonValue& v)
{const auto text=Text(v);Require(text=="classic_burst" || text=="branch_solo_burst");return text=="classic_burst"?ProbeRouting::ClassicBurst:ProbeRouting::BranchSoloBurst;}
ProbeSpec Spec(const QJsonValue& value)
{
    const auto o=Object(value);ProbeSpec s;s.schemaVersion=Version(o["schema_version"]);
    s.runId=Text(o["run_id"]);s.trialId=Text(o["trial_id"]);s.sampleId=Text(o["sample_id"]);
    s.patternHash=Text(o["pattern_hash"]);s.modelRevision=Text(o["effective_model_revision"]);
    s.begin=Number(o["begin"]);s.end=Number(o["end"]);s.maximumSampleGap=Number(o["maximum_sample_gap"]);
    s.minimumAmplitude=Number(o["minimum_amplitude"]);s.maximumSamples=Count(o["maximum_samples"]);s.recordSpikes=Boolean(o["record_spikes"]);
    for(const auto& v:Array(o["baseline"]))s.baseline.push_back(Number(v));
    for(const auto& v:Array(o["windows"]))
    {const auto w=Object(v);s.windows.push_back({Text(w["input_id"]),Count(w["channel"]),Number(w["begin"]),Number(w["end"]),Text(w["stimulus_id"])});}
    s.expectedStimulusIds=Strings(o["expected_stimulus_ids"]);
    if(!o["stimulus_profile"].isNull())
    {const auto p=Object(o["stimulus_profile"]);s.stimulusProfile=StimulusProfile{Text(p["generator_class"]),Number(p["amplitude"]),Number(p["pulse_length"]),
        Number(p["frequency_hz"]),Number(p["frequency_deviation"]),Boolean(p["use_transit_signal"]),Boolean(p["use_pattern_output"])};}
    const auto a=Text(o["wave_assignment"]);Require(a=="window_only" || a=="ordered_whole_burst");
    s.waveAssignment=a=="window_only"?WaveAssignmentPolicy::WindowOnly:WaveAssignmentPolicy::OrderedWholeBurst;
    const auto p=Text(o["peak_selection"]);Require(p=="isolated_window" || p=="assigned_burst_wave");
    s.peakSelection=p=="isolated_window"?PeakSelectionPolicy::IsolatedWindow:PeakSelectionPolicy::AssignedBurstWave;return s;
}
PreparedVerificationProbe Prepared(const QJsonValue& value)
{
    const auto o=Object(value);PreparedVerificationProbe p;p.routing=Routing(o["routing"]);
    p.inputId=Text(o["input_id"]);p.protocolRevision=Text(o["protocol_revision"]);
    p.baseModelRevision=Text(o["base_model_revision"]);p.effectiveModelRevision=Text(o["effective_model_revision"]);p.modelEpoch=Count(o["model_epoch"]);
    if(!o["selected_input_index"].isNull())p.selectedInputIndex=Count(o["selected_input_index"]);return p;
}
ProbeObservation Observation(const QJsonValue& value)
{
    const auto o=Object(value);ProbeObservation r;r.schemaVersion=Version(o["schema_version"]);r.specification=Spec(o["specification"]);
    r.complete=Boolean(o["complete"]);r.valid=Boolean(o["valid"]);r.endReason=Text(o["end_reason"]);r.reasons=Strings(o["reasons"]);
    r.sampleCount=Count(o["sample_count"]);
    for(const auto& v:Array(o["stimuli"]))
    {const auto e=Object(v);r.stimuli.push_back({Text(e["id"]),Number(e["time"])});}
    for(const auto& v:Array(o["spikes"]))
    {const auto e=Object(v);r.spikes.push_back({Count(e["channel"]),Number(e["time"])});}
    Require(r.specification.recordSpikes || r.spikes.empty());
    for(const auto& v:Array(o["peaks"]))
    {
        const auto p=Object(v);PeakObservation peak;peak.inputId=Text(p["input_id"]);peak.stimulusId=Text(p["stimulus_id"]);
        peak.validity=Validity(p["validity"]);peak.amplitude=Maybe(p["amplitude"]);peak.rawPotential=Maybe(p["raw_potential"]);
        peak.time=Maybe(p["time"]);peak.plateauEnd=Maybe(p["plateau_end"]);peak.delayFromStimulus=Maybe(p["delay_from_stimulus"]);
        peak.candidateCount=Count(p["candidate_count"]);r.peaks.push_back(peak);
    }
    for(const auto& v:Array(o["burst_assignments"]))
    {
        const auto a=Object(v);BurstAssignmentObservation assignment;assignment.channel=Count(a["channel"]);assignment.validity=Validity(a["validity"]);
        for(const auto& w:Array(a["waves"]))
        {const auto wave=Object(w);assignment.waves.push_back({Text(wave["stimulus_id"]),Number(wave["amplitude"]),Number(wave["raw_potential"]),Number(wave["time"]),Number(wave["plateau_end"])});}
        r.burstAssignments.push_back(std::move(assignment));
    }
    return r;
}
}
NativeProbeResult DecodeNativeProbeEvidence(const QJsonObject& o)
{
    Require(o["kind"]==QJsonValue("timelearner_native_probe_evidence") && o["evidence_schema_version"]==QJsonValue(2));
    NativeProbeResult r;const auto status=Text(o["native_status"]);bool known=false;
    for(auto s:{NativeProbeStatus::Idle,NativeProbeStatus::Queued,NativeProbeStatus::Running,NativeProbeStatus::Completed,NativeProbeStatus::Rejected,NativeProbeStatus::Cancelled})
        if(status==NativeProbeStatusName(s)){r.status=s;known=true;break;}
    Require(known);r.currentBaseRevision=Text(o["current_base_revision"]);r.reasons=Strings(o["reasons"]);
    r.requiresFreshVerification=Boolean(o["requires_fresh_verification"]);
    if(!o["requested_specification"].isNull())r.requestedSpecification=Spec(o["requested_specification"]);
    if(!o["prepared"].isNull())r.prepared=Prepared(o["prepared"]);
    if(!o["observation"].isNull())r.observation=Observation(o["observation"]);
    // Enforce the complete closed schema, optional/null semantics and all
    // derived relative timestamps/counts rather than accepting lossy coercions.
    if(EncodeNativeProbeEvidence(r)!=o)throw std::invalid_argument("noncanonical_native_probe_evidence");
    return r;
}
VerificationProtocol DecodeVerificationProtocol(const QJsonObject& o)
{
    // Reuse the same strict primitive/profile/enum decoder; this temporary
    // specification is data only and is never scheduled or accepted as a probe.
    QJsonObject spec{{"schema_version",o["schema_version"]},{"run_id",""},{"trial_id",""},
        {"sample_id",o["sample_id"]},{"pattern_hash",o["pattern_hash"]},{"effective_model_revision",""},
        {"begin",0.},{"end",0.},{"maximum_sample_gap",o["maximum_sample_gap"]},{"minimum_amplitude",o["minimum_amplitude"]},
        {"maximum_samples",0},{"record_spikes",false},{"baseline",QJsonArray{}},{"windows",QJsonArray{}},
        {"expected_stimulus_ids",o["stimulus_ids"]},{"stimulus_profile",o["stimulus_profile"]},
        {"wave_assignment",o["wave_assignment"]},{"peak_selection",o["peak_selection"]}};
    const auto s=Spec(spec);VerificationProtocol p;p.schemaVersion=s.schemaVersion;p.routing=Routing(o["routing"]);
    p.revision=Text(o["revision"]);p.sampleId=s.sampleId;p.patternHash=s.patternHash;p.originStimulusId=Text(o["origin_stimulus_id"]);
    p.stimulusIds=s.expectedStimulusIds;p.maximumSampleGap=s.maximumSampleGap;p.minimumAmplitude=s.minimumAmplitude;
    p.stimulusProfile=s.stimulusProfile;p.waveAssignment=s.waveAssignment;p.peakSelection=s.peakSelection;
    for(const auto& v:Array(o["inputs"]))
    {const auto i=Object(v);Require(i.size()==2);p.inputs.push_back({Text(i["id"]),Text(i["stimulus_id"])});}
    ValidateVerificationProtocol(p);Require(EncodeVerificationProtocol(p)==o);return p;
}
}
