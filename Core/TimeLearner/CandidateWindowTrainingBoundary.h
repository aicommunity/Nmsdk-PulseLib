#pragma once
#include "CandidateWindowPolicy.h"
#include "CandidateWindowMeasurementContract.h"
#include "CompositeTrainingMeasurement.h"
#include <memory>

namespace NMSDK::TimeLearner {
class CapturedReferences;
class CompositePortableTrainingProof;
class NativeCompositeStateFactory;
class NativeCompositeTrainingAttempt;
class CandidateWindowProposalReplayFactory;

enum class CandidateWindowCoordinateMode { ReferenceTime,CandidateFrame };
// Declaration DATA, not evidence of captured frames or native admission.
// Exactly one optional coordinate is allowed, as selected by coordinateMode.
struct CandidateWindowBasinSpecification {
    unsigned schemaVersion=CandidateWindowBasinSpecificationSchema;
    std::string methodId=DeclaredWindowBasinResult::MethodId();
    std::string groupId,inputId;
    std::size_t assignedInput=0,channel=0;
    CandidateWindowCoordinateMode coordinateMode=CandidateWindowCoordinateMode::ReferenceTime;
    std::optional<CompositeInputWindow> referenceTime;
    std::optional<CandidateFrameWindow> candidateFrame;
};

// Actual readback DATA retained beside the full semantic graph identity.
// Neither path strings nor these public facts attest graph membership.
struct CandidateWindowInputBankReadback {
    std::size_t inputOrdinal=0;
    std::string inputId,stimulusId,receiverPath,bankPath;
    std::size_t attachmentSegment=0;
    std::vector<std::string> excitationSynapsePaths;
    bool operator==(const CandidateWindowInputBankReadback&) const=default;
};
struct CandidateWindowPhysicalReadback {
    std::vector<InputParameters> parameters;
    std::vector<CandidateWindowInputBankReadback> inputBanks;
    std::optional<TrainingCapacitance> capacitance;
};

// Complete serialization inputs, still untrusted until native/portable replay.
// No waveform, residual, current amplitude or suggested maximum is accepted.
struct CandidateWindowProposalRecord {
    unsigned schemaVersion=1;
    std::string policyId="reference-anchored-candidate-window-v1";
    std::string sourceOrigin,ownerFamilyBinding,referenceProofId;
    std::string protocolBinding,policyBinding,candidateBinding;
    std::string referenceGroupId,attemptId,runId,trialId,groupId,inputId;
    std::size_t inputIndex=0,channel=0,modelEpoch=0;
    CompositeMeasurementPurpose purpose=CompositeMeasurementPurpose::Search;
    std::string candidateModelRevision,windowRecipeId,measurementInstanceId;
    CandidateWindowArithmeticSeed reference;
    CandidateWindowCandidateFacts candidate;
    CandidateWindowComputation computation;
};

// A private staged reference may exist before the last input is captured, but
// search/final can receive only this COMPLETE all-input set. Copying a set does
// not recreate live authority or bypass expected family/proof checks.
class CandidateWindowReferenceSet {
public:
    CandidateWindowReferenceSet(const CandidateWindowReferenceSet&)=default;
    CandidateWindowReferenceSet& operator=(const CandidateWindowReferenceSet&)=default;
    std::size_t InputCount() const {return seeds.size();}
    const std::string& SourceOrigin() const {return sourceOrigin;}
    const std::string& OwnerFamilyBinding() const {return ownerFamilyBinding;}
private:
    CandidateWindowReferenceSet(std::vector<CandidateWindowReferenceSeed> values,
        std::string source,std::string family,std::vector<std::string> groups,
        std::string protocol,std::string immutableSource):seeds(std::move(values)),
        sourceOrigin(std::move(source)),ownerFamilyBinding(std::move(family)),referenceGroupIds(std::move(groups)),
        protocolBinding(std::move(protocol)),sourceBindingJson(std::move(immutableSource)){}
    std::vector<CandidateWindowReferenceSeed> seeds;
    std::string sourceOrigin,ownerFamilyBinding;
    std::vector<std::string> referenceGroupIds;
    std::string protocolBinding,sourceBindingJson;
    friend class CapturedReferences;
    friend class CompositePortableTrainingProof;
    friend class NativeCompositeStateFactory;
    friend class NativeCompositeTrainingAttempt;
    friend class CandidateWindowProposalReplayFactory;
};

// Validated archived candidate DATA; never a live lease or proof of native
// execution. Membership/control-knob values are replayed against the complete
// semantic identity, not trusted from public paths or copied readback numbers.
class ReplayedCandidateWindowBinding {
public:
    ReplayedCandidateWindowBinding(const ReplayedCandidateWindowBinding&)=default;
    ReplayedCandidateWindowBinding& operator=(const ReplayedCandidateWindowBinding&)=default;
    const CandidateWindowCandidateFacts& Facts() const {return facts;}
    const std::string& ModelRevision() const {return modelRevision;}
    const std::string& CanonicalBindingJson() const {return bindingJson;}
private:
    ReplayedCandidateWindowBinding(CandidateWindowCandidateFacts value,std::string revision,std::string json)
        :facts(std::move(value)),modelRevision(std::move(revision)),bindingJson(std::move(json)){}
    CandidateWindowCandidateFacts facts;
    std::string modelRevision,bindingJson;
    friend class CandidateWindowProposalReplayFactory;
};

// Only an actual-owner readback factory may create this wrapper. Arithmetic
// DATA and decoded JSON cannot construct it; every use must recheck its live
// owner, graph identity, source, bank map and C0 at the legal boundary.
class NativeCandidateWindowSnapshot {
public:
    NativeCandidateWindowSnapshot(const NativeCandidateWindowSnapshot&)=default;
    NativeCandidateWindowSnapshot& operator=(const NativeCandidateWindowSnapshot&)=default;
private:
    NativeCandidateWindowSnapshot(CandidateWindowCandidateFacts value,CandidateWindowPhysicalReadback physical,
        std::string binding,std::shared_ptr<const CompositeTrainingDetail::Identity> owner)
        :facts(std::move(value)),readback(std::move(physical)),candidateBindingJson(std::move(binding)),identity(std::move(owner)){}
    CandidateWindowCandidateFacts facts;
    CandidateWindowPhysicalReadback readback;
    std::string candidateBindingJson;
    std::shared_ptr<const CompositeTrainingDetail::Identity> identity;
    friend class NativeCompositeTrainingAttempt;
};

// This process-local value must be frozen BEFORE the first N+2 queue. The
// constructor is inaccessible to callers, including successful pure policy
// computations. Native measurement admission must ALSO require independent
// portable proposal replay; this type alone does not accept a scalar.
class PreparedCandidateWindowProposal {
public:
    PreparedCandidateWindowProposal(const PreparedCandidateWindowProposal&)=default;
    PreparedCandidateWindowProposal& operator=(const PreparedCandidateWindowProposal&)=default;
    const CandidateWindowProposalRecord& Record() const {return record;}
private:
    PreparedCandidateWindowProposal(CandidateWindowProposalRecord value,
        std::shared_ptr<const CompositeTrainingDetail::Identity> owner)
        :record(std::move(value)),identity(std::move(owner)){}
    CandidateWindowProposalRecord record;
    std::shared_ptr<const CompositeTrainingDetail::Identity> identity;
    friend class NativeCompositeTrainingAttempt;
};

// Replay of proposal arithmetic/context is separate from a live owner lease,
// native execution and geometry/Train admission. No decoder/replay factory is
// supplied by this types-only commit, so public DATA cannot mint this wrapper.
class ReplayedCandidateWindowProposal {
public:
    ReplayedCandidateWindowProposal(const ReplayedCandidateWindowProposal&)=default;
    ReplayedCandidateWindowProposal& operator=(const ReplayedCandidateWindowProposal&)=default;
    const CandidateWindowProposalRecord& Record() const {return record;}
private:
    explicit ReplayedCandidateWindowProposal(CandidateWindowProposalRecord value):record(std::move(value)){}
    CandidateWindowProposalRecord record;
    friend class CandidateWindowProposalReplayFactory;
};
}
