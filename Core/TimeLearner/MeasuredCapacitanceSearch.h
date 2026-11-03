#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace NMSDK::TimeLearner {
struct MeasuredCapacitancePoint {
    double scale=1.,timingResidualLow=0.,timingResidualHigh=0.;
    // The native adapter must bind the input, selected membrane paths, C0,
    // complete effective graph and all OTHER physical parameters/protocols.
    // Actual model revision changes with C, so it is not this comparison key.
    std::string comparisonScope;
};

// Absolute positive multipliers of immutable individual C0, never repeated
// multiplication of the current C. This is a finite fallback mesh, not a
// statement about feasibility, monotonicity, convergence or integration safety.
std::vector<double> CapacitanceSeedScales(double minimumScale,double maximumScale,
    double currentScale,unsigned subdivisionDepth,std::size_t maximumCandidates);

// Uses completed valid native measurements selected by the caller. Missing,
// ambiguous or censored peaks cannot become these points. Three distinct
// measured scales with a consistent trend are required. Mixed scopes,
// inconsistent repeats, nonfinite/invalid intervals retain bounded fallback.
// A returned scale is only a proposal: joint L/Ns/Rs/C native preview, a fresh
// observation and all-input verification remain mandatory. No certificate or
// integration decision is produced here. The original SyncTolerance is not
// replaced by this optional refinement tolerance.
std::optional<double> ProposeMeasuredCapacitanceRefinement(
    std::vector<MeasuredCapacitancePoint> points,const std::string& comparisonScope,
    double minimumScale,double maximumScale,double refinementTolerance,double gain);
}
