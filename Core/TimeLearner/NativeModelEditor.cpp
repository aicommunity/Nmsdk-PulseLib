#include "NativeModelEditor.h"
#include "NativeCapacitanceEdit.h"
#include "NativeModelBuilder.h"
#include "ConfiguredNativeTraining.h"
#include "NativeOperationChecks.h"
#include "ModelArchive.h"
#include "ModelIdentity.h"
#include "../NPulseChannel.h"
#include "../NPulseSynapse.h"
#include "../NConstGenerator.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <cstdint>

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
    std::set<std::string> ownedExcitation,ownedInhibition,releasedExcitation;
    std::vector<int> cableLengths;
};
Plan MakePlan(const NativeProbeBindings& b,const std::vector<InputParameters>& previous,
    const std::vector<InputParameters>& values,const NativeEditSpec& s)
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
        p.ownedExcitation.insert(path);
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
        p.ownedInhibition.insert(path);
        auto [existing,added]=p.inhibition.emplace(path,Target{1,rule.resistance});
        Require(added || existing->second.resistance==rule.resistance,"incompatible_shared_inhibition_bank");
        auto& length=p.cableLengths[b.branch?0:rule.inputIndex];length=std::max(length,int(segment));
    }
    for(std::size_t i=0;i<previous.size();++i)
    {
        const auto path=MembranePath(b.branch,i,previous[i].length);p.ownedExcitation.insert(path);
        if(!p.excitation.count(path))p.releasedExcitation.insert(path);
    }
    for(const auto& rule:s.inhibition)
    {
        const long long segment=static_cast<long long>(previous.at(rule.inputIndex).length)+rule.offset;
        Require(segment<=std::numeric_limits<int>::max(),"invalid_previous_inhibition_position");
        p.ownedInhibition.insert(MembranePath(b.branch,rule.inputIndex,int(segment)));
    }
    if(b.neuron->StructureBuildMode.GetData()==2)
    {
        const auto allocation=b.neuron->NumDendriteMembranePartsVec.GetData();
        Require(allocation.size()==std::size_t(b.neuron->NumSomaMembraneParts.GetData()) && allocation.size()>=p.cableLengths.size(),
            "inconsistent_native_cable_allocation");
        if(s.preserveAllocatedCable)
        {
            const auto required=p.cableLengths;p.cableLengths=allocation;
            for(std::size_t i=0;i<required.size();++i)p.cableLengths[i]=std::max(required[i],allocation[i]);
        }
        else Require(allocation.size()==p.cableLengths.size(),"unowned_soma_allocation_would_change");
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
using ParameterMap=std::map<QString,QJsonObject>;
ParameterMap Parameters(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source)
{
    const auto identity=NmsdkTools::IdentifyModel(neuron,{{&source.Output,"burst"}});
    Require(identity["valid"].toBool(),"invalid_parameter_preservation_identity");
    ParameterMap result;
    for(const auto& entry:identity["model"].toObject()["nodes"].toArray())
    {const auto node=entry.toObject();result.emplace(node["path"].toString(),node);}
    return result;
}
void CheckPreserved(const ParameterMap& before,UEPtr<NPulseNeuron> after,NPulseGeneratorTransit& source,const Plan& plan)
{
    const auto current=Parameters(after,source);std::set<QString> adjustableR;
    for(const auto& [path,target]:plan.excitation)
        for(int k=1;k<=target.count;++k)
            adjustableR.insert("$."+QString::fromStdString(path)+".ExcSynapse"+QString::number(k));
    for(const auto& [path,target]:plan.inhibition)adjustableR.insert("$."+QString::fromStdString(path)+".InhSynapse1");
    for(const auto& [path,node]:before)if(auto found=current.find(path);found!=current.end())
    {
        Require(node["class"]==found->second["class"] && node["frequency_hz"]==found->second["frequency_hz"],"existing_component_class_or_frequency_changed");
        auto expected=node["parameters"].toObject(),actual=found->second["parameters"].toObject();
        const auto remove=[&](const char* name){expected.remove(name);actual.remove(name);};
        if(path=="$"){remove("NumDendriteMembraneParts");remove("NumDendriteMembranePartsVec");}
        if(path.startsWith("$.Dendrite") && path.count('.')==1)
        {
            const auto membrane=path.mid(2).toStdString();
            if(plan.excitation.count(membrane) || plan.releasedExcitation.count(membrane))remove("NumExcitatorySynapses");
            if(plan.inhibition.count(membrane))remove("NumInhibitorySynapses");
        }
        if(adjustableR.count(path))remove("Resistance");
        if(expected!=actual)
        {
            for(auto it=expected.begin();it!=expected.end();++it)if(actual[it.key()]!=it.value())
                throw std::runtime_error("existing_parameter_changed:"+path.toStdString()+":"+it.key().toStdString());
            throw std::runtime_error("existing_parameter_set_changed:"+path.toStdString());
        }
    }
}
void UniformTree(UEPtr<UContainer> node,unsigned frequency)
{
    std::vector<UEPtr<UContainer>> nodes;Collect(node,nodes);
    for(const auto& part:nodes)Require(part->GetTimeStep()==frequency && part->IsReady() && part->IsInit(),"edited_tree_not_uniform_ready_initialized");
}
// Only owned stimulus inputs are replaced. Other model-internal connections
// remain native Build's responsibility; source fan-out to analyzers is retained.
void DetachStimulus(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,UNet* owner,const Plan& plan)
{
    std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(const auto& part:nodes)if(auto synapse=dynamic_cast<NPulseSynapseCommon*>(part.Get()))
    {
        const auto membrane=synapse->GetOwner();Require(bool(membrane),"unowned_model_synapse");
        const auto path=membrane->GetLongName(neuron.Get());
        const bool owned=(synapse->GetName().rfind("ExcSynapse",0)==0 && plan.ownedExcitation.count(path)) ||
            (synapse->GetName().rfind("InhSynapse",0)==0 && plan.ownedInhibition.count(path));
        if(!owned)continue;
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
    const unsigned frequency=neuron->GetTimeStep();const auto parameters=Parameters(neuron,source);
    // Uniform physical overrides belong to images. Rebuilding with a live
    // parent override would overwrite learned/heterogeneous child physics.
    Require(!neuron->UseElementDefaults.GetData(),"native_edit_requires_storage_owned_physics");
    DetachStimulus(neuron,source,owner,p);
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
        // Release former learner input banks. Unowned banks and their internal
        // inputs belong to the supplied model, especially in explicit mode 0.
        const int count=exc!=p.excitation.end()?exc->second.count:
            (p.releasedExcitation.count(name)?0:membrane->NumExcitatorySynapses.GetData());
        if(membrane->NumExcitatorySynapses.GetData()!=count || (inh!=p.inhibition.end() && membrane->NumInhibitorySynapses.GetData()<1))
        {
            membrane->NumExcitatorySynapses=count;
            if(inh!=p.inhibition.end())membrane->NumInhibitorySynapses=1;
            membrane->Build();
        }
        if(exc!=p.excitation.end())
        {
            for(int k=1;k<=count;++k)
            {
                auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
                Require(bool(synapse),"missing_edited_excitation_synapse");
                // Learned Rs belongs to this input bank, including instances
                // just created from its storage image. Other physics is kept.
                synapse->Resistance=exc->second.resistance;ConnectStimulus(*synapse,source,owner);
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
    NmsdkTools::InitializeAndResetNative(neuron);neuron->InvalidateActiveComponentsCache();
    UniformTree(neuron,frequency);CheckPreserved(parameters,neuron,source,p);
}
void CheckRemovedListeners(UEPtr<NPulseNeuron> original,UEPtr<NPulseNeuron> candidate)
{
    std::set<const UIProperty*> removedOutputs;
    std::vector<UEPtr<UContainer>> nodes;Collect(original,nodes);
    for(const auto& node:nodes)
    {
        const auto path=node->GetLongName(original.Get());
        if(node==original || candidate->GetComponentL(path,true))continue;
        for(const auto& [name,entry]:node->GetPropertiesList())
            if(auto output=dynamic_cast<UIPropertyOutput*>(entry.Property.Get()))
            {
                removedOutputs.insert(output);
                // Native UItem::Connect and typed property bindings do not
                // necessarily populate the same reverse-connection cache.
                if(auto item=dynamic_cast<UItem*>(node.Get()))
                    for(int i=0;i<item->GetNumAConnectors(name);++i)
                    {
                        const auto consumer=item->GetAConnectorByIndex(name,i);
                        Require(!consumer || Inside(consumer.Get(),original.Get()),"edit_would_remove_external_observation_source");
                    }
                for(std::size_t i=0;i<output->GetNumConnectors();++i)
                {
                    const auto input=output->GetConnectorProperty(int(i));
                    Require(!input || Inside(dynamic_cast<UContainer*>(input->GetOwner()),original.Get()),"edit_would_remove_external_observation_source");
                }
            }
    }
    if(removedOutputs.empty())return;
    UEPtr<UContainer> scope(original.Get());while(scope->GetOwner())scope=scope->GetOwner();
    nodes.clear();Collect(scope,nodes);
    for(const auto& node:nodes)if(!Inside(node.Get(),original.Get()))
        for(const auto& [name,entry]:node->GetPropertiesList())if(auto input=dynamic_cast<UIPropertyInput*>(entry.Property.Get()))
        {
            if(!input->IsConnected() && !input->UsesExternalDataSource())continue;
            Require(input->CanInspectDataSources(),"uninspectable_external_observation_binding");
            for(const auto source:input->GetDataSources())
                Require(!removedOutputs.count(source),"edit_would_remove_external_observation_source");
        }
}
struct CapacitancePlan {
    std::map<std::string,double> targets,previous;
    QJsonObject applied;
};
QJsonObject SignedCapacitance(QJsonObject value)
{value.remove("sha256");value["sha256"]=NmsdkTools::CanonicalModelDataSha256(value);return value;}
bool Digest(const QJsonValue& value)
{
    if(!value.isString() || value.toString().size()!=64)return false;
    for(const auto c:value.toString())if(!(c>='0' && c<='9') && !(c>='a' && c<='f'))return false;
    return true;
}
double ParameterNumber(const QJsonObject& node,const QString& name)
{
    const auto parameter=node["parameters"].toObject()[name].toObject();
    const auto data=parameter["data"].toObject();const auto text=data["value"].toString();
    Require(parameter["binding"]==QJsonValue(false) && data["type"]==QJsonValue("binary64") && text.size()==16,
        "invalid_capacitance_numeric_witness");
    for(const auto c:text)Require((c>='0' && c<='9') || (c>='a' && c<='f'),"invalid_capacitance_numeric_witness");
    bool ok=false;const std::uint64_t bits=text.toULongLong(&ok,16);double result;
    std::memcpy(&result,&bits,sizeof(result));
    Require(ok && std::isfinite(result),"invalid_capacitance_numeric_witness");return result;
}
QJsonObject ChannelPhysics(const QJsonObject& node)
{
    QJsonObject result;const auto parameters=node["parameters"].toObject();
    for(const auto* name:{"Resistance","FBResistance","RestingResistance","Type"})
    {
        ParameterNumber(node,name);result[name]=parameters[name].toObject()["data"];
    }
    for(const auto* name:{"UseAveragePotential","UseAverageSynapsis"})
    {
        const auto parameter=parameters[name].toObject(),data=parameter["data"].toObject();
        Require(parameter["binding"]==QJsonValue(false) && data["type"]==QJsonValue("boolean") && data["value"].isBool(),
            "invalid_capacitance_boolean_witness");result[name]=data;
    }
    return result;
}
std::map<QString,QJsonObject> IdentityNodes(const QJsonObject& model)
{
    Require((model["schema_version"]==QJsonValue(1) || model["schema_version"]==QJsonValue(2)) &&
        model["nodes"].isArray(),"invalid_capacitance_capture_model");
    std::map<QString,QJsonObject> nodes;
    for(const auto& entry:model["nodes"].toArray())
    {
        Require(entry.isObject(),"invalid_capacitance_capture_node");const auto node=entry.toObject();
        Require(node["path"].isString() && nodes.emplace(node["path"].toString(),node).second,"invalid_capacitance_capture_node");
    }
    Require(nodes.count("$")>0,"missing_capacitance_capture_root");return nodes;
}
std::map<QString,QJsonObject> ImageRoots(const QJsonArray& definitions)
{
    std::map<QString,QJsonObject> result;
    for(const auto& entry:definitions)
    {
        const auto definition=entry.toObject();const auto nodes=IdentityNodes(definition["snapshot"].toObject()["identity"].toObject()["model"].toObject());
        Require(result.emplace(definition["name"].toString(),nodes.at("$")).second,"duplicate_capacitance_image");
    }
    return result;
}
void CheckCapacitanceDocument(const QJsonObject& baseline)
{
    const bool bound=baseline["schema_version"]==QJsonValue(2);
    std::set<QString> keys{"kind","schema_version","model_revision","frequency_hz","channels","sha256"};
    if(bound)for(const auto* key:{"model_spec","capture_model","storage_images","storage_images_sha256",
        "channel_classes","channel_physics","channel_origins"})keys.insert(key);
    const auto actual=baseline.keys();Require(std::set<QString>(actual.begin(),actual.end())==keys,"invalid_capacitance_baseline_schema");
    Require(baseline["kind"]==QJsonValue("timelearner_capacitance_baseline") &&
        (bound || baseline["schema_version"]==QJsonValue(1)) && SignedCapacitance(baseline)==baseline &&
        Digest(baseline["model_revision"]) && baseline["frequency_hz"].isDouble() && Positive(baseline["frequency_hz"].toDouble()) &&
        baseline["channels"].isObject() && !baseline["channels"].toObject().isEmpty(),"invalid_capacitance_baseline");
    const auto channels=baseline["channels"].toObject();
    for(auto it=channels.begin();it!=channels.end();++it)
        Require(!it.key().isEmpty() && it.value().isDouble() && Positive(it.value().toDouble()),"invalid_capacitance_baseline_channel");
    if(!bound)return;
    Require(baseline["model_spec"].isObject() && baseline["capture_model"].isObject() && baseline["storage_images"].isArray() &&
        baseline["channel_classes"].isObject() && baseline["channel_physics"].isObject() && baseline["channel_origins"].isObject(),
        "invalid_bound_capacitance_baseline");
    const auto declaration=DecodeNativeModelSpec(baseline["model_spec"].toObject());
    Require(baseline["frequency_hz"]==QJsonValue(double(declaration.frequencyHz)) &&
        baseline["model_revision"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(baseline["capture_model"])),
        "capacitance_capture_identity_mismatch");
    const auto captured=IdentityNodes(baseline["capture_model"].toObject());
    Require(captured.at("$")["class"]==QJsonValue(QString::fromStdString(declaration.neuronClass)),"capacitance_capture_class_mismatch");
    const auto definitions=baseline["storage_images"].toArray();
    Require(baseline["storage_images_sha256"]==QJsonValue(NmsdkTools::ModelImageDefinitionsFingerprint(definitions)),
        "capacitance_storage_images_digest_mismatch");
    const auto roots=ImageRoots(definitions);const auto classes=baseline["channel_classes"].toObject();
    const auto physics=baseline["channel_physics"].toObject(),origins=baseline["channel_origins"].toObject();
    Require(channels.keys()==classes.keys() && channels.keys()==physics.keys() && channels.keys()==origins.keys(),"incomplete_capacitance_witnesses");
    for(auto it=channels.begin();it!=channels.end();++it)
    {
        const auto path=QString("$.")+it.key(),cls=classes[it.key()].toString();
        Require(!cls.isEmpty() && roots.count(cls)>0 && physics[it.key()].isObject(),"missing_capacitance_image_witness");
        QJsonObject origin;
        if(origins[it.key()]==QJsonValue("capture"))
        {Require(captured.count(path)>0,"missing_capacitance_capture_channel");origin=captured.at(path);Require(origin["class"]==classes[it.key()],"capacitance_capture_class_mismatch");}
        else
        {Require(origins[it.key()]==QJsonValue("storage_image") && !captured.count(path),"invalid_capacitance_channel_origin");origin=roots.at(cls);}
        Require(ParameterNumber(origin,"Capacity")==it.value().toDouble() && ChannelPhysics(origin)==physics[it.key()].toObject(),
            "capacitance_channel_witness_mismatch");
    }
}
void ValidateCapacitanceBaseline(UEPtr<NPulseNeuron> neuron,const QJsonObject& baseline,
    const std::map<std::string,double>& scales,double maximumRelativeChange)
{
        Require(std::isfinite(maximumRelativeChange) && maximumRelativeChange>=0. && !scales.empty(),"invalid_capacitance_scale_policy");
        CheckCapacitanceDocument(baseline);
        Require(baseline["frequency_hz"]==QJsonValue(double(neuron->GetTimeStep())),"capacitance_baseline_frequency_mismatch");
}
QJsonObject BindCapacitanceToGraph(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,
    const QJsonObject& baseline,bool permitNativeGrowth)
{
    CheckCapacitanceDocument(baseline);
    if(baseline["schema_version"]!=QJsonValue(2))return baseline;
    const auto declaration=DecodeNativeModelSpec(baseline["model_spec"].toObject());
    // Retain and verify the entire original image closure even when shrink
    // temporarily removes its only live consumer.
    std::vector<std::string> imageRoots{declaration.datasetClass,declaration.generatorClass};
    for(const auto& entry:baseline["storage_images"].toArray())
        imageRoots.push_back(entry.toObject()["name"].toString().toStdString());
    const auto definitions=NmsdkTools::CaptureModelImageDefinitions(neuron,{{&source.Output,"burst"}},imageRoots);
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(definitions)==baseline["storage_images_sha256"].toString(),
        "capacitance_storage_profile_changed");
    const auto captured=IdentityNodes(baseline["capture_model"].toObject());
    const auto current=Parameters(neuron,source),roots=ImageRoots(baseline["storage_images"].toArray());
    auto result=baseline;auto channels=result["channels"].toObject(),classes=result["channel_classes"].toObject();
    auto physics=result["channel_physics"].toObject(),origins=result["channel_origins"].toObject();
    std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(const auto& component:nodes)if(auto channel=dynamic_cast<NPulseChannelCommon*>(component.Get()))
    {
        Require(typeid(*channel)==typeid(NPulseChannel),"unsupported_capacitance_channel");
        const auto key=QString::fromStdString(component->GetLongName(neuron.Get())),path=QString("$.")+key;
        Require(current.count(path)>0,"missing_effective_capacitance_channel");const auto node=current.at(path);
        if(!channels.contains(key))
        {
            Require(permitNativeGrowth && !captured.count(path),"missing_captured_capacitance_channel");
            const auto cls=node["class"].toString();Require(roots.count(cls)>0,"missing_growth_capacitance_image");
            const auto image=roots.at(cls);
            Require(ChannelPhysics(node)==ChannelPhysics(image) && ParameterNumber(node,"Capacity")==ParameterNumber(image,"Capacity"),
                "new_channel_differs_from_storage_image");
            channels[key]=ParameterNumber(image,"Capacity");classes[key]=cls;physics[key]=ChannelPhysics(image);origins[key]="storage_image";
        }
        Require(node["class"]==classes[key] && ChannelPhysics(node)==physics[key].toObject(),"capacitance_channel_physics_changed");
    }
    result["channels"]=channels;result["channel_classes"]=classes;result["channel_physics"]=physics;result["channel_origins"]=origins;
    result=SignedCapacitance(result);CheckCapacitanceDocument(result);return result;
}
CapacitancePlan PlanCapacitances(UEPtr<NPulseNeuron> neuron,const QJsonObject& baseline,
    const std::map<std::string,double>& scales,double maximumRelativeChange)
{
    ValidateCapacitanceBaseline(neuron,baseline,scales,maximumRelativeChange);
        const auto channels=baseline["channels"].toObject();CapacitancePlan result;
        Require(!channels.isEmpty(),"missing_capacitance_channels");
        for(auto it=channels.begin();it!=channels.end();++it)
            Require(!it.key().isEmpty() && it.value().isDouble() && Positive(it.value().toDouble()),"invalid_capacitance_baseline_channel");
        for(const auto& [path,scale]:scales)
        {
            Require(Positive(scale) && scale>=1.-maximumRelativeChange && scale<=1.+maximumRelativeChange,"capacitance_scale_outside_bounds");
            auto membrane=neuron->GetComponentL<NPulseMembrane>(path,true);
            Require(membrane && membrane->GetOwner().Get()==neuron.Get() && membrane->GetName().rfind("Dendrite",0)==0,
                "capacitance_scale_requires_dendritic_membrane");
            std::vector<UEPtr<UContainer>> nodes;Collect(membrane,nodes);std::size_t count=0;
            for(const auto& node:nodes)if(auto common=dynamic_cast<NPulseChannelCommon*>(node.Get()))
            {
                Require(typeid(*common)==typeid(NPulseChannel),"unsupported_capacitance_channel");
                auto channel=static_cast<NPulseChannel*>(common);StaticParameters({&channel->Capacity});
                const auto name=node->GetLongName(neuron.Get());const auto c0=channels[QString::fromStdString(name)];
                Require(c0.isDouble() && Positive(c0.toDouble()) && Positive(c0.toDouble()*scale),"missing_or_invalid_channel_capacitance_baseline");
                result.targets[name]=c0.toDouble()*scale;result.previous[name]=channel->Capacity.GetData();
                result.applied[QString::fromStdString(name)]=QJsonObject{{"baseline",c0},{"scale",scale},{"capacity",result.targets[name]}};++count;
            }
            Require(count>0,"missing_capacitance_channels");
        }
    return result;
}
void AssignCapacitances(UEPtr<NPulseNeuron> neuron,const std::map<std::string,double>& values)
{
        for(const auto& [path,value]:values)
        {
            auto channel=neuron->GetComponentL<NPulseChannel>(path,true);
            Require(bool(channel),"missing_capacitance_edit_channel");channel->Capacity=value;
            Require(channel->Capacity.GetData()==value,"capacitance_edit_readback_mismatch");
        }
        NmsdkTools::InitializeAndResetNative(neuron);
}
void VerifyOnlyCapacitiesChanged(const QJsonObject& before,const QJsonObject& after,
    const std::map<std::string,double>& targets)
{
    Require(before["valid"].toBool() && after["valid"].toBool(),"invalid_capacitance_preservation_identity");
    auto expected=before["model"].toObject();auto nodes=expected["nodes"].toArray();
    std::map<QString,QJsonObject> current;
    for(const auto& entry:after["model"].toObject()["nodes"].toArray())
    {auto node=entry.toObject();current.emplace(node["path"].toString(),node);}
    std::size_t found=0;
    for(int i=0;i<nodes.size();++i)
    {
        auto node=nodes[i].toObject();const auto path=node["path"].toString();
        if(!targets.count(path.mid(2).toStdString()))continue;
        Require(path.startsWith("$.") && current.count(path),"capacitance_preservation_channel_lost");
        auto parameters=node["parameters"].toObject();
        parameters["Capacity"]=current.at(path)["parameters"].toObject()["Capacity"];
        node["parameters"]=parameters;nodes[i]=node;++found;
    }
    expected["nodes"]=nodes;
    Require(found==targets.size() && expected==after["model"].toObject(),"capacitance_edit_changed_other_model_semantics");
}
bool HasBaselineMembrane(const QJsonObject& baseline,const std::string& path)
{
    if(path.rfind("Dendrite",0)!=0)return false;
    const auto separator=path.find('_',8);
    if(separator==std::string::npos || separator==8 || separator+1>=path.size())return false;
    const auto positiveDigits=[&](std::size_t begin,std::size_t end) {
        if(path[begin]<'1' || path[begin]>'9')return false;
        for(auto i=begin+1;i<end;++i)if(path[i]<'0' || path[i]>'9')return false;
        return true;
    };
    if(!positiveDigits(8,separator) || !positiveDigits(separator+1,path.size()))return false;
    const auto prefix=QString::fromStdString(path)+'.';
    const auto channels=baseline["channels"].toObject();
    for(auto it=channels.begin();it!=channels.end();++it)if(it.key().startsWith(prefix))return true;
    return false;
}
NativeCapacitanceCorrection ReconcileCapacitanceCorrection(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,
    const NativeCapacitanceCorrection& requested,bool permitNativeGrowth)
{
    auto result=requested;
    result.baseline=BindCapacitanceToGraph(neuron,source,requested.baseline,permitNativeGrowth);
    if(result.baseline["schema_version"]!=QJsonValue(2))
    {Require(requested.dormantMembraneScales.empty(),"legacy_capacitance_cannot_retain_dormant_scales");return result;}
    auto all=requested.membraneScales;
    for(const auto& entry:requested.dormantMembraneScales)
        Require(all.emplace(entry).second,"overlapping_active_dormant_capacitance_scales");
    Require(!all.empty() && std::isfinite(result.maximumRelativeChange) && result.maximumRelativeChange>=0.,
        "invalid_capacitance_scale_policy");
    result.membraneScales.clear();result.dormantMembraneScales.clear();
    for(const auto& [path,scale]:all)
    {
        Require(HasBaselineMembrane(result.baseline,path),"unknown_capacitance_membrane_baseline");
        Require(Positive(scale) && scale>=1.-result.maximumRelativeChange && scale<=1.+result.maximumRelativeChange,
            "capacitance_scale_outside_bounds");
        auto membrane=neuron->GetComponentL<NPulseMembrane>(path,true);
        if(membrane && membrane->GetOwner().Get()==neuron.Get())result.membraneScales.emplace(path,scale);
        else result.dormantMembraneScales.emplace(path,scale);
    }
    // Recreated heterogeneous channels must recover their original C0 even
    // when the caller never selected them for a non-unit correction.
    std::vector<UEPtr<UContainer>> nodes;Collect(neuron,nodes);
    for(const auto& node:nodes)if(auto membrane=dynamic_cast<NPulseMembrane*>(node.Get()))
        if(membrane->GetOwner().Get()==neuron.Get() && membrane->GetName().rfind("Dendrite",0)==0)
            result.membraneScales.emplace(membrane->GetName(),1.);
    return result;
}
QJsonObject ApplyCapacitanceCorrection(UEPtr<NPulseNeuron> neuron,NPulseGeneratorTransit& source,
    const NativeCapacitanceCorrection* correction,NativeCapacitanceCorrection* effective=nullptr)
{
    if(!correction)return {};
    Require(!neuron->UseElementDefaults.GetData(),"capacitance_edit_requires_storage_owned_physics");
    const auto applied=ReconcileCapacitanceCorrection(neuron,source,*correction,true);
    const auto plan=PlanCapacitances(neuron,applied.baseline,applied.membraneScales,applied.maximumRelativeChange);
    const NmsdkTools::ModelBoundarySources ports{{&source.Output,"burst"}};
    const auto before=NmsdkTools::IdentifyModel(neuron,ports);
    if(plan.previous!=plan.targets)AssignCapacitances(neuron,plan.targets);
    VerifyOnlyCapacitiesChanged(before,NmsdkTools::IdentifyModel(neuron,ports),plan.targets);
    if(effective)*effective=applied;
    return applied.baseline;
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

const char* ResistanceDomainStatusName(ResistanceDomainStatus status)
{
    switch(status){case ResistanceDomainStatus::Available:return "available";
    case ResistanceDomainStatus::Unavailable:return "unavailable";
    case ResistanceDomainStatus::Unsupported:return "unsupported";
    case ResistanceDomainStatus::Failed:return "failed";}return "unknown";
}

static ResistanceDomainAssessment AssessNativeInputResistanceDomainImpl(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec,
    std::size_t inputIndex,bool scaleInhibitionWithInput,const NativeCapacitanceCorrection* capacitance)
{
    ResistanceDomainAssessment result;result.inputIndex=inputIndex;result.geometry=requested;
    QJsonObject before;UEPtr<NPulseGeneratorTransit> source;
    try
    {
        Require(executor.AtEditBoundary(),"native_domain_requires_consumed_probe_boundary");
        Require(b.owner && b.neuron && b.dataset && b.owner->GetComponentL("Neuron",true).Get()==b.neuron.Get(),"invalid_native_domain_binding");
        Require(!b.legacyPhysicsPending && !b.postTuneActive,"conflicting_native_domain_controller");
        Require(spec.normalization==Normalization::TipResistance,"native_resistance_domain_requires_n1");
        Require(inputIndex<requested.size() && inputIndex!=requested.size()-1,"native_reference_input_not_tunable");
        Require(Positive(spec.maximumResistanceRatio) && spec.maximumResistanceRatio>=1. &&
            std::isfinite(spec.configuredMinimumResistance) && spec.configuredMinimumResistance>=0.,"invalid_native_resistance_policy");
        source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_domain_stimulus_source");
        const auto previous=ReadNativeInputParameters(b,attachments);const auto plan=MakePlan(b,previous,requested,spec);
        UEPtr<UContainer> live(b.neuron.Get());UniformTree(live,b.neuron->GetTimeStep());
        const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
        before=NmsdkTools::IdentifyModel(live,ports);Require(before["valid"].toBool(),"invalid_live_domain_identity");
        result.baseModelRevision=before["sha256"].toString().toStdString();
        auto preview=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ClonePhysicalGraphForEdit(live,ports));Lease lease{preview};
        Require(bool(preview),"failed_domain_preview");Transform(preview,*source,plan,nullptr);ApplyCapacitanceCorrection(preview,*source,capacitance);CheckRemovedListeners(b.neuron,preview);
        std::vector<int> positions;for(const auto& p:requested)positions.push_back(p.length);
        auto previewBindings=b;previewBindings.neuron=preview;
        Require(ReadNativeInputParameters(previewBindings,positions)==requested,"domain_preview_readback_mismatch");
        const auto identity=NmsdkTools::IdentifyModel(preview,ports);Require(identity["valid"].toBool(),"invalid_domain_preview_identity");
        result.previewModelRevision=identity["sha256"].toString().toStdString();
        result.previewModelJson=QJsonDocument(identity["model"].toObject()).toJson(QJsonDocument::Compact).toStdString();
        result.sourceAmplitude=source->Amplitude.GetData();result.branch=b.branch;
        result.maximumResistanceRatio=spec.maximumResistanceRatio;
        result.configuredMinimumResistance=spec.configuredMinimumResistance;
        // Record declared control dependencies, never infer scaling by equal
        // numeric Rs. Multiple logical owners lock a shared physical bank.
        std::map<NPulseSynapse*,std::set<std::size_t>> controls;
        for(std::size_t i=0;i<requested.size();++i)
        {
            auto synapse=preview->GetComponentL<NPulseSynapse>(MembranePath(b.branch,i,requested[i].length)+".ExcSynapse1",true);
            Require(bool(synapse),"unsupported_domain_excitation_bank");controls[synapse.Get()].insert(i);
        }
        if(scaleInhibitionWithInput)for(const auto& rule:spec.inhibition)
        {
            auto synapse=preview->GetComponentL<NPulseSynapse>(MembranePath(b.branch,rule.inputIndex,
                requested[rule.inputIndex].length+rule.offset)+".InhSynapse1",true);
            Require(bool(synapse),"unsupported_domain_inhibition_bank");controls[synapse.Get()].insert(rule.inputIndex);
        }
        for(const auto& [synapse,owners]:controls)
        {
            result.controls.push_back({synapse->GetLongName(preview.Get()),std::vector<std::size_t>(owners.begin(),owners.end())});
            if(owners.count(inputIndex) && owners.size()>1)result.lockedBySharedBank=true;
        }
        std::sort(result.controls.begin(),result.controls.end(),[](const auto& a,const auto& b){return a.synapsePath<b.synapsePath;});
        const auto calculate=[&] {
            auto membrane=preview->GetComponentL<NPulseMembrane>(MembranePath(b.branch,inputIndex,requested[inputIndex].length),true);
            Require(membrane && membrane->GetNumPosChannels()>0,"missing_domain_target_channel");
            auto target=dynamic_cast<NPulseChannel*>(membrane->GetPosChannel(0));
            Require(target && Positive(target->RestingResistance.GetData()),"invalid_domain_target_channel");
            const double rm=target->RestingResistance.GetData(),upper=rm*spec.maximumResistanceRatio;
            double lower=std::max(rm/spec.maximumResistanceRatio,spec.configuredMinimumResistance);
            Require(Positive(upper) && Positive(lower),"nonfinite_native_resistance_domain");
            std::vector<UEPtr<UContainer>> nodes;Collect(preview,nodes);
            for(const auto& node:nodes)if(auto channel=dynamic_cast<NPulseChannelCommon*>(node.Get()))
            {
                Require(typeid(*channel)==typeid(NPulseChannel),"unsupported_channel_integrator");
                auto c=static_cast<NPulseChannel*>(channel);
                StaticParameters({&c->Capacity,&c->Resistance,&c->RestingResistance,&c->FBResistance,&c->UseAverageSynapsis});
                double variable=0.,fixed=0.;
                for(std::size_t k=0;k<c->SynapticInputs->size();++k)
                {
                    auto synapse=dynamic_cast<NPulseSynapse*>(c->SynapticInputs.GetItem(int(k)));
                    Require(synapse && c->SynapticInputs.GetItemOutputName(int(k))=="Output","unsupported_channel_conductance_source");
                    const double g=SecretionBound(*synapse,source->Amplitude.GetData());const auto owners=controls.find(synapse);
                    if(owners!=controls.end() && owners->second.count(inputIndex))variable+=g*requested[inputIndex].resistance;
                    else fixed+=g;
                }
                if(c->UseAverageSynapsis.GetData() && !c->SynapticInputs->empty())
                {variable/=c->SynapticInputs->size();fixed/=c->SynapticInputs->size();}
                const double capacity=c->Capacity.GetData(),frequency=c->GetTimeStep();
                Require(Positive(capacity) && Positive(frequency) && std::isfinite(variable) && std::isfinite(fixed),"nonfinite_domain_channel_coefficients");
                double channelLower=0.;
                for(double r:{c->Resistance.GetData(),c->RestingResistance.GetData(),c->FBResistance.GetData()})
                {
                    Require(Positive(r),"invalid_channel_resistance");const double q=capacity*r*frequency;
                    Require(Positive(q),"nonfinite_domain_channel_scale");
                    if(variable==0.)
                    {
                        const double gain=(1.+fixed*r)*(1.+fixed*r)/q;
                        if(!std::isfinite(gain) || gain>=2.)
                        {result.status=ResistanceDomainStatus::Unavailable;result.reason="fixed_channel_not_strictly_stable";return;}
                        continue;
                    }
                    const double available=(std::sqrt(q)-1.)/r-fixed;
                    if(!Positive(available))
                    {result.status=ResistanceDomainStatus::Unavailable;result.reason="no_finite_native_no_crossing_domain";return;}
                    const double bound=variable/available;Require(Positive(bound),"nonfinite_native_resistance_floor");
                    channelLower=std::max(channelLower,bound);
                }
                if(variable>0.)
                {
                    // Round into the admissible half-line without introducing
                    // a physical percentage/timestep slack.
                    channelLower=std::nextafter(channelLower,std::numeric_limits<double>::infinity());
                    Require(Positive(channelLower),"nonfinite_native_resistance_floor");lower=std::max(lower,channelLower);
                    result.channels.push_back({node->GetLongName(preview.Get()),variable,fixed,channelLower});
                }
            }
            result.minimumResistance=lower;result.maximumResistance=upper;
            if(result.lockedBySharedBank)
            {
                const auto value=requested[inputIndex].resistance;
                result.minimumResistance=std::max(lower,value);result.maximumResistance=std::min(upper,value);
            }
            result.status=result.minimumResistance<=result.maximumResistance?ResistanceDomainStatus::Available:ResistanceDomainStatus::Unavailable;
            result.reason=result.status==ResistanceDomainStatus::Unavailable?"empty_native_resistance_domain":
                (result.lockedBySharedBank?"shared_bank_locks_resistance":"native_no_crossing_domain_v1");
        };
        calculate();
        preview->BreakLinks();preview->GetStorage()->ReturnObject(preview);lease.object=0;
        Require(NmsdkTools::IdentifyModel(live,ports)==before,"live_graph_changed_during_domain_preview");
    }
    catch(const std::exception& error)
    {
        result.status=ResistanceDomainStatus::Unsupported;result.reason=error.what();
        if(!before.isEmpty())try
        {
            if(NmsdkTools::IdentifyModel(UEPtr<UContainer>(b.neuron.Get()),{{&source->Output,"burst"}})!=before)
            {result.modelMayHaveChanged=true;result.status=ResistanceDomainStatus::Failed;}
        }
        catch(...){result.modelMayHaveChanged=true;result.status=ResistanceDomainStatus::Failed;}
    }
    return result;
}

ResistanceDomainAssessment AssessNativeInputResistanceDomain(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec,
    std::size_t inputIndex,bool scaleInhibitionWithInput)
{return AssessNativeInputResistanceDomainImpl(executor,b,attachments,requested,spec,inputIndex,scaleInhibitionWithInput,nullptr);}
ResistanceDomainAssessment AssessNativeInputResistanceDomain(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec,
    std::size_t inputIndex,bool scaleInhibitionWithInput,const NativeCapacitanceCorrection& capacitance)
{return AssessNativeInputResistanceDomainImpl(executor,b,attachments,requested,spec,inputIndex,scaleInhibitionWithInput,&capacitance);}

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
        // InputParameters has one Rs per bank. Do not silently flatten a
        // physically heterogeneous bank into its first synapse's value.
        for(int k=2;k<=membrane->NumExcitatorySynapses.GetData();++k)
        {
            auto member=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
            Require(bool(member),"missing_native_input_bank_member");
            Require(member->Resistance.GetData()==synapse->Resistance.GetData(),"heterogeneous_native_input_bank_resistance");
        }
        result.push_back({attachments[i],membrane->NumExcitatorySynapses.GetData(),synapse->Resistance.GetData()});
    }
    return result;
}

static NativeEditResult ApplyNativeInputParametersImpl(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec,const NativeCapacitanceCorrection* capacitance)
{
    NativeEditResult result;
    std::unique_ptr<NativeLearnerGraphCheckpoint> checkpoint;std::vector<InputParameters> initial;
    try
    {
        Require(executor.AtEditBoundary(),"native_edit_requires_consumed_probe_boundary");
        Require(b.owner && b.neuron && b.dataset && b.owner->GetComponentL("Neuron",true).Get()==b.neuron.Get(),"invalid_native_edit_binding");
        Require(!b.legacyPhysicsPending && !b.postTuneActive,"conflicting_native_edit_controller");
        auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_edit_stimulus_source");
        const auto previous=ReadNativeInputParameters(b,attachments);initial=previous;
        const auto plan=MakePlan(b,previous,requested,spec);const auto frequency=b.neuron->GetTimeStep();
        UEPtr<UContainer> live(b.neuron.Get());UniformTree(live,frequency);
        const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
        auto before=NmsdkTools::IdentifyModel(live,ports);Require(before["valid"].toBool(),"invalid_live_edit_identity");
        auto preview=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ClonePhysicalGraphForEdit(live,ports));Lease lease{preview};
        Require(bool(preview),"failed_edit_preview");
        NativeCapacitanceCorrection previewCapacitance;
        Transform(preview,*source,plan,nullptr);ApplyCapacitanceCorrection(preview,*source,capacitance,&previewCapacitance);CheckRemovedListeners(b.neuron,preview);
        std::vector<int> positions;for(const auto& input:requested)positions.push_back(input.length);
        auto previewBindings=b;previewBindings.neuron=preview;
        Require(ReadNativeInputParameters(previewBindings,positions)==requested,"preview_edit_readback_mismatch");
        const auto stability=AssessNativeIntegration(preview,source->Amplitude.GetData());
        Require(stability.valid,stability.reason.c_str());
        const auto intervals=ResistanceIntervals(preview,b.branch,requested,spec,source->Amplitude.GetData());
        auto predicted=NmsdkTools::IdentifyModel(preview,ports);Require(predicted["valid"].toBool(),"invalid_preview_edit_identity");
        QJsonObject preparedCapacitanceState;
        if(capacitance && previewCapacitance.baseline["schema_version"]==QJsonValue(2))
        {
            preparedCapacitanceState=EncodeNativeCapacitanceState(previewCapacitance,predicted["sha256"].toString().toStdString());
            ValidateAppliedNativeCapacitanceState(previewBindings,
                DecodeNativeModelSpec(previewCapacitance.baseline["model_spec"].toObject()),preparedCapacitanceState);
        }
        // Disposal must disconnect the speculative source consumers before the
        // live edit. No physical tick occurs on the preview or between edits.
        preview->BreakLinks();preview->GetStorage()->ReturnObject(preview);lease.object=0;
        Require(NmsdkTools::IdentifyModel(live,ports)==before,"live_graph_changed_during_preview");
        if(capacitance)checkpoint=NativeLearnerGraphCheckpoint::Capture(*b.owner);
        result.modelMayHaveChanged=true;
        NativeCapacitanceCorrection committedCapacitance;
        Transform(b.neuron,*source,plan,b.owner);const auto committedBaseline=ApplyCapacitanceCorrection(b.neuron.Get(),*source,capacitance,&committedCapacitance);
        if(capacitance)
            Require(committedBaseline==previewCapacitance.baseline &&
                committedCapacitance.membraneScales==previewCapacitance.membraneScales &&
                committedCapacitance.dormantMembraneScales==previewCapacitance.dormantMembraneScales,
                "live_capacitance_parameters_differ_from_preview");
        const auto after=NmsdkTools::VerifyModelIdentity(predicted,live,ports);
        if(!after["valid"].toBool() || !after["match"].toBool())
            throw std::runtime_error("live_edit_differs_from_native_preview:"+after["first_difference"].toString().toStdString());
        result.effective=ReadNativeInputParameters(b,positions);Require(result.effective==requested,"native_edit_readback_mismatch");
        result.minimumResistance=intervals.first;result.maximumResistance=intervals.second;
        result.capacitanceBaseline=committedBaseline;result.capacitanceState=preparedCapacitanceState;
        result.status=NativeEditStatus::Applied;
    }
    catch(const std::exception& error)
    {
        result.reason=error.what();result.status=result.modelMayHaveChanged?NativeEditStatus::Failed:NativeEditStatus::Rejected;
        if(checkpoint && result.modelMayHaveChanged)
        {
            const auto restored=checkpoint->Restore(*b.owner);
            result.graphWasReplaced=restored.modelMayHaveChanged;
            if(restored.status==NativeModelBuildStatus::Adopted)
            {result.modelMayHaveChanged=false;result.status=NativeEditStatus::Rejected;result.effective=initial;}
            else result.reason+=";rollback:"+restored.reason;
        }
    }
    return result;
}
NativeEditResult ApplyNativeInputParameters(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec)
{return ApplyNativeInputParametersImpl(executor,b,attachments,requested,spec,nullptr);}
NativeEditResult ApplyNativeInputParameters(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const std::vector<int>& attachments,const std::vector<InputParameters>& requested,const NativeEditSpec& spec,
    const NativeCapacitanceCorrection& capacitance)
{return ApplyNativeInputParametersImpl(executor,b,attachments,requested,spec,&capacitance);}

QJsonObject CaptureNativeCapacitanceBaseline(const NativeProbeExecutor& executor,const NativeProbeBindings& b)
{
    Require(executor.Status()==NativeProbeStatus::Idle || executor.AtEditBoundary(),"capacitance_capture_requires_probe_boundary");
    Require(b.owner && b.neuron && b.dataset && b.owner->GetComponentL("Neuron",true).Get()==b.neuron.Get(),"invalid_capacitance_binding");
    auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_capacitance_stimulus_source");
    UniformTree(b.neuron.Get(),b.neuron->GetTimeStep());
    const auto identity=NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}});
    Require(identity["valid"].toBool(),"invalid_capacitance_model_identity");
    QJsonObject channels;std::vector<UEPtr<UContainer>> nodes;Collect(b.neuron.Get(),nodes);
    for(const auto& node:nodes)if(auto common=dynamic_cast<NPulseChannelCommon*>(node.Get()))
    {
        Require(typeid(*common)==typeid(NPulseChannel),"unsupported_capacitance_channel");
        auto channel=static_cast<NPulseChannel*>(common);StaticParameters({&channel->Capacity});
        Require(Positive(channel->Capacity.GetData()),"invalid_capacitance_baseline");
        channels[QString::fromStdString(node->GetLongName(b.neuron.Get()))]=channel->Capacity.GetData();
    }
    Require(!channels.isEmpty(),"missing_capacitance_channels");
    QJsonObject result{{"kind","timelearner_capacitance_baseline"},{"schema_version",1},
        {"model_revision",identity["sha256"]},{"frequency_hz",double(b.neuron->GetTimeStep())},{"channels",channels}};
    result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
}

QJsonObject CaptureBoundNativeCapacitanceBaseline(const NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const NativeModelSpec& declaration)
{
    ValidateNativeModelSpec(declaration);
    auto result=CaptureNativeCapacitanceBaseline(executor,b);
    Require(declaration.branch==b.branch && declaration.frequencyHz==b.neuron->GetTimeStep(),"capacitance_declaration_mismatch");
    auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
    const auto identity=NmsdkTools::IdentifyModel(b.neuron.Get(),ports);
    const auto definitions=NmsdkTools::CaptureModelImageDefinitions(b.neuron.Get(),ports,
        {declaration.datasetClass,declaration.generatorClass});
    Require(NmsdkTools::IdentifyModel(b.neuron.Get(),ports)==identity,"capacitance_capture_changed_model");
    result["schema_version"]=2;result["model_spec"]=EncodeNativeModelSpec(declaration);result["capture_model"]=identity["model"];
    result["storage_images"]=definitions;result["storage_images_sha256"]=NmsdkTools::ModelImageDefinitionsFingerprint(definitions);
    QJsonObject classes,physics,origins;const auto nodes=IdentityNodes(identity["model"].toObject());
    const auto channels=result["channels"].toObject();
    for(auto it=channels.begin();it!=channels.end();++it)
    {
        const auto node=nodes.at(QString("$.")+it.key());
        classes[it.key()]=node["class"];physics[it.key()]=ChannelPhysics(node);origins[it.key()]="capture";
    }
    result["channel_classes"]=classes;result["channel_physics"]=physics;result["channel_origins"]=origins;
    result=SignedCapacitance(result);CheckCapacitanceDocument(result);return result;
}
QJsonObject EncodeNativeCapacitanceState(const NativeCapacitanceCorrection& correction,const std::string& revision)
{
    CheckCapacitanceDocument(correction.baseline);
    Require(correction.baseline["schema_version"]==QJsonValue(2) && Digest(QString::fromStdString(revision)) &&
        std::isfinite(correction.maximumRelativeChange) && correction.maximumRelativeChange>=0. && !correction.membraneScales.empty(),
        "invalid_capacitance_parameter_state");
    QJsonObject scales,dormant;
    for(const auto& [name,scale]:correction.membraneScales)
    {
        Require(HasBaselineMembrane(correction.baseline,name) && Positive(scale) && scale>=1.-correction.maximumRelativeChange && scale<=1.+correction.maximumRelativeChange,
            "invalid_capacitance_parameter_scale");scales[QString::fromStdString(name)]=scale;
    }
    for(const auto& [name,scale]:correction.dormantMembraneScales)
    {
        Require(!correction.membraneScales.count(name) && HasBaselineMembrane(correction.baseline,name) &&
            Positive(scale) && scale>=1.-correction.maximumRelativeChange && scale<=1.+correction.maximumRelativeChange,
            "invalid_dormant_capacitance_parameter_scale");dormant[QString::fromStdString(name)]=scale;
    }
    QJsonObject state{{"kind","timelearner_capacitance_parameter_state"},{"schema_version",dormant.isEmpty()?1:2},
        {"baseline",correction.baseline},{"membrane_scales",scales},{"maximum_relative_change",correction.maximumRelativeChange},
        {"applied_model_revision",QString::fromStdString(revision)}};
    if(!dormant.isEmpty())state["dormant_membrane_scales"]=dormant;
    return SignedCapacitance(state);
}
NativeCapacitanceCorrection ParseNativeCapacitanceState(const std::string& text,std::string& revision)
{
    const auto state=ParseStrictNativeJsonObject(text);const auto names=state.keys();
    const bool dormantVersion=state["schema_version"]==QJsonValue(2);
    std::set<QString> expected{"kind","schema_version","baseline","membrane_scales",
        "maximum_relative_change","applied_model_revision","sha256"};
    if(dormantVersion)expected.insert("dormant_membrane_scales");
    Require(std::set<QString>(names.begin(),names.end())==expected &&
        state["kind"]==QJsonValue("timelearner_capacitance_parameter_state") &&
        (dormantVersion || state["schema_version"]==QJsonValue(1)) &&
        (!dormantVersion || (state["dormant_membrane_scales"].isObject() && !state["dormant_membrane_scales"].toObject().isEmpty())) &&
        state["baseline"].isObject() && state["membrane_scales"].isObject() && state["maximum_relative_change"].isDouble() &&
        Digest(state["applied_model_revision"]),"invalid_capacitance_parameter_state");
    NativeCapacitanceCorrection result;result.baseline=state["baseline"].toObject();result.maximumRelativeChange=state["maximum_relative_change"].toDouble();
    const auto scales=state["membrane_scales"].toObject();
    for(auto it=scales.begin();it!=scales.end();++it)
    {Require(it.value().isDouble(),"invalid_capacitance_parameter_scale");result.membraneScales[it.key().toStdString()]=it.value().toDouble();}
    const auto dormant=state["dormant_membrane_scales"].toObject();
    for(auto it=dormant.begin();it!=dormant.end();++it)
    {Require(it.value().isDouble(),"invalid_dormant_capacitance_parameter_scale");result.dormantMembraneScales[it.key().toStdString()]=it.value().toDouble();}
    const auto actualRevision=state["applied_model_revision"].toString().toStdString();
    Require(EncodeNativeCapacitanceState(result,actualRevision)==state,"capacitance_parameter_state_digest_mismatch");
    revision=actualRevision;return result;
}
void ValidateAppliedNativeCapacitanceState(const NativeProbeBindings& b,const NativeModelSpec& declaration,const QJsonObject& state)
{
    std::string revision;const auto correction=ParseNativeCapacitanceState(QJsonDocument(state).toJson(QJsonDocument::Compact).toStdString(),revision);
    Require(b.neuron && b.dataset && correction.baseline["model_spec"]==QJsonValue(EncodeNativeModelSpec(declaration)),
        "capacitance_declaration_mismatch");
    UniformTree(b.neuron.Get(),declaration.frequencyHz);
    auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_capacitance_stimulus_source");
    Require(BindCapacitanceToGraph(b.neuron.Get(),*source,correction.baseline,false)==correction.baseline,"capacitance_baseline_changed");
    for(const auto& [name,scale]:correction.dormantMembraneScales)
        Require(!b.neuron->GetComponentL(name,true),"dormant_capacitance_membrane_is_present");
    const auto plan=PlanCapacitances(b.neuron.Get(),correction.baseline,correction.membraneScales,correction.maximumRelativeChange);
    Require(plan.targets==plan.previous,"capacitance_state_does_not_match_model");
    const auto channels=correction.baseline["channels"].toObject();std::vector<UEPtr<UContainer>> nodes;Collect(b.neuron.Get(),nodes);
    for(const auto& node:nodes)if(auto channel=dynamic_cast<NPulseChannel*>(node.Get()))
    {
        const auto name=node->GetLongName(b.neuron.Get());const auto target=plan.targets.find(name);
        const auto c0=channels[QString::fromStdString(name)];Require(c0.isDouble(),"missing_capacitance_state_channel");
        Require(channel->Capacity.GetData()==(target==plan.targets.end()?c0.toDouble():target->second),"incomplete_capacitance_parameter_scales");
    }
    const auto identity=NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}});
    Require(identity["valid"].toBool() && identity["sha256"]==QJsonValue(QString::fromStdString(revision)),"capacitance_applied_model_revision_mismatch");
}

NativeCapacitanceEditResult ApplyNativeCapacitanceScale(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const QJsonObject& baseline,const std::map<std::string,double>& scales,double maximumRelativeChange)
{
    NativeCapacitanceCorrection correction;correction.baseline=baseline;correction.membraneScales=scales;
    correction.maximumRelativeChange=maximumRelativeChange;
    return ApplyNativeCapacitanceScale(executor,b,correction);
}
NativeCapacitanceEditResult ApplyNativeCapacitanceScale(NativeProbeExecutor& executor,const NativeProbeBindings& b,
    const NativeCapacitanceCorrection& correction)
{
    NativeCapacitanceEditResult result;std::map<std::string,double> previous;
    UEPtr<NPulseGeneratorTransit> source;QJsonObject before;
    try
    {
        Require(executor.AtEditBoundary(),"capacitance_edit_requires_consumed_probe_boundary");
        Require(!b.legacyPhysicsPending && !b.postTuneActive,"conflicting_capacitance_controller");
        Require(b.owner && b.neuron && b.dataset && b.owner->GetComponentL("Neuron",true).Get()==b.neuron.Get(),"invalid_capacitance_binding");
        Require(!b.neuron->UseElementDefaults.GetData(),"capacitance_edit_requires_storage_owned_physics");
        source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_capacitance_stimulus_source");
        const auto effective=ReconcileCapacitanceCorrection(b.neuron.Get(),*source,correction,false);
        Require(effective.baseline==correction.baseline,"capacitance_baseline_changed");
        const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
        UniformTree(b.neuron.Get(),b.neuron->GetTimeStep());before=NmsdkTools::IdentifyModel(b.neuron.Get(),ports);
        Require(before["valid"].toBool(),"invalid_capacitance_model_identity");
        result.beforeRevision=before["sha256"].toString().toStdString();
        const auto capacitance=PlanCapacitances(b.neuron.Get(),effective.baseline,effective.membraneScales,effective.maximumRelativeChange);
        const auto& targets=capacitance.targets;previous=capacitance.previous;result.appliedChannels=capacitance.applied;
        auto preview=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ClonePhysicalGraphForEdit(b.neuron.Get(),ports));Lease lease{preview};
        Require(bool(preview),"failed_capacitance_preview");AssignCapacitances(preview,targets);
        const auto stable=AssessNativeIntegration(preview,source->Amplitude.GetData());Require(stable.valid,stable.reason.c_str());
        const auto predicted=NmsdkTools::IdentifyModel(preview,ports);Require(predicted["valid"].toBool(),"invalid_capacitance_preview_identity");
        VerifyOnlyCapacitiesChanged(before,predicted,targets);
        QJsonObject preparedState;
        if(effective.baseline["schema_version"]==QJsonValue(2))
        {
            preparedState=EncodeNativeCapacitanceState(effective,predicted["sha256"].toString().toStdString());
            auto previewBindings=b;previewBindings.neuron=preview;
            ValidateAppliedNativeCapacitanceState(previewBindings,
                DecodeNativeModelSpec(effective.baseline["model_spec"].toObject()),preparedState);
        }
        preview->BreakLinks();preview->GetStorage()->ReturnObject(preview);lease.object=0;
        Require(NmsdkTools::IdentifyModel(b.neuron.Get(),ports)==before,"live_graph_changed_during_capacitance_preview");
        if(previous!=targets)
        {
            result.modelMayHaveChanged=true;AssignCapacitances(b.neuron.Get(),targets);
        }
        const auto verified=NmsdkTools::VerifyModelIdentity(predicted,b.neuron.Get(),ports);
        Require(verified["valid"].toBool() && verified["match"].toBool(),"live_capacitance_edit_differs_from_preview");
        result.afterRevision=predicted["sha256"].toString().toStdString();result.capacitanceState=preparedState;
        result.status=NativeEditStatus::Applied;
    }
    catch(const std::exception& error)
    {
        result.reason=error.what();result.status=NativeEditStatus::Rejected;
        if(result.modelMayHaveChanged)
        {
            try
            {
                AssignCapacitances(b.neuron.Get(),previous);
                Require(NmsdkTools::IdentifyModel(b.neuron.Get(),{{&source->Output,"burst"}})==before,"capacitance_rollback_identity_mismatch");
                result.modelMayHaveChanged=false;
            }
            catch(const std::exception& rollback){result.reason+=";rollback:"+std::string(rollback.what());result.status=NativeEditStatus::Failed;}
        }
    }
    return result;
}

const char* NativeEditStatusName(NativeEditStatus status)
{switch(status){case NativeEditStatus::Applied:return "applied";case NativeEditStatus::Rejected:return "rejected";case NativeEditStatus::Failed:return "failed";}return "unknown";}

NativeEditSpec MakeNativeEditSpec(Normalization normalization,int maximumLength,const std::vector<InputParameters>& requested,
    bool nextSegment,bool segmentEI,int delta,double alpha,double maximumResistanceRatio,double configuredMinimumResistance,
    bool preserveAllocatedCable)
{
    NativeEditSpec result;result.normalization=normalization;result.maximumCableLength=maximumLength;
    result.maximumResistanceRatio=maximumResistanceRatio;result.configuredMinimumResistance=configuredMinimumResistance;
    result.preserveAllocatedCable=preserveAllocatedCable;
    Require(!segmentEI || (delta>=1 && Positive(alpha)),"invalid_segment_inhibition_policy");
    for(std::size_t i=0;i<requested.size();++i)
    {
        if(segmentEI)result.inhibition.push_back({i,delta,requested[i].resistance/alpha});
        if(nextSegment && !(segmentEI && delta==1))result.inhibition.push_back({i,1,requested[i].resistance});
    }
    return result;
}
}
