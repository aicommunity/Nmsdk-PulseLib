#pragma once
#include <optional>
#include <vector>

namespace NMSDK::TimeLearner {
struct MeasuredSynapseCountPoint {
    int synapses=0;
    double amplitudeResidual=0.;
    bool withinTiming=false;
};
struct MeasuredSynapseCountBracket { int lower=0,upper=0; };

// The caller binds measurements to one input, geometry, C and physical profile.
// A secant only proposes an unvisited integer; it never accepts an amplitude,
// asserts global monotonicity, or substitutes for another native measurement.
std::optional<int> ProposeMeasuredSynapseCount(std::vector<MeasuredSynapseCountPoint> points,
    int minimumSynapses,int maximumSynapses,double amplitudeTolerance);
// Adjacent opposite-sign measured counts, with no acceptable measured point.
// A proposal trigger, never a certificate or global infeasibility. No monotone
// trend is required; inconsistent repeats invalidate the whole local family.
std::vector<MeasuredSynapseCountBracket> MeasuredAdjacentSynapseCountBrackets(
    std::vector<MeasuredSynapseCountPoint> points,int minimum,int maximum,double tolerance);
}
