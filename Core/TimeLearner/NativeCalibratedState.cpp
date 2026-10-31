#include "NativeCalibratedState.h"
#include "NativeRecognitionObservationValidation.h"
#include "NativeTrainingBindings.h"
#include "NativeThresholdSupport.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <map>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& value)
{Require(value.isObject(),"invalid_calibration_proof_object");return value.toObject();}
QJsonArray Array(const QJsonValue& value)
{Require(value.isArray(),"invalid_calibration_proof_array");return value.toArray();}
QString Text(const std::string& value){return QString::fromStdString(value);}
std::string String(const QJsonValue& value)
{Require(value.isString() && !value.toString().trimmed().isEmpty(),"invalid_calibration_proof_text");return value.toString().toStdString();}
double Number(const QJsonValue& value)
{Require(value.isDouble() && std::isfinite(value.toDouble()),"invalid_calibration_proof_number");return value.toDouble();}
bool Flag(const QJsonValue& value)
{Require(value.isBool(),"invalid_calibration_proof_flag");return value.toBool();}
QJsonObject Signed(QJsonObject object)
{object.remove("sha256");object["sha256"]=NmsdkTools::CanonicalModelDataSha256(object);return object;}
QJsonObject Request(const NativeRecognitionTrialRequest& q)
{
    QJsonArray pattern;for(int i=0;i<q.pattern.GetRows();++i)pattern.append(q.pattern(i,0));
    return {{"run_id",Text(q.runId)},{"trial_id",Text(q.trialId)},{"sample_id",Text(q.sampleId)},
        {"label",Text(q.label)},{"pattern_hash",Text(q.patternHash)},{"pattern_isi",pattern},
        {"duration",q.duration},{"maximum_samples",double(q.maximumSamples)},
        {"reset_policy",q.resetPolicy==RecognitionResetPolicy::PerTrial?"reset_per_trial":"preserve_neuron_dynamics"},
        {"record_threshold_potential",q.recordThresholdPotential}};
}
NativeRecognitionTrialRequest ReadRequest(const QJsonObject& object)
{
    NativeRecognitionTrialRequest q;q.runId=String(object["run_id"]);q.trialId=String(object["trial_id"]);
    q.sampleId=String(object["sample_id"]);q.label=String(object["label"]);q.patternHash=String(object["pattern_hash"]);
    q.duration=Number(object["duration"]);const double limit=Number(object["maximum_samples"]);
    Require(limit>=2 && limit<=10000000 && std::floor(limit)==limit,"invalid_calibration_proof_sample_budget");
    q.maximumSamples=std::size_t(limit);q.recordThresholdPotential=Flag(object["record_threshold_potential"]);
    const auto policy=String(object["reset_policy"]);
    Require(policy=="reset_per_trial" || policy=="preserve_neuron_dynamics","invalid_calibration_proof_reset_policy");
    q.resetPolicy=policy=="reset_per_trial"?RecognitionResetPolicy::PerTrial:RecognitionResetPolicy::PreserveNeuronDynamics;
    const auto pattern=Array(object["pattern_isi"]);
    Require(!pattern.isEmpty() && pattern.size()<=10000,"invalid_calibration_proof_pattern_dimension");
    q.pattern.Resize(pattern.size(),1);for(int i=0;i<pattern.size();++i)q.pattern(i,0)=Number(pattern[i]);
    Require(Request(q)==object,"calibration_proof_request_encoding_mismatch");return q;
}
QJsonObject Matrix(const NativeCalibrationMatrix& m)
{
    QJsonArray requests,observations;
    for(const auto& q:m.requests)requests.append(Request(q));
    for(const auto& o:m.observations)observations.append(NativeRecognitionTrialJson(o));
    return {{"requests",requests},{"observations",observations}};
}
NativeCalibrationMatrix ReadMatrix(const QJsonObject& object)
{
    const auto requests=Array(object["requests"]),observations=Array(object["observations"]);
    Require(requests.size()>=2 && requests.size()<=10000 && observations.size()==requests.size(),
        "incomplete_calibration_proof_matrix");
    NativeCalibrationMatrix result;std::map<std::pair<std::string,std::string>,std::size_t> indices;
    for(const auto& value:requests)
    {
        auto q=ReadRequest(Object(value));
        Require(indices.emplace(std::make_pair(q.runId,q.trialId),result.requests.size()).second,
            "duplicate_calibration_proof_request");result.requests.push_back(std::move(q));
    }
    for(const auto& value:observations)
    {
        const auto o=Object(value);const auto found=indices.find({String(o["run_id"]),String(o["trial_id"])});
        Require(found!=indices.end(),"unexpected_calibration_proof_observation");
        result.observations.push_back(DecodeNativeRecognitionObservation(o,result.requests[found->second]));
    }
    Require(Matrix(result)==object,"calibration_proof_matrix_encoding_mismatch");return result;
}
QJsonObject Spec(const NativeThresholdConfirmationSpec& s)
{
    return {{"protocol",Text(s.protocol)},{"data_partition_id",Text(s.dataPartitionId)},
        {"attempt_id",Text(s.attemptId)},{"clock_id",Text(s.clockId)},
        {"criterion",QJsonObject{{"anchor",s.window.anchor==CalibrationAnchor::Onset?"onset":"last_stimulus"},
            {"begin_offset",s.window.beginOffset},{"end_offset",s.window.endOffset?QJsonValue(*s.window.endOffset):QJsonValue(QJsonValue::Null)},
            {"reject_target_outside_window",s.window.rejectTargetOutsideWindow}}}};
}
NativeThresholdConfirmationSpec ReadSpec(const QJsonObject& object)
{
    NativeThresholdConfirmationSpec s;s.protocol=String(object["protocol"]);s.dataPartitionId=String(object["data_partition_id"]);
    s.attemptId=String(object["attempt_id"]);s.clockId=String(object["clock_id"]);
    const auto criterion=Object(object["criterion"]);const auto anchor=String(criterion["anchor"]);
    Require(anchor=="onset" || anchor=="last_stimulus","unknown_calibration_anchor");
    s.window.anchor=anchor=="onset"?CalibrationAnchor::Onset:CalibrationAnchor::LastStimulus;
    s.window.beginOffset=Number(criterion["begin_offset"]);
    if(!criterion["end_offset"].isNull())s.window.endOffset=Number(criterion["end_offset"]);
    s.window.rejectTargetOutsideWindow=Flag(criterion["reject_target_outside_window"]);
    Require(Spec(s)==object,"calibration_proof_spec_encoding_mismatch");return s;
}
QJsonObject Proof(const QJsonObject& candidate,const NativeThresholdConfirmationSpec& spec,
    const NativeCalibrationMatrix& search,const NativeCalibrationMatrix& confirmation,
    const PreparedNativeRecognitionState& model)
{
    ReadSpec(Spec(spec));
    Require(search.requests.size()>=2 && search.requests.size()<=10000 &&
        confirmation.requests.size()>=2 && confirmation.requests.size()<=10000,"incomplete_calibration_proof_matrix");
    for(const auto* matrix:{&search,&confirmation})for(const auto& q:matrix->requests)ReadRequest(Request(q));
    const auto calibration=Object(Object(candidate["neuron"])["metadata"])["calibration"].toObject();
    const auto path=String(calibration["ltzone_path"]);
    Require(path.rfind("$.",0)==0,"invalid_calibrated_ltzone_path");
    auto zone=model.neuron->GetComponentL<NPulseLTZoneCommon>(path.substr(2),true);
    Require(bool(zone),"missing_calibrated_ltzone");
    const std::size_t channels=model.configuration.model.branch?1:model.parameters.size();
    for(const auto* matrix:{&search,&confirmation})for(const auto& o:matrix->observations)
    {
        Require(!o.samples.empty() && o.samples.front().somaPotentials.size()==channels,
            "native_confirmation_soma_binding_mismatch");
        if(o.request.recordThresholdPotential)Require(o.thresholdPotentialPath==path+".Potential" &&
            o.thresholdPotentialClass==zone->GetCompClassName(),"native_confirmation_threshold_binding_mismatch");
    }
    Require((spec.protocol=="historical_same_matrix" || spec.protocol=="frozen_independent_calibration") &&
        !spec.dataPartitionId.empty() && !spec.attemptId.empty() && !spec.clockId.empty(),"invalid_native_confirmation_spec");
    const auto source=Object(candidate["source_recognition"]),solo=Object(source["source_solo"]);
    const auto target=String(Object(solo["protocol"])["pattern_hash"]);
    const auto first=AssessNativeThresholdResponses(search.requests,search.observations,target,spec.window);
    const auto fresh=AssessNativeThresholdResponses(confirmation.requests,confirmation.observations,target,spec.window);
    Require(first.criterionMet && fresh.criterionMet,"native_confirmation_criterion_not_met");
    Require(first.evidence["model_revision"]==candidate["model_revision"] &&
        fresh.evidence["model_revision"]==candidate["model_revision"],"native_confirmation_model_changed");
    Require(search.requests.size()==confirmation.requests.size() &&
        search.requests.front().runId!=confirmation.requests.front().runId,"native_confirmation_requires_fresh_run");
    for(std::size_t i=0;i<search.requests.size();++i)
    {
        auto a=Request(search.requests[i]),b=Request(confirmation.requests[i]);
        a.remove("run_id");a.remove("trial_id");b.remove("run_id");b.remove("trial_id");
        Require(a==b,"native_confirmation_design_changed");
    }
    double searchEnd=search.observations.front().end,confirmationBegin=confirmation.observations.front().begin;
    for(const auto& o:search.observations)searchEnd=std::max(searchEnd,o.end);
    for(const auto& o:confirmation.observations)confirmationBegin=std::min(confirmationBegin,o.begin);
    Require(Detail::After(confirmationBegin,searchEnd),"native_confirmation_reused_or_overlapping_measurements");
    Require(search.observations.front().frequencyHz==model.configuration.model.frequencyHz &&
        confirmation.observations.front().frequencyHz==model.configuration.model.frequencyHz &&
        search.observations.front().samples.front().somaPotentials.size()==confirmation.observations.front().samples.front().somaPotentials.size(),
        "native_confirmation_physical_context_changed");
    return Signed({{"kind","native_selected_threshold_confirmation"},{"schema_version",1},
        {"algorithm","selected-candidate-fresh-confirmation-v1"},{"specification",Spec(spec)},
        {"candidate_sha256",candidate["sha256"]},{"model_revision",candidate["model_revision"]},
        {"search",Matrix(search)},{"confirmation",Matrix(confirmation)},
        {"search_assessment",first.evidence},{"confirmation_assessment",fresh.evidence},
        {"calibrated",true},{"grid_selection_verified",false},{"execution_attested",false},
        {"statistical_independence_verified",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}});
}
QJsonObject Package(const QJsonObject& candidate,const QJsonObject& proof)
{
    auto archive=Object(candidate["neuron"]),metadata=Object(archive["metadata"]),calibration=Object(metadata["calibration"]);
    calibration["status"]="calibrated";calibration["protocol"]=Object(proof["specification"])["protocol"];
    calibration["confirmation_sha256"]=proof["sha256"];metadata["calibration"]=calibration;
    archive["metadata"]=metadata;archive=Signed(archive);
    return Signed({{"kind","timelearner_calibrated_recognition_state"},{"schema_version",1},{"dynamic_checkpoint",false},
        {"state_kind","recognition"},{"implementation_id",candidate["implementation_id"]},
        {"source_candidate",candidate},{"calibration_proof",proof},{"threshold",candidate["threshold"]},
        {"model_revision",candidate["model_revision"]},{"recognition_certificate",QJsonValue(QJsonValue::Null)},
        {"dataset",candidate["dataset"]},{"neuron",archive}});
}
void CheckThresholdRole(PreparedNativeRecognitionState& model,const QJsonObject& candidate)
{
    const auto calibration=Object(Object(candidate["neuron"])["metadata"])["calibration"].toObject();
    const auto path=String(calibration["ltzone_path"]);
    Require(path.rfind("$.",0)==0,"invalid_calibrated_ltzone_path");
    auto zone=model.neuron->GetComponentL<NPulseLTZoneCommon>(path.substr(2),true);
    Require(bool(zone),"missing_calibrated_ltzone");RequireNativeScalarSpikeThreshold(*zone);
}
}
QJsonObject EncodeNativeCalibrationRequest(const NativeRecognitionTrialRequest& request){return Request(request);}
QJsonObject EncodeNativeCalibrationMatrix(const NativeCalibrationMatrix& matrix){return Matrix(matrix);}
NativeRecognitionTrialRequest DecodeNativeCalibrationRequest(const QJsonObject& request){return ReadRequest(request);}
QJsonObject EncodeNativeThresholdConfirmationSpec(const NativeThresholdConfirmationSpec& specification){return Spec(specification);}
NativeThresholdConfirmationSpec DecodeNativeThresholdConfirmationSpec(const QJsonObject& specification){return ReadSpec(specification);}
QJsonObject CreateNativeCalibratedState(const QJsonObject& candidate,const NativeThresholdConfirmationSpec& spec,
    const NativeCalibrationMatrix& search,const NativeCalibrationMatrix& confirmation,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation)
{
    auto model=PrepareNativeThresholdCandidate(candidate,storage,environment,implementation);
    CheckThresholdRole(*model,candidate);
    return Package(candidate,Proof(candidate,spec,search,confirmation,*model));
}
std::unique_ptr<PreparedNativeRecognitionState> PrepareNativeCalibratedState(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation)
{
    const auto candidate=Object(package["source_candidate"]),proof=Object(package["calibration_proof"]);
    auto prepared=PrepareNativeThresholdCandidate(candidate,storage,environment,implementation);
    CheckThresholdRole(*prepared,candidate);
    const auto expected=Package(candidate,Proof(candidate,ReadSpec(Object(proof["specification"])),
        ReadMatrix(Object(proof["search"])),ReadMatrix(Object(proof["confirmation"])),*prepared));
    Require(package==expected,"native_calibrated_state_derivation_mismatch");
    auto generator=prepared->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    prepared->neuron->BreakLinks();storage->ReturnObject(prepared->neuron);prepared->neuron=0;
    prepared->neuron=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(Object(package["neuron"]),storage,
        environment,Text(implementation),{{&generator->Output,"burst"}}));
    Require(bool(prepared->neuron),"native_calibrated_archive_not_neuron");
    prepared->calibrationConfirmed=true;return prepared;
}
}
