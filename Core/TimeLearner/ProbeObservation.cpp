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
    if((spec.schemaVersion!=1 && spec.schemaVersion!=2 && spec.schemaVersion!=3) ||
       (spec.schemaVersion==1 && spec.stimulusProfile) ||
       (spec.schemaVersion>=2 && (!spec.stimulusProfile || !ValidStimulusProfile(*spec.stimulusProfile))) ||
       (spec.schemaVersion<3 && spec.waveAssignment!=WaveAssignmentPolicy::WindowOnly) ||
       (spec.schemaVersion==3 && spec.waveAssignment!=WaveAssignmentPolicy::OrderedWholeBurst) ||
       (spec.peakSelection!=PeakSelectionPolicy::IsolatedWindow &&
        spec.peakSelection!=PeakSelectionPolicy::AssignedBurstWave) ||
       (spec.peakSelection==PeakSelectionPolicy::AssignedBurstWave && spec.schemaVersion!=3) ||
       spec.runId.empty() || spec.trialId.empty() ||
       spec.sampleId.empty() || spec.patternHash.empty() || spec.modelRevision.empty() ||
       !Finite(spec.begin) || !Finite(spec.end) || !Finite(spec.end-spec.begin) || !After(spec.end,spec.begin) ||
       !Finite(spec.maximumSampleGap) || spec.maximumSampleGap<=0. ||
       !Finite(spec.minimumAmplitude) || spec.minimumAmplitude<=0. ||
       !spec.maximumSamples || spec.baseline.empty() || spec.windows.empty() ||
       spec.expectedStimulusIds.empty())
        throw std::invalid_argument("invalid_probe_specification");
    if(!Detail::ResolvableTimeInterval(spec.begin,spec.end,spec.maximumSampleGap))
        throw std::invalid_argument("insufficient_probe_timestamp_resolution");
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
        if(Detail::IntervalAfter(time,frames.back().time,spec.maximumSampleGap))
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

PeakObservation ProbeRecorder::SelectAssigned(const PeakWindow& window,
    const BurstAssignmentObservation& assignment) const
{
    PeakObservation result{window.inputId,window.stimulusId,assignment.validity};
    if(assignment.validity!=PeakValidity::Valid)return result;
    const auto wave=std::find_if(assignment.waves.begin(),assignment.waves.end(),
        [&](const auto& w){return w.stimulusId==window.stimulusId;});
    if(wave==assignment.waves.end()) {result.validity=PeakValidity::Missing;return result;}
    // The peak and both neighbouring samples must be inside the declared
    // window. Numeric bounds around a peak alone do not prove coverage.
    const auto first=std::find_if(frames.begin(),frames.end(),[&](const auto& f){return f.time==wave->time;});
    const auto last=std::find_if(frames.begin(),frames.end(),[&](const auto& f){return f.time==wave->plateauEnd;});
    if(first==frames.end() || last==frames.end() || first==frames.begin() || last+1==frames.end() ||
       After(window.begin,(first-1)->time) || After((last+1)->time,window.end))
    {result.validity=PeakValidity::Clipped;return result;}
    result.candidateCount=1;result.amplitude=wave->amplitude;result.rawPotential=wave->rawPotential;
    result.time=wave->time;result.plateauEnd=wave->plateauEnd;
    return result;
}

BurstAssignmentObservation ProbeRecorder::AssignBurst(std::size_t channel,bool complete) const
{
    BurstAssignmentObservation result;result.channel=channel;
    if(!complete)return result;
    std::vector<const Frame*> samples;
    for(const auto& frame:frames)
        if(!After(spec.begin,frame.time) && !After(frame.time,spec.end))samples.push_back(&frame);
    if(samples.size()<3)return result;
    const auto value=[&](std::size_t i){return samples[i]->potentials[channel]-spec.baseline[channel];};
    for(std::size_t i=1;i+1<samples.size();)
    {
        std::size_t last=i;
        while(last+1<samples.size() && value(last+1)==value(i))++last;
        if(last+1<samples.size() && value(i)>=spec.minimumAmplitude &&
            value(i)>value(i-1) && value(last)>value(last+1))
            result.waves.push_back({"",value(i),samples[i]->potentials[channel],samples[i]->time,samples[last]->time});
        i=last+1;
    }
    if((value(0)>=spec.minimumAmplitude && value(0)>=value(1)) ||
       (value(samples.size()-1)>=spec.minimumAmplitude && value(samples.size()-1)>=value(samples.size()-2)))
    {result.validity=PeakValidity::Clipped;return result;}
    if(result.waves.size()!=spec.expectedStimulusIds.size())
    {result.validity=result.waves.size()<spec.expectedStimulusIds.size()?PeakValidity::Missing:PeakValidity::Ambiguous;return result;}
    if(stimuli.size()!=spec.expectedStimulusIds.size())return result;
    for(std::size_t i=0;i<stimuli.size();++i)
        if(stimuli[i].id!=spec.expectedStimulusIds[i] || (i && stimuli[i].time<=stimuli[i-1].time) ||
            After(stimuli[i].time,result.waves[i].time))
        {result.validity=PeakValidity::Ambiguous;return result;}
    result.validity=PeakValidity::Valid;
    for(std::size_t i=0;i<result.waves.size();++i)result.waves[i].stimulusId=stimuli[i].id;
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
    if(spec.waveAssignment==WaveAssignmentPolicy::OrderedWholeBurst)
    {
        std::set<std::size_t> channels;
        for(const auto& window:spec.windows)channels.insert(window.channel);
        for(const auto channel:channels)
        {
            auto assignment=AssignBurst(channel,complete);
            if(assignment.validity!=PeakValidity::Valid)
                Reason(std::string("burst_")+PeakValidityName(assignment.validity)+":"+std::to_string(channel));
            result.burstAssignments.push_back(std::move(assignment));
        }
    }
    for(const auto& window:spec.windows)
    {
        auto peak=Measure(window,complete);
        if(spec.waveAssignment==WaveAssignmentPolicy::OrderedWholeBurst)
        {
            const auto& assignment=*std::find_if(result.burstAssignments.begin(),result.burstAssignments.end(),
                [&](const auto& a){return a.channel==window.channel;});
            if(spec.peakSelection==PeakSelectionPolicy::AssignedBurstWave)peak=SelectAssigned(window,assignment);
            else if(assignment.validity!=PeakValidity::Valid)peak=PeakObservation{window.inputId,window.stimulusId,assignment.validity};
            else if(peak.validity==PeakValidity::Valid)
            {
                const auto wave=std::find_if(assignment.waves.begin(),assignment.waves.end(),
                    [&](const auto& w){return w.stimulusId==window.stimulusId;});
                if(wave==assignment.waves.end() || peak.time!=wave->time || peak.plateauEnd!=wave->plateauEnd ||
                   peak.rawPotential!=wave->rawPotential || peak.amplitude!=wave->amplitude)
                {
                    peak=PeakObservation{window.inputId,window.stimulusId,PeakValidity::Ambiguous};
                    Reason("burst_window_assignment_mismatch:"+window.inputId);
                }
            }
        }
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
