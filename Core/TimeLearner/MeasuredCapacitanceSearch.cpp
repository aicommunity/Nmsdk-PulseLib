#include "MeasuredCapacitanceSearch.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>

namespace NMSDK::TimeLearner {
namespace {
void Domain(double minimum,double maximum)
{
    if(!std::isfinite(minimum) || !std::isfinite(maximum) || minimum<=0. || maximum<minimum)
        throw std::invalid_argument("invalid_capacitance_scale_domain");
}
double Error(const MeasuredCapacitancePoint& point)
{return .5*point.timingResidualLow+.5*point.timingResidualHigh;}
}

std::vector<double> CapacitanceSeedScales(double minimum,double maximum,double current,
    unsigned depth,std::size_t maximumCandidates)
{
    Domain(minimum,maximum);
    if(!std::isfinite(current) || current<minimum || current>maximum || depth>20 || maximumCandidates==0)
        throw std::invalid_argument("invalid_capacitance_seed_configuration");
    std::vector<double> result;
    const auto add=[&](double value) {
        if(result.size()<maximumCandidates && std::isfinite(value) && value>=minimum && value<=maximum &&
            std::find(result.begin(),result.end(),value)==result.end())result.push_back(value);
    };
    add(current);add(1.);add(minimum);add(maximum);
    for(unsigned level=1;level<=depth && result.size()<maximumCandidates;++level)
    {
        const std::size_t denominator=std::size_t(1)<<level;
        for(std::size_t numerator=1;numerator<denominator && result.size()<maximumCandidates;numerator+=2)
            add(minimum+(maximum-minimum)*(double(numerator)/double(denominator)));
    }
    return result;
}

std::optional<double> ProposeMeasuredCapacitanceRefinement(
    std::vector<MeasuredCapacitancePoint> points,const std::string& scope,
    double minimum,double maximum,double tolerance,double gain)
{
    Domain(minimum,maximum);
    if(scope.empty() || !std::isfinite(tolerance) || tolerance<0. || !std::isfinite(gain) || gain<=0.)
        throw std::invalid_argument("invalid_measured_capacitance_configuration");
    for(const auto& point:points)
        if(point.comparisonScope!=scope || !std::isfinite(point.scale) || point.scale<=0. ||
            !std::isfinite(point.timingResidualLow) || !std::isfinite(point.timingResidualHigh) ||
            point.timingResidualLow>point.timingResidualHigh)return {};
    points.erase(std::remove_if(points.begin(),points.end(),[&](const auto& point) {
        return point.scale<minimum || point.scale>maximum;
    }),points.end());
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b){return a.scale<b.scale;});
    std::vector<MeasuredCapacitancePoint> unique;
    for(std::size_t begin=0;begin<points.size();)
    {
        std::size_t end=begin+1;auto representative=points[begin];
        double lowMinimum=representative.timingResidualLow,lowMaximum=lowMinimum;
        double highMinimum=representative.timingResidualHigh,highMaximum=highMinimum;
        while(end<points.size() && points[end].scale==representative.scale)
        {
            const auto& point=points[end++];
            lowMinimum=std::min(lowMinimum,point.timingResidualLow);lowMaximum=std::max(lowMaximum,point.timingResidualLow);
            highMinimum=std::min(highMinimum,point.timingResidualHigh);highMaximum=std::max(highMaximum,point.timingResidualHigh);
        }
        if(lowMaximum-lowMinimum>tolerance || highMaximum-highMinimum>tolerance)return {};
        // Do not average away peak plateaus or inconsistent repeats.
        representative.timingResidualLow=lowMinimum;representative.timingResidualHigh=highMaximum;
        if(std::max(std::fabs(lowMinimum),std::fabs(highMaximum))<=tolerance)return {};
        unique.push_back(representative);begin=end;
    }
    if(unique.size()<3)return {};
    bool increasing=true,decreasing=true;
    for(std::size_t i=1;i<unique.size();++i)
    {
        increasing=increasing && Error(unique[i])>=Error(unique[i-1]);
        decreasing=decreasing && Error(unique[i])<=Error(unique[i-1]);
    }
    if(!increasing && !decreasing)return {};
    const auto unmeasured=[&](double value) {
        return std::isfinite(value) && value>=minimum && value<=maximum &&
            std::none_of(unique.begin(),unique.end(),[&](const auto& point){return point.scale==value;});
    };
    std::optional<double> result;std::tuple<double,double,double> best;
    for(std::size_t i=1;i<unique.size();++i)
    {
        const auto& a=unique[i-1];const auto& b=unique[i];
        const bool bracket=(a.timingResidualHigh<0. && b.timingResidualLow>0.) ||
            (a.timingResidualLow>0. && b.timingResidualHigh<0.);
        if(!bracket)continue;
        const double span=b.scale-a.scale,candidate=a.scale+.5*span;
        if(!unmeasured(candidate))continue;
        const auto rank=std::tuple{std::min(std::fabs(Error(a)),std::fabs(Error(b))),span,a.scale};
        if(!result || rank<best){result=candidate;best=rank;}
    }
    if(result)return result;
    const bool negative=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.timingResidualHigh<0.;});
    const bool positive=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.timingResidualLow>0.;});
    if(!negative && !positive)return {};
    const auto nearest=std::min_element(unique.begin(),unique.end(),[](const auto& a,const auto& b) {
        return std::fabs(Error(a))<std::fabs(Error(b));
    });
    const std::size_t index=std::size_t(nearest-unique.begin()),neighbor=index==0?1:index-1;
    const auto& a=unique[neighbor];const auto& b=*nearest;
    const double delta=Error(b)-Error(a);
    if(!std::isfinite(delta) || delta==0.)return {};
    const double estimate=b.scale-gain*(Error(b)/delta)*(b.scale-a.scale);
    // Overflow cannot be interpreted as a measured direction toward a bound.
    if(!std::isfinite(estimate))return {};
    const double candidate=std::clamp(estimate,minimum,maximum);
    return unmeasured(candidate)?std::optional<double>(candidate):std::nullopt;
}
}
