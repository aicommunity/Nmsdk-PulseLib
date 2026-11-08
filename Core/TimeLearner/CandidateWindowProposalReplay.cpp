#include "CandidateWindowProposalReplay.h"
#include "ConfiguredNativeTraining.h"
#include "ModelIdentity.h"
#include "ModelArchive.h"
#include "ProbeEvidenceJson.h"
#include "CompositeSourceValidation.h"
#include "CandidateWindowLogicalContext.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringList>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
constexpr std::size_t MaximumBindingBytes=128u*1024u*1024u;
void Require(bool v,const char* reason="candidate_window_proposal_field") {if(!v)throw std::invalid_argument(reason);}
void Keys(const QJsonObject& o,const char* fields) {auto names=QString::fromLatin1(fields).split(' ',Qt::SkipEmptyParts);names.sort();Require(o.keys()==names);}
QJsonObject Object(const QJsonValue& v){Require(v.isObject());return v.toObject();}
QJsonArray Array(const QJsonValue& v){Require(v.isArray());return v.toArray();}
QString Text(const std::string& s){return QString::fromStdString(s);}
std::string Text(const QJsonValue& v){Require(v.isString());return v.toString().toStdString();}
std::string Nonempty(const QJsonValue& v){auto s=Text(v);Require(!s.empty());return s;}
double Number(const QJsonValue& v){Require(v.isDouble() && std::isfinite(v.toDouble()));return v.toDouble();}
double Number(double n){Require(std::isfinite(n));return n;}
std::uint64_t Count(const QJsonValue& v){const auto n=Number(v);Require(n>=0. && n<=double(CandidateWindowMaximumExactCount) && std::floor(n)==n);return std::uint64_t(n);}
double Count(std::uint64_t n){Require(n<=CandidateWindowMaximumExactCount);return double(n);}
std::size_t Index(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<std::size_t>::max());return std::size_t(n);}
std::int64_t Signed(const QJsonValue& v){const auto n=Number(v);Require(std::fabs(n)<=double(CandidateWindowMaximumExactCount) && std::floor(n)==n);return std::int64_t(n);}
double Signed(std::int64_t n){Require(n>=-std::int64_t(CandidateWindowMaximumExactCount) && n<=std::int64_t(CandidateWindowMaximumExactCount));return double(n);}
unsigned Version(const QJsonValue& v){const auto n=Count(v);Require(n<=std::numeric_limits<unsigned>::max());return unsigned(n);}
int Integer(const QJsonValue& v){const auto n=Count(v);Require(n>0 && n<=std::numeric_limits<int>::max());return int(n);}
std::string Hash(const QJsonValue& v){return NmsdkTools::CanonicalModelDataSha256(v).toStdString();}
void Bounded(const QJsonObject& o){Require(std::size_t(QJsonDocument(o).toJson(QJsonDocument::Compact).size())<=MaximumBindingBytes,"candidate_window_binding_byte_budget");}
QJsonObject Parameters(const InputParameters& p){Require(p.length>0 && p.synapses>0 && std::isfinite(p.resistance) && p.resistance>0.);
    return {{"length",p.length},{"synapses",p.synapses},{"resistance",p.resistance}};}
QJsonArray Parameters(const std::vector<InputParameters>& values){QJsonArray result;for(const auto& p:values)result.append(Parameters(p));return result;}
std::vector<InputParameters> Parameters(const QJsonValue& v){std::vector<InputParameters> result;for(const auto& item:Array(v)) {
    const auto p=Object(item);Keys(p,"length synapses resistance");InputParameters value{Integer(p["length"]),Integer(p["synapses"]),Number(p["resistance"])};
    Require(Parameters(value)==p);result.push_back(value);}return result;}
QJsonValue Capacitance(const std::optional<TrainingCapacitance>& c){ValidateTrainingCapacitance(c);if(!c)return QJsonValue(QJsonValue::Null);
    QJsonObject scales;for(const auto& [path,value]:c->membraneScales)scales[Text(path)]=Number(value);
    return QJsonObject{{"origin",Text(c->origin)},{"maximum_relative_change",Number(c->maximumRelativeChange)},{"membrane_scales",scales}};}
std::optional<TrainingCapacitance> Capacitance(const QJsonValue& v){if(v.isNull())return std::nullopt;const auto c=Object(v);Keys(c,"origin maximum_relative_change membrane_scales");
    TrainingCapacitance result;result.origin=Text(c["origin"]);result.maximumRelativeChange=Number(c["maximum_relative_change"]);const auto scales=Object(c["membrane_scales"]);
    for(auto i=scales.begin();i!=scales.end();++i)result.membraneScales.emplace(i.key().toStdString(),Number(i.value()));ValidateTrainingCapacitance(result);return result;}
QJsonObject Grid(const CandidateWindowGridBinding& g){Require(g.frequencyHz>0 && !g.sourceOrigin.empty() && !g.sourcePatternBinding.empty() && !g.sourceProfileBinding.empty() &&
    g.frameOriginPolicy=="native-full-before-frame-zero-v1" && g.sourcePhasePolicy=="same-declared-source-native-events-measured-v1" &&
    g.resetBinding=="native-neuron-dataset-reset-before-children-v1","candidate_window_grid_or_source_binding");
    return {{"frequency_hz",Count(g.frequencyHz)},{"source_origin",Text(g.sourceOrigin)},{"source_pattern_binding",Text(g.sourcePatternBinding)},
        {"source_profile_binding",Text(g.sourceProfileBinding)},{"frame_origin_policy",Text(g.frameOriginPolicy)},
        {"source_phase_policy",Text(g.sourcePhasePolicy)},{"reset_binding",Text(g.resetBinding)}};}
CandidateWindowGridBinding Grid(const QJsonValue& v){const auto o=Object(v);Keys(o,"frequency_hz source_origin source_pattern_binding source_profile_binding frame_origin_policy source_phase_policy reset_binding");
    CandidateWindowGridBinding g;g.frequencyHz=Count(o["frequency_hz"]);g.sourceOrigin=Text(o["source_origin"]);g.sourcePatternBinding=Text(o["source_pattern_binding"]);
    g.sourceProfileBinding=Text(o["source_profile_binding"]);g.frameOriginPolicy=Text(o["frame_origin_policy"]);g.sourcePhasePolicy=Text(o["source_phase_policy"]);g.resetBinding=Text(o["reset_binding"]);
    Require(Grid(g)==o);return g;}
QJsonObject Seed(const CandidateWindowArithmeticSeed& s){return {{"input_index",Count(s.inputIndex)},{"first_frame",Count(s.firstFrame)},{"last_frame",Count(s.lastFrame)},
    {"frame_count",Count(s.referenceFrameCount)},{"grid",Grid(s.grid)},{"initial_parameters",Parameters(s.initialParameters)},{"initial_capacitance",Capacitance(s.initialCapacitance)}};}
CandidateWindowArithmeticSeed Seed(const QJsonValue& v){const auto o=Object(v);Keys(o,"input_index first_frame last_frame frame_count grid initial_parameters initial_capacitance");
    CandidateWindowArithmeticSeed s;s.inputIndex=Index(o["input_index"]);s.firstFrame=Count(o["first_frame"]);s.lastFrame=Count(o["last_frame"]);s.referenceFrameCount=Count(o["frame_count"]);
    s.grid=Grid(o["grid"]);s.initialParameters=Parameters(o["initial_parameters"]);s.initialCapacitance=Capacitance(o["initial_capacitance"]);return s;}
QJsonObject Facts(const CandidateWindowCandidateFacts& f){return {{"grid",Grid(f.grid)},{"declared_maximum_samples",Count(f.declaredMaximumSamples)},
    {"parameters",Parameters(f.parameters)},{"capacitance",Capacitance(f.capacitance)}};}
CandidateWindowCandidateFacts Facts(const QJsonValue& v){const auto o=Object(v);Keys(o,"grid declared_maximum_samples parameters capacitance");
    CandidateWindowCandidateFacts f;f.grid=Grid(o["grid"]);f.declaredMaximumSamples=Count(o["declared_maximum_samples"]);f.parameters=Parameters(o["parameters"]);f.capacitance=Capacitance(o["capacitance"]);return f;}
QJsonObject Computation(const CandidateWindowComputation& c){Require(c.status==CandidateWindowComputationStatus::Computed && c.window && c.reasons.empty(),"candidate_window_proposal_not_computed");
    return {{"status","computed"},{"first_frame",Count(c.window->firstFrame)},{"last_frame",Count(c.window->lastFrame)},{"shift_frames",Signed(c.shiftFrames)},
        {"uncertainty_frames",Count(c.uncertaintyFrames)},{"length_change",Count(c.lengthChange)},{"synapse_count_change",Count(c.synapseCountChange)},
        {"resistance_changes",Count(c.resistanceChanges)},{"capacitance_changes",Count(c.capacitanceChanges)},{"feature_terms",Count(c.featureTerms)}};}
CandidateWindowComputation Computation(const QJsonValue& v){const auto o=Object(v);Keys(o,"status first_frame last_frame shift_frames uncertainty_frames length_change synapse_count_change resistance_changes capacitance_changes feature_terms");
    Require(o["status"]==QJsonValue("computed"));CandidateWindowComputation c;c.status=CandidateWindowComputationStatus::Computed;
    c.window=CandidateFrameWindow{Count(o["first_frame"]),Count(o["last_frame"])};c.shiftFrames=Signed(o["shift_frames"]);c.uncertaintyFrames=Count(o["uncertainty_frames"]);
    c.lengthChange=Count(o["length_change"]);c.synapseCountChange=Count(o["synapse_count_change"]);c.resistanceChanges=Count(o["resistance_changes"]);
    c.capacitanceChanges=Count(o["capacitance_changes"]);c.featureTerms=Count(o["feature_terms"]);Require(Computation(c)==o);return c;}
QString Purpose(CompositeMeasurementPurpose p){if(p==CompositeMeasurementPurpose::Search)return "search";if(p==CompositeMeasurementPurpose::Final)return "final";throw std::invalid_argument("candidate_window_proposal_purpose");}
CompositeMeasurementPurpose Purpose(const QJsonValue& v){const auto s=Text(v);if(s=="search")return CompositeMeasurementPurpose::Search;Require(s=="final");return CompositeMeasurementPurpose::Final;}
void NonemptyBindings(const CandidateWindowProposalRecord& r){for(const auto* s:{&r.sourceOrigin,&r.ownerFamilyBinding,&r.referenceProofId,&r.protocolBinding,&r.policyBinding,&r.candidateBinding,
    &r.referenceGroupId,&r.attemptId,&r.runId,&r.trialId,&r.groupId,&r.inputId,&r.candidateModelRevision,&r.windowRecipeId,&r.measurementInstanceId})Require(!s->empty());
    for(const auto* s:{&r.sourceOrigin,&r.ownerFamilyBinding,&r.referenceProofId,&r.protocolBinding,&r.policyBinding,&r.candidateBinding,
        &r.candidateModelRevision,&r.windowRecipeId,&r.measurementInstanceId})Require(s->size()==64 && std::all_of(s->begin(),s->end(),[](char c) {
            return (c>='0' && c<='9') || (c>='a' && c<='f');}),"candidate_window_hash_id_format");}

}
QJsonObject CandidateWindowRecipeContent(const CandidateWindowProposalRecord& r){return {{"kind","candidate_window_recipe_v1"},
    {"policy_binding",Text(r.policyBinding)},{"reference_proof_id",Text(r.referenceProofId)},{"source_origin",Text(r.sourceOrigin)},
    {"owner_family_binding",Text(r.ownerFamilyBinding)},{"protocol_binding",Text(r.protocolBinding)},{"input_id",Text(r.inputId)},
    {"input_index",Count(r.inputIndex)},{"channel",Count(r.channel)},{"candidate_model_revision",Text(r.candidateModelRevision)},
    {"model_epoch",Count(r.modelEpoch)},{"candidate_binding",Text(r.candidateBinding)},{"reference",Seed(r.reference)},
    {"candidate",Facts(r.candidate)},{"computation",Computation(r.computation)}};}
QJsonObject CandidateWindowInstanceContent(const CandidateWindowProposalRecord& r){return {{"kind","candidate_window_instance_v1"},
    {"window_recipe_id",Text(r.windowRecipeId)},{"attempt_id",Text(r.attemptId)},{"run_id",Text(r.runId)},
    {"trial_id",Text(r.trialId)},{"group_id",Text(r.groupId)},{"purpose",Purpose(r.purpose)}};}
QJsonObject EncodeCandidateWindowProposalData(const CandidateWindowProposalRecord& r){Require(r.schemaVersion==1 && r.policyId=="reference-anchored-candidate-window-v1");NonemptyBindings(r);
    const auto n=r.reference.initialParameters.size();Require(n>=2 && r.inputIndex<n && r.reference.inputIndex==r.inputIndex &&
        r.candidate.parameters.size()==n && r.reference.grid==r.candidate.grid && r.reference.grid.sourceOrigin==r.sourceOrigin &&
        r.reference.referenceFrameCount>=3 && r.reference.firstFrame>0 && r.reference.firstFrame<=r.reference.lastFrame &&
        r.reference.lastFrame<r.reference.referenceFrameCount-1 && r.computation.window &&
        r.computation.window->firstFrame<r.computation.window->lastFrame && r.computation.window->lastFrame<r.candidate.declaredMaximumSamples,
        "candidate_window_proposal_data_dimensions");
    Require(r.windowRecipeId==Hash(CandidateWindowRecipeContent(r)) && r.measurementInstanceId==Hash(CandidateWindowInstanceContent(r)),
        "candidate_window_proposal_self_binding");
    QJsonObject o{{"kind","composite_candidate_window_proposal_v1"},{"schema_version",Count(r.schemaVersion)},{"policy_id",Text(r.policyId)},
        {"source_origin",Text(r.sourceOrigin)},{"owner_family_binding",Text(r.ownerFamilyBinding)},{"reference_proof_id",Text(r.referenceProofId)},
        {"protocol_binding",Text(r.protocolBinding)},{"policy_binding",Text(r.policyBinding)},{"candidate_binding",Text(r.candidateBinding)},
        {"reference_group_id",Text(r.referenceGroupId)},{"attempt_id",Text(r.attemptId)},{"run_id",Text(r.runId)},{"trial_id",Text(r.trialId)},
        {"group_id",Text(r.groupId)},{"input_id",Text(r.inputId)},{"input_index",Count(r.inputIndex)},{"channel",Count(r.channel)},
        {"model_epoch",Count(r.modelEpoch)},{"purpose",Purpose(r.purpose)},{"candidate_model_revision",Text(r.candidateModelRevision)},
        {"window_recipe_id",Text(r.windowRecipeId)},{"measurement_instance_id",Text(r.measurementInstanceId)},
        {"reference",Seed(r.reference)},{"candidate",Facts(r.candidate)},{"computation",Computation(r.computation)}};Bounded(o);return o;}
CandidateWindowProposalRecord DecodeCandidateWindowProposalData(const QJsonObject& o){Bounded(o);
    Keys(o,"kind schema_version policy_id source_origin owner_family_binding reference_proof_id protocol_binding policy_binding candidate_binding reference_group_id attempt_id run_id trial_id group_id input_id input_index channel model_epoch purpose candidate_model_revision window_recipe_id measurement_instance_id reference candidate computation");
    Require(o["kind"]==QJsonValue("composite_candidate_window_proposal_v1"));CandidateWindowProposalRecord r;r.schemaVersion=Version(o["schema_version"]);r.policyId=Text(o["policy_id"]);
    r.sourceOrigin=Nonempty(o["source_origin"]);r.ownerFamilyBinding=Nonempty(o["owner_family_binding"]);r.referenceProofId=Nonempty(o["reference_proof_id"]);
    r.protocolBinding=Nonempty(o["protocol_binding"]);r.policyBinding=Nonempty(o["policy_binding"]);r.candidateBinding=Nonempty(o["candidate_binding"]);
    r.referenceGroupId=Nonempty(o["reference_group_id"]);r.attemptId=Nonempty(o["attempt_id"]);r.runId=Nonempty(o["run_id"]);r.trialId=Nonempty(o["trial_id"]);
    r.groupId=Nonempty(o["group_id"]);r.inputId=Nonempty(o["input_id"]);r.inputIndex=Index(o["input_index"]);r.channel=Index(o["channel"]);r.modelEpoch=Index(o["model_epoch"]);
    r.purpose=Purpose(o["purpose"]);r.candidateModelRevision=Nonempty(o["candidate_model_revision"]);r.windowRecipeId=Nonempty(o["window_recipe_id"]);
    r.measurementInstanceId=Nonempty(o["measurement_instance_id"]);r.reference=Seed(o["reference"]);r.candidate=Facts(o["candidate"]);r.computation=Computation(o["computation"]);
    Require(EncodeCandidateWindowProposalData(r)==o);return r;}
CandidateWindowProposalRecord DecodeCandidateWindowProposalData(const QByteArray& b){return DecodeCandidateWindowProposalData(ParseStrictNativeJsonObject(b.toStdString(),MaximumBindingBytes));}

void CandidateWindowProposalReplayFactory::CheckReferences(const CandidateWindowReferenceSet& refs,const VerificationProtocol& p) {
    ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);const auto n=p.inputs.size();
    Require(refs.seeds.size()==n && refs.referenceGroupIds.size()==n && !refs.sourceOrigin.empty() && !refs.ownerFamilyBinding.empty() &&
        refs.protocolBinding==Hash(EncodeVerificationProtocol(p)),"candidate_window_incomplete_or_wrong_reference_family");
    const auto source=ParseStrictNativeJsonObject(refs.sourceBindingJson,MaximumBindingBytes);
    Require(Hash(source)==refs.sourceOrigin,"candidate_window_source_origin_changed");
    CompositeEvidenceDetail::ValidateInitialSource(p,source,refs.sourceOrigin);
    const auto featureBudget=p.candidateWindowMeasurement->candidateWindowPolicy.maximumFeatureTerms;
    Require(!refs.seeds.front().arithmetic.initialCapacitance ||
        refs.seeds.front().arithmetic.initialCapacitance->membraneScales.size()<=featureBudget-std::uint64_t(n)*3,
        "candidate_window_feature_budget_exhausted");
    std::set<std::string> proofs,groups;
    for(std::size_t i=0;i<n;++i) {
        const auto& seed=refs.seeds[i];const auto& s=seed.arithmetic;
        Require(seed.ownerFamilyBinding==refs.ownerFamilyBinding && !seed.referenceProofId.empty() &&
            proofs.insert(seed.referenceProofId).second && !refs.referenceGroupIds[i].empty() && groups.insert(refs.referenceGroupIds[i]).second &&
            s.inputIndex==i && s.initialParameters.size()==n && s.grid.sourceOrigin==refs.sourceOrigin && s.grid.sourcePatternBinding==p.patternHash &&
            s.grid.sourceProfileBinding==Hash(source["actual_source_profile"]) && s.grid.frequencyHz==p.stimulusProfile->frequencyHz &&
            s.referenceFrameCount>=3 && s.referenceFrameCount<=CandidateWindowMaximumExactCount && s.firstFrame>0 && s.firstFrame<=s.lastFrame &&
            s.lastFrame<s.referenceFrameCount-1,"candidate_window_reference_proof_or_frame_mismatch");
        (void)Seed(s);
        Require(s.initialParameters==refs.seeds.front().arithmetic.initialParameters &&
            s.initialCapacitance==refs.seeds.front().arithmetic.initialCapacitance && s.grid==refs.seeds.front().arithmetic.grid,
            "candidate_window_mixed_initial_reference_features");
    }
}
ReplayedCandidateWindowBinding CandidateWindowProposalReplayFactory::CheckCandidateBinding(const CandidateWindowReferenceSet& refs,
    const VerificationProtocol& p,const QJsonObject& b) {
    CheckReferences(refs,p);Bounded(b);Keys(b,"kind schema_version source_origin model_identity storage_images_sha256 optional_C0_witness physical_readback");
    Require(b["kind"]==QJsonValue("composite_train_candidate_binding_v2") && Version(b["schema_version"])==2 &&
        Text(b["source_origin"])==refs.sourceOrigin,"candidate_window_candidate_binding_version_or_source");
    const auto source=ParseStrictNativeJsonObject(refs.sourceBindingJson,MaximumBindingBytes);
    Require(b["storage_images_sha256"]==source["storage_images_sha256"] && source["storage_images_sha256"]==
        QJsonValue(NmsdkTools::ModelImageDefinitionsFingerprint(Array(source["storage_images"]))),"candidate_window_storage_images_changed");
    const auto identity=Object(b["model_identity"]),model=Object(identity["model"]);const auto version=Version(identity["schema_version"]);
    Require(identity["kind"]==QJsonValue("semantic_model_identity") && version==2 &&
        identity["canonical_encoding"]==QJsonValue("length-framed-utf8-binary64-v1") && identity["valid"].isBool() && identity["valid"].toBool() &&
        Array(identity["diagnostics"]).isEmpty() && Version(model["schema_version"])==version &&
        Text(identity["sha256"])==Hash(model),"candidate_window_candidate_identity");
    Require(model["stimulus_ports"]==QJsonValue(QJsonArray{QJsonObject{{"id","burst"},{"type","matrix_binary64"}}}),
        "candidate_window_actual_source_port");
    const auto physical=Object(b["physical_readback"]);Keys(physical,"parameters input_banks capacitance");
    const auto declared=Array(physical["parameters"]),banks=Array(physical["input_banks"]);const auto n=p.inputs.size();
    Require(std::size_t(declared.size())==n && std::size_t(banks.size())==n,"candidate_window_actual_input_dimensions");
    const auto parameters=Parameters(declared);const auto nativeModel=Object(source["model_spec"]);
    const auto modelSpec=DecodeNativeModelSpec(nativeModel);
    Require(nativeModel["branch"].isBool() && nativeModel["branch"].toBool()==(p.routing==ProbeRouting::BranchSoloBurst) &&
        nativeModel["allow_muted_input_baseline"].isBool(),"candidate_window_actual_routing_declaration");
    const bool branch=p.routing==ProbeRouting::BranchSoloBurst,allowMuted=nativeModel["allow_muted_input_baseline"].toBool();
    std::map<std::string,QJsonObject> nodes;for(const auto& entry:Array(model["nodes"])) {
        const auto node=Object(entry);const auto path=Nonempty(node["path"]);
        Require(nodes.emplace(path,node).second && node["class"].isString() && !node["class"].toString().isEmpty() &&
            node["frequency_hz"]==QJsonValue(QString::number(qulonglong(p.stimulusProfile->frequencyHz))),"candidate_window_graph_node_or_frequency");
    }
    Require(nodes.count("$") && nodes.at("$")["class"]==nativeModel["neuron_class"],"candidate_window_candidate_root_class");
    const auto parameter=[&](const std::string& path,const char* name,const char* type) {
        Require(nodes.count(path),"candidate_window_missing_bank_member");const auto value=Object(Object(nodes.at(path)["parameters"])[name]);
        Require(value["binding"].isBool() && !value["binding"].toBool(),"candidate_window_bound_control_knob");
        const auto data=Object(value["data"]);Require(data["type"]==QJsonValue(type));return data;
    };
    const auto binary64=[&](const std::string& path) {
        const auto value=parameter(path,"Resistance","binary64")["value"];Require(value.isString());const auto text=value.toString();
        bool ok=false;const auto bits=text.toULongLong(&ok,16);Require(ok && text.size()==16 &&
            text==QString::number(bits,16).rightJustified(16,QLatin1Char('0')),"candidate_window_noncanonical_control_resistance");
        double result;static_assert(sizeof(result)==sizeof(bits));std::memcpy(&result,&bits,sizeof(result));
        Require(std::isfinite(result) && result>0.,"candidate_window_invalid_control_resistance");return result;
    };
    std::map<std::string,std::vector<QJsonObject>> inputs;for(const auto& value:Array(model["bindings"])) {
        const auto link=Object(value);inputs[Nonempty(link["target"])].push_back(link);
    }
    for(std::size_t i=0;i<n;++i) {
        const auto& value=parameters[i];Require(value.length<=modelSpec.editPolicy.maximumCableLength && value.synapses<=128 &&
            (!branch || !i || parameters[i-1].length>=value.length),"candidate_window_actual_native_parameter_scope");
        const auto bank=Object(banks[int(i)]);Keys(bank,"input_ordinal input_id stimulus_id receiver_path bank_path attachment_segment excitation_synapse_paths");
        const std::string membrane="$.Dendrite"+std::to_string(branch?1:i+1)+"_"+std::to_string(value.length),channel=membrane+".ExcChannel";
        Require(Index(bank["input_ordinal"])==i && Text(bank["input_id"])==p.inputs[i].id && Text(bank["stimulus_id"])==p.inputs[i].stimulusId &&
            Text(bank["bank_path"])==membrane && Text(bank["receiver_path"])==channel && Count(bank["attachment_segment"])==std::uint64_t(value.length) &&
            nodes.count(channel) && parameter(membrane,"NumExcitatorySynapses","signed_integer")["value"]==QJsonValue(QString::number(value.synapses)),
            "candidate_window_actual_bank_membership");
        const auto members=Array(bank["excitation_synapse_paths"]);Require(members.size()==value.synapses,"candidate_window_actual_bank_membership");
        const auto receivers=inputs[channel+":SynapticInputs"];
        std::optional<bool> connected;
        for(int k=0;k<value.synapses;++k) {
            const auto path=membrane+".ExcSynapse"+std::to_string(k+1);Require(Text(members[k])==path && binary64(path)==value.resistance,"candidate_window_actual_bank_control_knob");
            const auto links=inputs[path+":Input"];Require(links.size()<=1,"candidate_window_actual_stimulus_binding");
            const bool active=links.size()==1;
            if(active)Require(links.front()["kind"]==QJsonValue("stimulus_port") && links.front()["source"]==QJsonValue("burst") &&
                links.front()["slot"]==QJsonValue("0"),"candidate_window_actual_stimulus_binding");
            Require((active || allowMuted) && (!connected || *connected==active),"candidate_window_actual_partial_or_unowned_bank");connected=active;
            const auto found=std::find_if(receivers.begin(),receivers.end(),[&](const auto& link) {
                return link["source"]==Text(path+":Output") && link["kind"]==QJsonValue("property_pointer");
            });Require(found!=receivers.end(),"candidate_window_actual_receiver_membership");
        }
    }
    const auto& initial=refs.seeds.front().arithmetic.initialCapacitance;
    const auto initialTerms=initial?std::uint64_t(initial->membraneScales.size()):0;
    const auto remaining=p.candidateWindowMeasurement->candidateWindowPolicy.maximumFeatureTerms-std::uint64_t(n)*3-initialTerms;
    if(!physical["capacitance"].isNull())Require(std::uint64_t(Object(Object(physical["capacitance"])["membrane_scales"]).size())<=remaining,
        "candidate_window_feature_budget_exhausted");
    const auto capacitance=Capacitance(physical["capacitance"]);
    Require(bool(capacitance)==bool(initial),"candidate_window_capacitance_family_changed");
    if(capacitance) {
        Require(!b["optional_C0_witness"].isNull() && capacitance->origin==initial->origin &&
            capacitance->maximumRelativeChange==initial->maximumRelativeChange,"candidate_window_capacitance_family_changed");
        const auto witness=DecodeNativeTrainingCapacitanceWitness(Object(b["optional_C0_witness"]));
        Require(witness.parameters==*capacitance && witness.modelRevision==Text(identity["sha256"]) &&
            ParseStrictNativeJsonObject(witness.modelIdentityJson,MaximumBindingBytes)==identity,"candidate_window_actual_C0_binding");
    } else Require(b["optional_C0_witness"].isNull(),"candidate_window_unbound_C0_witness");
    CandidateWindowCandidateFacts facts;facts.grid=refs.seeds.front().arithmetic.grid;
    // Physical binding alone does not declare a logical trial's sample budget.
    // Compute supplies the actual requested maximum, bounded by the protocol.
    facts.declaredMaximumSamples=0;facts.parameters=parameters;facts.capacitance=capacitance;
    return ReplayedCandidateWindowBinding(std::move(facts),Text(identity["sha256"]),QJsonDocument(b).toJson(QJsonDocument::Compact).toStdString());
}
CandidateWindowProposalRecord CandidateWindowProposalReplayFactory::Compute(const CandidateWindowReferenceSet& refs,const VerificationProtocol& p,
    const ReplayedCandidateWindowBinding& binding,const VerificationRequest& expected,const std::string& attempt,const std::string& group,
    std::uint64_t declaredMaximumSamples,CompositeMeasurementPurpose purpose) {
    CheckReferences(refs,p);
    Require(purpose==CompositeMeasurementPurpose::Search || purpose==CompositeMeasurementPurpose::Final,"candidate_window_proposal_purpose");
    const auto i=expected.inputIndex;Require(i<p.inputs.size() && expected.inputId==p.inputs[i].id && expected.sampleId==p.sampleId &&
        expected.patternHash==p.patternHash && expected.protocolRevision==p.revision && expected.routing==p.routing &&
        expected.baseModelRevision==binding.modelRevision && expected.epoch<=CandidateWindowMaximumExactCount && !expected.runId.empty() &&
        !expected.trialId.empty() && !attempt.empty() && !group.empty(),"candidate_window_proposal_logical_context");
    ValidateCandidateWindowLogicalContext(p,expected,attempt,group,purpose);
    Require(declaredMaximumSamples>=3 && declaredMaximumSamples<=p.candidateWindowMeasurement->contextPolicy.maximumSamples,
        "candidate_window_declared_sample_budget");
    // The archived candidate facts are rederived from the COMPLETE identity.
    const auto verified=CheckCandidateBinding(refs,p,ParseStrictNativeJsonObject(binding.bindingJson,MaximumBindingBytes));
    const auto& seed=refs.seeds[i];CandidateWindowProposalRecord actual;
    actual.schemaVersion=1;actual.policyId=p.candidateWindowMeasurement->candidateWindowPolicy.policyId;
    actual.sourceOrigin=refs.sourceOrigin;actual.ownerFamilyBinding=refs.ownerFamilyBinding;actual.referenceProofId=seed.referenceProofId;
    actual.protocolBinding=refs.protocolBinding;actual.policyBinding=Hash(EncodeCandidateWindowPolicy(p.candidateWindowMeasurement->candidateWindowPolicy));
    actual.candidateBinding=Hash(ParseStrictNativeJsonObject(verified.bindingJson,MaximumBindingBytes));actual.referenceGroupId=refs.referenceGroupIds[i];
    actual.attemptId=attempt;actual.runId=expected.runId;actual.trialId=expected.trialId;actual.groupId=group;actual.inputId=expected.inputId;
    actual.inputIndex=i;actual.channel=p.routing==ProbeRouting::ClassicBurst?i:0;actual.modelEpoch=expected.epoch;actual.purpose=purpose;
    actual.candidateModelRevision=verified.modelRevision;actual.reference=seed.arithmetic;actual.candidate=verified.facts;
    actual.candidate.declaredMaximumSamples=declaredMaximumSamples;
    actual.computation=CalculateCandidateWindowArithmetic(p.candidateWindowMeasurement->candidateWindowPolicy,actual.reference,actual.candidate);
    Require(actual.computation.status==CandidateWindowComputationStatus::Computed,"candidate_window_proposal_arithmetic_rejected");
    actual.windowRecipeId=Hash(CandidateWindowRecipeContent(actual));actual.measurementInstanceId=Hash(CandidateWindowInstanceContent(actual));
    return actual;
}
ReplayedCandidateWindowProposal CandidateWindowProposalReplayFactory::Replay(const CandidateWindowReferenceSet& refs,const VerificationProtocol& p,
    const ReplayedCandidateWindowBinding& binding,const VerificationRequest& expected,const std::string& attempt,const std::string& group,
    std::uint64_t declaredMaximumSamples,CompositeMeasurementPurpose purpose,const CandidateWindowProposalRecord& claim) {
    auto actual=Compute(refs,p,binding,expected,attempt,group,declaredMaximumSamples,purpose);
    Require(EncodeCandidateWindowProposalData(actual)==EncodeCandidateWindowProposalData(claim),"candidate_window_proposal_replay_mismatch");
    return ReplayedCandidateWindowProposal(std::move(actual));
}
}
