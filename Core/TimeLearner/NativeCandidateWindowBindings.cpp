#include "NativeCandidateWindowBindings.h"
#include "NativeModelBuilder.h"
#include "../NPulseNeuron.h"
#include "../NPulseMembrane.h"
#include "../NPulseChannelCommon.h"
#include "../NPulseSynapseCommon.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}
QString Text(const std::string& s){return QString::fromStdString(s);}
std::string Bank(bool branch,std::size_t input,int attachment) {
    return "Dendrite"+std::to_string(branch?1:input+1)+"_"+std::to_string(attachment);
}
void Context(const NativeProbeBindings& b,const VerificationProtocol& p,const NativeModelSpec& s) {
    ValidateVerificationProtocol(p);ValidateNativeModelSpec(s);
    Require((p.schemaVersion==4 || p.schemaVersion==5) && p.inputs.size()>=2 && b.owner && b.neuron && b.dataset &&
        b.inputCount>0 && std::size_t(b.inputCount)==p.inputs.size() && s.inputs.size()==p.inputs.size() &&
        b.branch==(p.routing==ProbeRouting::BranchSoloBurst) && b.branch==s.branch && b.routingComplete &&
        b.owner->IsReady() && b.owner->IsInit() && b.neuron->IsReady() && b.neuron->IsInit() && b.dataset->IsReady() && b.dataset->IsInit(),
        "candidate_window_actual_owner_or_routing");
}
}
CandidateWindowPhysicalReadback ReadNativeCandidateWindowPhysical(const NativeProbeBindings& b,const std::vector<int>& attachments,
    const VerificationProtocol& p,const NativeModelSpec& s,const std::optional<TrainingCapacitance>& capacitance) {
    Context(b,p,s);ValidateTrainingCapacitance(capacitance);
    Require(attachments.size()==p.inputs.size(),"candidate_window_actual_attachment_dimensions");
    CandidateWindowPhysicalReadback result;result.parameters=ReadNativeInputParameters(b,attachments);result.capacitance=capacitance;
    const auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"candidate_window_actual_source_missing");
    for(std::size_t i=0;i<attachments.size();++i) {
        const auto& value=result.parameters[i];Require(value.length<=s.editPolicy.maximumCableLength && value.synapses<=128 &&
            (!b.branch || !i || attachments[i-1]>=attachments[i]),"candidate_window_actual_attachment_scope");
        const auto path=Bank(b.branch,i,value.length);const auto membrane=b.neuron->GetComponentL<NPulseMembrane>(path,true);
        Require(bool(membrane),"candidate_window_actual_bank_missing");
        const auto channel=membrane->GetComponentL<NPulseChannelCommon>("ExcChannel",true);
        Require(channel && membrane->GetNumPosChannels()>0 && membrane->GetPosChannel(0)==channel.Get() &&
            channel->SynapticInputs.CanInspectDataSources(),"candidate_window_actual_receiver_missing");
        const auto receivers=channel->SynapticInputs.GetDataSources();std::optional<bool> connected;
        CandidateWindowInputBankReadback bank;bank.inputOrdinal=i;bank.inputId=p.inputs[i].id;bank.stimulusId=p.inputs[i].stimulusId;
        bank.bankPath="$."+path;bank.receiverPath=bank.bankPath+".ExcChannel";bank.attachmentSegment=std::size_t(value.length);
        for(int k=1;k<=value.synapses;++k) {
            const auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
            Require(synapse && synapse->Input.CanInspectDataSources() && !synapse->Input.UsesExternalDataSource() &&
                std::find(receivers.begin(),receivers.end(),&synapse->Output)!=receivers.end(),"candidate_window_actual_receiver_membership");
            const auto links=synapse->Input.GetDataSources();const bool active=links.size()==1 && links.front()==&source->Output;
            Require(active || (s.allowMutedInputBaseline && links.empty()),"candidate_window_actual_source_binding");
            Require(!connected || *connected==active,"candidate_window_actual_partial_bank");connected=active;
            bank.excitationSynapsePaths.push_back(bank.bankPath+".ExcSynapse"+std::to_string(k));
        }
        result.inputBanks.push_back(std::move(bank));
    }
    return result;
}
QJsonObject EncodeCandidateWindowPhysicalReadback(const CandidateWindowPhysicalReadback& r) {
    Require(!r.parameters.empty() && r.parameters.size()==r.inputBanks.size(),"candidate_window_actual_readback_dimensions");
    QJsonArray parameters,banks;for(const auto& p:r.parameters) {
        Require(p.length>0 && p.synapses>0 && std::isfinite(p.resistance) && p.resistance>0.,"candidate_window_actual_readback_parameter");
        parameters.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});
    }
    for(const auto& b:r.inputBanks) {
        Require(b.inputOrdinal<=CandidateWindowMaximumExactCount && b.attachmentSegment<=CandidateWindowMaximumExactCount,"candidate_window_actual_readback_count");
        QJsonArray members;for(const auto& path:b.excitationSynapsePaths)members.append(Text(path));
        banks.append(QJsonObject{{"input_ordinal",double(b.inputOrdinal)},{"input_id",Text(b.inputId)},{"stimulus_id",Text(b.stimulusId)},
            {"receiver_path",Text(b.receiverPath)},{"bank_path",Text(b.bankPath)},{"attachment_segment",double(b.attachmentSegment)},{"excitation_synapse_paths",members}});
    }
    QJsonValue capacitance(QJsonValue::Null);ValidateTrainingCapacitance(r.capacitance);
    if(r.capacitance) {
        QJsonObject scales;for(const auto& [path,scale]:r.capacitance->membraneScales)scales[Text(path)]=scale;
        capacitance=QJsonObject{{"origin",Text(r.capacitance->origin)},{"maximum_relative_change",r.capacitance->maximumRelativeChange},{"membrane_scales",scales}};
    }
    return {{"parameters",parameters},{"input_banks",banks},{"capacitance",capacitance}};
}
void ValidateNativeCandidateWindowPreparedSelector(const NativeProbeBindings& b,const CandidateWindowPhysicalReadback& expected,
    const VerificationProtocol& p,const NativeModelSpec& s,std::size_t assigned,const PreparedVerificationProbe& prepared) {
    Context(b,p,s);Require(assigned<p.inputs.size() && expected.parameters.size()==p.inputs.size() && expected.inputBanks.size()==p.inputs.size() &&
        prepared.routing==p.routing && prepared.inputId==p.inputs[assigned].id && prepared.protocolRevision==p.revision,
        "candidate_window_actual_prepared_selector_context");
    if(!b.branch) {Require(!prepared.selectedInputIndex,"candidate_window_classic_selected_input");return;}
    Require(prepared.selectedInputIndex==assigned,"candidate_window_actual_selected_ordinal");
    std::vector<NPulseSynapseCommon*> wanted;const auto& value=expected.parameters[assigned];const auto path=Bank(true,assigned,value.length);
    Require(expected.inputBanks[assigned].inputOrdinal==assigned && expected.inputBanks[assigned].bankPath=="$."+path,
        "candidate_window_actual_selected_bank");
    for(int k=1;k<=value.synapses;++k) {
        const auto synapse=b.neuron->GetComponentL<NPulseSynapseCommon>(path+".ExcSynapse"+std::to_string(k),true);
        Require(bool(synapse),"candidate_window_actual_selected_member");wanted.push_back(synapse.Get());
    }
    for(const auto& rule:s.editPolicy.inhibition)if(rule.inputIndex==assigned) {
        const long long position=static_cast<long long>(value.length)+rule.offset;
        Require(position>0 && position<=s.editPolicy.maximumCableLength,"candidate_window_actual_inhibition_attachment");
        const auto synapse=b.neuron->GetComponentL<NPulseSynapseCommon>(Bank(true,assigned,int(position))+".InhSynapse1",true);
        Require(bool(synapse),"candidate_window_actual_selected_inhibition");wanted.push_back(synapse.Get());
    }
    Require(b.soloSynapses==wanted,"candidate_window_actual_selector_bank_membership");
    const auto source=b.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(source),"candidate_window_actual_source_missing");
    for(std::size_t i=0;i<expected.parameters.size();++i)for(int k=1;k<=expected.parameters[i].synapses;++k) {
        const auto synapse=b.neuron->GetComponentL<NPulseSynapseCommon>(Bank(true,i,expected.parameters[i].length)+".ExcSynapse"+std::to_string(k),true);
        Require(bool(synapse),"candidate_window_actual_selector_member_missing");const auto links=synapse->Input.GetDataSources();
        const bool selected=std::find(wanted.begin(),wanted.end(),synapse.Get())!=wanted.end();
        Require(selected?(links.size()==1 && links.front()==&source->Output):links.empty(),"candidate_window_actual_onehot_binding");
    }
}
}
