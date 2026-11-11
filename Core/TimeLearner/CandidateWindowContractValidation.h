#pragma once
#include "CandidateWindowMeasurementContract.h"
namespace NMSDK::TimeLearner {
struct VerificationProtocol;
void ValidateCandidateWindowPolicyDeclaration(const CandidateWindowPolicy&,
    std::size_t inputCount,std::uint64_t maximumSamples);
void ValidateCandidateWindowMeasurementContract(const VerificationProtocol&);
}
