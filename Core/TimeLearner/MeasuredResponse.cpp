#include "MeasuredResponse.h"
#include <cmath>

namespace NMSDK::TimeLearner {
MeasuredResponseFeatures ExtractMeasuredResponse(std::span<const ResponseSample> samples,
    double baseline,double minimumAmplitude)
{
    MeasuredResponseFeatures result;
    if(!std::isfinite(baseline) || !std::isfinite(minimumAmplitude) || minimumAmplitude<=0.)
    {result.validity=ResponseFeatureValidity::InvalidSpecification;return result;}
    for(std::size_t i=0;i<samples.size();++i)
    {
        if(!std::isfinite(samples[i].time) || !std::isfinite(samples[i].rawPotential) ||
           !std::isfinite(samples[i].rawPotential-baseline))
        {result.validity=ResponseFeatureValidity::NonfiniteSample;return result;}
        if(i && samples[i].time<=samples[i-1].time)
        {result.validity=ResponseFeatureValidity::NonmonotonicTime;return result;}
    }
    if(samples.size()<3)return result;
    const auto value=[&](std::size_t i){return samples[i].rawPotential-baseline;};
    const auto maximum=[&](std::size_t first,std::size_t last) {
        return ResponseMaximum{first,last,value(first),samples[first].rawPotential,
            samples[first].time,samples[last].time};
    };
    std::size_t firstMaximum=0;
    for(std::size_t i=1;i<samples.size();++i)
        if(value(i)>value(firstMaximum))firstMaximum=i;
    std::size_t lastMaximum=firstMaximum;
    while(lastMaximum+1<samples.size() && value(lastMaximum+1)==value(firstMaximum))++lastMaximum;
    result.globalMaximum=maximum(firstMaximum,lastMaximum);
    result.globalMaximumClipped=firstMaximum==0 || lastMaximum+1==samples.size();
    // Retain all interior features even when an edge clips another response.
    for(std::size_t i=1;i+1<samples.size();)
    {
        std::size_t last=i;
        while(last+1<samples.size() && value(last+1)==value(i))++last;
        if(last+1<samples.size() && value(i)>=minimumAmplitude &&
           value(i)>value(i-1) && value(last)>value(last+1))
        {
            result.positiveMaxima.push_back(maximum(i,last));
            if(value(i)==value(firstMaximum))++result.largestInteriorMaximumCount;
        }
        i=last+1;
    }
    result.boundaryResponseClipped=(value(0)>=minimumAmplitude && value(0)>=value(1)) ||
        (value(samples.size()-1)>=minimumAmplitude && value(samples.size()-1)>=value(samples.size()-2));
    result.validity=ResponseFeatureValidity::Valid;
    return result;
}
}
