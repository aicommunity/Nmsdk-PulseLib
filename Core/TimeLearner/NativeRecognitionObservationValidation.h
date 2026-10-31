#pragma once
#include "NativeRecognitionTrial.h"

namespace NMSDK::TimeLearner {
// Validate the actual closed raw observation, not merely its validity flag.
// Throws a diagnostic; does not calculate/reset components or change a model.
// The declared request is supplied independently of the recorded observation.
void ValidateNativeRecognitionObservation(const NativeRecognitionTrialRequest& expected,
    const NativeRecognitionTrialObservation& observation);
// Persistence path for complete calibration evidence. The request must come
// from the independently declared design, including duration (never end-begin).
// Rechecks all raw samples/events and rejects unknown or inconsistent fields.
// Censored/invalid diagnostic observations remain readable by historical tools,
// but cannot enter this calibration-proof path.
NativeRecognitionTrialObservation DecodeNativeRecognitionObservation(const QJsonObject& object,
    const NativeRecognitionTrialRequest& expected);
NativeRecognitionTrialObservation ParseNativeRecognitionObservation(const std::string& text,
    const NativeRecognitionTrialRequest& expected,std::size_t maximumBytes=64*1024*1024);
}
