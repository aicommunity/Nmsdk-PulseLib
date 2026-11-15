#include "MeasuredLengthSearch.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace NMSDK::TimeLearner {
bool SameBranchLengthProposalFamily(const std::vector<InputParameters>& background,std::size_t input,
    const std::vector<InputParameters>& previous,const std::vector<InputParameters>& current)
{
    if(background.size()<2 || input>=background.size()-1 || previous.size()!=background.size() ||
       current.size()!=background.size())return false;
    for(std::size_t i=0;i<background.size();++i)
    {
        for(const auto* p:{&background[i],&previous[i],&current[i]})
            if(p->length<1 || p->synapses<1 || !std::isfinite(p->resistance) || p->resistance<=0.)return false;
        if(previous[i].resistance!=current[i].resistance || previous[i].synapses!=current[i].synapses)return false;
        if(i!=input && (previous[i].resistance!=background[i].resistance ||
                       previous[i].synapses!=background[i].synapses))return false;
        if(i>input && (previous[i]!=background[i] || current[i]!=background[i]))return false;
        if(i+1<background.size() && (background[i].length<background[i+1].length ||
           previous[i].length<previous[i+1].length || current[i].length<current[i+1].length))return false;
    }
    for(const std::int64_t separation:{0,1})
    {
        if(separation && !input)continue;
        bool same=true;
        for(std::size_t i=0;i<input;++i)
        {
            const auto oldLength=std::max<std::int64_t>(background[i].length,
                std::int64_t(previous[input].length)+separation);
            const auto newLength=std::max<std::int64_t>(background[i].length,
                std::int64_t(current[input].length)+separation);
            same=same && previous[i].length==oldLength && current[i].length==newLength;
        }
        if(same)return true;
    }
    return false;
}

std::optional<int> ProposeMeasuredLength(std::vector<MeasuredLengthPoint> points,
    int minimumLength,int maximumLength,double tolerance)
{
    if(minimumLength<1 || maximumLength<minimumLength || !std::isfinite(tolerance) || tolerance<0.)
        throw std::invalid_argument("invalid_measured_length_domain");
    for(const auto& p:points)
        if(p.length<1 || !std::isfinite(p.timingResidualLow) || !std::isfinite(p.timingResidualHigh) ||
            p.timingResidualLow>p.timingResidualHigh)return {};
    points.erase(std::remove_if(points.begin(),points.end(),[&](const auto& p) {
        return p.length<minimumLength || p.length>maximumLength;
    }),points.end());
    std::sort(points.begin(),points.end(),[](const auto& a,const auto& b){return a.length<b.length;});
    std::vector<MeasuredLengthPoint> unique;
    for(std::size_t begin=0;begin<points.size();)
    {
        std::size_t end=begin+1;auto representative=points[begin];
        double lowMinimum=representative.timingResidualLow,lowMaximum=lowMinimum;
        double highMinimum=representative.timingResidualHigh,highMaximum=highMinimum;
        while(end<points.size() && points[end].length==representative.length)
        {
            const auto& p=points[end++];
            lowMinimum=std::min(lowMinimum,p.timingResidualLow);lowMaximum=std::max(lowMaximum,p.timingResidualLow);
            highMinimum=std::min(highMinimum,p.timingResidualHigh);highMaximum=std::max(highMaximum,p.timingResidualHigh);
        }
        if(lowMaximum-lowMinimum>tolerance || highMaximum-highMinimum>tolerance)return {};
        // Preserve the measured interval instead of averaging away uncertainty.
        representative.timingResidualLow=lowMinimum;representative.timingResidualHigh=highMaximum;
        if(std::max(std::fabs(lowMinimum),std::fabs(highMaximum))<=tolerance)return {};
        unique.push_back(representative);begin=end;
    }
    if(unique.size()<3)return {};
    const auto error=[](const auto& p){return .5*p.timingResidualLow+.5*p.timingResidualHigh;};
    bool increasing=true,decreasing=true;
    for(std::size_t i=1;i<unique.size();++i)
    {
        increasing=increasing && error(unique[i])>=error(unique[i-1]);
        decreasing=decreasing && error(unique[i])<=error(unique[i-1]);
    }
    if(!increasing && !decreasing)return {};
    std::optional<int> result;
    std::tuple<double,std::int64_t,int> best;
    for(std::size_t i=1;i<unique.size();++i)
    {
        const auto& a=unique[i-1];const auto& b=unique[i];
        // A bracket requires two measured intervals wholly on opposite sides.
        // Straddling/flat peak intervals are not predicted away.
        const bool bracket=(a.timingResidualHigh<0. && b.timingResidualLow>0.) ||
            (a.timingResidualLow>0. && b.timingResidualHigh<0.);
        if(!bracket)continue;
        const auto span=std::int64_t(b.length)-a.length;
        if(span<2)continue;
        const int candidate=int(std::int64_t(a.length)+span/2);
        const auto rank=std::tuple{std::min(std::fabs(error(a)),std::fabs(error(b))),span,a.length};
        if(!result || rank<best){result=candidate;best=rank;}
    }
    if(result)return result;
    // With no measured bracket, use only a one-sided measured trend. This is
    // an extrapolated candidate, never a claim of monotonicity or convergence.
    const bool negative=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.timingResidualHigh<0.;});
    const bool positive=std::all_of(unique.begin(),unique.end(),[](const auto& p){return p.timingResidualLow>0.;});
    if(!negative && !positive)return {};
    const auto nearest=std::min_element(unique.begin(),unique.end(),[&](const auto& a,const auto& b) {
        return std::fabs(error(a))<std::fabs(error(b));
    });
    const std::size_t index=std::size_t(nearest-unique.begin());
    const std::size_t neighbor=index==0?1:index-1;
    const auto& a=unique[neighbor];const auto& b=*nearest;
    const double delta=error(b)-error(a);
    if(!std::isfinite(delta) || delta==0.)return {};
    const double estimate=double(b.length)-error(b)/delta*(double(b.length)-a.length);
    if(!std::isfinite(estimate))return {};
    const int candidate=int(std::round(std::clamp(estimate,double(minimumLength),double(maximumLength))));
    if(std::any_of(unique.begin(),unique.end(),[&](const auto& p){return p.length==candidate;}))return {};
    return candidate;
}
}
