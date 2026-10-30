#include "NativeRecognitionObservationValidation.h"
#include "NativeTrainingBindings.h"
#include "ModelIdentity.h"
#include "NumericChecks.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
bool EqualTime(double a,double b){return !Detail::After(a,b) && !Detail::After(b,a);}
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
}
