#include "NativeLearnedState.h"
#include "NativeTrainingProof.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>
#include <set>
#include <map>

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
void CheckStoppedPackets(const std::vector<NativeProbeResult>& references,const std::vector<NativeProbeResult>& finals,
    const std::vector<NativeProbeResult>& observed)
{
    const auto id=[](const NativeProbeResult& p) {
        Require(p.requestedSpecification && !p.requestedSpecification->runId.empty() && !p.requestedSpecification->trialId.empty(),
            "stopped_packet_requires_trial_identity");
        return p.requestedSpecification->trialId;
    };
    std::map<std::string,QJsonObject> attempts;
    for(const auto& p:observed)Require(attempts.emplace(id(p),EncodeNativeProbeEvidence(p)).second,"duplicate_stopped_trial");
    std::set<std::string> accepted;
    for(const auto& p:references)
    {
        const auto trial=id(p);Require(accepted.insert(trial).second,"duplicate_stopped_reference");
        // A warm attempt legitimately retains references from an older attempt.
        const auto i=attempts.find(trial);
        if(i!=attempts.end())Require(i->second==EncodeNativeProbeEvidence(p),"stopped_reference_observation_mismatch");
    }
    for(const auto& p:finals)
    {
        const auto trial=id(p);const auto i=attempts.find(trial);
        Require(accepted.insert(trial).second && i!=attempts.end() && i->second==EncodeNativeProbeEvidence(p),
            "stopped_final_observation_mismatch");
    }
}
TrainingStatus StoppedStatus(const QJsonValue& value)
{
    Require(value.isString(),"invalid_stopped_training_status");
    for(const auto s:{TrainingStatus::BudgetExhausted,TrainingStatus::ExternalStop,TrainingStatus::InvalidObservation,
        TrainingStatus::InvalidModel,TrainingStatus::SearchUnresolved})
        if(value.toString()==QLatin1String(TrainingStatusName(s)))return s;
    throw std::invalid_argument("invalid_stopped_training_status");
}
QJsonArray RawReferences(const VerificationProtocol& protocol,const std::vector<NativeProbeResult>& packets)
{
    QJsonArray result;for(std::size_t i=0;i<packets.size();++i)
        result.append(ValidateProbeMeasurement(protocol,i,*packets[i].prepared,*packets[i].observation).rawPotential);
    return result;
}
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
    const bool stopped=!service.Certificate();
    Require(service.State()!=NativeTrainingState::CapturingReferences && service.State()!=NativeTrainingState::Searching &&
        (stopped?(service.State()==NativeTrainingState::Finished && service.SessionAttempt()>0):bool(service.References())),
        "learned_state_requires_completed_or_stopped_boundary");
    Require(!implementationId.empty() && (version==1 || version==2) && (calculateMode==0 || calculateMode==1) &&
        (structureMode==0 || structureMode==1),"invalid_learned_state_component_options");
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    Require(neuron && dataset,"learned_state_missing_graph");const auto c=Configuration(declaration,service);CheckDataset(c,dataset);
    auto source=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
    const auto identity=NmsdkTools::IdentifyModel(neuron,ports);Require(identity["valid"].toBool(),"learned_state_invalid_model");
    const auto revision=identity["sha256"].toString().toStdString();const auto epoch=service.ModelEpoch();
    const auto& protocol=service.References()?service.References()->Protocol():service.Plan().protocol;
    std::optional<NativeTrainingProof> proof;
    if(stopped)
    {
        StoppedStatus(QJsonValue(QLatin1String(TrainingStatusName(service.Outcome()))));
        Require(!service.Reason().empty(),"stopped_state_requires_reason");
        CheckStoppedPackets(service.ReferencePackets(),service.FinalPackets(),service.ObservedPackets());
        const auto captured=ReplayNativeReferencePrefix(protocol,service.ReferencePackets(),service.CaptureRevision(),service.CaptureEpoch(),bool(service.References()));
        if(captured)Require(captured->Inputs()==service.References()->Inputs(),"stopped_state_reference_mismatch");
    }
    else
    {
        proof.emplace(ReplayNativeTrainingProof(protocol,service.Certificate()->Tolerances(),
            service.ReferencePackets(),service.FinalPackets(),revision,epoch,service.Configuration().maximumInvalidObservations));
        Require(EncodeTrainingCertificate(proof->certificate)==EncodeTrainingCertificate(*service.Certificate()) &&
            service.Certificate()->Matches(revision,*service.References(),service.Certificate()->Tolerances(),ModelStateKind::LearnedSolo,epoch),
            "learned_state_certificate_does_not_match_model");
    }
    std::vector<int> positions;QJsonArray inputs;
    for(const auto& p:actual){positions.push_back(p.length);inputs.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});}
    const auto checked=InspectNativeLearnerModel(owner,declaration,positions);
    Require(checked.status==NativeModelBuildStatus::Adopted && checked.modelRevision==revision && checked.effective==actual,"learned_state_readback_mismatch");
    const auto datasetSnapshot=NmsdkTools::ExportDatasetSnapshot(dataset);std::vector<std::string> additional;
    for(const auto& cls:Array(datasetSnapshot["required_class_names"]))additional.push_back(cls.toString().toStdString());
    const auto [ltPath,threshold]=Threshold(identity);QJsonArray target;
    for(double isi:declaration.initialIsi)target.append(isi);
    const auto references=RawReferences(protocol,service.ReferencePackets());
    const auto status=stopped?TrainingStatusName(service.Outcome()):(proof->certificate.Exact()?"exact":"search_unresolved");
    const auto stateKind=stopped?"diagnostic":"learned_solo";
    QJsonObject metadata{{"implementation_id",Text(implementationId)},{"state_kind",stateKind},
        {"training",QJsonObject{{"status",status},{"target_id",Text(c.sampleId)},{"trainer",declaration.branch?"Branch":"Classic"},
            {"normalization",declaration.normalization==Normalization::SynapseCount?"N0":"N1"},{"target_pattern",target},{"captured_per_input",references},
            {"certificate",QJsonObject{{"state_identity",Text(revision)},{"status",status}}}}},
        {"calibration",QJsonObject{{"status","fixed"},{"protocol","fixed-trained-graph"},{"ltzone_path",ltPath},{"threshold",threshold}}},
        {"conversion",QJsonObject{{"mode","none"}}}};
    if(stopped)
    {
        auto training=metadata["training"].toObject();training["certificate"]=QJsonValue(QJsonValue::Null);
        training["reference_complete"]=bool(service.References());training["capture_revision"]=Text(service.CaptureRevision());
        training["capture_epoch"]=Integer(service.CaptureEpoch());training["reason"]=Text(service.Reason());metadata["training"]=training;
    }
    const auto archive=NmsdkTools::ExportModelArchive(neuron,metadata,ports,additional);
    QJsonObject package{{"kind",stopped?"timelearner_stopped_state":"timelearner_completed_state"},{"schema_version",1},{"dynamic_checkpoint",false},{"state_kind",stateKind},
        {"implementation_id",Text(implementationId)},{"controller_version",version},{"calculate_mode",calculateMode},{"structure_build_mode",structureMode},
        {"configuration",EncodeConfiguredNativeTraining(c)},{"protocol",EncodeVerificationProtocol(protocol)},
        {"trained_inputs",inputs},{"model_revision",Text(revision)},{"model_epoch",Integer(epoch)},{"session_attempt",Integer(service.SessionAttempt())},
        {"training_outcome",status},{"certificate",proof?QJsonValue(EncodeTrainingCertificate(proof->certificate)):QJsonValue(QJsonValue::Null)},
        {"reference_packets",Packets(service.ReferencePackets())},{"final_packets",Packets(service.FinalPackets())},
        {"dataset",datasetSnapshot},{"neuron",archive}};
    if(stopped)
    {
        package["reason"]=Text(service.Reason());package["capture_revision"]=Text(service.CaptureRevision());
        package["capture_epoch"]=Integer(service.CaptureEpoch());package["reference_complete"]=bool(service.References());
        package["observed_packets"]=Packets(service.ObservedPackets());
    }
    package["sha256"]=NmsdkTools::CanonicalModelDataSha256(package);return package;
}
QJsonObject ParseNativeLearnedState(const std::string& text)
{return ParseStrictNativeJsonObject(text,128*1024*1024);}
std::unique_ptr<PreparedNativeLearnedState> PrepareNativeLearnedState(const QJsonObject& package,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId)
{
    auto body=package;body.remove("sha256");
    const bool stopped=package["kind"]==QJsonValue("timelearner_stopped_state");
    Require((stopped?(package.size()==26 && package["state_kind"]==QJsonValue("diagnostic")):
        (package.size()==21 && package["kind"]==QJsonValue("timelearner_completed_state") && package["state_kind"]==QJsonValue("learned_solo"))) &&
        package["schema_version"]==QJsonValue(1) && package["dynamic_checkpoint"]==QJsonValue(false),"unsupported_learned_state_schema");
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
    if(stopped)
    {
        Require(package["certificate"].isNull() && package["reference_complete"].isBool() &&
            package["reason"].isString() && !package["reason"].toString().isEmpty() && package["capture_revision"].isString(),
            "invalid_stopped_state_claim");
        auto refs=Packets(package["reference_packets"]),finals=Packets(package["final_packets"]),observed=Packets(package["observed_packets"]);
        CheckStoppedPackets(refs,finals,observed);
        result->service.RestoreStoppedBoundary(plan,std::move(refs),std::move(finals),std::move(observed),
            result->modelRevision,Count(package["model_epoch"]),Count(package["session_attempt"]),
            package["capture_revision"].toString().toStdString(),Count(package["capture_epoch"]),package["reference_complete"].toBool(),
            StoppedStatus(package["training_outcome"]),package["reason"].toString().toStdString());
    }
    else
    {
        result->service.RestoreBoundary(plan,Packets(package["reference_packets"]),Packets(package["final_packets"]),result->modelRevision,
            Count(package["model_epoch"]),Count(package["session_attempt"]));
        Require(EncodeTrainingCertificate(*result->service.Certificate())==Object(package["certificate"]) &&
            package["training_outcome"]==QJsonValue(result->service.Certificate()->Exact()?"exact":"search_unresolved"),"learned_state_certificate_claim_mismatch");
    }
    const auto archive=Object(package["neuron"]);Require(Object(Object(archive["graph"])["identity"])["sha256"]==package["model_revision"],"learned_state_archive_revision_mismatch");
    const auto metadata=Object(archive["metadata"]),training=Object(metadata["training"]);
    QJsonArray target;for(double value:c.model.initialIsi)target.append(value);
    const auto raw=RawReferences(protocol,result->service.ReferencePackets());
    Require(metadata["state_kind"]==package["state_kind"] && training["status"]==package["training_outcome"] &&
        training["target_pattern"]==QJsonValue(target) && training["captured_per_input"]==QJsonValue(raw) &&
        training["target_id"]==QJsonValue(Text(c.sampleId)) && training["trainer"]==QJsonValue(c.model.branch?"Branch":"Classic") &&
        training["normalization"]==QJsonValue(c.model.normalization==Normalization::SynapseCount?"N0":"N1") &&
        Object(metadata["calibration"])["status"]==QJsonValue("fixed") && Object(metadata["conversion"])["mode"]==QJsonValue("none"),
        "learned_state_archive_metadata_mismatch");
    if(stopped)Require(training["certificate"].isNull() && training["reference_complete"]==package["reference_complete"] &&
        training["capture_revision"]==package["capture_revision"] && training["capture_epoch"]==package["capture_epoch"] &&
        training["reason"]==package["reason"],"stopped_state_archive_metadata_mismatch");
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
