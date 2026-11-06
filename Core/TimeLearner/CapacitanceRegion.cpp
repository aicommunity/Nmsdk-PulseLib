#include "CapacitanceRegion.h"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
CapacitanceRegion SelectCapacitanceRegion(ProbeRouting routing,
    const std::vector<InputParameters>& parameters,std::size_t input)
{
    if((routing!=ProbeRouting::ClassicBurst && routing!=ProbeRouting::BranchSoloBurst) ||
       parameters.empty() || input>=parameters.size())
        throw std::invalid_argument("invalid_capacitance_region_input");
    for(std::size_t i=0;i<parameters.size();++i)
        if(parameters[i].length<1 || (routing==ProbeRouting::BranchSoloBurst && i &&
            parameters[i-1].length<parameters[i].length))
            throw std::invalid_argument("invalid_capacitance_region_geometry");
    if(input==parameters.size()-1)
        throw std::invalid_argument("capacitance_search_anchor_is_fixed");
    if(routing==ProbeRouting::ClassicBurst)return {input+1,1,parameters[input].length};
    return {1,std::int64_t(parameters[input+1].length)+1,parameters[input].length};
}

TrainingCapacitance MakeCapacitanceRegionRequest(const TrainingCapacitance& owned,
    const CapacitanceRegion& region,double scale,std::size_t maximumMembranes)
{
    ValidateTrainingCapacitance(owned);
    if(region.dendrite==0 || region.first<1 || region.last<0 ||
       region.last>std::numeric_limits<int>::max() || region.first>region.last+1)
        throw std::invalid_argument("invalid_capacitance_region");
    if(!std::isfinite(scale) || scale<=0. || scale<1.-owned.maximumRelativeChange ||
       scale>1.+owned.maximumRelativeChange)
        throw std::invalid_argument("capacitance_region_scale_outside_owned_policy");
    const auto count=region.first>region.last?0:std::uint64_t(region.last-region.first)+1;
    if(count>maximumMembranes)
        throw std::invalid_argument("capacitance_region_exceeds_declared_budget");
    auto result=owned;
    for(auto segment=region.first;segment<=region.last;++segment)
        result.membraneScales["Dendrite"+std::to_string(region.dendrite)+"_"+std::to_string(segment)]=scale;
    ValidateTrainingCapacitance(result);
    return result;
}
}
