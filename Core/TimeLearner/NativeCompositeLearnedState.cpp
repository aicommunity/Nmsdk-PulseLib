#include "NativeCompositeLearnedState.h"
#include "CompositeTrainingDeclarations.h"
#include "CandidateWindowTrainingDeclarations.h"
#include "CandidateWindowRecoveryLedger.h"
#include <QJsonArray>
#include <algorithm>
#include <QJsonDocument>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
bool RequiresCandidateWindowRecoveryBoundary(const NativeTrainingService& service) {
    return service.Plan().protocol.schemaVersion==5 &&
        (service.CompositeInitialConfiguration()["schema_version"]==QJsonValue(8) ||
         service.Configuration().invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
         !service.CandidateWindowRecoveryDeliveries().empty() || !service.CandidateWindowRecoveryDeliveryLogIntact());
}
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& value){Require(value.isObject(),"composite_package_object_required");return value.toObject();}
QJsonArray Array(const QJsonValue& value){Require(value.isArray(),"composite_package_array_required");return value.toArray();}
std::string Text(const QJsonValue& value){Require(value.isString(),"composite_package_text_required");return value.toString().toStdString();}
QString Text(const std::string& value){return QString::fromStdString(value);}
std::size_t Count(const QJsonValue& value){Require(value.isDouble(),"composite_package_count_required");const auto n=value.toDouble();
    Require(std::isfinite(n) && n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n,
        "composite_package_count_required");return std::size_t(n);}
QJsonValue Count(std::size_t value){Require(value<=9007199254740991ULL,"composite_package_inexact_count");return double(value);}
void Keys(const QJsonObject& object,const char* fields){const auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);const auto keys=object.keys();
    Require(std::set<QString>(names.begin(),names.end())==std::set<QString>(keys.begin(),keys.end()),"composite_package_fields");}
TrainingStatus Terminal(const QJsonValue& value){const auto text=Text(value);for(const auto state:{TrainingStatus::BudgetExhausted,
    TrainingStatus::ExternalStop,TrainingStatus::InvalidModel,TrainingStatus::InvalidObservation,TrainingStatus::SearchUnresolved})
    if(text==TrainingStatusName(state))return state;throw std::invalid_argument("composite_package_stopped_outcome");}
QJsonArray Parameters(const std::vector<InputParameters>& values){QJsonArray result;for(const auto& p:values)
    result.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});return result;}
std::vector<InputParameters> Parameters(const QJsonValue& value,const NativeModelSpec& model){std::vector<InputParameters> result;
    const auto values=Array(value);Require(values.size()==int(model.inputs.size()),"composite_package_input_dimension");for(const auto& item:values) {
        const auto p=Object(item);Keys(p,"length synapses resistance");const auto l=Count(p["length"]),n=Count(p["synapses"]);const auto r=p["resistance"];
        Require(l>=1 && l<=std::size_t(model.editPolicy.maximumCableLength) && n>=1 && n<=128 && r.isDouble() &&
            std::isfinite(r.toDouble()) && r.toDouble()>0.,"composite_package_input_parameters");result.push_back({int(l),int(n),r.toDouble()});
    }return result;}
NativeTrainingPlan Plan(const ConfiguredNativeTraining& c,const VerificationProtocol& protocol,const QJsonObject& runtime) {
    Require(bool(c.composite),"composite_package_requires_cold6");const auto canonical=MakeConfiguredNativeTrainingPlan(c);const auto& p=canonical.protocol;
    Require(protocol.schemaVersion==4 && protocol.routing==p.routing && protocol.revision==p.revision && protocol.sampleId==p.sampleId &&
        protocol.patternHash==p.patternHash && protocol.inputs.size()==p.inputs.size() && protocol.stimulusIds==p.stimulusIds &&
        protocol.originStimulusId==p.originStimulusId && protocol.maximumSampleGap==p.maximumSampleGap && protocol.minimumAmplitude==p.minimumAmplitude &&
        protocol.stimulusProfile==p.stimulusProfile && protocol.waveAssignment==p.waveAssignment && protocol.peakSelection==p.peakSelection &&
        protocol.compositeMeasurement==p.compositeMeasurement,"composite_package_protocol_configuration_mismatch");
    auto result=DecodeCompositeRuntimePlan(runtime,protocol);result.probe=canonical.probe;result.inputWindows=canonical.inputWindows;
    Require(result.composite && result.search.normalization==c.model.normalization &&
        result.composite->contextPolicy==protocol.compositeMeasurement->contextPolicy && result.composite->geometryPolicy==protocol.compositeMeasurement->geometryPolicy &&
        result.composite->methodId==protocol.compositeMeasurement->methodId,"composite_package_runtime_method_changed");
    for(std::size_t i=0;i<protocol.inputs.size();++i){result.inputWindows[i].inputId=protocol.inputs[i].id;result.inputWindows[i].stimulusId=protocol.inputs[i].stimulusId;}
    return result;
}
void CheckRuntime(const NativeTrainingPlan& plan,const DecodedNativeCompositeArchive& archive) {
    Require(plan.composite && Count(archive.Document()["maximum_total_primary_bytes"])==plan.composite->maximumTotalPrimaryBytes,
        "composite_package_archive_primary_limit");
    bool first=true;for(const auto& record:archive.Records())if(record.attemptId==archive.CurrentAttemptId()) {
        const auto& a=record.attemptLimits;const auto& p=*plan.composite;
        Require(a.maximumGroups==p.maximumGroups && a.maximumNativeProbes==p.maximumNativeProbes && a.maximumTotalSamples==p.maximumTotalSamples &&
            a.maximumModelSeconds==plan.maximumModelSeconds && a.maximumWallSeconds==plan.maximumWallSeconds &&
            record.maximumGroupModelSeconds<=p.maximumGroupModelSeconds && record.maximumGroupWallSeconds<=p.maximumGroupWallSeconds &&
            record.policy.maximumTotalPrimaryBytes==p.maximumTotalPrimaryBytes,"composite_package_runtime_attempt_limits");
        if(first){Require(record.logical.baseModelRevision==plan.search.initialModelRevision && record.logical.epoch==plan.search.initialModelEpoch,
            "composite_package_runtime_initial_model");first=false;}
    }
}
void CheckDataset(const ConfiguredNativeTraining& c,RDK::UEPtr<NDatasetMatrix> dataset,const QJsonObject& source) {
    Require(dataset && dataset->MatrixData.GetRows()==int(c.model.initialIsi.size()) && dataset->MatrixData.GetCols()==1,"composite_package_dataset_dimension");
    for(std::size_t i=0;i<c.model.initialIsi.size();++i)Require(dataset->MatrixData(int(i),0)==c.model.initialIsi[i],"composite_package_source_isi_changed");
    const auto identity=NmsdkTools::IdentifyModel(dataset.Get());Require(identity["valid"]==QJsonValue(true) &&
        identity["model"]==Object(Object(source["initial_dataset"])["identity"])["model"],"composite_package_restored_source_changed");
    auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(generator),"composite_package_missing_source");
    const StimulusProfile actual{generator->GetCompClassName(),generator->Amplitude.GetData(),generator->PulseLength.GetData(),double(generator->GetTimeStep()),
        generator->FrequencyDeviation.GetData(),generator->UseTransitSignal.GetData(),generator->UsePatternOutput.GetData()};
    Require(actual==c.stimulusProfile,"composite_package_source_profile_changed");
}
void CheckConfiguration(const ConfiguredNativeTraining& c,const QJsonObject& source) {
    auto configured=EncodeNativeModelSpec(c.model),original=Object(source["model_spec"]);configured.remove("inputs");original.remove("inputs");
    Require(configured==original,"composite_package_initial_creation_policy_changed");
    const auto isi=Array(source["applied_source_isi"]);Require(isi.size()==int(c.model.initialIsi.size()),"composite_package_initial_isi_dimension");
    for(std::size_t i=0;i<c.model.initialIsi.size();++i)Require(isi.at(int(i))==QJsonValue(c.model.initialIsi[i]),"composite_package_initial_isi_changed");
}
void CheckC0(const QJsonObject& source,const QJsonValue& state,const NativeProbeBindings& bindings,const NativeModelSpec& declaration) {
    const auto initial=source["optional_C0_witness"];
    Require(initial.isNull()==state.isNull(),"composite_package_C0_lost_or_invented");if(state.isNull())return;
    const auto actual=Object(state),before=Object(Object(initial)["native_state"]);const auto baseline=Object(actual["baseline"]),old=Object(before["baseline"]);
    Require(actual["maximum_relative_change"]==before["maximum_relative_change"],"composite_package_C0_policy_changed");
    for(const char* key:{"model_spec","model_revision","capture_model","storage_images_sha256"})
        Require(baseline[key]==old[key],"composite_package_C0_recaptured");
    ValidateAppliedNativeCapacitanceState(bindings,declaration,actual);
}
std::pair<QString,double> Threshold(const QJsonObject& identity) {
    const auto model=Object(identity["model"]);QString path;for(const auto& value:Array(model["caches"]))
        if(!Object(value)["cached_ltzone"].toString().isEmpty()){path=Object(value)["cached_ltzone"].toString();break;}
    Require(!path.isEmpty(),"composite_package_missing_ltzone");for(const auto& value:Array(model["nodes"]))if(Object(value)["path"]==QJsonValue(path)) {
        const auto data=Object(Object(Object(Object(value)["parameters"])["Threshold"])["data"]);bool ok=false;
        const auto hex=data["value"].toString();const std::uint64_t bits=hex.toULongLong(&ok,16);double threshold;std::memcpy(&threshold,&bits,sizeof(threshold));
        Require(ok && hex.size()==16 && data["type"]==QJsonValue("binary64") && std::isfinite(threshold),"composite_package_threshold");return {path,threshold};
    }throw std::invalid_argument("composite_package_missing_threshold");
}
QJsonArray References(const std::optional<CapturedReferences>& refs){QJsonArray result;if(refs)for(const auto& input:refs->Inputs())result.append(input.rawPotential);return result;}
template<class Proof> QJsonObject Metadata(const ConfiguredNativeTraining& c,const NativeTrainingPlan& plan,const Proof& proof,
    const std::string& implementation,const std::string& status,const std::string& reason,bool stopped,const QJsonObject& identity) {
    const auto [path,threshold]=Threshold(identity);QJsonArray target;for(double isi:c.model.initialIsi)target.append(isi);
    QJsonObject training{{"status",Text(status)},{"target_id",Text(plan.protocol.sampleId)},{"trainer",c.model.branch?"Branch":"Classic"},
        {"normalization",plan.search.normalization==Normalization::SynapseCount?"N0":"N1"},{"target_pattern",target},
        {"captured_per_input",References(proof.References())},{"certificate",stopped?QJsonValue(QJsonValue::Null):
            QJsonValue(QJsonObject{{"state_identity",Text(proof.ModelRevision())},{"status",Text(status)}})}};
    if(stopped){training["reference_complete"]=bool(proof.References());training["capture_revision"]=proof.Archive().SourceBinding()["initial_model"].toObject()["sha256"];
        training["capture_epoch"]=proof.Archive().SourceBinding()["initial_epoch"];training["reason"]=Text(reason);}
    if(plan.search.capacitanceSearch)training["capacitance_search"]=EncodeCapacitanceSearchPolicy(*plan.search.capacitanceSearch);
    return {{"implementation_id",Text(implementation)},{"state_kind",stopped?"diagnostic":"learned_solo"},{"training",training},
        {"calibration",QJsonObject{{"status","fixed"},{"protocol","fixed-trained-graph"},{"ltzone_path",path},{"threshold",threshold}}},
        {"conversion",QJsonObject{{"mode","none"}}}};
}
}
QJsonObject NativeCompositeStateFactory::Export(RDK::UNet& owner,const NativeModelSpec& declaration,const NativeTrainingService& service,
    const std::vector<InputParameters>& actual,int version,int mode,int structure,const std::string& implementation,const NativeCapacitanceCorrection* capacitance) {
    Require(service.Plan().protocol.schemaVersion==4 && service.State()==NativeTrainingState::Finished && service.SessionAttempt()>0 &&
        version==2 && (mode==0 || mode==1) && (structure==0 || structure==1) && !implementation.empty(),"composite_export_requires_stopped_or_completed_boundary");
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    Require(neuron && dataset,"composite_package_missing_graph");auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"composite_package_missing_source");const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};
    const auto identity=NmsdkTools::IdentifyModel(neuron.Get(),ports);Require(identity["valid"]==QJsonValue(true),"composite_package_invalid_model");
    const auto revision=Text(identity["sha256"]);const auto epoch=service.ModelEpoch();const bool stopped=!service.Certificate();
    const auto c=DecodeConfiguredNativeTraining(service.CompositeInitialConfiguration());CheckConfiguration(c,service.CompositeSourceBinding());
    CheckDataset(c,dataset,service.CompositeSourceBinding());Require(QJsonValue(EncodeNativeModelSpec(declaration))==service.CompositeSourceBinding()["model_spec"],"composite_package_source_declaration_changed");
    auto archive=EncodeNativeCompositeTrainingArchive(service);auto proof=CompositePortableTrainingProof::Replay(
        DecodeNativeCompositeTrainingArchive(archive.document),service.Configuration().tolerances,revision,epoch);
    Require(bool(proof.References())==bool(service.References()),"composite_package_reference_completeness");
    if(service.References())Require(proof.References()->Inputs()==service.References()->Inputs(),"composite_package_reference_readback");
    if(stopped){Terminal(QJsonValue(Text(std::string(TrainingStatusName(service.Outcome())))));Require(!service.Reason().empty(),"composite_package_stopped_reason");}
    else Require(proof.Certificate() && EncodeTrainingCertificate(*proof.Certificate())==EncodeTrainingCertificate(*service.Certificate()) &&
        service.Certificate()->Matches(revision,*service.References(),service.Configuration().tolerances,ModelStateKind::LearnedSolo,epoch),"composite_package_certificate_readback");
    std::vector<int> attachments;for(const auto& input:actual)attachments.push_back(input.length);
    const auto inspected=InspectNativeLearnerModel(owner,declaration,attachments);Require(inspected.status==NativeModelBuildStatus::Adopted &&
        inspected.modelRevision==revision && inspected.effective==actual,"composite_package_actual_parameter_readback");
    NativeProbeBindings bindings;bindings.neuron=neuron;bindings.dataset=dataset;bindings.branch=declaration.branch;bindings.inputCount=int(actual.size());
    QJsonValue c0=capacitance?QJsonValue(EncodeNativeCapacitanceState(*capacitance,revision)):QJsonValue(QJsonValue::Null);
    CheckC0(service.CompositeSourceBinding(),c0,bindings,declaration);
    std::vector<std::string> images;for(const auto& image:service.CompositeSourceBinding()["storage_images"].toArray())images.push_back(Text(Object(image)["name"]));
    const auto status=std::string(TrainingStatusName(service.Outcome()));const auto metadata=Metadata(c,service.Plan(),proof,implementation,status,service.Reason(),stopped,identity);
    const auto physical=NmsdkTools::ExportModelArchive(neuron.Get(),metadata,ports,images);
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(Array(physical["class_definitions"]))==service.CompositeSourceBinding()["storage_images_sha256"].toString(),
        "composite_package_storage_changed");
    QJsonObject package{{"kind",stopped?"timelearner_stopped_state":"timelearner_completed_state"},{"schema_version",5},{"dynamic_checkpoint",false},
        {"state_kind",stopped?"diagnostic":"learned_solo"},{"implementation_id",Text(implementation)},{"controller_version",version},{"calculate_mode",mode},
        {"structure_build_mode",structure},{"configuration",service.CompositeInitialConfiguration()},{"runtime_plan",EncodeCompositeRuntimePlan(service.Plan())},
        {"protocol",EncodeVerificationProtocol(service.Plan().protocol)},{"trained_inputs",Parameters(actual)},{"model_revision",Text(revision)},
        {"model_epoch",Count(epoch)},{"session_attempt",Count(service.SessionAttempt())},{"training_outcome",Text(status)},{"reason",Text(service.Reason())},
        {"certificate",stopped?QJsonValue(QJsonValue::Null):QJsonValue(EncodeTrainingCertificate(*proof.Certificate()))},
        {"capture_revision",service.CompositeSourceBinding()["initial_model"].toObject()["sha256"]},{"capture_epoch",service.CompositeSourceBinding()["initial_epoch"]},
        {"reference_complete",bool(proof.References())},{"measurement_archive",archive.document},{"dataset",NmsdkTools::ExportDatasetSnapshot(dataset.Get())},
        {"neuron",physical},{"capacitance_state",c0}};
    package["sha256"]=NmsdkTools::CanonicalModelDataSha256(package);return package;
}
namespace {
template<unsigned Protocol,unsigned Summary> QJsonObject CompositeSummary(const NativeTrainingService& service,const std::string& revision,
    const std::vector<InputParameters>& parameters,const QJsonObject* checkedRecoveryLedger=nullptr) {
    static_assert((Protocol==4 && Summary==6) || (Protocol==5 && (Summary==7 || Summary==8)));
    const auto& plan=service.Plan();Require(plan.protocol.schemaVersion==Protocol && plan.composite,"composite_summary_protocol");
    if constexpr(Summary!=8)Require(!RequiresCandidateWindowRecoveryBoundary(service),
        "candidate_window_recovery_requires_versioned_summary_ledger");
    const auto& records=service.CompositeMeasurements();const auto find=[&](const std::string& id,bool trial=false)->const NativeCompositeMeasurementRecord& {
        const auto found=std::find_if(records.begin(),records.end(),[&](const auto& r){return (trial?r.logical.trialId:r.groupId)==id;});
        Require(found!=records.end() && found->consumed && !found->semanticProofId.empty(),"composite_summary_group_missing");return *found;
    };
    QJsonArray refs,referenceIds,finalIds,referenceProofs,finalProofs;std::size_t raw=0,samples=0;
    for(const auto& r:records){Require(raw<=9007199254740991ULL-r.issuedNativeProbes && samples<=9007199254740991ULL-r.sampledFrames,
        "composite_summary_counter_overflow");raw+=r.issuedNativeProbes;samples+=r.sampledFrames;}
    for(const auto& id:service.CompositeReferenceGroupIds()) {const auto& r=find(id);referenceIds.append(Text(id));referenceProofs.append(Text(r.semanticProofId));
        refs.append(QJsonObject{{"input_id",Text(r.logical.inputId)},{"run_id",Text(r.logical.runId)},{"trial_id",Text(r.logical.trialId)},
            {"effective_model_revision",Text(r.group.specification.effectiveModelRevision)},{"raw_potential",r.selection->geometry->measuredMaximum->rawPotential},
            {"measurement_group_id",Text(r.groupId)},{"semantic_proof_id",Text(r.semanticProofId)}});}
    for(const auto& id:service.CompositeFinalGroupIds()){const auto& r=find(id);finalIds.append(Text(id));finalProofs.append(Text(r.semanticProofId));}
    QJsonObject counters{{"attempt",QJsonObject{{"logical_groups",Count(service.CompositeGroups())},{"native_probes",Count(service.CompositeNativeProbes())},
            {"sampled_frames",Count(service.CompositeSampledFrames())}}},
        {"archive",QJsonObject{{"groups",Count(records.size())},{"native_probes",Count(raw)},{"sampled_frames",Count(samples)},
            {"primary_bytes",Count(service.CompositePrimaryBytes())}}}};
    QJsonArray candidates,rejections,domains;const auto search=service.Search();
    if(search) {
        for(const auto& e:search->Evidence()){const auto& current=find(e.trialId,true);const auto& anchor=find(e.anchorTrialId,true);
            QJsonObject c{{"input_index",Count(e.inputIndex)},{"model_epoch",Count(e.modelEpoch)},{"parameters",Parameters({e.parameters}).at(0)},
                {"full_parameters",Parameters(e.fullParameters)},{"model_revision",Text(e.modelRevision)},{"trial_id",Text(e.trialId)},
                {"anchor_trial_id",Text(e.anchorTrialId)},{"timing_low",e.timingResidualLow},{"timing_high",e.timingResidualHigh},
                {"amplitude_residual",e.amplitudeResidual},{"within_timing",e.withinTiming},{"within_amplitude",e.withinAmplitude},
                {"parameter_change_cost",e.parameterChangeCost},{"measurement_group_id",Text(current.groupId)},{"semantic_proof_id",Text(current.semanticProofId)},
                {"anchor_group_id",Text(anchor.groupId)},{"anchor_semantic_proof_id",Text(anchor.semanticProofId)}};
            if(e.capacitance){const auto witness=Object(ParseStrictNativeJsonObject(current.candidateBindingJson)["optional_C0_witness"]);
                const auto checked=DecodeNativeTrainingCapacitanceWitness(witness);Require(checked.parameters==*e.capacitance,"composite_summary_candidate_C0_changed");
                c["capacitance"]=witness["controller_parameters"];}
            candidates.append(c);
        }
        for(const auto& r:search->Rejected())rejections.append(QJsonObject{{"trial_id",Text(r.trialId)},{"reason",Text(r.reason)}});
        for(const auto& d:search->Domains())domains.append(EncodeResistanceDomainAssessment(d));
    }
    const auto& t=plan.search.tolerances;const auto& source=service.CompositeSourceBinding();
    const auto certificate=service.Certificate();const bool matches=certificate && service.References() &&
        certificate->Matches(revision,*service.References(),t,ModelStateKind::LearnedSolo,service.ModelEpoch());
    const auto runtime=[&](){if constexpr(Protocol==4)return EncodeCompositeRuntimePlan(plan);else return EncodeCandidateWindowRuntimePlan(plan);}();
    QJsonObject summary{{"kind","native_training_state_summary"},{"schema_version",int(Summary)},{"state",NativeTrainingStateName(service.State())},
        {"outcome",TrainingStatusName(service.Outcome())},{"reason",Text(service.Reason())},{"model_state_kind",certificate?"learned_solo":"diagnostic"},
        {"actual_model_revision",Text(revision)},{"model_epoch",Count(service.ModelEpoch())},{"actual_parameters",Parameters(parameters)},
        {"tolerances",QJsonObject{{"amplitude_absolute",t.amplitudeAbsolute},{"amplitude_relative",t.amplitudeRelative},{"sync",t.sync}}},
        {"runtime_plan",runtime},{"measurement_method",DeclaredWindowBasinResult::MethodId()},
        {"source_origin",NmsdkTools::CanonicalModelDataSha256(source)},{"owner_family_binding",Text(service.CompositeOwnerFamilyBinding())},
        {"reference_complete",bool(service.References())},{"capture_revision",Object(source["initial_model"])["sha256"]},{"capture_epoch",source["initial_epoch"]},
        {"reference_group_ids",referenceIds},{"final_group_ids",finalIds},{"reference_semantic_proof_ids",referenceProofs},{"final_semantic_proof_ids",finalProofs},
        {"composite_counters",counters},{"certificate",certificate?QJsonValue(EncodeTrainingCertificate(*certificate)):QJsonValue(QJsonValue::Null)},
        {"certificate_matches_actual_model",matches},{"contains_evaluation_verdict",false},{"search_history_retained",bool(search)},
        {"candidates",candidates},{"rejections",rejections},{"resistance_domains",domains},{"references",refs},
        {"accepted_parameters",search?QJsonValue(Parameters(search->Parameters())):QJsonValue(QJsonValue::Null)},
        {"applied_edits",search?Count(search->AppliedEdits()):QJsonValue(QJsonValue::Null)},
        {"search_probes",search?Count(search->IssuedProbes()):QJsonValue(QJsonValue::Null)}};
    if constexpr(Summary==8) {
        const auto initial=DecodeConfiguredNativeTraining(service.CompositeInitialConfiguration());
        Require(plan.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
            initial.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
            !service.CandidateWindowRecoveryDeliveries().empty(),"recovery_summary_requires_explicit_recovery_family");
        if(checkedRecoveryLedger) {
            // Private factory-only view: ExportImpl already replayed this archive
            // and validated these deliveries at this immutable finished boundary.
            summary["recovery_ledger"]=*checkedRecoveryLedger;
        } else {
            const auto archive=EncodeAndValidateNativeCandidateWindowServiceArchive(service);
            summary["recovery_ledger"]=EncodeCandidateWindowRecoveryLedger(service.CandidateWindowRecoveryDeliveries(),
                archive.second,service.CandidateWindowRecoveryDeliveryLogIntact());
        }
        summary["contains_controller_checkpoint"]=false;
    }
    return summary;
}
}
QJsonObject EncodeNativeCompositeTrainingState(const NativeTrainingService& service,const std::string& revision,
    const std::vector<InputParameters>& parameters) {return CompositeSummary<4,6>(service,revision,parameters);}
QJsonObject EncodeNativeCandidateWindowTrainingState(const NativeTrainingService& service,const std::string& revision,
    const std::vector<InputParameters>& parameters) {
    ValidateCandidateWindowMeasurementContract(service.Plan().protocol);
    Require(service.State()==NativeTrainingState::Finished && !service.CompositeSourceBinding().isEmpty(),"candidate_window_summary_requires_owned_finished_boundary");
    return CompositeSummary<5,7>(service,revision,parameters);
}
QJsonObject EncodeNativeCandidateWindowRecoveryTrainingState(const NativeTrainingService& service,const std::string& revision,
    const std::vector<InputParameters>& parameters) {
    ValidateCandidateWindowMeasurementContract(service.Plan().protocol);
    Require(service.State()==NativeTrainingState::Finished && !service.CompositeSourceBinding().isEmpty(),"candidate_window_summary_requires_owned_finished_boundary");
    return CompositeSummary<5,8>(service,revision,parameters);
}
void NativeCompositeStateFactory::Restore(NativeTrainingService& service,CompositePortableTrainingProof proof,NativeTrainingPlan plan,
    QJsonObject configuration,std::size_t sessions,TrainingStatus outcome,const std::string& reason,bool completed) {
    Require(sessions>0,"composite_package_session_attempt");const auto& a=proof.Archive();
    service.plan=std::move(plan);service.references=proof.References();service.certificate=completed?proof.Certificate():std::optional<TrainingCertificate>{};
    service.referenceBinding=proof.References()?proof.ModelRevision():Text(Object(a.SourceBinding()["initial_model"])["sha256"]);
    service.retainedEpoch=proof.ModelEpoch();service.attempt=sessions;service.outcome=outcome;service.reason=reason;service.state=NativeTrainingState::Finished;
    service.compositeSourceBinding=a.SourceBinding();service.compositeOwnerFamily=a.OwnerFamilyBinding();service.compositeInitialConfiguration=std::move(configuration);
    service.compositeRecords=a.Records();service.compositeReferenceGroups=a.ReferenceGroups();service.compositeFinalGroups=a.FinalGroups();
    if(!proof.References())service.incompleteCaptureConstraint.emplace(Text(Object(a.SourceBinding()["initial_model"])["sha256"]),Count(a.SourceBinding()["initial_epoch"]));
    const auto attempt=Object(Object(a.Document()["counters"])["attempt"]);service.compositeGroups=Count(attempt["logical_groups"]);
    service.compositeNativeProbes=Count(attempt["native_probes"]);service.compositeSampledFrames=Count(attempt["sampled_frames"]);
    service.compositePrimaryBytes=Count(Object(Object(a.Document()["counters"])["archive"])["primary_bytes"]);
    service.compositeImportedProof=std::make_shared<const CompositePortableTrainingProof>(std::move(proof));
}
std::unique_ptr<PreparedNativeLearnedState> NativeCompositeStateFactory::Prepare(const QJsonObject& package,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation) {
    Keys(package,"kind schema_version dynamic_checkpoint state_kind implementation_id controller_version calculate_mode structure_build_mode configuration protocol runtime_plan trained_inputs model_revision model_epoch session_attempt training_outcome reason certificate capture_revision capture_epoch reference_complete measurement_archive dataset neuron capacitance_state sha256");
    auto body=package;body.remove("sha256");Require(Count(package["schema_version"])==5 && package["dynamic_checkpoint"]==QJsonValue(false) &&
        package["sha256"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(body)),"composite_package_schema_or_digest");
    const bool stopped=package["kind"]==QJsonValue("timelearner_stopped_state");Require((stopped && package["state_kind"]==QJsonValue("diagnostic")) ||
        (package["kind"]==QJsonValue("timelearner_completed_state") && package["state_kind"]==QJsonValue("learned_solo")),"composite_package_kind");
    Require(!implementation.empty() && package["implementation_id"]==QJsonValue(Text(implementation)) && Count(package["controller_version"])==2 &&
        Count(package["calculate_mode"])<=1 && Count(package["structure_build_mode"])<=1,"composite_package_control");
    auto result=std::make_unique<PreparedNativeLearnedState>();result->storage=storage;result->configuration=DecodeConfiguredNativeTraining(Object(package["configuration"]));
    const auto& c=result->configuration;const auto protocol=DecodeVerificationProtocol(Object(package["protocol"]));auto plan=Plan(c,protocol,Object(package["runtime_plan"]));
    result->modelRevision=Text(package["model_revision"]);Require(!result->modelRevision.empty(),"composite_package_empty_revision");
    auto proof=CompositePortableTrainingProof::Replay(DecodeNativeCompositeTrainingArchive(Object(package["measurement_archive"])),
        plan.search.tolerances,result->modelRevision,Count(package["model_epoch"]));const auto& source=proof.Archive().SourceBinding();
    CheckConfiguration(c,source);CheckRuntime(plan,proof.Archive());Require(proof.Archive().Protocol()==protocol && proof.Archive().CurrentAttemptId()==plan.search.runId &&
        package["capture_revision"]==Object(source["initial_model"])["sha256"] && Count(package["capture_epoch"])==Count(source["initial_epoch"]) &&
        package["reference_complete"].isBool() && package["reference_complete"]==QJsonValue(bool(proof.References())),"composite_package_capture_context");
    const auto reason=Text(package["reason"]);TrainingStatus outcome;
    if(stopped){outcome=Terminal(package["training_outcome"]);Require(package["certificate"].isNull() && !reason.empty(),"composite_package_stopped_claim");}
    else {Require(proof.Certificate() && package["certificate"]==QJsonValue(EncodeTrainingCertificate(*proof.Certificate())),"composite_package_certificate_claim");
        outcome=proof.Certificate()->Exact()?TrainingStatus::Exact:TrainingStatus::SearchUnresolved;
        Require(package["training_outcome"]==QJsonValue(Text(std::string(TrainingStatusName(outcome)))),"composite_package_completed_outcome");}
    result->sourceDeclaration=DecodeNativeModelSpec(Object(source["model_spec"]));const auto& declaration=*result->sourceDeclaration;
    result->parameters=Parameters(package["trained_inputs"],declaration);if(!stopped)ValidateBoundedTrainingInputs(protocol,plan.search,result->parameters);
    const auto physical=Object(package["neuron"]),identity=Object(Object(physical["graph"])["identity"]);
    Require(identity["sha256"]==package["model_revision"] && NmsdkTools::ModelImageDefinitionsFingerprint(Array(physical["class_definitions"]))==
        source["storage_images_sha256"].toString(),"composite_package_physical_revision_or_images");
    Require(Object(physical["metadata"])==Metadata(c,plan,proof,implementation,Text(package["training_outcome"]),reason,stopped,identity),"composite_package_metadata");
    std::set<QString> declared;for(const auto& image:Array(physical["class_definitions"]))declared.insert(Object(image)["name"].toString());
    for(const auto& cls:Array(Object(package["dataset"])["required_class_names"]))Require(cls.isString() && declared.count(cls.toString()),"composite_package_dataset_dependency");
    NmsdkTools::PrepareModelArchive(physical,storage,environment,Text(implementation));
    result->dataset=RDK::dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(Object(package["dataset"]),storage,environment));
    CheckDataset(c,result->dataset,source);auto generator=result->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};
    // Independently import/read the INITIAL source snapshot: configuration is
    // initial planning data, never a template for the current trained graph.
    auto initial=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportPhysicalGraphForEdit(Object(source["initial_native_snapshot"]),storage,environment,ports));
    Require(bool(initial),"composite_package_initial_neuron");try {
        NativeProbeBindings bindings;bindings.neuron=initial;bindings.dataset=result->dataset;bindings.branch=declaration.branch;bindings.inputCount=int(c.model.inputs.size());
        std::vector<int> positions;std::vector<InputParameters> expected;for(const auto& input:c.model.inputs) {
            Require(bool(input.resistance),"composite_package_initial_explicit_readback_required");positions.push_back(input.attachment);expected.push_back({input.attachment,input.synapses,*input.resistance});}
        Require(ReadNativeInputParameters(bindings,positions)==expected,"composite_package_initial_planning_readback");
        initial->BreakLinks();storage->ReturnObject(initial);initial=nullptr;
    }catch(...){if(initial){initial->BreakLinks();storage->ReturnObject(initial);}throw;}
    result->neuron=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(physical,storage,environment,Text(implementation),ports));
    NativeProbeBindings bindings;bindings.neuron=result->neuron;bindings.dataset=result->dataset;bindings.branch=declaration.branch;bindings.inputCount=int(result->parameters.size());
    std::vector<int> positions;for(const auto& p:result->parameters)positions.push_back(p.length);
    Require(ReadNativeInputParameters(bindings,positions)==result->parameters,"composite_package_current_native_readback");
    CheckC0(source,package["capacitance_state"],bindings,declaration);if(!package["capacitance_state"].isNull())result->capacitanceState=Object(package["capacitance_state"]);
    result->controllerVersion=2;result->calculateMode=int(Count(package["calculate_mode"]));result->structureBuildMode=int(Count(package["structure_build_mode"]));
    result->configurationText=QJsonDocument(EncodeConfiguredNativeTraining(c)).toJson(QJsonDocument::Compact).toStdString();
    Restore(result->service,std::move(proof),std::move(plan),Object(package["configuration"]),Count(package["session_attempt"]),outcome,reason,!stopped);
    return result;
}

namespace {
NativeTrainingPlan CandidateWindowPlan(const ConfiguredNativeTraining& c,const VerificationProtocol& protocol,const QJsonObject& runtime) {
    Require(c.composite && c.composite->schemaVersion==2 && c.candidateWindowPolicy,
        "candidate_window_package_requires_cold7");
    const auto canonical=MakeConfiguredNativeTrainingPlan(c);
    Require(protocol.schemaVersion==5 && EncodeVerificationProtocol(protocol)==EncodeVerificationProtocol(canonical.protocol),
        "candidate_window_package_initial_protocol_mismatch");
    // The immutable initial declaration binds the target/method. Actual warm
    // search bounds/tolerances/caps/model/epoch belong to runtime2, not cold7.
    auto result=DecodeCandidateWindowRuntimePlan(runtime,protocol);result.probe=canonical.probe;result.inputWindows=canonical.inputWindows;
    Require(result.search.normalization==c.model.normalization,"candidate_window_package_normalization_changed");return result;
}
void CandidateWindowRuntime(const NativeTrainingPlan& plan,const DecodedNativeCompositeArchive& archive,
    const std::string& revision,std::size_t epoch) {
    CheckRuntime(plan,archive);bool observed=false;
    for(const auto& r:archive.Records())if(r.attemptId==archive.CurrentAttemptId()){observed=true;break;}
    if(!observed)Require(plan.search.initialModelRevision==revision && plan.search.initialModelEpoch==epoch,
        "candidate_window_package_empty_attempt_model");
}
}
QJsonObject NativeCandidateWindowStateFactory::Export(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& actual,int version,int mode,int structure,
    const std::string& implementation,const NativeCapacitanceCorrection* capacitance) {
    return ExportImpl(owner,declaration,service,actual,version,mode,structure,implementation,capacitance,false);
}
QJsonObject NativeCandidateWindowStateFactory::ExportRecovery(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& actual,int version,int mode,int structure,
    const std::string& implementation,const NativeCapacitanceCorrection* capacitance) {
    return ExportImpl(owner,declaration,service,actual,version,mode,structure,implementation,capacitance,true);
}
NativeTrainingExport NativeCandidateWindowStateFactory::ExportBoundary(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& actual,int version,int mode,int structure,
    const std::string& implementation,const NativeCapacitanceCorrection* capacitance) {
    NativeTrainingExport result;
    result.learnedState=ExportImpl(owner,declaration,service,actual,version,mode,structure,implementation,capacitance,
        RequiresCandidateWindowRecoveryBoundary(service),&result.summary);
    return result;
}
QJsonObject NativeCandidateWindowStateFactory::ExportImpl(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& actual,int version,int mode,int structure,
    const std::string& implementation,const NativeCapacitanceCorrection* capacitance,bool recovery,QJsonObject* trainingSummary) {
    if(!recovery)Require(!RequiresCandidateWindowRecoveryBoundary(service),
        "candidate_window_recovery_requires_versioned_model_ledger");
    Require(service.Plan().protocol.schemaVersion==5 && service.State()==NativeTrainingState::Finished && service.SessionAttempt()>0 &&
        version==2 && (mode==0 || mode==1) && (structure==0 || structure==1) && !implementation.empty(),
        "candidate_window_export_requires_stopped_or_completed_boundary");
    const auto boundaryAttempt=service.SessionAttempt();const auto boundaryOutcome=service.Outcome();const auto boundaryReason=service.Reason();
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    Require(neuron && dataset,"composite_package_missing_graph");auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    Require(bool(generator),"composite_package_missing_source");const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};
    const auto identity=NmsdkTools::IdentifyModel(neuron.Get(),ports);Require(identity["valid"]==QJsonValue(true),"composite_package_invalid_model");
    const auto revision=Text(identity["sha256"]);const auto epoch=service.ModelEpoch();const bool stopped=!service.Certificate();
    const auto c=DecodeConfiguredNativeTraining(service.CompositeInitialConfiguration());const auto& source=service.CompositeSourceBinding();
    CheckConfiguration(c,source);CheckDataset(c,dataset,source);
    Require(QJsonValue(EncodeNativeModelSpec(declaration))==source["model_spec"],"composite_package_source_declaration_changed");
    const auto runtime=EncodeCandidateWindowRuntimePlan(service.Plan());auto plan=CandidateWindowPlan(c,service.Plan().protocol,runtime);
    auto checkedArchive=EncodeAndValidateNativeCandidateWindowServiceArchive(service);
    auto& archive=checkedArchive.first;
    auto proof=CandidateWindowPortableTrainingProof::Replay(std::move(checkedArchive.second),plan.search.tolerances,revision,epoch);
    QJsonObject recoveryLedger;
    if(recovery) {
        Require(plan.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
            c.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
            !service.CandidateWindowRecoveryDeliveries().empty(),"recovery_package_requires_explicit_recovery_family");
        recoveryLedger=EncodeCandidateWindowRecoveryLedger(service.CandidateWindowRecoveryDeliveries(),proof.Archive(),
            service.CandidateWindowRecoveryDeliveryLogIntact());
    }
    CandidateWindowRuntime(plan,proof.Archive(),revision,epoch);
    Require(bool(proof.References())==bool(service.References()),"composite_package_reference_completeness");
    if(service.References())Require(proof.References()->Inputs()==service.References()->Inputs(),"composite_package_reference_readback");
    if(stopped){Terminal(QJsonValue(Text(std::string(TrainingStatusName(service.Outcome())))));Require(!service.Reason().empty(),"composite_package_stopped_reason");}
    else {
        proof.RequireCertificate(EncodeTrainingCertificate(*service.Certificate()));
        Require(service.Outcome()==(service.Certificate()->Exact()?TrainingStatus::Exact:TrainingStatus::SearchUnresolved) &&
            service.Certificate()->Matches(revision,*service.References(),plan.search.tolerances,ModelStateKind::LearnedSolo,epoch),
            "candidate_window_package_live_completed_boundary");
    }
    std::vector<int> attachments;for(const auto& input:actual)attachments.push_back(input.length);
    const auto inspected=InspectNativeLearnerModel(owner,declaration,attachments);
    Require(inspected.status==NativeModelBuildStatus::Adopted && inspected.modelRevision==revision && inspected.effective==actual,
        "composite_package_actual_parameter_readback");
    NativeProbeBindings bindings;bindings.neuron=neuron;bindings.dataset=dataset;bindings.branch=declaration.branch;bindings.inputCount=int(actual.size());
    const QJsonValue c0=capacitance?QJsonValue(EncodeNativeCapacitanceState(*capacitance,revision)):QJsonValue(QJsonValue::Null);CheckC0(source,c0,bindings,declaration);
    std::vector<std::string> images;for(const auto& image:source["storage_images"].toArray())images.push_back(Text(Object(image)["name"]));
    const auto status=std::string(TrainingStatusName(service.Outcome()));
    auto metadata=Metadata(c,plan,proof,implementation,status,service.Reason(),stopped,identity);
    if(recovery)metadata["recovery_ledger_sha256"]=recoveryLedger["sha256"];
    const auto physical=NmsdkTools::ExportModelArchive(neuron.Get(),metadata,ports,images);
    Require(NmsdkTools::ModelImageDefinitionsFingerprint(Array(physical["class_definitions"]))==source["storage_images_sha256"].toString(),
        "composite_package_storage_changed");
    Require(service.State()==NativeTrainingState::Finished && service.SessionAttempt()==boundaryAttempt &&
        service.ModelEpoch()==epoch && service.Outcome()==boundaryOutcome && service.Reason()==boundaryReason &&
        service.CompositeMeasurements().size()==proof.Archive().Records().size() &&
        service.CompositeReferenceGroupIds()==proof.Archive().ReferenceGroups() && service.CompositeFinalGroupIds()==proof.Archive().FinalGroups() &&
        Object(Object(physical["graph"])["identity"])["sha256"]==QJsonValue(Text(revision)),"candidate_window_export_boundary_changed");
    if(recovery && trainingSummary)Require(recoveryLedger==EncodeCandidateWindowRecoveryLedger(
        service.CandidateWindowRecoveryDeliveries(),proof.Archive(),service.CandidateWindowRecoveryDeliveryLogIntact()),
        "candidate_window_export_recovery_deliveries_changed");
    QJsonObject package{{"kind",stopped?"timelearner_stopped_state":"timelearner_completed_state"},{"schema_version",recovery?7:6},{"dynamic_checkpoint",false},
        {"state_kind",stopped?"diagnostic":"learned_solo"},{"implementation_id",Text(implementation)},{"controller_version",version},{"calculate_mode",mode},
        {"structure_build_mode",structure},{"configuration",service.CompositeInitialConfiguration()},{"runtime_plan",runtime},
        {"protocol",EncodeVerificationProtocol(plan.protocol)},{"trained_inputs",Parameters(actual)},{"model_revision",Text(revision)},
        {"model_epoch",Count(epoch)},{"session_attempt",Count(service.SessionAttempt())},{"training_outcome",Text(status)},{"reason",Text(service.Reason())},
        {"certificate",stopped?QJsonValue(QJsonValue::Null):QJsonValue(EncodeTrainingCertificate(*service.Certificate()))},
        {"capture_revision",source["initial_model"].toObject().value("sha256")},{"capture_epoch",source["initial_epoch"]},
        {"reference_complete",bool(proof.References())},{"measurement_archive",archive.document},{"dataset",NmsdkTools::ExportDatasetSnapshot(dataset.Get())},
        {"neuron",physical},{"capacitance_state",c0}};
    if(recovery)package["recovery_ledger"]=recoveryLedger;
    package["sha256"]=NmsdkTools::CanonicalModelDataSha256(package);
    if(trainingSummary)*trainingSummary=recovery?CompositeSummary<5,8>(service,revision,actual,&recoveryLedger):
        CompositeSummary<5,7>(service,revision,actual);
    return package;
}
void NativeCandidateWindowStateFactory::Restore(NativeTrainingService& service,CandidateWindowPortableTrainingProof proof,
    NativeTrainingPlan plan,QJsonObject configuration,std::size_t sessions,TrainingStatus outcome,const std::string& reason,bool completed) {
    Require(sessions>0,"composite_package_session_attempt");const auto& a=proof.Archive();
    service.plan=std::move(plan);service.references=proof.References();service.certificate=completed?proof.Certificate():std::optional<TrainingCertificate>{};
    service.referenceBinding=proof.References()?proof.ModelRevision():Text(Object(a.SourceBinding()["initial_model"])["sha256"]);
    service.retainedEpoch=proof.ModelEpoch();service.attempt=sessions;service.outcome=outcome;service.reason=reason;service.state=NativeTrainingState::Finished;
    service.compositeSourceBinding=a.SourceBinding();service.compositeOwnerFamily=a.OwnerFamilyBinding();service.compositeInitialConfiguration=std::move(configuration);
    service.compositeRecords=a.Records();service.compositeReferenceGroups=a.ReferenceGroups();service.compositeFinalGroups=a.FinalGroups();
    if(proof.WindowReferences())service.compositeCandidateReferences=std::make_shared<const CandidateWindowReferenceSet>(*proof.WindowReferences());
    if(!proof.References())service.incompleteCaptureConstraint.emplace(Text(Object(a.SourceBinding()["initial_model"])["sha256"]),Count(a.SourceBinding()["initial_epoch"]));
    const auto attempt=Object(Object(a.Document()["counters"])["attempt"]);service.compositeGroups=Count(attempt["logical_groups"]);
    service.compositeNativeProbes=Count(attempt["native_probes"]);service.compositeSampledFrames=Count(attempt["sampled_frames"]);
    service.compositePrimaryBytes=Count(Object(Object(a.Document()["counters"])["archive"])["primary_bytes"]);
    // No executor, prepared object, search traversal, token or old callback is
    // restored. A later native Start5 replays this ledger into a NEW owner lease.
}
std::unique_ptr<PreparedNativeLearnedState> NativeCandidateWindowStateFactory::Prepare(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation) {
    return PrepareImpl(package,storage,environment,implementation,false);
}
std::unique_ptr<PreparedNativeLearnedState> NativeCandidateWindowStateFactory::PrepareRecovery(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation) {
    return PrepareImpl(package,storage,environment,implementation,true);
}
std::unique_ptr<PreparedNativeLearnedState> NativeCandidateWindowStateFactory::PrepareImpl(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementation,bool recovery) {
    const char* baseFields="kind schema_version dynamic_checkpoint state_kind implementation_id controller_version calculate_mode structure_build_mode configuration protocol runtime_plan trained_inputs model_revision model_epoch session_attempt training_outcome reason certificate capture_revision capture_epoch reference_complete measurement_archive dataset neuron capacitance_state sha256";
    const auto fields=std::string(baseFields)+(recovery?" recovery_ledger":"");Keys(package,fields.c_str());
    auto body=package;body.remove("sha256");Require(Count(package["schema_version"])==std::size_t(recovery?7:6) && package["dynamic_checkpoint"]==QJsonValue(false) &&
        package["sha256"]==QJsonValue(NmsdkTools::CanonicalModelDataSha256(body)),"candidate_window_package_schema_or_digest");
    const bool stopped=package["kind"]==QJsonValue("timelearner_stopped_state");
    Require((stopped && package["state_kind"]==QJsonValue("diagnostic")) ||
        (package["kind"]==QJsonValue("timelearner_completed_state") && package["state_kind"]==QJsonValue("learned_solo")),"composite_package_kind");
    Require(!implementation.empty() && package["implementation_id"]==QJsonValue(Text(implementation)) && Count(package["controller_version"])==2 &&
        Count(package["calculate_mode"])<=1 && Count(package["structure_build_mode"])<=1,"composite_package_control");
    auto result=std::make_unique<PreparedNativeLearnedState>();result->storage=storage;
    result->configuration=DecodeConfiguredNativeTraining(Object(package["configuration"]));const auto& c=result->configuration;
    const auto protocol=DecodeVerificationProtocol(Object(package["protocol"]));auto plan=CandidateWindowPlan(c,protocol,Object(package["runtime_plan"]));
    result->modelRevision=Text(package["model_revision"]);Require(!result->modelRevision.empty(),"composite_package_empty_revision");
    const auto epoch=Count(package["model_epoch"]);
    auto proof=CandidateWindowPortableTrainingProof::Replay(DecodeNativeCompositeTrainingArchive(Object(package["measurement_archive"])),
        plan.search.tolerances,result->modelRevision,epoch);const auto& source=proof.Archive().SourceBinding();
    std::vector<NativeCandidateWindowRecoveryDelivery> deliveries;
    if(recovery) {
        deliveries=DecodeCandidateWindowRecoveryLedger(Object(package["recovery_ledger"]),proof.Archive());
        Require(plan.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow ||
            c.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow || !deliveries.empty(),
            "recovery_package_requires_explicit_recovery_family");
    } else Require(plan.search.invalidMeasurementRecovery!=InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow &&
        c.search.invalidMeasurementRecovery!=InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow,
        "candidate_window_recovery_requires_versioned_model_ledger");
    CheckConfiguration(c,source);CandidateWindowRuntime(plan,proof.Archive(),result->modelRevision,epoch);
    Require(proof.Archive().Protocol()==protocol && proof.Archive().CurrentAttemptId()==plan.search.runId &&
        package["capture_revision"]==Object(source["initial_model"])["sha256"] && Count(package["capture_epoch"])==Count(source["initial_epoch"]) &&
        package["reference_complete"].isBool() && package["reference_complete"]==QJsonValue(bool(proof.References())),"composite_package_capture_context");
    const auto reason=Text(package["reason"]);TrainingStatus outcome;
    if(stopped){outcome=Terminal(package["training_outcome"]);Require(package["certificate"].isNull() && !reason.empty(),"composite_package_stopped_claim");}
    else {proof.RequireCertificate(Object(package["certificate"]));outcome=proof.Certificate()->Exact()?TrainingStatus::Exact:TrainingStatus::SearchUnresolved;
        Require(package["training_outcome"]==QJsonValue(Text(std::string(TrainingStatusName(outcome)))),"composite_package_completed_outcome");}
    result->sourceDeclaration=DecodeNativeModelSpec(Object(source["model_spec"]));const auto& declaration=*result->sourceDeclaration;
    result->parameters=Parameters(package["trained_inputs"],declaration);if(!stopped)ValidateCandidateWindowTrainingConfiguration(protocol,plan.search,result->parameters);
    const auto physical=Object(package["neuron"]),identity=Object(Object(physical["graph"])["identity"]);
    Require(identity["sha256"]==package["model_revision"] && NmsdkTools::ModelImageDefinitionsFingerprint(Array(physical["class_definitions"]))==
        source["storage_images_sha256"].toString(),"composite_package_physical_revision_or_images");
    auto expectedMetadata=Metadata(c,plan,proof,implementation,Text(package["training_outcome"]),reason,stopped,identity);
    if(recovery)expectedMetadata["recovery_ledger_sha256"]=Object(package["recovery_ledger"])["sha256"];
    Require(Object(physical["metadata"])==expectedMetadata,"composite_package_metadata");
    std::set<QString> declared;for(const auto& image:Array(physical["class_definitions"]))declared.insert(Object(image)["name"].toString());
    for(const auto& cls:Array(Object(package["dataset"])["required_class_names"]))Require(cls.isString() && declared.count(cls.toString()),"composite_package_dataset_dependency");
    NmsdkTools::PrepareModelArchive(physical,storage,environment,Text(implementation));
    result->dataset=RDK::dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(Object(package["dataset"]),storage,environment));
    CheckDataset(c,result->dataset,source);auto generator=result->dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);
    const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};
    auto initial=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportPhysicalGraphForEdit(Object(source["initial_native_snapshot"]),storage,environment,ports));
    Require(bool(initial),"composite_package_initial_neuron");try {
        Require(NmsdkTools::IdentifyModel(initial.Get(),ports)==Object(source["initial_model"]),"candidate_window_package_initial_physics_readback");
        NativeProbeBindings bindings;bindings.neuron=initial;bindings.dataset=result->dataset;bindings.branch=declaration.branch;bindings.inputCount=int(c.model.inputs.size());
        std::vector<int> positions;std::vector<InputParameters> expected;for(const auto& input:c.model.inputs) {
            Require(bool(input.resistance),"composite_package_initial_explicit_readback_required");positions.push_back(input.attachment);expected.push_back({input.attachment,input.synapses,*input.resistance});}
        Require(ReadNativeInputParameters(bindings,positions)==expected,"composite_package_initial_planning_readback");
        initial->BreakLinks();storage->ReturnObject(initial);initial=nullptr;
    }catch(...){if(initial){initial->BreakLinks();storage->ReturnObject(initial);}throw;}
    result->neuron=RDK::dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(physical,storage,environment,Text(implementation),ports));
    Require(NmsdkTools::IdentifyModel(result->neuron.Get(),ports)==identity,"candidate_window_package_current_physics_readback");
    NativeProbeBindings bindings;bindings.neuron=result->neuron;bindings.dataset=result->dataset;bindings.branch=declaration.branch;bindings.inputCount=int(result->parameters.size());
    std::vector<int> positions;for(const auto& p:result->parameters)positions.push_back(p.length);
    Require(ReadNativeInputParameters(bindings,positions)==result->parameters,"composite_package_current_native_readback");
    CheckC0(source,package["capacitance_state"],bindings,declaration);if(!package["capacitance_state"].isNull())result->capacitanceState=Object(package["capacitance_state"]);
    result->controllerVersion=2;result->calculateMode=int(Count(package["calculate_mode"]));result->structureBuildMode=int(Count(package["structure_build_mode"]));
    result->configurationText=QJsonDocument(EncodeConfiguredNativeTraining(c)).toJson(QJsonDocument::Compact).toStdString();
    Restore(result->service,std::move(proof),std::move(plan),Object(package["configuration"]),Count(package["session_attempt"]),outcome,reason,!stopped);
    if(recovery) {
        result->service.compositeRecoveryDeliveries=std::move(deliveries);
        result->service.compositeRecoveryDeliveryLogIntact=true;
        if(result->service.plan.search.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow)
            result->service.compositeRecoveryRuntime=EncodeCandidateWindowRuntimePlan(result->service.plan);
    }
    return result;
}
}
