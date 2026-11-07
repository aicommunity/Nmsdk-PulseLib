#include "StimulusSubset.h"
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
StimulusSubsetPlan PlanStimulusSubset(const std::vector<double>& isi,
                                    const std::vector<std::size_t>& ordinals)
{
    if(isi.empty() || ordinals.empty() || ordinals.size()>isi.size())
        throw std::invalid_argument("invalid_stimulus_subset_size");
    std::vector<double> offsets;offsets.reserve(isi.size());double offset=0.;
    for(double gap:isi)
    {
        if(!std::isfinite(gap) || gap<0.)
            throw std::invalid_argument("invalid_stimulus_subset_source_interval");
        offset+=gap;
        if(!std::isfinite(offset))throw std::invalid_argument("stimulus_subset_source_time_overflow");
        offsets.push_back(offset);
    }
    for(std::size_t i=0;i<ordinals.size();++i)
        if(ordinals[i]>=isi.size() || (i && ordinals[i]<=ordinals[i-1]))
            throw std::invalid_argument("invalid_stimulus_subset_source_ordinals");
    StimulusSubsetPlan result;
    if(ordinals.size()==isi.size())return {isi,offsets};
    std::size_t next=0;
    for(const auto ordinal:ordinals)
    {
        double gap=0.;
        while(next<=ordinal)gap+=isi[next++];
        if(!std::isfinite(gap))throw std::invalid_argument("stimulus_subset_presented_time_overflow");
        result.presentedIsi.push_back(gap);result.sourceOffsets.push_back(offsets[ordinal]);
    }
    return result;
}
}
