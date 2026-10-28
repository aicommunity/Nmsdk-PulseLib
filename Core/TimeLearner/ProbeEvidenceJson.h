#pragma once
#include "NativeProbe.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
// Full internal measurement evidence, distinct from the recognition CSV schema.
// Nonfinite numbers and integers not exactly representable in JSON are errors;
// they must never become an apparently valid null/rounded measurement.
QJsonObject EncodeNativeProbeEvidence(const NativeProbeResult& result);
NativeProbeResult DecodeNativeProbeEvidence(const QJsonObject& object);
QJsonObject EncodeTrainingCertificate(const TrainingCertificate& certificate);
QJsonObject EncodeVerificationProtocol(const VerificationProtocol& protocol);
VerificationProtocol DecodeVerificationProtocol(const QJsonObject& object);
class NativeTrainingService;
struct InputParameters;
// Diagnostic state summary, not a checkpoint/import format or a scientific
// verdict. The actual graph/readback is supplied independently by the adapter.
QJsonObject EncodeNativeTrainingState(const NativeTrainingService& service,const std::string& actualModelRevision,
                                      const std::vector<InputParameters>& actualParameters);
}
