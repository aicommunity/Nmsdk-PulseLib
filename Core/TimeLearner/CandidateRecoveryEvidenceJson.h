#pragma once
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct CandidateRecoveryEvidence;
struct CandidateWindowRecoveryEvidence;
// Shared decision serializer. It does not certify an observation or model.
QJsonObject EncodeCandidateRecoveryEvidence(const CandidateRecoveryEvidence&);
// Versioned DATA only. An event/content hash cannot restore live authority.
QJsonObject EncodeCandidateWindowRecoveryEvidence(const CandidateWindowRecoveryEvidence&);
}
