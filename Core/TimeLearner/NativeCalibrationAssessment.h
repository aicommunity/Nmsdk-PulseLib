#pragma once
#include "NativeCalibrationCriterion.h"
#include "NativeRecognitionTrial.h"

namespace NMSDK::TimeLearner {
struct NativeCalibrationAssessment {
    bool criterionMet=false;
    std::size_t targetViolations=0,foilResponses=0;
    QJsonObject evidence;
};
// One predeclared, complete candidate matrix on one native model clock.
// Physical revision/frequency must remain fixed; expected gives presentation
// order independently of the order of recorded rows. Uses real spike histories,
// never threshold crossings inferred from an analogue trace.
// This assessment is neither a calibrated-state commit nor a training certificate.
NativeCalibrationAssessment AssessNativeThresholdResponses(
    const std::vector<NativeRecognitionTrialRequest>& expected,
    const std::vector<NativeRecognitionTrialObservation>& observations,
    const std::string& trainingPatternHash,const CalibrationResponseWindow& window);
}
