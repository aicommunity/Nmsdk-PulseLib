#include "NativeThresholdCandidate.h"
#include "NativeThresholdSupport.h"
#include "../NPulseLTZoneCommon.h"
#include <QJsonArray>
#include <cmath>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool condition,const char* reason)
{if(!condition)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& value)
{Require(value.isObject(),"threshold_candidate_requires_object");return value.toObject();}
QString Text(const std::string& value){return QString::fromStdString(value);}
struct Candidate {
    std::unique_ptr<PreparedNativeRecognitionState> model;
    QJsonObject metadata,identity,record;
};
Candidate Construct(const QJsonObject& source,double threshold,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation,int algorithmVersion=2)
{
    Require(std::isfinite(threshold) && (algorithmVersion==2 || threshold>0.),"invalid_threshold_candidate_value");
    // Do not accept a candidate as its own source, or recursively rebuild an
    // arbitrary chain of speculative changes. Conversion is applied only once.
    Require(source["kind"]==QJsonValue("timelearner_recognition_state") && source["schema_version"]==QJsonValue(1),
        "threshold_candidate_requires_base_recognition");
    Candidate c;c.model=PrepareNativeRecognitionState(source,storage,environment,implementation);
    const auto archive=Object(source["neuron"]);c.metadata=Object(archive["metadata"]);
    auto calibration=Object(c.metadata["calibration"]);
    const auto path=calibration["ltzone_path"].toString();
    Require(path.startsWith("$."),"threshold_candidate_invalid_ltzone_path");
    // Identity paths are rooted at '$'; native GetComponentL uses names
    // relative to the neuron. Keep the semantic path in the provenance.
    auto zone=c.model->neuron->GetComponentL<NPulseLTZoneCommon>(path.mid(2).toStdString(),true);
    Require(bool(zone),"threshold_candidate_missing_ltzone");
    if(algorithmVersion==2)RequireNativeScalarSpikeThreshold(*zone);
    Require(!zone->Threshold.IsConnected() && !zone->Threshold.UsesExternalDataSource(),
        "threshold_candidate_connected_threshold");
    const double original=zone->Threshold.GetData();
    if(original!=threshold)zone->Threshold=threshold;
    Require(zone->Threshold.GetData()==threshold,"threshold_candidate_readback_mismatch");
    auto generator=c.model->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"threshold_candidate_missing_generator");
    c.identity=NmsdkTools::IdentifyModel(c.model->neuron,{{&generator->Output,"burst"}});
    Require(c.identity["valid"]==QJsonValue(true),"threshold_candidate_invalid_identity");
    c.record={{"schema_version",1},{"algorithm",algorithmVersion==2?"explicit-native-threshold-v2":"explicit-native-threshold-v1"},
        {"source_recognition_sha256",source["sha256"]},{"source_model_revision",source["model_revision"]},
        {"model_revision",c.identity["sha256"]},{"ltzone_path",path},
        {"original_threshold",original},{"threshold",threshold},{"calibrated",false}};
    // Fixed here means the actual frozen parameter, not a fitted/certified
    // result. CalibrationResult requires later real complete candidate trials.
    calibration["status"]="fixed";calibration["protocol"]="threshold-candidate-not-calibrated";
    calibration["threshold"]=threshold;calibration["candidate_record"]=c.record;
    c.metadata["calibration"]=calibration;
    auto verification=Object(c.metadata["recognition_verification"]);
    verification["model_identity"]=c.identity["sha256"];c.metadata["recognition_verification"]=verification;
    c.model->modelRevision=c.identity["sha256"].toString().toStdString();
    return c;
}
}
QJsonObject CreateNativeThresholdCandidate(const QJsonObject& source,double threshold,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation)
{
    auto c=Construct(source,threshold,storage,environment,implementation);
    auto generator=c.model->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const auto dataset=Object(source["dataset"]);std::vector<std::string> roots;
    for(const auto& name:dataset["required_class_names"].toArray())roots.push_back(name.toString().toStdString());
    const auto archive=NmsdkTools::ExportModelArchive(c.model->neuron,c.metadata,{{&generator->Output,"burst"}},roots);
    QJsonObject result{{"kind","timelearner_recognition_threshold_candidate"},{"schema_version",1},
        {"dynamic_checkpoint",false},{"state_kind","recognition"},{"implementation_id",Text(implementation)},
        {"source_recognition",source},{"threshold",threshold},{"candidate_record",c.record},
        {"model_revision",c.identity["sha256"]},{"recognition_certificate",QJsonValue(QJsonValue::Null)},
        {"dataset",dataset},{"neuron",archive}};
    result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
}
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeThresholdCandidate(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation)
{
    const std::set<QString> fields{"kind","schema_version","dynamic_checkpoint","state_kind","implementation_id",
        "source_recognition","threshold","candidate_record","model_revision","recognition_certificate","dataset","neuron","sha256"};
    Require(package.size()==int(fields.size()),"threshold_candidate_field_set_mismatch");
    for(const auto& field:fields)Require(package.contains(field),"threshold_candidate_missing_field");
    Require(package["kind"]==QJsonValue("timelearner_recognition_threshold_candidate") &&
        package["schema_version"]==QJsonValue(1) && package["dynamic_checkpoint"]==QJsonValue(false) &&
        package["state_kind"]==QJsonValue("recognition") && package["recognition_certificate"].isNull(),
        "unsupported_threshold_candidate_claim");
    auto body=package;body.remove("sha256");
    Require(package["sha256"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(body)),"threshold_candidate_digest_mismatch");
    Require(!implementation.empty() && package["implementation_id"]==QJsonValue(Text(implementation)),
        "threshold_candidate_implementation_mismatch");
    Require(package["threshold"].isDouble(),"invalid_threshold_candidate_value");
    // Materialize the value: operator[] on the temporary QJsonObject returns
    // a QJsonValueRef whose owner would expire at the end of this statement.
    const QJsonValue algorithm=Object(package["candidate_record"])["algorithm"];
    Require(algorithm==QJsonValue("explicit-native-threshold-v1") || algorithm==QJsonValue("explicit-native-threshold-v2"),
        "unknown_threshold_candidate_algorithm");
    const auto source=Object(package["source_recognition"]);
    auto expected=Construct(source,package["threshold"].toDouble(),storage,environment,implementation,
        algorithm==QJsonValue("explicit-native-threshold-v2")?2:1);
    const auto archive=Object(package["neuron"]);
    Require(package["candidate_record"]==QJsonValue(expected.record) && package["model_revision"]==expected.identity["sha256"] &&
        package["dataset"]==source["dataset"] && archive["metadata"]==QJsonValue(expected.metadata) &&
        Object(archive["graph"])["identity"]==QJsonValue(expected.identity),"threshold_candidate_derivation_mismatch");
    auto& m=*expected.model;auto generator=m.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    m.neuron->BreakLinks();storage->ReturnObject(m.neuron);m.neuron=0;
    m.neuron=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(archive,storage,environment,
        Text(implementation),{{&generator->Output,"burst"}}));
    Require(bool(m.neuron),"threshold_candidate_archive_not_neuron");
    return std::move(expected.model);
}
}
