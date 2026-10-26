#include "ProbeEvidenceJson.h"
#include <QJsonArray>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
QString Text(const std::string& value){return QString::fromStdString(value);}
QJsonValue Number(double value)
{if(!std::isfinite(value))throw std::invalid_argument("nonfinite_native_probe_evidence_number");return value;}
QJsonValue Count(std::size_t value)
{if(value>9007199254740991ULL)throw std::invalid_argument("inexact_native_probe_evidence_integer");return double(value);}
QJsonValue Maybe(const std::optional<double>& value)
{return value?Number(*value):QJsonValue(QJsonValue::Null);}
QJsonArray Strings(const std::vector<std::string>& values)
{QJsonArray result;for(const auto& value:values)result.append(Text(value));return result;}
QString Routing(ProbeRouting routing)
{if(routing==ProbeRouting::ClassicBurst)return "classic_burst";if(routing==ProbeRouting::BranchSoloBurst)return "branch_solo_burst";throw std::invalid_argument("unknown_probe_routing");}
QString Assignment(WaveAssignmentPolicy policy)
{if(policy==WaveAssignmentPolicy::WindowOnly)return "window_only";if(policy==WaveAssignmentPolicy::OrderedWholeBurst)return "ordered_whole_burst";throw std::invalid_argument("unknown_wave_assignment");}
QJsonObject Profile(const StimulusProfile& p)
{return {{"generator_class",Text(p.generatorClass)},{"amplitude",Number(p.amplitude)},{"pulse_length",Number(p.pulseLength)},
 {"frequency_hz",Number(p.frequencyHz)},{"frequency_deviation",Number(p.frequencyDeviation)},
 {"use_transit_signal",p.useTransitSignal},{"use_pattern_output",p.usePatternOutput}};}
QJsonObject Spec(const ProbeSpec& s,const char* timeFrame)
{
    QJsonArray baseline,windows;
    for(double value:s.baseline)baseline.append(Number(value));
    for(const auto& w:s.windows)windows.append(QJsonObject{{"input_id",Text(w.inputId)},{"stimulus_id",Text(w.stimulusId)},
        {"channel",Count(w.channel)},{"begin",Number(w.begin)},{"end",Number(w.end)}});
    return {{"time_frame",timeFrame},{"schema_version",int(s.schemaVersion)},{"run_id",Text(s.runId)},{"trial_id",Text(s.trialId)},
        {"sample_id",Text(s.sampleId)},{"pattern_hash",Text(s.patternHash)},{"effective_model_revision",Text(s.modelRevision)},
        {"begin",Number(s.begin)},{"end",Number(s.end)},{"maximum_sample_gap",Number(s.maximumSampleGap)},
        {"minimum_amplitude",Number(s.minimumAmplitude)},{"maximum_samples",Count(s.maximumSamples)},
        {"record_spikes",s.recordSpikes},{"baseline",baseline},{"windows",windows},
        {"expected_stimulus_ids",Strings(s.expectedStimulusIds)},
        {"stimulus_profile",s.stimulusProfile?QJsonValue(Profile(*s.stimulusProfile)):QJsonValue(QJsonValue::Null)},
        {"wave_assignment",Assignment(s.waveAssignment)}};
}
QJsonObject Observation(const ProbeObservation& o)
{
    QJsonArray stimuli,spikes,peaks,assignments;
    for(const auto& event:o.stimuli)stimuli.append(QJsonObject{{"id",Text(event.id)},{"time",Number(event.time)},
        {"relative_to_trial_start",Number(event.time-o.specification.begin)}});
    for(const auto& spike:o.spikes)spikes.append(QJsonObject{{"channel",Count(spike.channel)},
        {"time",Number(spike.time)},{"relative_to_trial_start",Number(spike.time-o.specification.begin)}});
    for(const auto& p:o.peaks)peaks.append(QJsonObject{{"input_id",Text(p.inputId)},{"stimulus_id",Text(p.stimulusId)},
        {"validity",PeakValidityName(p.validity)},{"amplitude",Maybe(p.amplitude)},{"raw_potential",Maybe(p.rawPotential)},
        {"time",Maybe(p.time)},{"plateau_end",Maybe(p.plateauEnd)},{"delay_from_stimulus",Maybe(p.delayFromStimulus)},
        {"relative_to_trial_start",p.time?Number(*p.time-o.specification.begin):QJsonValue(QJsonValue::Null)},
        {"candidate_count",Count(p.candidateCount)}});
    for(const auto& a:o.burstAssignments)
    {
        QJsonArray waves;
        for(const auto& w:a.waves)waves.append(QJsonObject{{"stimulus_id",Text(w.stimulusId)},
            {"amplitude",Number(w.amplitude)},{"raw_potential",Number(w.rawPotential)},
            {"time",Number(w.time)},{"plateau_end",Number(w.plateauEnd)},
            {"relative_to_trial_start",Number(w.time-o.specification.begin)}});
        assignments.append(QJsonObject{{"channel",Count(a.channel)},{"validity",PeakValidityName(a.validity)},{"waves",waves}});
    }
    return {{"schema_version",int(o.schemaVersion)},{"specification",Spec(o.specification,"absolute_model_time")},
        {"complete",o.complete},{"valid",o.valid},{"end_reason",Text(o.endReason)},{"reasons",Strings(o.reasons)},
        {"sample_count",Count(o.sampleCount)},{"stimuli",stimuli},{"peaks",peaks},{"burst_assignments",assignments},
        {"spikes",spikes},{"spike_count",o.specification.recordSpikes?Count(o.spikes.size()):QJsonValue(QJsonValue::Null)},
        {"first_stimulus_time",o.stimuli.empty()?QJsonValue(QJsonValue::Null):Number(o.stimuli.front().time)}};
}
QJsonObject Prepared(const PreparedVerificationProbe& p)
{return {{"routing",Routing(p.routing)},{"input_id",Text(p.inputId)},{"protocol_revision",Text(p.protocolRevision)},
 {"base_model_revision",Text(p.baseModelRevision)},{"effective_model_revision",Text(p.effectiveModelRevision)},
 {"model_epoch",Count(p.modelEpoch)},{"selected_input_index",p.selectedInputIndex?Count(*p.selectedInputIndex):QJsonValue(QJsonValue::Null)}};}
}
QJsonObject EncodeNativeProbeEvidence(const NativeProbeResult& result)
{
    return {{"kind","timelearner_native_probe_evidence"},{"evidence_schema_version",1},
        {"native_status",NativeProbeStatusName(result.status)},{"current_base_revision",Text(result.currentBaseRevision)},
        {"reasons",Strings(result.reasons)},{"requires_fresh_verification",result.requiresFreshVerification},
        {"requested_specification",result.requestedSpecification?
            QJsonValue(Spec(*result.requestedSpecification,"relative_to_trial_start")):QJsonValue(QJsonValue::Null)},
        {"prepared",result.prepared?QJsonValue(Prepared(*result.prepared)):QJsonValue(QJsonValue::Null)},
        {"observation",result.observation?QJsonValue(Observation(*result.observation)):QJsonValue(QJsonValue::Null)}};
}
}
