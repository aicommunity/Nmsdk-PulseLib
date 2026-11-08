#pragma once
#include "TrainingParameters.h"
#include "TrainingCapacitance.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace NMSDK::TimeLearner {
inline constexpr std::uint64_t CandidateWindowMaximumExactCount=9007199254740991ULL;
// These are declarations checked for exact equality, not attestations of
// native source phase, actual event frames, physical routing or execution.
struct CandidateWindowGridBinding {
    std::uint64_t frequencyHz=0;
    std::string sourceOrigin,sourcePatternBinding,sourceProfileBinding;
    std::string frameOriginPolicy="native-full-before-frame-zero-v1";
    std::string sourcePhasePolicy="same-declared-source-native-events-measured-v1";
    std::string resetBinding="native-neuron-dataset-reset-before-children-v1";
    bool operator==(const CandidateWindowGridBinding&) const=default;
};
struct CandidateInputWindowPolicy {
    std::uint64_t delayFramesPerSegment=0;
    std::uint64_t baseLeftGuardFrames=0,baseRightGuardFrames=0;
    std::uint64_t lengthGuardFrames=0,synapseCountGuardFrames=0;
    std::uint64_t resistanceChangeGuardFrames=0,capacitanceChangeGuardFrames=0;
    std::uint64_t hardFirstFrame=0,hardLastFrame=0,maximumWindowFrames=0;
    bool operator==(const CandidateInputWindowPolicy&) const=default;
};
struct CandidateWindowPolicy {
    unsigned schemaVersion=1;
    std::string policyId="reference-anchored-candidate-window-v1";
    std::string seedRule="immutable_reference_full_before";
    std::string coordinateMode="relative_native_frame_interval";
    std::uint64_t proposalsPerLogicalRequest=1,maximumFeatureTerms=0;
    std::vector<CandidateInputWindowPolicy> inputs;
    bool operator==(const CandidateWindowPolicy&) const=default;
};
// Untrusted arithmetic facts. No validity flag, waveform, amplitude, residual,
// predictor-as-measurement or public constructor from raw facts for a validated seed exists.
// Computing from these facts yields data only, never a Train admission token.
struct CandidateWindowArithmeticSeed {
    std::size_t inputIndex=0;
    std::uint64_t firstFrame=0,lastFrame=0,referenceFrameCount=0;
    CandidateWindowGridBinding grid;
    std::vector<InputParameters> initialParameters;
    std::optional<TrainingCapacitance> initialCapacitance;
};
struct CandidateWindowCandidateFacts {
    CandidateWindowGridBinding grid;
    std::uint64_t declaredMaximumSamples=0;
    std::vector<InputParameters> parameters;
    std::optional<TrainingCapacitance> capacitance;
};
struct CandidateFrameWindow {
    std::uint64_t firstFrame=0,lastFrame=0;
    bool operator==(const CandidateFrameWindow&) const=default;
};
enum class CandidateWindowComputationStatus {
    Computed,Unsupported,InvalidSpecification,InputMismatch,BudgetExhausted,ArithmeticOverflow,OutsideEnvelope
};
struct CandidateWindowComputation {
    CandidateWindowComputationStatus status=CandidateWindowComputationStatus::InvalidSpecification;
    std::vector<std::string> reasons;
    std::optional<CandidateFrameWindow> window;
    std::int64_t shiftFrames=0;
    std::uint64_t uncertaintyFrames=0,lengthChange=0,synapseCountChange=0,resistanceChanges=0,capacitanceChanges=0,featureTerms=0;
};
CandidateWindowComputation CalculateCandidateWindowArithmetic(const CandidateWindowPolicy&,
    const CandidateWindowArithmeticSeed&,const CandidateWindowCandidateFacts&);
// The native/reference bridge is the ONLY future minting boundary. STEP1
// adds no minting factory; raw arithmetic success cannot construct this type.
class CandidateWindowReferenceSeed {
public:
    CandidateWindowReferenceSeed(const CandidateWindowReferenceSeed&)=default;
    CandidateWindowReferenceSeed& operator=(const CandidateWindowReferenceSeed&)=default;
    const std::string& ReferenceProofId() const {return referenceProofId;}
    const std::string& OwnerFamilyBinding() const {return ownerFamilyBinding;}
private:
    CandidateWindowReferenceSeed(CandidateWindowArithmeticSeed value,std::string proof,std::string family)
        :arithmetic(std::move(value)),referenceProofId(std::move(proof)),ownerFamilyBinding(std::move(family)){}
    CandidateWindowArithmeticSeed arithmetic;
    std::string referenceProofId,ownerFamilyBinding;
    friend class NativeCompositeTrainingAttempt;
    friend class CompositePortableTrainingProof;
    friend class NativeCompositeStateFactory;
    friend class CandidateWindowProposalReplayFactory;
    friend class CandidateWindowPortableReferenceFactory;
    friend CandidateWindowComputation ProposeCandidateWindow(const CandidateWindowPolicy&,
        const CandidateWindowReferenceSeed&,const CandidateWindowCandidateFacts&);
};
CandidateWindowComputation ProposeCandidateWindow(const CandidateWindowPolicy&,
    const CandidateWindowReferenceSeed&,const CandidateWindowCandidateFacts&);
const char* CandidateWindowComputationStatusName(CandidateWindowComputationStatus);
}
