#pragma once
// Private shared validation of the actual closed native N+2 envelope.
// It adds no selector policy, owner lease or physical execution attestation.
#include "MatchedOmissionValidation.h"
#include "NativeMatchedOmissionGroup.h"

namespace NMSDK::TimeLearner::NativeBasinDetail {
void CheckNativeEnvelope(const NativeMatchedOmissionResult&,
    const MatchedOmissionDetail::CheckedContext&);
}
