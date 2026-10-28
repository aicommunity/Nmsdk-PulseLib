#pragma once
#include "NativeLearnedState.h"
#include "NativeModelOwner.h"
namespace NMSDK::TimeLearner {
template<class Owner> NativeModelBuildResult InstallNativeLearnedState(Owner& owner,bool branch,const QJsonObject& package,
    const std::string& implementationId,bool& owned,std::optional<NativeModelSpec>& declaration,
    std::optional<ConfiguredNativeTraining>& configured,std::string& configuredText,
    NativeProbeExecutor& probe,NativeTrainingService& service,std::string& failure)
{
    NativeModelBuildResult result;const bool wasOwned=owned;
    if(NativeTrainingIsActive(service) || probe.Status()==NativeProbeStatus::Queued || probe.Status()==NativeProbeStatus::Running)
    {result.reason="native_learned_import_requires_idle_boundary";return result;}
    try
    {
        CheckNativeOwnerOverrides(owner);
        auto prepared=PrepareNativeLearnedState(package,owner.GetStorage(),owner.GetEnvironment(),implementationId);
        if(prepared->configuration.model.branch!=branch)throw std::invalid_argument("native_learned_import_topology_mismatch");
        // Prepare all controller state and copies before replacing live children.
        auto restoredDeclaration=prepared->configuration.model;
        owned=true;
        result=CommitNativeLearnerChildren(owner,restoredDeclaration,prepared->parameters,prepared->neuron,prepared->dataset);
        if(result.status!=NativeModelBuildStatus::Adopted)
        {owned=wasOwned;if(result.modelMayHaveChanged){owned=true;failure=result.reason;service.FailConfiguration(result.reason);}return result;}
        declaration.emplace(std::move(restoredDeclaration));
        MirrorNativeModel(owner,*declaration,prepared->parameters,prepared->dataset->MatrixData.GetData());
        owner.ControllerVersion.SetDataDirect(prepared->controllerVersion);owner.CalculateMode.SetDataDirect(prepared->calculateMode);
        owner.StructureBuildMode.SetDataDirect(prepared->structureBuildMode);owner.ConfiguredModelInitialized.SetDataDirect(true);
        owner.ControllerConfiguration.SetDataDirect(prepared->configurationText);configuredText=prepared->configurationText;
        configured.emplace(std::move(prepared->configuration));
        owner.SyncCapacitanceTrim.SetDataDirect(0);owner.EnablePostTrainTuning.SetDataDirect(false);
        owner.AutoCalibrateFixedLTZThreshold.SetDataDirect(false);owner.IsNeedToTrain.SetDataDirect(false);
        probe.ResetControl();service=std::move(prepared->service);failure.clear();
    }
    catch(const std::exception& error)
    {
        result.reason=error.what();result.status=result.modelMayHaveChanged?NativeModelBuildStatus::Failed:NativeModelBuildStatus::Rejected;
        owned=result.modelMayHaveChanged?true:wasOwned;
        if(result.modelMayHaveChanged){failure=result.reason;service.FailConfiguration(result.reason);}
    }
    return result;
}
}
