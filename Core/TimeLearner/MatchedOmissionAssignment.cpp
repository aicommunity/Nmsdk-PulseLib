#include "MatchedOmissionAssignment.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
using Validity=MatchedOmissionValidity;
struct Failure {Validity validity;const char* reason;};
void Require(bool condition,Validity validity,const char* reason)
{if(!condition)throw Failure{validity,reason};}
bool Finite(double value){return std::isfinite(value);}
double Difference(double a,double b)
{
    const double result=std::fabs(a-b);
    Require(Finite(result),Validity::InvalidContext,"nonfinite_comparison_arithmetic");return result;
}
double Tolerance(double absolute,double relative,double a,double b)
{
    const double result=absolute+relative*std::max(std::fabs(a),std::fabs(b));
    Require(Finite(result),Validity::InvalidContext,"nonfinite_comparison_tolerance");return result;
}
bool Near(double a,double b,double tolerance){return Difference(a,b)<=tolerance;}
bool NonemptyUnique(const std::vector<std::string>& values)
{return !values.empty() && std::set<std::string>(values.begin(),values.end()).size()==values.size() &&
    std::none_of(values.begin(),values.end(),[](const auto& value){return value.empty();});}
std::vector<std::size_t> FullMask(std::size_t size)
{std::vector<std::size_t> result;for(std::size_t i=0;i<size;++i)result.push_back(i);return result;}
void CheckPolicy(const MatchedOmissionPolicy& p,const StimulusProfile& source,std::size_t count)
{
    Require(count>0,Validity::InvalidContext,"invalid_matched_input_count");
    Require(count>1,Validity::Unsupported,"single_input_uses_existing_single_impulse_path");
    Require(!source.generatorClass.empty() && Finite(source.amplitude) && source.amplitude>0. &&
        Finite(source.frequencyHz) && source.frequencyHz>0. && Finite(source.pulseLength) &&
        source.pulseLength>=1./source.frequencyHz && source.frequencyDeviation==0. &&
        !source.useTransitSignal && !source.usePatternOutput,
        Validity::Unsupported,"unsupported_matched_source_profile");
    const double step=1./source.frequencyHz;
    Require(p.schemaVersion==1 && Finite(step) && step>0. && Finite(p.timeComparisonTolerance) &&
        p.timeComparisonTolerance>=0. && p.timeComparisonTolerance<.5*step &&
        p.comparisonRadiusTicks>=1 && p.maximumSamples>=3 && p.comparisonRadiusTicks<p.maximumSamples &&
        p.maximumTotalSamples>=p.maximumSamples && p.maximumNativeProbes>=2,
        Validity::InvalidContext,"invalid_matched_omission_policy");
    for(const double value:{p.baselineAbsolute,p.repeatabilityAbsolute,p.repeatabilityRelative,
                           p.influenceAbsolute,p.influenceRelative})
        Require(Finite(value) && value>=0.,Validity::InvalidContext,"invalid_matched_omission_tolerance");
    Require(count<=p.maximumNativeProbes-2,Validity::BudgetExhausted,"matched_native_probe_budget_exhausted");
    const double width=std::floor(source.pulseLength*source.frequencyHz);
    Require(Finite(width) && width>=1. && width<double(p.maximumSamples),
        Validity::InvalidContext,"unsupported_matched_source_pulse_width");
}
struct CheckedTrace {
    const MatchedOmissionTrace* trace=nullptr;
    std::vector<std::size_t> eventFrames;
};

CheckedTrace CheckTrace(const MatchedOmissionSpecification& s,const MatchedOmissionPolicy& p,
    const MatchedOmissionTrace& t,const std::vector<std::size_t>& mask,std::set<std::string>& trials,
    std::size_t& totalSamples)
{
    const auto& o=t.observation;const auto& context=t.prepared;
    Require(t.nativeCompleted && o.complete,Validity::Incomplete,"native_trace_not_completed");
    Require(t.fullMaskRestored,Validity::InvalidContext,"full_presentation_not_restored");
    Require(t.groupId==s.groupId && t.sourcePatternHash==s.patternHash && t.resetBinding==s.resetBinding &&
        t.modelInstanceBinding==s.modelInstanceBinding && t.currentBaseRevision==s.baseModelRevision &&
        context.baseModelRevision==s.baseModelRevision && context.effectiveModelRevision==s.effectiveModelRevision &&
        context.modelEpoch==s.modelEpoch && context.routing==s.routing && context.inputId==s.inputId &&
        context.protocolRevision==s.protocolRevision && o.modelRevision==s.effectiveModelRevision &&
        o.protocolRevision==s.protocolRevision && o.runId==s.runId && o.sampleId==s.sampleId,
        Validity::InvalidContext,"matched_omission_context_changed");
    Require(s.routing==ProbeRouting::BranchSoloBurst ? context.selectedInputIndex==s.assignedInput :
        !context.selectedInputIndex,Validity::InvalidContext,"matched_omission_selector_changed");
    Require(!o.trialId.empty() && trials.insert(o.trialId).second,Validity::InvalidContext,"reused_matched_trial_id");
    Require(t.stimulusProfile==s.stimulusProfile && t.physicalChannelPaths==s.physicalChannelPaths &&
        t.presentedOrdinals==mask && !o.patternHash.empty(),Validity::InvalidContext,"matched_omission_declaration_changed");
    if(mask.size()==s.stimulusIds.size())
        Require(o.patternHash==s.patternHash,Validity::InvalidContext,"full_pattern_hash_changed");
    const double step=1./s.stimulusProfile.frequencyHz;
    Require(Finite(o.begin) && Finite(o.end) && o.end>o.begin && Near(o.end-o.begin,s.duration,p.timeComparisonTolerance) &&
        o.maximumSampleGap==step && o.minimumAmplitude==s.minimumAmplitude,
        Validity::InvalidContext,"matched_trace_window_changed");
    Require(o.channels.size()==s.physicalChannelPaths.size(),Validity::InvalidContext,"matched_channel_inventory_changed");
    const auto count=o.channels.front().samples.size();
    Require(count>=3,Validity::Incomplete,"insufficient_matched_samples");
    Require(count<=p.maximumSamples && totalSamples<=p.maximumTotalSamples &&
        count<=p.maximumTotalSamples-totalSamples,Validity::BudgetExhausted,"matched_sample_budget_exhausted");
    totalSamples+=count;
    Require(t.stimulusSamples.size()==count,Validity::Incomplete,"matched_stimulus_sample_count_changed");
    for(std::size_t channel=0;channel<o.channels.size();++channel)
    {
        const auto& c=o.channels[channel];
        Require(c.channel==channel && Finite(c.baseline),Validity::InvalidContext,"invalid_matched_channel_or_baseline");
        Require(c.samples.size()==count,Validity::Incomplete,"matched_channel_sample_count_changed");
        for(std::size_t i=0;i<count;++i)
        {
            const auto& sample=c.samples[i];
            Require(Finite(sample.time) && Finite(sample.rawPotential) && Finite(sample.rawPotential-c.baseline),
                Validity::InvalidContext,"nonfinite_matched_sample");
            Require(sample.time==o.channels.front().samples[i].time,Validity::InvalidContext,"matched_channel_clock_changed");
            Require(!i || sample.time>c.samples[i-1].time,Validity::Incomplete,"nonmonotonic_matched_grid");
            Require(Near(sample.time-o.begin,double(i)*step,p.timeComparisonTolerance),
                Validity::Incomplete,"matched_sample_grid_changed");
        }
    }
    const auto& grid=o.channels.front().samples;
    Require(!Detail::After(o.end,grid.back().time) && Detail::After(o.end,grid[count-2].time),
        Validity::Incomplete,"matched_window_coverage_changed");
    Require(o.events.size()==mask.size(),Validity::Incomplete,"matched_actual_event_inventory_changed");
    CheckedTrace result{&t,{}};
    for(std::size_t event=0;event<mask.size();++event)
    {
        const auto& e=o.events[event];
        Require(e.id==s.stimulusIds[mask[event]] && Finite(e.time) &&
            (!event || e.time>o.events[event-1].time),Validity::InvalidContext,"matched_actual_event_identity_changed");
        std::optional<std::size_t> frame;
        for(std::size_t i=0;i<count;++i)if(Near(e.time,grid[i].time,p.timeComparisonTolerance))
        {Require(!frame,Validity::InvalidContext,"ambiguous_actual_event_frame");frame=i;}
        Require(bool(frame),Validity::InvalidContext,"actual_event_not_on_matched_grid");
        result.eventFrames.push_back(*frame);
    }
    const double widthValue=std::floor(s.stimulusProfile.pulseLength*s.stimulusProfile.frequencyHz);
    Require(Finite(widthValue) && widthValue>=1. && widthValue<=double(count),
        Validity::InvalidContext,"unsupported_matched_source_pulse_width");
    const auto width=static_cast<std::size_t>(widthValue);
    std::vector<bool> active(count,false);
    for(const auto frame:result.eventFrames)
    {
        Require(width<count-frame,Validity::Incomplete,"matched_source_pulse_clipped");
        for(std::size_t i=frame;i<frame+width;++i)
        {Require(!active[i],Validity::InvalidContext,"overlapping_matched_source_pulses");active[i]=true;}
    }
    for(std::size_t i=0;i<count;++i)
        Require(Finite(t.stimulusSamples[i]) && t.stimulusSamples[i]==(active[i]?s.stimulusProfile.amplitude:0.),
            Validity::InvalidContext,"matched_source_waveform_changed");
    return result;
}

void MatchTrace(const MatchedOmissionSpecification& s,const MatchedOmissionPolicy& p,
    const CheckedTrace& full,const CheckedTrace& other)
{
    const auto& a=full.trace->observation;const auto& b=other.trace->observation;
    Require(a.channels.front().samples.size()==b.channels.front().samples.size(),
        Validity::Incomplete,"matched_frame_count_changed");
    for(std::size_t channel=0;channel<a.channels.size();++channel)
        Require(Near(a.channels[channel].baseline,b.channels[channel].baseline,p.baselineAbsolute),
            Validity::InvalidContext,"matched_baseline_changed");
    const auto& mask=other.trace->presentedOrdinals;
    for(std::size_t i=0;i<mask.size();++i)
    {
        const auto ordinal=mask[i];
        Require(full.eventFrames[ordinal]==other.eventFrames[i] &&
            Near(a.events[ordinal].time-a.begin,b.events[i].time-b.begin,p.timeComparisonTolerance),
            Validity::InvalidContext,"retained_actual_stimulus_time_changed");
    }
    (void)s;
}

std::vector<std::pair<ResponseMaximum,bool>> FullMaxima(const ResponseDomainChannel& channel,
    double minimumAmplitude)
{
    const auto features=ExtractMeasuredResponse(channel.samples,channel.baseline,minimumAmplitude);
    Require(features.validity==ResponseFeatureValidity::Valid,Validity::Incomplete,"invalid_full_response_features");
    std::vector<std::pair<ResponseMaximum,bool>> result;
    for(const auto& maximum:features.positiveMaxima)result.push_back({maximum,false});
    const auto& samples=channel.samples;const auto size=samples.size();
    const auto value=[&](std::size_t i){return samples[i].rawPotential-channel.baseline;};
    const auto add=[&](std::size_t first,std::size_t last) {
        result.push_back({ResponseMaximum{first,last,value(first),samples[first].rawPotential,
            samples[first].time,samples[last].time},true});
    };
    if(value(0)>=minimumAmplitude && value(0)>=value(1))
    {std::size_t last=0;while(last+1<size && value(last+1)==value(0))++last;add(0,last);}
    if(value(size-1)>=minimumAmplitude && value(size-1)>=value(size-2))
    {
        std::size_t first=size-1;while(first && value(first-1)==value(size-1))--first;
        if(first)add(first,size-1); // An all-window plateau was already added.
    }
    std::sort(result.begin(),result.end(),[](const auto& a,const auto& b){return a.first.firstSample<b.first.firstSample;});
    return result;
}
}

MatchedOmissionResult SelectMatchedOmissionMaximum(const MatchedOmissionSpecification& s,
    const MatchedOmissionPolicy& p,const MatchedOmissionWitness& w)
{
    MatchedOmissionResult result;
    try
    {
        const auto count=s.stimulusIds.size();const auto& source=s.stimulusProfile;
        Require(s.schemaVersion==1 && p.schemaVersion==1 && NonemptyUnique(s.stimulusIds) &&
            NonemptyUnique(s.physicalChannelPaths) && !s.runId.empty() && !s.groupId.empty() &&
            !s.sampleId.empty() && !s.protocolRevision.empty() && !s.patternHash.empty() && !s.inputId.empty() &&
            !s.baseModelRevision.empty() && !s.effectiveModelRevision.empty() && !s.resetBinding.empty() &&
            !s.modelInstanceBinding.empty() && s.assignedInput<count && s.channel<s.physicalChannelPaths.size() &&
            Finite(s.duration) && s.duration>0. && Finite(s.minimumAmplitude) && s.minimumAmplitude>0.,
            Validity::InvalidContext,"invalid_matched_omission_specification");
        Require(s.routing==ProbeRouting::ClassicBurst || s.routing==ProbeRouting::BranchSoloBurst,
            Validity::InvalidContext,"invalid_matched_routing");
        Require(s.routing==ProbeRouting::ClassicBurst ? s.physicalChannelPaths.size()==count && s.channel==s.assignedInput :
            s.physicalChannelPaths.size()==1 && s.channel==0,Validity::InvalidContext,"matched_physical_channel_mapping_changed");
        CheckPolicy(p,source,count);
        Require(count<=p.maximumNativeProbes-2 && w.omitEachInput.size()<=p.maximumNativeProbes-2,
            Validity::BudgetExhausted,"matched_native_probe_budget_exhausted");
        Require(w.omitEachInput.size()>=count,Validity::Incomplete,"missing_omission_controls");
        Require(w.omitEachInput.size()==count,Validity::InvalidContext,"unexpected_omission_control_count");
        std::vector<const MatchedOmissionTrace*> omitted(count,nullptr);
        for(const auto& trace:w.omitEachInput)
        {
            const auto& mask=trace.presentedOrdinals;
            Require(mask.size()==count-1 && std::is_sorted(mask.begin(),mask.end()) &&
                std::adjacent_find(mask.begin(),mask.end())==mask.end() && mask.back()<count,
                Validity::InvalidContext,"invalid_omission_mask");
            std::size_t missing=0;while(missing<mask.size() && mask[missing]==missing)++missing;
            Require(!omitted[missing],Validity::InvalidContext,"duplicate_omission_control");omitted[missing]=&trace;
        }
        std::set<std::string> trials;std::size_t totalSamples=0;const auto fullMask=FullMask(count);
        const auto before=CheckTrace(s,p,w.fullBefore,fullMask,trials,totalSamples);
        std::vector<CheckedTrace> controls;
        for(std::size_t input=0;input<count;++input)
        {
            Require(omitted[input]!=nullptr,Validity::Incomplete,"missing_omission_control");
            auto mask=fullMask;mask.erase(mask.begin()+input);
            controls.push_back(CheckTrace(s,p,*omitted[input],mask,trials,totalSamples));
        }
        const auto after=CheckTrace(s,p,w.fullAfter,fullMask,trials,totalSamples);
        MatchTrace(s,p,before,after);for(const auto& control:controls)MatchTrace(s,p,before,control);
        std::vector<const MatchedOmissionTrace*> sequence;
        for(const auto& control:controls)sequence.push_back(control.trace);
        std::sort(sequence.begin(),sequence.end(),[](const auto* a,const auto* b){return a->observation.begin<b->observation.begin;});
        double previousEnd=w.fullBefore.observation.channels.front().samples.back().time;
        for(const auto* trace:sequence)
        {
            Require(previousEnd<=trace->observation.begin,
                Validity::InvalidContext,"matched_native_trials_overlap");
            previousEnd=trace->observation.channels.front().samples.back().time;
        }
        Require(previousEnd<=w.fullAfter.observation.begin,
            Validity::InvalidContext,"full_repeat_does_not_follow_controls");
        const auto& first=w.fullBefore.observation;const auto& repeat=w.fullAfter.observation;
        const auto frames=first.channels.front().samples.size();
        for(std::size_t channel=0;channel<first.channels.size();++channel)for(std::size_t i=0;i<frames;++i)
        {
            const double a=first.channels[channel].samples[i].rawPotential-first.channels[channel].baseline;
            const double b=repeat.channels[channel].samples[i].rawPotential-repeat.channels[channel].baseline;
            Require(Difference(a,b)<=Tolerance(p.repeatabilityAbsolute,p.repeatabilityRelative,a,b),
                Validity::NonRepeatable,"full_response_not_repeatable");
        }
        const auto& channel=first.channels[s.channel];const auto& repeated=repeat.channels[s.channel];
        const auto support=[&](const ResponseMaximum& maximum,bool clipped) {
            FullMaximumSupport evidence;evidence.maximum=maximum;evidence.clipped=clipped;
            evidence.precedesAssignedStimulus=maximum.firstSample<before.eventFrames[s.assignedInput];
            evidence.comparisonFirstSample=maximum.firstSample>p.comparisonRadiusTicks?
                maximum.firstSample-p.comparisonRadiusTicks:0;
            evidence.comparisonLastSample=maximum.lastSample+std::min(p.comparisonRadiusTicks,frames-1-maximum.lastSample);
            for(std::size_t input=0;input<count;++input)
            {
                bool definitelySupported=false,possiblySupported=false;
                const auto& control=controls[input].trace->observation.channels[s.channel];
                for(std::size_t i=evidence.comparisonFirstSample;i<=evidence.comparisonLastSample;++i)
                {
                    const double a=channel.samples[i].rawPotential-channel.baseline;
                    const double b=repeated.samples[i].rawPotential-repeated.baseline;
                    const double c=control.samples[i].rawPotential-control.baseline;
                    // The full-repeat uncertainty is an interval, not an
                    // average that could hide a threshold-crossing effect.
                    const double low=std::min(a,b),high=std::max(a,b);
                    const double minimum=c<low?Difference(c,low):c>high?Difference(c,high):0.;
                    const double maximumDifference=std::max(Difference(a,c),Difference(b,c));
                    const double threshold=Tolerance(p.influenceAbsolute,p.influenceRelative,
                        std::max(std::fabs(a),std::fabs(b)),c);
                    definitelySupported=definitelySupported || minimum>threshold;
                    possiblySupported=possiblySupported || maximumDifference>threshold;
                }
                const auto influence=definitelySupported?OmissionInfluence::Supported:
                    possiblySupported?OmissionInfluence::Unknown:OmissionInfluence::NotSupported;
                evidence.influenceByInput.push_back(influence);
                if(influence==OmissionInfluence::Supported)evidence.supportedInputs.push_back(input);
                evidence.unknown=evidence.unknown || influence==OmissionInfluence::Unknown;
            }
            return evidence;
        };
        const auto choose=[&](const std::vector<FullMaximumSupport>& maxima) {
            std::optional<std::size_t> best;bool tied=false;
            for(std::size_t i=0;i<maxima.size();++i)
            {
                const auto& peak=maxima[i];
                if(peak.clipped || peak.precedesAssignedStimulus || peak.unknown ||
                    peak.supportedInputs!=std::vector<std::size_t>{s.assignedInput})continue;
                if(!best || peak.maximum.amplitude>maxima[*best].maximum.amplitude){best=i;tied=false;}
                else if(peak.maximum.amplitude==maxima[*best].maximum.amplitude)tied=true;
            }
            for(const auto& peak:maxima)
            {
                if(peak.influenceByInput[s.assignedInput]==OmissionInfluence::NotSupported)continue;
                if(peak.precedesAssignedStimulus)
                    throw Failure{Validity::Ambiguous,"assigned_omission_changes_pre_stimulus_maximum"};
                if(peak.clipped)throw Failure{Validity::Clipped,"possibly_assigned_maximum_is_clipped"};
                if((peak.unknown || peak.supportedInputs.size()!=1) &&
                    (!best || peak.maximum.amplitude>=maxima[*best].maximum.amplitude))
                    throw Failure{Validity::Ambiguous,"highest_assigned_maximum_has_shared_or_unknown_support"};
            }
            Require(bool(best),Validity::Missing,"no_exclusive_assigned_full_maximum");
            Require(!tied,Validity::Ambiguous,"separated_assigned_maxima_tie");return *best;
        };
        for(const auto& [maximum,clipped]:FullMaxima(channel,s.minimumAmplitude))
            result.maxima.push_back(support(maximum,clipped));
        const auto best=choose(result.maxima);
        // Pointwise tolerance does not resolve a swapped highest peak or a
        // shifted plateau. The same measured sample range must win the repeat.
        std::vector<FullMaximumSupport> repeatedMaxima;
        for(const auto& [maximum,clipped]:FullMaxima(repeated,s.minimumAmplitude))
            repeatedMaxima.push_back(support(maximum,clipped));
        std::size_t repeatedBest=0;
        try{repeatedBest=choose(repeatedMaxima);}
        catch(const Failure&){throw Failure{Validity::NonRepeatable,"assigned_maximum_not_repeated"};}
        Require(result.maxima[best].maximum.firstSample==repeatedMaxima[repeatedBest].maximum.firstSample &&
            result.maxima[best].maximum.lastSample==repeatedMaxima[repeatedBest].maximum.lastSample,
            Validity::NonRepeatable,"assigned_maximum_sample_range_not_repeated");
        result.validity=Validity::Valid;result.selectedFullMaximum=best;result.measuredMaximum=result.maxima[best].maximum;
    }
    catch(const Failure& failure){result.validity=failure.validity;result.reasons.push_back(failure.reason);}
    return result;
}

MatchedOmissionResult ValidateMatchedOmissionPolicy(const MatchedOmissionPolicy& p,
    const StimulusProfile& source,std::size_t inputCount)
{
    MatchedOmissionResult result;
    try{CheckPolicy(p,source,inputCount);result.validity=Validity::Valid;}
    catch(const Failure& failure){result.validity=failure.validity;result.reasons.push_back(failure.reason);}
    return result;
}

const char* MatchedOmissionValidityName(MatchedOmissionValidity validity)
{
    switch(validity){
    case Validity::Valid:return "valid";case Validity::Missing:return "missing";
    case Validity::Ambiguous:return "ambiguous";case Validity::Clipped:return "clipped";
    case Validity::Incomplete:return "incomplete";case Validity::InvalidContext:return "invalid_context";
    case Validity::NonRepeatable:return "non_repeatable";case Validity::BudgetExhausted:return "budget_exhausted";
    case Validity::Unsupported:return "unsupported";}
    return "unknown";
}
const char* OmissionInfluenceName(OmissionInfluence influence)
{
    switch(influence){case OmissionInfluence::Supported:return "supported";
    case OmissionInfluence::NotSupported:return "not_supported";case OmissionInfluence::Unknown:return "unknown";}
    return "unknown";
}
}
