#pragma once
#include "NativeRecognitionTrial.h"
#include "NativeCalibrationCriterion.h"

namespace NMSDK::TimeLearner {
struct NativeThresholdProposal {
    std::optional<double> threshold;
    QJsonObject evidence;
};
// A proposal from complete NONSPIKING characterisation only. It is not a
// separability certificate: the proposed threshold still needs actual spiking
// trials and a fresh full confirmation. The expected set is declared before
// acquisition; targets are identified by their original training-pattern hash.
NativeThresholdProposal ProposeNativeThreshold(const std::vector<NativeRecognitionTrialRequest>& expected,
    const std::vector<NativeRecognitionTrialObservation>& observations,const std::string& trainingPatternHash,
    const CalibrationResponseWindow& window,double minimumThreshold,double maximumThreshold);
}
