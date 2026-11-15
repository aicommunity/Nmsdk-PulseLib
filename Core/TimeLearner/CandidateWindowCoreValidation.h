#pragma once
#include "FinalVerification.h"
#include "CandidateWindowTrainingBoundary.h"
#include "CandidateWindowContractValidation.h"
#include <stdexcept>

namespace NMSDK::TimeLearner::CandidateWindowCoreDetail {
// Private pure consistency check over two independently closed values. No JSON,
// callback/address restoration, native readback, arithmetic mint or Rdk work.
inline void RequireCapturedReferences(const CapturedReferences& captured,const CandidateWindowReferenceSet& closed) {
    const auto& p=captured.Protocol();ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    if(closed.Protocol()!=p || closed.InputCount()!=p.inputs.size() || captured.Inputs().size()!=p.inputs.size() ||
        closed.TargetRawPotentials().size()!=p.inputs.size() || closed.ReferenceGroupIds().size()!=p.inputs.size() ||
        closed.SourceOrigin()!=captured.CompositeSourceOrigin() || closed.OwnerFamilyBinding()!=captured.CompositeOwnerScope() ||
        closed.InitialModelRevision()!=captured.InitialModelRevision() || closed.InitialModelEpoch()!=captured.InitialModelEpoch())
        throw std::invalid_argument("candidate_window_captured_reference_family_mismatch");
    const auto proofs=closed.ReferenceProofIds();
    for(std::size_t i=0;i<p.inputs.size();++i)if(captured.Inputs()[i].id!=p.inputs[i].id ||
        captured.Inputs()[i].measurementGroupId!=closed.ReferenceGroupIds()[i] || captured.Inputs()[i].semanticProofId!=proofs[i] ||
        captured.Inputs()[i].rawPotential!=closed.TargetRawPotentials()[i])
        throw std::invalid_argument("candidate_window_captured_reference_proof_mismatch");
}
}
