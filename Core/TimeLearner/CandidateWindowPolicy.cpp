#include "CandidateWindowPolicy.h"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
using Status=CandidateWindowComputationStatus;
constexpr auto Max=CandidateWindowMaximumExactCount;
struct Failure {Status status;const char* reason;};
void Require(bool condition,Status status,const char* reason){if(!condition)throw Failure{status,reason};}
std::uint64_t Add(std::uint64_t a,std::uint64_t b) {
    Require(a<=Max && b<=Max && b<=Max-a,Status::ArithmeticOverflow,"candidate_window_count_addition_overflow");return a+b;
}
std::uint64_t Multiply(std::uint64_t a,std::uint64_t b) {
    Require(a<=Max && b<=Max && (!a || b<=Max/a),Status::ArithmeticOverflow,"candidate_window_count_product_overflow");return a*b;
}
std::int64_t SignedAdd(std::int64_t a,std::int64_t b) {
    constexpr auto bound=std::int64_t(Max);
    Require(a>=-bound && a<=bound && b>=-bound && b<=bound &&
        (b>=0?a<=bound-b:a>=-bound-b),Status::ArithmeticOverflow,"candidate_window_signed_frame_overflow");return a+b;
}
std::uint64_t Difference(int a,int b) {return a>=b?std::uint64_t(std::int64_t(a)-b):std::uint64_t(std::int64_t(b)-a);}
void Grid(const CandidateWindowGridBinding& g) {
    Require(g.frameOriginPolicy=="native-full-before-frame-zero-v1" &&
        g.sourcePhasePolicy=="same-declared-source-native-events-measured-v1" &&
        g.resetBinding=="native-neuron-dataset-reset-before-children-v1",Status::Unsupported,"candidate_window_unknown_grid_or_phase_policy");
    Require(g.frequencyHz>0 && g.frequencyHz<=Max && !g.sourceOrigin.empty() && !g.sourcePatternBinding.empty() &&
        !g.sourceProfileBinding.empty(),Status::InvalidSpecification,"candidate_window_invalid_grid_or_source_binding");
}
void Parameters(const std::vector<InputParameters>& values) {
    for(const auto& p:values)Require(p.length>0 && p.synapses>0 && std::isfinite(p.resistance) && p.resistance>0.,
        Status::InvalidSpecification,"candidate_window_invalid_parameter_feature");
}
void Validate(const CandidateWindowPolicy& policy,const CandidateWindowArithmeticSeed& seed,const CandidateWindowCandidateFacts& current,
    CandidateWindowComputation& result) {
    Require(policy.schemaVersion==1 && policy.policyId=="reference-anchored-candidate-window-v1" &&
        policy.seedRule=="immutable_reference_full_before" && policy.coordinateMode=="relative_native_frame_interval" &&
        policy.proposalsPerLogicalRequest==1,Status::Unsupported,"candidate_window_unknown_policy_or_strategy");
    Require(policy.maximumFeatureTerms>0 && policy.maximumFeatureTerms<=Max,
        Status::InvalidSpecification,"candidate_window_invalid_feature_budget");
    const auto n=seed.initialParameters.size();
    Require(n>0 && seed.inputIndex<n && current.parameters.size()==n && policy.inputs.size()==n,
        Status::InputMismatch,"candidate_window_parameter_or_policy_dimensions");
    // Check the actual container sizes BEFORE scanning a vector/map or forming
    // n*3. The budget counts L/Ns/Rs terms and retained initial/current C0 rows.
    Require(n<=Max && n<=policy.maximumFeatureTerms/3,Status::BudgetExhausted,"candidate_window_feature_budget_exhausted");
    result.featureTerms=std::uint64_t(n)*3;
    for(const auto* c:{&seed.initialCapacitance,&current.capacitance})if(*c) {
        const auto size=(*c)->membraneScales.size();Require(size<=Max && size<=policy.maximumFeatureTerms-result.featureTerms,
            Status::BudgetExhausted,"candidate_window_feature_budget_exhausted");result.featureTerms+=std::uint64_t(size);
    }
    Grid(seed.grid);Grid(current.grid);Require(seed.grid==current.grid,Status::InputMismatch,"candidate_window_grid_or_source_changed");
    Require(seed.referenceFrameCount>=3 && seed.referenceFrameCount<=Max && seed.firstFrame>0 && seed.firstFrame<=seed.lastFrame &&
        seed.lastFrame<seed.referenceFrameCount-1,Status::InvalidSpecification,"candidate_window_invalid_reference_frame_seed");
    Require(current.declaredMaximumSamples>=3 && current.declaredMaximumSamples<=Max,
        Status::InvalidSpecification,"candidate_window_invalid_sample_budget");
    for(const auto& p:policy.inputs) {
        for(const auto value:{p.delayFramesPerSegment,p.baseLeftGuardFrames,p.baseRightGuardFrames,p.lengthGuardFrames,
            p.synapseCountGuardFrames,p.resistanceChangeGuardFrames,p.capacitanceChangeGuardFrames,p.hardFirstFrame,
            p.hardLastFrame,p.maximumWindowFrames})Require(value<=Max,Status::InvalidSpecification,"candidate_window_inexact_policy_count");
        Require(p.hardFirstFrame<p.hardLastFrame && p.hardLastFrame<current.declaredMaximumSamples && p.maximumWindowFrames>=2,
            Status::InvalidSpecification,"candidate_window_invalid_declared_envelope");
    }
    Parameters(seed.initialParameters);Parameters(current.parameters);
    try {ValidateTrainingCapacitance(seed.initialCapacitance);ValidateTrainingCapacitance(current.capacitance);}
    catch(const std::invalid_argument&) {throw Failure{Status::InvalidSpecification,"candidate_window_invalid_capacitance_feature"};}
    Require(bool(seed.initialCapacitance)==bool(current.capacitance),Status::InputMismatch,"candidate_window_capacitance_family_changed");
    if(seed.initialCapacitance)Require(seed.initialCapacitance->origin==current.capacitance->origin &&
        seed.initialCapacitance->maximumRelativeChange==current.capacitance->maximumRelativeChange,
        Status::InputMismatch,"candidate_window_capacitance_family_changed");
}
}
CandidateWindowComputation CalculateCandidateWindowArithmetic(const CandidateWindowPolicy& policy,
    const CandidateWindowArithmeticSeed& seed,const CandidateWindowCandidateFacts& current) {
    CandidateWindowComputation result;
    try {
        Validate(policy,seed,current,result);const auto& p=policy.inputs[seed.inputIndex];
        for(std::size_t i=0;i<current.parameters.size();++i) {
            const auto& before=seed.initialParameters[i];const auto& after=current.parameters[i];
            result.lengthChange=Add(result.lengthChange,Difference(before.length,after.length));
            result.synapseCountChange=Add(result.synapseCountChange,Difference(before.synapses,after.synapses));
            result.resistanceChanges=Add(result.resistanceChanges,before.resistance!=after.resistance?1:0);
        }
        result.capacitanceChanges=EquivalentTrainingCapacitance(seed.initialCapacitance,current.capacitance)?0:1;
        const auto magnitude=Multiply(Difference(current.parameters[seed.inputIndex].length,seed.initialParameters[seed.inputIndex].length),p.delayFramesPerSegment);
        result.shiftFrames=current.parameters[seed.inputIndex].length>=seed.initialParameters[seed.inputIndex].length?
            std::int64_t(magnitude):-std::int64_t(magnitude);
        result.uncertaintyFrames=Add(Add(Multiply(result.lengthChange,p.lengthGuardFrames),Multiply(result.synapseCountChange,p.synapseCountGuardFrames)),
            Add(Multiply(result.resistanceChanges,p.resistanceChangeGuardFrames),Multiply(result.capacitanceChanges,p.capacitanceChangeGuardFrames)));
        const auto left=Add(p.baseLeftGuardFrames,result.uncertaintyFrames),right=Add(p.baseRightGuardFrames,result.uncertaintyFrames);
        const auto first=SignedAdd(SignedAdd(std::int64_t(seed.firstFrame),result.shiftFrames),-std::int64_t(left));
        const auto last=SignedAdd(SignedAdd(std::int64_t(seed.lastFrame),result.shiftFrames),std::int64_t(right));
        Require(first>=0 && last>=0 && first<last && std::uint64_t(first)>=p.hardFirstFrame && std::uint64_t(last)<=p.hardLastFrame,
            Status::OutsideEnvelope,"candidate_window_outside_declared_envelope");
        const auto width=Add(std::uint64_t(last-first),1);
        Require(width<=p.maximumWindowFrames,Status::OutsideEnvelope,"candidate_window_width_exceeds_policy");
        result.window=CandidateFrameWindow{std::uint64_t(first),std::uint64_t(last)};result.status=Status::Computed;
    }
    catch(const Failure& failure){result.status=failure.status;result.reasons.push_back(failure.reason);result.window.reset();}
    return result;
}
CandidateWindowComputation ProposeCandidateWindow(const CandidateWindowPolicy& policy,const CandidateWindowReferenceSeed& seed,
    const CandidateWindowCandidateFacts& current) {
    // Even this typed overload returns arithmetic DATA, not a live/native proof.
    // Matching expected owner/proof and verifying current physical map/phase is
    // the responsibility of the future admitted native/portable bridge.
    if(seed.referenceProofId.empty() || seed.ownerFamilyBinding.empty()) {
        CandidateWindowComputation result;result.status=Status::InputMismatch;
        result.reasons.push_back("candidate_window_reference_binding_missing");return result;
    }
    return CalculateCandidateWindowArithmetic(policy,seed.arithmetic,current);
}
const char* CandidateWindowComputationStatusName(CandidateWindowComputationStatus status) {
    switch(status) {
    case Status::Computed:return "computed";
    case Status::Unsupported:return "unsupported";
    case Status::InvalidSpecification:return "invalid_specification";
    case Status::InputMismatch:return "input_mismatch";
    case Status::BudgetExhausted:return "budget_exhausted";
    case Status::ArithmeticOverflow:return "arithmetic_overflow";
    case Status::OutsideEnvelope:return "outside_envelope";
    }return "unknown";
}
}
