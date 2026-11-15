#pragma once
// Private reuse of the existing strict archived physical source validation.
// No native execution, live ownership or reference/Train authority is minted.
#include "FinalVerification.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner::CompositeEvidenceDetail {
void ValidateInitialSource(const VerificationProtocol&,const QJsonObject& source,const std::string& sourceOrigin);
}
