#pragma once
#include "NativeModelBuilder.h"
#include "NativeTrainingBindings.h"
#include "ConfiguredNativeTraining.h"
#include "ProbeEvidenceJson.h"
#include <bit>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace NMSDK::TimeLearner {
// Native-owned, idle stage boundary only. The completed training service is
// observed, never copied or reset. Candidate-search control belongs to the
// PostTune controller; this class restores its source graph and owner policy.
// Explicit retarget/reconfiguration or owner Reset starts a different scope.
class NativePostTuneOwnerBoundary {
public:
    template<class Owner> static std::unique_ptr<NativePostTuneOwnerBoundary> Capture(Owner& owner)
    {
        RequireQuiet(owner);CheckLocalProperties(owner);
        Require(!owner.IsNeedToTrain.GetData(),"posttune_boundary_requires_stopped_training_control");
        auto result=std::unique_ptr<NativePostTuneOwnerBoundary>(new NativePostTuneOwnerBoundary);
        result->capturedOwner=&owner;
        result->graph=NativeLearnerGraphCheckpoint::Capture(owner);
        const auto values=Values(Properties(owner));
        const auto local=Values(LocalControl(owner));
        const auto configuration=Values(Configuration(owner));
        const auto goal=owner.InputPattern.GetData();
        const auto declaration=Declaration(owner);
        const auto controller=ControllerIdentity(owner);
        result->checkScope=[&owner,configuration,goal,declaration,controller] {
            RequireQuiet(owner);CheckLocalProperties(owner);
            Require(Matches(Configuration(owner),configuration),"posttune_boundary_configuration_changed");
            Require(NativeTrainingPatternEqual(owner.InputPattern.GetData(),goal),"posttune_boundary_training_target_changed");
            Require(Declaration(owner)==declaration,"posttune_boundary_declaration_changed");
            Require(ControllerIdentity(owner)==controller,"posttune_boundary_training_controller_changed");
        };
        result->restorePolicy=[&owner,values,local] {
            AssignProperties(Properties(owner),values);
            AssignLocal(LocalControl(owner),local);
            Require(Matches(Properties(owner),values) && Matches(LocalControl(owner),local),
                "posttune_boundary_owner_readback_mismatch");
        };
        result->refresh=[&owner] {
            owner.Neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
            owner.Dataset=owner.template GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        };
        return result;
    }
    template<class Owner> NativeModelBuildResult Restore(Owner& owner) const
    {
        NativeModelBuildResult result;
        try
        {
            Require(&owner==capturedOwner,"posttune_boundary_different_owner");
            checkScope();
            result=graph->Restore(owner);refresh();
            if(result.status!=NativeModelBuildStatus::Adopted)return result;
            restorePolicy();
            // A graph restore does not silently clear/replace the training
            // certificate or references. They retain their original scope.
            return result;
        }
        catch(const std::exception& error)
        {
            result.status=result.modelMayHaveChanged?NativeModelBuildStatus::Failed:NativeModelBuildStatus::Rejected;
            result.reason=error.what();return result;
        }
    }
private:
    NativePostTuneOwnerBoundary()=default;
    RDK::UNet* capturedOwner=nullptr;
    std::unique_ptr<NativeLearnerGraphCheckpoint> graph;
    std::function<void()> checkScope,restorePolicy,refresh;
    static void Require(bool value,const char* reason){if(!value)throw std::logic_error(reason);}
    template<class Owner> static void RequireQuiet(const Owner& owner)
    {
        Require(owner.NativeTrainingOwned,"posttune_boundary_requires_native_owned_model");
        Require(!NativeTrainingIsActive(owner.NativeTrainingControl),"posttune_boundary_training_is_active");
        Require(owner.NativeProbeControl.Status()!=NativeProbeStatus::Queued &&
            owner.NativeProbeControl.Status()!=NativeProbeStatus::Running && !owner.NativeProbeControl.Result(),
            "posttune_boundary_probe_not_consumed");
        Require(!owner.CapApplyPending,"posttune_boundary_pending_physics_edit");
        Require(!owner.PostTuneFreeRunActive && !owner.PostTuneInferenceMidPending,
            "posttune_boundary_legacy_control_is_active");
    }
    template<class Owner> static auto Properties(Owner& o)
    {
        return std::tie(o.DendriteLength,o.NumSynapse,o.TipSynapseResistance,
            o.LTZThreshold,o.FixedLTZThreshold,o.TrainingLTZThreshold,o.CalibratedFixedLTZThreshold,
            o.UseFixedLTZThreshold,o.AutoCalibrateFixedLTZThreshold,o.IsNeedToTrain,
            o.TrainingPhase,o.TrainingFailureReason,o.PostTrainTuneComplete,o.PostTuneResult,
            o.ResistanceMin,o.ResistanceMax,o.ResistanceMinRatio,o.ResistanceMinByInput,
            o.ResistanceMaxByInput,o.ResistanceMinRatioByInput);
    }
    template<class Owner> static auto LocalControl(Owner& o)
    {
        return std::tie(o.OldDendriteLength,o.LoadedTipSynapseResistance,o.HasLoadedTipSynapseResistance,
            o.CanChangeDendLength,o.CapApplyPending,o.NativeEditFailure);
    }
    template<class Owner> static auto Configuration(Owner& o)
    {
        return std::tie(o.ControllerVersion,o.ControllerConfiguration,o.ConfiguredModelInitialized,
            o.CalculateMode,o.StructureBuildMode,o.NumInputDendrite,o.NeuronClassName,o.PulseGeneratorClassName,
            o.NormalizationMode,o.MaxDendriteLength,o.MaxSynapseToMembraneResistanceRatio);
    }
    template<class Owner> static QJsonObject Declaration(const Owner& owner)
    {return owner.NativeModelDeclaration?EncodeNativeModelSpec(*owner.NativeModelDeclaration):QJsonObject{};}
    template<class Owner> static QJsonObject ControllerIdentity(const Owner& owner)
    {
        const auto& c=owner.NativeTrainingControl;
        return {{"state",int(c.State())},{"outcome",int(c.Outcome())},{"reason",QString::fromStdString(c.Reason())},
            {"attempt",QString::number(qulonglong(c.SessionAttempt()))},{"epoch",QString::number(qulonglong(c.ModelEpoch()))},
            {"capture_revision",QString::fromStdString(c.CaptureRevision())},
            {"capture_epoch",QString::number(qulonglong(c.CaptureEpoch()))},
            {"certificate",c.Certificate()?QJsonValue(EncodeTrainingCertificate(*c.Certificate())):QJsonValue(QJsonValue::Null)}};
    }
    template<class P> static void Local(const P& property)
    {Require(!property.IsConnected() && !property.UsesExternalDataSource(),"posttune_boundary_connected_owner_property");}
    template<class Owner> static void CheckLocalProperties(Owner& owner)
    {
        std::apply([](auto&... p){(Local(p),...);},Properties(owner));
        std::apply([](auto&... p){(Local(p),...);},Configuration(owner));Local(owner.InputPattern);
    }
    template<class T> static auto Read(const T& value)
    {if constexpr(requires{value.GetData();})return value.GetData();else return value;}
    template<class Tuple> static auto Values(const Tuple& refs)
    {return std::apply([](const auto&... p){return std::make_tuple(Read(p)...);},refs);}
    template<class T> static bool Equal(const T& a,const T& b){return a==b;}
    static bool Equal(double a,double b){return std::bit_cast<std::uint64_t>(a)==std::bit_cast<std::uint64_t>(b);}
    static bool Equal(const std::vector<double>& a,const std::vector<double>& b)
    {if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(!Equal(a[i],b[i]))return false;return true;}
    template<class Refs,class Data,std::size_t... I> static bool MatchesImpl(const Refs& refs,const Data& data,std::index_sequence<I...>)
    {return (Equal(Read(std::get<I>(refs)),std::get<I>(data)) && ...);}
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
