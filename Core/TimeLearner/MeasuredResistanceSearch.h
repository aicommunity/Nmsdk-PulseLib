#pragma once
#include <optional>
#include <vector>

namespace NMSDK::TimeLearner {
struct MeasuredResistancePoint {
    double resistance=0.,amplitudeResidual=0.;
    bool withinTiming=false;
};

// The caller binds every point to one input, full geometry and freshly
// assessed normalized physical scope. This only proposes a candidate: it
// neither certifies monotonic physics nor accepts an unmeasured solution.
// Inconsistent, nonmonotone or timing-invalid samples retain mesh fallback.
std::optional<double> ProposeMeasuredResistanceRefinement(
    std::vector<MeasuredResistancePoint> points,double minimumResistance,
    double maximumResistance,double amplitudeTolerance);
}
