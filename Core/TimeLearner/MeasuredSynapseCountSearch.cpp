#include "MeasuredSynapseCountSearch.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>

namespace NMSDK::TimeLearner {
std::vector<MeasuredSynapseCountBracket> MeasuredAdjacentSynapseCountBrackets(
    std::vector<MeasuredSynapseCountPoint> points,int minimum,int maximum,double tolerance)
{
    if(minimum<1 || maximum<minimum || !std::isfinite(tolerance) || tolerance<0.)
        throw std::invalid_argument("invalid_measured_synapse_count_domain");
    for(const auto& p:points)
        if(p.synapses<1 || !std::isfinite(p.amplitudeResidual) || !p.withinTiming)return {};
    points.erase(std::remove_if(points.begin(),points.end(),[&](const auto& p) {
        return p.synapses<minimum || p.synapses>maximum;
    }),points.end());
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b){return a.synapses<b.synapses;});
    std::vector<MeasuredSynapseCountPoint> unique;
    for(std::size_t begin=0;begin<points.size();)
    {
        auto representative=points[begin];std::size_t end=begin+1;
        double low=representative.amplitudeResidual,high=low;
        while(end<points.size() && points[end].synapses==representative.synapses)
        {
            const auto& p=points[end++];low=std::min(low,p.amplitudeResidual);high=std::max(high,p.amplitudeResidual);
            if(std::fabs(p.amplitudeResidual)>std::fabs(representative.amplitudeResidual))representative=p;
        }
        if(!std::isfinite(high-low) || high-low>tolerance ||
            std::fabs(representative.amplitudeResidual)<=tolerance)return {};
        unique.push_back(representative);begin=end;
    }
    std::vector<MeasuredSynapseCountBracket> result;
    for(std::size_t i=1;i<unique.size();++i)
    {
        const auto& a=unique[i-1];const auto& b=unique[i];
        if(b.synapses-a.synapses==1 && ((a.amplitudeResidual<0.)!=(b.amplitudeResidual<0.)))
            result.push_back({a.synapses,b.synapses});
    }
    return result;
}

std::optional<int> ProposeMeasuredSynapseCount(std::vector<MeasuredSynapseCountPoint> points,
    int minimum,int maximum,double tolerance)
{
    if(minimum<1 || maximum<minimum || !std::isfinite(tolerance) || tolerance<0.)
        throw std::invalid_argument("invalid_measured_synapse_count_domain");
    for(const auto& p:points)
        if(p.synapses<1 || !std::isfinite(p.amplitudeResidual) || !p.withinTiming)return {};
    points.erase(std::remove_if(points.begin(),points.end(),[&](const auto& p) {
        return p.synapses<minimum || p.synapses>maximum;
    }),points.end());
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b) {
        return a.synapses<b.synapses;
    });
    std::vector<MeasuredSynapseCountPoint> unique;
    for(std::size_t begin=0;begin<points.size();)
    {
        auto representative=points[begin];std::size_t end=begin+1;
        double low=representative.amplitudeResidual,high=low;
        while(end<points.size() && points[end].synapses==representative.synapses)
        {
            const auto& p=points[end++];low=std::min(low,p.amplitudeResidual);high=std::max(high,p.amplitudeResidual);
            if(std::fabs(p.amplitudeResidual)>std::fabs(representative.amplitudeResidual))representative=p;
        }
        if(!std::isfinite(high-low) || high-low>tolerance)return {};
        if(std::fabs(representative.amplitudeResidual)<=tolerance)return {};
        unique.push_back(representative);begin=end;
    }
    if(unique.size()<2)return {};
    bool increasing=true,decreasing=true;
    for(std::size_t i=1;i<unique.size();++i)
    {
        increasing=increasing && unique[i].amplitudeResidual>=unique[i-1].amplitudeResidual;
        decreasing=decreasing && unique[i].amplitudeResidual<=unique[i-1].amplitudeResidual;
    }
    if(!increasing && !decreasing)return {};
    const auto proposal=[&](const auto& a,const auto& b,bool bracket)->std::optional<int> {
        const double scale=std::max(std::fabs(a.amplitudeResidual),std::fabs(b.amplitudeResidual));
        if(scale==0.)return {};
        const double left=a.amplitudeResidual/scale,right=b.amplitudeResidual/scale,delta=right-left;
        if(delta==0. || std::fabs(delta)<=tolerance/scale)return {};
        double fraction=-left/delta;
        if(!std::isfinite(fraction))return {};
        if(bracket)fraction=std::clamp(fraction,.25,.75);
        const double root=double(a.synapses)+fraction*(double(b.synapses)-a.synapses);
        if(!std::isfinite(root))return {};
        const int low=bracket?a.synapses+1:minimum,high=bracket?b.synapses-1:maximum;
        if(low>high)return {};
        const int next=int(std::round(std::clamp(root,double(low),double(high))));
        if(std::any_of(unique.begin(),unique.end(),[&](const auto& p){return p.synapses==next;}))return {};
        return next;
    };
    std::optional<int> result;std::tuple<double,int,int> best;
    for(std::size_t i=1;i<unique.size();++i)
    {
        const auto& a=unique[i-1];const auto& b=unique[i];
        if((a.amplitudeResidual<0.)==(b.amplitudeResidual<0.))continue;
        if(const auto next=proposal(a,b,true))
        {
            const auto rank=std::tuple{std::min(std::fabs(a.amplitudeResidual),std::fabs(b.amplitudeResidual)),
                b.synapses-a.synapses,a.synapses};
            if(!result || rank<best){result=next;best=rank;}
        }
    }
    if(result)return result;
    // An adjacent opposite-sign integer pair is quantization evidence, not
    // permission to extrapolate across its measured root or certify Exact.
    const bool negative=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.amplitudeResidual<0.;});
    const bool positive=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.amplitudeResidual>0.;});
    if(!negative && !positive)return {};
    const auto closest=std::min_element(unique.begin(),unique.end(),[](const auto& a,const auto& b) {
        return std::fabs(a.amplitudeResidual)<std::fabs(b.amplitudeResidual);
    });
    const auto index=std::size_t(closest-unique.begin());
    return index==0?proposal(unique[0],unique[1],false):proposal(unique[index-1],unique[index],false);
}
}
