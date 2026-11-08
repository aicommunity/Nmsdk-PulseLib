#pragma once
#include "FinalVerification.h"
#include "ResponseDomainAssociation.h"
#include "MatchedOmissionPolicy.h"

namespace NMSDK::TimeLearner {

struct MatchedOmissionSpecification {
    unsigned schemaVersion=1;
    std::string runId,groupId,sampleId,protocolRevision,patternHash,inputId;
    std::string baseModelRevision,effectiveModelRevision,resetBinding,modelInstanceBinding;
    std::size_t modelEpoch=0,assignedInput=0,channel=0;
    ProbeRouting routing=ProbeRouting::ClassicBurst;
    std::vector<std::string> stimulusIds,physicalChannelPaths;
    StimulusProfile stimulusProfile;
    double duration=0.,minimumAmplitude=1e-6;
};

// Built from actual native callbacks and a successfully closed native packet.
// The pure validator checks these records; it never drives a component. The
// producer must obtain instance/reset bindings from its live native context.
struct MatchedOmissionTrace {
    std::string groupId,sourcePatternHash,resetBinding,modelInstanceBinding,currentBaseRevision;
    PreparedVerificationProbe prepared;
    ResponseDomainObservation observation;
    std::vector<std::string> physicalChannelPaths;
    std::vector<std::size_t> presentedOrdinals;
    StimulusProfile stimulusProfile;
    std::vector<double> stimulusSamples;
    bool nativeCompleted=false,fullMaskRestored=false;
};
struct MatchedOmissionWitness {
    MatchedOmissionTrace fullBefore,fullAfter;
    // Exactly one trace omitting each original ordinal, in any vector order.
    std::vector<MatchedOmissionTrace> omitEachInput;
};
enum class OmissionInfluence { Supported, NotSupported, Unknown };
enum class MatchedOmissionValidity {
    Valid,Missing,Ambiguous,Clipped,Incomplete,InvalidContext,NonRepeatable,
    BudgetExhausted,Unsupported
};
struct FullMaximumSupport {
    ResponseMaximum maximum;
    std::size_t comparisonFirstSample=0,comparisonLastSample=0;
    std::vector<OmissionInfluence> influenceByInput;
    std::vector<std::size_t> supportedInputs;
    bool unknown=false,precedesAssignedStimulus=false,clipped=false;
};
struct MatchedOmissionResult {
    MatchedOmissionValidity validity=MatchedOmissionValidity::InvalidContext;
    std::vector<std::string> reasons;
    std::vector<FullMaximumSupport> maxima;
    std::optional<std::size_t> selectedFullMaximum;
    // A value from fullBefore only. Never a maximum of a difference trace.
    std::optional<ResponseMaximum> measuredMaximum;
    static constexpr const char* MethodId(){return "matched-omission-assigned-maximum-v1";}
};
// Cheap preflight before arming physical probes. Does not require a prepared
// effective revision; Select checks the complete bound context afterwards.
MatchedOmissionResult ValidateMatchedOmissionPolicy(const MatchedOmissionPolicy& policy,
    const StimulusProfile& source,std::size_t inputCount);
MatchedOmissionResult SelectMatchedOmissionMaximum(const MatchedOmissionSpecification& specification,
    const MatchedOmissionPolicy& policy,const MatchedOmissionWitness& witness);
const char* MatchedOmissionValidityName(MatchedOmissionValidity validity);
const char* OmissionInfluenceName(OmissionInfluence influence);
}
