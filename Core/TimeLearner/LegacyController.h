#pragma once
#include <optional>
#include <vector>

namespace NMSDK::TimeLearner::Legacy {
// Compatibility policies only. In particular, the delay fallback is a model
// prediction and must never be accepted as measured evidence by controller v2.
double ChooseDelay(double modelDelay, double measuredDelay, bool peakValid,
                   double measuredPeak, double expectedPulse, double syncTolerance,
                   double agreementMarginMinimum);

struct ResistancePolicy {
    double defaultGain;
    double minimumMeasurableAmplitude;
    double overshootGainFactor;
    double undershootGainFactor;
    double undershootBoostRatio;
};
struct ResistanceDecision {
    double proposedResistance;
    double effectiveGain;
    bool adjusted;
};
// Does not apply bounds: each topology owns its effective per-input interval.
ResistanceDecision ProposeResistance(double oldResistance, double amplitude,
    double reference, double error, double configuredGain,
    std::optional<double> previousError, const ResistancePolicy& policy);

void ComputePeakTimes(int count, double firstImpulse,
    const std::vector<bool>& valid, const std::vector<double>& absolutePeaks,
    const std::vector<double>& relativePulses, std::vector<double>& relativePeaks,
    std::vector<double>& delays);

int DetectPulses(int counter, double level, int& previousCounter, double& previousLevel);
}
