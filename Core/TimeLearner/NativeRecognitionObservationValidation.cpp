#include "NativeRecognitionObservationValidation.h"
#include "NativeTrainingBindings.h"
#include "ModelIdentity.h"
#include "NumericChecks.h"
#include "ConfiguredNativeTraining.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
bool EqualTime(double a,double b){return !Detail::After(a,b) && !Detail::After(b,a);}
double Number(const QJsonValue& value)
{
    Require(value.isDouble() && Detail::Finite(value.toDouble()),"invalid_native_observation_json_number");
    return value.toDouble();
}
std::string Text(const QJsonValue& value)
{
    Require(value.isString(),"invalid_native_observation_json_text");return value.toString().toStdString();
}
bool Flag(const QJsonValue& value)
{
    Require(value.isBool(),"invalid_native_observation_json_flag");return value.toBool();
}
std::vector<double> Numbers(const QJsonValue& value)
{
    Require(value.isArray(),"invalid_native_observation_json_array");
    std::vector<double> result;result.reserve(std::size_t(value.toArray().size()));
    for(const auto& item:value.toArray())result.push_back(Number(item));return result;
}
void History(const std::vector<double>& history,double time,double begin,
    std::optional<double>& watermark,std::vector<double>& actual)
{
    std::optional<double> previous;
    for(double event:history)
    {
        Require(Detail::Finite(event) && !Detail::After(event,time) && (!previous || event>*previous),
            "invalid_calibration_raw_event_history");
        previous=event;
        if((!watermark || event>*watermark) && !Detail::After(begin,event))actual.push_back(event);
    }
    if(previous && (!watermark || *previous>*watermark))watermark=previous;
}
}
void ValidateNativeRecognitionObservation(const NativeRecognitionTrialRequest& q,
    const NativeRecognitionTrialObservation& o)
{
    Require(!q.runId.empty() && !q.trialId.empty() && !q.sampleId.empty() &&
        (q.label=="target" || q.label=="foil") &&
        (q.resetPolicy==RecognitionResetPolicy::PerTrial || q.resetPolicy==RecognitionResetPolicy::PreserveNeuronDynamics) &&
        Detail::Finite(q.duration) && q.duration>0. && q.maximumSamples>=2,
        "invalid_calibration_observation_design");
    Require(q.pattern.GetCols()==1 && q.pattern.GetRows()>0,"invalid_calibration_observation_pattern");
    QJsonArray pattern;
    for(int i=0;i<q.pattern.GetRows();++i)
    {
        const double value=q.pattern(i,0);
        Require(Detail::Finite(value) && value>=0.,"invalid_calibration_observation_pattern");pattern.append(value);
    }
    Require(q.patternHash==NmsdkTools::CanonicalModelDataSha256(pattern).toStdString(),
        "calibration_observation_pattern_hash_mismatch");
    Require(o.request.runId==q.runId && o.request.trialId==q.trialId && o.request.sampleId==q.sampleId &&
        o.request.label==q.label && o.request.patternHash==q.patternHash && o.request.duration==q.duration &&
        o.request.resetPolicy==q.resetPolicy && o.request.maximumSamples==q.maximumSamples &&
        o.request.recordThresholdPotential==q.recordThresholdPotential && NativeTrainingPatternEqual(o.request.pattern,q.pattern),
        "calibration_observation_request_changed");
    Require(o.armed && o.complete && o.valid && o.reason=="window_closed" &&
        !o.modelBefore.empty() && o.modelBefore==o.modelAfter,"invalid_calibration_observation");
    Require(Detail::Finite(o.begin) && Detail::Finite(o.end) && Detail::Finite(o.closedAt) &&
        Detail::Finite(o.frequencyHz) && o.frequencyHz>0. && o.end>o.begin &&
        EqualTime(o.end,o.begin+q.duration) && EqualTime(o.closedAt,o.end) &&
        Detail::ResolvableTimeInterval(o.begin,o.end,1./o.frequencyHz),"invalid_calibration_observation_clock");
    Require(o.samples.size()>=2 && o.samples.size()<=q.maximumSamples,"invalid_calibration_observation_sample_count");
    const double ticks=q.duration*o.frequencyHz;
    Require(Detail::Finite(ticks) && ticks>=1. && EqualTime(ticks,std::round(ticks)) &&
        double(o.samples.size()-1)==std::round(ticks),"incomplete_calibration_observation_grid");
    if(q.recordThresholdPotential)Require(!o.thresholdPotentialPath.empty() && !o.thresholdPotentialClass.empty(),
        "missing_calibration_threshold_potential_source");
    const auto channels=o.samples.front().somaPotentials.size();
    Require(channels>0,"missing_calibration_soma_channels");
    std::optional<double> stimulusWatermark,spikeWatermark,previous;
    std::vector<double> stimuli,spikes;
    for(std::size_t i=0;i<o.samples.size();++i)
    {
        const auto& sample=o.samples[i];
        Require(Detail::Finite(sample.time) && EqualTime(sample.time,o.begin+double(i)/o.frequencyHz) &&
            (!previous || (sample.time>*previous && !Detail::IntervalAfter(sample.time,*previous,1./o.frequencyHz))),
            "invalid_calibration_observation_sample_grid");
        previous=sample.time;
        Require(Detail::Finite(sample.stimulus) && Detail::Finite(sample.ltOutput) &&
            sample.somaPotentials.size()==channels && std::all_of(sample.somaPotentials.begin(),sample.somaPotentials.end(),Detail::Finite),
            "invalid_calibration_observation_sample_values");
        if(q.recordThresholdPotential)Require(Detail::Finite(sample.thresholdPotential),"nonfinite_calibration_threshold_potential");
        History(sample.stimulusHistory,sample.time,o.begin,stimulusWatermark,stimuli);
        History(sample.spikeHistory,sample.time,o.begin,spikeWatermark,spikes);
    }
    Require(EqualTime(o.samples.front().time,o.begin) && EqualTime(o.samples.back().time,o.end),
        "incomplete_calibration_observation_window");
    Require(stimuli==o.stimulusTimes && spikes==o.spikeTimes,"calibration_observation_events_disagree_with_frames");
    Require(stimuli.size()==std::size_t(q.pattern.GetRows()),"calibration_observation_stimulus_count_mismatch");
    double planned=o.begin;
    for(int i=0;i<q.pattern.GetRows();++i)
    {
        planned+=q.pattern(i,0);
        Require(Detail::Finite(planned) && !Detail::After(planned,stimuli[std::size_t(i)]) &&
            !Detail::After(stimuli[std::size_t(i)],planned+1./o.frequencyHz),"calibration_observation_stimulus_time_mismatch");
    }
}
NativeRecognitionTrialObservation DecodeNativeRecognitionObservation(const QJsonObject& object,
    const NativeRecognitionTrialRequest& expected)
{
    NativeRecognitionTrialObservation o;
    // Request metadata is not inferred from measured timestamps. Exact encoding
    // comparison below binds every serialized request field to this declaration.
    o.request=expected;
    o.modelBefore=Text(object["model_before"]);o.modelAfter=Text(object["model_after"]);
    o.reason=Text(object["reason"]);o.begin=Number(object["begin"]);o.end=Number(object["end"]);
    o.closedAt=Number(object["closed_at"]);o.frequencyHz=Number(object["frequency_hz"]);
    o.armed=Flag(object["armed"]);o.complete=Flag(object["complete"]);o.valid=Flag(object["valid"]);
    o.stimulusTimes=Numbers(object["stimulus_times"]);o.spikeTimes=Numbers(object["spike_times"]);
    Require(object["samples"].isArray(),"invalid_native_observation_json_samples");
    const auto frames=object["samples"].toArray();
    Require(frames.size()>=2 && std::size_t(frames.size())<=expected.maximumSamples,
        "invalid_calibration_observation_sample_count");
    o.samples.reserve(std::size_t(frames.size()));
    for(const auto& value:frames)
    {
        Require(value.isObject(),"invalid_native_observation_json_frame");const auto frame=value.toObject();
        NativeRecognitionTrialSample sample;sample.time=Number(frame["time"]);
        sample.stimulus=Number(frame["stimulus"]);sample.ltOutput=Number(frame["lt_output"]);
        sample.somaPotentials=Numbers(frame["soma"]);sample.stimulusHistory=Numbers(frame["stimulus_history"]);
        sample.spikeHistory=Numbers(frame["spike_history"]);
        if(expected.recordThresholdPotential)sample.thresholdPotential=Number(frame["lt_potential"]);
        o.samples.push_back(std::move(sample));
    }
    if(expected.recordThresholdPotential)
    {
        Require(object["threshold_potential_source"].isObject(),"missing_calibration_threshold_potential_source");
        const auto source=object["threshold_potential_source"].toObject();
        o.thresholdPotentialPath=Text(source["path"]);o.thresholdPotentialClass=Text(source["class"]);
    }
    // Includes schema/field sets, all relative times/counts, onset, descriptor,
    // verdict=null, pattern, identities and boolean types. No defaulted or extra
    // field can disappear silently during reconstruction.
    Require(NativeRecognitionTrialJson(o)==object,"native_observation_json_encoding_mismatch");
    ValidateNativeRecognitionObservation(expected,o);return o;
}
NativeRecognitionTrialObservation ParseNativeRecognitionObservation(const std::string& text,
    const NativeRecognitionTrialRequest& expected,std::size_t maximumBytes)
{
    return DecodeNativeRecognitionObservation(ParseStrictNativeJsonObject(text,maximumBytes),expected);
}
}
