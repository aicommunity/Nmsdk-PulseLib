#pragma once
#include "CandidateWindowTrainingBoundary.h"
#include "CandidateWindowContractValidation.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
QJsonObject EncodeCandidateWindowPolicy(const CandidateWindowPolicy&);
CandidateWindowPolicy DecodeCandidateWindowPolicy(const QJsonObject&);
QJsonObject EncodeCandidateWindowMeasurementContract(const CandidateWindowMeasurementContract&);
CandidateWindowMeasurementContract DecodeCandidateWindowMeasurementContract(const QJsonObject&);
QJsonObject EncodeCandidateWindowBasinSpecification(const CandidateWindowBasinSpecification&);
CandidateWindowBasinSpecification DecodeCandidateWindowBasinSpecification(const QJsonObject&);
}
