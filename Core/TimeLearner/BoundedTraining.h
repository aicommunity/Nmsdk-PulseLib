#pragma once
#include "FinalVerification.h"
#include "TrainingParameters.h"
#include "ResistanceDomain.h"
#include "TrainingCapacitance.h"
#include <memory>

namespace NMSDK::TimeLearner {
enum class TrainingStatus { Searching, Verifying, Exact, SearchUnresolved, BudgetExhausted,
    InvalidModel, InvalidObservation, ExternalStop };
enum class InvalidMeasurementRecovery { RetrySameCandidate, AdvanceUnassignedBurst };
struct InputSearchBounds {
    int minimumLength=1,maximumLength=1,minimumSynapses=1,maximumSynapses=128;
    double minimumResistance=0.,maximumResistance=0.;
};
struct CapacitanceSearchPolicy {
    // Finite absolute C/C0 mesh. It includes the unchanged full state and
    // independent per-scale normalizer/length cursors; no trend is assumed.
    double minimumScale=0.8,maximumScale=1.2;
    unsigned subdivisionDepth=2;
    std::size_t maximumContexts=5,maximumMembranes=1024;
    // Total retained L/Ns/Rs seed entries, reserved equally among C contexts.
    std::size_t maximumCursorElements=131072;
    bool operator==(const CapacitanceSearchPolicy&) const=default;
};
struct BoundedTrainingConfig {
    std::string runId,initialModelRevision;
    std::size_t initialModelEpoch=0,maximumProbes=0,maximumEdits=0;
    std::size_t maximumCandidatesPerInput=0,maximumSweeps=1,maximumInvalidObservations=0;
    // maximumInvalidObservations is a recoverable-invalid budget for the
    // entire attempt, shared by search and every final verification sweep.
    // Finite declared seed mesh. Native N1 with nonzero depth may prioritize
    // measured-bracket refinements within the same overall candidate budget.
    // Exhaustion never proves infeasibility or global monotonicity.
    unsigned resistanceSubdivisionDepth=4;
    // One preserves historical protocol. New N0 protocols may explicitly
    // require repeated native anchor/input pairs for every measured candidate.
    std::size_t n0CandidateRepeats=1;
    Normalization normalization=Normalization::SynapseCount;
    VerificationTolerances tolerances;
    std::vector<InputSearchBounds> bounds;
    std::optional<CapacitanceSearchPolicy> capacitanceSearch={};
    // Default preserves compatibility. Advancing is restricted to a complete
    // unassigned native burst; it never skips reference capture or final inputs.
    InvalidMeasurementRecovery invalidMeasurementRecovery=InvalidMeasurementRecovery::RetrySameCandidate;
};
void ValidateInvalidMeasurementRecovery(InvalidMeasurementRecovery policy,std::size_t invalidAllowance);
enum class TrainingActionKind { Edit, Probe, ResistanceDomain };
struct TrainingAction {
    TrainingActionKind kind=TrainingActionKind::Probe;
    std::size_t serial=0,inputIndex=0,modelEpoch=0;
    std::string runId,trialId,baseModelRevision;
    std::vector<InputParameters> parameters;
    bool finalVerification=false;
    std::optional<TrainingCapacitance> capacitance={};
};
struct CandidateEvidence {
    std::size_t inputIndex=0,modelEpoch=0;
    InputParameters parameters;
    std::string modelRevision,trialId,anchorTrialId;
    double timingResidualLow=0.,timingResidualHigh=0.,amplitudeResidual=0.;
    double measuredRawPotential=0.;
    double parameterChangeCost=0.;
    bool withinTiming=false,withinAmplitude=false;
    std::vector<InputParameters> fullParameters;
    std::optional<TrainingCapacitance> capacitance={};
};
struct TrainingRejection { std::string trialId,reason; };
enum class CandidateRecoveryDisposition { CursorAdvanced, InvalidBudgetStop, RestoredBestStop };
struct CandidateRecoveryEvidence {
    std::size_t actionSerial=0,inputIndex=0,candidateInputIndex=0,sweep=0,modelEpoch=0,invalidBefore=0,invalidAfter=0;
    std::string runId,trialId,baseModelRevision,effectiveModelRevision;
    bool anchor=false;
    PeakValidity validity=PeakValidity::Missing;
    std::vector<InputParameters> fullParameters;
    std::optional<TrainingCapacitance> capacitance={};
    CandidateRecoveryDisposition disposition=CandidateRecoveryDisposition::InvalidBudgetStop;
    TrainingStatus statusAfter=TrainingStatus::InvalidObservation;
    std::string reasonAfter;
};
struct CandidateRepeatEvidence {
    std::size_t inputIndex=0,modelEpoch=0;
    std::string modelRevision;
    std::vector<InputParameters> fullParameters;
    std::vector<std::string> trialIds,anchorTrialIds;
    std::vector<double> rawPotentials;
    double minimumRawPotential=0.,maximumRawPotential=0.;
    bool withinTiming=false,withinAmplitude=false;
    bool complete=true;
    std::string reason="required_measurements_collected";
    std::optional<TrainingCapacitance> capacitance={};
};
// Shared preflight before paying for reference measurements. Uses the same
// bounds/physical-bank legality and tolerances as the actual decision session.
void ValidateBoundedTrainingInputs(const VerificationProtocol& protocol,const BoundedTrainingConfig& configuration,
                                   const std::vector<InputParameters>& initial);

void ValidateCapacitanceSearchPolicy(const CapacitanceSearchPolicy& policy);
void ValidateCapacitanceSearchPolicy(const BoundedTrainingConfig& configuration,ProbeRouting routing);

// Native preflight calls this before reference probes; the standalone decision
// session uses the same owner/policy contract.
void ValidateBoundedTrainingCapacitance(const BoundedTrainingConfig& configuration,
    const std::optional<TrainingCapacitance>& owned);

// Common decision controller. Adapters alone apply edits/read back the graph,
// prepare native probes and schedule children. No Rdk calls or physical reset
// live here. A local candidate is never an Exact result: the unchanged final
// graph must pass the independent all-input FinalVerificationSession.
class BoundedTrainingSession {
public:
    // Standalone callers may declare their own physical-domain responsibility.
    // Native N1 service always requests the explicit derived-domain actions.
    BoundedTrainingSession(CapturedReferences references,BoundedTrainingConfig config,
                           std::vector<InputParameters> initial,bool requireResistanceDomains=false,
                           std::optional<TrainingCapacitance> initialCapacitance={});
    std::optional<TrainingAction> NextAction();
    void Applied(std::size_t actionSerial,const std::vector<InputParameters>& effective,
                 const std::string& actualModelRevision,std::optional<TrainingCapacitance> effectiveCapacitance={});
    void DeclineEdit(std::size_t actionSerial,const std::string& reason,const std::string& unchangedModelRevision);
    void AssessedDomain(std::size_t actionSerial,const ResistanceDomainAssessment& assessment,
                        const std::string& unchangedModelRevision);
    void Arm(std::size_t actionSerial,const PreparedVerificationProbe& prepared);
    void Consume(const ProbeObservation& observation,const std::string& actualModelRevision);
    void ArmComposite(std::size_t actionSerial,const PreparedCompositeMeasurement& prepared);
    void ConsumeComposite(const ValidatedCompositeMeasurement& measurement,const std::string& actualModelRevision);
    void RejectComposite(const std::string& reason);
    void ExternalStop();
    // Adapter-owned wall/model-time deadline; cannot manufacture convergence.
    void ExhaustBudget();
    TrainingStatus Status() const { return status; }
    const std::string& Reason() const { return stopReason; }
    const std::optional<TrainingCertificate>& Certificate() const;
    const std::vector<CandidateEvidence>& Evidence() const { return evidence; }
    const std::vector<TrainingRejection>& Rejected() const { return rejected; }
    const std::vector<CandidateRecoveryEvidence>& Recoveries() const { return recoveries; }
    const std::vector<CandidateRepeatEvidence>& CandidateRepeats() const { return candidateRepeats; }
    const std::vector<ResistanceDomainAssessment>& Domains() const { return domains; }
    const std::vector<InputParameters>& Parameters() const { return current; }
    const std::optional<TrainingCapacitance>& Capacitance() const { return currentCapacitance; }
    const std::string& ModelRevision() const { return revision; }
    std::size_t ModelEpoch() const { return epoch; }
    std::size_t IssuedProbes() const { return probes; }
    std::size_t AppliedEdits() const { return edits; }
    // Attempt-wide charge, including every final verification sweep. This is
    // not Rejected().size(): that array also contains non-observation failures.
    std::size_t InvalidObservations() const { return invalid; }
    std::size_t AcceptedCompositeObservations() const {return acceptedComposite;}
private:
    enum class Phase { Candidate, Anchor, Input, Verify };
    CapturedReferences references;
    BoundedTrainingConfig config;
    TrainingStatus status=TrainingStatus::Searching;
    std::string stopReason;
    Phase phase=Phase::Candidate;
    std::vector<InputParameters> current,background;
    std::optional<TrainingCapacitance> currentCapacitance,backgroundCapacitance;
    std::vector<std::size_t> order;
    std::size_t position=0,sweep=0,candidates=0;
    std::size_t serial=0,epoch=0,probes=0,edits=0,invalid=0,acceptedComposite=0;
    std::string revision,anchorTrial;
    std::string compositeAnchorWitness;
    std::optional<ValidatedProbeMeasurement> anchor;
    std::optional<TrainingAction> pending;
    std::optional<PreparedVerificationProbe> armed;
    std::optional<PreparedCompositeMeasurement> compositeArmed;
    std::optional<CandidateEvidence> best;
    bool restoringBest=false;
    // Enumeration state belongs to a single physical search context. Keeping
    // it separate from measurements/references enables fair bounded traversal
    // of joint physical candidates without resetting the training session.
    struct CandidateCursor {
        std::vector<int> lengths,counts;
        std::vector<double> resistances;
        std::vector<std::vector<InputParameters>> n0Priority,issuedN0Candidates;
        std::size_t lengthIndex=0,normalizerIndex=0;
        bool initialCandidatePending=false,domainReady=false;
        // Explicit AdvanceUnassignedBurst uses one domain candidate per
        // geometry turn. RetrySameCandidate retains its original order.
        // These belong to each suspended C context, not the whole session.
        bool domainYieldPending=false,domainPassIssuedCandidate=false;
        std::size_t domainLength=0,domainNormalizer=0,domainVariant=0;
        std::vector<InputParameters> domainGeometry;
        std::string domainScope,candidateResistanceScope;
        std::vector<std::pair<std::string,CandidateEvidence>> resistanceMeasurements;
        std::vector<std::pair<std::string,std::vector<InputParameters>>> visitedDomainCandidates;
        std::vector<CandidateEvidence> lengthMeasurements;
    };
    CandidateCursor cursor;
    struct CapacitanceContext {
        CandidateCursor suspended;
        std::optional<double> absoluteScale={};
        bool exhausted=false;
    };
    // The active cursor lives in cursor; only inactive cursors are suspended.
    std::vector<CapacitanceContext> capacitanceContexts;
    std::size_t activeCapacitanceContext=0,nextCapacitanceContext=0;
    std::vector<std::pair<std::vector<InputParameters>,std::optional<TrainingCapacitance>>> issuedJointCandidates;
    void SelectCapacitanceContext(std::size_t index);
    void AdvanceCapacitanceContext();
    std::optional<TrainingCapacitance> CandidateCapacitance(const std::vector<InputParameters>& parameters) const;
    bool requireResistanceDomains=false;
    std::vector<ResistanceDomainAssessment> domains;
    std::vector<CandidateEvidence> evidence;
    std::vector<CandidateEvidence> candidateMeasurements;
    std::vector<CandidateRepeatEvidence> candidateRepeats;
    std::vector<TrainingRejection> rejected;
    std::vector<CandidateRecoveryEvidence> recoveries;
    std::set<std::string> seenTrials;
    std::set<std::string> seenCompositeGroups,seenCompositeProofs;
    std::unique_ptr<FinalVerificationSession> verification;
    std::size_t verificationInvalidAccounted=0;
    void BeginInput();
    void AdvanceInput();
    void BeginVerification();
    void FinishVerification();
    bool Legal(const std::vector<InputParameters>& parameters) const;
    bool PrepareCandidate(std::vector<InputParameters>& parameters) const;
    bool NextCandidate(std::vector<InputParameters>& parameters);
    enum class DynamicCandidate { Candidate, NeedDomain, Exhausted };
    DynamicCandidate NextDomainCandidate(std::vector<InputParameters>& parameters);
    void AdvanceDomainGeometry();
    void PrioritizeMeasuredLength(const CandidateEvidence& measurement);
    // Shared private acceptance path after legacy or typed admission.
    void AcceptValidatedScalar(const ValidatedProbeMeasurement& measurement);
    void Reject(const std::string& reason);
    void RecordIncompleteCandidateRepeats(const std::string& reason);
    void Stop(TrainingStatus terminal,const std::string& reason={});
};
const char* TrainingStatusName(TrainingStatus status);
}
