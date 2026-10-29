#pragma once
#include "NativeModelEditor.h"
#include <functional>
#include <utility>

namespace NMSDK::TimeLearner {
enum class NativeTrainingState { Idle, CapturingReferences, Searching, Finished };

struct NativeTrainingPlan {
    // A declared native burst protocol, including explicit relative windows.
    // Model/protocol validators run on actual observations, never predictions.
    VerificationProtocol protocol;
    NativeProbeRequest probe;
    BoundedTrainingConfig search;
    std::vector<PeakWindow> inputWindows;
    std::size_t maximumReferenceProbes=0,maximumInvalidReferenceProbes=0;
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
    std::function<void(const NativeProbeResult&)> observationSink;
};

// Component-specific adapters alone touch Rdk objects. BeforeTick is called
// before native children; AfterTick after the existing probe executor sampled
// those children. The service never calculates a component or loops model time.
struct NativeTrainingAdapter {
    std::function<std::vector<InputParameters>()> readParameters;
    std::function<std::string()> readModelRevision;
    std::function<NativeEditResult(const std::vector<InputParameters>&)> apply;
    std::function<void(NativeProbeRequest)> queue;
    std::function<NativeProbeStatus()> probeState;
    std::function<std::optional<NativeProbeResult>()> takeResult;
    std::function<void(const std::string&)> cancel;
    std::function<std::string()> readFullModelRevision;
    std::function<ResistanceDomainAssessment(const std::vector<InputParameters>&,std::size_t)> assessResistanceDomain;
    // Native property connections can change without invoking the owner setter.
    // Pure controller adapters may omit this provider (always requested).
    std::function<bool()> trainingRequested;
};

class NativeTrainingService {
public:
    void Start(NativeTrainingPlan plan,const NativeTrainingAdapter& adapter,double modelTime,double wallTime);
    void BeforeTick(const NativeTrainingAdapter& adapter,double modelTime,double wallTime);
    void AfterTick(const NativeTrainingAdapter& adapter,double modelTime,double wallTime);
    void ExternalStop(const NativeTrainingAdapter& adapter);
    void FailConfiguration(const std::string& diagnostic);
    // Reset controller dynamics, retaining the learned goal and the last
    // verified model binding. Retarget/cold-start is a separate explicit call.
    void ResetControl();
    void InvalidateTarget();
    NativeTrainingState State() const { return state; }
    TrainingStatus Outcome() const { return outcome; }
    const std::string& Reason() const { return reason; }
    const std::optional<CapturedReferences>& References() const { return references; }
    const std::optional<TrainingCertificate>& Certificate() const { return certificate; }
    const std::vector<ReferenceEvidence>& ReferenceObservations() const { return referenceEvidence; }
    const std::vector<NativeProbeResult>& ReferencePackets() const { return referencePackets; }
    const std::vector<NativeProbeResult>& FinalPackets() const { return finalPackets; }
    const std::vector<NativeProbeResult>& ObservedPackets() const { return observedPackets; }
    const NativeTrainingPlan& Plan() const { return plan; }
    std::size_t SessionAttempt() const { return attempt; }
    const std::string& CaptureRevision() const {
        return references?references->InitialModelRevision():(incompleteCaptureConstraint?incompleteCaptureConstraint->first:plan.search.initialModelRevision);
    }
    std::size_t CaptureEpoch() const {
        return references?references->InitialModelEpoch():(incompleteCaptureConstraint?incompleteCaptureConstraint->second:plan.search.initialModelEpoch);
    }
    // Completed-stage restore, never a mid-probe checkpoint. Replays the actual
    // reference/final evidence through the production validators before commit.
    void RestoreBoundary(NativeTrainingPlan plan,std::vector<NativeProbeResult> references,
        std::vector<NativeProbeResult> finals,const std::string& actualModelRevision,
        std::size_t modelEpoch,std::size_t sessionAttempt);
    // Halted model/evidence, without a certificate or mid-probe continuation.
    void RestoreStoppedBoundary(NativeTrainingPlan plan,std::vector<NativeProbeResult> references,
        std::vector<NativeProbeResult> finals,std::vector<NativeProbeResult> observations,
        const std::string& actualModelRevision,std::size_t modelEpoch,std::size_t sessionAttempt,
        const std::string& captureRevision,std::size_t captureEpoch,bool referenceComplete,
        TrainingStatus outcome,const std::string& reason);
    const BoundedTrainingSession* Search() const { return search.get(); }
    std::size_t IssuedReferenceProbes() const { return referenceProbes; }
    std::size_t ModelEpoch() const { return search?search->ModelEpoch():retainedEpoch; }
    const BoundedTrainingConfig& Configuration() const { return plan.search; }
private:
    NativeTrainingState state=NativeTrainingState::Idle;
    TrainingStatus outcome=TrainingStatus::Searching;
    std::string reason,referenceBinding;
    NativeTrainingPlan plan;
    std::optional<CapturedReferences> references;
    std::optional<TrainingCertificate> certificate;
    std::vector<ReferenceEvidence> referenceEvidence;
    std::vector<NativeProbeResult> referencePackets,finalPackets,observedPackets;
    std::optional<std::pair<std::string,std::size_t>> incompleteCaptureConstraint;
    std::unique_ptr<BoundedTrainingSession> search;
    std::optional<TrainingAction> activeAction;
    bool waiting=false;
    std::size_t referenceProbes=0,invalidReferences=0,attempt=0,retainedEpoch=0;
    double startModel=0.,startWall=0.,lastModel=0.,lastWall=0.;
    void CheckBudget(const NativeTrainingAdapter& adapter,double modelTime,double wallTime);
    void BeginSearch(const NativeTrainingAdapter& adapter);
    NativeProbeRequest Request(std::size_t input,const std::string& run,const std::string& trial,
                               const std::string& revision,std::size_t epoch);
    void Finish(TrainingStatus result,const std::string& diagnostic);
    void SyncOutcome();
    void RecordCancelled(const NativeTrainingAdapter& adapter);
};
const char* NativeTrainingStateName(NativeTrainingState state);
}
