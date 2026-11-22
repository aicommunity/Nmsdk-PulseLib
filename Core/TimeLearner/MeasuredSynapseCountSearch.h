#pragma once
#include <optional>
#include <vector>

namespace NMSDK::TimeLearner {
struct MeasuredSynapseCountPoint {
    int synapses=0;
    double amplitudeResidual=0.;
    bool withinTiming=false;
};

// The caller binds measurements to one input, geometry, C and physical profile.
// A secant only proposes an unvisited integer; it never accepts an amplitude,
// asserts global monotonicity, or substitutes for another native measurement.
std::optional<int> ProposeMeasuredSynapseCount(std::vector<MeasuredSynapseCountPoint> points,
    int minimumSynapses,int maximumSynapses,double amplitudeTolerance);
}
