#include "NativeTrainingCapacitance.h"
#include "ModelIdentity.h"
#include "ModelArchive.h"
#include "../NPulseGenerator.h"
#include <QJsonDocument>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool condition,const char* reason)
{if(!condition)throw std::invalid_argument(reason);}
void Boundary(const NativeProbeExecutor& executor)
{
    Require(executor.Status()!=NativeProbeStatus::Queued && executor.Status()!=NativeProbeStatus::Running,
        "training_capacitance_read_during_probe");
}
NativeCapacitanceCorrection Parse(const QJsonObject& owned)
{
    std::string revision;
    return ParseNativeCapacitanceState(QJsonDocument(owned).toJson(QJsonDocument::Compact).toStdString(),revision);
}
TrainingCapacitance Parameters(const NativeCapacitanceCorrection& correction)
{
    const auto& baseline=correction.baseline;
    // Full baseline.sha256 grows with the witnessed channel inventory. These
    // original capture/profile fields remain immutable across native growth.
    const QJsonObject origin{{"kind","timelearner_training_capacitance_origin"},{"schema_version",1},
        {"capture_model_revision",baseline["model_revision"]},{"model_spec",baseline["model_spec"]},
        {"storage_images_sha256",baseline["storage_images_sha256"]}};
    TrainingCapacitance result;result.origin=NmsdkTools::CanonicalModelDataSha256(origin).toStdString();
    result.maximumRelativeChange=correction.maximumRelativeChange;
    result.membraneScales=correction.membraneScales;
    for(const auto& entry:correction.dormantMembraneScales)
        Require(result.membraneScales.emplace(entry).second,"overlapping_training_capacitance_scales");
    const auto channels=baseline["channels"].toObject();
    for(auto it=channels.begin();it!=channels.end();++it)
    {
        const auto name=it.key().toStdString();const auto separator=name.find('.');
        if(name.starts_with("Dendrite") && separator!=std::string::npos)
            result.membraneScales.try_emplace(name.substr(0,separator),1.);
    }
    ValidateTrainingCapacitance(result);return result;
}
}
TrainingCapacitance DecodeNativeTrainingCapacitance(const QJsonObject& state)
{
    const auto correction=Parse(state);
    Require(correction.baseline["schema_version"]==QJsonValue(2),
        "training_capacitance_requires_bound_baseline");
    return Parameters(correction);
}
std::optional<TrainingCapacitance> ReadNativeTrainingCapacitance(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned)
{
    Boundary(executor);
    if(owned.isEmpty())return {};
    Require(bool(declaration),"training_capacitance_requires_model_declaration");
    ValidateAppliedNativeCapacitanceState(bindings,*declaration,owned);
    return DecodeNativeTrainingCapacitance(owned);
}
std::optional<TrainingCapacitanceWitness> ReadNativeTrainingCapacitanceWitness(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned)
{
    const auto parameters=ReadNativeTrainingCapacitance(executor,bindings,declaration,owned);
    if(!parameters)return {};
    Require(bool(bindings.neuron) && bool(bindings.dataset),"missing_training_capacitance_witness_graph");
    const auto source=bindings.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(source),"missing_training_capacitance_witness_source");
    const NmsdkTools::ModelBoundarySources ports{{&source->Output,"burst"}};
    RDK::UEPtr<RDK::UContainer> live(bindings.neuron.Get());
    const auto before=NmsdkTools::IdentifyModel(live,ports);
    Require(before["valid"].toBool(),"invalid_training_capacitance_witness_identity");
    std::vector<std::string> roots;
    for(const auto& definition:owned["baseline"].toObject()["storage_images"].toArray())
        roots.push_back(definition.toObject()["name"].toString().toStdString());
    const auto images=NmsdkTools::CaptureModelImageDefinitions(live,ports,roots);
    // Capture the real registry, including baseline-only Dataset/growth images.
    // A copy of baseline definitions would not establish current storage physics.
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(images)==
        owned["baseline"].toObject()["storage_images_sha256"].toString(),
        "training_capacitance_witness_storage_changed");
    ValidateAppliedNativeCapacitanceState(bindings,*declaration,owned);
    const auto after=NmsdkTools::IdentifyModel(live,ports);
    Require(after==before,"training_capacitance_witness_model_changed");
    TrainingCapacitanceWitness result;result.parameters=*parameters;
    result.modelRevision=before["sha256"].toString().toStdString();
    result.modelIdentityJson=QJsonDocument(before).toJson(QJsonDocument::Compact).toStdString();
    result.nativeStateJson=QJsonDocument(owned).toJson(QJsonDocument::Compact).toStdString();
    result.storageImagesJson=QJsonDocument(images).toJson(QJsonDocument::Compact).toStdString();
    return result;
}
QJsonObject PrepareNativeTrainingCapacitanceBinding(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned,
    double maximumRelativeChange)
{
    Boundary(executor);
    Require(std::isfinite(maximumRelativeChange) && maximumRelativeChange>=0.,
        "invalid_training_capacitance_binding_policy");
    Require(bool(declaration),"training_capacitance_requires_model_declaration");
    if(!owned.isEmpty())
    {
        const auto current=ReadNativeTrainingCapacitance(executor,bindings,declaration,owned);
        Require(current->maximumRelativeChange==maximumRelativeChange,
            "training_capacitance_origin_or_policy_changed");
        return owned;
    }
    NativeCapacitanceCorrection correction;
    correction.baseline=CaptureBoundNativeCapacitanceBaseline(executor,bindings,*declaration);
    correction.maximumRelativeChange=maximumRelativeChange;
    const auto channels=correction.baseline["channels"].toObject();
    for(auto it=channels.begin();it!=channels.end();++it)
    {
        const auto path=it.key().toStdString();const auto separator=path.find('.');
        if(path.starts_with("Dendrite") && separator!=std::string::npos)
            correction.membraneScales.emplace(path.substr(0,separator),1.);
    }
    const auto state=EncodeNativeCapacitanceState(correction,correction.baseline["model_revision"].toString().toStdString());
    ValidateAppliedNativeCapacitanceState(bindings,*declaration,state);
    DecodeNativeTrainingCapacitance(state); // Validate the complete controller map too.
    return state;
}
std::optional<NativeCapacitanceCorrection> ResolveNativeTrainingCapacitance(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned,
    const std::optional<TrainingCapacitance>& requested)
{
    const auto current=ReadNativeTrainingCapacitance(executor,bindings,declaration,owned);
    ValidateTrainingCapacitance(requested);
    if(!current)
    {
        Require(!requested,"training_capacitance_requires_owned_baseline");return {};
    }
    Require(bool(requested),"training_capacitance_state_required");
    Require(requested->origin==current->origin && requested->maximumRelativeChange==current->maximumRelativeChange,
        "training_capacitance_origin_or_policy_changed");
    for(const auto& [path,scale]:current->membraneScales)
        Require(requested->membraneScales.count(path)>0,"training_capacitance_history_lost");
    auto correction=Parse(owned);
    correction.membraneScales=requested->membraneScales;correction.dormantMembraneScales.clear();
    // New paths are deliberately left to the native joint preview: only that
    // graph can prove their existence, C0/profile and active/dormant partition.
    return correction;
}
}
