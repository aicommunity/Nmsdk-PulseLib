#include "CandidateWindowContractValidation.h"
#include "FinalVerification.h"
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace { void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);} }
void ValidateCandidateWindowPolicyDeclaration(const CandidateWindowPolicy& p,std::size_t n,std::uint64_t samples) {
    Require(p.schemaVersion==1 && p.policyId=="reference-anchored-candidate-window-v1" &&
        p.seedRule=="immutable_reference_full_before" && p.coordinateMode=="relative_native_frame_interval" &&
        p.proposalsPerLogicalRequest==1,"candidate_window_unknown_policy_or_strategy");
    Require(n>=2 && n<=CandidateWindowMaximumExactCount && p.inputs.size()==n && samples>=3 && samples<=CandidateWindowMaximumExactCount,
        "candidate_window_declaration_dimensions");
    Require(p.maximumFeatureTerms>0 && p.maximumFeatureTerms<=CandidateWindowMaximumExactCount &&
        n<=p.maximumFeatureTerms/3,"candidate_window_invalid_feature_budget");
    for(const auto& i:p.inputs) {
        for(auto v:{i.delayFramesPerSegment,i.baseLeftGuardFrames,i.baseRightGuardFrames,i.lengthGuardFrames,
            i.synapseCountGuardFrames,i.resistanceChangeGuardFrames,i.capacitanceChangeGuardFrames,
            i.hardFirstFrame,i.hardLastFrame,i.maximumWindowFrames})
            Require(v<=CandidateWindowMaximumExactCount,"candidate_window_inexact_policy_count");
        Require(i.hardFirstFrame<i.hardLastFrame && i.hardLastFrame<samples && i.maximumWindowFrames>=2,
            "candidate_window_invalid_declared_envelope");
    }
}
void ValidateCandidateWindowMeasurementContract(const VerificationProtocol& p) {
    Require(p.schemaVersion==5 && p.candidateWindowMeasurement && !p.compositeMeasurement,"candidate_window_protocol5_required");
    const auto& c=*p.candidateWindowMeasurement;
    Require(c.schemaVersion==2 && c.methodId==DeclaredWindowBasinResult::MethodId(),"candidate_window_measurement_contract_version");
    // Data-only reuse of the existing common numeric/context declarations.
    // This temporary descriptor is never queued, decoded as proof or admitted.
    auto common=p;common.schemaVersion=4;common.candidateWindowMeasurement.reset();
    CompositeMeasurementContract fixed;fixed.methodId=c.methodId;fixed.contextPolicy=c.contextPolicy;
    fixed.geometryPolicy=c.geometryPolicy;fixed.inputWindows=c.referenceInputWindows;common.compositeMeasurement=std::move(fixed);
    ValidateCompositeMeasurementContract(common);
    ValidateCandidateWindowPolicyDeclaration(c.candidateWindowPolicy,p.inputs.size(),c.contextPolicy.maximumSamples);
}
}
