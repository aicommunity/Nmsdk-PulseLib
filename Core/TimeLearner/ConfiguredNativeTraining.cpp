#include "ConfiguredNativeTraining.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <cmath>
#include <limits>
#include <set>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
void Keys(const QJsonObject& o,std::initializer_list<const char*> names)
{
    Require(o.size()==int(names.size()),"native_configuration_field_set_mismatch");
    for(auto name:names)Require(o.contains(name),"missing_native_configuration_field");
}
double Number(const QJsonValue& v)
{Require(v.isDouble() && std::isfinite(v.toDouble()),"invalid_native_configuration_number");return v.toDouble();}
QJsonValue Numeric(double value)
{Require(std::isfinite(value),"nonfinite_native_configuration_number");return value;}
std::size_t Count(const QJsonValue& v,std::size_t maximum=std::numeric_limits<std::size_t>::max())
{
    const auto d=Number(v);Require(d>=0. && d<=9007199254740991. && d<=double(maximum) && std::floor(d)==d,
        "invalid_native_configuration_integer");return std::size_t(d);
}
QJsonValue Integer(std::size_t value)
{Require(value<=9007199254740991ULL,"unrepresentable_native_configuration_integer");return double(value);}
int Int(const QJsonValue& v){return int(Count(v,std::numeric_limits<int>::max()));}
bool Boolean(const QJsonValue& v)
{Require(v.isBool(),"invalid_native_configuration_boolean");return v.toBool();}
std::string Text(const QJsonValue& v)
{Require(v.isString() && !v.toString().trimmed().isEmpty(),"invalid_native_configuration_string");return v.toString().toStdString();}
QJsonObject Object(const QJsonValue& v)
{Require(v.isObject(),"invalid_native_configuration_object");return v.toObject();}
QJsonArray Array(const QJsonValue& v)
{Require(v.isArray(),"invalid_native_configuration_array");return v.toArray();}
QString Norm(Normalization n)
{Require(n==Normalization::SynapseCount || n==Normalization::TipResistance,"invalid_native_configuration_normalization");return n==Normalization::SynapseCount?"N0":"N1";}
Normalization Norm(const QJsonValue& v)
{const auto n=Text(v);Require(n=="N0" || n=="N1","invalid_native_configuration_normalization");return n=="N0"?Normalization::SynapseCount:Normalization::TipResistance;}
QString Selection(PeakSelectionPolicy p)
{Require(p==PeakSelectionPolicy::IsolatedWindow || p==PeakSelectionPolicy::AssignedBurstWave,"invalid_native_configuration_selection");return p==PeakSelectionPolicy::IsolatedWindow?"isolated_window":"assigned_burst_wave";}
// Qt validates syntax first. This second traversal preserves object-member
// multiplicity, which QJsonObject by itself would silently discard.
struct DuplicateKeys {
    QByteArray bytes;int position=0;
    void Space(){while(position<bytes.size() && (bytes[position]==' ' || bytes[position]=='\n' || bytes[position]=='\r' || bytes[position]=='\t'))++position;}
    QByteArray String(){const int start=position++;while(position<bytes.size()){const char c=bytes[position++];if(c=='\\')++position;else if(c=='"')break;}return bytes.mid(start,position-start);}
    void Value(unsigned depth=0)
    {
        Require(depth<=64,"native_configuration_depth_exceeded");Space();const char c=bytes.at(position);
        if(c=='{')
        {
            ++position;Space();std::set<QString> seen;if(bytes.at(position)=='}'){++position;return;}
            while(true)
            {
                Space();const auto key=QJsonDocument::fromJson(QByteArray("[")+String()+"]").array().at(0).toString();
                Require(seen.insert(key).second,"duplicate_native_configuration_key");Space();++position;Value(depth+1);Space();
                if(bytes.at(position++)=='}')break;
            }
        }
        else if(c=='[')
        {
            ++position;Space();if(bytes.at(position)==']'){++position;return;}
            while(true){Value(depth+1);Space();if(bytes.at(position++)==']')break;}
        }
        else if(c=='"')String();
        else while(position<bytes.size() && bytes[position]!=',' && bytes[position]!='}' && bytes[position]!=']')++position;
    }
};
}
QJsonObject EncodeNativeModelSpec(const NativeModelSpec& s)
{
    ValidateNativeModelSpec(s);QJsonArray inputs,isi,allocation,inhibition;
    for(const auto& p:s.inputs)inputs.append(QJsonObject{{"attachment",p.attachment},{"synapses",p.synapses},
        {"resistance",p.resistance?Numeric(*p.resistance):QJsonValue(QJsonValue::Null)}});
    for(double v:s.initialIsi)isi.append(Numeric(v));for(int v:s.allocatedCableLengths)allocation.append(v);
    for(const auto& r:s.editPolicy.inhibition)inhibition.append(QJsonObject{{"input_index",Integer(r.inputIndex)},{"offset",r.offset},{"resistance",Numeric(r.resistance)}});
    return {{"schema_version",1},{"branch",s.branch},{"frequency_hz",double(s.frequencyHz)},
        {"neuron_class",QString::fromStdString(s.neuronClass)},{"dataset_class",QString::fromStdString(s.datasetClass)},
        {"generator_class",QString::fromStdString(s.generatorClass)},{"neuron_structure_mode",s.neuronStructureMode},
        {"normalization",Norm(s.normalization)},{"inputs",inputs},{"initial_isi",isi},{"allocated_cable_lengths",allocation},
        {"maximum_cable_length",s.editPolicy.maximumCableLength},{"preserve_allocated_cable",s.editPolicy.preserveAllocatedCable},
        {"maximum_resistance_ratio",Numeric(s.editPolicy.maximumResistanceRatio)},
        {"configured_minimum_resistance",Numeric(s.editPolicy.configuredMinimumResistance)},{"inhibition",inhibition},
        {"scale_inhibition_with_input",s.scaleInhibitionWithInput},{"allow_muted_input_baseline",s.allowMutedInputBaseline}};
}
NativeModelSpec DecodeNativeModelSpec(const QJsonObject& o)
{
    Keys(o,{"schema_version","branch","frequency_hz","neuron_class","dataset_class","generator_class","neuron_structure_mode",
        "normalization","inputs","initial_isi","allocated_cable_lengths","maximum_cable_length","preserve_allocated_cable",
        "maximum_resistance_ratio","configured_minimum_resistance","inhibition","scale_inhibition_with_input","allow_muted_input_baseline"});
    Require(Int(o["schema_version"])==1,"unsupported_native_model_spec_schema");NativeModelSpec s;
    s.branch=Boolean(o["branch"]);s.frequencyHz=unsigned(Count(o["frequency_hz"],std::numeric_limits<unsigned>::max()));
    s.neuronClass=Text(o["neuron_class"]);s.datasetClass=Text(o["dataset_class"]);s.generatorClass=Text(o["generator_class"]);
    s.neuronStructureMode=Int(o["neuron_structure_mode"]);s.normalization=Norm(o["normalization"]);s.editPolicy.normalization=s.normalization;
    for(const auto& v:Array(o["inputs"]))
    {const auto p=Object(v);Keys(p,{"attachment","synapses","resistance"});s.inputs.push_back({Int(p["attachment"]),Int(p["synapses"]),
        p["resistance"].isNull()?std::nullopt:std::optional<double>(Number(p["resistance"]))});}
    for(const auto& v:Array(o["initial_isi"]))s.initialIsi.push_back(Number(v));
    for(const auto& v:Array(o["allocated_cable_lengths"]))s.allocatedCableLengths.push_back(Int(v));
    s.editPolicy.maximumCableLength=Int(o["maximum_cable_length"]);s.editPolicy.preserveAllocatedCable=Boolean(o["preserve_allocated_cable"]);
    s.editPolicy.maximumResistanceRatio=Number(o["maximum_resistance_ratio"]);s.editPolicy.configuredMinimumResistance=Number(o["configured_minimum_resistance"]);
    for(const auto& v:Array(o["inhibition"]))
    {const auto r=Object(v);Keys(r,{"input_index","offset","resistance"});s.editPolicy.inhibition.push_back({Count(r["input_index"]),Int(r["offset"]),Number(r["resistance"])});}
    s.scaleInhibitionWithInput=Boolean(o["scale_inhibition_with_input"]);s.allowMutedInputBaseline=Boolean(o["allow_muted_input_baseline"]);
    ValidateNativeModelSpec(s);return s;
}
NativeTrainingPlan MakeConfiguredNativeTrainingPlan(const ConfiguredNativeTraining& c)
{
    ValidateNativeModelSpec(c.model);const auto size=c.model.inputs.size();
    Require(ValidStimulusProfile(c.stimulusProfile) && c.stimulusProfile.generatorClass==c.model.generatorClass &&
        c.stimulusProfile.frequencyHz==c.model.frequencyHz,"invalid_configured_native_stimulus_profile");
    Require(!c.runId.empty() && !c.protocolRevision.empty() && !c.sampleId.empty() && c.windows.size()==size &&
        std::isfinite(c.duration) && c.duration>0. && std::isfinite(c.maximumSampleGap) && c.maximumSampleGap>=1./c.model.frequencyHz &&
        std::isfinite(c.minimumAmplitude) && c.minimumAmplitude>0. && c.maximumSamples>0 &&
        c.maximumReferenceProbes>=size && std::isfinite(c.maximumModelSeconds) && c.maximumModelSeconds>0. &&
        std::isfinite(c.maximumWallSeconds) && c.maximumWallSeconds>0.,"invalid_configured_native_training");
    Require(c.search.normalization==c.model.normalization && c.search.bounds.size()==size && c.search.maximumProbes &&
        c.search.maximumEdits && c.search.maximumCandidatesPerInput && c.search.maximumSweeps && c.search.resistanceSubdivisionDepth<=20,
        "invalid_configured_native_search");
    Require(c.search.n0CandidateRepeats>=1 && c.search.n0CandidateRepeats<=128 &&
        (c.search.normalization==Normalization::SynapseCount || c.search.n0CandidateRepeats==1),"invalid_n0_candidate_repeats");
    Require(c.search.initialModelRevision.empty() && c.search.initialModelEpoch==0,"configuration_is_not_controller_checkpoint");
    for(double value:{c.search.tolerances.amplitudeAbsolute,c.search.tolerances.amplitudeRelative,c.search.tolerances.sync})
        Require(std::isfinite(value) && value>=0.,"invalid_configured_native_tolerance");
    NativeTrainingPlan plan;auto& p=plan.protocol;p.schemaVersion=3;
    p.routing=c.model.branch?ProbeRouting::BranchSoloBurst:ProbeRouting::ClassicBurst;
    p.revision=c.protocolRevision;p.sampleId=c.sampleId;p.originStimulusId="pulse0";
    p.maximumSampleGap=c.maximumSampleGap;p.minimumAmplitude=c.minimumAmplitude;p.stimulusProfile=c.stimulusProfile;
    p.waveAssignment=WaveAssignmentPolicy::OrderedWholeBurst;p.peakSelection=c.peakSelection;Selection(c.peakSelection);
    QJsonArray pattern;for(double v:c.model.initialIsi)pattern.append(v);
    p.patternHash=NmsdkTools::CanonicalModelDataSha256(pattern).toStdString();
    auto& request=plan.probe;request.routing=p.routing;request.protocolRevision=p.revision;request.expectedInputIsi=c.model.initialIsi;
    auto& s=request.specification;s.schemaVersion=3;s.sampleId=p.sampleId;s.patternHash=p.patternHash;s.end=c.duration;
    s.maximumSampleGap=c.maximumSampleGap;s.minimumAmplitude=c.minimumAmplitude;s.maximumSamples=c.maximumSamples;
    s.recordSpikes=c.recordSpikes;s.waveAssignment=p.waveAssignment;s.peakSelection=p.peakSelection;s.stimulusProfile=p.stimulusProfile;
    double onset=0.;
    for(std::size_t i=0;i<size;++i)
    {
        const auto& b=c.search.bounds[i];const auto& input=c.model.inputs[i];
        Require(b.minimumLength>=1 && b.maximumLength>=b.minimumLength && b.maximumLength<=c.model.editPolicy.maximumCableLength &&
            b.minimumSynapses>=1 && b.maximumSynapses>=b.minimumSynapses && b.maximumSynapses<=128 &&
            std::isfinite(b.minimumResistance) && std::isfinite(b.maximumResistance) && b.minimumResistance>0. && b.maximumResistance>=b.minimumResistance &&
            input.attachment>=b.minimumLength && input.attachment<=b.maximumLength && input.synapses>=b.minimumSynapses && input.synapses<=b.maximumSynapses &&
            (!input.resistance || (*input.resistance>=b.minimumResistance && *input.resistance<=b.maximumResistance)),"invalid_configured_native_bounds");
        const auto [begin,end]=c.windows[i];onset+=c.model.initialIsi[i];
        Require(std::isfinite(begin) && std::isfinite(end) && begin>=0. && end>begin && end<=c.duration && onset<c.duration,
            "invalid_configured_native_window");
        const auto id="input"+std::to_string(i),event="pulse"+std::to_string(i);
        p.inputs.push_back({id,event});p.stimulusIds.push_back(event);s.expectedStimulusIds.push_back(event);
        plan.inputWindows.push_back({id,c.model.branch?0u:i,begin,end,event});
    }
    ValidateVerificationProtocol(p);plan.search=c.search;plan.search.runId=c.runId;
    plan.maximumReferenceProbes=c.maximumReferenceProbes;plan.maximumInvalidReferenceProbes=c.maximumInvalidReferenceProbes;
    plan.maximumModelSeconds=c.maximumModelSeconds;plan.maximumWallSeconds=c.maximumWallSeconds;return plan;
}
QJsonObject EncodeConfiguredNativeTraining(const ConfiguredNativeTraining& c)
{
    MakeConfiguredNativeTrainingPlan(c);QJsonArray windows,bounds;
    for(auto [begin,end]:c.windows)windows.append(QJsonObject{{"begin",Numeric(begin)},{"end",Numeric(end)}});
    for(const auto& b:c.search.bounds)bounds.append(QJsonObject{{"minimum_length",b.minimumLength},{"maximum_length",b.maximumLength},
        {"minimum_synapses",b.minimumSynapses},{"maximum_synapses",b.maximumSynapses},{"minimum_resistance",Numeric(b.minimumResistance)},
        {"maximum_resistance",Numeric(b.maximumResistance)}});
    const auto& source=c.stimulusProfile;
    QJsonObject result{{"kind","timelearner_native_cold_configuration"},{"schema_version",1},{"model",EncodeNativeModelSpec(c.model)},
        {"stimulus_profile",QJsonObject{{"generator_class",QString::fromStdString(source.generatorClass)},
            {"amplitude",Numeric(source.amplitude)},{"pulse_length",Numeric(source.pulseLength)},{"frequency_hz",Numeric(source.frequencyHz)},
            {"frequency_deviation",Numeric(source.frequencyDeviation)},{"use_transit_signal",source.useTransitSignal},{"use_pattern_output",source.usePatternOutput}}},
        {"run_id",QString::fromStdString(c.runId)},{"protocol_revision",QString::fromStdString(c.protocolRevision)},{"sample_id",QString::fromStdString(c.sampleId)},
        {"duration",Numeric(c.duration)},{"maximum_sample_gap",Numeric(c.maximumSampleGap)},{"minimum_amplitude",Numeric(c.minimumAmplitude)},
        {"maximum_samples",Integer(c.maximumSamples)},{"record_spikes",c.recordSpikes},{"peak_selection",Selection(c.peakSelection)},{"windows",windows},
        {"search",QJsonObject{{"maximum_probes",Integer(c.search.maximumProbes)},{"maximum_edits",Integer(c.search.maximumEdits)},
            {"maximum_candidates_per_input",Integer(c.search.maximumCandidatesPerInput)},{"maximum_sweeps",Integer(c.search.maximumSweeps)},
            {"maximum_invalid_observations",Integer(c.search.maximumInvalidObservations)},{"resistance_subdivision_depth",double(c.search.resistanceSubdivisionDepth)},
            {"bounds",bounds},{"tolerances",QJsonObject{{"amplitude_absolute",Numeric(c.search.tolerances.amplitudeAbsolute)},
                {"amplitude_relative",Numeric(c.search.tolerances.amplitudeRelative)},{"sync",Numeric(c.search.tolerances.sync)}}}}},
        {"maximum_reference_probes",Integer(c.maximumReferenceProbes)},{"maximum_invalid_reference_probes",Integer(c.maximumInvalidReferenceProbes)},
        {"maximum_model_seconds",Numeric(c.maximumModelSeconds)},{"maximum_wall_seconds",Numeric(c.maximumWallSeconds)}};
    if(c.search.n0CandidateRepeats!=1)
    {
        result["schema_version"]=2;
        auto search=result["search"].toObject();search["n0_candidate_repeats"]=Integer(c.search.n0CandidateRepeats);
        result["search"]=search;
    }
    return result;
}
ConfiguredNativeTraining DecodeConfiguredNativeTraining(const QJsonObject& o)
{
    Keys(o,{"kind","schema_version","model","stimulus_profile","run_id","protocol_revision","sample_id","duration","maximum_sample_gap","minimum_amplitude",
        "maximum_samples","record_spikes","peak_selection","windows","search","maximum_reference_probes","maximum_invalid_reference_probes",
        "maximum_model_seconds","maximum_wall_seconds"});
    const int schema=Int(o["schema_version"]);
    Require(Text(o["kind"])=="timelearner_native_cold_configuration" && (schema==1 || schema==2),"unsupported_native_configuration_schema");
    ConfiguredNativeTraining c;c.model=DecodeNativeModelSpec(Object(o["model"]));c.runId=Text(o["run_id"]);
    const auto source=Object(o["stimulus_profile"]);Keys(source,{"generator_class","amplitude","pulse_length","frequency_hz",
        "frequency_deviation","use_transit_signal","use_pattern_output"});
    c.stimulusProfile={Text(source["generator_class"]),Number(source["amplitude"]),Number(source["pulse_length"]),Number(source["frequency_hz"]),
        Number(source["frequency_deviation"]),Boolean(source["use_transit_signal"]),Boolean(source["use_pattern_output"])};
    c.protocolRevision=Text(o["protocol_revision"]);c.sampleId=Text(o["sample_id"]);c.duration=Number(o["duration"]);
    c.maximumSampleGap=Number(o["maximum_sample_gap"]);c.minimumAmplitude=Number(o["minimum_amplitude"]);
    c.maximumSamples=Count(o["maximum_samples"]);c.recordSpikes=Boolean(o["record_spikes"]);
    const auto selection=Text(o["peak_selection"]);Require(selection=="isolated_window" || selection=="assigned_burst_wave","invalid_native_configuration_selection");
    c.peakSelection=selection=="isolated_window"?PeakSelectionPolicy::IsolatedWindow:PeakSelectionPolicy::AssignedBurstWave;
    for(const auto& v:Array(o["windows"]))
    {const auto w=Object(v);Keys(w,{"begin","end"});c.windows.emplace_back(Number(w["begin"]),Number(w["end"]));}
    auto s=Object(o["search"]);
    if(schema==2)
    {
        c.search.n0CandidateRepeats=Count(s["n0_candidate_repeats"],128);
        Require(c.search.n0CandidateRepeats>=2 && c.model.normalization==Normalization::SynapseCount,"invalid_n0_candidate_repeats");
        s.remove("n0_candidate_repeats");
    }
    Keys(s,{"maximum_probes","maximum_edits","maximum_candidates_per_input","maximum_sweeps",
        "maximum_invalid_observations","resistance_subdivision_depth","bounds","tolerances"});
    c.search.normalization=c.model.normalization;c.search.runId=c.runId;
    c.search.maximumProbes=Count(s["maximum_probes"]);c.search.maximumEdits=Count(s["maximum_edits"]);
    c.search.maximumCandidatesPerInput=Count(s["maximum_candidates_per_input"]);c.search.maximumSweeps=Count(s["maximum_sweeps"]);
    c.search.maximumInvalidObservations=Count(s["maximum_invalid_observations"]);c.search.resistanceSubdivisionDepth=unsigned(Count(s["resistance_subdivision_depth"],20));
    const auto t=Object(s["tolerances"]);Keys(t,{"amplitude_absolute","amplitude_relative","sync"});
    c.search.tolerances={Number(t["amplitude_absolute"]),Number(t["amplitude_relative"]),Number(t["sync"])};
    for(const auto& v:Array(s["bounds"]))
    {const auto b=Object(v);Keys(b,{"minimum_length","maximum_length","minimum_synapses","maximum_synapses","minimum_resistance","maximum_resistance"});
     c.search.bounds.push_back({Int(b["minimum_length"]),Int(b["maximum_length"]),Int(b["minimum_synapses"]),Int(b["maximum_synapses"]),Number(b["minimum_resistance"]),Number(b["maximum_resistance"])});}
    c.maximumReferenceProbes=Count(o["maximum_reference_probes"]);c.maximumInvalidReferenceProbes=Count(o["maximum_invalid_reference_probes"]);
    c.maximumModelSeconds=Number(o["maximum_model_seconds"]);c.maximumWallSeconds=Number(o["maximum_wall_seconds"]);
    MakeConfiguredNativeTrainingPlan(c);return c;
}
QJsonObject ParseStrictNativeJsonObject(const std::string& text,std::size_t maximumBytes)
{
    Require(!text.empty() && text.size()<=maximumBytes && text.size()<=std::size_t(std::numeric_limits<int>::max()),"invalid_native_configuration_size");
    const auto bytes=QByteArray::fromStdString(text);QJsonParseError error;const auto document=QJsonDocument::fromJson(bytes,&error);
    Require(error.error==QJsonParseError::NoError && document.isObject(),"invalid_native_configuration_json");
    DuplicateKeys{bytes}.Value();return document.object();
}
ConfiguredNativeTraining ParseConfiguredNativeTraining(const std::string& text)
{return DecodeConfiguredNativeTraining(ParseStrictNativeJsonObject(text));}
}
