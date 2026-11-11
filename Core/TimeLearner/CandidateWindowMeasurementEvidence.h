#pragma once
#include "NativeCompositeTrainingRecord.h"

namespace NMSDK::TimeLearner {
// Canonical CONTENT identity of a complete new-method measurement. It is not
// a live lease, proof of execution, reference-set mint or training certificate.
std::string CandidateWindowTrainingSemanticProofId(const VerificationProtocol&,
    const NativeCompositeMeasurementRecord&);
// Diagnostic CONTENT only: no peak, live lease, reference or certificate.
std::string CandidateWindowRejectedGroupContentId(const VerificationProtocol&,
    const NativeCompositeMeasurementRecord&);
}
