#pragma once
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct CandidateRecoveryEvidence;
// Shared decision serializer. It does not certify an observation or model.
QJsonObject EncodeCandidateRecoveryEvidence(const CandidateRecoveryEvidence&);
}
