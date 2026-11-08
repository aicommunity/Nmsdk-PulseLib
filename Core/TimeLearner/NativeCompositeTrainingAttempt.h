#pragma once
#include "CompositeTrainingMeasurement.h"
#include "NativeDeclaredWindowBasinAssignment.h"
#include "ModelIdentity.h"
#include "TrainingCapacitance.h"
#include <set>

namespace NMSDK { class NNeuronTimeLearner;class NNeuronTimeLearnerBranch; }
namespace NMSDK::TimeLearner {
struct CompositeAttemptLimits {
    std::size_t maximumGroups=0,maximumNativeProbes=0,maximumTotalSamples=0;
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
};
// Live native authority for protocol 4, separate from diagnostic data/codec.
// The owner-specific factory binds actual roots, executor, images and optional
// existing C0. No Calculate/Build/Reset, model edits or invented C0 occur here.
// Service composition supplies the existing scheduler's before/after hooks.
class NativeCompositeTrainingAttempt {
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
        std::function<void(const PreparedCompositeMeasurement&)> onPrepared={});
    void BeforeTick(double modelTime,double wallTime);
    void AfterTick(double modelTime,double wallTime);
    void Cancel(const std::string& reason);
    bool Running() const;
    const std::optional<ValidatedCompositeMeasurement>& Measurement() const {return measurement;}
    const std::optional<NativeDeclaredWindowBasinResult>& Selection() const {return selection;}
    const std::string& RejectionReason() const {return rejection;}
    const NativeMatchedOmissionResult* Group() const {return group?&group->Result():nullptr;}
    const std::string& SourceOrigin() const {return sourceOrigin;}
    const QJsonObject& SourceBinding() const {return sourceBinding;}
    std::size_t IssuedGroups() const {return issuedGroups;}
    std::size_t IssuedNativeProbes() const;
    std::size_t RetainedSamples() const {return totalSamples;}
    const std::string& InitialModelRevision() const {return initialRevision;}
    std::size_t InitialModelEpoch() const {return initialEpoch;}
private:
    template<class Owner> void Bind(Owner& owner);
    void Initialize(VerificationProtocol,CompositeAttemptLimits,double,double);
    void CheckClock(double,double);
    std::string CheckBoundary(QJsonObject* retained=nullptr,std::optional<TrainingCapacitance>* parameters=nullptr);
    void CloseGroup();
    std::function<NativeProbeBindings()> bindings;
    std::function<QJsonObject()> declaration;
    std::function<double()> actualModelTime;
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
    std::set<std::string> logicalTrials;
    std::vector<std::string> imageRoots;
    QJsonObject initialDeclaration,sourceBinding;
    std::string ownerScope,sourceOrigin,initialRevision,storageFingerprint,initialCapacitanceOrigin,rejection;
    double initialCapacitanceMaximumRelativeChange=0.;
    std::size_t generation=0,initialEpoch=0,issuedGroups=0,completedNativeProbes=0,totalSamples=0,activeSamples=0;
    double startModel=0.,startWall=0.,lastModel=0.,lastWall=0.;
    double groupStartWall=0.,groupWallLimit=0.;
    bool closed=false,cancelled=false;
};
}
