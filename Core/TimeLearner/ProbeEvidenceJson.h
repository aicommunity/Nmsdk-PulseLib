#pragma once
#include "NativeProbe.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
// Full internal measurement evidence, distinct from the recognition CSV schema.
// Nonfinite numbers and integers not exactly representable in JSON are errors;
// they must never become an apparently valid null/rounded measurement.
QJsonObject EncodeNativeProbeEvidence(const NativeProbeResult& result);
QJsonObject EncodeNativeProbeSpecification(const ProbeSpec& specification,bool relativeToTrialStart=false);
NativeProbeResult DecodeNativeProbeEvidence(const QJsonObject& object);
QJsonObject EncodeTrainingCertificate(const TrainingCertificate& certificate);
QJsonObject EncodeVerificationProtocol(const VerificationProtocol& protocol);
VerificationProtocol DecodeVerificationProtocol(const QJsonObject& object);
class NativeTrainingService;
struct InputParameters;
struct ResistanceDomainAssessment;
struct TrainingCapacitanceWitness;
QJsonObject EncodeResistanceDomainAssessment(const ResistanceDomainAssessment& assessment);
// Versioned physical evidence for a history entry; no training verdict.
QJsonObject EncodeNativeTrainingCapacitanceWitness(const TrainingCapacitanceWitness& witness);
TrainingCapacitanceWitness DecodeNativeTrainingCapacitanceWitness(const QJsonObject& witness);
// Evidence identity includes dormant history/C0 inventory. Physical revision
// alone cannot identify a complete training parameter state.
std::string NativeTrainingCapacitanceWitnessId(const TrainingCapacitanceWitness& witness);
// Diagnostic state summary, not a checkpoint/import format or a scientific
// verdict. The actual graph/readback is supplied independently by the adapter.
QJsonObject EncodeNativeTrainingState(const NativeTrainingService& service,const std::string& actualModelRevision,
                                      const std::vector<InputParameters>& actualParameters);
}
