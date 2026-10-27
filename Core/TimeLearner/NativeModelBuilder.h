#pragma once
#include "NativeModelEditor.h"

namespace NMSDK::TimeLearner {
struct NativeInitialInput {
    int attachment=1,synapses=1;
    // Omitted means the effective storage image value, never a parent fallback.
    std::optional<double> resistance;
};
struct NativeModelSpec {
    unsigned schemaVersion=1,frequencyHz=0;
    bool branch=false;
    std::string neuronClass,datasetClass="NDatasetMatrix",generatorClass="NPulseGeneratorTransit";
    int neuronStructureMode=2;
    Normalization normalization=Normalization::SynapseCount;
    std::vector<NativeInitialInput> inputs;
    std::vector<double> initialIsi;
    // Optional explicit allocation, including extra cable. Mode 1 requires a
    // common scalar allocation; mode 0 uses the graph in the declared image.
    std::vector<int> allocatedCableLengths;
    NativeEditSpec editPolicy;
    bool scaleInhibitionWithInput=true;
    // Compatibility adoption may retain a Branch's native muted baseline.
    // Actual binding state remains part of the model hash; no implicit relink.
    bool allowMutedInputBaseline=false;
};
enum class NativeModelBuildStatus { Constructed,Adopted,Rejected,Failed };
struct NativeModelBuildResult {
    NativeModelBuildStatus status=NativeModelBuildStatus::Rejected;
    std::string reason,modelRevision;
    bool modelMayHaveChanged=false;
    std::vector<InputParameters> effective;
};
void ValidateNativeModelSpec(const NativeModelSpec& specification);
// Creates both children privately before attaching them. Existing named slots
// are rejected; this is an explicit cold operation, never an inferred reset.
// Uniform physics comes entirely from images. No Default or Calculate calls.
NativeModelBuildResult ConstructNativeLearnerModel(RDK::UNet& owner,const NativeModelSpec& specification,
                                                  bool explicitColdReplacement=false);
// Read-only validation of the current learned graph, not an initial-parameter
// overlay. Attachments are current learned values; Rs/C may differ from initial.
NativeModelBuildResult InspectNativeLearnerModel(RDK::UNet& owner,const NativeModelSpec& specification,
                                               const std::vector<int>& attachments);
NativeModelSpec DescribeNativeLearnerModel(RDK::UNet& owner,bool branch,Normalization normalization,
    const std::vector<int>& attachments,const NativeEditSpec& policy);
NativeEditSpec NativeModelEditPolicy(const NativeModelSpec& declaration,const std::vector<InputParameters>& requested);
bool ValidNativeTarget(const MDMatrix<double>& pattern,std::size_t inputs);
void AssignNativeTarget(NDatasetMatrix& dataset,const MDMatrix<double>& pattern);
const char* NativeModelBuildStatusName(NativeModelBuildStatus status);
}
