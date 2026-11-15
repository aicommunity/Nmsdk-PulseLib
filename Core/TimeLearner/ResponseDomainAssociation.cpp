#include "ResponseDomainAssociation.h"
#include "NumericChecks.h"
#include <algorithm>
#include <map>
#include <set>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
using Detail::After;
using Detail::Finite;
struct Slice {std::size_t first=0,count=0;};
Slice Window(std::span<const ResponseSample> samples,double begin,double end)
{
    std::size_t first=0,last=samples.size();
    while(first<last && After(begin,samples[first].time))++first;
    while(last>first && After(samples[last-1].time,end))--last;
    return {first,last-first};
}
bool Intersects(const DeclaredResponseDomain& domain,const ResponseMaximum& maximum)
{return !After(domain.begin,maximum.plateauEnd) && !After(maximum.time,domain.end);}
}

ResponseDomainResult AnalyzeDeclaredResponseDomains(const ResponseDomainSpec& s,
    const ResponseDomainObservation& o)
{
    ResponseDomainResult result;
    result.runId=o.runId;result.trialId=o.trialId;result.sampleId=o.sampleId;result.patternHash=o.patternHash;
    result.modelRevision=o.modelRevision;result.protocolRevision=o.protocolRevision;
    std::map<std::size_t,std::size_t> channels;
    bool invalidSamples=false,coverage=true,duplicateChannel=false;
    // Preserve independently measured features when attribution cannot proceed.
    for(const auto& channel:o.channels)
    {
        DomainChannelFeatures measured;measured.channel=channel.channel;
        measured.features=ExtractMeasuredResponse(channel.samples,channel.baseline,o.minimumAmplitude);
        if(measured.features.validity==ResponseFeatureValidity::Valid)
        {
            const auto slice=Window(channel.samples,o.begin,o.end);measured.firstSourceSample=slice.first;
            measured.features=ExtractMeasuredResponse(std::span(channel.samples).subspan(slice.first,slice.count),
                channel.baseline,o.minimumAmplitude);
        }
        if(measured.features.validity==ResponseFeatureValidity::InsufficientSamples)coverage=false;
        else if(measured.features.validity!=ResponseFeatureValidity::Valid)invalidSamples=true;
        if(channel.samples.empty() || After(channel.samples.front().time,o.begin) ||
           After(o.end,channel.samples.back().time))coverage=false;
        for(std::size_t i=1;i<channel.samples.size();++i)
            if(!Finite(channel.samples[i].time-channel.samples[i-1].time) ||
               Detail::IntervalAfter(channel.samples[i].time,channel.samples[i-1].time,o.maximumSampleGap))coverage=false;
        if(!channels.emplace(channel.channel,result.channels.size()).second)duplicateChannel=true;
        result.channels.push_back(std::move(measured));
    }
    for(const auto& domain:s.domains)
        result.associations.push_back({domain.channel,domain.stimulusId});
    const auto fail=[&](DomainAssociationStatus status,const char* reason) {
        result.reasons.push_back(reason);
        for(auto& association:result.associations){association.status=status;association.reason=reason;}
        return std::move(result);
    };
    if(s.schemaVersion!=1 || s.modelRevision.empty() || s.protocolRevision.empty() || s.patternHash.empty() ||
       o.modelRevision.empty() || o.protocolRevision.empty() || o.patternHash.empty() ||
       o.runId.empty() || o.trialId.empty() || o.sampleId.empty() ||
       !Finite(o.begin) || !Finite(o.end) || !Finite(o.end-o.begin) || !After(o.end,o.begin) ||
       !Finite(o.maximumSampleGap) || o.maximumSampleGap<=0. ||
       !Finite(o.minimumAmplitude) || o.minimumAmplitude<=0. ||
       !Detail::ResolvableTimeInterval(o.begin,o.end,o.maximumSampleGap) ||
       s.channelIds.empty() || s.stimulusIds.empty() || duplicateChannel)
        return fail(DomainAssociationStatus::InvalidSpecification,"invalid_response_domain_context");
    std::set<std::size_t> expectedChannels(s.channelIds.begin(),s.channelIds.end());
    std::set<std::string> expectedEvents(s.stimulusIds.begin(),s.stimulusIds.end());
    if(expectedChannels.size()!=s.channelIds.size() || expectedEvents.size()!=s.stimulusIds.size() ||
       expectedEvents.count("") || channels.size()!=expectedChannels.size() || o.events.size()!=s.stimulusIds.size())
        return fail(DomainAssociationStatus::InvalidSpecification,"response_domain_inventory_mismatch");
    for(const auto& [id,index]:channels)
        if(!expectedChannels.count(id))return fail(DomainAssociationStatus::InvalidSpecification,"unexpected_response_channel");
    std::map<std::string,double> events;
    for(std::size_t i=0;i<o.events.size();++i)
    {
        const auto& event=o.events[i];
        if(event.id!=s.stimulusIds[i] || !events.emplace(event.id,event.time).second || !Finite(event.time) ||
           After(o.begin,event.time) || After(event.time,o.end) || (i && event.time<o.events[i-1].time))
            return fail(DomainAssociationStatus::InvalidSpecification,"invalid_actual_response_stimulus");
    }
    std::set<std::pair<std::size_t,std::string>> pairs;
    for(const auto& domain:s.domains)
        if(!expectedChannels.count(domain.channel) || !expectedEvents.count(domain.stimulusId) ||
           !pairs.emplace(domain.channel,domain.stimulusId).second || !Finite(domain.begin) || !Finite(domain.end) ||
           !After(domain.end,domain.begin) || After(o.begin,domain.begin) || After(domain.end,o.end))
            return fail(DomainAssociationStatus::InvalidSpecification,"invalid_competing_response_domain");
    for(const auto channel:s.channelIds)for(const auto& event:s.stimulusIds)
        if(!pairs.count({channel,event}))return fail(DomainAssociationStatus::InvalidSpecification,"missing_competing_response_domain");
    if(s.modelRevision!=o.modelRevision || s.protocolRevision!=o.protocolRevision || s.patternHash!=o.patternHash)
        return fail(DomainAssociationStatus::OutsideApplicability,"response_domain_revision_changed");
    if(invalidSamples)return fail(DomainAssociationStatus::InvalidSamples,"invalid_response_samples");
    if(!o.complete || !coverage)return fail(DomainAssociationStatus::Incomplete,"incomplete_response_domain_observation");

    for(std::size_t i=0;i<s.domains.size();++i)
    {
        const auto& domain=s.domains[i];auto& association=result.associations[i];
        association.actualStimulusTime=events.at(domain.stimulusId);
        const auto channelIndex=channels.at(domain.channel);
        const auto& channel=o.channels[channelIndex];const auto& full=result.channels[channelIndex];
        for(std::size_t j=0;j<full.features.positiveMaxima.size();++j)
            if(Intersects(domain,full.features.positiveMaxima[j]))association.candidateMaxima.push_back(j);
        const auto slice=Window(channel.samples,domain.begin,domain.end);
        const auto local=ExtractMeasuredResponse(std::span(channel.samples).subspan(slice.first,slice.count),channel.baseline,o.minimumAmplitude);
        const auto reject=[&](DomainAssociationStatus status,const char* reason) {association.status=status;association.reason=reason;};
        if(local.validity!=ResponseFeatureValidity::Valid)
        {reject(DomainAssociationStatus::Incomplete,"response_domain_has_insufficient_samples");continue;}
        const auto& maximum=*local.globalMaximum;
        if(maximum.amplitude<o.minimumAmplitude)
        {reject(DomainAssociationStatus::NoCandidate,"no_measurable_maximum_in_declared_domain");continue;}
        if(local.globalMaximumClipped)
        {reject(DomainAssociationStatus::Clipped,"declared_domain_clips_maximum");continue;}
        if(local.largestInteriorMaximumCount!=1)
        {reject(DomainAssociationStatus::Ambiguous,"separated_largest_maxima_tie");continue;}
        const auto first=slice.first+maximum.firstSample,last=slice.first+maximum.lastSample;
        std::optional<std::size_t> selected;
        for(const auto index:association.candidateMaxima)
        {
            const auto& feature=full.features.positiveMaxima[index];
            if(full.firstSourceSample+feature.firstSample==first && full.firstSourceSample+feature.lastSample==last)
                selected=index;
        }
        if(!selected)
        {reject(DomainAssociationStatus::Incomplete,"maximum_not_bound_to_full_observation");continue;}
        association.largestMeasuredMaximum=selected;
        association.maximumOffsetLow=maximum.time-events.at(domain.stimulusId);
        association.maximumOffsetHigh=maximum.plateauEnd-events.at(domain.stimulusId);
        if(After(events.at(domain.stimulusId),maximum.time))
        {reject(DomainAssociationStatus::OutsideApplicability,"maximum_precedes_actual_stimulus");continue;}
        for(const auto& other:s.domains)
            if(other.channel==domain.channel && other.stimulusId!=domain.stimulusId &&
               !After(events.at(other.stimulusId),std::min(other.end,maximum.plateauEnd)) && Intersects(other,maximum))
                association.competingStimulusIds.push_back(other.stimulusId);
        std::sort(association.competingStimulusIds.begin(),association.competingStimulusIds.end());
        if(!association.competingStimulusIds.empty())
        {reject(DomainAssociationStatus::Ambiguous,"maximum_shared_by_competing_domains");continue;}
        association.status=DomainAssociationStatus::Hypothesis;
        association.reason="exclusive_maximum_under_declared_domains_not_proven_origin";
        association.selectedMaximum=selected;
    }
    return result;
}

const char* DomainAssociationStatusName(DomainAssociationStatus status)
{
    switch(status){
    case DomainAssociationStatus::Hypothesis:return "hypothesis";
    case DomainAssociationStatus::NoCandidate:return "no_candidate";
    case DomainAssociationStatus::Ambiguous:return "ambiguous";
    case DomainAssociationStatus::Clipped:return "clipped";
    case DomainAssociationStatus::Incomplete:return "incomplete";
    case DomainAssociationStatus::OutsideApplicability:return "outside_applicability";
    case DomainAssociationStatus::InvalidSpecification:return "invalid_specification";
    case DomainAssociationStatus::InvalidSamples:return "invalid_samples";
    }
    return "unknown";
}
}
