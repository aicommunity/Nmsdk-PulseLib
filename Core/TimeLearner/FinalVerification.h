#pragma once
#include "ProbeObservation.h"
#include "CompositeTrainingMeasurement.h"
#include <optional>
#include <set>

namespace NMSDK::TimeLearner {
enum class ProbeRouting { ClassicBurst, BranchSoloBurst };
enum class ModelStateKind { LearnedSolo, Recognition };
enum class VerificationStatus { Collecting, Exact, OutsideTolerance, InvalidObservation, BudgetExhausted, ExternalStop };

struct VerificationInput { std::string id, stimulusId; bool operator==(const VerificationInput&) const=default; };
struct VerificationProtocol {
    unsigned schemaVersion=1;
    ProbeRouting routing=ProbeRouting::ClassicBurst;
    std::string revision, sampleId, patternHash, originStimulusId;
    // Last input is the fixed reference. Schema 3 follows physical/chronological
    // input order: Classic input i owns soma channel i; Branch uses channel 0.
    std::vector<VerificationInput> inputs;
    std::vector<std::string> stimulusIds;
    double maximumSampleGap=0., minimumAmplitude=1e-6;
    std::optional<StimulusProfile> stimulusProfile;
    WaveAssignmentPolicy waveAssignment=WaveAssignmentPolicy::WindowOnly;
    PeakSelectionPolicy peakSelection=PeakSelectionPolicy::IsolatedWindow;
    std::optional<CompositeMeasurementContract> compositeMeasurement={};
    bool operator==(const VerificationProtocol&) const=default;
};
// Adapter-produced declaration after native routing/reset at a legal boundary.
// Base revision covers the learned model; effective revision also covers the
// transient stimulus routing of this particular probe.
struct PreparedVerificationProbe {
    ProbeRouting routing=ProbeRouting::ClassicBurst;
    std::string inputId, protocolRevision, baseModelRevision, effectiveModelRevision;
    std::size_t modelEpoch=0;
    // Actual Branch solo selector, produced by the native adapter. Schema 3
    // binds it to protocol input order; Classic measures an unmuted full burst.
    std::optional<std::size_t> selectedInputIndex={};
};
struct ReferenceEvidence {
    PreparedVerificationProbe prepared;
    ProbeObservation observation;
};
void ValidateVerificationProtocol(const VerificationProtocol& protocol);
// Shared production validation for search and final verification. Arrival is
// relative to the actual protocol origin, not a predicted cable delay.
// Distinct assigned stimuli cannot reuse intersecting peak/plateau samples
// on one measured channel. Equal times on different channels remain legal.
struct ValidatedProbeMeasurement { double rawPotential,arrival,plateauEnd; };
ValidatedProbeMeasurement ValidateProbeMeasurement(const VerificationProtocol& protocol,
    std::size_t inputIndex,const PreparedVerificationProbe& prepared,const ProbeObservation& observation);
// A closed native single-input probe with only an unassigned full burst may
// reject a search candidate. This never accepts a measurement or certificate.
// Context, coverage, numerical, clipped-window and other failures return false.
bool CanAdvanceUnassignedBurstCandidate(const VerificationProtocol& protocol,std::size_t inputIndex,
    const PreparedVerificationProbe& prepared,const ProbeObservation& observation);
struct CapturedInputReference {
    std::string id, runId, trialId, effectiveModelRevision;
    double rawPotential=0.;
    std::string measurementGroupId,semanticProofId;
    bool operator==(const CapturedInputReference&) const=default;
};
class CapturedReferences {
public:
    CapturedReferences(const CapturedReferences&)=default;
    CapturedReferences(CapturedReferences&&)=default;
    CapturedReferences& operator=(const CapturedReferences&)=default;
    CapturedReferences& operator=(CapturedReferences&&)=default;
    static CapturedReferences Capture(VerificationProtocol protocol,
                                      const std::vector<ReferenceEvidence>& evidence);
    static CapturedReferences CaptureComposite(VerificationProtocol protocol,
        const std::vector<ValidatedCompositeMeasurement>& evidence);
    const VerificationProtocol& Protocol() const { return protocol; }
    const std::vector<CapturedInputReference>& Inputs() const { return inputs; }
    const std::string& InitialModelRevision() const { return initialModelRevision; }
    std::size_t InitialModelEpoch() const { return initialModelEpoch; }
    const std::string& CompositeOwnerScope() const {return compositeOwnerScope;}
    const std::string& CompositeSourceOrigin() const {return compositeSourceOrigin;}
private:
    CapturedReferences()=default;
    VerificationProtocol protocol;
    std::vector<CapturedInputReference> inputs;
    std::string initialModelRevision;
    std::size_t initialModelEpoch=0;
    std::string compositeOwnerScope,compositeSourceOrigin;
};

struct VerificationTolerances {
    double amplitudeAbsolute=1e-5, amplitudeRelative=0., sync=.02;
    bool operator==(const VerificationTolerances&) const=default;
};
struct VerificationConfig {
    // Unique verification-attempt identity, not a reusable condition hash.
    std::string runId, currentModelRevision;
    std::size_t maximumProbes=0, maximumInvalidObservations=0;
    std::size_t modelEpoch=0;
    VerificationTolerances tolerances;
};
struct VerificationRequest {
    std::string runId, trialId, inputId, sampleId, patternHash, protocolRevision, baseModelRevision;
    ProbeRouting routing=ProbeRouting::ClassicBurst;
    std::size_t epoch=0;
    std::size_t inputIndex=0;
};
struct InputVerificationResidual {
    std::string inputId, trialId, effectiveModelRevision;
    double targetRawPotential=0., measuredRawPotential=0., amplitudeResidual=0., amplitudeTolerance=0.;
    double timingResidualLow=0., timingResidualHigh=0.;
    bool withinAmplitude=false, withinTiming=false;
};
class TrainingCertificate {
public:
    TrainingCertificate(const TrainingCertificate&)=default;
    TrainingCertificate(TrainingCertificate&&)=default;
    TrainingCertificate& operator=(const TrainingCertificate&)=default;
    TrainingCertificate& operator=(TrainingCertificate&&)=default;
    unsigned SchemaVersion() const { return protocol.schemaVersion==4?2:1; }
    bool Exact() const { return exact; }
    const std::string& ModelRevision() const { return modelRevision; }
    const std::string& ProtocolRevision() const { return protocol.revision; }
    const VerificationProtocol& Protocol() const { return protocol; }
    const std::string& ReferenceModelRevision() const { return referenceModelRevision; }
    std::size_t ReferenceModelEpoch() const { return referenceModelEpoch; }
    const std::vector<InputVerificationResidual>& Residuals() const { return residuals; }
    const VerificationTolerances& Tolerances() const { return tolerances; }
    const std::vector<std::string>& CompositeProofIds() const {return compositeProofIds;}
    std::vector<std::string> CompositeReferenceProofIds() const {std::vector<std::string> ids;for(const auto& r:referenceInputs)ids.push_back(r.semanticProofId);return ids;}
    const std::string& CompositeSourceOrigin() const {return compositeSourceOrigin;}
    const std::string& CompositeOwnerFamilyBinding() const {return compositeOwnerFamilyBinding;}
    bool Matches(const std::string& currentModel,const CapturedReferences& currentReferences,
                 const VerificationTolerances& currentTolerances,ModelStateKind kind,std::size_t currentEpoch) const;
    std::size_t ModelEpoch() const { return modelEpoch; }
private:
    TrainingCertificate()=default;
    friend class FinalVerificationSession;
    bool exact=false;
    std::size_t modelEpoch=0;
    std::size_t referenceModelEpoch=0;
    std::string modelRevision, referenceModelRevision,compositeSourceOrigin,compositeOwnerFamilyBinding;
    VerificationProtocol protocol;
    VerificationTolerances tolerances;
    std::vector<CapturedInputReference> referenceInputs;
    std::vector<InputVerificationResidual> residuals;
    std::vector<std::string> compositeProofIds;
};
struct RejectedVerificationProbe { std::string trialId, reason; };

// Stateful final-verification controller. It queues input-specific probes;
// adapters alone own Rdk edits, reset, scheduling, and revision production.
// It never concludes selectivity or searches/changes a physical parameter.
class FinalVerificationSession {
public:
    FinalVerificationSession(CapturedReferences references,VerificationConfig configuration);
    FinalVerificationSession(const FinalVerificationSession&)=delete;
    FinalVerificationSession& operator=(const FinalVerificationSession&)=delete;
    std::optional<VerificationRequest> NextRequest();
    void Arm(const PreparedVerificationProbe& prepared);
    void Consume(const ProbeObservation& observation,const std::string& currentBaseRevision);
    void ArmComposite(const PreparedCompositeMeasurement& prepared);
    void ConsumeComposite(const ValidatedCompositeMeasurement& measurement,const std::string& currentBaseRevision);
    void RejectComposite(const std::string& reason);
    void Invalidate(const std::string& newBaseRevision);
    void ExternalStop();
    void ExhaustBudget();
    VerificationStatus Status() const { return status; }
    const std::optional<TrainingCertificate>& Certificate() const { return certificate; }
    const std::vector<RejectedVerificationProbe>& Rejected() const { return rejected; }
    std::size_t IssuedProbes() const { return issued; }
    // Counts rejected observations/preparations, not revision invalidation
    // records. An owning training attempt shares this budget with its search.
    std::size_t InvalidObservations() const { return invalidCount; }
    std::size_t AcceptedInputs() const;
    std::size_t AcceptedCompositeObservations() const {return acceptedComposite;}
    std::size_t ModelEpoch() const { return epoch; }
    bool WaitingForObservation() const { return bool(armed) || bool(compositeArmed); }
private:
    struct Accepted { std::string trialId,effectiveRevision;double raw,arrival,plateauEnd;std::string compositeProofId,candidateWitnessId; };
    CapturedReferences references;
    VerificationConfig config;
    VerificationStatus status=VerificationStatus::Collecting;
    std::vector<std::size_t> order;
    std::vector<std::optional<Accepted>> accepted;
    std::optional<VerificationRequest> pending;
    std::optional<PreparedVerificationProbe> armed;
    std::optional<PreparedCompositeMeasurement> compositeArmed;
    std::optional<TrainingCertificate> certificate;
    std::vector<RejectedVerificationProbe> rejected;
    std::set<std::string> seenTrials;
    std::set<std::string> seenCompositeGroups,seenCompositeProofs;
    std::size_t position=0,issued=0,serial=0,epoch=0,invalidCount=0,acceptedComposite=0;
    bool budgetStopped=false;
    void Reject(const std::string& reason);
    // Admission remains method-specific; scalar acceptance/finish are private.
    void AcceptValidatedScalar(std::size_t index,double raw,double arrival,double plateauEnd);
    void CompleteValidatedInput();
    void Finish();
};
const char* VerificationStatusName(VerificationStatus status);
}
