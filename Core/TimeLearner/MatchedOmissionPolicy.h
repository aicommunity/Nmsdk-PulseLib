#pragma once
#include <cstddef>
#include <limits>

namespace NMSDK::TimeLearner {
// This is an operational measurement contract. It neither decomposes a
// nonlinear response nor constructs a TrainingCertificate from old diagnostics.
struct MatchedOmissionPolicy {
    unsigned schemaVersion=1;
    std::size_t comparisonRadiusTicks=1;
    std::size_t maximumSamples=0,maximumTotalSamples=0,maximumNativeProbes=0;
    // Must be explicitly supplied, finite, >=0 and strictly less than half a
    // native tick. It accommodates absolute-clock subtraction, not tick drift.
    double timeComparisonTolerance=std::numeric_limits<double>::quiet_NaN();
    double baselineAbsolute=0.,repeatabilityAbsolute=0.,repeatabilityRelative=0.;
    double influenceAbsolute=0.,influenceRelative=0.;
    bool operator==(const MatchedOmissionPolicy&) const=default;
};

}
