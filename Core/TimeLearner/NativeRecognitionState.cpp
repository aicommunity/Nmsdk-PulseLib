#include "NativeRecognitionState.h"
#include "NativeThresholdCandidate.h"
#include "../NPulseChannel.h"
#include "../NPulseSynapse.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>

namespace NMSDK::TimeLearner {
using namespace RDK;
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
bool Positive(double value){return std::isfinite(value) && value>0.;}
QString Text(const std::string& text){return QString::fromStdString(text);}
QJsonObject Object(const QJsonValue& value){Require(value.isObject(),"invalid_recognition_object");return value.toObject();}
QJsonArray Array(const QJsonValue& value){Require(value.isArray(),"invalid_recognition_array");return value.toArray();}
double Number(const QJsonValue& value){Require(value.isDouble() && std::isfinite(value.toDouble()),"invalid_recognition_number");return value.toDouble();}
void Keys(const QJsonObject& object,std::initializer_list<const char*> names)
{Require(object.size()==int(names.size()),"recognition_field_set_mismatch");for(auto name:names)Require(object.contains(name),"missing_recognition_field");}
const char* Mode(RecognitionConversionMode mode)
{
    switch(mode){case RecognitionConversionMode::Identity:return "identity";
    case RecognitionConversionMode::HistoricalBranchN1:return "historical_branch_n1_v1";}
    throw std::invalid_argument("unknown_recognition_conversion");
}
void Validate(const NativeRecognitionSpec& s)
{
    Mode(s.mode);
    Require(s.mode!=RecognitionConversionMode::Identity || s.resistanceBounds.empty(),"identity_conversion_has_unused_bounds");
    Require(s.mode!=RecognitionConversionMode::HistoricalBranchN1 || !s.resistanceBounds.empty(),"scaled_conversion_requires_bounds");
    for(const auto& b:s.resistanceBounds)Require(Positive(b.minimum) && Positive(b.maximum) && b.minimum<=b.maximum,"invalid_recognition_bounds");
}
QJsonArray Parameters(const std::vector<InputParameters>& parameters)
{QJsonArray r;for(const auto& p:parameters)r.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});return r;}
std::string Membrane(bool branch,std::size_t index,int length)
{return "Dendrite"+std::to_string(branch?1:index+1)+"_"+std::to_string(length);}
struct Converted {
    std::unique_ptr<PreparedNativeLearnedState> model;
    QJsonObject record,identity;
};
Converted Convert(const QJsonObject& source,const NativeRecognitionSpec& spec,UEPtr<UStorage> storage,
                  UEPtr<UEnvironment> environment,const std::string& implementation)
{
    Validate(spec);
    Require(source["kind"]==QJsonValue("timelearner_completed_state") && source["state_kind"]==QJsonValue("learned_solo"),
            "recognition_requires_completed_solo");
    Converted converted;converted.model=PrepareNativeLearnedState(source,storage,environment,implementation);
    auto& m=*converted.model;const auto& declaration=m.configuration.model;const auto n=m.parameters.size();
    auto generator=m.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(generator),"missing_recognition_source");
    std::vector<double> factors(n,1.);
    if(spec.mode==RecognitionConversionMode::HistoricalBranchN1)
    {
        Require(declaration.branch && declaration.normalization==Normalization::TipResistance,"historical_conversion_requires_branch_n1");
        Require(spec.resistanceBounds.size()==n,"recognition_bounds_dimension_mismatch");
        const auto& references=m.service.References();Require(bool(references) && references->Inputs().size()==n,"recognition_missing_references");
        if(n>1)
        {
            double mean=0.;
            for(std::size_t i=0;i<n;++i)
            {
                const auto amplitude=references->Inputs()[i].rawPotential;
                Require(Positive(amplitude),"historical_conversion_requires_positive_references");
                if(i+1<n)mean+=amplitude/double(n-1);
            }
            Require(Positive(mean),"nonfinite_recognition_reference_mean");
            for(std::size_t i=0;i<n;++i)
            {
                factors[i]=double(n)*(references->Inputs()[i].rawPotential/mean);
                Require(Positive(factors[i]),"nonfinite_recognition_factor");
            }
        }
    }
    struct Bank { UEPtr<NPulseSynapseCommon> synapse;double resistance; };
    std::map<std::string,Bank> banks;QJsonArray decisions,activation;
    for(std::size_t i=0;i<n;++i)
    {
        const auto original=m.parameters[i];double low=original.resistance,high=original.resistance,applied=original.resistance;
        const double requested=original.resistance*factors[i];Require(Positive(requested),"nonfinite_recognition_resistance");
        const auto path=Membrane(declaration.branch,i,original.length);
        auto membrane=m.neuron->GetComponentL<NPulseMembrane>(path,true);Require(bool(membrane),"missing_recognition_membrane");
        if(spec.mode==RecognitionConversionMode::HistoricalBranchN1)
        {
            Require(membrane->GetNumPosChannels()>0,"missing_recognition_channel");
            auto channel=dynamic_cast<NPulseChannel*>(membrane->GetPosChannel(0));Require(channel,"unsupported_recognition_channel");
            const double rm=channel->RestingResistance.GetData(),ratio=declaration.editPolicy.maximumResistanceRatio;
            Require(Positive(rm) && Positive(ratio) && ratio>=1.,"invalid_recognition_native_bounds");
            low=std::max({spec.resistanceBounds[i].minimum,rm/ratio,declaration.editPolicy.configuredMinimumResistance});
            high=std::min(spec.resistanceBounds[i].maximum,rm*ratio);
            Require(Positive(low) && Positive(high) && low<=high,"empty_recognition_resistance_interval");
            if(n==1)Require(original.resistance>=low && original.resistance<=high,"single_input_conversion_would_change_resistance");
            applied=std::clamp(requested,low,high);
        }
        for(int k=1;k<=original.synapses;++k)
        {
            auto synapse=membrane->GetComponentL<NPulseSynapseCommon>("ExcSynapse"+std::to_string(k),true);
            Require(bool(synapse),"missing_recognition_synapse");const auto name=synapse->GetLongName(m.neuron.Get());
            auto [found,inserted]=banks.emplace(name,Bank{synapse,applied});
            Require(inserted || found->second.resistance==applied,"shared_recognition_bank_conflict");
        }
        decisions.append(QJsonObject{{"input_index",double(i)},{"original_resistance",original.resistance},
            {"factor",factors[i]},{"requested_resistance",requested},{"minimum_resistance",low},{"maximum_resistance",high},
            {"applied_resistance",applied},{"clipped",applied!=requested}});
        m.parameters[i].resistance=applied;
    }
    for(auto& [path,bank]:banks)
    {
        auto& s=*bank.synapse;
        if(auto input=dynamic_cast<UIPropertyInput*>(&s.Resistance))
            Require(!input->IsConnected() && !input->UsesExternalDataSource(),"connected_recognition_resistance");
        const auto sources=s.Input.GetDataSources();
        Require(!s.Input.UsesExternalDataSource(),"external_recognition_input_storage");
        if(sources.empty())
        {
            int slot=0;generator->Connect(&s,"Output","Input",slot,true);activation.append(Text(path));
        }
        else Require(sources.size()==1 && sources.front()==&generator->Output,"recognition_would_replace_unowned_input");
        if(s.Resistance.GetData()!=bank.resistance)s.Resistance=bank.resistance;
        const auto actual=s.Input.GetDataSources();
        Require(actual.size()==1 && actual.front()==&generator->Output && s.Resistance.GetData()==bank.resistance,"recognition_effective_readback_mismatch");
    }
    const auto integration=AssessNativeIntegration(m.neuron,generator->Amplitude.GetData());
    Require(integration.valid,integration.reason.c_str());
    NativeProbeBindings bindings;bindings.neuron=m.neuron;bindings.branch=declaration.branch;bindings.inputCount=int(n);
    std::vector<int> positions;for(const auto& p:m.parameters)positions.push_back(p.length);
    Require(ReadNativeInputParameters(bindings,positions)==m.parameters,"recognition_parameter_graph_mismatch");
    converted.identity=NmsdkTools::IdentifyModel(m.neuron,{{&generator->Output,"burst"}});
    Require(converted.identity["valid"].toBool(),"invalid_recognition_model_identity");
    converted.record={{"schema_version",1},{"algorithm",Mode(spec.mode)},{"source_solo_sha256",source["sha256"]},
        {"source_solo_identity",source["model_revision"]},{"result_identity",converted.identity["sha256"]},
        {"decisions",decisions},{"activated_synapses",activation},{"inhibition_policy","preserve_source"}};
    return converted;
}
QJsonObject Metadata(const QJsonObject& source,const Converted& c)
{
    auto metadata=Object(Object(source["neuron"])["metadata"]);metadata["state_kind"]="recognition";
    metadata["recognition_schema_version"]=1;
    auto training=Object(metadata["training"]);training["scope"]="source_solo";metadata["training"]=training;
    auto calibration=Object(metadata["calibration"]);calibration["protocol"]="fixed-inherited-from-solo";
    calibration["source_solo_sha256"]=source["sha256"];metadata["calibration"]=calibration;
    QJsonArray factors;for(const auto& d:c.record["decisions"].toArray())factors.append(d.toObject()["factor"]);
    metadata["conversion"]=QJsonObject{{"mode","parallel"},{"source_solo_identity",source["model_revision"]},
        {"source_solo_sha256",source["sha256"]},{"factors",factors},{"record",c.record}};
    metadata["recognition_verification"]=QJsonObject{{"status","unverified"},{"model_identity",c.identity["sha256"]},
        {"certificate",QJsonValue(QJsonValue::Null)}};
    return metadata;
}
}
QJsonObject EncodeNativeRecognitionSpec(const NativeRecognitionSpec& s)
{
    Validate(s);QJsonArray bounds;for(const auto& b:s.resistanceBounds)bounds.append(QJsonObject{{"minimum",b.minimum},{"maximum",b.maximum}});
    return {{"schema_version",1},{"mode",Mode(s.mode)},{"resistance_bounds",bounds}};
}
NativeRecognitionSpec DecodeNativeRecognitionSpec(const QJsonObject& o)
{
    Keys(o,{"schema_version","mode","resistance_bounds"});Require(o["schema_version"]==QJsonValue(1),"unsupported_recognition_spec_schema");
    NativeRecognitionSpec s;const auto mode=o["mode"];
    if(mode==QJsonValue("identity"))s.mode=RecognitionConversionMode::Identity;
    else if(mode==QJsonValue("historical_branch_n1_v1"))s.mode=RecognitionConversionMode::HistoricalBranchN1;
    else throw std::invalid_argument("unknown_recognition_conversion");
    for(const auto& value:Array(o["resistance_bounds"]))
    {const auto b=Object(value);Keys(b,{"minimum","maximum"});s.resistanceBounds.push_back({Number(b["minimum"]),Number(b["maximum"])});}
    Validate(s);return s;
}
QJsonObject ConvertNativeSoloToRecognition(const QJsonObject& source,const NativeRecognitionSpec& spec,
    UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const std::string& implementation)
{
    auto c=Convert(source,spec,storage,environment,implementation);auto& m=*c.model;
    auto generator=m.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const auto dataset=Object(source["dataset"]);std::vector<std::string> roots;
    for(const auto& name:Array(dataset["required_class_names"]))roots.push_back(name.toString().toStdString());
    const auto archive=NmsdkTools::ExportModelArchive(m.neuron,Metadata(source,c),{{&generator->Output,"burst"}},roots);
    QJsonObject result{{"kind","timelearner_recognition_state"},{"schema_version",1},{"dynamic_checkpoint",false},
        {"state_kind","recognition"},{"implementation_id",Text(implementation)},{"source_solo",source},
        {"conversion_spec",EncodeNativeRecognitionSpec(spec)},{"conversion_record",c.record},
        {"recognition_inputs",Parameters(m.parameters)},{"model_revision",c.identity["sha256"]},
        {"recognition_certificate",QJsonValue(QJsonValue::Null)},{"dataset",dataset},{"neuron",archive}};
    result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
}
QJsonObject ParseNativeRecognitionState(const std::string& text)
{return ParseStrictNativeJsonObject(text,256*1024*1024);}
PreparedNativeRecognitionState::~PreparedNativeRecognitionState()
{
    if(neuron && !neuron->GetOwner()){neuron->BreakLinks();storage->ReturnObject(neuron);}
    if(dataset && !dataset->GetOwner()){dataset->BreakLinks();storage->ReturnObject(dataset);}
}
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeRecognitionState(const QJsonObject& package,
    UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const std::string& implementation)
{
    if(package["kind"]==QJsonValue("timelearner_recognition_threshold_candidate"))
        return PrepareNativeThresholdCandidate(package,storage,environment,implementation);
    Keys(package,{"kind","schema_version","dynamic_checkpoint","state_kind","implementation_id","source_solo",
        "conversion_spec","conversion_record","recognition_inputs","model_revision","recognition_certificate","dataset","neuron","sha256"});
    Require(package["kind"]==QJsonValue("timelearner_recognition_state") && package["schema_version"]==QJsonValue(1) &&
        package["state_kind"]==QJsonValue("recognition") && package["dynamic_checkpoint"]==QJsonValue(false) &&
        package["recognition_certificate"].isNull(),"unsupported_recognition_state_claim");
    auto body=package;body.remove("sha256");
    Require(package["sha256"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(body)),"recognition_digest_mismatch");
    Require(!implementation.empty() && package["implementation_id"]==QJsonValue(Text(implementation)),"recognition_implementation_mismatch");
    const auto source=Object(package["source_solo"]);
    auto expected=Convert(source,DecodeNativeRecognitionSpec(Object(package["conversion_spec"])),storage,environment,implementation);
    Require(package["conversion_record"]==QJsonValue(expected.record) && package["recognition_inputs"]==QJsonValue(Parameters(expected.model->parameters)) &&
        package["model_revision"]==expected.identity["sha256"] && package["dataset"]==source["dataset"],"recognition_conversion_mismatch");
    const auto archive=Object(package["neuron"]);
    Require(archive["metadata"]==QJsonValue(Metadata(source,expected)) && Object(archive["graph"])["identity"]==QJsonValue(expected.identity),
        "recognition_archive_context_mismatch");
    auto& m=*expected.model;auto generator=m.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    m.neuron->BreakLinks();storage->ReturnObject(m.neuron);m.neuron=0;
    m.neuron=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(archive,storage,environment,Text(implementation),{{&generator->Output,"burst"}}));
    Require(bool(m.neuron),"recognition_archive_not_neuron");
    auto result=std::make_unique<PreparedNativeRecognitionState>();result->storage=storage;
    result->neuron=m.neuron;m.neuron=0;result->dataset=m.dataset;m.dataset=0;result->parameters=m.parameters;
    result->modelRevision=package["model_revision"].toString().toStdString();result->sourceSoloRevision=source["model_revision"].toString().toStdString();
    result->configuration=std::move(m.configuration);
    // Recognition has every declared input connected; muted solo adoption is
    // no longer an allowed effective graph for this state.
    result->configuration.model.allowMutedInputBaseline=false;
    result->configurationText=m.configurationText;
    result->controllerVersion=m.controllerVersion;result->calculateMode=m.calculateMode;
    result->structureBuildMode=m.structureBuildMode;
    result->conversionRecord=expected.record;return result;
}
}
