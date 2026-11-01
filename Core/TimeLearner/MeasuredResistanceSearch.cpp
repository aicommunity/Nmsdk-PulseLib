#include "MeasuredResistanceSearch.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>

namespace NMSDK::TimeLearner {
std::optional<double> ProposeMeasuredResistanceRefinement(
    std::vector<MeasuredResistancePoint> points,double low,double high,double tolerance)
{
    if(!std::isfinite(low) || !std::isfinite(high) || low<=0. || high<low ||
       !std::isfinite(tolerance) || tolerance<0.)
        throw std::invalid_argument("invalid_measured_resistance_refinement_domain");
    for(const auto& point:points)
        if(!std::isfinite(point.resistance) || point.resistance<=0. ||
           !std::isfinite(point.amplitudeResidual))return {};
    points.erase(std::remove_if(points.begin(),points.end(),[&](const auto& p) {
        return p.resistance<low || p.resistance>high;
    }),points.end());
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b) {
        return a.resistance<b.resistance;
    });
    std::vector<MeasuredResistancePoint> unique;
    for(std::size_t begin=0;begin<points.size();)
    {
        std::size_t end=begin+1;auto representative=points[begin];
        double minimum=representative.amplitudeResidual,maximum=minimum;
        while(end<points.size() && points[end].resistance==representative.resistance)
        {
            const auto& p=points[end++];
            representative.withinTiming=representative.withinTiming && p.withinTiming;
            minimum=std::min(minimum,p.amplitudeResidual);maximum=std::max(maximum,p.amplitudeResidual);
            if(std::fabs(p.amplitudeResidual)>std::fabs(representative.amplitudeResidual))
                representative.amplitudeResidual=p.amplitudeResidual;
        }
        // Repeated errors are actual measurements. Their spread cannot be
        // averaged away to manufacture a smooth bracket or exact amplitude.
        if(maximum-minimum>tolerance)return {};
        unique.push_back(representative);begin=end;
    }
    if(unique.size()<3)return {};
    bool increasing=true,decreasing=true;
    for(std::size_t i=0;i<unique.size();++i)
    {
        if(!unique[i].withinTiming || std::fabs(unique[i].amplitudeResidual)<=tolerance)return {};
        if(i)
        {
            increasing=increasing && unique[i].amplitudeResidual>=unique[i-1].amplitudeResidual;
            decreasing=decreasing && unique[i].amplitudeResidual<=unique[i-1].amplitudeResidual;
        }
    }
    if(!increasing && !decreasing)return {};
    std::optional<double> result;
    std::tuple<double,double,double> best;
    for(std::size_t i=1;i<unique.size();++i)
    {
        const auto& a=unique[i-1];const auto& b=unique[i];
        if((a.amplitudeResidual<0.)==(b.amplitudeResidual<0.))continue;
        const double logLow=std::log(a.resistance),span=std::log(b.resistance)-logLow;
        const double candidate=std::exp(logLow+.5*span);
        if(!std::isfinite(candidate) || candidate<=a.resistance || candidate>=b.resistance)continue;
        const auto rank=std::tuple{std::min(std::fabs(a.amplitudeResidual),std::fabs(b.amplitudeResidual)),span,a.resistance};
        if(!result || rank<best){result=candidate;best=rank;}
    }
    return result;
}
}
