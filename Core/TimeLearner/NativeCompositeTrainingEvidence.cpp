#include "NativeCompositeTrainingEvidence.h"
#include "CompositeTrainingDeclarations.h"
#include "CompositeSourceValidation.h"
#include "CandidateWindowLogicalContext.h"
#include "CandidateWindowProposalReplay.h"
#include "CandidateWindowPortableReferences.h"
#include "CandidateWindowMeasurementEvidence.h"
#include "NativeCompositeTrainingAttempt.h"
#include "ConfiguredNativeTraining.h"
#include "ProbeEvidenceJson.h"
#include "ModelArchive.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
void Keys(const QJsonObject& o,const char* fields){auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);names.sort();Require(o.keys()==names,"composite_archive_fields");}
QJsonObject Object(const QJsonValue& v){Require(v.isObject(),"composite_archive_object");return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray(),"composite_archive_array");return v.toArray();}
QString Text(const std::string& v){return QString::fromStdString(v);}
std::string Text(const QJsonValue& v){Require(v.isString(),"composite_archive_string");return v.toString().toStdString();}
double Number(const QJsonValue& v){Require(v.isDouble() && std::isfinite(v.toDouble()),"composite_archive_number");return v.toDouble();}
double Number(double v){Require(std::isfinite(v),"composite_archive_nonfinite");return v;}
double Count(std::size_t n){Require(n<=9007199254740991ULL,"composite_archive_inexact_count");return double(n);}
std::size_t Count(const QJsonValue& v){const auto n=Number(v);Require(n>=0. && n<=9007199254740991. && n<=double(std::numeric_limits<std::size_t>::max()) && std::floor(n)==n,"composite_archive_count");return std::size_t(n);}
bool Boolean(const QJsonValue& v){Require(v.isBool(),"composite_archive_boolean");return v.toBool();}
std::size_t Add(std::size_t a,std::size_t b){Require(b<=9007199254740991ULL && a<=9007199254740991ULL-b,"composite_archive_counter_overflow");return a+b;}
QString Hash(const QJsonValue& v){return NmsdkTools::CanonicalModelDataSha256(v);}
QString BytesHash(const QByteArray& b){return QString::fromLatin1(QCryptographicHash::hash(b,QCryptographicHash::Sha256).toHex());}
QJsonArray Strings(const std::vector<std::string>& values){QJsonArray out;for(const auto& value:values)out.append(Text(value));return out;}
std::vector<std::string> Strings(const QJsonValue& values){std::vector<std::string> out;for(const auto& v:Array(values))out.push_back(Text(v));return out;}
void SafePath(const QString& path) {
    Require(!path.isEmpty() && !path.startsWith('/') && !path.contains('\\') && path.size()<4096,"composite_archive_unsafe_path");
    for(const auto& part:path.split('/')) {
        Require(!part.isEmpty() && part!="." && part!=".." && !part.endsWith('.') && !part.endsWith(' '),"composite_archive_unsafe_path");
        for(const auto c:part)Require(c.unicode()>=32 && !QString("<>:\"|?*").contains(c),"composite_archive_unsafe_path");
    }
}
const char* Purpose(CompositeMeasurementPurpose p){switch(p){case CompositeMeasurementPurpose::Reference:return "reference";case CompositeMeasurementPurpose::Search:return "search";case CompositeMeasurementPurpose::Final:return "final";}throw std::invalid_argument("composite_archive_purpose");}
CompositeMeasurementPurpose Purpose(const QJsonValue& v){const auto s=Text(v);for(auto p:{CompositeMeasurementPurpose::Reference,CompositeMeasurementPurpose::Search,CompositeMeasurementPurpose::Final})if(s==Purpose(p))return p;throw std::invalid_argument("composite_archive_purpose");}
const char* StateName(NativeMatchedOmissionState p){switch(p){case NativeMatchedOmissionState::Idle:return "idle";case NativeMatchedOmissionState::Running:return "running";case NativeMatchedOmissionState::Completed:return "completed";case NativeMatchedOmissionState::Rejected:return "rejected";case NativeMatchedOmissionState::Cancelled:return "cancelled";case NativeMatchedOmissionState::BudgetExhausted:return "budget_exhausted";}throw std::invalid_argument("composite_archive_group_state");}
NativeMatchedOmissionState State(const QJsonValue& v){const auto s=Text(v);for(auto p:{NativeMatchedOmissionState::Idle,NativeMatchedOmissionState::Running,NativeMatchedOmissionState::Completed,NativeMatchedOmissionState::Rejected,NativeMatchedOmissionState::Cancelled,NativeMatchedOmissionState::BudgetExhausted})if(s==StateName(p))return p;throw std::invalid_argument("composite_archive_group_state");}
QJsonObject Maximum(const ResponseMaximum& m){return {{"first_sample",Count(m.firstSample)},{"last_sample",Count(m.lastSample)},{"amplitude",Number(m.amplitude)},
    {"raw_potential",Number(m.rawPotential)},{"time",Number(m.time)},{"plateau_end",Number(m.plateauEnd)}};}
QJsonObject Anchor(const BasinAnchor& a){return {{"first_sample",Count(a.firstSample)},{"last_sample",Count(a.lastSample)},{"record_edge",a.recordEdge}};}
QJsonObject Geometry(const DeclaredWindowBasinResult& g){QJsonArray candidates;for(const auto& c:g.candidates) {
    QJsonArray controls,necessary;for(auto p:c.controls)controls.append(BasinControlPresenceName(p));for(auto i:c.necessaryInputs)necessary.append(Count(i));
    candidates.append(QJsonObject{{"maximum",Maximum(c.maximum)},{"left",Anchor(c.left)},{"right",Anchor(c.right)},
        {"status",BasinCandidateStatusName(c.status)},{"controls",controls},{"necessary_inputs",necessary},{"reasons",Strings(c.reasons)}});
    }return {{"validity",DeclaredWindowBasinValidityName(g.validity)},{"reasons",Strings(g.reasons)},{"candidates",candidates},
        {"selected_candidate",g.selectedCandidate?QJsonValue(Count(*g.selectedCandidate)):QJsonValue(QJsonValue::Null)},
        {"measured_maximum",g.measuredMaximum?QJsonValue(Maximum(*g.measuredMaximum)):QJsonValue(QJsonValue::Null)}};
}
NativeDeclaredWindowBasinSpecification Specification(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    Require(r.logical.inputIndex<p.inputs.size(),"composite_archive_input_index");NativeDeclaredWindowBasinSpecification s;
    s.groupId=r.groupId;s.inputId=p.inputs[r.logical.inputIndex].id;s.assignedInput=r.logical.inputIndex;
    s.channel=p.routing==ProbeRouting::ClassicBurst?r.logical.inputIndex:0;const auto& w=p.compositeMeasurement->inputWindows[r.logical.inputIndex];
    s.windowBeginOffset=w.beginOffset;s.windowEndOffset=w.endOffset;return s;
}
QJsonObject Specification(const NativeDeclaredWindowBasinSpecification& s){return {{"schema_version",int(s.schemaVersion)},{"method_id",Text(s.methodId)},
    {"group_id",Text(s.groupId)},{"input_id",Text(s.inputId)},{"assigned_input",Count(s.assignedInput)},{"channel",Count(s.channel)},
    {"window_begin_offset",Number(s.windowBeginOffset)},{"window_end_offset",Number(s.windowEndOffset)}};}
const MatchedOmissionPolicy& ContextPolicy(const VerificationProtocol& p) {
    if(p.schemaVersion==5){ValidateCandidateWindowMeasurementContract(p);return p.candidateWindowMeasurement->contextPolicy;}
    ValidateCompositeMeasurementContract(p);return p.compositeMeasurement->contextPolicy;
}
const DeclaredWindowBasinPolicy& GeometryPolicy(const VerificationProtocol& p) {
    if(p.schemaVersion==5){ValidateCandidateWindowMeasurementContract(p);return p.candidateWindowMeasurement->geometryPolicy;}
    ValidateCompositeMeasurementContract(p);return p.compositeMeasurement->geometryPolicy;
}
NativeDeclaredWindowBasinResult FreshSelection(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    if(p.schemaVersion==5){Require(bool(r.candidateWindowSpecification),"candidate_window_archive_specification_missing");
        return SelectNativeDeclaredWindowBasinMaximum(*r.candidateWindowSpecification,GeometryPolicy(p),r.group);}
    return SelectNativeDeclaredWindowBasinMaximum(Specification(p,r),p.compositeMeasurement->geometryPolicy,r.group);
}
QJsonObject Request(const VerificationRequest& r){return {{"run_id",Text(r.runId)},{"trial_id",Text(r.trialId)},{"input_id",Text(r.inputId)},
    {"sample_id",Text(r.sampleId)},{"pattern_hash",Text(r.patternHash)},{"protocol_revision",Text(r.protocolRevision)},
    {"base_model_revision",Text(r.baseModelRevision)},{"routing",r.routing==ProbeRouting::ClassicBurst?"classic_burst":"branch_solo_burst"},
    {"model_epoch",Count(r.epoch)},{"input_index",Count(r.inputIndex)}};}
VerificationRequest Request(const QJsonValue& value){const auto o=Object(value);Keys(o,"run_id trial_id input_id sample_id pattern_hash protocol_revision base_model_revision routing model_epoch input_index");
    const auto routing=Text(o["routing"]);Require(routing=="classic_burst" || routing=="branch_solo_burst","composite_archive_routing");
    return {Text(o["run_id"]),Text(o["trial_id"]),Text(o["input_id"]),Text(o["sample_id"]),Text(o["pattern_hash"]),Text(o["protocol_revision"]),
        Text(o["base_model_revision"]),routing=="classic_burst"?ProbeRouting::ClassicBurst:ProbeRouting::BranchSoloBurst,Count(o["model_epoch"]),Count(o["input_index"])};
}
void CheckIdentity(const QJsonObject& identity){Require(identity["kind"]==QJsonValue("semantic_model_identity") &&
    (identity["schema_version"]==QJsonValue(1) || identity["schema_version"]==QJsonValue(2)) &&
    identity["canonical_encoding"]==QJsonValue("length-framed-utf8-binary64-v1") && Boolean(identity["valid"]) &&
    Array(identity["diagnostics"]).isEmpty() && Object(identity["model"])["schema_version"]==identity["schema_version"] &&
    identity["sha256"]==QJsonValue(Hash(identity["model"])),"composite_archive_model_identity");}
void CheckSnapshot(const QJsonObject& s){Keys(s,"component_name component_class native_xml native_xml_sha256 identity");
    const auto identity=Object(s["identity"]);CheckIdentity(identity);const auto xml=Text(s["native_xml"]);
    const auto nodes=Array(Object(identity["model"])["nodes"]);const auto root=std::find_if(nodes.begin(),nodes.end(),[](const auto& v){return v.toObject()["path"]==QJsonValue("$");});
    Require(root!=nodes.end() && Object(*root)["class"]==s["component_class"],"composite_archive_snapshot_root_class");
    Require(s["native_xml_sha256"]==QJsonValue(BytesHash(QByteArray::fromStdString(xml))),"composite_archive_snapshot_sha");
}
QString Binary64(double value){Number(value);if(value==0.)value=0.;std::uint64_t bits;static_assert(sizeof(bits)==sizeof(value));std::memcpy(&bits,&value,sizeof(bits));
    return QString::number(qulonglong(bits),16).rightJustified(16,QLatin1Char('0'));}
QJsonObject Parameter(const QJsonObject& node,const char* name,const char* type) {
    const auto parameter=Object(Object(node["parameters"])[name]);Require(!Boolean(parameter["binding"]),"composite_archive_bound_initial_source_parameter");
    const auto data=Object(parameter["data"]);Require(data["type"]==QJsonValue(type),"composite_archive_initial_source_parameter_type");return data;
}
void CheckDatasetSource(const QJsonObject& source,const NativeModelSpec& model,const StimulusProfile& profile,const QJsonArray& isi) {
    auto dataset=Object(source["initial_dataset"]);Keys(dataset,"component_name component_class native_xml native_xml_sha256 identity required_class_names");
    const auto required=Array(dataset["required_class_names"]);dataset.remove("required_class_names");CheckSnapshot(dataset);
    std::set<QString> classes;std::map<QString,QJsonObject> nodes;for(const auto& entry:Array(Object(Object(dataset["identity"])["model"])["nodes"])) {
        const auto node=Object(entry);Require(nodes.emplace(QString::fromStdString(Text(node["path"])),node).second,"composite_archive_dataset_duplicate_node");classes.insert(QString::fromStdString(Text(node["class"])));
        const auto parameters=Object(node["parameters"]);for(auto it=parameters.begin();it!=parameters.end();++it)if(it.key().endsWith("ClassName")) {
            const auto data=it.value().toObject()["data"].toObject();if(data["type"]==QJsonValue("utf8") && !data["value"].toString().isEmpty())classes.insert(data["value"].toString());
        }
    }
    QJsonArray expected;for(const auto& cls:classes)expected.append(cls);Require(required==expected && nodes.count("$") && nodes.count("$.Generator1"),"composite_archive_dataset_closure");
    const auto& root=nodes.at("$");const auto& generator=nodes.at("$.Generator1");
    Require(root["class"]==Text(model.datasetClass) && generator["class"]==Text(profile.generatorClass) &&
        generator["frequency_hz"]==QString::number(qulonglong(profile.frequencyHz)),"composite_archive_dataset_source_class_or_clock");
    const auto matrix=Parameter(root,"MatrixData","binary64_matrix");QJsonArray rows;for(const auto& value:isi)rows.append(QJsonArray{Binary64(Number(value))});
    Require(matrix["rows"]==QString::number(isi.size()) && matrix["columns"]==QJsonValue("1") && matrix["value"]==QJsonValue(rows),"composite_archive_dataset_isi_readback");
    Require(Parameter(root,"NumSamples","signed_integer")["value"]==QJsonValue("1") &&
        Parameter(root,"NumFeatures","signed_integer")["value"]==QJsonValue("1") &&
        Parameter(root,"MaxSpikesPerFeature","signed_integer")["value"]==QString::number(isi.size()),"composite_archive_dataset_dimensions");
    for(const auto& field:std::vector<std::pair<const char*,double>>{{"Amplitude",profile.amplitude},{"PulseLength",profile.pulseLength},{"FrequencyDeviation",profile.frequencyDeviation}})
        Require(Parameter(generator,field.first,"binary64")["value"]==QJsonValue(Binary64(field.second)),"composite_archive_dataset_source_number");
    Require(Parameter(generator,"UseTransitSignal","boolean")["value"]==QJsonValue(profile.useTransitSignal) &&
        Parameter(generator,"UsePatternOutput","boolean")["value"]==QJsonValue(profile.usePatternOutput),"composite_archive_dataset_source_boolean");
}
void CheckSource(const VerificationProtocol& p,const QJsonObject& source,const std::string& origin) {
    Keys(source,"kind schema_version initial_model initial_native_snapshot initial_dataset initial_epoch model_spec storage_images_sha256 storage_images applied_source_isi pattern_hash optional_C0_origin optional_C0_witness actual_source_profile");
    Require(source["kind"]==QJsonValue("composite_train_initial_source_v1") && Count(source["schema_version"])==2 &&
        Hash(source)==Text(origin),"composite_archive_initial_source");CheckIdentity(Object(source["initial_model"]));CheckSnapshot(Object(source["initial_native_snapshot"]));
    Require(Object(source["initial_native_snapshot"])["identity"]==source["initial_model"],"composite_archive_initial_snapshot_identity");
    (void)Count(source["initial_epoch"]);const auto model=DecodeNativeModelSpec(Object(source["model_spec"]));
    const auto isi=Array(source["applied_source_isi"]);Require(isi.size()==int(p.inputs.size()) && Hash(isi)==Text(p.patternHash) && source["pattern_hash"]==Text(p.patternHash),"composite_archive_source_isi");
    for(const auto& value:isi)Require(Number(value)>=0.,"composite_archive_source_isi");
    const auto& profile=*p.stimulusProfile;const QJsonArray actual{Text(profile.generatorClass),profile.amplitude,profile.pulseLength,profile.frequencyHz,
        profile.frequencyDeviation,profile.useTransitSignal,profile.usePatternOutput};
    Require(source["actual_source_profile"]==QJsonValue(actual) && model.inputs.size()==p.inputs.size() &&
        Object(source["initial_native_snapshot"])["component_class"]==Text(model.neuronClass) &&
        model.branch==(p.routing==ProbeRouting::BranchSoloBurst) && model.frequencyHz==profile.frequencyHz &&
        model.generatorClass==profile.generatorClass && profile.generatorClass=="NPulseGeneratorTransit" && profile.amplitude>0. &&
        profile.pulseLength>=1./profile.frequencyHz && profile.frequencyDeviation==0. && !profile.useTransitSignal && !profile.usePatternOutput,
        "composite_archive_source_profile");
    Require(source["storage_images_sha256"]==QJsonValue(NmsdkTools::ModelImageDefinitionsFingerprint(Array(source["storage_images"]))),"composite_archive_storage_closure");
    if(!source["optional_C0_witness"].isNull()) {const auto witness=DecodeNativeTrainingCapacitanceWitness(Object(source["optional_C0_witness"]));
        Require(witness.parameters.origin==Text(source["optional_C0_origin"]) && witness.modelRevision==Text(Object(source["initial_model"])["sha256"]),"composite_archive_initial_C0");
    } else Require(Text(source["optional_C0_origin"]).empty(),"composite_archive_unbound_C0");
    CheckDatasetSource(source,model,profile,isi);
}
bool Decimal(const std::string& s){return !s.empty() && (s=="0" || s.front()!='0') && std::all_of(s.begin(),s.end(),[](char c){return c>='0' && c<='9';});}
void CheckLogical(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    const auto& l=r.logical;Require(!r.attemptId.empty() && !l.runId.empty() && !l.trialId.empty() && !r.groupId.empty() &&
        l.inputIndex<p.inputs.size() && l.inputId==p.inputs[l.inputIndex].id && l.sampleId==p.sampleId && l.patternHash==p.patternHash &&
        l.protocolRevision==p.revision && l.routing==p.routing && !l.baseModelRevision.empty(),"composite_archive_logical_binding");
    if(r.purpose==CompositeMeasurementPurpose::Search)Require(l.runId==r.attemptId,"composite_archive_attempt_namespace");
    else {const auto prefix=r.attemptId+(r.purpose==CompositeMeasurementPurpose::Reference?"/capture/":"/final/");
        Require(l.runId.starts_with(prefix) && Decimal(l.runId.substr(prefix.size())),"composite_archive_attempt_namespace");}
    const auto prefix=l.runId+(r.purpose==CompositeMeasurementPurpose::Reference?"/":"/"+std::string(r.purpose==CompositeMeasurementPurpose::Search?"search/":"verify/")+std::to_string(l.epoch)+"/");
    // Search serials have no epoch component; final verification has it.
    const auto actualPrefix=r.purpose==CompositeMeasurementPurpose::Search?l.runId+"/search/":prefix;
    Require(l.trialId.starts_with(actualPrefix) && Decimal(l.trialId.substr(actualPrefix.size())),"composite_archive_trial_namespace");
    Require(r.groupId.starts_with(l.trialId+"/composite/") && Decimal(r.groupId.substr(l.trialId.size()+11)),"composite_archive_group_namespace");
}
QJsonArray Events(const NativeCompositeRawTrial& raw){QJsonArray out;for(std::size_t i=0;i<raw.samples.size();++i)for(const auto& e:raw.samples[i].newStimuli)
    out.append(QJsonObject{{"id",Text(e.id)},{"time",Number(e.time)},{"frame_index",Count(i)}});return out;}
NativeCompositeRawTrial ReadRaw(const QJsonObject& o,const QByteArray& bytes) {
    Keys(o,"native_trial_id prepared_context actual_specification primary_ref actual_stimuli");NativeCompositeRawTrial raw;
    raw.prepared=DecodeNativeProbePreparation(Object(o["prepared_context"]));raw.actual=DecodeNativeProbeSpecification(Object(o["actual_specification"]));
    Require(Text(o["native_trial_id"])==raw.actual.trialId,"composite_archive_raw_trial");
    const auto rows=bytes.split('\n');Require(!rows.isEmpty() && rows.back().isEmpty() && !bytes.contains('\r'),"composite_archive_csv_newlines");
    QByteArray header="time,stimulus";for(std::size_t i=0;i<raw.actual.baseline.size();++i)header+=",soma"+QByteArray::number(qulonglong(i));
    Require(rows.front()==header && std::size_t(rows.size()-2)<=raw.actual.maximumSamples,"composite_archive_csv_shape");
    for(int row=1;row+1<rows.size();++row) {
        const auto fields=rows[row].split(',');Require(fields.size()==int(raw.actual.baseline.size()+2),"composite_archive_csv_shape");
        const auto number=[](const QByteArray& text){bool ok=false;const auto d=text.toDouble(&ok);Require(ok && std::isfinite(d),"composite_archive_csv_number");return d;};
        NativeProbeSample sample;sample.time=number(fields[0]);sample.stimulus=number(fields[1]);sample.effectiveModelRevision=raw.actual.modelRevision;
        for(int i=2;i<fields.size();++i)sample.potentials.push_back(number(fields[i]));raw.samples.push_back(std::move(sample));
    }
    std::size_t ordinal=0;for(const auto& value:Array(o["actual_stimuli"])) {
        const auto event=Object(value);Keys(event,"id time frame_index");const auto index=Count(event["frame_index"]);const auto time=Number(event["time"]);
        Require(index<raw.samples.size() && ordinal<raw.actual.expectedStimulusIds.size() && Text(event["id"])==raw.actual.expectedStimulusIds[ordinal++] &&
            time<=raw.samples[index].time && (!index || time>raw.samples[index-1].time),"composite_archive_actual_event_frame");
        raw.samples[index].newStimuli.push_back({Text(event["id"]),time});
    }
    Require(EncodeCompositeRawPrimary(raw)==bytes && Events(raw)==Array(o["actual_stimuli"]),"composite_archive_noncanonical_csv");return raw;
}
QJsonObject Budgets(const NativeCompositeMeasurementRecord& r){const auto& a=r.attemptLimits;return {{"attempt_limits",QJsonObject{
    {"maximum_groups",Count(a.maximumGroups)},{"maximum_native_probes",Count(a.maximumNativeProbes)},{"maximum_total_samples",Count(a.maximumTotalSamples)},
    {"maximum_model_seconds",Number(a.maximumModelSeconds)},{"maximum_wall_seconds",Number(a.maximumWallSeconds)}}},
    {"maximum_group_model_seconds",Number(r.maximumGroupModelSeconds)},{"maximum_group_wall_seconds",Number(r.maximumGroupWallSeconds)},
    {"maximum_total_primary_bytes",Count(r.policy.maximumTotalPrimaryBytes)}};}
struct Totals {std::size_t groups=0,native=0,samples=0,bytes=0;void AddRecord(const NativeCompositeMeasurementRecord& r){groups=Add(groups,1);native=Add(native,r.issuedNativeProbes);samples=Add(samples,r.sampledFrames);bytes=Add(bytes,CompositeRecordPrimaryBytes(r));}};
QJsonObject Counters(const std::vector<NativeCompositeMeasurementRecord>& records,const std::string& current) {Totals attempt,archive;for(const auto& r:records){archive.AddRecord(r);if(r.attemptId==current)attempt.AddRecord(r);}
    return {{"attempt",QJsonObject{{"logical_groups",Count(attempt.groups)},{"native_probes",Count(attempt.native)},{"sampled_frames",Count(attempt.samples)}}},
        {"archive",QJsonObject{{"groups",Count(archive.groups)},{"native_probes",Count(archive.native)},{"sampled_frames",Count(archive.samples)},{"primary_bytes",Count(archive.bytes)}}}};
}
void CheckCandidateWindowPrimaryHistory(const std::vector<NativeCompositeMeasurementRecord>& records,
    const std::vector<std::string>& references,const std::string& current,std::size_t currentCap) {
    // The array is the producer's retained append-ledger order. Prefix bytes
    // are a lower bound if a producer pruned history; imported clocks are not
    // treated as a global cross-process execution-order attestation.
    Totals prefix;std::map<std::string,Totals> attempts;std::map<std::string,std::size_t> caps;
    std::map<std::string,qulonglong> lastOrdinal;std::set<std::string> blocks,seenGroups;
    std::string block;
    for(const auto& r:records) {
        if(r.attemptId!=block) {Require(blocks.insert(r.attemptId).second,"candidate_window_archive_reopened_attempt_block");block=r.attemptId;}
        const auto slash=r.groupId.rfind('/');Require(slash!=std::string::npos,"candidate_window_archive_group_order");bool ok=false;
        const auto ordinal=QString::fromStdString(r.groupId.substr(slash+1)).toULongLong(&ok);
        Require(ok && ordinal>0 && ordinal<=9007199254740991ULL && (!lastOrdinal.count(r.attemptId) || ordinal>lastOrdinal.at(r.attemptId)),
            "candidate_window_archive_group_order");lastOrdinal[r.attemptId]=ordinal;
        if(r.purpose!=CompositeMeasurementPurpose::Reference)Require(!references.empty() &&
            std::all_of(references.begin(),references.end(),[&](const auto& id){return seenGroups.count(id);}),
            "candidate_window_archive_candidate_precedes_references");
        seenGroups.insert(r.groupId);prefix.AddRecord(r);attempts[r.attemptId].AddRecord(r);
        const auto prior=caps.find(r.attemptId);Require(prior==caps.end() || prior->second==r.policy.maximumTotalPrimaryBytes,
            "candidate_window_archive_attempt_primary_cap");caps[r.attemptId]=r.policy.maximumTotalPrimaryBytes;
        Require(attempts.at(r.attemptId).bytes<=r.policy.maximumTotalPrimaryBytes,"candidate_window_archive_attempt_primary_budget");
        Require(prefix.bytes<=r.policy.maximumTotalPrimaryBytes,"candidate_window_archive_retained_prefix_primary_budget");
    }
    Require(!blocks.count(current) || block==current,"candidate_window_archive_current_attempt_order");
    Require(prefix.bytes<=currentCap,"candidate_window_archive_current_retained_primary_budget");
}
}
namespace CompositeEvidenceDetail {
void ValidateInitialSource(const VerificationProtocol& p,const QJsonObject& source,const std::string& origin) {
    ValidateVerificationProtocol(p);
    Require(p.schemaVersion==4 || p.schemaVersion==5,"composite_initial_source_explicit_protocol_required");
    CheckSource(p,source,origin);
}
}
QJsonObject EncodeNativeCompositeSelection(const NativeDeclaredWindowBasinResult& s) {
    const char* status=s.status==NativeDeclaredWindowBasinStatus::Rejected?"rejected":s.status==NativeDeclaredWindowBasinStatus::Completed?"completed":s.status==NativeDeclaredWindowBasinStatus::CausalityRejected?"causality_rejected":nullptr;
    Require(status,"composite_archive_selection_status");return {{"status",status},{"status_enum",int(s.status)},{"reasons",Strings(s.reasons)},
        {"actual_assigned_onset",s.actualAssignedOnset?QJsonValue(Number(*s.actualAssignedOnset)):QJsonValue(QJsonValue::Null)},
        {"geometry",s.geometry?QJsonValue(Geometry(*s.geometry)):QJsonValue(QJsonValue::Null)}};
}
std::string CompositeTrainingSemanticProofId(const VerificationProtocol& p,const NativeCompositeMeasurementRecord& r) {
    Require(!r.candidateWindowSpecification && !r.windowProposal,"candidate_window_record_requires_archive2");
    Require(r.selection && r.selection->status==NativeDeclaredWindowBasinStatus::Completed && r.selection->geometry &&
        r.selection->geometry->validity==DeclaredWindowBasinValidity::Valid && r.selection->geometry->measuredMaximum,"composite_archive_measurement_not_valid");
    const auto candidate=Hash(ParseStrictNativeJsonObject(r.candidateBindingJson)).toStdString();
    return CompositeTrainingSemanticProofId(p,CompositeSemanticProofContent{r.sourceOrigin,candidate,r.liveAttemptBinding,r.attemptLimits,
        r.maximumGroupModelSeconds,r.maximumGroupWallSeconds,r.logical.trialId,r.groupId,r.purpose,r.group,*r.selection->geometry->measuredMaximum});
}
std::string CompositeTrainingSemanticProofId(const VerificationProtocol& p,const CompositeSemanticProofContent& r) {
    const auto& group=r.nativeGroup;const auto& peak=r.actualPeak;std::vector<const MatchedOmissionTrace*> traces{&group.witness.fullBefore};
    for(const auto& c:group.witness.omitEachInput)traces.push_back(&c);traces.push_back(&group.witness.fullAfter);
    Require(traces.size()==group.packets.size(),"composite_archive_proof_primary_count");QJsonArray primary;
    for(std::size_t i=0;i<traces.size();++i)primary.append(QJsonObject{{"native",EncodeNativeProbeEvidence(group.packets[i])},
        {"csv_sha256",BytesHash(EncodeMatchedOmissionPrimaryTrace(*traces[i],group.packets[i]))}});
    const auto& a=r.attemptLimits;
    return Hash(QJsonObject{{"kind","live-composite-training-measurement-v1"},{"method_binding",CompositeTrainingProtocolBinding(p)},
        {"source_origin",Text(r.sourceOrigin)},{"candidate_witness",Text(r.candidateWitnessId)},{"live_attempt_binding",Text(r.liveAttemptBinding)},
        {"attempt_limits",QJsonArray{Count(a.maximumGroups),Count(a.maximumNativeProbes),Count(a.maximumTotalSamples),a.maximumModelSeconds,a.maximumWallSeconds}},
        {"group_limits",QJsonArray{r.maximumGroupModelSeconds,r.maximumGroupWallSeconds}},
        {"logical_trial",Text(r.logicalTrialId)},{"group_id",Text(r.groupId)},{"purpose",int(r.purpose)},
        {"primary",primary},{"raw",peak.rawPotential},{"time",peak.time},{"plateau_end",peak.plateauEnd}}).toStdString();
}

namespace {
void CheckRecord(const VerificationProtocol& p,const QJsonObject& source,const NativeCompositeMeasurementRecord& r) {
    const bool windowMode=p.schemaVersion==5;
    if(windowMode) {
        ValidateCandidateWindowLogicalContext(p,r.logical,r.attemptId,r.groupId,r.purpose);
        Require(r.policy.schemaVersion==2 && r.candidateWindowSpecification,"candidate_window_archive_record_contract");
        const auto& s=*r.candidateWindowSpecification;
        Require(s.groupId==r.groupId && s.inputId==r.logical.inputId && s.assignedInput==r.logical.inputIndex &&
            s.channel==(p.routing==ProbeRouting::ClassicBurst?r.logical.inputIndex:0u),"candidate_window_archive_specification_binding");
        if(r.purpose==CompositeMeasurementPurpose::Reference)
            Require(!r.windowProposal && s.coordinateMode==CandidateWindowCoordinateMode::ReferenceTime &&
                s.referenceTime==p.candidateWindowMeasurement->referenceInputWindows.at(r.logical.inputIndex),"candidate_window_archive_reference_specification");
        else Require(r.windowProposal && s.coordinateMode==CandidateWindowCoordinateMode::CandidateFrame && s.candidateFrame &&
            r.windowProposal->computation.window==s.candidateFrame,"candidate_window_archive_proposal_specification");
    } else {Require(!r.candidateWindowSpecification && !r.windowProposal,"candidate_window_record_requires_archive2");CheckLogical(p,r);}
    const auto candidate=ParseStrictNativeJsonObject(r.candidateBindingJson);
    Keys(candidate,windowMode?"kind schema_version source_origin model_identity storage_images_sha256 optional_C0_witness physical_readback":
        "kind source_origin model_identity storage_images_sha256 optional_C0_witness");
    Require((windowMode?(candidate["kind"]==QJsonValue("composite_train_candidate_binding_v2") && Count(candidate["schema_version"])==2):
        candidate["kind"]==QJsonValue("composite_train_candidate_binding_v1")) && candidate["source_origin"]==Text(r.sourceOrigin) &&
        candidate["storage_images_sha256"]==source["storage_images_sha256"] && !r.ownerFamilyBinding.empty() && !r.liveAttemptBinding.empty(),"composite_archive_candidate_binding");
    CheckIdentity(Object(candidate["model_identity"]));Require(Object(candidate["model_identity"])["sha256"]==Text(r.logical.baseModelRevision),"composite_archive_candidate_revision");
    const auto& a=r.attemptLimits;Require(a.maximumGroups && a.maximumNativeProbes>=p.inputs.size()+2 && a.maximumTotalSamples &&
        Number(a.maximumModelSeconds)>0. && Number(a.maximumWallSeconds)>0. && Number(r.maximumGroupModelSeconds)>0. &&
        r.maximumGroupModelSeconds<=a.maximumModelSeconds && Number(r.maximumGroupWallSeconds)>0. && r.maximumGroupWallSeconds<=a.maximumWallSeconds &&
        r.policy.maximumTotalPrimaryBytes && r.issuedNativeProbes<=p.inputs.size()+2 && r.issuedNativeProbes<=a.maximumNativeProbes &&
        r.sampledFrames<=a.maximumTotalSamples && r.policy.contextPolicy==ContextPolicy(p) && r.policy.geometryPolicy==GeometryPolicy(p),
        "composite_archive_record_budget");
    if(windowMode)Require(r.policy.methodId==DeclaredWindowBasinResult::MethodId() && r.policy.maximumGroups==a.maximumGroups &&
        r.policy.maximumNativeProbes==a.maximumNativeProbes && r.policy.maximumTotalSamples==a.maximumTotalSamples &&
        r.policy.maximumGroupModelSeconds==r.maximumGroupModelSeconds && r.policy.maximumGroupWallSeconds==r.maximumGroupWallSeconds,
        "candidate_window_archive_policy_duplicates");
    Require(Number(r.startModel)<=Number(r.endModel) && Number(r.startWall)<=Number(r.endWall),"composite_archive_clock_order");
    std::size_t retained=0;std::set<std::string> rawIds,packetIds;for(const auto& raw:r.rawTrials) {
        const auto& spec=raw.actual;const auto index=rawIds.size();Require(index<r.issuedNativeProbes &&
            spec.trialId==r.groupId+"/native/"+std::to_string(index) && rawIds.insert(spec.trialId).second &&
            raw.prepared.baseModelRevision==r.logical.baseModelRevision && raw.prepared.modelEpoch==r.logical.epoch &&
            raw.prepared.inputId==r.logical.inputId && raw.prepared.protocolRevision==p.revision && raw.prepared.routing==p.routing &&
            spec.runId==r.logical.runId && spec.sampleId==p.sampleId && spec.schemaVersion==6 && spec.stimulusSubset &&
            spec.stimulusSubset->sourceTrainingPatternHash==p.patternHash && spec.stimulusSubset->physicalInputCount==p.inputs.size() &&
            spec.maximumSamples<=ContextPolicy(p).maximumSamples && spec.stimulusProfile==p.stimulusProfile &&
            spec.maximumSampleGap==p.maximumSampleGap && spec.minimumAmplitude==p.minimumAmplitude && !spec.recordSpikes &&
            spec.waveAssignment==WaveAssignmentPolicy::WindowOnly && spec.peakSelection==PeakSelectionPolicy::IsolatedWindow &&
            (p.routing==ProbeRouting::ClassicBurst?!raw.prepared.selectedInputIndex:raw.prepared.selectedInputIndex==r.logical.inputIndex),
            "composite_archive_raw_context");
        std::vector<std::size_t> mask;for(std::size_t i=0;i<p.inputs.size();++i)if(!index || index==p.inputs.size()+1 || i!=index-1)mask.push_back(i);
        Require(spec.stimulusSubset->sourceOrdinals==mask,"composite_archive_raw_source_mask");
        std::vector<double> original;for(const auto& isi:Array(source["applied_source_isi"]))original.push_back(Number(isi));
        QJsonArray presented;for(double isi:PlanStimulusSubset(original,mask).presentedIsi)presented.append(isi);
        std::vector<std::string> expectedIds;for(auto ordinal:mask)expectedIds.push_back(p.stimulusIds[ordinal]);
        Require(spec.patternHash==Hash(presented).toStdString() && spec.expectedStimulusIds==expectedIds,"composite_archive_raw_source_plan");
        retained=Add(retained,raw.samples.size());(void)EncodeCompositeRawPrimary(raw);
    }
    Require(retained==r.sampledFrames && retained<=ContextPolicy(p).maximumTotalSamples &&
        CompositeRecordPrimaryBytes(r)<=r.policy.maximumTotalPrimaryBytes,"composite_archive_retained_sample_charge");
    for(const auto& packet:r.group.packets) {
        Require(packet.requestedSpecification && packetIds.insert(packet.requestedSpecification->trialId).second,"composite_archive_packet_identity");
        const auto prefix=r.groupId+"/native/";const auto& id=packet.requestedSpecification->trialId;
        Require(id.starts_with(prefix) && Decimal(id.substr(prefix.size())),"composite_archive_packet_namespace");
        bool ok=false;const auto ordinal=QString::fromStdString(id.substr(prefix.size())).toULongLong(&ok);
        Require(ok && ordinal<r.issuedNativeProbes,"composite_archive_packet_ordinal");
        (void)DecodeNativeProbeEvidence(EncodeNativeProbeEvidence(packet));
        const auto raw=std::find_if(r.rawTrials.begin(),r.rawTrials.end(),[&](const auto& raw){return raw.actual.trialId==id;});
        if(raw!=r.rawTrials.end()) {
            if(packet.prepared)Require(EncodeNativeProbePreparation(*packet.prepared)==EncodeNativeProbePreparation(raw->prepared),"composite_archive_raw_prepared_mismatch");
            if(packet.observation) {
                Require(EncodeNativeProbeSpecification(packet.observation->specification)==EncodeNativeProbeSpecification(raw->actual),"composite_archive_raw_specification_mismatch");
                std::vector<StimulusObservation> events;for(const auto& frame:raw->samples)for(const auto& event:frame.newStimuli)events.push_back(event);
                Require(events.size()==packet.observation->stimuli.size(),"composite_archive_raw_event_mismatch");
                for(std::size_t i=0;i<events.size();++i)Require(events[i].id==packet.observation->stimuli[i].id && events[i].time==packet.observation->stimuli[i].time,"composite_archive_raw_event_mismatch");
            }
        }
    }
    if(candidate["optional_C0_witness"].isNull()) {
        Require(source["optional_C0_witness"].isNull() && r.capacitanceWitnesses.empty() && r.nativeTrialWitnessIds.empty(),"composite_archive_unbound_C0_map");
    } else {
        const auto c=DecodeNativeTrainingCapacitanceWitness(Object(candidate["optional_C0_witness"]));
        Require(!source["optional_C0_witness"].isNull() && c.parameters.origin==Text(source["optional_C0_origin"]) &&
            c.modelRevision==r.logical.baseModelRevision && ParseStrictNativeJsonObject(c.modelIdentityJson)==Object(candidate["model_identity"]),"composite_archive_candidate_C0");
        std::set<std::string> required;Require(r.nativeTrialWitnessIds.size()==r.issuedNativeProbes,"composite_archive_raw_C0_inventory");
        for(std::size_t i=0;i<r.issuedNativeProbes;++i) {
            const auto found=r.nativeTrialWitnessIds.find(r.groupId+"/native/"+std::to_string(i));Require(found!=r.nativeTrialWitnessIds.end(),"composite_archive_raw_C0_inventory");
            const auto witness=r.capacitanceWitnesses.find(found->second);Require(witness!=r.capacitanceWitnesses.end() &&
                witness->second.modelRevision==r.logical.baseModelRevision && NativeTrainingCapacitanceWitnessId(witness->second)==found->second &&
                witness->second.parameters==c.parameters,"composite_archive_raw_C0_binding");required.insert(found->second);
        }
        Require(required.size()==r.capacitanceWitnesses.size(),"composite_archive_extra_C0_witness");
    }
    Require(!r.consumed || (r.trainingRequestedAtConsume==true && !r.semanticProofId.empty()),"composite_archive_consumption_without_request");
}
QJsonValue CompleteGroup(const NativeCompositeMeasurementRecord& r,const std::vector<QString>& paths,const MatchedOmissionPrimary& primary) {
    if(r.group.packets.size()!=r.rawTrials.size() || r.rawTrials.size()!=r.group.sourceIsi.size()+2)return QJsonValue(QJsonValue::Null);
    try {return EncodeMatchedOmissionEvidence(r.group,paths,primary);}
    catch(const std::invalid_argument&){return QJsonValue(QJsonValue::Null);}
}
QJsonObject EncodeRecord(const VerificationProtocol& p,const QJsonObject& source,const NativeCompositeMeasurementRecord& r,
    const std::vector<QString>& paths,const MatchedOmissionPrimary& primary) {
    CheckRecord(p,source,r);Require(paths.size()==r.rawTrials.size(),"composite_archive_primary_inventory");QJsonArray rawTrials,packets;
    for(std::size_t i=0;i<r.rawTrials.size();++i) {
        const auto& raw=r.rawTrials[i];const auto found=primary.find(paths[i]);Require(found!=primary.end() && found->second==EncodeCompositeRawPrimary(raw),"composite_archive_primary_bytes");
        rawTrials.append(QJsonObject{{"native_trial_id",Text(raw.actual.trialId)},{"prepared_context",EncodeNativeProbePreparation(raw.prepared)},
            {"actual_specification",EncodeNativeProbeSpecification(raw.actual)},{"primary_ref",QJsonObject{{"relative_path",paths[i]},
                {"bytes",Count(std::size_t(found->second.size()))},{"sha256",BytesHash(found->second)}}},{"actual_stimuli",Events(raw)}});
    }
    for(const auto& packet:r.group.packets)packets.append(EncodeNativeProbeEvidence(packet));
    const auto native=CompleteGroup(r,paths,primary);const auto candidate=ParseStrictNativeJsonObject(r.candidateBindingJson);
    if(p.schemaVersion==5)Require(r.groupState!=NativeMatchedOmissionState::Completed || native.isObject(),
        "candidate_window_archive_completed_state_without_native_closure");
    if(p.schemaVersion==5 && native.isObject())Require(r.group.specification.resetBinding=="native-neuron-dataset-reset-before-children-v1" &&
        (!r.windowProposal || r.group.specification.resetBinding==r.windowProposal->candidate.grid.resetBinding),"candidate_window_archive_native_reset_binding");
    if(native.isObject() && r.selection)Require(EncodeNativeCompositeSelection(*r.selection)==EncodeNativeCompositeSelection(
        FreshSelection(p,r)),"composite_archive_stale_diagnostic_selection");
    if(!native.isObject() && r.selection)Require(!r.selection->geometry && r.selection->status==NativeDeclaredWindowBasinStatus::Rejected,
        "composite_archive_partial_geometry_claim");
    QJsonValue measured(QJsonValue::Null);if(!r.semanticProofId.empty()) {
        Require(native.isObject() && r.selection && EncodeNativeCompositeSelection(*r.selection)==EncodeNativeCompositeSelection(
            FreshSelection(p,r)) &&
            (p.schemaVersion==5?CandidateWindowTrainingSemanticProofId(p,r):CompositeTrainingSemanticProofId(p,r))==r.semanticProofId,"composite_archive_stale_measurement");
        const auto& peak=*r.selection->geometry->measuredMaximum;const auto ordinal=std::find(p.stimulusIds.begin(),p.stimulusIds.end(),p.originStimulusId)-p.stimulusIds.begin();
        const double origin=r.group.witness.fullBefore.observation.events.at(ordinal).time;
        measured=QJsonObject{{"raw_potential",Number(peak.rawPotential)},{"arrival",Number(peak.time-origin)},
            {"plateau_end",Number(peak.plateauEnd-origin)},{"actual_peak",Maximum(peak)},{"actual_origin_time",Number(origin)},{"semantic_proof_id",Text(r.semanticProofId)}};
    }
    Require(!r.consumed || measured.isObject(),"composite_archive_unmeasured_consumption");
    QJsonObject witnesses,trialWitnesses;for(const auto& [id,w]:r.capacitanceWitnesses)witnesses[Text(id)]=EncodeNativeTrainingCapacitanceWitness(w);
    for(const auto& [trial,id]:r.nativeTrialWitnessIds)trialWitnesses[Text(trial)]=Text(id);
    QJsonObject encoded{{"kind","timelearner_composite_measurement"},{"schema_version",p.schemaVersion==5?2:1},{"logical_request",Request(r.logical)},
        {"purpose",Purpose(r.purpose)},{"group_id",Text(r.groupId)},{"attempt_id",Text(r.attemptId)},{"source_origin",Text(r.sourceOrigin)},
        {"owner_family_binding",Text(r.ownerFamilyBinding)},{"live_attempt_binding",Text(r.liveAttemptBinding)},
        {"method_specification",p.schemaVersion==5?EncodeCandidateWindowBasinSpecification(*r.candidateWindowSpecification):Specification(Specification(p,r))},
        {"geometry_policy",EncodeCompositeGeometryPolicy(GeometryPolicy(p))},
        {"candidate_binding",candidate},{"candidate_witness_id",Hash(candidate)},{"group_state",StateName(r.groupState)},
        {"complete_group",native.isObject()},{"native_group",native},{"raw_trials",rawTrials},{"native_packets",packets},
        {"selection",r.selection?QJsonValue(EncodeNativeCompositeSelection(*r.selection)):QJsonValue(QJsonValue::Null)},
        {"measurement",measured},{"consumed",r.consumed},{"training_requested_at_consume",r.trainingRequestedAtConsume?QJsonValue(*r.trainingRequestedAtConsume):QJsonValue(QJsonValue::Null)},
        {"rejection_reason",Text(r.reason)},{"budgets",Budgets(r)},
        {"counters",QJsonObject{{"native_probes",Count(r.issuedNativeProbes)},{"sampled_frames",Count(r.sampledFrames)},
            {"retained_frames",Count(r.sampledFrames)},{"primary_bytes",Count(CompositeRecordPrimaryBytes(r))}}},
        {"clock_observation",QJsonObject{{"start_model_time",Number(r.startModel)},{"start_wall_time",Number(r.startWall)},
            {"end_model_time",Number(r.endModel)},{"end_wall_time",Number(r.endWall)},
            {"wall_enforcement",r.semanticProofId.empty()?"not_completed":"live_checked_before_mint"}}},
        {"capacitance_witnesses",witnesses},{"native_trial_witness_ids",trialWitnesses}};
    if(p.schemaVersion==5)encoded["window_proposal"]=r.windowProposal?QJsonValue(EncodeCandidateWindowProposalData(*r.windowProposal)):QJsonValue(QJsonValue::Null);
    return encoded;
}
void CheckAcceptedSets(const VerificationProtocol& p,const QJsonObject& source,const std::vector<NativeCompositeMeasurementRecord>& records,
    const std::vector<std::string>& refs,const std::vector<std::string>& finals,const std::string& currentAttempt) {
    Require(refs.size()<=p.inputs.size() && finals.size()<=p.inputs.size() && (finals.empty() || refs.size()==p.inputs.size()),"composite_archive_accepted_set_dimension");
    std::set<std::string> used;const auto find=[&](const std::string& id)->const NativeCompositeMeasurementRecord& {
        const auto found=std::find_if(records.begin(),records.end(),[&](const auto& r){return r.groupId==id;});
        Require(found!=records.end() && used.insert(id).second && found->consumed && found->trainingRequestedAtConsume==true &&
            !found->semanticProofId.empty(),"composite_archive_accepted_group");return *found;
    };
    std::string latestActualRun;std::optional<qulonglong> latestSweep;
    const auto finalPrefix=currentAttempt+"/final/";
    for(const auto& record:records)if(record.purpose==CompositeMeasurementPurpose::Final && record.attemptId==currentAttempt) {
        Require(record.logical.runId.starts_with(finalPrefix) && Decimal(record.logical.runId.substr(finalPrefix.size())),"composite_archive_final_run_namespace");
        bool ok=false;const auto sweep=QString::fromStdString(record.logical.runId.substr(finalPrefix.size())).toULongLong(&ok);
        Require(ok && sweep<=9007199254740991ULL,"composite_archive_final_sweep_number");
        if(!latestSweep || sweep>*latestSweep){latestSweep=sweep;latestActualRun=record.logical.runId;}
    }
    std::string referenceCandidate,finalCandidate,finalRevision,finalRun;std::size_t finalEpoch=0,previous=0;
    for(std::size_t i=0;i<refs.size();++i){const auto& r=find(refs[i]);Require(r.purpose==CompositeMeasurementPurpose::Reference && r.logical.inputIndex==i &&
        r.logical.baseModelRevision==Text(Object(source["initial_model"])["sha256"]) && r.logical.epoch==Count(source["initial_epoch"]),"composite_archive_reference_origin");
        const auto candidate=Hash(ParseStrictNativeJsonObject(r.candidateBindingJson)).toStdString();if(!i)referenceCandidate=candidate;
        Require(candidate==referenceCandidate,"composite_archive_mixed_reference_candidate");}
    for(std::size_t i=0;i<finals.size();++i){const auto& r=find(finals[i]);const auto candidate=Hash(ParseStrictNativeJsonObject(r.candidateBindingJson)).toStdString();
        if(!i){finalCandidate=candidate;finalRevision=r.logical.baseModelRevision;finalEpoch=r.logical.epoch;finalRun=r.logical.runId;}
        Require(r.purpose==CompositeMeasurementPurpose::Final && r.attemptId==currentAttempt && r.logical.runId==finalRun && r.logical.runId==latestActualRun && (!i || r.logical.inputIndex>previous) &&
            candidate==finalCandidate && r.logical.baseModelRevision==finalRevision && r.logical.epoch==finalEpoch,"composite_archive_mixed_final_candidate");previous=r.logical.inputIndex;
    }
}
}
NativeCompositeArchiveDocument EncodeNativeCompositeTrainingArchive(const NativeTrainingService& service,const QString& prefix) {
    SafePath(prefix);const auto& p=service.Plan().protocol;ValidateCompositeMeasurementContract(p);const auto& source=service.CompositeSourceBinding();
    Require(!source.isEmpty(),"composite_archive_missing_initial_source");const auto origin=Hash(source).toStdString();CheckSource(p,source,origin);
    const auto& records=service.CompositeMeasurements();const auto& references=service.CompositeReferenceGroupIds();const auto finals=service.CompositeFinalGroupIds();
    CheckAcceptedSets(p,source,records,references,finals,service.Configuration().runId);const auto family=records.empty()?std::string{}:records.front().ownerFamilyBinding;
    Require(!family.empty() || records.empty(),"composite_archive_missing_family");NativeCompositeArchiveDocument output;QJsonArray encoded;QJsonObject blobs;
    std::set<std::string> groups,trials,proofs;std::set<QString> folded;
    for(std::size_t index=0;index<records.size();++index) {
        const auto& r=records[index];Require(r.sourceOrigin==origin && r.ownerFamilyBinding==family && groups.insert(r.groupId).second &&
            trials.insert(r.logical.trialId).second && (r.semanticProofId.empty() || proofs.insert(r.semanticProofId).second),"composite_archive_replayed_group");
        MatchedOmissionPrimary primary;std::vector<QString> paths;
        for(std::size_t i=0;i<r.rawTrials.size();++i) {
            const auto name=i==0?QString("full-before.csv"):i+1==p.inputs.size()+2?QString("full-after.csv"):QString("omit-%1.csv").arg(qulonglong(i-1));
            const auto path=prefix+QString("/group-%1/").arg(qulonglong(index+1),6,10,QLatin1Char('0'))+name;SafePath(path);
            Require(folded.insert(path.toCaseFolded()).second,"composite_archive_colliding_path");const auto bytes=EncodeCompositeRawPrimary(r.rawTrials[i]);
            paths.push_back(path);primary.emplace(path,bytes);output.primary.emplace(path,bytes);
            const auto sha=BytesHash(bytes);const QJsonObject blob{{"bytes",Count(std::size_t(bytes.size()))},{"base64",QString::fromLatin1(bytes.toBase64())}};
            Require(!blobs.contains(sha) || blobs[sha]==QJsonValue(blob),"composite_archive_blob_collision");blobs[sha]=blob;
        }
        encoded.append(EncodeRecord(p,source,r,paths,primary));
    }
    const auto counters=Counters(records,service.Configuration().runId);
    Require(Object(counters["attempt"])["logical_groups"]==Count(service.CompositeGroups()) &&
        Object(counters["attempt"])["native_probes"]==Count(service.CompositeNativeProbes()) &&
        Object(counters["attempt"])["sampled_frames"]==Count(service.CompositeSampledFrames()) &&
        Object(counters["archive"])["primary_bytes"]==Count(service.CompositePrimaryBytes()) &&
        service.CompositePrimaryBytes()<=service.Plan().composite->maximumTotalPrimaryBytes,"composite_archive_service_counter_mismatch");
    // A just-started, zero-group family still has its owned immutable identity.
    const auto ownedFamily=service.CompositeOwnerFamilyBinding();Require(!ownedFamily.empty() && (records.empty() || family==ownedFamily),"composite_archive_owner_family");
    output.document={{"kind","timelearner_composite_measurement_archive"},{"schema_version",1},{"method_id",DeclaredWindowBasinResult::MethodId()},
        {"protocol",EncodeVerificationProtocol(p)},{"source_origin",Text(origin)},{"owner_family_binding",Text(ownedFamily)},{"source_binding",source},
        {"maximum_total_primary_bytes",Count(service.Plan().composite->maximumTotalPrimaryBytes)},{"current_attempt_id",Text(service.Configuration().runId)},
        {"records",encoded},{"reference_group_ids",Strings(references)},{"final_group_ids",Strings(finals)},{"counters",counters},{"primary_blobs",blobs}};
    output.document["artifact_document_sha256"]=Hash(output.document);return output;
}

NativeCompositeArchiveDocument EncodeNativeCandidateWindowTrainingArchive(const NativeCompositeTrainingAttempt& attempt,const QString& prefix) {
    SafePath(prefix);const auto& p=*attempt.protocol;ValidateCandidateWindowMeasurementContract(p);
    Require(attempt.windowDeclaration && !attempt.Running() && (!attempt.group || attempt.closed),"candidate_window_archive_requires_group_boundary");
    const auto& source=attempt.sourceBinding;const auto origin=attempt.sourceOrigin;CheckSource(p,source,origin);
    const auto& records=attempt.windowRecords;const auto current=attempt.windowDeclaration->attemptId;
    std::vector<std::string> references;
    if(attempt.windowReferences)for(std::size_t input=0;input<p.inputs.size();++input) {
        const auto found=std::find_if(records.begin(),records.end(),[&](const auto& r){return r.purpose==CompositeMeasurementPurpose::Reference && r.logical.inputIndex==input && r.consumed;});
        Require(found!=records.end(),"candidate_window_archive_complete_reference_record_missing");references.push_back(found->groupId);
    }
    // This STEP2 archive never promotes a Final measurement to old Train4
    // consumption/certificate. Final selection is a future explicit boundary.
    const std::vector<std::string> finals;CheckAcceptedSets(p,source,records,references,finals,current);
    CheckCandidateWindowPrimaryHistory(records,references,current,attempt.windowDeclaration->maximumTotalPrimaryBytes);
    NativeCompositeArchiveDocument output;QJsonArray encoded;QJsonObject blobs;std::set<QString> folded;
    for(std::size_t index=0;index<records.size();++index) {
        const auto& r=records[index];std::vector<QString> paths;MatchedOmissionPrimary primary;
        for(std::size_t i=0;i<r.rawTrials.size();++i) {
            const auto name=i==0?QString("full-before.csv"):i==p.inputs.size()+1?QString("full-after.csv"):QString("omit-%1.csv").arg(qulonglong(i-1));
            const auto path=prefix+QString("/group-%1/").arg(qulonglong(index+1),6,10,QLatin1Char('0'))+name;SafePath(path);
            Require(folded.insert(path.toCaseFolded()).second,"composite_archive_colliding_path");const auto bytes=EncodeCompositeRawPrimary(r.rawTrials[i]);
            paths.push_back(path);primary.emplace(path,bytes);output.primary.emplace(path,bytes);const auto sha=BytesHash(bytes);
            const QJsonObject blob{{"bytes",Count(std::size_t(bytes.size()))},{"base64",QString::fromLatin1(bytes.toBase64())}};
            Require(!blobs.contains(sha) || blobs[sha]==QJsonValue(blob),"composite_archive_blob_collision");blobs[sha]=blob;
        }
        encoded.append(EncodeRecord(p,source,r,paths,primary));
    }
    const auto counters=Counters(records,current);const auto cap=attempt.windowDeclaration->maximumTotalPrimaryBytes;
    Require(Object(counters["attempt"])["logical_groups"]==Count(attempt.IssuedGroups()) &&
        Object(counters["attempt"])["native_probes"]==Count(attempt.IssuedNativeProbes()) &&
        Object(counters["attempt"])["sampled_frames"]==Count(attempt.RetainedSamples()) &&
        Object(counters["archive"])["primary_bytes"]==Count(attempt.windowPrimaryBytes) && attempt.windowPrimaryBytes<=cap,
        "candidate_window_archive_factory_counter_mismatch");
    output.document={{"kind","timelearner_composite_measurement_archive"},{"schema_version",2},{"method_id",DeclaredWindowBasinResult::MethodId()},
        {"protocol",EncodeVerificationProtocol(p)},{"source_origin",Text(origin)},{"owner_family_binding",Text(attempt.ownerScope)},{"source_binding",source},
        {"maximum_total_primary_bytes",Count(cap)},{"current_attempt_id",Text(current)},{"records",encoded},
        {"reference_group_ids",Strings(references)},{"final_group_ids",Strings(finals)},{"counters",counters},{"primary_blobs",blobs}};
    output.document["artifact_document_sha256"]=Hash(output.document);
    // The writer never exports data that its independent strict decoder cannot
    // replay, including all proposal math and original reference primary.
    (void)DecodeNativeCompositeTrainingArchive(output.document,[&](const QString& path){return output.primary.at(path);});return output;
}

DecodedNativeCompositeArchive DecodeNativeCompositeTrainingArchive(const QJsonObject& object,
    const std::function<QByteArray(const QString&)>& resolver) {
    Keys(object,"kind schema_version method_id protocol source_origin owner_family_binding source_binding maximum_total_primary_bytes current_attempt_id records reference_group_ids final_group_ids counters primary_blobs artifact_document_sha256");
    const auto schema=Count(object["schema_version"]);Require(schema==1 || schema==2,"composite_archive_explicit_schema");const bool windowMode=schema==2;
    auto unsignedObject=object;unsignedObject.remove("artifact_document_sha256");Require(object["kind"]==QJsonValue("timelearner_composite_measurement_archive") &&
        object["method_id"]==QJsonValue(DeclaredWindowBasinResult::MethodId()) &&
        object["artifact_document_sha256"]==QJsonValue(Hash(unsignedObject)),"composite_archive_document_identity");
    DecodedNativeCompositeArchive decoded;decoded.protocol=DecodeVerificationProtocol(Object(object["protocol"]));
    if(windowMode)ValidateCandidateWindowMeasurementContract(decoded.protocol);else ValidateCompositeMeasurementContract(decoded.protocol);
    decoded.sourceBinding=Object(object["source_binding"]);decoded.sourceOrigin=Text(object["source_origin"]);
    decoded.ownerFamilyBinding=Text(object["owner_family_binding"]);decoded.currentAttemptId=Text(object["current_attempt_id"]);
    Require(!decoded.ownerFamilyBinding.empty() && !decoded.currentAttemptId.empty(),"composite_archive_missing_owner_attempt");
    CheckSource(decoded.protocol,decoded.sourceBinding,decoded.sourceOrigin);const auto cap=Count(object["maximum_total_primary_bytes"]);Require(cap>0,"composite_archive_primary_budget");
    decoded.referenceGroups=Strings(object["reference_group_ids"]);decoded.finalGroups=Strings(object["final_group_ids"]);
    const auto& p=decoded.protocol;const auto blobs=Object(object["primary_blobs"]);std::map<QString,QByteArray> decodedBlobs;
    std::size_t stored=0;for(auto i=blobs.begin();i!=blobs.end();++i) {
        const auto blob=Object(i.value());Keys(blob,"bytes base64");const auto encoded=QString::fromStdString(Text(blob["base64"])).toLatin1();const auto bytes=QByteArray::fromBase64(encoded);
        Require(bytes.toBase64()==encoded && Count(blob["bytes"])==std::size_t(bytes.size()) && BytesHash(bytes)==i.key(),"composite_archive_blob_bytes");
        stored=Add(stored,bytes.size());Require(stored<=cap,"composite_archive_blob_budget");decodedBlobs.emplace(i.key(),bytes);
    }
    QJsonArray rebuiltRecords;std::set<QString> pathsUsed,folded,blobUsed;std::set<std::string> groups,trials,proofs;
    std::map<std::string,QJsonObject> attemptCaps;std::map<std::string,Totals> attempts;
    std::map<std::string,std::size_t> attemptPrimaryCaps;
    for(const auto& value:Array(object["records"])) {
        const auto o=Object(value);auto commonFields=o;if(windowMode){Require(o.contains("window_proposal"),"candidate_window_archive_proposal_missing");commonFields.remove("window_proposal");}
        Keys(commonFields,"kind schema_version logical_request purpose group_id attempt_id source_origin owner_family_binding live_attempt_binding method_specification geometry_policy candidate_binding candidate_witness_id group_state complete_group native_group raw_trials native_packets selection measurement consumed training_requested_at_consume rejection_reason budgets counters clock_observation capacitance_witnesses native_trial_witness_ids");
        Require(o["kind"]==QJsonValue("timelearner_composite_measurement") && Count(o["schema_version"])==schema,"composite_archive_record_schema");NativeCompositeMeasurementRecord r;
        r.logical=Request(o["logical_request"]);r.purpose=Purpose(o["purpose"]);r.groupId=Text(o["group_id"]);r.attemptId=Text(o["attempt_id"]);
        r.sourceOrigin=Text(o["source_origin"]);r.ownerFamilyBinding=Text(o["owner_family_binding"]);r.liveAttemptBinding=Text(o["live_attempt_binding"]);
        Require(r.sourceOrigin==decoded.sourceOrigin && r.ownerFamilyBinding==decoded.ownerFamilyBinding && groups.insert(r.groupId).second && trials.insert(r.logical.trialId).second,
            "composite_archive_mixed_or_replayed_group");
        const auto candidate=Object(o["candidate_binding"]);r.candidateBindingJson=QJsonDocument(candidate).toJson(QJsonDocument::Compact).toStdString();
        Require(o["candidate_witness_id"]==QJsonValue(Hash(candidate)),"composite_archive_candidate_sha");r.groupState=State(o["group_state"]);
        r.consumed=Boolean(o["consumed"]);if(!o["training_requested_at_consume"].isNull())r.trainingRequestedAtConsume=Boolean(o["training_requested_at_consume"]);
        r.reason=Text(o["rejection_reason"]);r.policy.schemaVersion=unsigned(schema);r.policy.contextPolicy=ContextPolicy(p);r.policy.geometryPolicy=GeometryPolicy(p);
        if(windowMode) {
            r.candidateWindowSpecification=DecodeCandidateWindowBasinSpecification(Object(o["method_specification"]));
            if(!o["window_proposal"].isNull())r.windowProposal=DecodeCandidateWindowProposalData(Object(o["window_proposal"]));
        }
        const auto budget=Object(o["budgets"]);Keys(budget,"attempt_limits maximum_group_model_seconds maximum_group_wall_seconds maximum_total_primary_bytes");
        const auto limits=Object(budget["attempt_limits"]);Keys(limits,"maximum_groups maximum_native_probes maximum_total_samples maximum_model_seconds maximum_wall_seconds");
        r.attemptLimits={Count(limits["maximum_groups"]),Count(limits["maximum_native_probes"]),Count(limits["maximum_total_samples"]),Number(limits["maximum_model_seconds"]),Number(limits["maximum_wall_seconds"])};
        r.maximumGroupModelSeconds=Number(budget["maximum_group_model_seconds"]);r.maximumGroupWallSeconds=Number(budget["maximum_group_wall_seconds"]);
        r.policy.maximumTotalPrimaryBytes=Count(budget["maximum_total_primary_bytes"]);r.policy.maximumGroups=r.attemptLimits.maximumGroups;
        r.policy.maximumNativeProbes=r.attemptLimits.maximumNativeProbes;r.policy.maximumTotalSamples=r.attemptLimits.maximumTotalSamples;
        r.policy.maximumGroupModelSeconds=r.maximumGroupModelSeconds;r.policy.maximumGroupWallSeconds=r.maximumGroupWallSeconds;
        if(windowMode) {
            const auto previous=attemptPrimaryCaps.find(r.attemptId);
            Require((previous==attemptPrimaryCaps.end() || previous->second==r.policy.maximumTotalPrimaryBytes) &&
                (r.attemptId!=decoded.currentAttemptId || r.policy.maximumTotalPrimaryBytes==cap),"candidate_window_archive_attempt_primary_cap");
            attemptPrimaryCaps[r.attemptId]=r.policy.maximumTotalPrimaryBytes;
        }
        const auto priorCaps=attemptCaps.find(r.attemptId);Require(priorCaps==attemptCaps.end() || priorCaps->second==limits,"composite_archive_mixed_attempt_caps");attemptCaps[r.attemptId]=limits;
        const auto counters=Object(o["counters"]);Keys(counters,"native_probes sampled_frames retained_frames primary_bytes");
        r.issuedNativeProbes=Count(counters["native_probes"]);r.sampledFrames=Count(counters["sampled_frames"]);
        const auto clock=Object(o["clock_observation"]);Keys(clock,"start_model_time start_wall_time end_model_time end_wall_time wall_enforcement");
        r.startModel=Number(clock["start_model_time"]);r.startWall=Number(clock["start_wall_time"]);r.endModel=Number(clock["end_model_time"]);r.endWall=Number(clock["end_wall_time"]);
        std::vector<QString> paths;MatchedOmissionPrimary primary;
        for(const auto& rawValue:Array(o["raw_trials"])) {
            const auto raw=Object(rawValue);const auto ref=Object(raw["primary_ref"]);Keys(ref,"relative_path bytes sha256");const auto path=QString::fromStdString(Text(ref["relative_path"]));SafePath(path);
            Require(pathsUsed.insert(path).second && folded.insert(path.toCaseFolded()).second,"composite_archive_reused_or_colliding_path");
            const auto sha=QString::fromStdString(Text(ref["sha256"]));const auto found=decodedBlobs.find(sha);
            Require(found!=decodedBlobs.end() && Count(ref["bytes"])==std::size_t(found->second.size()),"composite_archive_missing_primary_blob");
            if(resolver)Require(resolver(path)==found->second,"composite_archive_external_primary_mismatch");
            blobUsed.insert(sha);paths.push_back(path);primary.emplace(path,found->second);r.rawTrials.push_back(ReadRaw(raw,found->second));
        }
        std::vector<NativeProbeResult> packets;for(const auto& packet:Array(o["native_packets"]))packets.push_back(DecodeNativeProbeEvidence(Object(packet)));
        const bool complete=Boolean(o["complete_group"]);Require(complete==o["native_group"].isObject(),"composite_archive_complete_group_binding");
        if(complete) {
            r.group=DecodeMatchedOmissionEvidence(Object(o["native_group"]),primary);
            Require(r.group.maximumModelSeconds==r.maximumGroupModelSeconds && r.group.maximumWallSeconds==r.maximumGroupWallSeconds &&
                r.group.specification.groupId==r.groupId && r.group.specification.inputId==r.logical.inputId &&
                r.group.specification.runId==r.logical.runId && r.group.specification.baseModelRevision==r.logical.baseModelRevision &&
                r.group.specification.modelEpoch==r.logical.epoch && r.group.specification.patternHash==p.patternHash &&
                r.group.specification.assignedInput==r.logical.inputIndex && r.group.specification.routing==p.routing &&
                r.group.specification.channel==(p.routing==ProbeRouting::ClassicBurst?r.logical.inputIndex:0u) &&
                r.group.specification.protocolRevision==p.revision && r.group.specification.sampleId==p.sampleId &&
                r.group.specification.stimulusIds==p.stimulusIds && r.group.specification.stimulusProfile==p.stimulusProfile &&
                r.group.specification.minimumAmplitude==p.minimumAmplitude &&
                r.group.sourceIsi.size()==Array(decoded.sourceBinding["applied_source_isi"]).size() && r.group.policy==ContextPolicy(p),"composite_archive_native_group_binding");
            const auto isi=Array(decoded.sourceBinding["applied_source_isi"]);for(int i=0;i<isi.size();++i)
                Require(r.group.sourceIsi[std::size_t(i)]==Number(isi[i]),"composite_archive_group_source_isi");
            Require(r.group.packets.size()==packets.size(),"composite_archive_packet_inventory");for(std::size_t i=0;i<packets.size();++i)
                Require(EncodeNativeProbeEvidence(r.group.packets[i])==EncodeNativeProbeEvidence(packets[i]),"composite_archive_packet_inventory");
            for(std::size_t i=0;i<r.rawTrials.size();++i)Require(EncodeCompositeRawPrimary(r.rawTrials[i])==primary.at(paths[i]),"composite_archive_raw_primary_binding");
            if(!o["selection"].isNull())r.selection=FreshSelection(p,r);
        } else {
            Require(o["native_group"].isNull() && o["measurement"].isNull() && !r.consumed,"composite_archive_partial_promotion");r.group.packets=std::move(packets);
            if(!o["selection"].isNull()) {const auto selection=Object(o["selection"]);Keys(selection,"status status_enum reasons actual_assigned_onset geometry");
                Require(selection["status"]==QJsonValue("rejected") && Count(selection["status_enum"])==0 && selection["geometry"].isNull() && selection["actual_assigned_onset"].isNull(),
                    "composite_archive_partial_geometry_claim");NativeDeclaredWindowBasinResult rejected;rejected.reasons=Strings(selection["reasons"]);r.selection=std::move(rejected);}
        }
        for(auto& raw:r.rawTrials)for(const auto& packet:r.group.packets)if(packet.requestedSpecification && packet.requestedSpecification->trialId==raw.actual.trialId)raw.packet=packet;
        const auto witnesses=Object(o["capacitance_witnesses"]);for(auto i=witnesses.begin();i!=witnesses.end();++i) {
            auto witness=DecodeNativeTrainingCapacitanceWitness(Object(i.value()));Require(NativeTrainingCapacitanceWitnessId(witness)==i.key().toStdString(),"composite_archive_C0_sha");
            r.capacitanceWitnesses.emplace(i.key().toStdString(),std::move(witness));}
        const auto trialWitnesses=Object(o["native_trial_witness_ids"]);for(auto i=trialWitnesses.begin();i!=trialWitnesses.end();++i)r.nativeTrialWitnessIds.emplace(i.key().toStdString(),Text(i.value()));
        if(!o["measurement"].isNull()) {const auto measurement=Object(o["measurement"]);Keys(measurement,"raw_potential arrival plateau_end actual_peak actual_origin_time semantic_proof_id");
            r.semanticProofId=Text(measurement["semantic_proof_id"]);Require(!r.semanticProofId.empty() && proofs.insert(r.semanticProofId).second,"composite_archive_replayed_semantic_proof");}
        const auto rebuilt=EncodeRecord(p,decoded.sourceBinding,r,paths,primary);Require(rebuilt==o,"composite_archive_record_replay_mismatch");
        attempts[r.attemptId].AddRecord(r);const auto& total=attempts.at(r.attemptId);
        Require(total.groups<=r.attemptLimits.maximumGroups && total.native<=r.attemptLimits.maximumNativeProbes &&
            total.samples<=r.attemptLimits.maximumTotalSamples,"composite_archive_attempt_counter_budget");
        rebuiltRecords.append(rebuilt);decoded.records.push_back(std::move(r));
    }
    Require(blobUsed.size()==decodedBlobs.size(),"composite_archive_extra_primary_blob");CheckAcceptedSets(p,decoded.sourceBinding,decoded.records,
        decoded.referenceGroups,decoded.finalGroups,decoded.currentAttemptId);
    if(windowMode)CheckCandidateWindowPrimaryHistory(decoded.records,decoded.referenceGroups,decoded.currentAttemptId,cap);
    const auto counters=Counters(decoded.records,decoded.currentAttemptId);Require(Object(counters["archive"])["primary_bytes"].toDouble()<=double(cap),"composite_archive_total_primary_budget");
    Require(object["counters"]==QJsonValue(counters),"composite_archive_counter_replay_mismatch");
    decoded.document=object;
    if(windowMode) {
        std::optional<CandidateWindowReferenceSet> references;
        if(decoded.referenceGroups.size()==p.inputs.size())references=CandidateWindowPortableReferenceFactory::Replay(decoded);
        for(const auto& r:decoded.records) {
            const auto candidate=ParseStrictNativeJsonObject(r.candidateBindingJson,128u*1024u*1024u);
            if(references) {
                const auto checked=CandidateWindowProposalReplayFactory::CheckCandidateBinding(*references,p,candidate);
                if(r.purpose!=CompositeMeasurementPurpose::Reference) {
                    Require(r.windowProposal && r.candidateWindowSpecification,"candidate_window_archive_candidate_without_proposal");
                    // A pre-preparation cancellation can retain its frozen
                    // declaration, but attests no actual native maximumSamples.
                    const auto maximum=r.rawTrials.empty()?r.windowProposal->candidate.declaredMaximumSamples:r.rawTrials.front().actual.maximumSamples;
                    for(const auto& raw:r.rawTrials)Require(raw.actual.maximumSamples==maximum,"candidate_window_archive_actual_sample_budget_changed");
                    const auto replay=CandidateWindowProposalReplayFactory::Replay(*references,p,checked,r.logical,r.attemptId,r.groupId,
                        maximum,r.purpose,*r.windowProposal);
                    Require(replay.Record().computation.window==r.candidateWindowSpecification->candidateFrame,"candidate_window_archive_window_replay");
                }
            } else Require(r.purpose==CompositeMeasurementPurpose::Reference && !r.windowProposal && !r.consumed,
                "candidate_window_archive_candidate_without_complete_references");
        }
    }
    return decoded;
}
DecodedNativeCompositeArchive DecodeNativeCompositeTrainingArchive(const QByteArray& bytes,
    const std::function<QByteArray(const QString&)>& resolver) {
    return DecodeNativeCompositeTrainingArchive(ParseStrictNativeJsonObject(bytes.toStdString(),128*1024*1024),resolver);
}
}
