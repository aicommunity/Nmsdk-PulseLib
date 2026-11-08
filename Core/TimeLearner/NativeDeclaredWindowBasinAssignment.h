#pragma once
#include "DeclaredWindowBasinAssignment.h"
#include "NativeMatchedOmissionGroup.h"

namespace NMSDK::TimeLearner {
struct NativeDeclaredWindowBasinSpecification {
    unsigned schemaVersion=1;
    std::string methodId=DeclaredWindowBasinResult::MethodId();
    std::string groupId,inputId;
    std::size_t assignedInput=0,channel=0;
    // Relative to each group's actual fullBefore begin. Fixed before analysis.
    double windowBeginOffset=0.,windowEndOffset=0.;
};
enum class NativeDeclaredWindowBasinStatus { Rejected,Completed,CausalityRejected };
struct NativeDeclaredWindowBasinResult {
    NativeDeclaredWindowBasinStatus status=NativeDeclaredWindowBasinStatus::Rejected;
    std::vector<std::string> reasons;
    // Context/native rejection never returns a geometry result to promote.
    std::optional<DeclaredWindowBasinResult> geometry;
    std::optional<double> actualAssignedOnset;
    static constexpr const char* MethodId(){return DeclaredWindowBasinResult::MethodId();}
    static constexpr bool TrainingCertificateEligible=false;
};
// Replays closed native packets and uses the SAME private context validator as
// v1. Never accepts v1 validity/reason lists as proof. Diagnostic result only:
// no model mutation, scheduler calls, native execution attestation or Train API.
NativeDeclaredWindowBasinResult SelectNativeDeclaredWindowBasinMaximum(
    const NativeDeclaredWindowBasinSpecification& specification,
    const DeclaredWindowBasinPolicy& geometryPolicy,const NativeMatchedOmissionResult& group);
}
