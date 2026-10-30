#pragma once
#include "NativeRecognitionState.h"

namespace NMSDK::TimeLearner {
// A candidate is a distinct frozen physical model, not a calibrated result.
// Every candidate starts from the same immutable converted recognition package.
// No controller certificate is transferred to the changed recognition graph.
// Algorithm v2 accepts any finite signed threshold on an explicitly supported
// scalar spike-threshold implementation. Historical v1 packages retain their
// positive-only contract and remain readable.
QJsonObject CreateNativeThresholdCandidate(const QJsonObject& sourceRecognition,double threshold,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,
    const std::string& implementationId);

// NativeRecognitionState dispatches this explicitly tagged package here.
// Reconstruct and compare the sole permitted edit before native archive import.
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeThresholdCandidate(const QJsonObject& candidate,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,
    const std::string& implementationId);
}
