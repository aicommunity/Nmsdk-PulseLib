#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace NMSDK::TimeLearner {
// Diagnostic presentation only: source ordinals always refer to the original
// training pattern. No parameter, topology or training reference is retargeted.
struct StimulusSubsetCharacterization {
    std::string sourceTrainingPatternHash;
    std::size_t physicalInputCount=0;
    std::vector<std::size_t> sourceOrdinals;
    bool operator==(const StimulusSubsetCharacterization&) const=default;
};
struct StimulusSubsetPlan {
    std::vector<double> presentedIsi;
    std::vector<double> sourceOffsets;
};
bool ValidStimulusSubset(const StimulusSubsetCharacterization& value);
// All requested timings still require actual native event readback. Regrouping
// binary64 additions can cross a scheduler grid boundary; this is not evidence
// that an omitted-pulse trial preserved every retained actual emission time.
StimulusSubsetPlan PlanStimulusSubset(const std::vector<double>& originalIsi,
                                    const std::vector<std::size_t>& sourceOrdinals);
}
