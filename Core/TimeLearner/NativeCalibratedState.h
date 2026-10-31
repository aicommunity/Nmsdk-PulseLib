#pragma once
#include "NativeThresholdCandidate.h"
#include "NativeCalibrationAssessment.h"

namespace NMSDK::TimeLearner {
struct NativeCalibrationMatrix {
    std::vector<NativeRecognitionTrialRequest> requests;
    std::vector<NativeRecognitionTrialObservation> observations;
};
// Shared production encoding used by the proof and transaction report.
QJsonObject EncodeNativeCalibrationRequest(const NativeRecognitionTrialRequest& request);
NativeRecognitionTrialRequest DecodeNativeCalibrationRequest(const QJsonObject& request);
QJsonObject EncodeNativeCalibrationMatrix(const NativeCalibrationMatrix& matrix);
struct NativeThresholdConfirmationSpec {
    std::string protocol="historical_same_matrix";
    std::string dataPartitionId,attemptId,clockId;
    CalibrationResponseWindow window;
};
// The same strict codec is used by saved proofs and production job input.
QJsonObject EncodeNativeThresholdConfirmationSpec(const NativeThresholdConfirmationSpec& specification);
NativeThresholdConfirmationSpec DecodeNativeThresholdConfirmationSpec(const QJsonObject& specification);
// Finalize an explicitly selected candidate after two complete successful
// matrices: search and fresh confirmation on the same declared clock/design.
// This verifies the selected candidate, not selection over an undeclared grid,
// statistical independence, independent Test performance or training Exact.
// Native imports remain detached; no Calculate or live owner edits occur here.
QJsonObject CreateNativeCalibratedState(const QJsonObject& candidate,
    const NativeThresholdConfirmationSpec& specification,const NativeCalibrationMatrix& search,
    const NativeCalibrationMatrix& confirmation,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeCalibratedState(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
}
