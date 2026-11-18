#pragma once
#include "NativeModelBuilder.h"
#include "ConfiguredNativeTraining.h"
#include "ModelArchive.h"
#include <QJsonDocument>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace NMSDK::TimeLearner {
class NativeBoundaryRetentionBudgetExhausted : public std::runtime_error {
public:
    NativeBoundaryRetentionBudgetExhausted():std::runtime_error("native_measured_boundary_storage_budget_exhausted") {}
};

// A process-local physical checkpoint of a controller-selected, fully measured
// candidate. It carries neither a certificate nor reusable training authority.
// Capture does not Build/Reset/Calculate or restore a graph. Terminal restore
// must separately reconcile the stopped controller, epoch and portable export.
class NativeMeasuredTrainingBoundary {
public:
    static constexpr std::size_t MaximumRetainedBytes=64*1024*1024;
    template<class Owner> static std::shared_ptr<const NativeMeasuredTrainingBoundary> Capture(
        Owner& owner,const MeasuredCandidateSelection& selected)
    {
        const auto& control=owner.NativeTrainingControl;
        Require(owner.NativeTrainingOwned,"measured_boundary_requires_native_owner");
        CheckLocalProperties(owner);
        Require(control.State()==NativeTrainingState::Searching && control.Search() &&
            control.Search()->SelectedMeasuredBoundary() &&
            control.Search()->SelectedMeasuredBoundary()->serial==selected.serial &&
            control.Search()->SelectedMeasuredBoundary()->candidate.trialId==selected.candidate.trialId,
            "measured_boundary_controller_selection_changed");
        RequireQuiet(owner);
        Require(control.ModelEpoch()==selected.candidate.modelEpoch &&
            owner.ReadNativeParameters()==selected.candidate.fullParameters,
            "measured_boundary_parameters_or_epoch_changed");
        auto result=std::shared_ptr<NativeMeasuredTrainingBoundary>(new NativeMeasuredTrainingBoundary);
        result->selection=selected;result->ownerLifecycle=owner.ObserveLifecycle();
        result->graph=NativeLearnerGraphCheckpoint::Capture(owner);
        Require(result->graph->NeuronIdentity()["sha256"].toString().toStdString()==selected.candidate.modelRevision,
            "measured_boundary_physical_revision_changed");
        result->capacitanceState=owner.NativeLearnedCapacitanceState;
        if(!result->capacitanceState.isEmpty())owner.ValidateAppliedNativeCapacitanceState(result->capacitanceState);
        auto neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
        auto dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        auto source=owner.template GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
        Require(neuron && dataset && source,"measured_boundary_children_missing");
        std::vector<std::string> additional;
        for(const auto& definition:result->capacitanceState["baseline"].toObject()["storage_images"].toArray())
            additional.push_back(definition.toObject()["name"].toString().toStdString());
        additional.push_back(dataset->GetCompClassName());
        additional.push_back(source->GetCompClassName());
        result->images=NmsdkTools::CaptureModelImageDefinitions(neuron,{{&source->Output,"burst"}},additional);
        result->imageFingerprint=NmsdkTools::ModelImageDefinitionsFingerprint(result->images).toStdString();
        result->retainedBytes=result->graph->EncodedBytes()+
            std::size_t(QJsonDocument(result->images).toJson(QJsonDocument::Compact).size())+
            std::size_t(QJsonDocument(result->capacitanceState).toJson(QJsonDocument::Compact).size());
        if(result->retainedBytes>MaximumRetainedBytes)throw NativeBoundaryRetentionBudgetExhausted{};
        const auto values=Values(Mirrors(owner));const auto local=Values(Local(owner));
        const auto configuration=Values(Configuration(owner));const auto target=owner.InputPattern.GetData();
        const auto declaration=owner.NativeModelDeclaration?EncodeNativeModelSpec(*owner.NativeModelDeclaration):QJsonObject{};
        const auto attempt=control.SessionAttempt(),captureEpoch=control.CaptureEpoch();
        const auto captureRevision=control.CaptureRevision();
        result->checkScope=[&owner,configuration,target,declaration,attempt,captureEpoch,captureRevision] {
            RequireQuiet(owner);
            Require(Matches(Configuration(owner),configuration) &&
                (owner.NativeModelDeclaration?EncodeNativeModelSpec(*owner.NativeModelDeclaration):QJsonObject{})==declaration,
                "measured_boundary_configuration_changed");
            Require(PatternEqual(owner.InputPattern.GetData(),target),"measured_boundary_target_changed");
            const auto& current=owner.NativeTrainingControl;
            Require(current.SessionAttempt()==attempt && current.CaptureEpoch()==captureEpoch &&
                current.CaptureRevision()==captureRevision,"measured_boundary_training_scope_changed");
        };
        result->restoreMirrors=[&owner,values,local,state=result->capacitanceState] {
            AssignProperties(Mirrors(owner),values);AssignLocal(Local(owner),local);
            owner.NativeLearnedCapacitanceState=state;
            Require(Matches(Mirrors(owner),values) && Matches(Local(owner),local),
                "measured_boundary_owner_readback_mismatch");
            if(!state.isEmpty())owner.ValidateAppliedNativeCapacitanceState(state);
        };
        result->refresh=[&owner] {
            owner.Neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
            owner.Dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        };
        return result;
    }
    const MeasuredCandidateSelection& Selection() const {return selection;}
    QJsonObject NeuronIdentity() const {return graph->NeuronIdentity();}
    QJsonObject DatasetIdentity() const {return graph->DatasetIdentity();}
    const QJsonObject& CapacitanceState() const {return capacitanceState;}
    const std::string& StorageFingerprint() const {return imageFingerprint;}
    std::size_t RetainedBytes() const {return retainedBytes;}
private:
    friend class NativeTrainingService;
    NativeMeasuredTrainingBoundary()=default;
    MeasuredCandidateSelection selection;
    RDK::UModuleLifecycleObservation ownerLifecycle;
    std::unique_ptr<NativeLearnerGraphCheckpoint> graph;
    QJsonObject capacitanceState;
    QJsonArray images;
    std::string imageFingerprint;
    std::size_t retainedBytes=0;
    // Mutating callbacks intentionally remain private until the service's
    // separate terminal transaction and versioned portable event are bound.
    std::function<void()> checkScope,restoreMirrors,refresh;
    static void Require(bool value,const char* reason){if(!value)throw std::logic_error(reason);}
    template<class Owner> static void RequireQuiet(const Owner& owner)
    {
        Require(owner.NativeProbeControl.Status()!=NativeProbeStatus::Queued &&
            owner.NativeProbeControl.Status()!=NativeProbeStatus::Running && !owner.NativeProbeControl.Result(),
            "measured_boundary_probe_not_consumed");
        Require(!owner.CapApplyPending && !owner.PostTuneFreeRunActive && !owner.PostTuneInferenceMidPending,
            "measured_boundary_other_control_active");
    }
    template<class Owner> static auto Mirrors(Owner& o)
    {return std::tie(o.DendriteLength,o.NumSynapse,o.TipSynapseResistance);}
    template<class Owner> static auto Local(Owner& o)
    {return std::tie(o.OldDendriteLength,o.LoadedTipSynapseResistance,o.HasLoadedTipSynapseResistance,
        o.CanChangeDendLength,o.CapApplyPending,o.NativeEditFailure);}
    template<class Owner> static auto Configuration(Owner& o)
    {return std::tie(o.ControllerVersion,o.ControllerConfiguration,o.ConfiguredModelInitialized,
        o.CalculateMode,o.StructureBuildMode,o.NumInputDendrite,o.NeuronClassName,o.PulseGeneratorClassName,
        o.NormalizationMode,o.MaxDendriteLength,o.MaxSynapseToMembraneResistanceRatio);}
    template<class Owner> static void CheckLocalProperties(Owner& owner)
    {
        const auto local=[](const auto& property) {
            Require(!property.IsConnected() && !property.UsesExternalDataSource(),
                "measured_boundary_connected_owner_property");
        };
        std::apply([&](const auto&... p){(local(p),...);},Mirrors(owner));
        std::apply([&](const auto&... p){(local(p),...);},Configuration(owner));local(owner.InputPattern);
    }
    template<class P> static bool PatternEqual(const P& a,const P& b)
    {
        if(a.GetRows()!=b.GetRows() || a.GetCols()!=b.GetCols())return false;
        for(int r=0;r<a.GetRows();++r)for(int c=0;c<a.GetCols();++c)if(a(r,c)!=b(r,c))return false;
        return true;
    }
    template<class T> static auto Read(const T& v)
    {if constexpr(requires{v.GetData();})return v.GetData();else return v;}
    template<class Tuple> static auto Values(const Tuple& refs)
    {return std::apply([](const auto&... v){return std::make_tuple(Read(v)...);},refs);}
    template<class Refs,class Data,std::size_t... I> static bool MatchesImpl(const Refs& refs,const Data& data,std::index_sequence<I...>)
    {return ((Read(std::get<I>(refs))==std::get<I>(data)) && ...);}
    template<class Refs,class Data> static bool Matches(const Refs& refs,const Data& data)
    {return MatchesImpl(refs,data,std::make_index_sequence<std::tuple_size_v<Data>>{});}
    template<class Refs,class Data,std::size_t... I> static void AssignPropertiesImpl(Refs refs,const Data& data,std::index_sequence<I...>)
    {(std::get<I>(refs).SetDataDirect(std::get<I>(data)),...);}
    template<class Refs,class Data> static void AssignProperties(Refs refs,const Data& data)
    {AssignPropertiesImpl(refs,data,std::make_index_sequence<std::tuple_size_v<Data>>{});}
    template<class Refs,class Data,std::size_t... I> static void AssignLocalImpl(Refs refs,const Data& data,std::index_sequence<I...>)
    {((std::get<I>(refs)=std::get<I>(data)),...);}
    template<class Refs,class Data> static void AssignLocal(Refs refs,const Data& data)
    {AssignLocalImpl(refs,data,std::make_index_sequence<std::tuple_size_v<Data>>{});}
};
}
