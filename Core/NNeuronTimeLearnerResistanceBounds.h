#ifndef NNEURONTIMELEARNER_RESISTANCE_BOUNDS_H
#define NNEURONTIMELEARNER_RESISTANCE_BOUNDS_H

#include <cmath>
#include <limits>
#include <vector>

namespace NMSDK {
namespace TimeLearnerResistanceBounds {

enum class FloorStatus
{
    Ok,
    InvalidInput,
    NoFiniteNoCrossingFloor
};

struct FloorResult
{
    FloorStatus status;
    double dimensionlessTimeScale;
    double minimumResistance;

    FloorResult()
        : status(FloorStatus::InvalidInput)
        , dimensionlessTimeScale(0.0)
        , minimumResistance(0.0)
    {}
};

struct PerInputBoundsResult
{
    bool valid;
    bool commonIntervalExists;
    std::vector<double> minimumResistance;
    std::vector<double> maximumResistance;
    std::vector<double> minimumRatio;
    double commonMinimumResistance;
    double commonMaximumResistance;
    double maximumMinimumRatio;

    PerInputBoundsResult()
        : valid(false)
        , commonIntervalExists(false)
        , commonMinimumResistance(0.0)
        , commonMaximumResistance(0.0)
        , maximumMinimumRatio(0.0)
    {}
};

/// Derive a separate Rs range for each input from its own membrane R and
/// channel-specific integration floor. The common interval is diagnostic only:
/// independently adjustable tips do not require all per-input intervals to overlap.
inline PerInputBoundsResult ComputePerInputBounds(
    const std::vector<double>& membraneResistance,
    const std::vector<double>& stabilityMinimumResistance,
    const double maximumSynapseToMembraneRatio)
{
    PerInputBoundsResult result;
    if (membraneResistance.empty()
        || membraneResistance.size() != stabilityMinimumResistance.size()
        || !std::isfinite(maximumSynapseToMembraneRatio)
        || maximumSynapseToMembraneRatio < 1.0)
        return result;

    result.minimumResistance.reserve(membraneResistance.size());
    result.maximumResistance.reserve(membraneResistance.size());
    result.minimumRatio.reserve(membraneResistance.size());
    result.commonMinimumResistance = 0.0;
    result.commonMaximumResistance = std::numeric_limits<double>::max();

    for (size_t i = 0; i < membraneResistance.size(); ++i)
    {
        const double membrane = membraneResistance[i];
        const double stability = stabilityMinimumResistance[i];
        if (!std::isfinite(membrane) || membrane <= 0.0
            || !std::isfinite(stability) || stability < 0.0)
            return PerInputBoundsResult();

        const double minimum = std::max(
            membrane / maximumSynapseToMembraneRatio, stability);
        const double maximum = membrane * maximumSynapseToMembraneRatio;
        const double minimumRatio = minimum / membrane;
        if (!std::isfinite(minimum) || minimum <= 0.0
            || !std::isfinite(maximum) || maximum < minimum
            || !std::isfinite(minimumRatio) || minimumRatio <= 0.0)
            return PerInputBoundsResult();

        result.minimumResistance.push_back(minimum);
        result.maximumResistance.push_back(maximum);
        result.minimumRatio.push_back(minimumRatio);
        result.commonMinimumResistance = std::max(result.commonMinimumResistance, minimum);
        result.commonMaximumResistance = std::min(result.commonMaximumResistance, maximum);
        result.maximumMinimumRatio = std::max(result.maximumMinimumRatio, minimumRatio);
    }

    result.valid = true;
    result.commonIntervalExists =
        result.commonMaximumResistance >= result.commonMinimumResistance;
    return result;
}

inline int EffectiveConductanceCount(const int connectedSynapses, const bool useAverageSynapsis)
{
    const int connected = connectedSynapses > 0 ? connectedSynapses : 0;
    return useAverageSynapsis && connected > 0 ? 1 : connected;
}

/// Compute the Rs floor that keeps the explicit NPulseChannel update from
/// crossing its instantaneous equilibrium, assuming each connected synapse
/// contributes no more than normalizedMaxConductance / Rs.
///
/// For Q = C*R*f and rho = Rs/R, the update coefficient is
/// K = (1 + G*R)^2/Q. Requiring K<=1 gives
/// Rs >= N*normalizedMaxConductance*R/(sqrt(Q)-1), which has a finite value
/// only when Q>1. This is an integration-stability bound, not a learning rule.
inline FloorResult ComputeNoCrossingResistanceFloor(
    const double capacity,
    const double channelResistance,
    const double updateFrequency,
    const int connectedSynapses,
    const double normalizedMaxConductance = 1.0)
{
    FloorResult result;
    if (!std::isfinite(capacity) || capacity <= 0.0
        || !std::isfinite(channelResistance) || channelResistance <= 0.0
        || !std::isfinite(updateFrequency) || updateFrequency <= 0.0
        || connectedSynapses <= 0
        || !std::isfinite(normalizedMaxConductance)
        || normalizedMaxConductance <= 0.0)
        return result;

    const double q = capacity * channelResistance * updateFrequency;
    if (!std::isfinite(q) || q <= 0.0)
        return result;
    result.dimensionlessTimeScale = q;
    if (q <= 1.0)
    {
        result.status = FloorStatus::NoFiniteNoCrossingFloor;
        return result;
    }

    const double denominator = std::sqrt(q) - 1.0;
    const double numerator = channelResistance
        * static_cast<double>(connectedSynapses)
        * normalizedMaxConductance;
    const double minimumResistance = numerator / denominator;
    if (!std::isfinite(minimumResistance) || minimumResistance <= 0.0)
        return result;

    result.status = FloorStatus::Ok;
    result.minimumResistance = minimumResistance;
    return result;
}

} // namespace TimeLearnerResistanceBounds
} // namespace NMSDK

#endif
