#pragma once
#include "FinalVerification.h"
namespace NMSDK::TimeLearner {
// Strict DATA binding/namespace validation shared by native5 and archive2.
// It does not attest that any native request was issued or consumed.
void ValidateCandidateWindowLogicalContext(const VerificationProtocol&,const VerificationRequest&,
    const std::string& attemptId,const std::string& groupId,CompositeMeasurementPurpose);
}
