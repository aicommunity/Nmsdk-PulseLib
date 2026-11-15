#include "TrainingCapacitance.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
bool PositiveInteger(const std::string& value)
{return !value.empty() && value[0]>='1' && value[0]<='9' &&
    std::all_of(value.begin(),value.end(),[](char c){return c>='0' && c<='9';});}
bool Membrane(const std::string& name)
{
    if(!name.starts_with("Dendrite"))return false;
    const auto separator=name.find('_',8);
    return separator!=std::string::npos && PositiveInteger(name.substr(8,separator-8)) &&
        PositiveInteger(name.substr(separator+1));
}
bool SameOrigin(const TrainingCapacitance& a,const TrainingCapacitance& b)
{return a.origin==b.origin && a.maximumRelativeChange==b.maximumRelativeChange;}
}
void ValidateTrainingCapacitance(const std::optional<TrainingCapacitance>& value)
{
    if(!value)return;
    const auto& state=*value;
    if(state.origin.size()!=64 || !std::all_of(state.origin.begin(),state.origin.end(),[](char c) {
            return (c>='0' && c<='9') || (c>='a' && c<='f');
        }) || !std::isfinite(state.maximumRelativeChange) || state.maximumRelativeChange<0. || state.membraneScales.empty())
        throw std::invalid_argument("invalid_training_capacitance_state");
    for(const auto& [path,scale]:state.membraneScales)
        if(!Membrane(path) || !std::isfinite(scale) || scale<=0. ||
            scale<1.-state.maximumRelativeChange || scale>1.+state.maximumRelativeChange)
            throw std::invalid_argument("invalid_training_capacitance_scale");
}
bool EquivalentTrainingCapacitance(const std::optional<TrainingCapacitance>& a,
    const std::optional<TrainingCapacitance>& b)
{
    ValidateTrainingCapacitance(a);ValidateTrainingCapacitance(b);
    if(!a || !b)return bool(a)==bool(b);
    if(!SameOrigin(*a,*b))return false;
    const auto unchanged=[](const auto& first,const auto& second) {
        for(const auto& [path,scale]:first)
        {
            const auto it=second.find(path);
            if(it==second.end()?scale!=1.:it->second!=scale)return false;
        }
        return true;
    };
    return unchanged(a->membraneScales,b->membraneScales) && unchanged(b->membraneScales,a->membraneScales);
}
bool TrainingCapacitanceReadbackMatches(const std::optional<TrainingCapacitance>& requested,
    const std::optional<TrainingCapacitance>& effective)
{
    ValidateTrainingCapacitance(requested);ValidateTrainingCapacitance(effective);
    if(!requested || !effective)return bool(requested)==bool(effective);
    if(!SameOrigin(*requested,*effective))return false;
    for(const auto& [path,scale]:requested->membraneScales)
    {
        const auto actual=effective->membraneScales.find(path);
        if(actual==effective->membraneScales.end() || actual->second!=scale)return false;
    }
    for(const auto& [path,scale]:effective->membraneScales)
        if(!requested->membraneScales.count(path) && scale!=1.)return false;
    return true;
}
std::optional<TrainingCapacitance> RestoreTrainingCapacitance(
    const std::optional<TrainingCapacitance>& historical,const std::optional<TrainingCapacitance>& current)
{
    ValidateTrainingCapacitance(historical);ValidateTrainingCapacitance(current);
    if(!historical && !current)return {};
    if(!historical || !current || !SameOrigin(*historical,*current))
        throw std::invalid_argument("training_capacitance_origin_or_policy_changed");
    for(const auto& [path,scale]:historical->membraneScales)
        if(!current->membraneScales.count(path))
            throw std::invalid_argument("training_capacitance_history_lost");
    auto result=*historical;
    for(const auto& [path,scale]:current->membraneScales)
        result.membraneScales.try_emplace(path,1.);
    return result;
}
}
