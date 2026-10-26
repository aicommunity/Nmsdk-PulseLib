#pragma once
#include "FinalVerification.h"
#include <memory>

namespace NMSDK::TimeLearner {
enum class Normalization { SynapseCount, TipResistance };
enum class TrainingStatus { Searching, Verifying, Exact, SearchUnresolved, BudgetExhausted,
    InvalidModel, InvalidObservation, ExternalStop };
struct InputParameters {
    // Effective physical bank values. Coincident Branch attachments share a
    // bank: their Ns/Rs must agree. Legacy requested counts are not readback.
    int length=1,synapses=1;
    double resistance=0.;
    bool operator==(const InputParameters&) const=default;
};
struct InputSearchBounds {
    int minimumLength=1,maximumLength=1,minimumSynapses=1,maximumSynapses=128;
    double minimumResistance=0.,maximumResistance=0.;
};
struct BoundedTrainingConfig {
    std::string runId,initialModelRevision;
    std::size_t initialModelEpoch=0,maximumProbes=0,maximumEdits=0;
    std::size_t maximumCandidatesPerInput=0,maximumSweeps=1,maximumInvalidObservations=0;
    // A finite, explicitly declared search mesh; its exhaustion never proves
    // physical infeasibility or a global monotonicity assumption.
    unsigned resistanceSubdivisionDepth=4;
    Normalization normalization=Normalization::SynapseCount;
    VerificationTolerances tolerances;
    std::vector<InputSearchBounds> bounds;
};
enum class TrainingActionKind { Edit, Probe };
struct TrainingAction {
    TrainingActionKind kind=TrainingActionKind::Probe;
    std::size_t serial=0,inputIndex=0,modelEpoch=0;
    std::string runId,trialId,baseModelRevision;
    std::vector<InputParameters> parameters;
    bool finalVerification=false;
};
struct CandidateEvidence {
    std::size_t inputIndex=0,modelEpoch=0;
    InputParameters parameters;
    std::string modelRevision,trialId,anchorTrialId;
    double timingResidualLow=0.,timingResidualHigh=0.,amplitudeResidual=0.;
    bool withinTiming=false,withinAmplitude=false;
};
struct TrainingRejection { std::string trialId,reason; };

// Common decision controller. Adapters alone apply edits/read back the graph,
// prepare native probes and schedule children. No Rdk calls or physical reset
// live here. A local candidate is never an Exact result: the unchanged final
// graph must pass the independent all-input FinalVerificationSession.
class BoundedTrainingSession {
public:
    BoundedTrainingSession(CapturedReferences references,BoundedTrainingConfig config,
                           std::vector<InputParameters> initial);
    std::optional<TrainingAction> NextAction();
    void Applied(std::size_t actionSerial,const std::vector<InputParameters>& effective,
                 const std::string& actualModelRevision);
    void DeclineEdit(std::size_t actionSerial,const std::string& reason,const std::string& unchangedModelRevision);
    void Arm(std::size_t actionSerial,const PreparedVerificationProbe& prepared);
    void Consume(const ProbeObservation& observation,const std::string& actualModelRevision);
    void ExternalStop();
    // Adapter-owned wall/model-time deadline; cannot manufacture convergence.
    void ExhaustBudget();
    TrainingStatus Status() const { return status; }
    const std::optional<TrainingCertificate>& Certificate() const;
    const std::vector<CandidateEvidence>& Evidence() const { return evidence; }
    const std::vector<TrainingRejection>& Rejected() const { return rejected; }
    const std::vector<InputParameters>& Parameters() const { return current; }
    const std::string& ModelRevision() const { return revision; }
    std::size_t ModelEpoch() const { return epoch; }
    std::size_t IssuedProbes() const { return probes; }
    std::size_t AppliedEdits() const { return edits; }
private:
    enum class Phase { Candidate, Anchor, Input, Verify };
    CapturedReferences references;
    BoundedTrainingConfig config;
    TrainingStatus status=TrainingStatus::Searching;
    Phase phase=Phase::Candidate;
    std::vector<InputParameters> current,background;
    std::vector<std::size_t> order;
    std::vector<int> lengths,counts;
    std::vector<double> resistances;
    std::size_t lengthIndex=0,normalizerIndex=0,position=0,sweep=0,candidates=0;
    std::size_t serial=0,epoch=0,probes=0,edits=0,invalid=0;
    std::string revision,anchorTrial;
    std::optional<ValidatedProbeMeasurement> anchor;
    std::optional<TrainingAction> pending;
    std::optional<PreparedVerificationProbe> armed;
    std::optional<CandidateEvidence> best;
    bool restoringBest=false;
    std::vector<CandidateEvidence> evidence;
    std::vector<TrainingRejection> rejected;
    std::set<std::string> seenTrials;
    std::unique_ptr<FinalVerificationSession> verification;
    void BeginInput();
    void AdvanceInput();
    void BeginVerification();
    void FinishVerification();
    bool Legal(const std::vector<InputParameters>& parameters) const;
    bool NextCandidate(std::vector<InputParameters>& parameters);
    void Reject(const std::string& reason);
    void Stop(TrainingStatus terminal);
};
const char* TrainingStatusName(TrainingStatus status);
}
