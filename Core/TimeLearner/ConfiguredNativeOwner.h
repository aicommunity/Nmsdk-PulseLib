#pragma once
#include "ConfiguredNativeTraining.h"

namespace NMSDK::TimeLearner {
template<class Owner> ConfiguredNativeTraining PrepareConfiguredNativeOwner(Owner& owner,bool branch,bool hasDeclaration)
{
    auto c=ParseConfiguredNativeTraining(owner.ControllerConfiguration.GetData());
    if(c.model.branch!=branch)throw std::invalid_argument("configured_native_topology_mismatch");
    if(owner.SyncCapacitanceTrim.GetData()>0 || owner.EnablePostTrainTuning.GetData() || owner.AutoCalibrateFixedLTZThreshold.GetData())
        throw std::invalid_argument("configured_native_optional_mode_not_yet_supported");
    if(!hasDeclaration)
    {
        if(owner.ConfiguredModelInitialized.GetData())throw std::invalid_argument("configured_trained_model_requires_explicit_state_import");
        const bool need=owner.IsNeedToTrain.GetData();
        const auto result=owner.ConstructNativeModel(c.model);
        if(result.status!=NativeModelBuildStatus::Constructed)throw std::invalid_argument(result.reason);
        owner.ConfiguredModelInitialized.SetDataDirect(true);owner.IsNeedToTrain.SetDataDirect(need);
    }
    auto generator=owner.template GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
    if(!generator)throw std::invalid_argument("missing_configured_native_generator");
    const StimulusProfile actual{generator->GetCompClassName(),generator->Amplitude.GetData(),generator->PulseLength.GetData(),
        double(generator->GetTimeStep()),generator->FrequencyDeviation.GetData(),generator->UseTransitSignal.GetData(),generator->UsePatternOutput.GetData()};
    if(actual!=c.stimulusProfile)throw std::invalid_argument("configured_native_effective_stimulus_mismatch");
    owner.ConfiguredModelInitialized.SetDataDirect(true);
    return c;
}
}
