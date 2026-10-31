#pragma once
#include "../NPulseLTZone.h"
#include <stdexcept>
#include <typeinfo>

namespace NMSDK::TimeLearner {
// Use the actual C++ implementation, not a storage image name. Stock Bio,
// IaF, Cable and the image named Izhikevich can all be Threshold instances.
// NPulseSimpleLTZone derives from NPulseLTZone but encodes frequency and does
// not use Threshold for its spikes. Unknown overrides require an explicit
// decision-source adapter before amplitude-bracket characterization is valid.
inline bool SupportsNativeScalarSpikeThreshold(const NPulseLTZoneCommon& zone)
{
    return typeid(zone)==typeid(NPulseLTZoneThreshold) || typeid(zone)==typeid(NPulseLTZone);
}
inline void RequireNativeScalarSpikeThreshold(const NPulseLTZoneCommon& zone)
{
    if(!SupportsNativeScalarSpikeThreshold(zone))
        throw std::invalid_argument("native_scalar_spike_threshold_not_supported_by_ltzone");
}
}
