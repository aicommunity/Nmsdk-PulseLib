#pragma once
#include "NativeModelEditor.h"
#include "NativeCompositeTrainingAttempt.h"
#include <functional>
#include <utility>

namespace NMSDK::TimeLearner {
enum class NativeTrainingState { Idle, CapturingReferences, Searching, Finished };
double NativeTrainingWallTime();

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
    // Explicit evidence capability, separate from a future C-search policy.
    // Requires an already bound C0 owner; never captures a new baseline.
    bool recordCapacitanceHistory=false;
    std::optional<NativeCompositeTrainingPolicy> composite={};
    std::function<void(const NativeCompositeMeasurementRecord&)> compositeSink;
    std::function<void(const NativeCompositeMeasurementRecord&)> beforeCompositeConsume;
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
    // Native adapters validate their scheduler capability before Start/actions.
    // Pure controller adapters may omit it; model revision reads remain usable
    // while closing invalid/cancelled observations and restoring checkpoints.
    std::function<void()> validateExecution;
    // Optional all-or-none boundary capabilities. Called only by the explicit
    // record plan; None is rejected, never upgraded to an invented unit C0.
    std::function<std::optional<TrainingCapacitanceWitness>()> readCapacitanceWitness;
    std::function<NativeEditResult(const std::vector<InputParameters>&,
        const std::optional<TrainingCapacitance>&)> applyWithCapacitance;
    std::function<ResistanceDomainAssessment(const std::vector<InputParameters>&,std::size_t,
        const std::optional<TrainingCapacitance>&)> assessResistanceDomainWithCapacitance;
    std::function<std::shared_ptr<NativeCompositeTrainingAttempt>(VerificationProtocol,CompositeAttemptLimits,double,double)> createComposite;
};

struct NativeTrainingCapacitanceHistory {
    // Evidence IDs, not physical revisions: dormant histories can differ on
    // an identical physical graph. Every trial retains its exact boundary state.
    std::string captureWitnessId,currentWitnessId;
    bool currentVerified=false;
    std::map<std::string,TrainingCapacitanceWitness> witnesses;
    std::map<std::string,std::string> trialWitnessIds;
    std::vector<std::string> domainBaseWitnessIds;
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
    const std::optional<NativeTrainingCapacitanceHistory>& CapacitanceHistory() const {return capacitanceHistory;}
    const std::vector<NativeCompositeMeasurementRecord>& CompositeMeasurements() const {return compositeRecords;}
    const std::vector<std::string>& CompositeReferenceGroupIds() const {return compositeReferenceGroups;}
    std::vector<std::string> CompositeFinalGroupIds() const {
        std::vector<std::string> result;for(const auto& id:compositeFinalGroups)if(!id.empty())result.push_back(id);return result;
    }
    const QJsonObject& CompositeSourceBinding() const {return compositeSourceBinding;}
    const QJsonObject& CompositeInitialConfiguration() const {return compositeInitialConfiguration;}
    const std::string& CompositeOwnerFamilyBinding() const {return compositeOwnerFamily;}
    std::size_t CompositeNativeProbes() const {return composite?composite->IssuedNativeProbes():compositeNativeProbes;}
    std::size_t CompositeGroups() const {return composite?composite->IssuedGroups():compositeGroups;}
    std::size_t CompositeSampledFrames() const {return composite?composite->RetainedSamples():compositeSampledFrames;}
    std::size_t CompositePrimaryBytes() const {return compositePrimaryBytes;}
    // Called from the actual owner's lifecycle BEFORE clearing its executor.
    // This adapter is supplied by that owner hook, never decoded from a package.
    void InterruptComposite(const NativeTrainingAdapter&,const std::string& reason);
    // Install only the portable completed/stopped boundary evidence after
    // normal proof replay and physical import. Never restores a search object.
    void RestoreCapacitanceBoundary(NativeTrainingCapacitanceHistory history,
        const std::string& actualModelRevision,const QJsonObject& actualState);
private:
    friend class NativeCompositeTrainingAttempt;
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
    std::optional<NativeTrainingCapacitanceHistory> capacitanceHistory;
    std::shared_ptr<NativeCompositeTrainingAttempt> composite;
    QJsonObject compositeSourceBinding,compositeInitialConfiguration;
    std::vector<NativeCompositeMeasurementRecord> compositeRecords;
    std::vector<ValidatedCompositeMeasurement> compositeReferenceMeasurements;
    std::vector<std::string> compositeReferenceGroups,compositeFinalGroups;
    std::optional<VerificationRequest> compositeRequest;
    std::optional<TrainingCapacitance> compositeCapacitance;
    std::string compositeOwnerFamily;
    std::size_t compositeNativeProbes=0,compositeGroups=0,compositeSampledFrames=0,compositePrimaryBytes=0;
    std::size_t compositeGeneration=0,compositeDispatchDepth=0;
    bool compositeHarvested=true;
    std::map<std::string,TrainingCapacitanceWitness> compositeGroupWitnesses;
    std::map<std::string,std::string> compositeGroupTrialWitnessIds;
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
    std::string RememberCapacitance(const NativeTrainingAdapter& adapter,const std::string& revision,
        const std::optional<TrainingCapacitance>& requested);
    void BindTrialCapacitance(const NativeTrainingAdapter& adapter,const NativeProbeRequest& request);
    void ValidateTrialCapacitance(const NativeTrainingAdapter& adapter,const NativeProbeResult& result);
    void StartComposite(NativeTrainingPlan,const NativeTrainingAdapter&,double,double);
    void BeforeComposite(const NativeTrainingAdapter&,double,double);
    void AfterComposite(const NativeTrainingAdapter&,double,double);
    void StopComposite(const NativeTrainingAdapter&,TrainingStatus,const std::string&);
    void QueueComposite(const NativeTrainingAdapter&,const VerificationRequest&,CompositeMeasurementPurpose,double,double);
    void ArchiveComposite(const std::shared_ptr<NativeCompositeTrainingAttempt>&);
    void ClosedComposite(const NativeTrainingAdapter&,const std::shared_ptr<NativeCompositeTrainingAttempt>&);
    void RawCompositeResult(const NativeTrainingAdapter&,const NativeProbeResult&);
    void CompositeBeginSearch(const NativeTrainingAdapter&);
    void CompositeRefreshCapacitance(const NativeTrainingAdapter&,const std::optional<TrainingCapacitance>& requested,bool requireMatch);
};
const char* NativeTrainingStateName(NativeTrainingState state);
void ValidateNativeTrainingCapacitanceBoundary(const NativeTrainingCapacitanceHistory& history,
    const NativeTrainingService& service,const std::string& actualModelRevision,const QJsonObject& actualState);
}
