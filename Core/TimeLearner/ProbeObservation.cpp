#include "ProbeObservation.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
using Detail::Finite;
using Detail::After;
bool ValidStimulusProfile(const StimulusProfile& p)
{
    return !p.generatorClass.empty() && Finite(p.amplitude) && Finite(p.pulseLength) && p.pulseLength>0. &&
        Finite(p.frequencyHz) && p.frequencyHz>0. && Finite(p.frequencyDeviation) && p.frequencyDeviation>=0.;
}

ProbeRecorder::ProbeRecorder(ProbeSpec specification):spec(std::move(specification))
{
    if((spec.schemaVersion!=1 && spec.schemaVersion!=2) ||
       (spec.schemaVersion==1 && spec.stimulusProfile) ||
       (spec.schemaVersion==2 && (!spec.stimulusProfile || !ValidStimulusProfile(*spec.stimulusProfile))) ||
       spec.runId.empty() || spec.trialId.empty() ||
       spec.sampleId.empty() || spec.patternHash.empty() || spec.modelRevision.empty() ||
       !Finite(spec.begin) || !Finite(spec.end) || !Finite(spec.end-spec.begin) || !After(spec.end,spec.begin) ||
       !Finite(spec.maximumSampleGap) || spec.maximumSampleGap<=0. ||
       !Finite(spec.minimumAmplitude) || spec.minimumAmplitude<=0. ||
       !spec.maximumSamples || spec.baseline.empty() || spec.windows.empty() ||
       spec.expectedStimulusIds.empty())
        throw std::invalid_argument("invalid_probe_specification");
    for(double baseline:spec.baseline)
        if(!Finite(baseline)) throw std::invalid_argument("nonfinite_probe_baseline");
    std::set<std::string> inputs,events;
    for(const auto& id:spec.expectedStimulusIds)
        if(id.empty() || !events.insert(id).second)
            throw std::invalid_argument("invalid_probe_stimulus_ids");
    for(const auto& window:spec.windows)
        if(window.inputId.empty() || !inputs.insert(window.inputId).second ||
           !events.count(window.stimulusId) ||
           window.channel>=spec.baseline.size() || !Finite(window.begin) || !Finite(window.end) ||
           !After(window.end,window.begin) || After(spec.begin,window.begin) || After(window.end,spec.end))
            throw std::invalid_argument("invalid_probe_peak_window");
}

void ProbeRecorder::Reason(const std::string& reason)
{
    if(std::find(reasons.begin(),reasons.end(),reason)==reasons.end()) reasons.push_back(reason);
}

void ProbeRecorder::Observe(double time,const std::vector<double>& potentials,
                            const std::string& currentModelRevision)
{
    if(closed) throw std::logic_error("probe_already_closed");
    if(currentModelRevision!=spec.modelRevision) Reason("model_revision_changed");
    if(!Finite(time) || potentials.size()!=spec.baseline.size())
    { Reason("invalid_sample"); coverageBroken=true; return; }
    for(std::size_t i=0;i<potentials.size();++i)
        if(!Finite(potentials[i]) || !Finite(potentials[i]-spec.baseline[i]))
        { Reason("nonfinite_sample"); coverageBroken=true; return; }
    if(!frames.empty())
    {
        if(time<=frames.back().time)
        { Reason("nonmonotonic_sample_time"); coverageBroken=true; return; }
        const double interval=time-frames.back().time;
        if(!Finite(interval))
        { Reason("nonfinite_sample_interval"); coverageBroken=true; return; }
        if(After(interval,spec.maximumSampleGap))
        { Reason("sample_gap"); coverageBroken=true; }
    }
    if(frames.size()>=spec.maximumSamples)
    { Reason("sample_budget_exhausted"); coverageBroken=true; return; }
    frames.push_back({time,potentials});
}

void ProbeRecorder::RecordStimulus(const std::string& id,double time)
{
    if(closed) throw std::logic_error("probe_already_closed");
    if(!Finite(time) || After(spec.begin,time) || After(time,spec.end))
    { Reason("invalid_stimulus_time"); return; }
    if(std::find(spec.expectedStimulusIds.begin(),spec.expectedStimulusIds.end(),id)==spec.expectedStimulusIds.end())
    { Reason("unexpected_stimulus_id"); return; }
    if(std::any_of(stimuli.begin(),stimuli.end(),[&](const auto& event){return event.id==id;}))
    { Reason("duplicate_stimulus_id"); return; }
    if(!stimuli.empty() && time<stimuli.back().time)
    { Reason("nonmonotonic_stimulus_time"); return; }
    stimuli.push_back({id,time});
}

void ProbeRecorder::RecordSpike(std::size_t channel,double time)
{
    if(closed) throw std::logic_error("probe_already_closed");
    if(!spec.recordSpikes || channel>=spec.baseline.size())
    { Reason("unexpected_spike_channel"); return; }
    if(spikes.size()/spec.baseline.size()>=spec.maximumSamples)
    { Reason("spike_budget_exhausted"); return; }
    if(!Finite(time) || After(spec.begin,time) || After(time,spec.end))
    { Reason("invalid_spike_time"); return; }
    if(!spikes.empty() && time<spikes.back().time)
    { Reason("nonmonotonic_spike_time"); return; }
    if(std::any_of(spikes.begin(),spikes.end(),[&](const auto& spike){return spike.channel==channel && spike.time==time;}))
    { Reason("duplicate_spike"); return; }
    spikes.push_back({channel,time});
}

PeakObservation ProbeRecorder::Measure(const PeakWindow& window,bool complete) const
{
    PeakObservation result;result.inputId=window.inputId;result.stimulusId=window.stimulusId;
    if(!complete) return result;
    std::vector<const Frame*> samples;
    for(const auto& frame:frames)
        if(!After(window.begin,frame.time) && !After(frame.time,window.end)) samples.push_back(&frame);
    if(samples.size()<3) return result;
    const auto value=[&](std::size_t i){return samples[i]->potentials[window.channel]-spec.baseline[window.channel];};
    std::size_t maximum=0;
    for(std::size_t i=1;i<samples.size();++i) if(value(i)>value(maximum)) maximum=i;
    if(!Finite(value(maximum))) return result;
    if(value(maximum)<spec.minimumAmplitude)
    { result.validity=PeakValidity::Missing;return result; }
    std::size_t lastMaximum=maximum;
    while(lastMaximum+1<samples.size() && value(lastMaximum+1)==value(maximum)) ++lastMaximum;
    if(maximum==0 || lastMaximum+1==samples.size())
    { result.validity=PeakValidity::Clipped;return result; }
    for(std::size_t i=1;i+1<samples.size();)
    {
        std::size_t end=i;
        while(end+1<samples.size() && value(end+1)==value(i)) ++end;
        if(end+1<samples.size() && value(i)>=spec.minimumAmplitude &&
           value(i)>value(i-1) && value(end)>value(end+1)) ++result.candidateCount;
        i=end+1;
    }
    if(result.candidateCount!=1)
    { result.validity=PeakValidity::Ambiguous;return result; }
    result.validity=PeakValidity::Valid;result.amplitude=value(maximum);
    result.rawPotential=samples[maximum]->potentials[window.channel];
    result.time=samples[maximum]->time;result.plateauEnd=samples[lastMaximum]->time;
    return result;
}

ProbeObservation ProbeRecorder::Close(double time,const std::string& currentModelRevision,
                                      const std::string& endReason)
{
    if(closed) throw std::logic_error("probe_already_closed");
    closed=true;
    if(endReason.empty()) Reason("missing_end_reason");
    if(currentModelRevision!=spec.modelRevision) Reason("model_revision_changed");
    const bool complete=!coverageBroken && !frames.empty() && Finite(time) &&
        !After(spec.end,time) && !After(frames.front().time,spec.begin) &&
        !After(spec.end,frames.back().time) && !After(frames.back().time,time);
    if(!complete) Reason("incomplete_observation");
    if(stimuli.size()!=spec.expectedStimulusIds.size()) Reason("missing_stimulus");
    ProbeObservation result;result.schemaVersion=spec.schemaVersion;result.specification=spec;result.complete=complete;
    result.endReason=endReason;result.sampleCount=frames.size();result.stimuli=stimuli;result.spikes=spikes;
    for(const auto& window:spec.windows)
    {
        auto peak=Measure(window,complete);
        if(peak.validity==PeakValidity::Valid)
        {
            const auto event=std::find_if(stimuli.begin(),stimuli.end(),[&](const auto& stimulus){return stimulus.id==window.stimulusId;});
            if(event!=stimuli.end())
            {
                peak.delayFromStimulus=peak.time.value()-event->time;
                if(After(event->time,peak.time.value())) Reason("peak_precedes_stimulus:"+peak.inputId);
            }
        }
        if(peak.validity!=PeakValidity::Valid)
            Reason(std::string("peak_")+PeakValidityName(peak.validity)+":"+peak.inputId);
        result.peaks.push_back(std::move(peak));
    }
    result.reasons=reasons;result.valid=result.complete && reasons.empty();
    return result;
}

const char* PeakValidityName(PeakValidity validity)
{
    switch(validity) {
    case PeakValidity::Valid:return "valid";
    case PeakValidity::Missing:return "missing";
    case PeakValidity::Ambiguous:return "ambiguous";
    case PeakValidity::Clipped:return "clipped";
    case PeakValidity::Incomplete:return "incomplete";
    }
    return "unknown";
}
}
