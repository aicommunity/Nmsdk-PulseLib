#pragma once
#include "CompositeTrainingMeasurement.h"
#include "NativeDeclaredWindowBasinAssignment.h"
#include "ModelIdentity.h"
#include "TrainingCapacitance.h"
#include "NativeCompositeTrainingRecord.h"
#include <set>
#include <memory>

namespace NMSDK { class NNeuronTimeLearner;class NNeuronTimeLearnerBranch; }
namespace NMSDK::TimeLearner {
// Live native authority for protocol 4, separate from diagnostic data/codec.
// The owner-specific factory binds actual roots, executor, images and optional
// existing C0. No Calculate/Build/Reset, model edits or invented C0 occur here.
// Service composition supplies the existing scheduler's before/after hooks.
class NativeCompositeTrainingAttempt : public std::enable_shared_from_this<NativeCompositeTrainingAttempt> {
public:
    NativeCompositeTrainingAttempt(NNeuronTimeLearner& owner,VerificationProtocol protocol,
        CompositeAttemptLimits limits,double modelTime,double wallTime);
    NativeCompositeTrainingAttempt(NNeuronTimeLearnerBranch& owner,VerificationProtocol protocol,
        CompositeAttemptLimits limits,double modelTime,double wallTime);
    ~NativeCompositeTrainingAttempt();
    NativeCompositeTrainingAttempt(const NativeCompositeTrainingAttempt&)=delete;
    NativeCompositeTrainingAttempt& operator=(const NativeCompositeTrainingAttempt&)=delete;
    void StartMeasurement(const VerificationRequest& logical,CompositeMeasurementPurpose purpose,
        NativeProbeRequest prototype,double maximumGroupModelSeconds,double maximumGroupWallSeconds,
        double modelTime,double wallTime,
        std::function<void(const PreparedCompositeMeasurement&)> onPrepared={},
        std::function<void(const NativeProbeRequest&)> onNativeQueue={},
        std::function<void(const NativeProbeResult&)> onNativeResult={});
    void BeforeTick(double modelTime,double wallTime);
    void AfterTick(double modelTime,double wallTime);
    void Cancel(const std::string& reason);
    bool Running() const;
    const std::optional<ValidatedCompositeMeasurement>& Measurement() const {return measurement;}
    const std::optional<NativeDeclaredWindowBasinResult>& Selection() const {return selection;}
    const std::string& RejectionReason() const {return rejection;}
    const NativeMatchedOmissionResult* Group() const {return group?&group->Result():nullptr;}
    const std::string& SourceOrigin() const {return sourceOrigin;}
    const std::string& OwnerFamilyBinding() const {return ownerScope;}
    const QJsonObject& SourceBinding() const {return sourceBinding;}
    std::size_t IssuedGroups() const {return issuedGroups;}
    std::size_t IssuedNativeProbes() const;
    std::size_t RetainedSamples() const {return totalSamples+(group && !closed?activeSamples:0);}
    bool TimeBudgetExhausted() const {return budgetExhausted;}
    const std::string& InitialModelRevision() const {return initialRevision;}
    std::size_t InitialModelEpoch() const {return initialEpoch;}
    NativeCompositeMeasurementRecord Record() const;
private:
    friend class NativeTrainingService;
    void RestartOwned(CompositeAttemptLimits,double modelTime,double wallTime,const std::string& currentRevision);
    void RefreshOwnedAfterEdit(const std::string& currentRevision);
    void RevokeLive();
    void InterruptOwnedRaw(const std::string& reason,bool lifecycleAuthority=false);
    void SuspendOwned(const std::string& reason);
    void RetainRawPacket(const NativeProbeResult&);
    std::vector<NativeProbeResult> TakeRetainedRawPackets();
    template<class Owner> void Bind(Owner& owner);
    void Initialize(VerificationProtocol,CompositeAttemptLimits,double,double);
    void CheckClock(double,double);
    std::string CheckBoundary(QJsonObject* retained=nullptr,std::optional<TrainingCapacitance>* parameters=nullptr);
    void CloseGroup();
    std::function<NativeProbeBindings()> bindings;
    std::function<QJsonObject()> declaration;
    std::function<double()> actualModelTime;
    std::function<RDK::UModuleLifecycleObservation()> observeOwner;
    std::function<void()> requireOwnerControl;
    std::function<std::optional<TrainingCapacitanceWitness>()> capacitanceWitness;
    NativeMatchedOmissionAdapter adapter;
    NmsdkTools::ModelRevisionObserver revisionObserver;
    std::shared_ptr<const VerificationProtocol> protocol;
    std::shared_ptr<CompositeTrainingDetail::LiveLease> lease;
    RDK::UModuleLifecycleObservation ownerLife;
    CompositeAttemptLimits limits;
    std::unique_ptr<NativeMatchedOmissionGroup> group;
    std::shared_ptr<CompositeTrainingDetail::Identity> activeIdentity;
    std::optional<PreparedCompositeMeasurement> prepared;
    std::optional<ValidatedCompositeMeasurement> measurement;
    std::optional<NativeDeclaredWindowBasinResult> selection;
    VerificationRequest logical;
    CompositeMeasurementPurpose purpose=CompositeMeasurementPurpose::Reference;
    std::vector<NativeCompositeRawTrial> rawTrials;
    std::vector<NativeProbeResult> interruptedRawPackets;
    std::size_t rawDeliveryCursor=0;
    std::string ownedRawRun,ownedRawTrial;
    std::size_t ownedRawControlGeneration=0;
    std::function<std::optional<NativeProbeResult>(const std::string&,bool)> cancelOwnedRaw;
    std::function<void(const NativeProbeRequest&)> beforeNativeQueue;
    std::function<void(const NativeProbeResult&)> afterNativeResult;
    std::set<std::string> logicalTrials;
    std::vector<std::string> imageRoots;
    QJsonObject initialDeclaration,sourceBinding;
    std::string ownerScope,liveAttemptBinding,sourceOrigin,initialRevision,storageFingerprint,initialCapacitanceOrigin,rejection;
    double initialCapacitanceMaximumRelativeChange=0.;
    std::size_t generation=0,initialEpoch=0,issuedGroups=0,completedNativeProbes=0,totalSamples=0,activeSamples=0;
    double startModel=0.,startWall=0.,lastModel=0.,lastWall=0.;
    double groupStartModel=0.,groupStartWall=0.,groupWallLimit=0.,groupModelLimit=0.,groupEndModel=0.,groupEndWall=0.;
    bool closed=false,cancelled=false,budgetExhausted=false;
};
}
