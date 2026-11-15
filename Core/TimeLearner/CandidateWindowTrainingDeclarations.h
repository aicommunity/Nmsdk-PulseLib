#pragma once
#include "CandidateWindowTrainingBoundary.h"
#include "CandidateWindowContractValidation.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct NativeTrainingPlan;
QJsonObject EncodeCandidateWindowPolicy(const CandidateWindowPolicy&);
CandidateWindowPolicy DecodeCandidateWindowPolicy(const QJsonObject&);
QJsonObject EncodeCandidateWindowMeasurementContract(const CandidateWindowMeasurementContract&);
CandidateWindowMeasurementContract DecodeCandidateWindowMeasurementContract(const QJsonObject&);
QJsonObject EncodeCandidateWindowBasinSpecification(const CandidateWindowBasinSpecification&);
CandidateWindowBasinSpecification DecodeCandidateWindowBasinSpecification(const QJsonObject&);
// Portable declarations of an already bound service5 plan. Decoding retains
// only the declared search/budget fields and protocol: no callbacks, probe
// executor, physical owner, live lease or native admission is restored.
QJsonObject EncodeCandidateWindowRuntimePlan(const NativeTrainingPlan&);
NativeTrainingPlan DecodeCandidateWindowRuntimePlan(const QJsonObject&,const VerificationProtocol&);
}
