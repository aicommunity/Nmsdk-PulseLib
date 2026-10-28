#include "NativeLearnedState.h"
#include "NativeTrainingProof.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool c,const char* reason){if(!c)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& v){Require(v.isObject(),"invalid_learned_state_object");return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray(),"invalid_learned_state_array");return v.toArray();}
std::size_t Count(const QJsonValue& v)
{Require(v.isDouble(),"invalid_learned_state_integer");const auto n=v.toDouble();Require(std::isfinite(n) && n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n,"invalid_learned_state_integer");return std::size_t(n);}
QJsonValue Integer(std::size_t n){Require(n<=9007199254740991ULL,"inexact_learned_state_integer");return double(n);}
QString Text(const std::string& value){return QString::fromStdString(value);}
QJsonArray Packets(const std::vector<NativeProbeResult>& packets)
{QJsonArray r;for(const auto& p:packets)r.append(EncodeNativeProbeEvidence(p));return r;}
std::vector<NativeProbeResult> Packets(const QJsonValue& value)
{std::vector<NativeProbeResult> r;for(const auto& p:Array(value))r.push_back(DecodeNativeProbeEvidence(Object(p)));return r;}
NativeTrainingPlan Plan(const ConfiguredNativeTraining& c,const VerificationProtocol& protocol)
{
    auto plan=MakeConfiguredNativeTrainingPlan(c);const auto& canonical=plan.protocol;
    Require(protocol.schemaVersion==3 && protocol.routing==canonical.routing && protocol.revision==canonical.revision &&
        protocol.sampleId==canonical.sampleId && protocol.patternHash==canonical.patternHash && protocol.inputs.size()==canonical.inputs.size() &&
        protocol.stimulusProfile==canonical.stimulusProfile && protocol.maximumSampleGap==canonical.maximumSampleGap &&
        protocol.minimumAmplitude==canonical.minimumAmplitude && protocol.waveAssignment==canonical.waveAssignment && protocol.peakSelection==canonical.peakSelection,
        "learned_state_protocol_configuration_mismatch");
    plan.protocol=protocol;plan.probe.specification.expectedStimulusIds=protocol.stimulusIds;
    for(std::size_t i=0;i<protocol.inputs.size();++i)
    {plan.inputWindows[i].inputId=protocol.inputs[i].id;plan.inputWindows[i].stimulusId=protocol.inputs[i].stimulusId;}
    return plan;
}
ConfiguredNativeTraining Configuration(const NativeModelSpec& declaration,const NativeTrainingService& service)
{
    const auto& plan=service.Plan();ConfiguredNativeTraining c;c.model=declaration;
    c.runId=plan.search.runId;c.protocolRevision=plan.protocol.revision;c.sampleId=plan.protocol.sampleId;
    Require(bool(plan.protocol.stimulusProfile),"learned_state_missing_stimulus_profile");c.stimulusProfile=*plan.protocol.stimulusProfile;
    c.duration=plan.probe.specification.end;c.maximumSampleGap=plan.protocol.maximumSampleGap;c.minimumAmplitude=plan.protocol.minimumAmplitude;
    c.maximumSamples=plan.probe.specification.maximumSamples;c.recordSpikes=plan.probe.specification.recordSpikes;c.peakSelection=plan.protocol.peakSelection;
    for(const auto& w:plan.inputWindows)c.windows.emplace_back(w.begin,w.end);
    c.search=plan.search;c.search.initialModelRevision.clear();c.search.initialModelEpoch=0;
    c.maximumReferenceProbes=plan.maximumReferenceProbes;c.maximumInvalidReferenceProbes=plan.maximumInvalidReferenceProbes;
    c.maximumModelSeconds=plan.maximumModelSeconds;c.maximumWallSeconds=plan.maximumWallSeconds;
    Plan(c,plan.protocol);return c;
}
std::pair<QString,double> Threshold(const QJsonObject& identity)
{
    const auto model=Object(identity["model"]);QString path;
    for(const auto& cache:Array(model["caches"]))
        if(!Object(cache)["cached_ltzone"].toString().isEmpty()){path=Object(cache)["cached_ltzone"].toString();break;}
    Require(!path.isEmpty(),"learned_state_missing_ltzone");
    for(const auto& node:Array(model["nodes"]))if(Object(node)["path"].toString()==path)
    {
        const auto data=Object(Object(Object(node)["parameters"])["Threshold"])["data"].toObject();
        bool ok=false;const auto hex=data["value"].toString();const std::uint64_t bits=hex.toULongLong(&ok,16);double value;
        std::memcpy(&value,&bits,sizeof(value));Require(ok && hex.size()==16 && data["type"].toString()=="binary64" && std::isfinite(value),"invalid_learned_state_threshold");
        return {path,value};
    }
    throw std::invalid_argument("learned_state_missing_threshold");
}
void CheckDataset(const ConfiguredNativeTraining& c,RDK::UEPtr<NDatasetMatrix> dataset)
{
    Require(bool(dataset) && dataset->MatrixData.GetRows()==int(c.model.initialIsi.size()) && dataset->MatrixData.GetCols()==1,"learned_state_dataset_dimension_mismatch");
    for(std::size_t i=0;i<c.model.initialIsi.size();++i)Require(dataset->MatrixData(int(i),0)==c.model.initialIsi[i],"learned_state_target_mismatch");
    auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(generator),"learned_state_missing_generator");
    const StimulusProfile actual{generator->GetCompClassName(),generator->Amplitude.GetData(),generator->PulseLength.GetData(),double(generator->GetTimeStep()),
        generator->FrequencyDeviation.GetData(),generator->UseTransitSignal.GetData(),generator->UsePatternOutput.GetData()};
    Require(actual==c.stimulusProfile,"learned_state_effective_stimulus_mismatch");
}
}
PreparedNativeLearnedState::~PreparedNativeLearnedState()
{
    if(neuron && !neuron->GetOwner()){neuron->BreakLinks();storage->ReturnObject(neuron);}
    if(dataset && !dataset->GetOwner()){dataset->BreakLinks();storage->ReturnObject(dataset);}
}
QJsonObject ExportNativeLearnedState(RDK::UNet& owner,const NativeModelSpec& declaration,const NativeTrainingService& service,const std::vector<InputParameters>& actual,
    int version,int calculateMode,int structureMode,const std::string& implementationId)
{
    Require(service.State()!=NativeTrainingState::CapturingReferences && service.State()!=NativeTrainingState::Searching &&
        service.References() && service.Certificate(),"learned_state_requires_completed_certified_boundary");
    Require(!implementationId.empty() && (version==1 || version==2) && (calculateMode==0 || calculateMode==1) &&
        (structureMode==0 || structureMode==1),"invalid_learned_state_component_options");
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    Require(neuron && dataset,"learned_state_missing_graph");const auto c=Configuration(declaration,service);CheckDataset(c,dataset);
    auto source=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
    const auto identity=NmsdkTools::IdentifyModel(neuron,ports);Require(identity["valid"].toBool(),"learned_state_invalid_model");
    const auto revision=identity["sha256"].toString().toStdString();const auto epoch=service.ModelEpoch();
    const auto proof=ReplayNativeTrainingProof(service.References()->Protocol(),service.Certificate()->Tolerances(),
        service.ReferencePackets(),service.FinalPackets(),revision,epoch,service.Configuration().maximumInvalidObservations);
    Require(EncodeTrainingCertificate(proof.certificate)==EncodeTrainingCertificate(*service.Certificate()) &&
        service.Certificate()->Matches(revision,*service.References(),service.Certificate()->Tolerances(),ModelStateKind::LearnedSolo,epoch),
        "learned_state_certificate_does_not_match_model");
    std::vector<int> positions;QJsonArray inputs;
    for(const auto& p:actual){positions.push_back(p.length);inputs.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});}
    const auto checked=InspectNativeLearnerModel(owner,declaration,positions);
    Require(checked.status==NativeModelBuildStatus::Adopted && checked.modelRevision==revision && checked.effective==actual,"learned_state_readback_mismatch");
    const auto datasetSnapshot=NmsdkTools::ExportDatasetSnapshot(dataset);std::vector<std::string> additional;
    for(const auto& cls:Array(datasetSnapshot["required_class_names"]))additional.push_back(cls.toString().toStdString());
    const auto [ltPath,threshold]=Threshold(identity);QJsonArray target,references;
    for(double isi:declaration.initialIsi)target.append(isi);for(const auto& r:proof.references.Inputs())references.append(r.rawPotential);
    const auto status=proof.certificate.Exact()?"exact":"search_unresolved";
    QJsonObject metadata{{"implementation_id",Text(implementationId)},{"state_kind","learned_solo"},
        {"training",QJsonObject{{"status",status},{"target_id",Text(c.sampleId)},{"trainer",declaration.branch?"Branch":"Classic"},
            {"normalization",declaration.normalization==Normalization::SynapseCount?"N0":"N1"},{"target_pattern",target},{"captured_per_input",references},
            {"certificate",QJsonObject{{"state_identity",Text(revision)},{"status",status}}}}},
        {"calibration",QJsonObject{{"status","fixed"},{"protocol","fixed-trained-graph"},{"ltzone_path",ltPath},{"threshold",threshold}}},
        {"conversion",QJsonObject{{"mode","none"}}}};
    const auto archive=NmsdkTools::ExportModelArchive(neuron,metadata,ports,additional);
    QJsonObject package{{"kind","timelearner_completed_state"},{"schema_version",1},{"dynamic_checkpoint",false},{"state_kind","learned_solo"},
        {"implementation_id",Text(implementationId)},{"controller_version",version},{"calculate_mode",calculateMode},{"structure_build_mode",structureMode},
        {"configuration",EncodeConfiguredNativeTraining(c)},{"protocol",EncodeVerificationProtocol(proof.references.Protocol())},
        {"trained_inputs",inputs},{"model_revision",Text(revision)},{"model_epoch",Integer(epoch)},{"session_attempt",Integer(service.SessionAttempt())},
        {"training_outcome",status},{"certificate",EncodeTrainingCertificate(proof.certificate)},
        {"reference_packets",Packets(service.ReferencePackets())},{"final_packets",Packets(service.FinalPackets())},
        {"dataset",datasetSnapshot},{"neuron",archive}};
    package["sha256"]=NmsdkTools::CanonicalModelDataSha256(package);return package;
}
QJsonObject ParseNativeLearnedState(const std::string& text)
{return ParseStrictNativeJsonObject(text,128*1024*1024);}
std::unique_ptr<PreparedNativeLearnedState> PrepareNativeLearnedState(const QJsonObject& package,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId)
{
    auto body=package;body.remove("sha256");
    Require(package.size()==21 && package["kind"]==QJsonValue("timelearner_completed_state") && package["schema_version"]==QJsonValue(1) &&
        package["dynamic_checkpoint"]==QJsonValue(false) && package["state_kind"]==QJsonValue("learned_solo"),"unsupported_learned_state_schema");
    Require(package["sha256"].isString() && package["sha256"].toString()==NmsdkTools::CanonicalModelDataSha256(body),"learned_state_digest_mismatch");
    Require(!implementationId.empty() && package["implementation_id"]==QJsonValue(Text(implementationId)),"learned_state_implementation_mismatch");
    auto result=std::make_unique<PreparedNativeLearnedState>();result->storage=storage;
    result->configuration=DecodeConfiguredNativeTraining(Object(package["configuration"]));const auto& c=result->configuration;
    const auto version=Count(package["controller_version"]),mode=Count(package["calculate_mode"]),structure=Count(package["structure_build_mode"]);
    Require((version==1 || version==2) && mode<=1 && structure<=1,"invalid_learned_state_component_options");
    result->controllerVersion=int(version);result->calculateMode=int(mode);result->structureBuildMode=int(structure);
    Require(package["model_revision"].isString(),"invalid_learned_state_revision");result->modelRevision=package["model_revision"].toString().toStdString();
    for(const auto& v:Array(package["trained_inputs"]))
    {
        const auto p=Object(v);const auto l=Count(p["length"]),n=Count(p["synapses"]);const auto r=p["resistance"];
        Require(p.size()==3 && l>=1 && l<=std::size_t(c.model.editPolicy.maximumCableLength) && n>=1 && n<=128 &&
            r.isDouble() && std::isfinite(r.toDouble()) && r.toDouble()>0.,"invalid_learned_state_parameters");
        result->parameters.push_back({int(l),int(n),r.toDouble()});
    }
    Require(result->parameters.size()==c.model.inputs.size(),"learned_state_input_dimension_mismatch");
    const auto protocol=DecodeVerificationProtocol(Object(package["protocol"]));auto plan=Plan(c,protocol);
    if(version==2)Require(protocol==MakeConfiguredNativeTrainingPlan(c).protocol,"configured_learned_state_requires_canonical_protocol");
    result->service.RestoreBoundary(plan,Packets(package["reference_packets"]),Packets(package["final_packets"]),result->modelRevision,
        Count(package["model_epoch"]),Count(package["session_attempt"]));
    Require(EncodeTrainingCertificate(*result->service.Certificate())==Object(package["certificate"]) &&
        package["training_outcome"]==QJsonValue(result->service.Certificate()->Exact()?"exact":"search_unresolved"),"learned_state_certificate_claim_mismatch");
    const auto archive=Object(package["neuron"]);Require(Object(Object(archive["graph"])["identity"])["sha256"]==package["model_revision"],"learned_state_archive_revision_mismatch");
    const auto metadata=Object(archive["metadata"]),training=Object(metadata["training"]);
    QJsonArray target,raw;for(double value:c.model.initialIsi)target.append(value);
    for(const auto& reference:result->service.References()->Inputs())raw.append(reference.rawPotential);
    Require(metadata["state_kind"]==QJsonValue("learned_solo") && training["status"]==package["training_outcome"] &&
        training["target_pattern"]==QJsonValue(target) && training["captured_per_input"]==QJsonValue(raw) &&
        training["target_id"]==QJsonValue(Text(c.sampleId)) && training["trainer"]==QJsonValue(c.model.branch?"Branch":"Classic") &&
        training["normalization"]==QJsonValue(c.model.normalization==Normalization::SynapseCount?"N0":"N1") &&
        Object(metadata["calibration"])["status"]==QJsonValue("fixed") && Object(metadata["conversion"])["mode"]==QJsonValue("none"),
        "learned_state_archive_metadata_mismatch");
    std::set<QString> declared;for(const auto& value:Array(archive["class_definitions"]))declared.insert(Object(value)["name"].toString());
    for(const auto& value:Array(Object(package["dataset"])["required_class_names"]))
        Require(value.isString() && declared.count(value.toString()),"learned_state_missing_dataset_dependency");
    NmsdkTools::PrepareModelArchive(archive,storage,environment,Text(implementationId));
    result->dataset=RDK::dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(Object(package["dataset"]),storage,environment));
    CheckDataset(c,result->dataset);auto source=result->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    result->neuron=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(archive,storage,environment,Text(implementationId),{{&source->Output,"burst"}}));
    result->configurationText=QJsonDocument(EncodeConfiguredNativeTraining(c)).toJson(QJsonDocument::Compact).toStdString();return result;
}
}
