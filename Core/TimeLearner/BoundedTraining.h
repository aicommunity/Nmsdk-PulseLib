#pragma once
#include "FinalVerification.h"
#include "TrainingParameters.h"
#include "ResistanceDomain.h"
#include "TrainingCapacitance.h"
#include <memory>

namespace NMSDK::TimeLearner {
enum class TrainingStatus { Searching, Verifying, Exact, SearchUnresolved, BudgetExhausted,
    InvalidModel, InvalidObservation, ExternalStop };
struct InputSearchBounds {
    int minimumLength=1,maximumLength=1,minimumSynapses=1,maximumSynapses=128;
    double minimumResistance=0.,maximumResistance=0.;
};
struct BoundedTrainingConfig {
    std::string runId,initialModelRevision;
    std::size_t initialModelEpoch=0,maximumProbes=0,maximumEdits=0;
    std::size_t maximumCandidatesPerInput=0,maximumSweeps=1,maximumInvalidObservations=0;
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
};
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
    void ExternalStop();
    // Adapter-owned wall/model-time deadline; cannot manufacture convergence.
    void ExhaustBudget();
    TrainingStatus Status() const { return status; }
    const std::string& Reason() const { return stopReason; }
    const std::optional<TrainingCertificate>& Certificate() const;
    const std::vector<CandidateEvidence>& Evidence() const { return evidence; }
    const std::vector<TrainingRejection>& Rejected() const { return rejected; }
    const std::vector<CandidateRepeatEvidence>& CandidateRepeats() const { return candidateRepeats; }
    const std::vector<ResistanceDomainAssessment>& Domains() const { return domains; }
    const std::vector<InputParameters>& Parameters() const { return current; }
    const std::optional<TrainingCapacitance>& Capacitance() const { return currentCapacitance; }
    const std::string& ModelRevision() const { return revision; }
    std::size_t ModelEpoch() const { return epoch; }
    std::size_t IssuedProbes() const { return probes; }
    std::size_t AppliedEdits() const { return edits; }
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
    std::size_t serial=0,epoch=0,probes=0,edits=0,invalid=0;
    std::string revision,anchorTrial;
    std::optional<ValidatedProbeMeasurement> anchor;
    std::optional<TrainingAction> pending;
    std::optional<PreparedVerificationProbe> armed;
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
        std::size_t domainLength=0,domainNormalizer=0,domainVariant=0;
        std::vector<InputParameters> domainGeometry;
        std::string domainScope,candidateResistanceScope;
        std::vector<std::pair<std::string,CandidateEvidence>> resistanceMeasurements;
        std::vector<std::pair<std::string,std::vector<InputParameters>>> visitedDomainCandidates;
        std::vector<CandidateEvidence> lengthMeasurements;
    };
    CandidateCursor cursor;
    bool requireResistanceDomains=false;
    std::vector<ResistanceDomainAssessment> domains;
    std::vector<CandidateEvidence> evidence;
    std::vector<CandidateEvidence> candidateMeasurements;
    std::vector<CandidateRepeatEvidence> candidateRepeats;
    std::vector<TrainingRejection> rejected;
    std::set<std::string> seenTrials;
    std::unique_ptr<FinalVerificationSession> verification;
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
    void Reject(const std::string& reason);
    void RecordIncompleteCandidateRepeats(const std::string& reason);
    void Stop(TrainingStatus terminal,const std::string& reason={});
};
const char* TrainingStatusName(TrainingStatus status);
}
