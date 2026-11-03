#pragma once
#include "FinalVerification.h"
#include "TrainingCapacitance.h"
#include "TrainingParameters.h"
#include <cstdint>
#include <vector>

namespace NMSDK::TimeLearner {
struct CapacitanceRegion {
    std::size_t dendrite=0;
    std::int64_t first=1,last=0;
    bool operator==(const CapacitanceRegion&) const=default;
};

// Classic: the selected independent path [1,L_i]. Branch: only the shared
// cable interval (L_{i+1},L_i], as in the topology's existing trim definition.
// Coincident Branch attachments have an empty interval. The last reference
// input is not a search control. Bank/Ns/Rs legality remains the controller's
// responsibility; this function validates the complete length geometry.
CapacitanceRegion SelectCapacitanceRegion(ProbeRouting routing,
    const std::vector<InputParameters>& parameters,std::size_t inputIndex);

// Absolute multipliers of immutable per-channel C0, never a common absolute
// Capacity and never multiplication of a previously trimmed C. Keep the full
// active/dormant map outside this region. The allocation budget is checked
// before expansion, including for very large declared cable lengths.
// New paths are REQUESTS: only native joint preview may establish their
// existence, effective storage physics and C0. This function certifies none.
TrainingCapacitance MakeCapacitanceRegionRequest(const TrainingCapacitance& owned,
    const CapacitanceRegion& region,double absoluteScale,std::size_t maximumMembranes);
}
