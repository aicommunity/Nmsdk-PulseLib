#include "NativeTerminalLearnedState.h"
#include "NativeCompositeLearnedState.h"
#include "NativeMeasuredTrainingBoundary.h"
#include "NativeTrainingProof.h"
#include <QJsonDocument>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
using namespace RDK;
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
QJsonObject Object(const QJsonValue& v){Require(v.isObject(),"terminal_package_object_required");return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray(),"terminal_package_array_required");return v.toArray();}
QString Text(const std::string& v){return QString::fromStdString(v);}
std::string String(const QJsonValue& v){Require(v.isString(),"terminal_package_string_required");return v.toString().toStdString();}
std::size_t Count(const QJsonValue& v){Require(v.isDouble(),"terminal_package_integer_required");const auto n=v.toDouble();
    Require(std::isfinite(n) && n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n,
        "terminal_package_integer_required");return std::size_t(n);}
QJsonValue Integer(std::size_t n){Require(n<=9007199254740991ULL,"terminal_package_inexact_integer");return double(n);}
void Keys(const QJsonObject& o,const char* fields){const auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);const auto keys=o.keys();
    Require(std::set<QString>(names.begin(),names.end())==std::set<QString>(keys.begin(),keys.end()),"terminal_package_fields");}
void Digest(const QJsonObject& o){auto body=o;body.remove("sha256");Require(o.value("sha256")==QJsonValue(NmsdkTools::CanonicalModelDataSha256(body)),"terminal_package_digest");}
QJsonArray Parameters(const std::vector<InputParameters>& values){QJsonArray result;for(const auto& p:values)
    result.append(QJsonObject{{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}});return result;}
std::vector<InputParameters> Parameters(const QJsonValue& v,std::size_t size){std::vector<InputParameters> result;const auto a=Array(v);
    Require(std::size_t(a.size())==size,"terminal_package_input_dimension");for(const auto& value:a){const auto p=Object(value);Keys(p,"length synapses resistance");
        const auto l=Count(p.value("length")),n=Count(p.value("synapses"));const auto r=p.value("resistance");
        Require(l>=1 && l<=std::size_t(std::numeric_limits<int>::max()) && n>=1 && n<=128 && r.isDouble() && std::isfinite(r.toDouble()) && r.toDouble()>0.,
            "terminal_package_input_value");result.push_back({int(l),int(n),r.toDouble()});}return result;}
QJsonArray Journal(const NativeTrainingService& s){QJsonArray result;for(const auto& e:s.TerminalJournal())result.append(e);return result;}
QJsonValue Policy(const NativeTrainingPlan& p){if(!p.terminalSelection)return QJsonValue(QJsonValue::Null);
    return QJsonObject{{"schema_version",1},{"maximum_wall_seconds",p.terminalSelection->maximumWallSeconds}};}
std::optional<NativeTerminalSelectionPolicy> Policy(const QJsonValue& v){if(v.isNull())return {};
    const auto o=Object(v);Keys(o,"schema_version maximum_wall_seconds");Require(o.value("schema_version")==QJsonValue(1) && o.value("maximum_wall_seconds").isDouble(),"terminal_package_policy");
    NativeTerminalSelectionPolicy result{o.value("maximum_wall_seconds").toDouble()};ValidateNativeTerminalSelectionPolicy(result);return result;}
std::pair<QString,double> Threshold(const QJsonObject& identity){QString path;const auto model=Object(identity.value("model"));
    for(const auto& c:Array(model.value("caches"))){const auto candidate=Object(c).value("cached_ltzone").toString();if(!candidate.isEmpty()){path=candidate;break;}}
    Require(!path.isEmpty(),"terminal_package_ltzone_missing");for(const auto& n:Array(model.value("nodes"))){const auto node=Object(n);if(node.value("path")!=QJsonValue(path))continue;
        const auto d=Object(Object(Object(node.value("parameters")).value("Threshold")).value("data"));const auto hex=d.value("value").toString();bool ok=false;
        const std::uint64_t bits=hex.toULongLong(&ok,16);double value;std::memcpy(&value,&bits,sizeof(value));
        Require(ok && hex.size()==16 && d.value("type")==QJsonValue("binary64") && std::isfinite(value),"terminal_package_threshold");return {path,value};}
    throw std::invalid_argument("terminal_package_ltzone_missing");}
QJsonObject Metadata(const QJsonObject& historical,const QJsonObject& identity,const QJsonValue& event,const QJsonArray& journal){
    auto result=Object(Object(historical.value("neuron")).value("metadata"));const auto [path,value]=Threshold(identity);
    auto calibration=Object(result.value("calibration"));calibration["ltzone_path"]=path;calibration["threshold"]=value;result["calibration"]=calibration;
    result["terminal_event_sha256"]=event.isNull()?QJsonValue(QJsonValue::Null):Object(event).value("sha256");
    result["terminal_journal_sha256"]=NmsdkTools::CanonicalModelDataSha256(QJsonObject{{"entries",journal}});return result;
}
std::map<QString,QJsonObject> Images(const QJsonValue& definitions){std::map<QString,QJsonObject> result;
    for(const auto& v:Array(definitions)){const auto o=Object(v);Require(result.emplace(o.value("name").toString(),o).second,"terminal_package_duplicate_image");}return result;}
// A private read-only reconstruction of the stopped search source. It never
// enters the Environment model or calculates a child, and never edits owner.
struct SourceOwner {
    UEPtr<UNet> owner;UEPtr<NDatasetMatrix> dataset;UEPtr<NPulseNeuron> neuron;UEPtr<UStorage> storage;
    SourceOwner(const NativeTerminalSourceBoundary& source,UEPtr<UStorage> st,UEPtr<UEnvironment> env,unsigned f):storage(st){
        owner=new UNet;owner->SetStorage(st);owner->SetEnvironment(env);owner->Default();owner->Name="HistoricalSource";owner->SetGlobalTimeStep(f);
        try {
            dataset=dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(source.DatasetSnapshot(),st,env));Require(bool(dataset),"terminal_source_dataset_type");
            auto generator=dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(generator),"terminal_source_generator_missing");
            neuron=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportPhysicalGraphForEdit(source.NeuronSnapshot(),st,env,{{&generator->Output,"burst"}}));Require(bool(neuron),"terminal_source_neuron_type");
            UEPtr<UContainer> child=dataset;Require(owner->AddComponent(child)!=ForbiddenId,"terminal_source_dataset_attach");
            child=neuron;Require(owner->AddComponent(child)!=ForbiddenId,"terminal_source_neuron_attach");
            Require(NmsdkTools::IdentifyModel(neuron.Get(),{{&generator->Output,"burst"}})==source.NeuronIdentity() &&
                NmsdkTools::IdentifyModel(dataset.Get())==source.DatasetIdentity(),"terminal_source_import_readback");
        }catch(...){Release();throw;}
    }
    void Release(){if(neuron){neuron->BreakLinks();if(neuron->GetOwner())owner->DelComponent("Neuron",true);else storage->ReturnObject(neuron);neuron=nullptr;}
        if(dataset){dataset->BreakLinks();if(dataset->GetOwner())owner->DelComponent("DatasetMatrix",true);else storage->ReturnObject(dataset);dataset=nullptr;}}
    ~SourceOwner(){Release();}
};
const NativeCompositeMeasurementRecord& Record(const NativeTrainingService& s,const std::string& trial){const auto& a=s.CompositeMeasurements();
    const auto i=std::find_if(a.begin(),a.end(),[&](const auto& r){return r.logical.trialId==trial;});
    Require(i!=a.end() && i->consumed && i->trainingRequestedAtConsume==true && !i->semanticProofId.empty() &&
        i->groupState==NativeMatchedOmissionState::Completed && i->selection && i->selection->status==NativeDeclaredWindowBasinStatus::Completed &&
        i->selection->geometry && i->selection->geometry->measuredMaximum,"terminal_selected_composite_measurement_missing");return *i;}
void ValidateSelection(const QJsonObject& event,const NativeTrainingService& s){const auto selected=Object(event.value("selected"));const auto& p=s.Plan().protocol;
    const auto input=Count(selected.value("input_index"));Require(input<p.inputs.size(),"terminal_selected_input");
    const auto revision=Object(selected.value("neuron_identity")).value("sha256");const auto epoch=Count(selected.value("model_epoch"));
    const auto trial=String(selected.value("trial_id")),anchor=String(selected.value("anchor_trial_id"));
    if(p.schemaVersion==3){const auto packets=Array(event.value("selected_probe_packets"));Require(packets.size()==2,"terminal_selected_probe_packets");
        for(std::size_t index=0;index<2;++index){const auto packet=DecodeNativeProbeEvidence(Object(packets.at(int(index))));
            Require(packet.status==NativeProbeStatus::Completed && packet.requestedSpecification && packet.prepared && packet.observation &&
                packet.requestedSpecification->trialId==(index?anchor:trial) && packet.prepared->baseModelRevision==String(revision) &&
                packet.prepared->modelEpoch==epoch && packet.currentBaseRevision==String(revision),"terminal_selected_probe_context");
            ValidateProbeMeasurement(p,index?p.inputs.size()-1:input,*packet.prepared,*packet.observation);}
    }else{
        const auto& candidate=Record(s,trial);const auto& reference=Record(s,anchor);
        Require(candidate.logical.inputIndex==input && reference.logical.inputIndex==p.inputs.size()-1 &&
            candidate.logical.baseModelRevision==String(revision) && reference.logical.baseModelRevision==String(revision) &&
            candidate.logical.epoch==epoch && reference.logical.epoch==epoch,"terminal_selected_composite_context");
        const auto binding=ParseStrictNativeJsonObject(candidate.candidateBindingJson);
        Require(binding.value("model_identity")==selected.value("neuron_identity"),"terminal_selected_composite_graph");
        Require(Array(event.value("selected_probe_packets")).isEmpty(),"terminal_selected_composite_unexpected_packets");
    }
    Parameters(selected.value("parameters"),p.inputs.size());
}
void ValidateEvent(const QJsonObject& e,const NativeTrainingService& s){
    Require(e.value("kind")==QJsonValue("timelearner_native_terminal_selection_event") && e.value("schema_version")==QJsonValue(2),"terminal_event_schema");Digest(e);
    Require(e.value("fresh_all_input_verification")==QJsonValue(false) && e.value("historical_measurements_rebound_to_actual_epoch")==QJsonValue(false) &&
        e.value("certificate").isNull(),"terminal_event_certificate_claim");
    const auto transactions=Count(e.value("native_transactions")),before=Count(e.value("search_terminal_epoch")),after=Count(e.value("actual_model_epoch"));
    Require(transactions<=NativeTerminalSelectionPolicy::MaximumTransactions && before<=9007199254740991ULL-transactions && after==before+transactions,"terminal_event_epoch_charge");
    const auto budget=Object(e.value("edit_budget"));const auto used=Count(budget.value("search_applied_edits")),maximum=Count(budget.value("maximum_edits"));
    Require(used<=maximum && transactions<=maximum-used && Count(budget.value("reserved_transactions"))==NativeTerminalSelectionPolicy::MaximumTransactions,
        "terminal_event_edit_charge");if(transactions)Require(maximum-used>=NativeTerminalSelectionPolicy::MaximumTransactions,"terminal_event_missing_rollback_reservation");
    Require(budget.value("maximum_finalization_wall_seconds").isDouble() && std::isfinite(budget.value("maximum_finalization_wall_seconds").toDouble()) &&
        budget.value("maximum_finalization_wall_seconds").toDouble()>0. && e.value("wall_seconds").isDouble() && std::isfinite(e.value("wall_seconds").toDouble()) &&
        e.value("wall_seconds").toDouble()>=0.,"terminal_event_time_budget");
    const auto status=String(e.value("status"));Require(status=="restored" || status=="already_current" || status=="budget_exhausted" || status=="rejected" || status=="rolled_back" || status=="failed","terminal_event_status");
    if(status=="already_current")Require(transactions==0,"terminal_event_noop_charge");
    if(status=="rolled_back")Require(transactions==2 && e.value("rollback_attempted")==QJsonValue(true) && e.value("rollback_succeeded")==QJsonValue(true),"terminal_event_rollback_claim");
    ValidateSelection(e,s);
}
void ValidateJournal(const QJsonArray& values,const QJsonValue& current,const NativeTrainingService& s){
    Require(std::size_t(values.size())<=NativeTerminalSelectionPolicy::MaximumJournalEntries,"terminal_journal_size");
    std::size_t previousSession=0,previousEpoch=0;std::set<QString> hashes;
    for(const auto& v:values){const auto e=Object(v);ValidateEvent(e,s);const auto session=Count(e.value("session_attempt")),before=Count(e.value("search_terminal_epoch"));
        Require(session>previousSession && session<=s.SessionAttempt() && before>=previousEpoch && hashes.insert(e.value("sha256").toString()).second,"terminal_journal_order");
        previousSession=session;previousEpoch=Count(e.value("actual_model_epoch"));}
    if(!current.isNull()){const auto event=Object(current);Require(!values.isEmpty() && values.last()==current && Count(event.value("session_attempt"))==s.SessionAttempt(),"terminal_journal_current_event");}
}
void ValidateActualEvent(const QJsonValue& value,const QJsonObject& history,const QJsonObject& actual,const QJsonObject& dataset,const QJsonValue& c0,
    const std::vector<InputParameters>& parameters,std::size_t epoch){
    const auto sourceIdentity=Object(Object(Object(history.value("neuron")).value("graph")).value("identity"));
    if(value.isNull()){Require(actual==sourceIdentity && epoch==Count(history.value("model_epoch")) && QJsonValue(Parameters(parameters))==history.value("trained_inputs"),"terminal_no_transition_changed_model");return;}
    const auto e=Object(value);Require(e.value("search_terminal_revision")==history.value("model_revision") && e.value("search_terminal_epoch")==history.value("model_epoch") &&
        e.value("search_terminal_parameters")==history.value("trained_inputs") && e.value("actual_model_epoch")==Integer(epoch),"terminal_history_source_binding");
    if(e.contains("source_neuron_identity"))Require(e.value("source_neuron_identity")==QJsonValue(sourceIdentity),"terminal_history_source_graph");
    const auto status=String(e.value("status"));const auto selected=Object(e.value("selected"));
    if(status=="restored" || status=="already_current"){
        Require(e.value("actual_model_revision")==actual.value("sha256") && e.value("actual_parameters")==QJsonValue(Parameters(parameters)) &&
            selected.value("neuron_identity")==QJsonValue(actual) && selected.value("dataset_identity")==QJsonValue(dataset) &&
            selected.value("parameters")==QJsonValue(Parameters(parameters)) && selected.value("capacitance_state")==QJsonValue(c0.isNull()?QJsonObject{}:Object(c0)),"terminal_actual_selected_graph");
    }else Require(actual==sourceIdentity && QJsonValue(Parameters(parameters))==history.value("trained_inputs"),"terminal_rejected_transition_changed_model");
}
}

NativeTrainingExport NativeTerminalStateFactory::Export(UNet& owner,const NativeModelSpec& declaration,const NativeTrainingService& service,
    const std::vector<InputParameters>& parameters,int version,int mode,int structure,const std::string& implementation,const NativeCapacitanceCorrection* capacitance)
{
    Require(service.RequiresTerminalSelectionArchive() && service.State()==NativeTrainingState::Finished && service.SessionAttempt()>0,
        "terminal_export_requires_closed_boundary");
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    auto generator=owner.GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);Require(neuron && dataset && generator,"terminal_export_missing_graph");
    const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};const auto actual=NmsdkTools::IdentifyModel(neuron.Get(),ports);
    const auto actualDataset=NmsdkTools::IdentifyModel(dataset.Get());const auto revision=String(actual.value("sha256"));
    std::vector<int> positions;for(const auto& p:parameters)positions.push_back(p.length);
    const auto inspected=InspectNativeLearnerModel(owner,declaration,positions);Require(inspected.status==NativeModelBuildStatus::Adopted && inspected.modelRevision==revision && inspected.effective==parameters,"terminal_export_actual_readback");
    const auto current=service.TerminalSelectionEvent().isEmpty()?QJsonValue(QJsonValue::Null):QJsonValue(service.TerminalSelectionEvent());const auto journal=Journal(service);
    QJsonObject history=service.terminalImportedHistory;
    if(history.isEmpty()){
        const auto source=service.TerminalSourceBoundary();std::unique_ptr<SourceOwner> privateSource;
        UNet* sourceOwner=&owner;QJsonObject sourceC=capacitance?EncodeNativeCapacitanceState(*capacitance,revision):QJsonObject{};
        if(source){privateSource=std::make_unique<SourceOwner>(*source,owner.GetStorage(),owner.GetEnvironment(),declaration.frequencyHz);sourceOwner=privateSource->owner.Get();sourceC=source->CapacitanceState();}
        auto sourceNeuron=sourceOwner->GetComponentL<NPulseNeuron>("Neuron",true);auto sourceDataset=sourceOwner->GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
        auto sourceGenerator=sourceOwner->GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
        const auto sourceIdentity=NmsdkTools::IdentifyModel(sourceNeuron.Get(),{{&sourceGenerator->Output,"burst"}});
        const auto sourceParameters=service.Search()?service.Search()->Parameters():parameters;
        NativeHistoricalExportBoundary boundary(service,sourceIdentity,NmsdkTools::IdentifyModel(sourceDataset.Get()),sourceParameters);
        std::optional<NativeCapacitanceCorrection> sourceCorrection;
        if(!sourceC.isEmpty()){std::string sourceRevision;sourceCorrection=ParseNativeCapacitanceState(QJsonDocument(sourceC).toJson(QJsonDocument::Compact).toStdString(),sourceRevision);
            Require(sourceRevision==String(sourceIdentity.value("sha256")),"terminal_export_source_C_revision");}
        history=ExportNativeHistoricalLearnedState(*sourceOwner,declaration,service,sourceParameters,version,mode,structure,implementation,
            sourceCorrection?&*sourceCorrection:nullptr,boundary);
    }
    Require(history.value("implementation_id")==QJsonValue(Text(implementation)) && history.value("controller_version")==QJsonValue(version) &&
        history.value("calculate_mode")==QJsonValue(mode) && history.value("structure_build_mode")==QJsonValue(structure),"terminal_export_history_options");
    ValidateJournal(journal,current,service);
    const QJsonValue c0=capacitance?QJsonValue(EncodeNativeCapacitanceState(*capacitance,revision)):QJsonValue(QJsonValue::Null);
    if(capacitance){NativeProbeBindings b;b.neuron=neuron;b.dataset=dataset;ValidateAppliedNativeCapacitanceState(b,declaration,Object(c0));}
    ValidateActualEvent(current,history,actual,actualDataset,c0,parameters,service.ModelEpoch());
    std::vector<std::string> images;for(const auto& i:Array(Object(history.value("neuron")).value("class_definitions")))images.push_back(String(Object(i).value("name")));
    const auto physical=NmsdkTools::ExportModelArchive(neuron.Get(),Metadata(history,actual,current,journal),ports,images);
    Require(Images(physical.value("class_definitions"))==Images(Object(history.value("neuron")).value("class_definitions")),"terminal_export_storage_changed");
    const auto certificate=service.Certificate()?QJsonValue(EncodeTrainingCertificate(*service.Certificate())):QJsonValue(QJsonValue::Null);
    Require(certificate==history.value("certificate"),"terminal_export_certificate_changed");
    QJsonObject package{{"kind",history.value("kind")},{"schema_version",8},{"dynamic_checkpoint",false},{"state_kind",history.value("state_kind")},
        {"implementation_id",Text(implementation)},{"controller_version",version},{"calculate_mode",mode},{"structure_build_mode",structure},
        {"configuration",history.value("configuration")},{"protocol",history.value("protocol")},{"trained_inputs",Parameters(parameters)},
        {"model_revision",Text(revision)},{"model_epoch",Integer(service.ModelEpoch())},{"session_attempt",Integer(service.SessionAttempt())},
        {"training_outcome",TrainingStatusName(service.Outcome())},{"reason",Text(service.Reason())},{"certificate",certificate},
        {"historical_state",history},{"terminal_policy",Policy(service.Plan())},{"terminal_event",current},{"terminal_journal",journal},
        {"dataset",NmsdkTools::ExportDatasetSnapshot(dataset.Get())},{"neuron",physical},{"capacitance_state",c0}};
    package["sha256"]=NmsdkTools::CanonicalModelDataSha256(package);
    Require(service.State()==NativeTrainingState::Finished && NmsdkTools::IdentifyModel(neuron.Get(),ports)==actual && Journal(service)==journal,
        "terminal_export_boundary_changed");return {package,Summary(service,revision,parameters)};
}

std::unique_ptr<PreparedNativeLearnedState> NativeTerminalStateFactory::Prepare(const QJsonObject& package,UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const std::string& implementation)
{
    Keys(package,"kind schema_version dynamic_checkpoint state_kind implementation_id controller_version calculate_mode structure_build_mode configuration protocol trained_inputs model_revision model_epoch session_attempt training_outcome reason certificate historical_state terminal_policy terminal_event terminal_journal dataset neuron capacitance_state sha256");
    Require(package.value("schema_version")==QJsonValue(8) && package.value("dynamic_checkpoint")==QJsonValue(false),"terminal_package_schema");Digest(package);
    const auto history=Object(package.value("historical_state"));Require(Count(history.value("schema_version"))>=1 && Count(history.value("schema_version"))<=7,"terminal_package_recursive_history");
    for(const auto* key:{"kind","state_kind","implementation_id","controller_version","calculate_mode","structure_build_mode","configuration","protocol","session_attempt","training_outcome","certificate"})
        Require(package.value(key)==history.value(key),"terminal_package_history_metadata");
    Require(package.value("reason")==history.value("reason") || (package.value("reason")==QJsonValue("") && !history.contains("reason")),"terminal_package_history_reason");
    auto result=PrepareNativeLearnedState(history,storage,environment,implementation);
    const auto policy=Policy(package.value("terminal_policy"));const auto journal=Array(package.value("terminal_journal"));const auto event=package.value("terminal_event");
    ValidateJournal(journal,event,result->service);
    const auto physical=Object(package.value("neuron"));const auto identity=Object(Object(physical.value("graph")).value("identity"));
    Require(identity.value("sha256")==package.value("model_revision") && Images(physical.value("class_definitions"))==Images(Object(history.value("neuron")).value("class_definitions")),"terminal_package_actual_images_or_revision");
    Require(Object(physical.value("metadata"))==Metadata(history,identity,event,journal),"terminal_package_actual_metadata");
    PreparedNativeLearnedState actual;actual.storage=storage;NmsdkTools::PrepareModelArchive(physical,storage,environment,Text(implementation));
    actual.dataset=dynamic_pointer_cast<NDatasetMatrix>(NmsdkTools::ImportDatasetSnapshot(Object(package.value("dataset")),storage,environment));Require(bool(actual.dataset),"terminal_package_dataset_type");
    auto generator=actual.dataset->GetComponentL<NPulseGeneratorTransit>("Generator1",true);Require(bool(generator),"terminal_package_generator_missing");
    const NmsdkTools::ModelBoundarySources ports{{&generator->Output,"burst"}};
    actual.neuron=dynamic_pointer_cast<NPulseNeuron>(NmsdkTools::ImportModelArchive(physical,storage,environment,Text(implementation),ports));Require(bool(actual.neuron),"terminal_package_neuron_type");
    Require(NmsdkTools::IdentifyModel(actual.neuron.Get(),ports)==identity && NmsdkTools::IdentifyModel(actual.dataset.Get())==NmsdkTools::IdentifyModel(result->dataset.Get()),"terminal_package_actual_native_readback");
    const auto& declaration=result->sourceDeclaration.value_or(result->configuration.model);const auto parameters=Parameters(package.value("trained_inputs"),declaration.inputs.size());
    std::vector<int> positions;for(const auto& p:parameters)positions.push_back(p.length);
    const auto checked=InspectNativePreparedChildren(declaration,actual.neuron,actual.dataset,positions);
    Require(checked.status==NativeModelBuildStatus::Adopted && checked.effective==parameters && checked.modelRevision==String(package.value("model_revision")),"terminal_package_actual_parameter_readback");
    const auto c0=package.value("capacitance_state");Require(c0.isNull()==result->capacitanceState.isEmpty(),"terminal_package_C0_lost_or_invented");
    if(!c0.isNull()){
        NativeProbeBindings b;b.neuron=actual.neuron;b.dataset=actual.dataset;const auto state=Object(c0);ValidateAppliedNativeCapacitanceState(b,declaration,state);
        const auto baseline=Object(state.value("baseline")),old=Object(result->capacitanceState.value("baseline"));
        for(const auto* key:{"model_spec","model_revision","capture_model","storage_images_sha256"})Require(baseline.value(key)==old.value(key),"terminal_package_C0_recaptured");
        result->capacitanceState=state;
    }
    ValidateActualEvent(event,history,identity,NmsdkTools::IdentifyModel(actual.dataset.Get()),c0,parameters,Count(package.value("model_epoch")));
    if(result->service.Certificate())Require(package.value("model_revision")==history.value("model_revision") && package.value("model_epoch")==history.value("model_epoch"),"terminal_package_stale_certificate");
    result->neuron->BreakLinks();storage->ReturnObject(result->neuron);result->neuron=nullptr;
    result->dataset->BreakLinks();storage->ReturnObject(result->dataset);result->dataset=nullptr;
    result->neuron=actual.neuron;actual.neuron=nullptr;result->dataset=actual.dataset;actual.dataset=nullptr;
    result->parameters=parameters;result->modelRevision=String(package.value("model_revision"));
    auto& service=result->service;service.plan.terminalSelection=policy;service.terminalSelectionArchiveRequired=true;
    service.terminalPhysicalEpoch=Count(package.value("model_epoch"));service.referenceBinding=result->modelRevision;
    service.terminalSelectionEvent=event.isNull()?QJsonObject{}:Object(event);service.terminalSelectionTransactions=event.isNull()?0:Count(Object(event).value("native_transactions"));
    for(const auto& v:journal)service.terminalJournal.push_back(Object(v));service.terminalImportedHistory=history;
    return result;
}

QJsonObject NativeTerminalStateFactory::Summary(const NativeTrainingService& service,const std::string& revision,const std::vector<InputParameters>& parameters)
{
    Require(service.RequiresTerminalSelectionArchive(),"terminal_summary_requires_terminal_scope");
    const auto event=service.TerminalSelectionEvent().isEmpty()?QJsonValue(QJsonValue::Null):QJsonValue(service.TerminalSelectionEvent());
    if(!event.isNull() && Object(event).contains("actual_model_revision"))Require(Object(event).value("actual_model_revision")==QJsonValue(Text(revision)) &&
        Object(event).value("actual_parameters")==QJsonValue(Parameters(parameters)),"terminal_summary_actual_binding");
    else if(service.Search())Require(service.Search()->ModelRevision()==revision && service.Search()->Parameters()==parameters,"terminal_summary_search_binding");
    else if(service.Certificate())Require(service.Certificate()->ModelRevision()==revision && service.Certificate()->ModelEpoch()==service.ModelEpoch(),"terminal_summary_certificate_binding");
    else if(!service.terminalImportedHistory.isEmpty())Require(service.terminalImportedHistory.value("model_revision")==QJsonValue(Text(revision)) &&
        service.terminalImportedHistory.value("trained_inputs")==QJsonValue(Parameters(parameters)),"terminal_summary_imported_binding");
    QJsonObject result{{"kind","native_training_state_summary"},{"schema_version",9},{"state",NativeTrainingStateName(service.State())},
        {"outcome",TrainingStatusName(service.Outcome())},{"reason",Text(service.Reason())},{"actual_model_revision",Text(revision)},
        {"model_epoch",Integer(service.ModelEpoch())},{"transcript_model_epoch",Integer(service.TranscriptModelEpoch())},
        {"parameters",Parameters(parameters)},{"session_attempt",Integer(service.SessionAttempt())},
        {"certificate",service.Certificate()?QJsonValue(EncodeTrainingCertificate(*service.Certificate())):QJsonValue(QJsonValue::Null)},
        {"terminal_policy",Policy(service.Plan())},{"terminal_event",event},{"terminal_journal",Journal(service)},
        {"closed_boundary",service.State()==NativeTrainingState::Finished},
        {"controller_checkpoint",false},{"restored_live_authority",false}};
    result["sha256"]=NmsdkTools::CanonicalModelDataSha256(result);return result;
}
}
