#pragma once
#include "NativeModelEditor.h"
#include "ModelIdentity.h"

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
// Reconstruct the actual physical neuron through the common native snapshot
// path. Dataset, images, effective child physics and owner order are retained.
// No defaults, inferred parent physics or nested Calculate. The caller must
// refresh any cached Neuron pointer after return, including a failed commit.
NativeModelBuildResult ReconstructNativeLearnerNeuron(RDK::UNet& owner,
    const NmsdkTools::ModelBoundarySources& boundarySources);
// Physical stage-boundary rollback foundation for PostTune. Captures both
// children before any candidate edit and reconnects the archived neuron to the
// restored Dataset generator. Same owner/storage/environment only. No clock,
// controller/learner properties, certificate or mid-probe dynamics are restored.
// Callers must refresh cached child aliases after every Restore result.
class NativeLearnerGraphCheckpoint {
public:
    static std::unique_ptr<NativeLearnerGraphCheckpoint> Capture(RDK::UNet& owner);
    NativeModelBuildResult Restore(RDK::UNet& owner) const;
    QJsonObject NeuronIdentity() const {return neuronSnapshot["identity"].toObject();}
private:
    NativeLearnerGraphCheckpoint()=default;
    RDK::UNet* capturedOwner=nullptr;
    RDK::UStorage* capturedStorage=nullptr;
    RDK::UEnvironment* capturedEnvironment=nullptr;
    RDK::UModuleLifecycleObservation ownerLifecycle;
    unsigned frequency=0;
    std::vector<RDK::NameT> order;
    QJsonObject neuronSnapshot,datasetSnapshot;
};
// Verified unowned import children. Preserves owner order, checks external
// listeners before replacement and restores the old children on commit failure.
NativeModelBuildResult CommitNativeLearnerChildren(RDK::UNet& owner,const NativeModelSpec& specification,
    const std::vector<InputParameters>& expected,RDK::UEPtr<NPulseNeuron> neuron,RDK::UEPtr<NDatasetMatrix> dataset);
NativeModelSpec DescribeNativeLearnerModel(RDK::UNet& owner,bool branch,Normalization normalization,
    const std::vector<int>& attachments,const NativeEditSpec& policy);
NativeEditSpec NativeModelEditPolicy(const NativeModelSpec& declaration,const std::vector<InputParameters>& requested);
bool ValidNativeTarget(const MDMatrix<double>& pattern,std::size_t inputs);
void AssignNativeTarget(NDatasetMatrix& dataset,const MDMatrix<double>& pattern);
const char* NativeModelBuildStatusName(NativeModelBuildStatus status);
}
