#pragma once
#include "CandidateWindowPolicy.h"
#include "CompositeInputWindow.h"
#include "MatchedOmissionPolicy.h"
#include "DeclaredWindowBasinAssignment.h"

namespace NMSDK::TimeLearner {
inline constexpr unsigned CandidateWindowVerificationProtocolSchema=5;
inline constexpr unsigned CandidateWindowMeasurementContractSchema=2;
inline constexpr unsigned CandidateWindowBasinSpecificationSchema=2;
inline constexpr unsigned CandidateWindowCandidateBindingSchema=2;
inline constexpr unsigned CandidateWindowMeasurementRecordSchema=2;
inline constexpr unsigned CandidateWindowMeasurementArchiveSchema=2;
struct CandidateWindowMeasurementContract {
    unsigned schemaVersion=CandidateWindowMeasurementContractSchema;
    std::string methodId=DeclaredWindowBasinResult::MethodId();
    MatchedOmissionPolicy contextPolicy;
    DeclaredWindowBasinPolicy geometryPolicy;
    std::vector<CompositeInputWindow> referenceInputWindows;
    CandidateWindowPolicy candidateWindowPolicy;
    bool operator==(const CandidateWindowMeasurementContract&) const=default;
};
}
