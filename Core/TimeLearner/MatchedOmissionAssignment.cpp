#include "MatchedOmissionAssignment.h"
#include "MatchedOmissionValidation.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
using namespace MatchedOmissionDetail;
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
        const auto checked=CheckMatchedOmissionContext(s,p,w);
        const auto count=s.stimulusIds.size();
        const auto& before=checked->Before();const auto& controls=checked->Controls();
        const auto& first=w.fullBefore.observation;const auto& repeat=w.fullAfter.observation;
        const auto frames=first.channels.front().samples.size();
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
