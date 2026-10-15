#ifndef NNEURONTIMELEARNER_STRUCTURAL_AMP_H
#define NNEURONTIMELEARNER_STRUCTURAL_AMP_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

namespace NMSDK {

/// Tracks reproducible adjacent integer synapse counts that straddle a target
/// peak. Only the lower, under-target count can be accepted as a quantized
/// structural-amplitude result. This tracker does not change trainer state.
class StructuralAmpQuantizationTracker
{
public:
    StructuralAmpQuantizationTracker()
        : maxCount_(0)
    {}

    void Reset(const size_t inputCount, const int maxCount)
    {
        maxCount_ = std::max(0, maxCount);
        inputs_.assign(inputCount, InputState());
        for (size_t i = 0; i < inputs_.size(); ++i)
            inputs_[i].counts.assign(static_cast<size_t>(maxCount_ + 1), CountState());
    }

    /// Invalidate every bracket when a structural change can alter all inputs'
    /// measured peaks (for example, changing a dendrite length).
    void InvalidateAll()
    {
        for (size_t i = 0; i < inputs_.size(); ++i)
            ClearInput(inputs_[i]);
    }

    /// A synapse-count change changes the shared neuron response for the other
    /// inputs. Preserve the changing input's own N/N+1 bracket, but invalidate
    /// all other inputs' evidence.
    void InvalidateOtherInputs(const size_t changedInput)
    {
        for (size_t i = 0; i < inputs_.size(); ++i)
        {
            if (i != changedInput)
                ClearInput(inputs_[i]);
        }
    }

    /// Record one valid measurement at a settled length. A count becomes
    /// evidence only after repeated observations remain on the same side of
    /// the target. A mixed-side count is not usable until reset by a new
    /// target or length.
    bool Observe(const size_t input,
                 const int count,
                 const int length,
                 const double target,
                 const double peak,
                 const bool peakValid,
                 const bool lengthSettled,
                 const double exactTolerance,
                 const int minRepeats = 2)
    {
        if (input >= inputs_.size() || count < 1 || count > maxCount_
            || length < 1 || !std::isfinite(target) || target <= 0.0
            || !std::isfinite(peak) || !std::isfinite(exactTolerance)
            || exactTolerance < 0.0 || minRepeats < 1
            || !peakValid || !lengthSettled)
            return false;

        InputState &state = inputs_[input];
        if (!state.initialized || state.length != length
            || !SameTarget(state.target, target))
        {
            ResetInput(state, length, target);
        }

        const double error = target - peak;
        CountState &observation = state.counts[static_cast<size_t>(count)];
        if (std::fabs(error) <= exactTolerance)
        {
            // Exact convergence belongs to the existing EOL path. Do not let
            // an old bracket classify this count as strictly under-target.
            observation = CountState();
            return false;
        }

        const int side = error > 0.0 ? 1 : -1; // +1 below target; -1 above.
        if (observation.side == side)
        {
            if (observation.samples < minRepeats)
                ++observation.samples;
        }
        else if (observation.side == 0)
        {
            observation.side = side;
            observation.samples = 1;
        }
        else
        {
            // The same count crossed the target across observations, so it is
            // not stable evidence for either side of the discrete bracket.
            observation = CountState();
            return false;
        }

        const int lowerCount = FindAcceptedLowerCount(state, minRepeats);
        return lowerCount == count;
    }

    bool IsAccepted(const size_t input,
                    const int count,
                    const int length,
                    const double target,
                    const int minRepeats = 2) const
    {
        if (input >= inputs_.size() || !std::isfinite(target))
            return false;
        const InputState &state = inputs_[input];
        return state.initialized && state.length == length
            && SameTarget(state.target, target)
            && FindAcceptedLowerCount(state, minRepeats) == count;
    }

    int AcceptedCount(const size_t input,
                      const int length,
                      const double target,
                      const int minRepeats = 2) const
    {
        if (input >= inputs_.size() || !std::isfinite(target))
            return -1;
        const InputState &state = inputs_[input];
        if (!state.initialized || state.length != length
            || !SameTarget(state.target, target))
            return -1;
        return FindAcceptedLowerCount(state, minRepeats);
    }

private:
    struct CountState
    {
        int side;
        int samples;

        CountState()
            : side(0)
            , samples(0)
        {}
    };

    struct InputState
    {
        bool initialized;
        int length;
        double target;
        std::vector<CountState> counts;

        InputState()
            : initialized(false)
            , length(0)
            , target(0.0)
        {}
    };

    static bool SameTarget(const double left, const double right)
    {
        const double scale = std::max(1.0, std::max(std::fabs(left), std::fabs(right)));
        return std::fabs(left - right) <= scale * 1e-12;
    }

    void ResetInput(InputState &state, const int length, const double target)
    {
        state.initialized = true;
        state.length = length;
        state.target = target;
        state.counts.assign(static_cast<size_t>(maxCount_ + 1), CountState());
    }

    void ClearInput(InputState &state)
    {
        state = InputState();
        state.counts.assign(static_cast<size_t>(maxCount_ + 1), CountState());
    }

    static int FindAcceptedLowerCount(const InputState &state, const int minRepeats)
    {
        if (!state.initialized || minRepeats < 1)
            return -1;
        // Prefer the nearest safe count to the target if the observed response
        // is not globally monotone in the number of synapses.
        for (size_t upper = state.counts.size(); upper > 2; --upper)
        {
            const size_t lower = upper - 2;
            const size_t adjacent = upper - 1;
            const CountState &below = state.counts[lower];
            const CountState &above = state.counts[adjacent];
            if (below.side == 1 && below.samples >= minRepeats
                && above.side == -1 && above.samples >= minRepeats)
                return static_cast<int>(lower);
        }
        return -1;
    }

    int maxCount_;
    std::vector<InputState> inputs_;
};

} // namespace NMSDK

#endif // NNEURONTIMELEARNER_STRUCTURAL_AMP_H
