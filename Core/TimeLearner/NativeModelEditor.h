#pragma once
#include "NativeProbe.h"
#include "BoundedTraining.h"

namespace NMSDK::TimeLearner {
struct NativeInhibitoryTarget { std::size_t inputIndex=0;int offset=1;double resistance=0.; };
struct NativeEditSpec {
    Normalization normalization=Normalization::SynapseCount;
    int maximumCableLength=1;
    bool preserveAllocatedCable=false;
    double maximumResistanceRatio=1000.,configuredMinimumResistance=0.;
    std::vector<NativeInhibitoryTarget> inhibition;
};
enum class NativeEditStatus { Applied, Rejected, Failed };
struct NativeEditResult {
    NativeEditStatus status=NativeEditStatus::Rejected;
    std::string reason;
    bool modelMayHaveChanged=false;
    std::vector<InputParameters> effective;
    std::vector<double> minimumResistance,maximumResistance;
};
struct IntegrationChannelBound {
    std::string path;
    double maximumConductance=0.,maximumStepGain=0.;
};
struct NativeIntegrationAssessment {
    bool valid=false;
    std::string reason;
    std::vector<IntegrationChannelBound> channels;
};

// Read-only assessment of the actual explicit NPulseChannel/NPulseSynapse
// equations, including secretion discretization, PSI, Weight and averaging.
// Unsupported equations/bindings are explicit diagnostics, never assumed safe.
NativeIntegrationAssessment AssessNativeIntegration(RDK::UEPtr<NPulseNeuron> neuron,double sourceAmplitude);
std::vector<InputParameters> ReadNativeInputParameters(const NativeProbeBindings& bindings,
                                                      const std::vector<int>& attachments);
// Native Save/Load preview validates a candidate before changing the live graph.
// No Calculate, learner Build/Reset, prototype mutation or per-tick repair.
NativeEditResult ApplyNativeInputParameters(NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& specification);
const char* NativeEditStatusName(NativeEditStatus status);
NativeEditSpec MakeNativeEditSpec(Normalization normalization,int maximumLength,const std::vector<InputParameters>& requested,
    bool nextSegment,bool segmentEI,int delta,double alpha,double maximumResistanceRatio,double configuredMinimumResistance,
    bool preserveAllocatedCable=false);
}
