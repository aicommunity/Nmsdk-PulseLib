#pragma once
#include "CandidateWindowTrainingBoundary.h"

namespace NMSDK::TimeLearner {
class DecodedNativeCompositeArchive;
// Portable DATA authority only: all selected original reference groups must
// have passed the strict archive2/native/context/geometry replay. No caller
// boolean, predicted frame or current trained-graph recapture is accepted.
class CandidateWindowPortableReferenceFactory {
public:
    static CandidateWindowReferenceSet Replay(const DecodedNativeCompositeArchive&);
};
}
