#include "BoundedTraining.h"
#include "NumericChecks.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <tuple>

namespace NMSDK::TimeLearner {
namespace {
bool Finite(double value) { return Detail::Finite(value); }
template<class T> void UniquePush(std::vector<T>& values,T value)
{ if(std::find(values.begin(),values.end(),value)==values.end())values.push_back(value); }
double AmplitudeTolerance(const BoundedTrainingConfig& config,const CapturedInputReference& reference)
{ return config.tolerances.amplitudeAbsolute+config.tolerances.amplitudeRelative*std::fabs(reference.rawPotential); }
auto Rank(const CandidateEvidence& e,const BoundedTrainingConfig& cfg,const CapturedInputReference& reference,
          const InputParameters& initial)
{
    const double time=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh));
    const double amplitude=std::fabs(e.amplitudeResidual);
    // Zero tolerances are exact requirements; no implicit timestep/percent floor.
    const auto normalized=[](double error,double tolerance) {
        return tolerance>0.?error/tolerance:(error==0.?0.:std::numeric_limits<double>::infinity());
    };
    const double residual=normalized(time,cfg.tolerances.sync)+
        normalized(amplitude,AmplitudeTolerance(cfg,reference));
    const double cost=std::fabs(double(e.parameters.length)-initial.length)+
        std::fabs(double(e.parameters.synapses)-initial.synapses)+
        std::fabs(std::log(e.parameters.resistance)-std::log(initial.resistance));
    return std::tuple{!e.withinTiming,!e.withinAmplitude,residual,cost};
}
}

BoundedTrainingSession::BoundedTrainingSession(CapturedReferences refs,BoundedTrainingConfig cfg,
                                             std::vector<InputParameters> initial)
    :references(std::move(refs)),config(std::move(cfg)),current(std::move(initial)),
     epoch(config.initialModelEpoch),revision(config.initialModelRevision)
{
    if(config.runId.empty() || revision.empty() || !config.maximumProbes || !config.maximumEdits ||
        !config.maximumCandidatesPerInput || !config.maximumSweeps || config.resistanceSubdivisionDepth>20 ||
        (config.normalization!=Normalization::SynapseCount && config.normalization!=Normalization::TipResistance) ||
        references.Protocol().schemaVersion!=3 || config.bounds.size()!=references.Inputs().size() ||
        current.size()!=references.Inputs().size() || current.empty())
        throw std::invalid_argument("invalid_bounded_training_configuration");
    // Use the exact same protocol/tolerance checks as the final gate.
    FinalVerificationSession validate(references,{config.runId,revision,config.maximumProbes,
        config.maximumInvalidObservations,epoch,config.tolerances});
    for(std::size_t i=0;i<config.bounds.size();++i)
    {
        const auto& b=config.bounds[i];
        if(b.minimumLength<1 || b.maximumLength<b.minimumLength || b.minimumSynapses<1 ||
            b.maximumSynapses>128 || b.maximumSynapses<b.minimumSynapses ||
            !Finite(b.minimumResistance) || !Finite(b.maximumResistance) || b.minimumResistance<=0. ||
            b.maximumResistance<b.minimumResistance || !Finite(AmplitudeTolerance(config,references.Inputs()[i])))
            throw std::invalid_argument("invalid_per_input_search_bounds");
    }
    if(!Legal(current))throw std::invalid_argument("initial_parameters_outside_search_domain");
    // Last input is always fixed. Classic round robin / Branch reverse order.
    for(std::size_t i=0;i+1<current.size();++i)
        order.push_back(references.Protocol().routing==ProbeRouting::ClassicBurst?i:current.size()-2-i);
    if(order.empty())BeginVerification();else BeginInput();
}

bool BoundedTrainingSession::Legal(const std::vector<InputParameters>& parameters) const
{
    if(parameters.size()!=config.bounds.size())return false;
    for(std::size_t i=0;i<parameters.size();++i)
    {
        const auto& p=parameters[i];const auto& b=config.bounds[i];
        if(p.length<b.minimumLength || p.length>b.maximumLength || p.synapses<b.minimumSynapses ||
            p.synapses>b.maximumSynapses || !Finite(p.resistance) || p.resistance<b.minimumResistance ||
            p.resistance>b.maximumResistance || (config.normalization==Normalization::TipResistance && p.synapses!=1))
            return false;
        if(references.Protocol().routing==ProbeRouting::BranchSoloBurst && i && parameters[i-1].length<p.length)
            return false;
        // Native Branch has one excitation bank per attachment, not per label.
        // A nominally local edit must not silently change the fixed anchor.
        if(references.Protocol().routing==ProbeRouting::BranchSoloBurst && i && parameters[i-1].length==p.length &&
            (parameters[i-1].synapses!=p.synapses || parameters[i-1].resistance!=p.resistance))
            return false;
    }
    return true;
}

void BoundedTrainingSession::BeginInput()
{
    const auto index=order.at(position);const auto& b=config.bounds[index];const auto& p=current[index];
    background=current;lengths.clear();counts.clear();resistances.clear();best.reset();anchor.reset();anchorTrial.clear();
    lengthIndex=normalizerIndex=candidates=0;restoringBest=false;phase=Phase::Candidate;
    // Nearest integers first. Enumeration is bounded by the declared candidate
    // budget, even for a very large physical length range.
    UniquePush(lengths,p.length);
    for(long long distance=1;lengths.size()<config.maximumCandidatesPerInput;++distance)
    {
        const long long up=static_cast<long long>(p.length)+distance,down=static_cast<long long>(p.length)-distance;
        if(up>b.maximumLength && down<b.minimumLength)break;
        if(up<=b.maximumLength)UniquePush(lengths,int(up));
        if(down>=b.minimumLength && lengths.size()<config.maximumCandidatesPerInput)UniquePush(lengths,int(down));
    }
    if(config.normalization==Normalization::SynapseCount)
    {
        UniquePush(counts,p.synapses);
        for(int distance=1;counts.size()<std::size_t(b.maximumSynapses-b.minimumSynapses+1);++distance)
        {
            if(p.synapses+distance<=b.maximumSynapses)UniquePush(counts,p.synapses+distance);
            if(p.synapses-distance>=b.minimumSynapses)UniquePush(counts,p.synapses-distance);
        }
    }
    else
    {
        UniquePush(resistances,p.resistance);
        UniquePush(resistances,b.minimumResistance);UniquePush(resistances,b.maximumResistance);
        for(unsigned level=1;level<=config.resistanceSubdivisionDepth && resistances.size()<config.maximumCandidatesPerInput;++level)
        {
            const std::size_t denominator=std::size_t(1)<<level;
            for(std::size_t numerator=1;numerator<denominator && resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
            {
                const double fraction=double(numerator)/double(denominator);
                const double value=std::exp(std::log(b.minimumResistance)+fraction*(std::log(b.maximumResistance)-std::log(b.minimumResistance)));
                if(Finite(value) && value>=b.minimumResistance && value<=b.maximumResistance)UniquePush(resistances,value);
            }
        }
    }
}

bool BoundedTrainingSession::NextCandidate(std::vector<InputParameters>& parameters)
{
    const auto index=order.at(position);
    const std::size_t normalizers=config.normalization==Normalization::SynapseCount?counts.size():resistances.size();
    // First evaluate the current normalizer at each length; then the next
    // normalizer at each length. No monotonicity or bracket is inferred.
    while(normalizerIndex<normalizers)
    {
        if(lengthIndex==lengths.size()){lengthIndex=0;++normalizerIndex;continue;}
        parameters=background;parameters[index].length=lengths[lengthIndex++];
        if(config.normalization==Normalization::SynapseCount)parameters[index].synapses=counts[normalizerIndex];
        else parameters[index].resistance=resistances[normalizerIndex];
        if(Legal(parameters))return true;
    }
    return false;
}

void BoundedTrainingSession::AdvanceInput()
{
    anchor.reset();anchorTrial.clear();++position;
    if(position==order.size())BeginVerification();else BeginInput();
}

void BoundedTrainingSession::BeginVerification()
{
    if(probes>=config.maximumProbes){Stop(TrainingStatus::BudgetExhausted);return;}
    phase=Phase::Verify;status=TrainingStatus::Verifying;
    verification=std::make_unique<FinalVerificationSession>(references,VerificationConfig{
        config.runId+"/final/"+std::to_string(sweep),revision,config.maximumProbes-probes,
        config.maximumInvalidObservations,epoch,config.tolerances});
}

void BoundedTrainingSession::FinishVerification()
{
    switch(verification->Status())
    {
    case VerificationStatus::Collecting:return;
    case VerificationStatus::Exact:Stop(TrainingStatus::Exact);return;
    case VerificationStatus::BudgetExhausted:Stop(TrainingStatus::BudgetExhausted);return;
    case VerificationStatus::ExternalStop:Stop(TrainingStatus::ExternalStop);return;
    case VerificationStatus::InvalidObservation:Stop(TrainingStatus::InvalidObservation);return;
    case VerificationStatus::OutsideTolerance:
        if(++sweep>=config.maximumSweeps || order.empty()){Stop(TrainingStatus::SearchUnresolved);return;}
        verification.reset();status=TrainingStatus::Searching;position=0;BeginInput();return;
    }
}

std::optional<TrainingAction> BoundedTrainingSession::NextAction()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return {};
    if(armed)return {};
    if(pending)return pending;
    if(phase==Phase::Candidate)
    {
        std::vector<InputParameters> candidate;
        if(candidates>=config.maximumCandidatesPerInput || !NextCandidate(candidate))
        {
            if(!best){Stop(TrainingStatus::SearchUnresolved);return {};}
            candidate=background;candidate[order.at(position)]=best->parameters;restoringBest=true;
        }
        else ++candidates;
        phase=Phase::Anchor;anchor.reset();anchorTrial.clear();
        if(candidate!=current)
        {
            if(edits>=config.maximumEdits || epoch==std::numeric_limits<std::size_t>::max())
            {Stop(TrainingStatus::BudgetExhausted);return {};}
            pending=TrainingAction{TrainingActionKind::Edit,++serial,order.at(position),epoch,
                config.runId,{},revision,std::move(candidate),false};return pending;
        }
    }
    if(probes>=config.maximumProbes){Stop(TrainingStatus::BudgetExhausted);return {};}
    if(phase==Phase::Verify)
    {
        auto request=verification->NextRequest();
        if(!request){FinishVerification();return NextAction();}
        pending=TrainingAction{TrainingActionKind::Probe,++serial,request->inputIndex,epoch,
            request->runId,request->trialId,revision,current,true};
    }
    else
    {
        const auto index=phase==Phase::Anchor?current.size()-1:order.at(position);
        const auto actionSerial=++serial;
        pending=TrainingAction{TrainingActionKind::Probe,actionSerial,index,epoch,config.runId,
            config.runId+"/search/"+std::to_string(actionSerial),revision,current,false};
    }
    return pending;
}

void BoundedTrainingSession::Applied(std::size_t actionSerial,const std::vector<InputParameters>& effective,
                                   const std::string& actualModelRevision)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::Edit || pending->serial!=actionSerial)
        throw std::logic_error("unrequested_training_edit");
    if(effective!=pending->parameters || !Legal(effective) || actualModelRevision.empty() || actualModelRevision==revision)
    {rejected.push_back({{},"effective_edit_readback_mismatch"});Stop(TrainingStatus::InvalidModel);return;}
    current=effective;revision=actualModelRevision;++epoch;++edits;pending.reset();
    // Every edit invalidates all current measurements, including Branch shared
    // cable dependencies. Earlier evidence remains historical, never reused.
    anchor.reset();anchorTrial.clear();
}

void BoundedTrainingSession::Reject(const std::string& reason)
{
    rejected.push_back({pending?pending->trialId:std::string{},reason});pending.reset();armed.reset();
    if(invalid++>=config.maximumInvalidObservations)Stop(TrainingStatus::InvalidObservation);
}

void BoundedTrainingSession::DeclineEdit(std::size_t actionSerial,const std::string& reason,const std::string& unchangedModelRevision)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::Edit || pending->serial!=actionSerial || reason.empty())
        throw std::logic_error("unrequested_training_edit_rejection");
    rejected.push_back({{},"edit_rejected:"+reason});
    if(unchangedModelRevision!=revision || unchangedModelRevision.empty() || restoringBest)
    {Stop(TrainingStatus::InvalidModel);return;}
    pending.reset();armed.reset();anchor.reset();anchorTrial.clear();phase=Phase::Candidate;
}

void BoundedTrainingSession::Arm(std::size_t actionSerial,const PreparedVerificationProbe& prepared)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(!pending || pending->kind!=TrainingActionKind::Probe || pending->serial!=actionSerial || armed)
        throw std::logic_error("unrequested_training_probe");
    ++probes;
    if(phase==Phase::Verify)
    {
        verification->Arm(prepared);
        if(verification->Status()!=VerificationStatus::Collecting){FinishVerification();return;}
        if(!verification->WaitingForObservation()){pending.reset();return;}
    }
    const auto& protocol=references.Protocol();
    if(prepared.inputId!=protocol.inputs.at(pending->inputIndex).id || prepared.routing!=protocol.routing ||
        prepared.protocolRevision!=protocol.revision || prepared.baseModelRevision!=revision ||
        prepared.effectiveModelRevision.empty() || prepared.modelEpoch!=epoch ||
        (protocol.routing==ProbeRouting::BranchSoloBurst?prepared.selectedInputIndex!=pending->inputIndex:bool(prepared.selectedInputIndex)))
    {Reject("prepared_training_context_mismatch");return;}
    armed=prepared;
}

void BoundedTrainingSession::Consume(const ProbeObservation& observation,const std::string& actualModelRevision)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(actualModelRevision!=revision || actualModelRevision.empty())
    {rejected.push_back({pending?pending->trialId:std::string{},"unexpected_training_model_revision"});Stop(TrainingStatus::InvalidModel);return;}
    if(!pending || !armed){Reject("unrequested_training_observation");return;}
    if(phase==Phase::Verify)
    {
        verification->Consume(observation,actualModelRevision);pending.reset();armed.reset();FinishVerification();return;
    }
    if(observation.specification.runId!=pending->runId || observation.specification.trialId!=pending->trialId ||
        !seenTrials.insert(observation.specification.trialId).second)
    {Reject("unexpected_or_replayed_training_trial");return;}
    ValidatedProbeMeasurement measurement;
    try {measurement=ValidateProbeMeasurement(references.Protocol(),pending->inputIndex,*armed,observation);}
    catch(const std::invalid_argument& error){Reject(error.what());return;}
    const auto trial=pending->trialId;pending.reset();armed.reset();
    if(phase==Phase::Anchor){anchor=measurement;anchorTrial=trial;phase=Phase::Input;return;}
    if(!anchor)throw std::logic_error("missing_same_revision_anchor");
    const auto index=order.at(position);const auto& reference=references.Inputs()[index];
    CandidateEvidence e;e.inputIndex=index;e.modelEpoch=epoch;e.parameters=current[index];
    e.modelRevision=revision;e.trialId=trial;e.anchorTrialId=anchorTrial;
    e.timingResidualLow=measurement.arrival-anchor->plateauEnd;
    e.timingResidualHigh=measurement.plateauEnd-anchor->arrival;
    e.amplitudeResidual=measurement.rawPotential-reference.rawPotential;
    if(!Finite(e.timingResidualLow) || !Finite(e.timingResidualHigh) || !Finite(e.amplitudeResidual))
    {Reject("nonfinite_training_residual");return;}
    e.withinTiming=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh))<=config.tolerances.sync;
    e.withinAmplitude=std::fabs(e.amplitudeResidual)<=AmplitudeTolerance(config,reference);
    evidence.push_back(e);
    if(!best || Rank(e,config,reference,background[index])<Rank(*best,config,reference,background[index]))best=e;
    if(restoringBest || (e.withinTiming && e.withinAmplitude))AdvanceInput();
    else {phase=Phase::Candidate;anchor.reset();anchorTrial.clear();}
}

void BoundedTrainingSession::Stop(TrainingStatus terminal)
{status=terminal;pending.reset();armed.reset();anchor.reset();}

void BoundedTrainingSession::ExternalStop()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(verification)verification->ExternalStop();Stop(TrainingStatus::ExternalStop);
}

void BoundedTrainingSession::ExhaustBudget()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(verification)verification->ExhaustBudget();Stop(TrainingStatus::BudgetExhausted);
}

const std::optional<TrainingCertificate>& BoundedTrainingSession::Certificate() const
{
    static const std::optional<TrainingCertificate> absent;
    return verification?verification->Certificate():absent;
}

const char* TrainingStatusName(TrainingStatus status)
{
    switch(status) {
    case TrainingStatus::Searching:return "searching";
    case TrainingStatus::Verifying:return "verifying";
    case TrainingStatus::Exact:return "exact";
    case TrainingStatus::SearchUnresolved:return "search_unresolved";
    case TrainingStatus::BudgetExhausted:return "budget_exhausted";
    case TrainingStatus::InvalidModel:return "invalid_model";
    case TrainingStatus::InvalidObservation:return "invalid_observation";
    case TrainingStatus::ExternalStop:return "external_stop";
    }
    return "unknown";
}
}
