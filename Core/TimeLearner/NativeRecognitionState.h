#pragma once
#include "NativeLearnedState.h"

namespace NMSDK::TimeLearner {
enum class RecognitionConversionMode { Identity, HistoricalBranchN1 };
struct RecognitionResistanceBounds { double minimum=0.,maximum=0.; };
struct NativeRecognitionSpec {
    RecognitionConversionMode mode=RecognitionConversionMode::Identity;
    // Explicit experiment conditions for scaled conversion. Native hard
    // ratio/floor limits are intersected and recorded separately.
    std::vector<RecognitionResistanceBounds> resistanceBounds;
};
struct PreparedNativeRecognitionState {
    RDK::UEPtr<RDK::UStorage> storage;
    RDK::UEPtr<NPulseNeuron> neuron;
    RDK::UEPtr<NDatasetMatrix> dataset;
    std::vector<InputParameters> parameters;
    std::string modelRevision,sourceSoloRevision;
    QJsonObject conversionRecord;
    ~PreparedNativeRecognitionState();
};
QJsonObject EncodeNativeRecognitionSpec(const NativeRecognitionSpec& specification);
NativeRecognitionSpec DecodeNativeRecognitionSpec(const QJsonObject& object);
// Always starts from an immutable completed solo package, never from already
// scaled live Rs. Uses private native imports; does not calculate components,
// change the caller's neuron or modify storage images. Unresolved completed
// training retains its outcome; a stopped/partial package is rejected.
QJsonObject ConvertNativeSoloToRecognition(const QJsonObject& solo,const NativeRecognitionSpec& specification,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
QJsonObject ParseNativeRecognitionState(const std::string& text);
// Re-derives the expected conversion from source_solo, checks the recorded
// decision, then loads the actual recognition archive through native import.
// No training service/certificate is installed in the returned frozen graph.
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeRecognitionState(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
}
