#pragma once
#include "NativeModelBuilder.h"
#include "NativeTrainingBindings.h"

namespace NMSDK::TimeLearner {

template<class Owner> NativeEditSpec NativeOwnerEditSpec(Owner& owner,const std::optional<NativeModelSpec>& declaration,
    const std::vector<InputParameters>& requested,double configuredFloor)
{
    auto policy=MakeNativeEditSpec(owner.NormalizationMode.GetData()==0?Normalization::SynapseCount:Normalization::TipResistance,
        owner.MaxDendriteLength.GetData(),requested,owner.EnableNextSegmentInhibition.GetData(),owner.EnableSegExcInh.GetData(),
        owner.SegExcInhDelta.GetData(),owner.SegExcInhAlpha.GetData(),owner.MaxSynapseToMembraneResistanceRatio.GetData(),
        configuredFloor,owner.StructureBuildMode.GetData()==0);
    if(declaration)policy=NativeModelEditPolicy(*declaration,requested);
    return policy;
}
template<class Owner> void CheckNativeOwnerOverrides(const Owner& owner)
{
    if(owner.OverrideMembraneCapacity.GetData()!=0. || owner.OverrideExcMembraneResistance.GetData()!=0. ||
       owner.OverrideInhMembraneResistance.GetData()!=0. || owner.OverrideExcSynapseResistance.GetData()!=0. ||
       owner.OverrideInhSynapseResistance.GetData()!=0.)
        throw std::invalid_argument("native_model_requires_images_instead_of_parent_overrides");
}
template<class Owner> void MirrorNativeModel(Owner& owner,const NativeModelSpec& s,const std::vector<InputParameters>& actual,
                                            const MDMatrix<double>& pattern)
{
    std::vector<int> positions,counts;std::vector<double> resistances;
    for(const auto& p:actual){positions.push_back(p.length);counts.push_back(p.synapses);resistances.push_back(p.resistance);}
    owner.NumInputDendrite.SetDataDirect(int(actual.size()));owner.NeuronClassName.SetDataDirect(s.neuronClass);
    owner.PulseGeneratorClassName.SetDataDirect(s.generatorClass);
    owner.NormalizationMode.SetDataDirect(s.normalization==Normalization::SynapseCount?0:1);
    owner.MaxDendriteLength.SetDataDirect(s.editPolicy.maximumCableLength);
    owner.MaxSynapseToMembraneResistanceRatio.SetDataDirect(s.editPolicy.maximumResistanceRatio);
    owner.DendriteLength.SetDataDirect(positions);owner.NumSynapse.SetDataDirect(counts);
    owner.TipSynapseResistance.SetDataDirect(resistances);owner.InputPattern.SetDataDirect(pattern);
}
template<class Owner> NativeModelBuildResult ValidateNativeOwner(Owner& owner,const NativeModelSpec& s)
{
    CheckNativeOwnerOverrides(owner);
    if(owner.NumInputDendrite.GetData()!=int(s.inputs.size()) || owner.NeuronClassName.GetData()!=s.neuronClass ||
       owner.PulseGeneratorClassName.GetData()!=s.generatorClass ||
       owner.NormalizationMode.GetData()!=(s.normalization==Normalization::SynapseCount?0:1) ||
       owner.MaxDendriteLength.GetData()!=s.editPolicy.maximumCableLength ||
       owner.MaxSynapseToMembraneResistanceRatio.GetData()!=s.editPolicy.maximumResistanceRatio)
        throw std::invalid_argument("native_model_owner_declaration_mismatch");
    auto actual=InspectNativeLearnerModel(owner,s,owner.DendriteLength.GetData());
    if(actual.status!=NativeModelBuildStatus::Adopted)throw std::invalid_argument(actual.reason);
    const auto dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    if(!NativeTrainingPatternEqual(owner.InputPattern.GetData(),dataset->MatrixData.GetData()))
        throw std::invalid_argument("native_model_owner_target_mismatch");
    if(owner.NumSynapse.size()!=actual.effective.size() || owner.TipSynapseResistance.size()!=actual.effective.size())
        throw std::invalid_argument("native_model_owner_parameter_dimension_mismatch");
    for(std::size_t i=0;i<actual.effective.size();++i)
        if(owner.NumSynapse[i]!=actual.effective[i].synapses || owner.TipSynapseResistance[i]!=actual.effective[i].resistance)
            throw std::invalid_argument("native_model_owner_parameter_readback_mismatch");
    return actual;
}
// Shared construction/adoption boundary. It never calls learner Build/Init or
// Calculate. The component wrapper marks itself unready for native lifecycle.
template<class Owner> NativeModelBuildResult InitializeNativeOwnedModel(Owner& owner,NativeModelSpec s,bool branch,bool adopt,
    bool& owned,std::optional<NativeModelSpec>& declaration,NativeProbeExecutor& probe,NativeTrainingService& service,
    std::string& failure)
{
    NativeModelBuildResult result;
    if(NativeTrainingIsActive(service) || probe.Status()==NativeProbeStatus::Queued || probe.Status()==NativeProbeStatus::Running)
        throw std::logic_error("native_model_initialization_requires_idle_boundary");
    const bool wasOwned=owned;
    try
    {
        ValidateNativeModelSpec(s);CheckNativeOwnerOverrides(owner);
        if(s.branch!=branch)throw std::invalid_argument("native_model_learner_topology_mismatch");
        owned=true; // Suppress legacy AAddComponent dataset repair during commit.
        std::vector<int> positions;for(const auto& p:s.inputs)positions.push_back(p.attachment);
        result=adopt?InspectNativeLearnerModel(owner,s,positions):ConstructNativeLearnerModel(owner,s,true);
        if(result.status!=NativeModelBuildStatus::Constructed && result.status!=NativeModelBuildStatus::Adopted)
        {owned=wasOwned;if(result.modelMayHaveChanged){owned=true;failure=result.reason;service.FailConfiguration(result.reason);}return result;}
        const auto dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        for(std::size_t i=0;i<s.inputs.size();++i)
        {
            if(adopt && (s.inputs[i].synapses!=result.effective[i].synapses ||
                (s.inputs[i].resistance && *s.inputs[i].resistance!=result.effective[i].resistance) ||
                s.initialIsi[i]!=dataset->MatrixData(int(i),0)))
                throw std::invalid_argument("native_adoption_declaration_does_not_match_actual_state");
            // Explicit baseline for inhibition scaling, not a target amplitude.
            s.inputs[i].resistance=result.effective[i].resistance;
        }
        declaration.emplace(std::move(s));MirrorNativeModel(owner,*declaration,result.effective,dataset->MatrixData.GetData());
        probe.ResetControl();service.InvalidateTarget();failure.clear();owner.IsNeedToTrain.SetDataDirect(false);
    }
    catch(const std::exception& e)
    {result.reason=e.what();result.status=result.modelMayHaveChanged?NativeModelBuildStatus::Failed:NativeModelBuildStatus::Rejected;
     owned=result.modelMayHaveChanged?true:wasOwned;if(result.modelMayHaveChanged){failure=result.reason;service.FailConfiguration(result.reason);}}
    return result;
}
}
