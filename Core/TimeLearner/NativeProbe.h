#pragma once
#include "FinalVerification.h"
#include "../NDatasetMatrix.h"
#include "../NPulseMembrane.h"
#include "../NPulseNeuron.h"
#include "../NPulseSynapseCommon.h"
#include <functional>
#include <memory>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
class NativeLifecycleScope {
public:
    explicit NativeLifecycleScope(unsigned& depth):depth(depth) { ++depth; }
    ~NativeLifecycleScope() { --depth; }
    NativeLifecycleScope(const NativeLifecycleScope&)=delete;
    NativeLifecycleScope& operator=(const NativeLifecycleScope&)=delete;
private:
    unsigned& depth;
};
enum class NativeProbeStatus { Idle, Queued, Running, Completed, Rejected, Cancelled };
struct NativeProbeSample {
    double time=0.,stimulus=0.;
    std::vector<double> potentials;
    std::vector<StimulusObservation> newStimuli;
    std::string effectiveModelRevision;
};
struct NativeProbeRequest {
    // Relative window: begin must be zero, end is duration. The adapter supplies
    // absolute time, effective revision and the native post-reset baseline.
    ProbeSpec specification;
    ProbeRouting routing=ProbeRouting::ClassicBurst;
    int activeInput=-1;
    std::string inputId,protocolRevision,baseModelRevision;
    std::size_t modelEpoch=0;
    std::vector<double> expectedInputIsi;
    // Must read the actual physical graph including bindings/frequency. Called
    // before/after preparation and on every observed tick; no cached revision
    // assumption is made by this adapter. Dataset pattern is checked separately.
    std::function<std::string()> readModelRevision;
    std::function<void(const PreparedVerificationProbe&,const ProbeSpec&)> onPrepared;
    // Read-only artifact sink. Runs after native sampling, before closing the
    // window; failure rejects the observation rather than silently losing data.
    std::function<void(const NativeProbeSample&)> onSample;
};
struct NativeProbeBindings {
    RDK::UNet* owner=nullptr;
    RDK::UEPtr<NPulseNeuron> neuron;
    RDK::UEPtr<NDatasetMatrix> dataset;
    std::vector<RDK::UEPtr<NPulseMembrane>> somas;
    std::vector<NPulseSynapseCommon*> soloSynapses;
    int inputCount=0;
    bool branch=false,legacyPhysicsPending=false,unsafeIntegration=false,postTuneActive=false,routingComplete=true;
};
struct NativeProbeResult {
    NativeProbeStatus status=NativeProbeStatus::Rejected;
    std::optional<PreparedVerificationProbe> prepared;
    std::optional<ProbeObservation> observation;
    std::string currentBaseRevision;
    std::vector<std::string> reasons;
    bool requiresFreshVerification=false;
};

// Called by the learner's native hooks, never calls Calculate. Taking control
// suppresses legacy training/inference edits until an explicit learner Reset or
// Default. Completed/error state remains held between probes. No mid-probe
// checkpoint, implicit legacy resume, or training-convergence claim is provided.
class NativeProbeExecutor {
public:
    void Queue(NativeProbeRequest request);
    bool OwnsControl() const { return state!=NativeProbeStatus::Idle; }
    NativeProbeStatus Status() const { return state; }
    int ActiveInput() const { return request.activeInput; }
    void BeforeChildren(const NativeProbeBindings& bindings,double time);
    void AfterChildren(const NativeProbeBindings& bindings,double time);
    void Cancel(const std::string& reason);
    void ResetControl();
    const std::optional<NativeProbeResult>& Result() const { return result; }
    std::optional<NativeProbeResult> TakeResult();
private:
    struct SavedInput {
        RDK::UEPtr<NPulseSynapseCommon> synapse;
        RDK::UIPropertyOutput* source;
        std::string path;
        RDK::UModuleLifecycleObservation lifetime;
    };
    struct WatchedNode {
        RDK::UEPtr<RDK::UContainer> node;
        std::string path;
        RDK::UModuleLifecycleObservation lifecycle;
        bool stable;
    };
    NativeProbeStatus state=NativeProbeStatus::Idle;
    NativeProbeRequest request;
    std::optional<NativeProbeBindings> physical;
    std::vector<SavedInput> savedInputs;
    std::vector<WatchedNode> watched;
    RDK::UModuleLifecycleObservation ownerLife,neuronLife,datasetLife,generatorLife;
    NPulseGeneratorTransit* capturedGenerator=nullptr;
    std::map<std::string,std::string> generatorParameters;
    std::unique_ptr<ProbeRecorder> recorder;
    std::optional<NativeProbeResult> result;
    std::optional<PreparedVerificationProbe> prepared;
    std::set<std::pair<std::string,std::string>> seenTrials;
    double oldDelay=0.,end=0.,lastTime=0.;
    bool oldAdvance=false,sourceSaved=false;
    std::optional<double> lastEmission;
    std::size_t emitted=0;
    double frequency=0.;
    void CheckPattern(const NativeProbeBindings& bindings) const;
    void CheckLiveGraph(const NativeProbeBindings& bindings) const;
    void CheckSource(const NativeProbeBindings& bindings) const;
    bool SameLiveRoots() const;
    bool CanReadModel() const;
    void StopSource();
    void Restore();
    void Finish(double time,const std::string& reason,NativeProbeStatus status);
    void Failure(double time,const std::string& reason);
};
const char* NativeProbeStatusName(NativeProbeStatus status);
}
