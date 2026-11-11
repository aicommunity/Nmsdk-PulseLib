#include "ProbeObservation.h"
#include "MeasuredResponse.h"
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
    if((spec.schemaVersion<1 || spec.schemaVersion>6) ||
       (spec.schemaVersion==1 && spec.stimulusProfile) ||
       (spec.schemaVersion>=2 && (!spec.stimulusProfile || !ValidStimulusProfile(*spec.stimulusProfile))) ||
       (spec.schemaVersion<3 && spec.waveAssignment!=WaveAssignmentPolicy::WindowOnly) ||
       (spec.schemaVersion==3 && spec.waveAssignment!=WaveAssignmentPolicy::OrderedWholeBurst) ||
       (spec.peakSelection!=PeakSelectionPolicy::IsolatedWindow &&
        spec.peakSelection!=PeakSelectionPolicy::AssignedBurstWave &&
        spec.peakSelection!=PeakSelectionPolicy::SingleImpulseMaximum) ||
       (spec.peakSelection==PeakSelectionPolicy::AssignedBurstWave && spec.schemaVersion!=3) ||
       (spec.peakSelection==PeakSelectionPolicy::SingleImpulseMaximum && spec.schemaVersion!=5) ||
       ((spec.schemaVersion==4 || spec.schemaVersion==5) ? (!spec.characterization ||
         spec.characterization->sourceTrainingPatternHash.empty() ||
         !spec.characterization->physicalInputCount ||
         spec.characterization->selectedInputIndex>=spec.characterization->physicalInputCount ||
         spec.expectedStimulusIds.size()!=1 || spec.waveAssignment!=WaveAssignmentPolicy::WindowOnly ||
         spec.peakSelection!=(spec.schemaVersion==4 ? PeakSelectionPolicy::IsolatedWindow :
             PeakSelectionPolicy::SingleImpulseMaximum)) : bool(spec.characterization)) ||
       (spec.schemaVersion==5 && (spec.windows.size()!=1 || spec.recordSpikes)) ||
       ((spec.schemaVersion==6) ? (!spec.stimulusSubset || !ValidStimulusSubset(*spec.stimulusSubset) ||
          spec.characterization || !spec.windows.empty() || spec.recordSpikes ||
          spec.waveAssignment!=WaveAssignmentPolicy::WindowOnly ||
          spec.peakSelection!=PeakSelectionPolicy::IsolatedWindow ||
          spec.expectedStimulusIds.size()!=spec.stimulusSubset->sourceOrdinals.size()) : bool(spec.stimulusSubset)) ||
       spec.runId.empty() || spec.trialId.empty() ||
       spec.sampleId.empty() || spec.patternHash.empty() || spec.modelRevision.empty() ||
       !Finite(spec.begin) || !Finite(spec.end) || !Finite(spec.end-spec.begin) || !After(spec.end,spec.begin) ||
       !Finite(spec.maximumSampleGap) || spec.maximumSampleGap<=0. ||
       !Finite(spec.minimumAmplitude) || spec.minimumAmplitude<=0. ||
       !spec.maximumSamples || spec.baseline.empty() || (spec.schemaVersion!=6 && spec.windows.empty()) ||
       spec.expectedStimulusIds.empty())
        throw std::invalid_argument("invalid_probe_specification");
    if(spec.stimulusSubset)
        for(std::size_t i=0;i<spec.expectedStimulusIds.size();++i)
            if(spec.expectedStimulusIds[i]!="pulse"+std::to_string(spec.stimulusSubset->sourceOrdinals[i]))
                throw std::invalid_argument("stimulus_subset_source_identity_mismatch");
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
    std::vector<ResponseSample> samples;
    for(const auto& frame:frames)
        if(!After(window.begin,frame.time) && !After(frame.time,window.end))
            samples.push_back({frame.time,frame.potentials[window.channel]});
    const auto features=ExtractMeasuredResponse(samples,spec.baseline[window.channel],spec.minimumAmplitude);
    if(features.validity!=ResponseFeatureValidity::Valid)return result;
    const auto& maximum=*features.globalMaximum;
    if(maximum.amplitude<spec.minimumAmplitude)
    { result.validity=PeakValidity::Missing;return result; }
    if(features.globalMaximumClipped)
    { result.validity=PeakValidity::Clipped;return result; }
    result.candidateCount=features.positiveMaxima.size();
    // A single presented impulse establishes attribution, but does not require
    // a unimodal physical response. Equal separated maxima still leave timing
    // ambiguous; a contiguous plateau retains its actual first/last samples.
    if((spec.peakSelection==PeakSelectionPolicy::SingleImpulseMaximum ? features.largestInteriorMaximumCount : result.candidateCount)!=1)
    { result.validity=PeakValidity::Ambiguous;return result; }
    result.validity=PeakValidity::Valid;result.amplitude=maximum.amplitude;
    result.rawPotential=maximum.rawPotential;
    result.time=maximum.time;result.plateauEnd=maximum.plateauEnd;
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
    std::vector<ResponseSample> samples;
    for(const auto& frame:frames)
        if(!After(spec.begin,frame.time) && !After(frame.time,spec.end))
            samples.push_back({frame.time,frame.potentials[channel]});
    const auto features=ExtractMeasuredResponse(samples,spec.baseline[channel],spec.minimumAmplitude);
    if(features.validity!=ResponseFeatureValidity::Valid)return result;
    for(const auto& wave:features.positiveMaxima)
        result.waves.push_back({"",wave.amplitude,wave.rawPotential,wave.time,wave.plateauEnd});
    if(features.boundaryResponseClipped)
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
