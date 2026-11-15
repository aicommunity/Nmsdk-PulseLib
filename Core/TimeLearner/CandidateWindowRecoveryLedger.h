#pragma once
#include "NativeTrainingService.h"

namespace NMSDK::TimeLearner {
class DecodedNativeCompositeArchive;
// Portable supplied delivery DATA, joined to a separately decoded/replayed
// archive. This is not a complete controller checkpoint or proof that no
// delivery was omitted. The wire explicitly retains that limitation.
QJsonObject EncodeCandidateWindowRecoveryLedger(const std::vector<NativeCandidateWindowRecoveryDelivery>&,
    const DecodedNativeCompositeArchive&,bool producerLogIntact);
std::vector<NativeCandidateWindowRecoveryDelivery> DecodeCandidateWindowRecoveryLedger(const QJsonObject&,
    const DecodedNativeCompositeArchive&);
}
