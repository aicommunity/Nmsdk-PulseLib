#include "NativeModelBuilder.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
bool Positive(double value){return std::isfinite(value) && value>0.;}
std::string Path(bool branch,std::size_t input,int position)
{return "Dendrite"+std::to_string(branch?1u:input+1)+"_"+std::to_string(position);}
void Tree(UEPtr<UContainer> node,unsigned frequency)
{
    Require(node && node->IsReady() && node->IsInit() && node->GetTimeStep()==frequency,"native_model_not_uniform_ready_initialized");
    std::vector<NameT> names;node->GetComponentsList(names);
    for(const auto& name:names)Tree(node->GetComponent(name,true),frequency);
}
void OwnerRate(UNet& owner,unsigned frequency)
{
    Require(owner.GetStorage() && owner.GetEnvironment() && frequency>0,"missing_native_model_context");
    for(UEPtr<UContainer> node(&owner);node;node=node->GetOwner())
        Require(node->GetTimeStep()==frequency,"native_model_owner_frequency_mismatch");
}
std::vector<int> Allocation(const NativeModelSpec& s)
{
    std::vector<int> required(s.branch?1:s.inputs.size(),1);
    for(std::size_t i=0;i<s.inputs.size();++i)required[s.branch?0:i]=std::max(required[s.branch?0:i],s.inputs[i].attachment);
    for(const auto& rule:s.editPolicy.inhibition)
    {
        const long long position=static_cast<long long>(s.inputs.at(rule.inputIndex).attachment)+rule.offset;
        Require(position<=s.editPolicy.maximumCableLength,"native_model_missing_inhibition_headroom");
        required[s.branch?0:rule.inputIndex]=std::max(required[s.branch?0:rule.inputIndex],int(position));
    }
    if(!s.allocatedCableLengths.empty())
    {
        Require(s.allocatedCableLengths.size()==required.size(),"native_model_allocation_dimension_mismatch");
        for(std::size_t i=0;i<required.size();++i)Require(s.allocatedCableLengths[i]>=required[i] &&
            s.allocatedCableLengths[i]<=s.editPolicy.maximumCableLength,"native_model_allocation_outside_bounds");
        required=s.allocatedCableLengths;
    }
    if(s.neuronStructureMode==1)
    {
        const int maximum=*std::max_element(required.begin(),required.end());
        if(!s.allocatedCableLengths.empty())for(int value:required)Require(value==maximum,"native_scalar_allocation_is_not_uniform");
        std::fill(required.begin(),required.end(),maximum);
    }
    return required;
}
template<class T> UEPtr<T> Take(UNet& owner,const std::string& name,unsigned frequency)
{
    Require(!name.empty() && owner.GetStorage()->CheckClass(name),"missing_native_model_image");
    auto object=dynamic_pointer_cast<UContainer>(owner.GetStorage()->TakeObject(name));
    auto typed=dynamic_pointer_cast<T>(object);
    if(!typed){if(object)owner.GetStorage()->ReturnObject(object);throw std::invalid_argument("wrong_native_model_image_type");}
    typed->SetEnvironment(owner.GetEnvironment());typed->UnInit();typed->SetGlobalTimeStep(frequency);return typed;
}
void Connect(NPulseSynapseCommon& synapse,NPulseGeneratorTransit& source)
{
    const auto sources=synapse.Input.GetDataSources();
    Require(sources.empty() || (sources.size()==1 && sources.front()==&source.Output),"native_initial_bank_has_foreign_stimulus");
    if(sources.empty()){int slot=0;source.Connect(&synapse,"Output","Input",slot,true);}
    const auto actual=synapse.Input.GetDataSources();
    Require(actual.size()==1 && actual.front()==&source.Output,"native_initial_stimulus_not_connected");
}
struct NewObjects {
    UNet& owner;UEPtr<NPulseNeuron> neuron;UEPtr<NDatasetMatrix> dataset;
    bool neuronAttached=false,datasetAttached=false,released=false;
    ~NewObjects()
    {
        if(released)return;
        if(neuron)neuron->BreakLinks();if(dataset)dataset->BreakLinks();
        if(neuronAttached)owner.DelComponent("Neuron",true);else if(neuron)owner.GetStorage()->ReturnObject(neuron);
        if(datasetAttached)owner.DelComponent("DatasetMatrix",true);else if(dataset)owner.GetStorage()->ReturnObject(dataset);
    }
};
NativeProbeBindings Bindings(UNet& owner,const NativeModelSpec& s)
{
    NativeProbeBindings b;b.owner=&owner;b.branch=s.branch;b.inputCount=int(s.inputs.size());
    b.neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);b.dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    return b;
}
bool Within(UContainer* node,UContainer* root)
{for(auto p=node;p;p=p->GetOwner().Get())if(p==root)return true;return false;}
void Collect(UEPtr<UContainer> root,std::vector<UEPtr<UContainer>>& nodes)
{if(!root)return;nodes.push_back(root);std::vector<NameT> names;root->GetComponentsList(names);for(const auto& n:names)Collect(root->GetComponent(n,true),nodes);}
void CheckReplacement(UNet& owner)
{
    auto oldNeuron=owner.GetComponent("Neuron",true),oldDataset=owner.GetComponent("DatasetMatrix",true);
    Require(!oldNeuron || dynamic_cast<NPulseNeuron*>(oldNeuron.Get()),"occupied_native_model_slot_wrong_type");
    Require(!oldDataset || dynamic_cast<NDatasetMatrix*>(oldDataset.Get()),"occupied_native_model_slot_wrong_type");
    const auto inside=[&](UContainer* node){return Within(node,oldNeuron.Get()) || Within(node,oldDataset.Get());};
    std::vector<UEPtr<UContainer>> old;Collect(oldNeuron,old);Collect(oldDataset,old);
    std::set<const UIProperty*> outputs;
    for(const auto& node:old)for(const auto& [name,entry]:node->GetPropertiesList())
        if(auto output=dynamic_cast<UIPropertyOutput*>(entry.Property.Get()))
        {
            outputs.insert(output);
            for(std::size_t i=0;i<output->GetNumConnectors();++i)
            {auto input=output->GetConnectorProperty(int(i));Require(!input || inside(input->GetOwner()),"native_cold_replace_has_external_listener");}
            if(auto item=dynamic_cast<UItem*>(node.Get()))for(int i=0;i<item->GetNumAConnectors(name);++i)
            {auto consumer=item->GetAConnectorByIndex(name,i);Require(!consumer || inside(consumer.Get()),"native_cold_replace_has_external_listener");}
        }
    UEPtr<UContainer> root(&owner);while(root->GetOwner())root=root->GetOwner();
    if(outputs.empty())return;
    std::vector<UEPtr<UContainer>> all;Collect(root,all);
    for(const auto& node:all)if(!inside(node.Get()))for(const auto& [name,entry]:node->GetPropertiesList())
        if(auto input=dynamic_cast<UIPropertyInput*>(entry.Property.Get()))
        {
            if(!input->IsConnected() && !input->UsesExternalDataSource())continue;
            Require(input->CanInspectDataSources(),"uninspectable_native_replacement_listener");
            for(auto source:input->GetDataSources())Require(!outputs.count(source),"native_cold_replace_has_external_listener");
        }
}
}
bool ValidNativeTarget(const MDMatrix<double>& pattern,std::size_t inputs)
{
    if(!inputs || pattern.GetRows()!=int(inputs) || pattern.GetCols()!=1)return false;
    double total=0.;for(std::size_t i=0;i<inputs;++i)
    {const auto value=pattern(int(i),0);if(!std::isfinite(value) || value<0. || !std::isfinite(total+=value))return false;}
    return true;
}
void AssignNativeTarget(NDatasetMatrix& dataset,const MDMatrix<double>& pattern)
{
    Require(ValidNativeTarget(pattern,std::size_t(dataset.MaxSpikesPerFeature.GetData())),"invalid_native_target_pattern");
    Require(dataset.NumSamples.GetData()==1 && dataset.NumFeatures.GetData()==1,"native_target_dataset_dimension_mismatch");
    dataset.MatrixData=pattern;dataset.Build();dataset.Init();
    Require(dataset.MatrixData.GetData().GetRows()==pattern.GetRows() && dataset.MatrixData.GetData().GetCols()==1,
        "native_target_readback_dimension_mismatch");
    for(int i=0;i<pattern.GetRows();++i)Require(dataset.MatrixData(i,0)==pattern(i,0),"native_target_readback_mismatch");
}
void ValidateNativeModelSpec(const NativeModelSpec& s)
{
    Require(s.schemaVersion==1 && s.frequencyHz>0 && !s.neuronClass.empty() && !s.datasetClass.empty() && !s.generatorClass.empty() &&
        !s.inputs.empty() && s.inputs.size()<=std::size_t(std::numeric_limits<int>::max()) &&
        s.initialIsi.size()==s.inputs.size(),"invalid_native_model_specification");
    Require(s.neuronStructureMode>=0 && s.neuronStructureMode<=2 &&
        (s.normalization==Normalization::SynapseCount || s.normalization==Normalization::TipResistance) &&
        s.editPolicy.normalization==s.normalization && s.editPolicy.maximumCableLength>=1,"invalid_native_model_policy");
    Require(!s.allowMutedInputBaseline || s.branch,"muted_native_baseline_requires_branch");
    Require(s.neuronStructureMode!=0 || s.allocatedCableLengths.empty(),"explicit_native_allocation_belongs_to_image");
    Require(std::isfinite(s.editPolicy.maximumResistanceRatio) && s.editPolicy.maximumResistanceRatio>=1. &&
        std::isfinite(s.editPolicy.configuredMinimumResistance) && s.editPolicy.configuredMinimumResistance>=0.,"invalid_native_model_resistance_policy");
    double total=0.;std::map<int,const NativeInitialInput*> shared;
    for(std::size_t i=0;i<s.inputs.size();++i)
    {
        const auto& p=s.inputs[i];Require(p.attachment>=1 && p.attachment<=s.editPolicy.maximumCableLength &&
            p.synapses>=1 && p.synapses<=128 && (!p.resistance || Positive(*p.resistance)) &&
            (s.normalization!=Normalization::TipResistance || p.synapses==1),"invalid_native_initial_input");
        Require(std::isfinite(s.initialIsi[i]) && s.initialIsi[i]>=0. && std::isfinite(total+=s.initialIsi[i]),"invalid_native_initial_isi");
        if(s.branch)
        {
            Require(!i || s.inputs[i-1].attachment>=p.attachment,"invalid_native_branch_order");
            auto [found,inserted]=shared.emplace(p.attachment,&p);
            Require(inserted || (found->second->synapses==p.synapses && found->second->resistance==p.resistance),"incompatible_native_initial_shared_bank");
        }
    }
    for(const auto& rule:s.editPolicy.inhibition)Require(rule.inputIndex<s.inputs.size() && rule.offset>0 && Positive(rule.resistance),"invalid_native_initial_inhibition");
    Allocation(s);
}
static NativeModelBuildResult InspectBuiltModel(UNet& owner,const NativeModelSpec& s,const std::vector<int>& attachments,
                                               NativeProbeBindings b)
{
    NativeModelBuildResult r;
    try
    {
        ValidateNativeModelSpec(s);OwnerRate(owner,s.frequencyHz);
        Require(b.neuron && b.dataset,"missing_native_model_children");
        Require(b.neuron->GetCompClassName()==s.neuronClass && b.dataset->GetCompClassName()==s.datasetClass,"native_model_class_mismatch");
        Require(!b.neuron->UseElementDefaults.GetData(),"native_model_requires_storage_owned_physics");
        Require(b.neuron->StructureBuildMode.GetData()==s.neuronStructureMode &&
            b.neuron->NumSomaMembraneParts.GetData()==(s.branch?1:int(s.inputs.size())),"native_model_topology_mismatch");
        Tree(b.neuron,s.frequencyHz);Tree(b.dataset,s.frequencyHz);
        const int somas=s.branch?1:int(s.inputs.size());
        auto allocation=b.neuron->NumDendriteMembranePartsVec.GetData();
        if(s.neuronStructureMode==1)allocation.assign(somas,b.neuron->NumDendriteMembraneParts.GetData());
        if(s.neuronStructureMode!=0)Require(allocation.size()==std::size_t(somas),"native_model_cable_allocation_dimension_mismatch");
        for(int i=0;i<somas;++i)
        {
            Require(bool(b.neuron->GetComponentL<NPulseMembrane>("Soma"+std::to_string(i+1),true)),"missing_native_model_soma");
            if(s.neuronStructureMode!=0)
            {
                Require(allocation[i]>=1,"invalid_native_model_cable_allocation");
                for(int k=1;k<=allocation[i];++k)
                    Require(bool(b.neuron->GetComponentL<NPulseMembrane>(Path(s.branch,std::size_t(i),k),true)),"missing_native_model_cable_segment");
            }
        }
        auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        Require(source && source->GetCompClassName()==s.generatorClass,"native_model_generator_class_mismatch");
        Require(b.dataset->Generators.size()==1 && b.dataset->Generators[0]==source.Get(),"native_model_generator_cache_mismatch");
        Require(Positive(source->Amplitude.GetData()) && Positive(source->PulseLength.GetData()) &&
            source->PulseLength.GetData()>=1./s.frequencyHz && !source->UseTransitSignal.GetData() &&
            !source->UsePatternOutput.GetData() && source->FrequencyDeviation.GetData()==0.,"native_model_unsupported_generator_profile");
        Require(b.dataset->NumSamples.GetData()==1 && b.dataset->NumFeatures.GetData()==1 &&
            b.dataset->Iteration.GetData()==0 &&
            b.dataset->MaxSpikesPerFeature.GetData()==int(s.inputs.size()) &&
            ValidNativeTarget(b.dataset->MatrixData.GetData(),s.inputs.size()),"native_model_dataset_mismatch");
        r.effective=ReadNativeInputParameters(b,attachments);
        for(std::size_t i=0;i<attachments.size();++i)
        {
            Require(attachments[i]>=1 && attachments[i]<=s.editPolicy.maximumCableLength &&
                (!s.branch || !i || attachments[i-1]>=attachments[i]),"native_model_attachment_mismatch");
            auto membrane=b.neuron->GetComponentL<NPulseMembrane>(Path(s.branch,i,attachments[i]),true);
            std::optional<bool> bankConnected;
            for(int k=1;k<=r.effective[i].synapses;++k)
            {
                auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
                const auto inputs=synapse->Input.GetDataSources();
                const bool connected=inputs.size()==1 && inputs.front()==&source->Output;
                Require(connected || (s.allowMutedInputBaseline && inputs.empty()),"native_model_input_binding_mismatch");
                Require(!bankConnected || *bankConnected==connected,"native_model_partially_connected_input_bank");bankConnected=connected;
            }
        }
        for(const auto& rule:s.editPolicy.inhibition)
        {
            const long long position=static_cast<long long>(attachments.at(rule.inputIndex))+rule.offset;
            Require(position<=s.editPolicy.maximumCableLength,"native_inhibition_attachment_outside_bounds");
            auto synapse=b.neuron->GetComponentL<NPulseSynapseCommon>(Path(s.branch,rule.inputIndex,int(position))+".InhSynapse1",true);
            Require(bool(synapse),"missing_native_inhibitory_attachment");const auto inputs=synapse->Input.GetDataSources();
            Require((inputs.size()==1 && inputs.front()==&source->Output) ||
                (s.allowMutedInputBaseline && inputs.empty()),"native_model_inhibition_binding_mismatch");
        }
        const auto identity=NmsdkTools::IdentifyModel(b.neuron,{{&source->Output,"burst"}});
        Require(identity["valid"].toBool(),"invalid_native_built_model_identity");
        r.modelRevision=identity["sha256"].toString().toStdString();r.status=NativeModelBuildStatus::Adopted;
    }
    catch(const std::exception& e){r.reason=e.what();}
    return r;
}
NativeModelBuildResult InspectNativeLearnerModel(UNet& owner,const NativeModelSpec& s,const std::vector<int>& attachments)
{return InspectBuiltModel(owner,s,attachments,Bindings(owner,s));}
NativeModelBuildResult CommitNativeLearnerChildren(UNet& owner,const NativeModelSpec& s,
    const std::vector<InputParameters>& expected,UEPtr<NPulseNeuron> neuron,UEPtr<NDatasetMatrix> dataset)
{
    NativeModelBuildResult result;UEPtr<UContainer> oldNeuron,oldDataset;bool detached=false,committed=false;
    std::vector<NameT> order;owner.GetComponentsList(order);
    const auto restoreOrder=[&] {
        for(std::size_t i=0;i<order.size();++i)
        {
            std::vector<NameT> now;owner.GetComponentsList(now);const auto at=std::find(now.begin(),now.end(),order[i]);
            Require(at!=now.end(),"import_owner_member_lost");const int offset=int(i)-int(at-now.begin());
            if(offset)Require(owner.ChangeComponentPosition(order[i],offset),"import_owner_order_restore_failed");
        }
    };
    try
    {
        Require(neuron && dataset && !neuron->GetOwner() && !dataset->GetOwner(),"import_children_must_be_unowned");
        Require(neuron->GetName()=="Neuron" && dataset->GetName()=="DatasetMatrix","import_children_name_mismatch");
        std::vector<int> positions;for(const auto& p:expected)positions.push_back(p.length);
        NativeProbeBindings b;b.owner=&owner;b.neuron=neuron;b.dataset=dataset;b.branch=s.branch;b.inputCount=int(s.inputs.size());
        const auto preview=InspectBuiltModel(owner,s,positions,b);
        Require(preview.status==NativeModelBuildStatus::Adopted,preview.reason.c_str());
        Require(preview.effective==expected,"import_effective_parameters_mismatch");
        auto source=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
        const auto stability=AssessNativeIntegration(neuron,source->Amplitude.GetData());Require(stability.valid,stability.reason.c_str());
        CheckReplacement(owner);oldNeuron=owner.GetComponent("Neuron",true);oldDataset=owner.GetComponent("DatasetMatrix",true);
        if(oldNeuron)owner.DelComponent("Neuron",false);if(oldDataset)owner.DelComponent("DatasetMatrix",false);detached=true;
        UEPtr<UContainer> child=dataset;Require(owner.AddComponent(child)!=ForbiddenId,"import_dataset_attach_failed");
        child=neuron;Require(owner.AddComponent(child)!=ForbiddenId,"import_neuron_attach_failed");restoreOrder();
        result=InspectNativeLearnerModel(owner,s,positions);
        Require(result.status==NativeModelBuildStatus::Adopted && result.modelRevision==preview.modelRevision && result.effective==expected,
            "import_graph_changed_during_attachment");
        committed=true;result.modelMayHaveChanged=true;
        if(oldNeuron)dynamic_pointer_cast<UNet>(oldNeuron)->BreakLinks();if(oldDataset)dynamic_pointer_cast<UNet>(oldDataset)->BreakLinks();
        if(oldNeuron)owner.GetStorage()->ReturnObject(oldNeuron);if(oldDataset)owner.GetStorage()->ReturnObject(oldDataset);
        result.modelMayHaveChanged=true;return result;
    }
    catch(const std::exception& error)
    {
        if(committed){result.status=NativeModelBuildStatus::Failed;result.reason=error.what();result.modelMayHaveChanged=true;return result;}
        result.status=NativeModelBuildStatus::Rejected;result.reason=error.what();result.modelMayHaveChanged=false;
        if(detached)try
        {
            if(neuron->GetOwner().Get()==&owner)owner.DelComponent("Neuron",false);
            if(dataset->GetOwner().Get()==&owner)owner.DelComponent("DatasetMatrix",false);
            if(oldDataset)Require(owner.AddComponent(oldDataset)!=ForbiddenId,"import_rollback_dataset_failed");
            if(oldNeuron)Require(owner.AddComponent(oldNeuron)!=ForbiddenId,"import_rollback_neuron_failed");restoreOrder();
        }
        catch(...){result.status=NativeModelBuildStatus::Failed;result.modelMayHaveChanged=true;result.reason+=";import_rollback_failed";}
        return result;
    }
}
NativeModelBuildResult ConstructNativeLearnerModel(UNet& owner,const NativeModelSpec& s,bool explicitColdReplacement)
{
    NativeModelBuildResult r;NewObjects lease{owner};
    try
    {
        ValidateNativeModelSpec(s);OwnerRate(owner,s.frequencyHz);
        if(explicitColdReplacement)CheckReplacement(owner);
        else Require(!owner.GetComponent("Neuron",true) && !owner.GetComponent("DatasetMatrix",true),"native_construction_requires_empty_named_slots");
        lease.neuron=Take<NPulseNeuron>(owner,s.neuronClass,s.frequencyHz);
        auto neuron=lease.neuron;neuron->Name="Neuron";
        Require(!neuron->UseElementDefaults.GetData(),"native_model_requires_storage_owned_physics");
        const auto allocation=Allocation(s);neuron->StructureBuildMode=s.neuronStructureMode;
        if(s.neuronStructureMode!=0)
        {
            neuron->NumSomaMembraneParts=s.branch?1:int(s.inputs.size());
            if(s.neuronStructureMode==1)neuron->NumDendriteMembraneParts=allocation.front();
            else neuron->NumDendriteMembranePartsVec=allocation;
        }
        neuron->Build();
        lease.dataset=Take<NDatasetMatrix>(owner,s.datasetClass,s.frequencyHz);
        auto dataset=lease.dataset;dataset->Name="DatasetMatrix";dataset->PulseGeneratorClassName=s.generatorClass;
        dataset->NumSamples=1;dataset->NumFeatures=1;dataset->MaxSpikesPerFeature=int(s.inputs.size());
        dataset->AdvanceSampleAfterBurst=false;dataset->Iteration=0;dataset->StateGeneration=0;
        MDMatrix<double> pattern;pattern.Assign(int(s.inputs.size()),1,0.);
        for(std::size_t i=0;i<s.inputs.size();++i)pattern(int(i),0)=s.initialIsi[i];
        AssignNativeTarget(*dataset,pattern);
        auto source=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"missing_native_constructed_generator");
        std::map<std::string,NativeInitialInput> banks;
        for(std::size_t i=0;i<s.inputs.size();++i)banks.emplace(Path(s.branch,i,s.inputs[i].attachment),s.inputs[i]);
        for(const auto& [path,p]:banks)
        {
            auto membrane=neuron->GetComponentL<NPulseMembrane>(path,true);Require(bool(membrane),"native_initial_attachment_outside_graph");
            if(membrane->NumExcitatorySynapses.GetData()!=p.synapses){membrane->NumExcitatorySynapses=p.synapses;membrane->Build();}
            for(int k=1;k<=p.synapses;++k)
            {
                auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
                Require(bool(synapse),"missing_native_initial_bank_member");
                if(p.resistance)synapse->Resistance=*p.resistance;
                Connect(*synapse,*source);
            }
        }
        std::map<std::string,double> inhib;
        for(const auto& rule:s.editPolicy.inhibition)
        {
            const auto path=Path(s.branch,rule.inputIndex,s.inputs[rule.inputIndex].attachment+rule.offset);
            auto [found,inserted]=inhib.emplace(path,rule.resistance);
            Require(inserted || found->second==rule.resistance,"incompatible_native_initial_inhibition_bank");
        }
        for(const auto& [path,resistance]:inhib)
        {
            auto membrane=neuron->GetComponentL<NPulseMembrane>(path,true);Require(bool(membrane),"native_initial_inhibition_outside_graph");
            if(membrane->NumInhibitorySynapses.GetData()<1){membrane->NumInhibitorySynapses=1;membrane->Build();}
            auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("InhSynapse1",true);Require(bool(synapse),"missing_native_initial_inhibition_bank");
            synapse->Resistance=resistance;Connect(*synapse,*source);
        }
        neuron->Init();Tree(neuron,s.frequencyHz);Tree(dataset,s.frequencyHz);
        const auto integration=AssessNativeIntegration(neuron,source->Amplitude.GetData());Require(integration.valid,integration.reason.c_str());
        std::vector<int> positions;for(const auto& p:s.inputs)positions.push_back(p.attachment);
        NativeProbeBindings preview;preview.owner=&owner;preview.neuron=neuron;preview.dataset=dataset;
        preview.branch=s.branch;preview.inputCount=int(s.inputs.size());
        const auto expected=InspectBuiltModel(owner,s,positions,preview);
        Require(expected.status==NativeModelBuildStatus::Adopted,expected.reason.c_str());
        r.modelMayHaveChanged=true;
        if(explicitColdReplacement){owner.DelComponent("Neuron",true);owner.DelComponent("DatasetMatrix",true);}
        UEPtr<UContainer> child=dataset;Require(owner.AddComponent(child)!=ForbiddenId,"native_dataset_attach_failed");lease.datasetAttached=true;
        child=neuron;Require(owner.AddComponent(child)!=ForbiddenId,"native_neuron_attach_failed");lease.neuronAttached=true;
        auto checked=InspectNativeLearnerModel(owner,s,positions);
        Require(checked.status==NativeModelBuildStatus::Adopted,checked.reason.c_str());
        Require(checked.modelRevision==expected.modelRevision,"native_model_changed_during_attachment");
        checked.status=NativeModelBuildStatus::Constructed;checked.modelMayHaveChanged=true;lease.released=true;return checked;
    }
    catch(const std::exception& e){r.reason=e.what();r.status=r.modelMayHaveChanged?NativeModelBuildStatus::Failed:NativeModelBuildStatus::Rejected;}
    return r;
}
NativeModelSpec DescribeNativeLearnerModel(UNet& owner,bool branch,Normalization normalization,
    const std::vector<int>& attachments,const NativeEditSpec& policy)
{
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    Require(neuron && dataset,"missing_native_model_children");auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"missing_native_model_generator");NativeModelSpec s;s.frequencyHz=owner.GetTimeStep();s.branch=branch;
    s.normalization=normalization;s.editPolicy=policy;s.neuronClass=neuron->GetCompClassName();s.datasetClass=dataset->GetCompClassName();
    s.generatorClass=generator->GetCompClassName();s.neuronStructureMode=neuron->StructureBuildMode.GetData();
    if(s.neuronStructureMode==2)s.allocatedCableLengths=neuron->NumDendriteMembranePartsVec.GetData();
    else if(s.neuronStructureMode==1)s.allocatedCableLengths.assign(branch?1:attachments.size(),neuron->NumDendriteMembraneParts.GetData());
    NativeProbeBindings b;b.owner=&owner;b.neuron=neuron;b.dataset=dataset;b.branch=branch;b.inputCount=int(attachments.size());
    for(const auto& p:ReadNativeInputParameters(b,attachments))s.inputs.push_back({p.length,p.synapses,p.resistance});
    if(branch)for(std::size_t i=0;i<attachments.size();++i)
    {
        auto synapse=neuron->GetComponentL<NPulseSynapseCommon>(Path(true,i,attachments[i])+".ExcSynapse1",true);
        if(synapse && synapse->Input.GetDataSources().empty())s.allowMutedInputBaseline=true;
    }
    for(int i=0;i<dataset->MatrixData.GetRows();++i)s.initialIsi.push_back(dataset->MatrixData(i,0));
    ValidateNativeModelSpec(s);return s;
}
NativeEditSpec NativeModelEditPolicy(const NativeModelSpec& declaration,const std::vector<InputParameters>& requested)
{
    Require(requested.size()==declaration.inputs.size(),"native_edit_declaration_dimension_mismatch");
    auto policy=declaration.editPolicy;
    if(declaration.scaleInhibitionWithInput)for(auto& rule:policy.inhibition)
    {
        const auto baseline=declaration.inputs.at(rule.inputIndex).resistance;
        Require(baseline && Positive(*baseline),"native_inhibition_reference_resistance_missing");
        rule.resistance=rule.resistance/ *baseline*requested.at(rule.inputIndex).resistance;
        Require(Positive(rule.resistance),"nonfinite_native_inhibition_resistance");
    }
    return policy;
}
const char* NativeModelBuildStatusName(NativeModelBuildStatus s)
{switch(s){case NativeModelBuildStatus::Constructed:return "constructed";case NativeModelBuildStatus::Adopted:return "adopted";
 case NativeModelBuildStatus::Rejected:return "rejected";case NativeModelBuildStatus::Failed:return "failed";}return "unknown";}
}
