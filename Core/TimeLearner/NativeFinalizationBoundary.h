#pragma once
#include "NativePostTuneConfiguration.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace NMSDK::TimeLearner {

// Public legacy values deliberately do not have the native enum's order.
inline NativePostTuneControllerMode NativePostTuneModeFromPublic(int value)
{
    switch(value) {
    case 0:return NativePostTuneControllerMode::Off;
    case 1:return NativePostTuneControllerMode::CanonRmin;
    case 2:return NativePostTuneControllerMode::FlatLastR;
    case 3:return NativePostTuneControllerMode::KeepDone;
    case 4:return NativePostTuneControllerMode::SearchSynthetic;
    }
    throw std::invalid_argument("invalid_public_post_tune_mode");
}
inline NativePostTuneMetric NativePostTuneMetricFromPublic(int value)
{
    switch(value) {
    case 0:return NativePostTuneMetric::Auto;
    case 1:return NativePostTuneMetric::LTZone;
    case 2:return NativePostTuneMetric::SomaSum;
    }
    throw std::invalid_argument("invalid_public_post_tune_metric");
}

template<class Owner> QJsonObject ReadNativeFinalizationControls(const Owner& owner)
{
    QJsonObject parameters,bindings;
    const auto add=[&](const auto& property) {
        const auto value=property.GetData();QJsonValue encoded;
        if constexpr(std::is_same_v<std::decay_t<decltype(value)>,std::string>)encoded=QString::fromStdString(value);
        else {
            if constexpr(std::is_floating_point_v<std::decay_t<decltype(value)>>)
                if(!std::isfinite(value))throw std::invalid_argument("nonfinite_finalization_control:"+property.GetName());
            encoded=QJsonValue(value);
        }
        const auto name=QString::fromStdString(property.GetName());parameters[name]=encoded;
        QJsonArray sources;
        for(const auto* source:property.GetDataSources()) {
            if(!source || !source->GetOwner())throw std::invalid_argument("unresolved_finalization_control_source:"+property.GetName());
            sources.append(QJsonObject{{"owner_path",QString::fromStdString(source->GetOwner()->GetFullName())},
                {"property",QString::fromStdString(source->GetName())}});
        }
        if((property.IsConnected() || property.UsesExternalDataSource()) && sources.isEmpty())
            throw std::invalid_argument("uninspectable_finalization_control_source:"+property.GetName());
        bindings[name]=QJsonObject{{"connected",property.IsConnected()},{"external_data_source",property.UsesExternalDataSource()},
            {"sources",sources}};
    };
    add(owner.ControllerVersion);add(owner.ControllerConfiguration);add(owner.CalculateMode);add(owner.StructureBuildMode);
    add(owner.IsNeedToTrain);add(owner.NormalizationMode);add(owner.SyncCapacitanceTrim);
    add(owner.TrainingLTZThreshold);add(owner.UseFixedLTZThreshold);add(owner.FixedLTZThreshold);add(owner.LTZThreshold);
    add(owner.AutoCalibrateFixedLTZThreshold);add(owner.CalibrateLTZThresholdMode);add(owner.CalibrateLTZThresholdFraction);
    add(owner.CalibrateLTZThresholdMin);add(owner.CalibrateLTZThresholdMax);add(owner.ResetNeuronBeforeTrainBurst);
    add(owner.EnablePostTrainTuning);add(owner.PostTrainTipResistanceMode);add(owner.EnablePostTrainMidThreshold);
    add(owner.PostTrainMidMetric);add(owner.TipResistanceCanonFloor);add(owner.TipResistanceCanonLast);
    add(owner.PostTrainSilentThreshold);add(owner.UseCandidateMidOnNonSeparableTest);
    add(owner.PostTrainSyntheticFoilCount);add(owner.PostTrainTipSearchIters);
    return {{"parameters",parameters},{"bindings",bindings}};
}

class NativeFinalizationBoundary;
template<class Owner> NativeFinalizationBoundary CaptureNativeFinalizationBoundary(
    Owner& owner,const std::string& implementationId,bool allowDiagnostic=false);

// Owns immutable values only. No owner, storage, callback or dynamic checkpoint
// survives here. Import may reset the live service/mirrors without losing the
// original declaration or changing a stopped outcome into completed training.
class NativeFinalizationBoundary {
public:
    NativeFinalizationBoundary(const NativeFinalizationBoundary&)=default;
    NativeFinalizationBoundary(NativeFinalizationBoundary&&)=default;
    const QJsonObject& Document() const {return document;}
    QJsonObject Solo() const {return document["source_solo"].toObject();}
    QJsonObject Controls() const {return document["controls"].toObject();}
    bool DiagnosticOnly() const {return document["diagnostic_only"].toBool();}
private:
    explicit NativeFinalizationBoundary(QJsonObject value):document(std::move(value)){}
    const QJsonObject document;
    template<class Owner> friend NativeFinalizationBoundary CaptureNativeFinalizationBoundary(
        Owner&,const std::string&,bool);
};

template<class Owner> NativeFinalizationBoundary CaptureNativeFinalizationBoundary(
    Owner& owner,const std::string& implementationId,bool allowDiagnostic)
{
    const auto require=[](bool value,const char* reason){if(!value)throw std::logic_error(reason);};
    require(!owner.HasNativeRecognitionState(),"finalization_boundary_requires_solo");
    require(owner.IsReady() && owner.IsInit(),"finalization_boundary_requires_initialized_owner");
    require(owner.IsNativeProbeAtEditBoundary(),"finalization_boundary_requires_consumed_probe_boundary");
    const auto& service=owner.NativeTrainingController();
    require(service.State()==NativeTrainingState::Finished,"finalization_boundary_requires_finished_training");
    std::vector<RDK::UModuleLifecycleObservation> lifecycles;std::set<RDK::UContainer*> seen;
    const auto watch=[&](auto&& visit,RDK::UContainer* node)->void {
        require(node && seen.insert(node).second,"invalid_finalization_owner_tree");
        lifecycles.push_back(node->ObserveLifecycle());
        std::vector<RDK::NameT> names;node->GetComponentsList(names);
        for(const auto& name:names)visit(visit,node->GetComponent(name,true).Get());
    };
    watch(watch,&owner);const auto controls=ReadNativeFinalizationControls(owner);
    const auto outcome=service.Outcome();const auto attempt=service.SessionAttempt(),epoch=service.ModelEpoch();
    const auto solo=owner.ExportNativeLearnedState(implementationId);
    const bool stopped=solo["kind"]==QJsonValue("timelearner_stopped_state");
    require(solo["kind"]==QJsonValue("timelearner_completed_state") || stopped,"invalid_finalization_solo_boundary");
    require(!stopped || allowDiagnostic,"finalization_boundary_stopped_training_requires_diagnostic_opt_in");
    require(std::all_of(lifecycles.begin(),lifecycles.end(),[](const auto& life){return life.IsCurrent();}) &&
        owner.IsNativeProbeAtEditBoundary() &&
        service.State()==NativeTrainingState::Finished && service.Outcome()==outcome &&
        service.SessionAttempt()==attempt && service.ModelEpoch()==epoch &&
        ReadNativeFinalizationControls(owner)==controls,"finalization_boundary_changed_during_capture");
    const auto parameters=controls["parameters"].toObject();
    const bool postTune=parameters["EnablePostTrainTuning"].toBool();
    if(postTune) {
        NativePostTuneModeFromPublic(parameters["PostTrainTipResistanceMode"].toInt());
        NativePostTuneMetricFromPublic(parameters["PostTrainMidMetric"].toInt());
    }
    const bool branch=solo["configuration"].toObject()["model"].toObject()["branch"].toBool();
    const bool parallelPeak=branch && !postTune;
    const bool targetReference=parameters["AutoCalibrateFixedLTZThreshold"].toBool();
    if(targetReference || parallelPeak)
        require(parameters["CalibrateLTZThresholdMode"]==QJsonValue(0) || parameters["CalibrateLTZThresholdMode"]==QJsonValue(1),
            "invalid_public_target_reference_threshold_mode");
    QJsonObject value{{"kind","timelearner_finalization_boundary"},{"schema_version",1},
        {"implementation_id",QString::fromStdString(implementationId)},{"source_solo",solo},
        {"controls",controls},{"training_outcome",solo["training_outcome"]},{"diagnostic_only",stopped},
        {"phase_requests",QJsonObject{{"target_reference",targetReference},{"post_tune",postTune},
            {"post_train_mid",parameters["EnablePostTrainMidThreshold"]},{"legacy_parallel_peak",parallelPeak}}},
        {"dynamic_checkpoint",false},{"mid_probe_resume_supported",false},{"recognition_success",QJsonValue(QJsonValue::Null)}};
    value["sha256"]=NmsdkTools::CanonicalModelDataSha256(value);
    return NativeFinalizationBoundary(std::move(value));
}
}
