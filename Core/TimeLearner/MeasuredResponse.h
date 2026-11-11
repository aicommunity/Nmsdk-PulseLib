#pragma once
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace NMSDK::TimeLearner {
struct ResponseSample {
    double time=0.,rawPotential=0.;
};
struct ResponseMaximum {
    std::size_t firstSample=0,lastSample=0;
    double amplitude=0.,rawPotential=0.,time=0.,plateauEnd=0.;
};
enum class ResponseFeatureValidity {
    Valid, InsufficientSamples, InvalidSpecification, NonfiniteSample, NonmonotonicTime
};
// Measured features, not an impulse assignment, completed observation or
// training certificate. Domain coverage/model revision belong to the caller.
struct MeasuredResponseFeatures {
    ResponseFeatureValidity validity=ResponseFeatureValidity::InsufficientSamples;
    std::vector<ResponseMaximum> positiveMaxima;
    std::optional<ResponseMaximum> globalMaximum;
    std::size_t largestInteriorMaximumCount=0;
    bool globalMaximumClipped=false,boundaryResponseClipped=false;
};
// Exact measured sample positions; no interpolation, predicted delay, causal
// labels, threshold adaptation or automatic waveform decomposition.
MeasuredResponseFeatures ExtractMeasuredResponse(std::span<const ResponseSample> samples,
    double baseline,double minimumAmplitude);
}
