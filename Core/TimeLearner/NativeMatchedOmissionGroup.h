#pragma once
#include "MatchedOmissionAssignment.h"
#include "NativeResponseDomainCapture.h"

namespace NMSDK::TimeLearner {
struct NativeMatchedOmissionPlan {
    NativeProbeRequest fullBurst;
    MatchedOmissionPolicy policy;
    std::string groupId;
    std::size_t channel=0;
    std::vector<std::string> physicalChannelPaths;
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
};
struct NativeMatchedOmissionAdapter {
    std::function<void(NativeProbeRequest)> queue;
    std::function<NativeProbeStatus()> probeState;
    std::function<std::optional<NativeProbeResult>()> takeResult;
    std::function<void(const std::string&)> cancel;
    std::function<std::string()> readModelRevision;
    std::function<bool()> requested;
    // Boundary sink after the actual packet and trace have been retained.
    // Failure rejects the group; the packet is not lost or re-consumed.
    std::function<void(const NativeProbeResult&)> onNativeResult;
};
enum class NativeMatchedOmissionState { Idle, Running, Completed, Rejected, Cancelled, BudgetExhausted };
struct NativeMatchedOmissionResult {
    MatchedOmissionSpecification specification;
    MatchedOmissionPolicy policy;
    std::vector<double> sourceIsi,appliedSourceIsi;
    std::vector<std::vector<double>> appliedPresentedIsi;
    // Declarations only: elapsed/enforcement attestation is not retained here.
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
    MatchedOmissionWitness witness;
    std::vector<NativeProbeResult> packets;
    std::optional<MatchedOmissionResult> assignment;
    std::string reason;
};
// A native, bounded measurement group, not a component scheduler. The caller
// invokes BeforeTick before the existing executor and AfterTick after it. Every
// child is still calculated exactly once by Rdk. No edits occur between probes.
// Completed means the entire group closed, not convergence or classification.
class NativeMatchedOmissionGroup {
public:
    void Start(NativeMatchedOmissionPlan plan,const NativeProbeBindings& bindings,
               double modelTime,double wallTime);
    void BeforeTick(const NativeMatchedOmissionAdapter& adapter,double modelTime,double wallTime);
    void AfterTick(const NativeMatchedOmissionAdapter& adapter,double modelTime,double wallTime);
    void Cancel(const NativeMatchedOmissionAdapter& adapter,const std::string& reason);
    NativeMatchedOmissionState State() const {return state;}
    const NativeMatchedOmissionResult& Result() const {return result;}
    std::size_t IssuedNativeProbes() const {return issued;}
private:
    struct Watched {
        RDK::UEPtr<RDK::UContainer> node;
        std::string path;
        RDK::UModuleLifecycleObservation lifetime;
    };
    NativeMatchedOmissionState state=NativeMatchedOmissionState::Idle;
    NativeMatchedOmissionPlan plan;
    NativeMatchedOmissionResult result;
    std::optional<NativeProbeBindings> bindings;
    RDK::UModuleLifecycleObservation ownerLifetime;
    std::vector<Watched> watched;
    std::unique_ptr<NativeResponseDomainCapture> capture;
    std::vector<std::size_t> activeMask;
    std::size_t issued=0,totalSamples=0;
    int initialClass=0;
    bool waiting=false;
    double startModel=0.,startWall=0.,lastModel=0.,lastWall=0.;
    void CheckInstances() const;
    void CheckRestoredPresentation() const;
    bool CheckClock(const NativeMatchedOmissionAdapter& adapter,double modelTime,double wallTime);
    void Fail(const NativeMatchedOmissionAdapter& adapter,NativeMatchedOmissionState status,const std::string& reason);
};
}
