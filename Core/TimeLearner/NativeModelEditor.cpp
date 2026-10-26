#include "NativeModelEditor.h"
#include "ModelArchive.h"
#include "ModelIdentity.h"
#include "../NPulseChannel.h"
#include "../NPulseSynapse.h"
#include "../NConstGenerator.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <typeinfo>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool value,const char* reason) { if(!value)throw std::runtime_error(reason); }
bool Positive(double value) { return std::isfinite(value) && value>0.; }
void StaticParameters(std::initializer_list<const UIProperty*> parameters)
{
    for(const auto parameter:parameters)if(auto input=dynamic_cast<const UIPropertyInput*>(parameter))
        Require(!input->IsConnected() && !input->UsesExternalDataSource(),"unsupported_connected_integration_parameter");
}
std::string MembranePath(bool branch,std::size_t input,int position)
{ return "Dendrite"+std::to_string(branch?1u:input+1)+"_"+std::to_string(position); }
void Collect(UEPtr<UContainer> node,std::vector<UEPtr<UContainer>>& nodes)
{
    Require(bool(node),"missing_edit_node");nodes.push_back(node);
    std::vector<NameT> names;node->GetComponentsList(names);
    for(const auto& name:names)Collect(node->GetComponent(name,true),nodes);
}
bool Inside(UContainer* node,UContainer* root)
{ for(auto current=node;current;current=current->GetOwner().Get())if(current==root)return true;return false; }
double SecretionBound(NPulseSynapse& synapse,double sourceAmplitude)
{
    Require(typeid(synapse)==typeid(NPulseSynapse),"unsupported_synapse_integrator");
    StaticParameters({&synapse.Resistance,&synapse.SecretionTC,&synapse.DissociationTC,&synapse.Weight,
        &synapse.PulseAmplitude,&synapse.UsePulseSignal,&synapse.UsePresynapticInhibition,&synapse.InhibitionCoeff});
    Require(!synapse.WeightInput.IsConnected() && std::isfinite(synapse.Weight.GetData()) && synapse.Weight.GetData()>=0.,
        "unsupported_dynamic_or_negative_synapse_weight");
    Require(Positive(synapse.Resistance.GetData()) && Positive(synapse.PulseAmplitude.GetData()) &&
        Positive(synapse.SecretionTC.GetData()) && Positive(synapse.DissociationTC.GetData()) && synapse.GetTimeStep()>0,
        "invalid_synapse_integration_parameters");
    const double alpha=1./(synapse.SecretionTC.GetData()*synapse.GetTimeStep());
    const double beta=1./(synapse.DissociationTC.GetData()*synapse.GetTimeStep());
    Require(Positive(alpha) && Positive(beta) && alpha<2. && beta<2.,"unstable_synapse_discretization");
    if(!synapse.UsePulseSignal.GetData() && synapse.Input.IsConnected())
    {
        const auto sources=synapse.Input.GetDataSources();
        Require(sources.size()==1 && sources.front() && sources.front()->GetName()=="Output","unsupported_analog_synapse_input");
        auto owner=sources.front()->GetOwner();
        if(auto generator=dynamic_cast<NPulseGeneratorTransit*>(owner))
        {
            Require(!generator->UseTransitSignal.GetData() && !generator->UsePatternOutput.GetData() &&
                generator->FrequencyDeviation.GetData()==0.,"unbounded_analog_stimulus_source");
            sourceAmplitude=generator->Amplitude.GetData();
        }
        else if(auto constant=dynamic_cast<NConstGenerator*>(owner))
        {
            Require(typeid(*constant)==typeid(NConstGenerator),"unsupported_constant_stimulus_equation");
            sourceAmplitude=std::max(0.,constant->Amplitude.GetData());
        }
        else throw std::runtime_error("unsupported_analog_stimulus_source");
    }
    const double drive=synapse.UsePulseSignal.GetData()?1.:sourceAmplitude/synapse.PulseAmplitude.GetData();
    double maximum=drive;
    if(alpha>1. || beta>1.)
    {
        const double contraction=std::max(std::fabs(1.-alpha),std::fabs(1.-beta));
        maximum=alpha*drive/(1.-contraction);
    }
    Require(std::isfinite(maximum) && maximum>=0.,"nonfinite_secretion_bound");
    double normalized=maximum;
    if(synapse.UsePresynapticInhibition.GetData())
    {
        const double k=synapse.InhibitionCoeff.GetData();
        Require(std::isfinite(k) && k>=0.,"invalid_presynaptic_inhibition");
        if(k>0. && maximum>0.)normalized=std::max(1.,(k>2.?.4:.2*k)*maximum);
    }
    const double conductance=normalized/synapse.Resistance.GetData()*synapse.Weight.GetData();
    Require(std::isfinite(conductance) && conductance>=0.,"nonfinite_conductance_bound");return conductance;
}
struct Target { int count=1;double resistance=0.; };
struct Plan {
    std::map<std::string,Target> excitation,inhibition;
    std::vector<int> cableLengths;
};
Plan MakePlan(const NativeProbeBindings& b,const std::vector<InputParameters>& values,const NativeEditSpec& s)
{
    Require(values.size()==std::size_t(b.inputCount) && b.inputCount>0 && s.maximumCableLength>0,"invalid_edit_dimensions");
    Require(s.normalization==Normalization::SynapseCount || s.normalization==Normalization::TipResistance,"invalid_edit_normalization");
    Plan p;p.cableLengths.assign(b.branch?1:values.size(),1);
    for(std::size_t i=0;i<values.size();++i)
    {
        const auto& value=values[i];
        Require(value.length>=1 && value.length<=s.maximumCableLength && value.synapses>=1 && value.synapses<=128 &&
            Positive(value.resistance) && (s.normalization!=Normalization::TipResistance || value.synapses==1),"invalid_edit_parameters");
        Require(!b.branch || !i || values[i-1].length>=value.length,"invalid_branch_attachment_order");
        const auto path=MembranePath(b.branch,i,value.length);
        auto [existing,added]=p.excitation.emplace(path,Target{value.synapses,value.resistance});
        Require(added || (existing->second.count==value.synapses && existing->second.resistance==value.resistance),"incompatible_shared_excitation_bank");
        auto& length=p.cableLengths[b.branch?0:i];length=std::max(length,value.length);
    }
    for(const auto& rule:s.inhibition)
    {
        Require(rule.inputIndex<values.size() && rule.offset>0 && Positive(rule.resistance),"invalid_inhibition_edit");
        const long long segment=static_cast<long long>(values[rule.inputIndex].length)+rule.offset;
        Require(segment<=s.maximumCableLength,"missing_inhibition_headroom");
        const auto path=MembranePath(b.branch,rule.inputIndex,int(segment));
        auto [existing,added]=p.inhibition.emplace(path,Target{1,rule.resistance});
        Require(added || existing->second.resistance==rule.resistance,"incompatible_shared_inhibition_bank");
        auto& length=p.cableLengths[b.branch?0:rule.inputIndex];length=std::max(length,int(segment));
    }
    return p;
}
std::pair<std::vector<double>,std::vector<double>> ResistanceIntervals(UEPtr<NPulseNeuron> neuron,bool branch,
    const std::vector<InputParameters>& values,const NativeEditSpec& spec,double amplitude)
{
    std::pair<std::vector<double>,std::vector<double>> result;
    if(spec.normalization!=Normalization::TipResistance)return result;
    Require(std::isfinite(spec.maximumResistanceRatio) && spec.maximumResistanceRatio>=1. &&
        std::isfinite(spec.configuredMinimumResistance) && spec.configuredMinimumResistance>=0.,"invalid_native_resistance_policy");
    std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(std::size_t i=0;i+1<values.size();++i)
    {
        auto membrane=neuron->GetComponentL<NPulseMembrane>(MembranePath(branch,i,values[i].length),true);
        Require(bool(membrane) && membrane->GetNumPosChannels()>0,"missing_resistance_target_channel");
        auto target=dynamic_cast<NPulseChannel*>(membrane->GetPosChannel(0));
        Require(target && Positive(target->RestingResistance.GetData()),"invalid_resistance_target_channel");
        auto tip=membrane->GetComponentL<NPulseSynapse>("ExcSynapse1",true);Require(bool(tip),"unsupported_resistance_target_synapse");
        const double rm=target->RestingResistance.GetData(),maximum=rm*spec.maximumResistanceRatio;
        double minimum=std::max(rm/spec.maximumResistanceRatio,spec.configuredMinimumResistance);
        bool foundTarget=false;
        for(const auto& node:nodes)if(auto channel=dynamic_cast<NPulseChannel*>(node.Get()))
        {
            double fixed=0.,variable=0.;
            for(std::size_t k=0;k<channel->SynapticInputs->size();++k)
            {
                auto synapse=dynamic_cast<NPulseSynapse*>(channel->SynapticInputs.GetItem(int(k)));
                Require(bool(synapse),"unsupported_resistance_conductance_source");
                const double g=SecretionBound(*synapse,amplitude);
                if(synapse==tip.Get())variable+=g*tip->Resistance.GetData();else fixed+=g;
            }
            if(variable==0.)continue;foundTarget=true;
            if(channel->UseAverageSynapsis.GetData() && !channel->SynapticInputs->empty())
            {fixed/=channel->SynapticInputs->size();variable/=channel->SynapticInputs->size();}
            for(double r:{channel->Resistance.GetData(),channel->RestingResistance.GetData(),channel->FBResistance.GetData()})
            {
                const double q=channel->Capacity.GetData()*r*channel->GetTimeStep();
                const double available=(std::sqrt(q)-1.)/r-fixed;
                Require(Positive(available),"no_finite_native_no_crossing_interval");
                minimum=std::max(minimum,variable/available);
            }
        }
        Require(foundTarget && Positive(minimum) && Positive(maximum) && minimum<=maximum,"no_admissible_native_resistance_interval");
        Require(values[i].resistance>=minimum && values[i].resistance<=maximum,"candidate_outside_effective_resistance_interval");
        result.first.push_back(minimum);result.second.push_back(maximum);
    }
    return result;
}
using ChannelPhysics=std::map<std::string,std::array<double,6>>;
ChannelPhysics Physics(UEPtr<NPulseNeuron> neuron)
{
    ChannelPhysics result;std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(const auto& node:nodes)if(auto channel=dynamic_cast<NPulseChannel*>(node.Get()))
        result.emplace(node->GetLongName(neuron.Get()),std::array<double,6>{channel->Capacity.GetData(),channel->Resistance.GetData(),
            channel->RestingResistance.GetData(),channel->FBResistance.GetData(),double(channel->GetTimeStep()),channel->Type.GetData()});
    return result;
}
void CheckPreserved(const ChannelPhysics& before,UEPtr<NPulseNeuron> after)
{
    const auto current=Physics(after);
    for(const auto& [path,values]:before)
        if(auto found=current.find(path);found!=current.end())Require(found->second==values,"existing_channel_physics_changed");
}
void UniformTree(UEPtr<UContainer> node,unsigned frequency)
{
    std::vector<UEPtr<UContainer>> nodes;Collect(node,nodes);
    for(const auto& part:nodes)Require(part->GetTimeStep()==frequency && part->IsReady() && part->IsInit(),"edited_tree_not_uniform_ready_initialized");
}
// Only owned stimulus inputs are replaced. Other model-internal connections
// remain native Build's responsibility; source fan-out to analyzers is retained.
void DetachStimulus(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,UNet* owner)
{
    std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(const auto& part:nodes)if(auto synapse=dynamic_cast<NPulseSynapseCommon*>(part.Get()))
    {
        const auto inputs=synapse->Input.GetDataSources();
        if(std::find(inputs.begin(),inputs.end(),&source.Output)==inputs.end())continue;
        if(owner)owner->BreakLink(source.GetLongName(owner),"Output",synapse->GetLongName(owner),"Input");
        synapse->Input.DetachFrom();
    }
}
void ConnectStimulus(NPulseSynapseCommon& synapse,NPulseGeneratorTransit& source,UNet* owner)
{
    Require(!synapse.Input.IsConnected(),"edit_would_replace_unowned_input");
    if(owner)owner->CreateLink(source.GetLongName(owner),"Output",synapse.GetLongName(owner),"Input",-1,true);
    else
    {
        int slot=0;source.Connect(&synapse,"Output","Input",slot,true);
    }
    const auto inputs=synapse.Input.GetDataSources();
    Require(inputs.size()==1 && inputs.front()==&source.Output,"edited_stimulus_binding_mismatch");
}
void Transform(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,const Plan& p,UNet* owner)
{
    const unsigned frequency=neuron->GetTimeStep();const auto physics=Physics(neuron);
    // Uniform physical overrides belong to images. Rebuilding with a live
    // parent override would overwrite learned/heterogeneous child physics.
    Require(!neuron->UseElementDefaults.GetData(),"native_edit_requires_storage_owned_physics");
    DetachStimulus(neuron,source,owner);
    const int mode=neuron->StructureBuildMode.GetData();
    if(mode==2)
    {
        if(neuron->NumDendriteMembranePartsVec.GetData()!=p.cableLengths)
        {neuron->NumDendriteMembranePartsVec=p.cableLengths;neuron->Build();}
    }
    else if(mode==1)
    {
        // Scalar mode has one common allocation; inputs remain attachment
        // positions. Preserve its existing extra cable and grow it natively.
        const int maximum=*std::max_element(p.cableLengths.begin(),p.cableLengths.end());
        if(maximum>neuron->NumDendriteMembraneParts.GetData())
        {neuron->NumDendriteMembraneParts=maximum;neuron->Build();}
    }
    else Require(mode==0,"unsupported_neuron_structure_mode");
    std::vector<NameT> names;neuron->GetComponentsList(names);
    for(const auto& name:names)
    {
        if(name.rfind("Dendrite",0)!=0)continue;
        auto membrane=neuron->GetComponentL<NPulseMembrane>(name,true);Require(bool(membrane),"unsupported_dendrite_component");
        const auto exc=p.excitation.find(name),inh=p.inhibition.find(name);
        // Unused branches have no driven excitation bank. Native role images
        // recreate a future bank; no old learned R is used as its prototype.
        const int count=exc==p.excitation.end()?0:exc->second.count;
        if(membrane->NumExcitatorySynapses.GetData()!=count || (inh!=p.inhibition.end() && membrane->NumInhibitorySynapses.GetData()<1))
        {
            membrane->NumExcitatorySynapses=count;
            if(inh!=p.inhibition.end())membrane->NumInhibitorySynapses=1;
            membrane->Build();
        }
        if(exc!=p.excitation.end())
        {
            auto first=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse1",true);Require(bool(first),"missing_edited_excitation_bank");
            first->Resistance=exc->second.resistance;
            for(int k=1;k<=count;++k)
            {
                auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
                Require(bool(synapse),"missing_edited_excitation_synapse");ConnectStimulus(*synapse,source,owner);
            }
        }
        if(inh!=p.inhibition.end())
        {
            auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("InhSynapse1",true);Require(bool(synapse),"missing_edited_inhibition_bank");
            synapse->Resistance=inh->second.resistance;ConnectStimulus(*synapse,source,owner);
        }
    }
    for(const auto& [name,target]:p.excitation)Require(bool(neuron->GetComponentL<NPulseMembrane>(name,true)),"attachment_outside_explicit_graph");
    for(const auto& [name,target]:p.inhibition)Require(bool(neuron->GetComponentL<NPulseMembrane>(name,true)),"inhibition_outside_explicit_graph");
    neuron->Init();neuron->Reset();neuron->InvalidateActiveComponentsCache();
    UniformTree(neuron,frequency);CheckPreserved(physics,neuron);
}
void CheckRemovedListeners(UEPtr<NPulseNeuron> original,UEPtr<NPulseNeuron> candidate)
{
    std::vector<UEPtr<UContainer>> nodes;Collect(original,nodes);
    for(const auto& node:nodes)
    {
        const auto path=node->GetLongName(original.Get());
        if(node==original || candidate->GetComponentL(path,true))continue;
        for(const auto& [name,entry]:node->GetPropertiesList())
            if(auto output=dynamic_cast<UIPropertyOutput*>(entry.Property.Get()))
                for(std::size_t i=0;i<output->GetNumConnectors();++i)
                {
                    const auto input=output->GetConnectorProperty(int(i));
                    Require(!input || Inside(dynamic_cast<UContainer*>(input->GetOwner()),original.Get()),"edit_would_remove_external_observation_source");
                }
    }
}
struct Lease {
    UEPtr<UContainer> object;
    ~Lease(){if(object){if(auto net=dynamic_pointer_cast<UNet>(object))net->BreakLinks();object->GetStorage()->ReturnObject(object);}}
};
}

NativeIntegrationAssessment AssessNativeIntegration(RDK::UEPtr<NPulseNeuron> neuron,double sourceAmplitude)
{
    NativeIntegrationAssessment result;
    try
    {
        Require(bool(neuron) && Positive(sourceAmplitude),"invalid_native_integration_context");
        std::vector<RDK::UEPtr<RDK::UContainer>> nodes;Collect(neuron,nodes);
        for(const auto& node:nodes)if(auto channel=dynamic_cast<NPulseChannelCommon*>(node.Get()))
        {
            Require(typeid(*channel)==typeid(NPulseChannel),"unsupported_channel_integrator");
            auto explicitChannel=static_cast<NPulseChannel*>(channel);
            StaticParameters({&explicitChannel->Capacity,&explicitChannel->Resistance,&explicitChannel->RestingResistance,
                &explicitChannel->FBResistance,&channel->UseAverageSynapsis});
            double conductance=0.;
            for(std::size_t k=0;k<channel->SynapticInputs->size();++k)
            {
                auto synapse=dynamic_cast<NPulseSynapse*>(channel->SynapticInputs.GetItem(int(k)));
                Require(synapse && channel->SynapticInputs.GetItemOutputName(int(k))=="Output","unsupported_channel_conductance_source");
                conductance+=SecretionBound(*synapse,sourceAmplitude);
            }
            if(channel->UseAverageSynapsis.GetData() && !channel->SynapticInputs->empty())conductance/=channel->SynapticInputs->size();
            const double capacity=explicitChannel->Capacity.GetData(),frequency=explicitChannel->GetTimeStep();
            Require(Positive(capacity) && Positive(frequency) && std::isfinite(conductance),"invalid_channel_integration_parameters");
            double maximum=0.;
            for(double resistance:{explicitChannel->Resistance.GetData(),explicitChannel->RestingResistance.GetData(),explicitChannel->FBResistance.GetData()})
            {
                Require(Positive(resistance),"invalid_channel_resistance");
                const double gain=(1.+conductance*resistance)*(1.+conductance*resistance)/(capacity*resistance*frequency);
                Require(std::isfinite(gain) && gain<2.,"channel_step_not_certified_strictly_stable");maximum=std::max(maximum,gain);
            }
            result.channels.push_back({node->GetLongName(neuron.Get()),conductance,maximum});
        }
        Require(!result.channels.empty(),"missing_native_channels");result.valid=true;
    }
    catch(const std::exception& error){result.reason=error.what();}
    return result;
}

std::vector<InputParameters> ReadNativeInputParameters(const NativeProbeBindings& b,const std::vector<int>& attachments)
{
    Require(bool(b.neuron) && attachments.size()==std::size_t(b.inputCount),"invalid_native_parameter_readback");
    std::vector<InputParameters> result;
    for(std::size_t i=0;i<attachments.size();++i)
    {
        auto membrane=b.neuron->GetComponentL<NPulseMembrane>(MembranePath(b.branch,i,attachments[i]),true);
        Require(bool(membrane),"missing_native_input_membrane");
        auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse1",true);
        Require(bool(synapse) && membrane->NumExcitatorySynapses.GetData()>=1 && Positive(synapse->Resistance.GetData()),"missing_native_input_bank");
        result.push_back({attachments[i],membrane->NumExcitatorySynapses.GetData(),synapse->Resistance.GetData()});
    }
    return result;
}

NativeEditResult ApplyNativeInputParameters(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec)
{
    NativeEditResult result;
    try
    {
        Require(executor.AtEditBoundary(),"native_edit_requires_consumed_probe_boundary");
        Require(b.owner && b.neuron && b.dataset && b.owner->GetComponentL("Neuron",true).Get()==b.neuron.Get(),"invalid_native_edit_binding");
        Require(!b.legacyPhysicsPending && !b.postTuneActive,"conflicting_native_edit_controller");
        auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_edit_stimulus_source");
        ReadNativeInputParameters(b,attachments);
        const auto plan=MakePlan(b,requested,spec);const auto frequency=b.neuron->GetTimeStep();
        UniformTree(b.neuron,frequency);
        const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
        auto before=NmsdkTools::IdentifyModel(b.neuron,ports);Require(before["valid"].toBool(),"invalid_live_edit_identity");
        auto preview=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ClonePhysicalGraphForEdit(b.neuron,ports));Lease lease{preview};
        Require(bool(preview),"failed_edit_preview");
        Transform(preview,*source,plan,nullptr);CheckRemovedListeners(b.neuron,preview);
        const auto stability=AssessNativeIntegration(preview,source->Amplitude.GetData());
        Require(stability.valid,stability.reason.c_str());
        const auto intervals=ResistanceIntervals(preview,b.branch,requested,spec,source->Amplitude.GetData());
        auto predicted=NmsdkTools::IdentifyModel(preview,ports);Require(predicted["valid"].toBool(),"invalid_preview_edit_identity");
        // Disposal must disconnect the speculative source consumers before the
        // live edit. No physical tick occurs on the preview or between edits.
        preview->BreakLinks();preview->GetStorage()->ReturnObject(preview);lease.object=0;
        Require(NmsdkTools::IdentifyModel(b.neuron,ports)==before,"live_graph_changed_during_preview");
        result.modelMayHaveChanged=true;
        Transform(b.neuron,*source,plan,b.owner);
        const auto after=NmsdkTools::VerifyModelIdentity(predicted,b.neuron,ports);
        Require(after["valid"].toBool() && after["match"].toBool(),"live_edit_differs_from_native_preview");
        std::vector<int> positions;for(const auto& input:requested)positions.push_back(input.length);
        result.effective=ReadNativeInputParameters(b,positions);Require(result.effective==requested,"native_edit_readback_mismatch");
        result.minimumResistance=intervals.first;result.maximumResistance=intervals.second;
        result.status=NativeEditStatus::Applied;
    }
    catch(const std::exception& error)
    {result.reason=error.what();result.status=result.modelMayHaveChanged?NativeEditStatus::Failed:NativeEditStatus::Rejected;}
    catch(const RDK::UException& error)
    {result.reason=error.what();result.status=result.modelMayHaveChanged?NativeEditStatus::Failed:NativeEditStatus::Rejected;}
    return result;
}
const char* NativeEditStatusName(NativeEditStatus status)
{switch(status){case NativeEditStatus::Applied:return "applied";case NativeEditStatus::Rejected:return "rejected";case NativeEditStatus::Failed:return "failed";}return "unknown";}

NativeEditSpec MakeNativeEditSpec(Normalization normalization,int maximumLength,const std::vector<InputParameters>& requested,
    bool nextSegment,bool segmentEI,int delta,double alpha,double maximumResistanceRatio,double configuredMinimumResistance)
{
    NativeEditSpec result;result.normalization=normalization;result.maximumCableLength=maximumLength;
    result.maximumResistanceRatio=maximumResistanceRatio;result.configuredMinimumResistance=configuredMinimumResistance;
    Require(!segmentEI || (delta>=1 && Positive(alpha)),"invalid_segment_inhibition_policy");
    for(std::size_t i=0;i<requested.size();++i)
    {
        if(segmentEI)result.inhibition.push_back({i,delta,requested[i].resistance/alpha});
        if(nextSegment && !(segmentEI && delta==1))result.inhibition.push_back({i,1,requested[i].resistance});
    }
    return result;
}
}
