#pragma once
#include "NativeRecognitionState.h"
#include "NativeModelOwner.h"
#include "NativeRecognitionTrial.h"
#include <cmath>

namespace NMSDK::TimeLearner {
// A frozen physical model, not a training controller/checkpoint. Source solo
// proof remains inside the immutable package and never becomes a recognition
// training certificate. Presentation dynamics remain native component state.
struct NativeRecognitionHold {
    QJsonObject package;
    NativeModelSpec model;
    std::vector<InputParameters> parameters;
    MDMatrix<double> target,presentation;
    StimulusProfile stimulus;
    std::string revision,configurationText;
    int controllerVersion=1,calculateMode=0,structureBuildMode=1;
    double threshold=0.;
    NativeRecognitionTrialExecutor trials;
    std::shared_ptr<NmsdkTools::ModelRevisionObserver> observer=std::make_shared<NmsdkTools::ModelRevisionObserver>();
};

template<class Owner,class Visitor> void VisitNativeRecognitionControls(Owner& owner,const NativeRecognitionHold& h,Visitor visit)
{
    visit(owner.ControllerVersion,h.controllerVersion);visit(owner.CalculateMode,h.calculateMode);
    visit(owner.StructureBuildMode,h.structureBuildMode);visit(owner.ConfiguredModelInitialized,true);
    visit(owner.ControllerConfiguration,h.configurationText);visit(owner.IsNeedToTrain,false);
    visit(owner.SyncCapacitanceTrim,0);visit(owner.EnablePostTrainTuning,false);
    visit(owner.AutoCalibrateFixedLTZThreshold,false);visit(owner.UseFixedLTZThreshold,true);
    visit(owner.LTZThreshold,h.threshold);visit(owner.FixedLTZThreshold,h.threshold);
    // Inherited fixed threshold is not the result of calibration on Test.
    visit(owner.CalibratedFixedLTZThreshold,0.);
}

template<class Owner> void CheckNativeRecognitionHold(Owner& owner,NativeRecognitionHold& h,bool full=false)
{
    CheckNativeOwnerOverrides(owner);
    const auto same=[](const auto& p,const auto& value) {
        if(!NativeOwnerValueEqual(p.GetData(),value))
            throw std::invalid_argument("native_recognition_owner_changed:"+p.GetName());
    };
    VisitNativeModelMirror(owner,h.model,h.parameters,h.target,same);
    VisitNativeRecognitionControls(owner,h,same);
    auto neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
    auto dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    if(!neuron || !dataset || owner.GetTimeStep()!=h.model.frequencyHz || dataset->GetTimeStep()!=h.model.frequencyHz)
        throw std::invalid_argument("native_recognition_context_changed");
    auto source=dataset->template GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    if(!source || !NativeTrainingPatternEqual(dataset->MatrixData.GetData(),h.presentation))
        throw std::invalid_argument("native_recognition_presentation_changed");
    const StimulusProfile actual{source->GetCompClassName(),source->Amplitude.GetData(),source->PulseLength.GetData(),
        double(source->GetTimeStep()),source->FrequencyDeviation.GetData(),source->UseTransitSignal.GetData(),source->UsePatternOutput.GetData()};
    if(actual!=h.stimulus)throw std::invalid_argument("native_recognition_stimulus_changed");
    if(h.observer->Read(neuron,{{&source->Output,"burst"}},full).toStdString()!=h.revision)
        throw std::invalid_argument("native_recognition_model_changed");
}

template<class Owner> NativeModelBuildResult InstallNativeRecognitionState(Owner& owner,bool branch,const QJsonObject& package,
    const std::string& implementation,bool& owned,std::optional<NativeModelSpec>& declaration,
    std::optional<ConfiguredNativeTraining>& configured,std::string& configuredText,
    NativeProbeExecutor& probe,NativeTrainingService& service,std::string& failure,std::unique_ptr<NativeRecognitionHold>& hold)
{
    NativeModelBuildResult result;const bool wasOwned=owned;
    if((hold && (hold->trials.Busy() || hold->trials.HasResult())) || NativeTrainingIsActive(service) || probe.Status()==NativeProbeStatus::Queued || probe.Status()==NativeProbeStatus::Running)
    {result.reason="native_recognition_import_requires_idle_boundary";return result;}
    try
    {
        CheckNativeOwnerOverrides(owner);
        auto prepared=PrepareNativeRecognitionState(package,owner.GetStorage(),owner.GetEnvironment(),implementation);
        if(prepared->configuration.model.branch!=branch)throw std::invalid_argument("native_recognition_import_topology_mismatch");
        auto next=std::make_unique<NativeRecognitionHold>();next->package=package;next->model=prepared->configuration.model;
        next->parameters=prepared->parameters;next->target=prepared->dataset->MatrixData.GetData();next->presentation=next->target;
        next->stimulus=prepared->configuration.stimulusProfile;next->revision=prepared->modelRevision;
        next->configurationText=prepared->configurationText;next->controllerVersion=prepared->controllerVersion;
        next->calculateMode=prepared->calculateMode;next->structureBuildMode=prepared->structureBuildMode;
        const auto threshold=package["neuron"].toObject()["metadata"].toObject()["calibration"].toObject()["threshold"];
        if(!threshold.isDouble() || !std::isfinite(threshold.toDouble()))throw std::invalid_argument("invalid_recognition_threshold");
        next->threshold=threshold.toDouble();
        CheckNativeModelMirrorAssignments(owner,next->model,next->parameters,next->target);
        VisitNativeRecognitionControls(owner,*next,[](const auto& p,const auto& value){CheckNativeOwnerAssignment(p,value);});
        owned=true;
        result=CommitNativeLearnerChildren(owner,next->model,next->parameters,prepared->neuron,prepared->dataset);
        if(result.status!=NativeModelBuildStatus::Adopted)
        {owned=result.modelMayHaveChanged?true:wasOwned;if(result.modelMayHaveChanged)failure=result.reason;return result;}
        MirrorNativeModel(owner,next->model,next->parameters,next->target);
        VisitNativeRecognitionControls(owner,*next,[](auto& p,const auto& value){AssignNativeOwnerValue(p,value);});
        declaration=next->model;configured=std::move(prepared->configuration);configuredText=next->configurationText;
        probe.ResetControl();service=NativeTrainingService{};failure.clear();hold=std::move(next);
        CheckNativeRecognitionHold(owner,*hold,true);
    }
    catch(const std::exception& error)
    {
        result.reason=error.what();result.status=result.modelMayHaveChanged?NativeModelBuildStatus::Failed:NativeModelBuildStatus::Rejected;
        owned=result.modelMayHaveChanged?true:wasOwned;
        if(result.modelMayHaveChanged){failure=result.reason;service.FailConfiguration(result.reason);}
    }
    return result;
}

template<class Owner> void BeforeNativeRecognitionTrial(Owner& owner,NativeRecognitionHold& h,
    const NativeProbeBindings& bindings,double time)
{
    h.trials.BeforeChildren(bindings,time,[&](const MDMatrix<double>& pattern) {
        auto dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        if(!dataset)throw std::runtime_error("missing_recognition_dataset");
        AssignNativeTarget(*dataset,pattern);h.presentation=pattern;
    },[&](bool full) { CheckNativeRecognitionHold(owner,h,full);return h.revision; });
}
}
