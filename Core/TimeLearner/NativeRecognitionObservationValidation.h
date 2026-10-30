#pragma once
#include "NativeRecognitionTrial.h"

namespace NMSDK::TimeLearner {
// Validate the actual closed raw observation, not merely its validity flag.
// Throws a diagnostic; does not calculate/reset components or change a model.
// The declared request is supplied independently of the recorded observation.
void ValidateNativeRecognitionObservation(const NativeRecognitionTrialRequest& expected,
    const NativeRecognitionTrialObservation& observation);
}
