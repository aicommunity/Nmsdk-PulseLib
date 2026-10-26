#include "FinalVerification.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
using Detail::Finite;
using Detail::After;
struct Measurement { double raw,arrival,plateauEnd; };

void ProtocolValid(const VerificationProtocol& p)
{
    if((p.schemaVersion!=1 && p.schemaVersion!=2) ||
       (p.schemaVersion==1 && p.stimulusProfile) ||
       (p.schemaVersion==2 && (!p.stimulusProfile || !ValidStimulusProfile(*p.stimulusProfile))) ||
       p.revision.empty() || p.sampleId.empty() || p.patternHash.empty() ||
       p.originStimulusId.empty() || p.inputs.empty() || p.stimulusIds.size()!=p.inputs.size() ||
       !Finite(p.maximumSampleGap) || p.maximumSampleGap<=0. ||
       !Finite(p.minimumAmplitude) || p.minimumAmplitude<=0. ||
       (p.routing!=ProbeRouting::ClassicBurst && p.routing!=ProbeRouting::BranchSoloBurst))
        throw std::invalid_argument("invalid_verification_protocol");
    std::set<std::string> ids,stimuli,mapped;
    for(const auto& id:p.stimulusIds)
        if(id.empty() || !stimuli.insert(id).second)throw std::invalid_argument("invalid_protocol_stimulus_ids");
    if(!stimuli.count(p.originStimulusId))throw std::invalid_argument("missing_origin_stimulus");
    for(const auto& input:p.inputs)
        if(input.id.empty() || !ids.insert(input.id).second || !stimuli.count(input.stimulusId) ||
           !mapped.insert(input.stimulusId).second)throw std::invalid_argument("invalid_protocol_input_mapping");
}

Measurement CheckedMeasurement(const VerificationProtocol& p,std::size_t inputIndex,
                               const PreparedVerificationProbe& prepared,const ProbeObservation& o)
{
    const auto fail=[](const char* reason){throw std::invalid_argument(reason);};
    const auto& input=p.inputs.at(inputIndex);
    if(prepared.inputId!=input.id || prepared.routing!=p.routing ||
       prepared.protocolRevision!=p.revision || prepared.baseModelRevision.empty() ||
       prepared.effectiveModelRevision.empty())fail("prepared_probe_context_mismatch");
    if(o.schemaVersion!=p.schemaVersion || !o.complete || !o.valid || !o.reasons.empty() || o.endReason.empty())
        fail("observation_not_valid_complete");
    // Reuse the production spec validator; do not infer valid data from a flag.
    ProbeRecorder specificationValidator(o.specification);
    const auto& s=o.specification;
    if(s.schemaVersion!=p.schemaVersion || s.stimulusProfile!=p.stimulusProfile ||
       s.modelRevision!=prepared.effectiveModelRevision || s.sampleId!=p.sampleId ||
       s.patternHash!=p.patternHash || s.minimumAmplitude!=p.minimumAmplitude ||
       After(s.maximumSampleGap,p.maximumSampleGap))fail("observation_protocol_mismatch");
    if(o.sampleCount<3 || o.sampleCount>s.maximumSamples)fail("invalid_observation_sample_count");
    const double covered=(double(o.sampleCount)-1.)*s.maximumSampleGap;
    if(!Finite(covered) || After(s.end-s.begin,covered))fail("incomplete_declared_sampling");
    const std::set<std::string> expected(p.stimulusIds.begin(),p.stimulusIds.end());
    if(std::set<std::string>(s.expectedStimulusIds.begin(),s.expectedStimulusIds.end())!=expected ||
       o.stimuli.size()!=expected.size())fail("stimulus_set_mismatch");
    std::map<std::string,double> events;
    std::optional<double> previous;
    for(const auto& event:o.stimuli)
    {
        if(!expected.count(event.id) || !Finite(event.time) || After(s.begin,event.time) ||
           After(event.time,s.end) || (previous && event.time<*previous) ||
           !events.emplace(event.id,event.time).second)fail("invalid_actual_stimuli");
        previous=event.time;
    }
    if(p.routing==ProbeRouting::BranchSoloBurst && s.windows.size()!=1)
        fail("branch_probe_is_not_one_selected_input");
    if(o.peaks.size()!=s.windows.size())fail("peak_window_set_mismatch");
    std::map<std::string,const PeakObservation*> peaks;
    for(const auto& peak:o.peaks)
        if(!peaks.emplace(peak.inputId,&peak).second)fail("duplicate_observed_input");
    std::optional<Measurement> result;
    for(const auto& window:s.windows)
    {
        auto definition=std::find_if(p.inputs.begin(),p.inputs.end(),[&](const auto& x){return x.id==window.inputId;});
        if(definition==p.inputs.end() || definition->stimulusId!=window.stimulusId)
            fail("unexpected_peak_window");
        if(p.routing==ProbeRouting::BranchSoloBurst && window.inputId!=input.id)
            fail("branch_selected_input_mismatch");
        auto found=peaks.find(window.inputId);
        if(found==peaks.end())fail("missing_observed_input");
        const auto& peak=*found->second;
        if(peak.validity!=PeakValidity::Valid || peak.stimulusId!=window.stimulusId || peak.candidateCount!=1 ||
           !peak.rawPotential || !peak.amplitude || !peak.time || !peak.plateauEnd || !peak.delayFromStimulus ||
           !Finite(*peak.rawPotential) || !Finite(*peak.amplitude) || !Finite(*peak.time) ||
           !Finite(*peak.plateauEnd) || !Finite(*peak.delayFromStimulus))fail("invalid_measured_peak");
        const double deviation=*peak.rawPotential-s.baseline.at(window.channel);
        const double delay=*peak.time-events.at(window.stimulusId);
        if(!Finite(deviation) || deviation!=*peak.amplitude || deviation<p.minimumAmplitude ||
           !Finite(delay) || delay!=*peak.delayFromStimulus || After(events.at(window.stimulusId),*peak.time) ||
           *peak.plateauEnd<*peak.time || After(window.begin,*peak.time) || After(*peak.plateauEnd,window.end))
            fail("inconsistent_measured_peak");
        const double arrival=*peak.time-events.at(p.originStimulusId);
        const double plateau=*peak.plateauEnd-events.at(p.originStimulusId);
        if(!Finite(arrival) || !Finite(plateau))fail("nonfinite_measured_arrival");
        if(window.inputId==input.id)result=Measurement{*peak.rawPotential,arrival,plateau};
    }
    if(!result)fail("requested_input_not_measured");
    return *result;
}
}

CapturedReferences CapturedReferences::Capture(VerificationProtocol p,const std::vector<ReferenceEvidence>& evidence)
{
    ProtocolValid(p);
    if(evidence.size()!=p.inputs.size())throw std::invalid_argument("incomplete_reference_set");
    CapturedReferences result;result.protocol=std::move(p);
    std::set<std::pair<std::string,std::string>> branchTrials;
    for(std::size_t i=0;i<result.protocol.inputs.size();++i)
    {
        const auto& input=result.protocol.inputs[i];
        auto found=std::find_if(evidence.begin(),evidence.end(),[&](const auto& e){return e.prepared.inputId==input.id;});
        if(found==evidence.end())throw std::invalid_argument("missing_reference_input");
        if(std::count_if(evidence.begin(),evidence.end(),[&](const auto& e){return e.prepared.inputId==input.id;})!=1)
            throw std::invalid_argument("duplicate_reference_input");
        const auto measured=CheckedMeasurement(result.protocol,i,found->prepared,found->observation);
        if(i==0){result.initialModelRevision=found->prepared.baseModelRevision;result.initialModelEpoch=found->prepared.modelEpoch;}
        if(found->prepared.baseModelRevision!=result.initialModelRevision || found->prepared.modelEpoch!=result.initialModelEpoch)
            throw std::invalid_argument("mixed_reference_model_revisions");
        const auto& s=found->observation.specification;
        if(result.protocol.routing==ProbeRouting::BranchSoloBurst && !branchTrials.emplace(s.runId,s.trialId).second)
            throw std::invalid_argument("branch_reference_trial_reused");
        result.inputs.push_back({input.id,s.runId,s.trialId,found->prepared.effectiveModelRevision,measured.raw});
    }
    return result;
}

bool TrainingCertificate::Matches(const std::string& currentModel,const CapturedReferences& currentReferences,
                                  const VerificationTolerances& currentTolerances,ModelStateKind kind,std::size_t currentEpoch) const
{
    return kind==ModelStateKind::LearnedSolo && currentModel==modelRevision &&
        currentEpoch==modelEpoch && currentTolerances==tolerances &&
        currentReferences.Protocol()==protocol && currentReferences.Inputs()==referenceInputs &&
        currentReferences.InitialModelRevision()==referenceModelRevision &&
        currentReferences.InitialModelEpoch()==referenceModelEpoch;
}

FinalVerificationSession::FinalVerificationSession(CapturedReferences refs,VerificationConfig cfg)
    :references(std::move(refs)),config(std::move(cfg)),epoch(config.modelEpoch)
{
    if(config.runId.empty() || config.currentModelRevision.empty() || !config.maximumProbes ||
       !Detail::Finite(config.tolerances.amplitudeAbsolute) || config.tolerances.amplitudeAbsolute<0. ||
       !Detail::Finite(config.tolerances.amplitudeRelative) || config.tolerances.amplitudeRelative<0. ||
       !Detail::Finite(config.tolerances.sync) || config.tolerances.sync<0.)
        throw std::invalid_argument("invalid_verification_configuration");
    ProtocolValid(references.Protocol());
    const auto n=references.Inputs().size();
    if(!n || n!=references.Protocol().inputs.size())throw std::invalid_argument("incomplete_captured_references");
    accepted.resize(n);
    for(std::size_t i=0;i<n;++i)
    {
        const double tolerance=config.tolerances.amplitudeAbsolute+
            config.tolerances.amplitudeRelative*std::fabs(references.Inputs()[i].rawPotential);
        if(!Detail::Finite(tolerance))throw std::invalid_argument("nonfinite_amplitude_tolerance");
        order.push_back(references.Protocol().routing==ProbeRouting::ClassicBurst?i:n-1-i);
    }
}

std::optional<VerificationRequest> FinalVerificationSession::NextRequest()
{
    if(status!=VerificationStatus::Collecting)return {};
    if(armed)return {};
    if(pending)return pending;
    if(budgetStopped || issued>=config.maximumProbes){ExhaustBudget();return {};}
    const auto& p=references.Protocol();
    const auto trial=config.runId+"/verify/"+std::to_string(epoch)+"/"+std::to_string(++serial);
    pending=VerificationRequest{config.runId,trial,p.inputs[order.at(position)].id,p.sampleId,p.patternHash,
        p.revision,config.currentModelRevision,p.routing,epoch};
    return pending;
}

void FinalVerificationSession::Reject(const std::string& reason)
{
    rejected.push_back({pending?pending->trialId:std::string{},reason});
    const bool retry=invalidCount<config.maximumInvalidObservations;
    if(invalidCount<std::numeric_limits<std::size_t>::max())++invalidCount;
    pending.reset();armed.reset();certificate.reset();
    if(!retry)status=VerificationStatus::InvalidObservation;
    else if(issued>=config.maximumProbes)ExhaustBudget();
}

void FinalVerificationSession::Arm(const PreparedVerificationProbe& prepared)
{
    if(status!=VerificationStatus::Collecting)return;
    if(!pending || armed)throw std::logic_error("verification_probe_not_requested_or_already_armed");
    if(issued>=config.maximumProbes){ExhaustBudget();return;}
    ++issued;
    if(prepared.inputId!=pending->inputId || prepared.routing!=pending->routing ||
       prepared.protocolRevision!=pending->protocolRevision || prepared.baseModelRevision!=config.currentModelRevision ||
       prepared.modelEpoch!=epoch || prepared.effectiveModelRevision.empty())
    {Reject("prepared_probe_context_mismatch");return;}
    armed=prepared;
}

void FinalVerificationSession::Consume(const ProbeObservation& observation,const std::string& currentBaseRevision)
{
    if(status!=VerificationStatus::Collecting)return;
    if(currentBaseRevision.empty())
    {
        const auto trial=pending?pending->trialId:std::string{};
        Invalidate(config.currentModelRevision);rejected.push_back({trial,"missing_current_model_revision"});
        status=VerificationStatus::InvalidObservation;return;
    }
    if(currentBaseRevision!=config.currentModelRevision)
    {Invalidate(currentBaseRevision);return;}
    if(std::find(observation.reasons.begin(),observation.reasons.end(),"model_revision_changed")!=observation.reasons.end())
    {Invalidate(currentBaseRevision);status=VerificationStatus::InvalidObservation;return;}
    if(!pending || !armed){Reject("unrequested_observation");return;}
    if(observation.specification.runId!=pending->runId || observation.specification.trialId!=pending->trialId ||
       !seenTrials.insert(observation.specification.trialId).second)
    {Reject("unexpected_or_replayed_trial");return;}
    try
    {
        const auto index=order.at(position);
        const auto measured=CheckedMeasurement(references.Protocol(),index,*armed,observation);
        accepted[index]=Accepted{pending->trialId,armed->effectiveModelRevision,measured.raw,measured.arrival,measured.plateauEnd};
    }
    catch(const std::invalid_argument& error){Reject(error.what());return;}
    pending.reset();armed.reset();++position;
    if(position==order.size())Finish();
}

void FinalVerificationSession::Finish()
{
    TrainingCertificate result;result.exact=true;result.modelRevision=config.currentModelRevision;
    result.protocol=references.Protocol();result.tolerances=config.tolerances;
    result.referenceInputs=references.Inputs();result.referenceModelRevision=references.InitialModelRevision();
    result.modelEpoch=epoch;result.referenceModelEpoch=references.InitialModelEpoch();
    if(accepted.empty() || std::any_of(accepted.begin(),accepted.end(),[](const auto& input){return !input;}))
    {status=VerificationStatus::InvalidObservation;return;}
    const auto& anchor=*accepted.back();
    for(std::size_t i=0;i<accepted.size();++i)
    {
        const auto& measured=*accepted[i];const auto& target=references.Inputs()[i];
        InputVerificationResidual residual;residual.inputId=target.id;residual.trialId=measured.trialId;
        residual.effectiveModelRevision=measured.effectiveRevision;residual.targetRawPotential=target.rawPotential;
        residual.measuredRawPotential=measured.raw;residual.amplitudeResidual=measured.raw-target.rawPotential;
        residual.amplitudeTolerance=config.tolerances.amplitudeAbsolute+config.tolerances.amplitudeRelative*std::fabs(target.rawPotential);
        if(i+1!=accepted.size())
        {
            residual.timingResidualLow=measured.arrival-anchor.plateauEnd;
            residual.timingResidualHigh=measured.plateauEnd-anchor.arrival;
        }
        if(!Detail::Finite(residual.amplitudeResidual) || !Detail::Finite(residual.timingResidualLow) ||
           !Detail::Finite(residual.timingResidualHigh))
        {status=VerificationStatus::InvalidObservation;rejected.push_back({measured.trialId,"nonfinite_residual"});return;}
        residual.withinAmplitude=std::fabs(residual.amplitudeResidual)<=residual.amplitudeTolerance;
        residual.withinTiming=std::max(std::fabs(residual.timingResidualLow),std::fabs(residual.timingResidualHigh))<=config.tolerances.sync;
        result.exact=result.exact && residual.withinAmplitude && residual.withinTiming;
        result.residuals.push_back(std::move(residual));
    }
    status=result.exact?VerificationStatus::Exact:VerificationStatus::OutsideTolerance;
    certificate=std::move(result);
}

void FinalVerificationSession::Invalidate(const std::string& newBaseRevision)
{
    if(newBaseRevision.empty())throw std::invalid_argument("empty_model_revision");
    if(armed && pending)rejected.push_back({pending->trialId,"model_revision_invalidated"});
    for(const auto& input:accepted)
        if(input)rejected.push_back({input->trialId,"model_revision_invalidated"});
    config.currentModelRevision=newBaseRevision;certificate.reset();pending.reset();armed.reset();
    for(auto& input:accepted)input.reset();position=0;
    if(status==VerificationStatus::ExternalStop)return;
    if(epoch==std::numeric_limits<std::size_t>::max()){ExhaustBudget();return;}
    ++epoch;
    status=VerificationStatus::Collecting;
    if(budgetStopped || issued>=config.maximumProbes)ExhaustBudget();
}

void FinalVerificationSession::ExternalStop()
{
    if(status!=VerificationStatus::Collecting)return;
    pending.reset();armed.reset();certificate.reset();status=VerificationStatus::ExternalStop;
}
void FinalVerificationSession::ExhaustBudget()
{
    budgetStopped=true;pending.reset();armed.reset();certificate.reset();status=VerificationStatus::BudgetExhausted;
}
std::size_t FinalVerificationSession::AcceptedInputs() const
{return std::count_if(accepted.begin(),accepted.end(),[](const auto& value){return bool(value);});}

const char* VerificationStatusName(VerificationStatus status)
{
    switch(status){
    case VerificationStatus::Collecting:return "collecting";
    case VerificationStatus::Exact:return "exact";
    case VerificationStatus::OutsideTolerance:return "outside_tolerance";
    case VerificationStatus::InvalidObservation:return "invalid_observation";
    case VerificationStatus::BudgetExhausted:return "budget_exhausted";
    case VerificationStatus::ExternalStop:return "external_stop";
    }
    return "unknown";
}
}
