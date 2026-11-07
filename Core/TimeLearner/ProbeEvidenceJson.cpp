#include "ProbeEvidenceJson.h"
#include "NativeTrainingService.h"
#include "NativeTrainingCapacitance.h"
#include "ConfiguredNativeTraining.h"
#include "ModelArchive.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
QString Text(const std::string& value){return QString::fromStdString(value);}
QJsonValue Number(double value)
{if(!std::isfinite(value))throw std::invalid_argument("nonfinite_native_probe_evidence_number");return value;}
QJsonValue Count(std::size_t value)
{if(value>9007199254740991ULL)throw std::invalid_argument("inexact_native_probe_evidence_integer");return double(value);}
QJsonArray Parameters(const std::vector<InputParameters>& values)
{
    QJsonArray result;for(const auto& p:values)result.append(QJsonObject{
        {"length",p.length},{"synapses",p.synapses},{"resistance",Number(p.resistance)}});return result;
}
QJsonObject CapacitanceParameters(const TrainingCapacitance& value)
{
    ValidateTrainingCapacitance(value);QJsonObject scales;
    for(const auto& [path,scale]:value.membraneScales)scales[Text(path)]=Number(scale);
    return {{"origin",Text(value.origin)},{"maximum_relative_change",Number(value.maximumRelativeChange)},
        {"membrane_scales",scales}};
}
QJsonValue Maybe(const std::optional<double>& value)
{return value?Number(*value):QJsonValue(QJsonValue::Null);}
QJsonArray Strings(const std::vector<std::string>& values)
{QJsonArray result;for(const auto& value:values)result.append(Text(value));return result;}
QString Routing(ProbeRouting routing)
{if(routing==ProbeRouting::ClassicBurst)return "classic_burst";if(routing==ProbeRouting::BranchSoloBurst)return "branch_solo_burst";throw std::invalid_argument("unknown_probe_routing");}
QString Assignment(WaveAssignmentPolicy policy)
{if(policy==WaveAssignmentPolicy::WindowOnly)return "window_only";if(policy==WaveAssignmentPolicy::OrderedWholeBurst)return "ordered_whole_burst";throw std::invalid_argument("unknown_wave_assignment");}
QString Selection(PeakSelectionPolicy policy)
{if(policy==PeakSelectionPolicy::IsolatedWindow)return "isolated_window";if(policy==PeakSelectionPolicy::AssignedBurstWave)return "assigned_burst_wave";throw std::invalid_argument("unknown_peak_selection");}
QJsonObject Profile(const StimulusProfile& p)
{return {{"generator_class",Text(p.generatorClass)},{"amplitude",Number(p.amplitude)},{"pulse_length",Number(p.pulseLength)},
 {"frequency_hz",Number(p.frequencyHz)},{"frequency_deviation",Number(p.frequencyDeviation)},
 {"use_transit_signal",p.useTransitSignal},{"use_pattern_output",p.usePatternOutput}};}
QJsonObject Spec(const ProbeSpec& s,const char* timeFrame)
{
    QJsonArray baseline,windows;
    for(double value:s.baseline)baseline.append(Number(value));
    for(const auto& w:s.windows)windows.append(QJsonObject{{"input_id",Text(w.inputId)},{"stimulus_id",Text(w.stimulusId)},
        {"channel",Count(w.channel)},{"begin",Number(w.begin)},{"end",Number(w.end)}});
    QJsonObject result{{"time_frame",timeFrame},{"schema_version",int(s.schemaVersion)},{"run_id",Text(s.runId)},{"trial_id",Text(s.trialId)},
        {"sample_id",Text(s.sampleId)},{"pattern_hash",Text(s.patternHash)},{"effective_model_revision",Text(s.modelRevision)},
        {"begin",Number(s.begin)},{"end",Number(s.end)},{"maximum_sample_gap",Number(s.maximumSampleGap)},
        {"minimum_amplitude",Number(s.minimumAmplitude)},{"maximum_samples",Count(s.maximumSamples)},
        {"record_spikes",s.recordSpikes},{"baseline",baseline},{"windows",windows},
        {"expected_stimulus_ids",Strings(s.expectedStimulusIds)},
        {"stimulus_profile",s.stimulusProfile?QJsonValue(Profile(*s.stimulusProfile)):QJsonValue(QJsonValue::Null)},
        {"wave_assignment",Assignment(s.waveAssignment)},{"peak_selection",Selection(s.peakSelection)}};
    if(s.characterization)result["characterization"]=QJsonObject{
        {"purpose","single_pulse_characterization"},
        {"source_training_pattern_hash",Text(s.characterization->sourceTrainingPatternHash)},
        {"physical_input_count",Count(s.characterization->physicalInputCount)},
        {"selected_input_index",Count(s.characterization->selectedInputIndex)}};
    return result;
}
QJsonObject Observation(const ProbeObservation& o)
{
    QJsonArray stimuli,spikes,peaks,assignments;
    for(const auto& event:o.stimuli)stimuli.append(QJsonObject{{"id",Text(event.id)},{"time",Number(event.time)},
        {"relative_to_trial_start",Number(event.time-o.specification.begin)}});
    for(const auto& spike:o.spikes)spikes.append(QJsonObject{{"channel",Count(spike.channel)},
        {"time",Number(spike.time)},{"relative_to_trial_start",Number(spike.time-o.specification.begin)}});
    for(const auto& p:o.peaks)peaks.append(QJsonObject{{"input_id",Text(p.inputId)},{"stimulus_id",Text(p.stimulusId)},
        {"validity",PeakValidityName(p.validity)},{"amplitude",Maybe(p.amplitude)},{"raw_potential",Maybe(p.rawPotential)},
        {"time",Maybe(p.time)},{"plateau_end",Maybe(p.plateauEnd)},{"delay_from_stimulus",Maybe(p.delayFromStimulus)},
        {"relative_to_trial_start",p.time?Number(*p.time-o.specification.begin):QJsonValue(QJsonValue::Null)},
        {"candidate_count",Count(p.candidateCount)}});
    for(const auto& a:o.burstAssignments)
    {
        QJsonArray waves;
        for(const auto& w:a.waves)waves.append(QJsonObject{{"stimulus_id",Text(w.stimulusId)},
            {"amplitude",Number(w.amplitude)},{"raw_potential",Number(w.rawPotential)},
            {"time",Number(w.time)},{"plateau_end",Number(w.plateauEnd)},
            {"relative_to_trial_start",Number(w.time-o.specification.begin)}});
        assignments.append(QJsonObject{{"channel",Count(a.channel)},{"validity",PeakValidityName(a.validity)},{"waves",waves}});
    }
    return {{"schema_version",int(o.schemaVersion)},{"specification",Spec(o.specification,"absolute_model_time")},
        {"complete",o.complete},{"valid",o.valid},{"end_reason",Text(o.endReason)},{"reasons",Strings(o.reasons)},
        {"sample_count",Count(o.sampleCount)},{"stimuli",stimuli},{"peaks",peaks},{"burst_assignments",assignments},
        {"spikes",spikes},{"spike_count",o.specification.recordSpikes?Count(o.spikes.size()):QJsonValue(QJsonValue::Null)},
        {"first_stimulus_time",o.stimuli.empty()?QJsonValue(QJsonValue::Null):Number(o.stimuli.front().time)}};
}
QJsonObject Prepared(const PreparedVerificationProbe& p)
{return {{"routing",Routing(p.routing)},{"input_id",Text(p.inputId)},{"protocol_revision",Text(p.protocolRevision)},
 {"base_model_revision",Text(p.baseModelRevision)},{"effective_model_revision",Text(p.effectiveModelRevision)},
 {"model_epoch",Count(p.modelEpoch)},{"selected_input_index",p.selectedInputIndex?Count(*p.selectedInputIndex):QJsonValue(QJsonValue::Null)}};}
}
QJsonObject EncodeNativeProbeSpecification(const ProbeSpec& specification,bool relativeToTrialStart)
{return Spec(specification,relativeToTrialStart?"relative_to_trial_start":"absolute_model_time");}
QJsonObject EncodeNativeProbeEvidence(const NativeProbeResult& result)
{
    return {{"kind","timelearner_native_probe_evidence"},{"evidence_schema_version",2},
        {"native_status",NativeProbeStatusName(result.status)},{"current_base_revision",Text(result.currentBaseRevision)},
        {"reasons",Strings(result.reasons)},{"requires_fresh_verification",result.requiresFreshVerification},
        {"requested_specification",result.requestedSpecification?
            QJsonValue(Spec(*result.requestedSpecification,"relative_to_trial_start")):QJsonValue(QJsonValue::Null)},
        {"prepared",result.prepared?QJsonValue(Prepared(*result.prepared)):QJsonValue(QJsonValue::Null)},
        {"observation",result.observation?QJsonValue(Observation(*result.observation)):QJsonValue(QJsonValue::Null)}};
}
QJsonObject EncodeTrainingCertificate(const TrainingCertificate& certificate)
{
        QJsonArray residuals;
        for(const auto& r:certificate.Residuals())residuals.append(QJsonObject{{"input_id",Text(r.inputId)},
            {"trial_id",Text(r.trialId)},{"effective_model_revision",Text(r.effectiveModelRevision)},
            {"target_raw_potential",Number(r.targetRawPotential)},{"measured_raw_potential",Number(r.measuredRawPotential)},
            {"amplitude_residual",Number(r.amplitudeResidual)},{"amplitude_tolerance",Number(r.amplitudeTolerance)},
            {"timing_low",Number(r.timingResidualLow)},{"timing_high",Number(r.timingResidualHigh)},
            {"within_timing",r.withinTiming},{"within_amplitude",r.withinAmplitude}});
        return QJsonObject{{"schema_version",int(certificate.SchemaVersion())},
            {"peak_selection",Selection(certificate.Protocol().peakSelection)},
            {"exact",certificate.Exact()},{"model_revision",Text(certificate.ModelRevision())},
            {"model_epoch",Count(certificate.ModelEpoch())},{"protocol_revision",Text(certificate.ProtocolRevision())},
            {"reference_model_revision",Text(certificate.ReferenceModelRevision())},
            {"reference_model_epoch",Count(certificate.ReferenceModelEpoch())},{"residuals",residuals}};
}
QJsonObject EncodeVerificationProtocol(const VerificationProtocol& p)
{
    ValidateVerificationProtocol(p);QJsonArray inputs;
    for(const auto& input:p.inputs)inputs.append(QJsonObject{{"id",Text(input.id)},{"stimulus_id",Text(input.stimulusId)}});
    return {{"schema_version",int(p.schemaVersion)},{"routing",Routing(p.routing)},{"revision",Text(p.revision)},
        {"sample_id",Text(p.sampleId)},{"pattern_hash",Text(p.patternHash)},{"origin_stimulus_id",Text(p.originStimulusId)},
        {"inputs",inputs},{"stimulus_ids",Strings(p.stimulusIds)},{"maximum_sample_gap",Number(p.maximumSampleGap)},
        {"minimum_amplitude",Number(p.minimumAmplitude)},{"stimulus_profile",p.stimulusProfile?QJsonValue(Profile(*p.stimulusProfile)):QJsonValue(QJsonValue::Null)},
        {"wave_assignment",Assignment(p.waveAssignment)},{"peak_selection",Selection(p.peakSelection)}};
}
QJsonObject EncodeResistanceDomainAssessment(const ResistanceDomainAssessment& d)
{
    QJsonArray channels,geometry,controls;
    for(const auto& c:d.channels)channels.append(QJsonObject{{"path",Text(c.path)},
        {"variable_coefficient",Number(c.variableCoefficient)},{"fixed_conductance",Number(c.fixedConductance)},
        {"minimum_resistance",Number(c.minimumResistance)}});
    for(const auto& p:d.geometry)geometry.append(QJsonObject{
        {"length",p.length},{"synapses",p.synapses},{"resistance",Number(p.resistance)}});
    for(const auto& c:d.controls)
    {
        QJsonArray owners;for(auto owner:c.inputIndices)owners.append(Count(owner));
        controls.append(QJsonObject{{"synapse_path",Text(c.synapsePath)},{"input_indices",owners}});
    }
    QJsonValue model(QJsonValue::Null);
    if(!d.previewModelJson.empty())
    {
        const auto document=QJsonDocument::fromJson(QByteArray::fromStdString(d.previewModelJson));
        if(!document.isObject())throw std::invalid_argument("invalid_native_domain_preview_json");
        model=document.object();
    }
    QJsonObject result{{"kind","native_resistance_domain_evidence"},{"schema_version",2},
        {"status",ResistanceDomainStatusName(d.status)},{"reason",Text(d.reason)},{"input_index",Count(d.inputIndex)},
        {"base_model_revision",Text(d.baseModelRevision)},{"preview_model_revision",Text(d.previewModelRevision)},
        {"geometry",geometry},{"locked_by_shared_bank",d.lockedBySharedBank},{"model_may_have_changed",d.modelMayHaveChanged},
        {"minimum_resistance",d.minimumResistance==0.?QJsonValue(QJsonValue::Null):QJsonValue(Number(d.minimumResistance))},
        {"maximum_resistance",d.maximumResistance==0.?QJsonValue(QJsonValue::Null):QJsonValue(Number(d.maximumResistance))},
        {"channels",channels},{"preview_model",model},{"controls",controls},{"branch",d.branch},
        {"source_amplitude",Number(d.sourceAmplitude)},
        {"policy",QJsonObject{{"equations","explicit_npulsechannel_npulsesynapse_v1"},
            {"maximum_resistance_ratio",Number(d.maximumResistanceRatio)},
            {"configured_minimum_resistance",Number(d.configuredMinimumResistance)}}}};
    if(d.capacitance)
    {
        const auto encode=[](const TrainingCapacitance& c) {
            ValidateTrainingCapacitance(c);QJsonObject scales;
            for(const auto& [path,scale]:c.membraneScales)scales[Text(path)]=Number(scale);
            return QJsonObject{{"origin",Text(c.origin)},{"maximum_relative_change",Number(c.maximumRelativeChange)},
                {"membrane_scales",scales}};
        };
        result["schema_version"]=3;result["requested_capacitance"]=encode(*d.capacitance);
        result["effective_capacitance"]=QJsonValue(QJsonValue::Null);
        result["capacitance_state"]=QJsonValue(QJsonValue::Null);
        result["capacitance_storage_images"]=QJsonValue(QJsonValue::Null);
        if(d.effectiveCapacitance)
        {
            const auto state=QJsonDocument::fromJson(QByteArray::fromStdString(d.capacitanceStateJson));
            const auto images=QJsonDocument::fromJson(QByteArray::fromStdString(d.capacitanceImagesJson));
            if(!state.isObject() || !images.isArray() || images.array().isEmpty() ||
               DecodeNativeTrainingCapacitance(state.object())!=*d.effectiveCapacitance ||
               !TrainingCapacitanceReadbackMatches(d.capacitance,d.effectiveCapacitance) ||
               state.object()["applied_model_revision"]!=QJsonValue(Text(d.previewModelRevision)))
                throw std::invalid_argument("invalid_training_domain_capacitance_witness");
            result["effective_capacitance"]=encode(*d.effectiveCapacitance);
            result["capacitance_state"]=state.object();result["capacitance_storage_images"]=images.array();
        }
        else if(d.status==ResistanceDomainStatus::Available || !d.capacitanceStateJson.empty() || !d.capacitanceImagesJson.empty())
            throw std::invalid_argument("missing_training_domain_capacitance_witness");
    }
    else if(d.effectiveCapacitance || !d.capacitanceStateJson.empty() || !d.capacitanceImagesJson.empty())
        throw std::invalid_argument("unversioned_training_domain_capacitance_witness");
    return result;
}
QJsonObject EncodeNativeTrainingCapacitanceWitness(const TrainingCapacitanceWitness& witness)
try
{
    ValidateTrainingCapacitance(witness.parameters);
    const auto identity=QJsonDocument::fromJson(QByteArray::fromStdString(witness.modelIdentityJson));
    const auto state=QJsonDocument::fromJson(QByteArray::fromStdString(witness.nativeStateJson));
    const auto images=QJsonDocument::fromJson(QByteArray::fromStdString(witness.storageImagesJson));
    if(!identity.isObject() || !state.isObject() || !images.isArray() || images.array().isEmpty())
        throw std::invalid_argument("malformed_training_capacitance_witness");
    const auto model=identity.object(),owned=state.object();
    if(model["valid"]!=QJsonValue(true) || !model["diagnostics"].isArray() || !model["diagnostics"].toArray().isEmpty() ||
       !model["model"].isObject() || model["sha256"]!=QJsonValue(Text(witness.modelRevision)) ||
       model["sha256"]!=QJsonValue(NmsdkTools::CanonicalModelDataSha256(model["model"])) ||
       owned["applied_model_revision"]!=QJsonValue(Text(witness.modelRevision)) ||
       DecodeNativeTrainingCapacitance(owned)!=witness.parameters ||
       NmsdkTools::ModelImageDefinitionsFingerprint(images.array())!=owned["baseline"].toObject()["storage_images_sha256"].toString())
        throw std::invalid_argument("inconsistent_training_capacitance_witness");
    ValidateArchivedNativeCapacitanceState(owned,model,images.array());
    QJsonObject scales;
    for(const auto& [path,scale]:witness.parameters.membraneScales)scales[Text(path)]=Number(scale);
    return {{"kind","native_training_capacitance_witness"},{"schema_version",1},
        {"model_revision",Text(witness.modelRevision)},
        {"controller_parameters",QJsonObject{{"origin",Text(witness.parameters.origin)},
            {"maximum_relative_change",Number(witness.parameters.maximumRelativeChange)},{"membrane_scales",scales}}},
        {"model",model},{"native_state",owned},{"storage_images",images.array()}};
}
catch(const std::runtime_error& error)
{
    // Native validation helpers also serve live models and report runtime
    // failures. Here their only input is an archived data envelope: preserve
    // the exact diagnostic while exposing the codec's invalid-data contract.
    throw std::invalid_argument(error.what());
}
TrainingCapacitanceWitness DecodeNativeTrainingCapacitanceWitness(const QJsonObject& object)
{
    if(object["kind"]!=QJsonValue("native_training_capacitance_witness") || object["schema_version"]!=QJsonValue(1) ||
       !object["model_revision"].isString() || !object["model"].isObject() || !object["native_state"].isObject() || !object["storage_images"].isArray())
        throw std::invalid_argument("unsupported_native_training_capacitance_witness");
    TrainingCapacitanceWitness result;
    result.parameters=DecodeNativeTrainingCapacitance(object["native_state"].toObject());
    result.modelRevision=object["model_revision"].toString().toStdString();
    result.modelIdentityJson=QJsonDocument(object["model"].toObject()).toJson(QJsonDocument::Compact).toStdString();
    result.nativeStateJson=QJsonDocument(object["native_state"].toObject()).toJson(QJsonDocument::Compact).toStdString();
    result.storageImagesJson=QJsonDocument(object["storage_images"].toArray()).toJson(QJsonDocument::Compact).toStdString();
    if(EncodeNativeTrainingCapacitanceWitness(result)!=object)
        throw std::invalid_argument("noncanonical_native_training_capacitance_witness");
    return result;
}
std::string NativeTrainingCapacitanceWitnessId(const TrainingCapacitanceWitness& witness)
{
    return NmsdkTools::CanonicalModelDataSha256(EncodeNativeTrainingCapacitanceWitness(witness)).toStdString();
}
QJsonObject EncodeCandidateRecoveryEvidence(const CandidateRecoveryEvidence& e)
{
    if((e.validity!=PeakValidity::Missing && e.validity!=PeakValidity::Ambiguous) ||
       (e.statusAfter!=TrainingStatus::Searching && e.statusAfter!=TrainingStatus::InvalidObservation &&
        e.statusAfter!=TrainingStatus::SearchUnresolved))
        throw std::invalid_argument("invalid_candidate_recovery_enum");
    const char* disposition=nullptr;
    switch(e.disposition)
    {
    case CandidateRecoveryDisposition::CursorAdvanced:disposition="cursor_advanced";break;
    case CandidateRecoveryDisposition::InvalidBudgetStop:disposition="invalid_budget_stop";break;
    case CandidateRecoveryDisposition::RestoredBestStop:disposition="restored_best_stop";break;
    }
    if(!disposition)throw std::invalid_argument("unknown_candidate_recovery_disposition");
    return {{"action_serial",Count(e.actionSerial)},{"input_index",Count(e.inputIndex)},
        {"candidate_input_index",Count(e.candidateInputIndex)},{"sweep",Count(e.sweep)},
        {"model_epoch",Count(e.modelEpoch)},{"invalid_before",Count(e.invalidBefore)},
        {"invalid_after",Count(e.invalidAfter)},{"run_id",Text(e.runId)},{"trial_id",Text(e.trialId)},
        {"base_model_revision",Text(e.baseModelRevision)},{"effective_model_revision",Text(e.effectiveModelRevision)},
        {"phase",e.anchor?"anchor":"input"},{"validity",PeakValidityName(e.validity)},
        {"full_parameters",Parameters(e.fullParameters)},
        {"capacitance",e.capacitance?QJsonValue(CapacitanceParameters(*e.capacitance)):QJsonValue(QJsonValue::Null)},
        {"disposition",disposition},{"status_after",TrainingStatusName(e.statusAfter)},
        {"reason_after",Text(e.reasonAfter)}};
}
QJsonObject EncodeNativeTrainingState(const NativeTrainingService& service,const std::string& actualModelRevision,
                                      const std::vector<InputParameters>& actualParameters)
{
    const auto parameters=Parameters;
    const auto& cfg=service.Configuration();
    const auto& history=service.CapacitanceHistory();
    const auto encodeCapacitance=CapacitanceParameters;
    const auto trialWitness=[&](const std::string& trial,const std::string& revision,
        const std::optional<TrainingCapacitance>& capacitance=std::optional<TrainingCapacitance>{}) {
        if(!history)throw std::invalid_argument("missing_training_capacitance_history");
        const auto id=history->trialWitnessIds.find(trial);
        if(id==history->trialWitnessIds.end())throw std::invalid_argument("unwitnessed_training_summary_trial");
        const auto witness=history->witnesses.find(id->second);
        if(witness==history->witnesses.end() || witness->second.modelRevision!=revision ||
           (capacitance && witness->second.parameters!=*capacitance))
            throw std::invalid_argument("training_summary_capacitance_context_mismatch");
        return Text(id->second);
    };
    QJsonObject tolerances{{"amplitude_absolute",Number(cfg.tolerances.amplitudeAbsolute)},
        {"amplitude_relative",Number(cfg.tolerances.amplitudeRelative)},{"sync",Number(cfg.tolerances.sync)}};
    QJsonObject result{{"kind","native_training_state_summary"},{"schema_version",1},
        {"state",NativeTrainingStateName(service.State())},{"outcome",TrainingStatusName(service.Outcome())},
        {"reason",Text(service.Reason())},{"model_state_kind",
            service.State()==NativeTrainingState::Finished && !service.Certificate()?"diagnostic":"learned_solo"},
        {"actual_model_revision",Text(actualModelRevision)},{"model_epoch",Count(service.ModelEpoch())},
        {"actual_parameters",parameters(actualParameters)},{"tolerances",tolerances},
        {"reference_probes",Count(service.IssuedReferenceProbes())},{"contains_evaluation_verdict",false}};
    // A model boundary retains physical parameters and final proof, not the
    // search object. Make that distinction explicit instead of inventing an
    // accepted candidate or leaving a repeated-summary array undefined.
    result["search_history_retained"]=bool(service.Search());
    QJsonArray bounds;
    for(const auto& b:cfg.bounds)bounds.append(QJsonObject{{"minimum_length",b.minimumLength},{"maximum_length",b.maximumLength},
        {"minimum_synapses",b.minimumSynapses},{"maximum_synapses",b.maximumSynapses},
        {"minimum_resistance",Number(b.minimumResistance)},{"maximum_resistance",Number(b.maximumResistance)}});
    result["configuration"]=QJsonObject{{"run_id",Text(cfg.runId)},
        {"normalization",cfg.normalization==Normalization::SynapseCount?"synapse_count":"tip_resistance"},
        {"maximum_probes",Count(cfg.maximumProbes)},{"maximum_edits",Count(cfg.maximumEdits)},
        {"maximum_candidates_per_input",Count(cfg.maximumCandidatesPerInput)},{"maximum_sweeps",Count(cfg.maximumSweeps)},
        {"maximum_invalid_observations",Count(cfg.maximumInvalidObservations)},
        {"resistance_subdivision_depth",int(cfg.resistanceSubdivisionDepth)},{"bounds",bounds}};
    if(cfg.n0CandidateRepeats>1)
    {
        result["schema_version"]=2;
        auto configuration=result["configuration"].toObject();
        configuration["n0_candidate_repeats"]=Count(cfg.n0CandidateRepeats);result["configuration"]=configuration;
        result["candidate_repeat_groups"]=QJsonArray{};
    }
    if(history)
    {
        result["schema_version"]=3;
        auto configuration=result["configuration"].toObject();
        configuration["n0_candidate_repeats"]=Count(cfg.n0CandidateRepeats);result["configuration"]=configuration;
        result["candidate_repeat_groups"]=QJsonArray{};
        QJsonObject witnesses,trials;QJsonArray domainIds;
        for(const auto& [id,witness]:history->witnesses)
        {
            if(NativeTrainingCapacitanceWitnessId(witness)!=id)throw std::invalid_argument("invalid_training_summary_witness_id");
            witnesses[Text(id)]=EncodeNativeTrainingCapacitanceWitness(witness);
        }
        for(const auto& [trial,id]:history->trialWitnessIds)
        {
            if(!history->witnesses.count(id))throw std::invalid_argument("missing_training_summary_trial_witness");
            trials[Text(trial)]=Text(id);
        }
        for(const auto& id:history->domainBaseWitnessIds)domainIds.append(Text(id));
        const auto current=history->witnesses.find(history->currentWitnessId);
        if(current==history->witnesses.end() || !history->witnesses.count(history->captureWitnessId))
            throw std::invalid_argument("missing_training_summary_boundary_witness");
        result["capacitance_history"]=QJsonObject{{"capture_witness_id",Text(history->captureWitnessId)},
            {"current_witness_id",Text(history->currentWitnessId)},{"witnesses",witnesses},
            {"trial_witness_ids",trials},{"resistance_domain_witness_ids",domainIds},{"current_verified",history->currentVerified}};
        result["actual_capacitance_witness_id"]=history->currentVerified && current->second.modelRevision==actualModelRevision?
            QJsonValue(Text(history->currentWitnessId)):QJsonValue(QJsonValue::Null);
    }
    QJsonArray references;
    if(service.References())
    {
        result["reference_model_revision"]=Text(service.References()->InitialModelRevision());
        result["reference_model_epoch"]=Count(service.References()->InitialModelEpoch());
        for(const auto& r:service.References()->Inputs())
        {
            QJsonObject reference{
            {"input_id",Text(r.id)},{"run_id",Text(r.runId)},{"trial_id",Text(r.trialId)},
            {"effective_model_revision",Text(r.effectiveModelRevision)},{"raw_potential",Number(r.rawPotential)}};
            if(history)reference["capacitance_witness_id"]=trialWitness(r.trialId,service.References()->InitialModelRevision());
            references.append(reference);
        }
    }
    result["references"]=references;
    QJsonArray candidates,rejections,domains;
    if(const auto search=service.Search())
    {
        result["search_probes"]=Count(search->IssuedProbes());result["applied_edits"]=Count(search->AppliedEdits());
        result["accepted_parameters"]=parameters(search->Parameters());
        if(history)
        {
            if(!search->Capacitance())throw std::invalid_argument("missing_training_summary_accepted_capacitance");
            result["accepted_capacitance"]=encodeCapacitance(*search->Capacitance());
        }
        for(const auto& e:search->Evidence())
        {
            QJsonObject candidate{{"input_index",Count(e.inputIndex)},
            {"model_epoch",Count(e.modelEpoch)},{"parameters",parameters({e.parameters}).at(0)},
            {"full_parameters",parameters(e.fullParameters)},
            {"model_revision",Text(e.modelRevision)},{"trial_id",Text(e.trialId)},{"anchor_trial_id",Text(e.anchorTrialId)},
            {"timing_low",Number(e.timingResidualLow)},{"timing_high",Number(e.timingResidualHigh)},
            {"amplitude_residual",Number(e.amplitudeResidual)},{"within_timing",e.withinTiming},
            {"within_amplitude",e.withinAmplitude},{"parameter_change_cost",Number(e.parameterChangeCost)}};
            if(history)
            {
                if(!e.capacitance)throw std::invalid_argument("missing_training_candidate_capacitance");
                candidate["capacitance"]=encodeCapacitance(*e.capacitance);
                candidate["capacitance_witness_id"]=trialWitness(e.trialId,e.modelRevision,e.capacitance);
                trialWitness(e.anchorTrialId,e.modelRevision,e.capacitance);
            }
            else if(e.capacitance)throw std::invalid_argument("unversioned_training_candidate_capacitance");
            candidates.append(candidate);
        }
        for(const auto& r:search->Rejected())rejections.append(QJsonObject{{"trial_id",Text(r.trialId)},{"reason",Text(r.reason)}});
        for(const auto& d:search->Domains())domains.append(EncodeResistanceDomainAssessment(d));
        if(history && history->domainBaseWitnessIds.size()!=search->Domains().size())
            throw std::invalid_argument("missing_training_domain_base_witness");
        if(cfg.n0CandidateRepeats>1)
        {
            QJsonArray groups;
            for(const auto& group:search->CandidateRepeats())
            {
                QJsonArray trials,anchors,rawPotentials;
                for(const auto& id:group.trialIds)trials.append(Text(id));
                for(const auto& id:group.anchorTrialIds)anchors.append(Text(id));
                for(const auto value:group.rawPotentials)rawPotentials.append(Number(value));
                QJsonObject encoded{{"input_index",Count(group.inputIndex)},{"model_epoch",Count(group.modelEpoch)},
                    {"model_revision",Text(group.modelRevision)},{"full_parameters",parameters(group.fullParameters)},
                    {"trial_ids",trials},{"anchor_trial_ids",anchors},{"minimum_raw_potential",Number(group.minimumRawPotential)},
                    {"maximum_raw_potential",Number(group.maximumRawPotential)},{"within_timing",group.withinTiming},
                    {"within_amplitude",group.withinAmplitude},{"raw_potentials",rawPotentials},{"valid",true},
                    {"complete",group.complete},{"reason",Text(group.reason)}};
                if(history)
                {
                    if(!group.capacitance)throw std::invalid_argument("missing_training_repeat_capacitance");
                    encoded["capacitance"]=encodeCapacitance(*group.capacitance);
                    QJsonArray witnessIds,anchorIds;
                    for(const auto& trial:group.trialIds)witnessIds.append(trialWitness(trial,group.modelRevision,group.capacitance));
                    for(const auto& trial:group.anchorTrialIds)anchorIds.append(trialWitness(trial,group.modelRevision,group.capacitance));
                    encoded["capacitance_witness_ids"]=witnessIds;encoded["anchor_capacitance_witness_ids"]=anchorIds;
                }
                else if(group.capacitance)throw std::invalid_argument("unversioned_training_repeat_capacitance");
                groups.append(encoded);
            }
            result["candidate_repeat_groups"]=groups;
        }
    }
    result["candidates"]=candidates;result["rejections"]=rejections;
    result["resistance_domains"]=domains;
    result["certificate"]=QJsonValue(QJsonValue::Null);
    result["certificate_matches_actual_model"]=false;
    if(const auto& certificate=service.Certificate())
    {
        result["certificate"]=EncodeTrainingCertificate(*certificate);
        result["certificate_matches_actual_model"]=service.References() && certificate->Matches(actualModelRevision,
            *service.References(),cfg.tolerances,ModelStateKind::LearnedSolo,service.ModelEpoch());
    }
    if(cfg.capacitanceSearch)
    {
        const auto& policy=*cfg.capacitanceSearch;
        result["schema_version"]=4;
        auto configuration=result["configuration"].toObject();
        configuration["capacitance_search"]=EncodeCapacitanceSearchPolicy(policy);
        result["configuration"]=configuration;
    }
    if(cfg.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceUnassignedBurst)
    {
        const auto baseSchema=result.value("schema_version");
        result["base_schema_version"]=baseSchema;
        result["schema_version"]=5;
        result["invalid_measurement_recovery"]="advance_unassigned_burst";
        // Boundary import intentionally does not reconstruct controller search
        // history. An absent search has an unknown charge, never invented zero.
        result["invalid_observations"]=service.Search()?Count(service.Search()->InvalidObservations()):
            QJsonValue(QJsonValue::Null);
        QJsonArray events;
        if(const auto search=service.Search())for(const auto& e:search->Recoveries())
        {
            auto event=EncodeCandidateRecoveryEvidence(e);
            if(history)event["capacitance_witness_id"]=trialWitness(e.trialId,e.baseModelRevision,e.capacitance);
            events.append(event);
        }
        result["candidate_recovery_events"]=events;
    }
    return result;
}
}
