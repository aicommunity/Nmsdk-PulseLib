#pragma once
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct CandidateRecoveryEvidence;
struct CandidateWindowRecoveryEvidence;
// Shared decision serializer. It does not certify an observation or model.
QJsonObject EncodeCandidateRecoveryEvidence(const CandidateRecoveryEvidence&);
// Versioned DATA only. An event/content hash cannot restore live authority.
QJsonObject EncodeCandidateWindowRecoveryEvidence(const CandidateWindowRecoveryEvidence&);
// Strict DATA readback only. Does not construct a sealed rejected measurement,
// authorize cursor advancement, restore a search, or certify a trained model.
CandidateWindowRecoveryEvidence DecodeCandidateWindowRecoveryEvidence(const QJsonObject&);
}
