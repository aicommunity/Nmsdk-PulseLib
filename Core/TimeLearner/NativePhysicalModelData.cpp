#include "NativePhysicalModelData.h"
#include "NativeRecognitionState.h"
#include "../NPulseLTZoneCommon.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool value,const char* reason)
{if(!value)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& value)
{Require(value.isObject(),"physical_model_object_required");return value.toObject();}
void Keys(const QJsonObject& object,std::initializer_list<const char*> fields)
{
    Require(object.size()==int(fields.size()),"physical_model_field_set");
    for(const auto name:fields)Require(object.contains(name),"physical_model_field_set");
}
std::string Text(const QJsonValue& value)
{
    Require(value.isString() && !value.toString().trimmed().isEmpty(),"physical_model_text_required");
    return value.toString().toStdString();
}
bool Sha(const QJsonValue& value)
{
    if(!value.isString() || value.toString().size()!=64)return false;
    const auto text=value.toString();return std::all_of(text.begin(),text.end(),[](QChar c) {
        return (c>='0' && c<='9') || (c>='a' && c<='f');
    });
}
int Integer(const QJsonValue& value,int low,int high)
{
    Require(value.isDouble(),"physical_model_integer_required");const auto number=value.toDouble();
    Require(std::isfinite(number) && number>=low && number<=high && std::floor(number)==number,
        "physical_model_integer_required");return int(number);
}
QJsonObject Signed(QJsonObject document)
{
    document.remove("sha256");document["sha256"]=NmsdkTools::CanonicalModelDataSha256(document);return document;
}
bool SoloKind(const QString& kind)
{return kind=="timelearner_completed_state" || kind=="timelearner_stopped_state";}
bool DiagnosticKind(const QString& kind)
{return kind=="timelearner_diagnostic_recognition_state" || kind=="timelearner_diagnostic_threshold_candidate";}
QJsonObject SourceSolo(const QJsonObject& source,unsigned depth=0)
{
    Require(depth<=3,"physical_model_source_nesting");const auto kind=source["kind"].toString();
    if(SoloKind(kind))return source;
    if(kind=="timelearner_recognition_state" || kind=="timelearner_diagnostic_recognition_state")
        return SourceSolo(Object(source["source_solo"]),depth+1);
    if(kind=="timelearner_recognition_threshold_candidate" || kind=="timelearner_diagnostic_threshold_candidate")
        return SourceSolo(Object(source["source_recognition"]),depth+1);
    if(kind=="timelearner_calibrated_recognition_state")
        return SourceSolo(Object(source["source_candidate"]),depth+1);
    throw std::invalid_argument("physical_model_source_kind");
}
QJsonArray Parameters(const std::vector<InputParameters>& parameters)
{
    QJsonArray result;for(const auto& p:parameters)
        result.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});
    return result;
}
std::vector<InputParameters> Parameters(const QJsonValue& value,const NativeModelSpec& model)
{
    Require(value.isArray() && value.toArray().size()==int(model.inputs.size()),"physical_model_input_dimension");
    std::vector<InputParameters> result;
    for(const auto& item:value.toArray())
    {
        const auto object=Object(item);Keys(object,{"length","synapses","resistance"});
        const auto resistance=object["resistance"];
        Require(resistance.isDouble() && std::isfinite(resistance.toDouble()) && resistance.toDouble()>0.,
            "physical_model_input_resistance");
        const int length=Integer(object["length"],1,model.editPolicy.maximumCableLength);
        Require(!model.branch || result.empty() || result.back().length>=length,"physical_model_attachment_order");
        result.push_back({length,Integer(object["synapses"],1,128),resistance.toDouble()});
    }
    return result;
}
void Frequency(UEPtr<UContainer> node,unsigned expected)
{
    Require(node && node->IsReady() && node->IsInit() && node->GetTimeStep()==expected,
        "physical_model_frequency_or_lifecycle");
    std::vector<NameT> names;node->GetComponentsList(names);
    for(const auto& name:names)Frequency(node->GetComponent(name,true),expected);
}
void CheckPhysical(PreparedNativePhysicalModelData& result)
{
    const auto& configuration=result.configuration;const auto& model=configuration.model;
    Frequency(result.neuron,model.frequencyHz);Frequency(result.dataset,model.frequencyHz);
    auto generator=result.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"physical_model_generator_missing");
    const StimulusProfile actual{generator->GetCompClassName(),generator->Amplitude.GetData(),generator->PulseLength.GetData(),
        double(generator->GetTimeStep()),generator->FrequencyDeviation.GetData(),
        generator->UseTransitSignal.GetData(),generator->UsePatternOutput.GetData()};
    Require(actual==configuration.stimulusProfile,"physical_model_stimulus_mismatch");
    Require(ValidNativeTarget(result.dataset->MatrixData.GetData(),model.inputs.size()),"physical_model_target_dimension");
    for(std::size_t i=0;i<model.inputs.size();++i)
        Require(result.dataset->MatrixData(int(i),0)==model.initialIsi.at(i),"physical_model_target_mismatch");
    std::vector<int> attachments;for(const auto& input:result.parameters)attachments.push_back(input.length);
    const auto inspected=InspectNativePreparedChildren(model,result.neuron,result.dataset,attachments);
    Require(inspected.status==NativeModelBuildStatus::Adopted && inspected.effective==result.parameters,
        "physical_model_effective_inputs_mismatch");
    Require(inspected.modelRevision==result.modelRevision,
        "physical_model_effective_graph_mismatch");
}
template<class Prepared> QJsonObject Capture(const QJsonObject& source,const QJsonObject& solo,
    const Prepared& prepared,const std::string& soloRevision,const QJsonValue& conversion)
{
    Require(Sha(source["sha256"]) && source["dynamic_checkpoint"]==QJsonValue(false),"physical_model_source_boundary");
    const QJsonObject boundary{{"kind",source["kind"]},{"schema_version",source["schema_version"]},
        {"state_kind",source["state_kind"]},{"sha256",source["sha256"]},{"model_revision",source["model_revision"]}};
    return Signed({{"kind","timelearner_physical_model_data"},{"schema_version",1},{"data_only",true},
        {"dynamic_checkpoint",false},{"certificate",QJsonValue(QJsonValue::Null)},
        {"implementation_id",source["implementation_id"]},{"source_boundary",boundary},
        {"configuration",EncodeConfiguredNativeTraining(prepared.configuration)},
        {"configuration_text",QString::fromStdString(prepared.configurationText)},
        {"protocol",solo["protocol"]},{"controls",QJsonObject{{"controller_version",prepared.controllerVersion},
            {"calculate_mode",prepared.calculateMode},{"structure_build_mode",prepared.structureBuildMode}}},
        {"parameters",Parameters(prepared.parameters)},{"model_revision",QString::fromStdString(prepared.modelRevision)},
        {"source_solo_revision",QString::fromStdString(soloRevision)},{"conversion_record",conversion},
        {"dataset",source["dataset"]},{"neuron",source["neuron"]}});
}
}

PreparedNativePhysicalModelData::~PreparedNativePhysicalModelData()
{
    if(neuron && !neuron->GetOwner()){neuron->BreakLinks();storage->ReturnObject(neuron);}
    if(dataset && !dataset->GetOwner()){dataset->BreakLinks();storage->ReturnObject(dataset);}
}
unsigned NativePhysicalModelSourceFrequency(const QJsonObject& source)
{return DecodeConfiguredNativeTraining(Object(SourceSolo(source)["configuration"])).model.frequencyHz;}
QJsonObject ExportNativePhysicalModelData(const QJsonObject& source,UEPtr<UStorage> storage,
    UEPtr<UEnvironment> environment,const std::string& implementation)
{
    Require(storage && environment && !implementation.empty(),"physical_model_import_context");
    const auto solo=SourceSolo(source);const auto kind=source["kind"].toString();
    if(SoloKind(kind))
    {
        const auto prepared=PrepareNativeLearnedState(source,storage,environment,implementation);
        return Capture(source,solo,*prepared,prepared->modelRevision,QJsonValue(QJsonValue::Null));
    }
    const auto prepared=DiagnosticKind(kind)?PrepareNativeDiagnosticRecognitionState(source,storage,environment,implementation):
        PrepareNativeRecognitionState(source,storage,environment,implementation);
    return Capture(source,solo,*prepared,prepared->sourceSoloRevision,prepared->conversionRecord);
}

std::unique_ptr<PreparedNativePhysicalModelData> PrepareNativePhysicalModelData(const QJsonObject& document,
    UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const std::string& implementation)
{
    Require(storage && environment && !implementation.empty(),"physical_model_import_context");
    Keys(document,{"kind","schema_version","data_only","dynamic_checkpoint","certificate","implementation_id",
        "source_boundary","configuration","configuration_text","protocol","controls","parameters","model_revision",
        "source_solo_revision","conversion_record","dataset","neuron","sha256"});
    Require(document["kind"]==QJsonValue("timelearner_physical_model_data") && document["schema_version"]==QJsonValue(1) &&
        document["data_only"]==QJsonValue(true) && document["dynamic_checkpoint"]==QJsonValue(false) && document["certificate"].isNull(),
        "physical_model_data_not_a_certificate_or_checkpoint");
    Require(Sha(document["sha256"]) && Signed(document)==document,"physical_model_document_digest");
    Require(document["implementation_id"]==QJsonValue(QString::fromStdString(implementation)),"physical_model_implementation");
    const auto boundary=Object(document["source_boundary"]);
    Keys(boundary,{"kind","schema_version","state_kind","sha256","model_revision"});
    Require(Sha(boundary["sha256"]) && Sha(boundary["model_revision"]) && boundary["model_revision"]==document["model_revision"] &&
        Sha(document["source_solo_revision"]),"physical_model_source_identity");
    const auto sourceKind=QString::fromStdString(Text(boundary["kind"]));
    const bool solo=SoloKind(sourceKind);
    Require(solo || sourceKind=="timelearner_recognition_state" || DiagnosticKind(sourceKind) ||
        sourceKind=="timelearner_recognition_threshold_candidate" || sourceKind=="timelearner_calibrated_recognition_state",
        "physical_model_source_kind");
    const char* expectedState=sourceKind=="timelearner_completed_state"?"learned_solo":
        sourceKind=="timelearner_stopped_state"?"diagnostic":DiagnosticKind(sourceKind)?"diagnostic_recognition":"recognition";
    Require(boundary["state_kind"]==QJsonValue(expectedState),"physical_model_source_state");
    Require(Integer(boundary["schema_version"],1,7)>=1,"physical_model_source_schema");
    auto result=std::make_unique<PreparedNativePhysicalModelData>();result->storage=storage;
    result->sourceBoundary=boundary;result->configuration=DecodeConfiguredNativeTraining(Object(document["configuration"]));
    result->configurationText=Text(document["configuration_text"]);result->protocol=Object(document["protocol"]);
    const auto protocol=DecodeVerificationProtocol(result->protocol);
    QJsonArray target;for(double value:result->configuration.model.initialIsi)target.append(value);
    Require(protocol.inputs.size()==result->configuration.model.inputs.size() &&
        protocol.routing==(result->configuration.model.branch?ProbeRouting::BranchSoloBurst:ProbeRouting::ClassicBurst) &&
        protocol.stimulusProfile==std::optional<StimulusProfile>(result->configuration.stimulusProfile) &&
        protocol.patternHash==NmsdkTools::CanonicalModelDataSha256(target).toStdString() &&
        protocol.sampleId==result->configuration.sampleId && protocol.revision==result->configuration.protocolRevision &&
        protocol.minimumAmplitude==result->configuration.minimumAmplitude &&
        protocol.maximumSampleGap==result->configuration.maximumSampleGap,"physical_model_protocol_scope");
    const auto controls=Object(document["controls"]);Keys(controls,{"controller_version","calculate_mode","structure_build_mode"});
    result->controllerVersion=Integer(controls["controller_version"],1,2);
    result->calculateMode=Integer(controls["calculate_mode"],0,1);result->structureBuildMode=Integer(controls["structure_build_mode"],0,1);
    result->parameters=Parameters(document["parameters"],result->configuration.model);
    result->modelRevision=Text(document["model_revision"]);result->sourceSoloRevision=Text(document["source_solo_revision"]);
    Require(solo?document["conversion_record"].isNull():document["conversion_record"].isObject(),"physical_model_conversion_scope");
    if(solo)Require(result->sourceSoloRevision==result->modelRevision,"physical_model_solo_revision");
    const auto archive=Object(document["neuron"]),dataset=Object(document["dataset"]);
    const auto metadata=Object(archive["metadata"]);
    Require(metadata["state_kind"]==boundary["state_kind"],"physical_model_state_scope");
    const auto training=Object(metadata["training"]);
    Require(training["trainer"]==QJsonValue(result->configuration.model.branch?"Branch":"Classic") &&
        training["normalization"]==QJsonValue(result->configuration.model.normalization==Normalization::SynapseCount?"N0":"N1") &&
        training["target_pattern"]==QJsonValue(target) && training["target_id"]==QJsonValue(QString::fromStdString(protocol.sampleId)),
        "physical_model_metadata_scope");
    Require(Object(archive["graph"])["identity"].toObject()["sha256"]==document["model_revision"],"physical_model_archive_identity");
    NmsdkTools::PrepareModelArchive(archive,storage,environment,QString::fromStdString(implementation));
    result->dataset=dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(dataset,storage,environment));
    Require(bool(result->dataset),"physical_model_dataset_class");
    auto generator=result->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"physical_model_generator_missing");
    result->neuron=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(archive,storage,environment,
        QString::fromStdString(implementation),{{&generator->Output,"burst"}}));
    Require(bool(result->neuron),"physical_model_neuron_class");
    CheckPhysical(*result);return result;
}
std::unique_ptr<PreparedNativePhysicalModelData> PrepareNativeFrozenPhysicalModelData(const QJsonObject& document,
    UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const std::string& implementation)
{
    const auto boundary=Object(document["source_boundary"]);
    Require(!SoloKind(boundary["kind"].toString()),"physical_model_frozen_requires_explicit_recognition_conversion");
    auto prepared=PrepareNativePhysicalModelData(document,storage,environment,implementation);
    Require(!prepared->configuration.model.allowMutedInputBaseline,
        "physical_model_frozen_requires_all_input_banks");
    // Metadata is not authority, but it must agree with the actual frozen
    // decision element. Do not apply a parent threshold overlay to repair it.
    const QJsonValue threshold=Object(Object(document["neuron"])["metadata"])["calibration"].toObject()["threshold"];
    auto zone=prepared->neuron->GetComponentL<NPulseLTZoneCommon>("LTZone",true);
    Require(zone && threshold.isDouble() && std::isfinite(threshold.toDouble()) &&
        zone->Threshold.GetData()==threshold.toDouble(),"physical_model_frozen_threshold_metadata_mismatch");
    return prepared;
}
}
