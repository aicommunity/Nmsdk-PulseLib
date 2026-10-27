#pragma once
#include "ProbeObservation.h"
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
    std::optional<std::size_t> selectedInputIndex;
};
struct ReferenceEvidence {
    PreparedVerificationProbe prepared;
    ProbeObservation observation;
};
void ValidateVerificationProtocol(const VerificationProtocol& protocol);
// Shared production validation for search and final verification. Arrival is
// relative to the actual protocol origin, not a predicted cable delay.
struct ValidatedProbeMeasurement { double rawPotential,arrival,plateauEnd; };
ValidatedProbeMeasurement ValidateProbeMeasurement(const VerificationProtocol& protocol,
    std::size_t inputIndex,const PreparedVerificationProbe& prepared,const ProbeObservation& observation);
struct CapturedInputReference {
    std::string id, runId, trialId, effectiveModelRevision;
    double rawPotential=0.;
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
    const VerificationProtocol& Protocol() const { return protocol; }
    const std::vector<CapturedInputReference>& Inputs() const { return inputs; }
    const std::string& InitialModelRevision() const { return initialModelRevision; }
    std::size_t InitialModelEpoch() const { return initialModelEpoch; }
private:
    CapturedReferences()=default;
    VerificationProtocol protocol;
    std::vector<CapturedInputReference> inputs;
    std::string initialModelRevision;
    std::size_t initialModelEpoch=0;
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
    unsigned SchemaVersion() const { return 1; }
    bool Exact() const { return exact; }
    const std::string& ModelRevision() const { return modelRevision; }
    const std::string& ProtocolRevision() const { return protocol.revision; }
    const VerificationProtocol& Protocol() const { return protocol; }
    const std::string& ReferenceModelRevision() const { return referenceModelRevision; }
    std::size_t ReferenceModelEpoch() const { return referenceModelEpoch; }
    const std::vector<InputVerificationResidual>& Residuals() const { return residuals; }
    const VerificationTolerances& Tolerances() const { return tolerances; }
    bool Matches(const std::string& currentModel,const CapturedReferences& currentReferences,
                 const VerificationTolerances& currentTolerances,ModelStateKind kind,std::size_t currentEpoch) const;
    std::size_t ModelEpoch() const { return modelEpoch; }
private:
    TrainingCertificate()=default;
    friend class FinalVerificationSession;
    bool exact=false;
    std::size_t modelEpoch=0;
    std::size_t referenceModelEpoch=0;
    std::string modelRevision, referenceModelRevision;
    VerificationProtocol protocol;
    VerificationTolerances tolerances;
    std::vector<CapturedInputReference> referenceInputs;
    std::vector<InputVerificationResidual> residuals;
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
    void Invalidate(const std::string& newBaseRevision);
    void ExternalStop();
    void ExhaustBudget();
    VerificationStatus Status() const { return status; }
    const std::optional<TrainingCertificate>& Certificate() const { return certificate; }
    const std::vector<RejectedVerificationProbe>& Rejected() const { return rejected; }
    std::size_t IssuedProbes() const { return issued; }
    std::size_t AcceptedInputs() const;
    std::size_t ModelEpoch() const { return epoch; }
    bool WaitingForObservation() const { return bool(armed); }
private:
    struct Accepted { std::string trialId,effectiveRevision;double raw,arrival,plateauEnd; };
    CapturedReferences references;
    VerificationConfig config;
    VerificationStatus status=VerificationStatus::Collecting;
    std::vector<std::size_t> order;
    std::vector<std::optional<Accepted>> accepted;
    std::optional<VerificationRequest> pending;
    std::optional<PreparedVerificationProbe> armed;
    std::optional<TrainingCertificate> certificate;
    std::vector<RejectedVerificationProbe> rejected;
    std::set<std::string> seenTrials;
    std::size_t position=0,issued=0,serial=0,epoch=0,invalidCount=0;
    bool budgetStopped=false;
    void Reject(const std::string& reason);
    void Finish();
};
const char* VerificationStatusName(VerificationStatus status);
}
