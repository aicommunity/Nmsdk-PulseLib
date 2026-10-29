#pragma once
#include "NativeProbe.h"
#include "../NPulseLTZoneCommon.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
enum class RecognitionResetPolicy { PerTrial, PreserveNeuronDynamics };
struct NativeRecognitionTrialRequest {
    std::string runId,trialId,sampleId,label,patternHash;
    MDMatrix<double> pattern;
    double duration=0.;
    RecognitionResetPolicy resetPolicy=RecognitionResetPolicy::PerTrial;
    std::size_t maximumSamples=1000000;
};
struct NativeRecognitionTrialSample {
    double time=0.,stimulus=0.,ltOutput=0.;
    std::vector<double> somaPotentials,stimulusHistory,spikeHistory;
};
struct NativeRecognitionTrialObservation {
    NativeRecognitionTrialRequest request;
    std::string modelBefore,modelAfter,reason;
    double begin=0.,end=0.,closedAt=0.,frequencyHz=0.;
    bool armed=false,complete=false,valid=false;
    std::vector<double> stimulusTimes,spikeTimes;
    std::vector<NativeRecognitionTrialSample> samples;
};
// This is a full-pattern recognition trial, not a peak-search/training probe.
// No Calculate calls; the owner invokes these around its native child scheduler.
class NativeRecognitionTrialExecutor {
public:
    void Queue(NativeRecognitionTrialRequest request);
    bool Busy() const { return queued || running; }
    bool HasResult() const { return bool(result); }
    void BeforeChildren(const NativeProbeBindings& bindings,double time,
        const std::function<void(const MDMatrix<double>&)>& applyPresentation,
        const std::function<std::string(bool)>& readRevision);
    void AfterChildren(const NativeProbeBindings& bindings,double time);
    void Cancel(const std::string& reason);
    std::optional<NativeRecognitionTrialObservation> TakeResult();
private:
    struct Watched {
        RDK::UEPtr<RDK::UContainer> node;std::string path;
        RDK::UModuleLifecycleObservation lifecycle;
    };
    std::optional<NativeRecognitionTrialObservation> current,result;
    std::optional<NativeProbeBindings> physical;
    RDK::UEPtr<NPulseGeneratorTransit> source;
    RDK::UEPtr<NPulseLTZoneCommon> zone;
    RDK::UModuleLifecycleObservation ownerLife;
    std::vector<Watched> watched;
    std::set<std::pair<std::string,std::string>> seen;
    std::function<std::string(bool)> revision;
    std::optional<double> lastStimulus,lastSpike,lastSample;
    std::map<std::string,std::string> sourceParameters;
    bool queued=false,running=false,saved=false;
    double oldDelay=0.;bool oldAdvance=false;
    void Check(const NativeProbeBindings& bindings) const;
    void Stop();
    void Finish(const std::string& reason,bool complete,bool valid=false);
};
RDK_LIB_TYPE QJsonObject NativeRecognitionTrialJson(const NativeRecognitionTrialObservation& observation);
}
