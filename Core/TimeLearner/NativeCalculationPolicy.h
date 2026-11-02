#pragma once
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include <stdexcept>
#include <set>
#include <vector>

namespace NMSDK::TimeLearner {
// Execution capability of the verified native protocol, not a restriction on
// Rdk construction, serialization or physical checkpoint restoration. Never
// repair the caller's policy by writing a default or calculating a component.
inline void RequireNativeCalculationScheduleNode(RDK::UContainer& node)
{
    if(node.GetMaxCalculationDuration()>=0)
        throw std::invalid_argument("unsupported_native_wall_limited_schedule");
}
inline void RequireNativeCalculationScheduleAncestors(RDK::UContainer& owner)
{
    std::set<RDK::UContainer*> seen;
    for(auto node=&owner;node;node=node->GetOwner().Get())
    {
        if(!seen.insert(node).second)throw std::invalid_argument("invalid_native_schedule_graph");
        RequireNativeCalculationScheduleNode(*node);
    }
}
inline void RequireNativeCalculationScheduleTree(RDK::UContainer& root,std::set<RDK::UContainer*>& seen)
{
    if(!seen.insert(&root).second)throw std::invalid_argument("invalid_native_schedule_graph");
    RequireNativeCalculationScheduleNode(root);
    std::vector<RDK::NameT> names;root.GetComponentsList(names);
    for(const auto& name:names)
    {
        auto child=root.GetComponent(name,true);
        if(!child)throw std::invalid_argument("missing_native_schedule_graph");
        RequireNativeCalculationScheduleTree(*child,seen);
    }
}
inline void RequireNativeCalculationSchedule(RDK::UContainer& owner)
{
    RequireNativeCalculationScheduleAncestors(owner);
    std::set<RDK::UContainer*> seen;
    for(const auto* name:{"Neuron","DatasetMatrix"})
    {
        auto child=owner.GetComponent(name,true);
        if(!child)throw std::invalid_argument("missing_native_schedule_graph");
        RequireNativeCalculationScheduleTree(*child,seen);
    }
}
}
