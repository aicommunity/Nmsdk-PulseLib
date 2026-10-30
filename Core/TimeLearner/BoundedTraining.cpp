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
auto Rank(const CandidateEvidence& e,const BoundedTrainingConfig& cfg,const CapturedInputReference& reference)
{
    const double time=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh));
    const double amplitude=std::fabs(e.amplitudeResidual);
    // Zero tolerances are exact requirements; no implicit timestep/percent floor.
    const auto normalized=[](double error,double tolerance) {
        return tolerance>0.?error/tolerance:(error==0.?0.:std::numeric_limits<double>::infinity());
    };
    const double residual=normalized(time,cfg.tolerances.sync)+
        normalized(amplitude,AmplitudeTolerance(cfg,reference));
    return std::tuple{!e.withinTiming,!e.withinAmplitude,residual,e.parameterChangeCost};
}
bool ParametersLegal(const VerificationProtocol& protocol,const BoundedTrainingConfig& config,
                     const std::vector<InputParameters>& parameters)
{
    if(parameters.size()!=config.bounds.size())return false;
    for(std::size_t i=0;i<parameters.size();++i)
    {
        const auto& p=parameters[i];const auto& b=config.bounds[i];
        if(p.length<b.minimumLength || p.length>b.maximumLength || p.synapses<b.minimumSynapses ||
            p.synapses>b.maximumSynapses || !Finite(p.resistance) || p.resistance<b.minimumResistance ||
            p.resistance>b.maximumResistance || (config.normalization==Normalization::TipResistance && p.synapses!=1))return false;
        if(protocol.routing==ProbeRouting::BranchSoloBurst && i && parameters[i-1].length<p.length)return false;
        if(protocol.routing==ProbeRouting::BranchSoloBurst && i && parameters[i-1].length==p.length &&
            (parameters[i-1].synapses!=p.synapses || parameters[i-1].resistance!=p.resistance))return false;
    }
    return true;
}
}

void ValidateBoundedTrainingInputs(const VerificationProtocol& protocol,const BoundedTrainingConfig& config,
                                   const std::vector<InputParameters>& initial)
{
    ValidateVerificationProtocol(protocol);
    if(config.runId.empty() || config.initialModelRevision.empty() || !config.maximumProbes || !config.maximumEdits ||
        !config.maximumCandidatesPerInput || !config.maximumSweeps || config.resistanceSubdivisionDepth>20 ||
        config.n0CandidateRepeats<1 || config.n0CandidateRepeats>128 ||
        (config.normalization==Normalization::TipResistance && config.n0CandidateRepeats!=1) ||
        (config.normalization!=Normalization::SynapseCount && config.normalization!=Normalization::TipResistance) ||
        protocol.schemaVersion!=3 || config.bounds.size()!=protocol.inputs.size() || initial.size()!=protocol.inputs.size() || initial.empty())
        throw std::invalid_argument("invalid_bounded_training_configuration");
    if(!Finite(config.tolerances.amplitudeAbsolute) || config.tolerances.amplitudeAbsolute<0. ||
        !Finite(config.tolerances.amplitudeRelative) || config.tolerances.amplitudeRelative<0. ||
        !Finite(config.tolerances.sync) || config.tolerances.sync<0.)
        throw std::invalid_argument("invalid_verification_configuration");
    for(const auto& b:config.bounds)
        if(b.minimumLength<1 || b.maximumLength<b.minimumLength || b.minimumSynapses<1 ||
            b.maximumSynapses>128 || b.maximumSynapses<b.minimumSynapses ||
            !Finite(b.minimumResistance) || !Finite(b.maximumResistance) || b.minimumResistance<=0. || b.maximumResistance<b.minimumResistance)
            throw std::invalid_argument("invalid_per_input_search_bounds");
    if(!ParametersLegal(protocol,config,initial))throw std::invalid_argument("initial_parameters_outside_search_domain");
}

BoundedTrainingSession::BoundedTrainingSession(CapturedReferences refs,BoundedTrainingConfig cfg,
                                             std::vector<InputParameters> initial,bool physicalDomains)
    :references(std::move(refs)),config(std::move(cfg)),current(std::move(initial)),
     epoch(config.initialModelEpoch),revision(config.initialModelRevision)
{
    requireResistanceDomains=physicalDomains && config.normalization==Normalization::TipResistance;
    ValidateBoundedTrainingInputs(references.Protocol(),config,current);
    if(current.size()!=references.Inputs().size())
        throw std::invalid_argument("invalid_bounded_training_configuration");
    // Use the exact same protocol/tolerance checks as the final gate.
    FinalVerificationSession validate(references,{config.runId,revision,config.maximumProbes,
        config.maximumInvalidObservations,epoch,config.tolerances});
    for(std::size_t i=0;i<config.bounds.size();++i)
    {
        if(!Finite(AmplitudeTolerance(config,references.Inputs()[i])))
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
    return ParametersLegal(references.Protocol(),config,parameters);
}

bool BoundedTrainingSession::PrepareCandidate(std::vector<InputParameters>& parameters) const
{
    if(references.Protocol().routing==ProbeRouting::BranchSoloBurst)
    {
        // In reverse order these earlier inputs are not yet visited in this
        // sweep. Cold all-L=1 graphs otherwise cannot grow input N-2 at all.
        // Propagate explicit positions, never silently alter their Ns/Rs or
        // the already visited inputs (including the fixed last anchor).
        for(std::size_t k=order.at(position);k>0;--k)
        {
            const auto& next=parameters[k];auto& previous=parameters[k-1];
            int minimum=next.length;
            if(previous.synapses!=next.synapses || previous.resistance!=next.resistance)
            {
                if(minimum==std::numeric_limits<int>::max())return false;
                ++minimum;
            }
            previous.length=std::max(previous.length,minimum);
        }
    }
    return Legal(parameters);
}

void BoundedTrainingSession::BeginInput()
{
    const auto index=order.at(position);const auto& b=config.bounds[index];const auto& p=current[index];
    background=current;lengths.clear();counts.clear();resistances.clear();best.reset();anchor.reset();anchorTrial.clear();
    candidateMeasurements.clear();
    lengthIndex=normalizerIndex=candidates=0;restoringBest=false;phase=Phase::Candidate;
    initialCandidatePending=requireResistanceDomains;domainReady=false;domainLength=domainNormalizer=domainVariant=0;
    domainGeometry.clear();domainScope.clear();visitedDomainCandidates.clear();
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
    else if(!requireResistanceDomains)
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
        if(PrepareCandidate(parameters))return true;
    }
    return false;
}

void BoundedTrainingSession::AdvanceDomainGeometry()
{domainReady=false;if(++domainVariant==2){domainVariant=0;++domainLength;}}

BoundedTrainingSession::DynamicCandidate BoundedTrainingSession::NextDomainCandidate(std::vector<InputParameters>& parameters)
{
    const auto input=order.at(position);
    if(initialCandidatePending)
    {
        initialCandidatePending=false;parameters=background;
        visitedDomainCandidates.push_back({revision,parameters});return DynamicCandidate::Candidate;
    }
    while(domainLength<lengths.size())
    {
        if(!domainReady)
        {
            auto geometry=background;geometry[input].length=lengths[domainLength];
            const bool branch=references.Protocol().routing==ProbeRouting::BranchSoloBurst;
            const bool locked=branch && geometry[input].length==geometry[input+1].length;
            if(locked)geometry[input].resistance=geometry[input+1].resistance;
            auto ordinary=geometry;
            if(!PrepareCandidate(ordinary))
            {
                // An upstream shared bank may be impossible to separate within
                // its length bounds. Its existing Rs still defines a legal
                // singleton candidate; do not drop that geometry altogether.
                if(branch && !locked && input>0 && geometry[input-1].length==geometry[input].length)
                {ordinary=geometry;ordinary[input].resistance=ordinary[input-1].resistance;}
                if(!PrepareCandidate(ordinary)){AdvanceDomainGeometry();continue;}
            }
            geometry=ordinary;
            // Earlier Branch inputs are not yet visited in reverse order.
            // Separate their bank before varying this Rs; downstream/anchor
            // banks remain fixed. The whole geometry is explicit in the action.
            if(domainVariant==1)
            {
                if(!branch || locked || input==0 || geometry[input].length==std::numeric_limits<int>::max())
                {AdvanceDomainGeometry();continue;}
                geometry[input-1].length=std::max(geometry[input-1].length,geometry[input].length+1);
                if(!PrepareCandidate(geometry) || geometry==ordinary){AdvanceDomainGeometry();continue;}
            }
            pending=TrainingAction{TrainingActionKind::ResistanceDomain,++serial,input,epoch,
                config.runId,{},revision,std::move(geometry),false};
            return DynamicCandidate::NeedDomain;
        }
        while(domainNormalizer<resistances.size())
        {
            parameters=domainGeometry;parameters[input].resistance=resistances[domainNormalizer++];
            if(!PrepareCandidate(parameters))continue;
            const auto seen=std::find_if(visitedDomainCandidates.begin(),visitedDomainCandidates.end(),[&](const auto& item) {
                return item.first==domainScope && item.second==parameters;
            });
            if(seen!=visitedDomainCandidates.end())continue;
            visitedDomainCandidates.push_back({domainScope,parameters});return DynamicCandidate::Candidate;
        }
        AdvanceDomainGeometry();
    }
    return DynamicCandidate::Exhausted;
}

void BoundedTrainingSession::AssessedDomain(std::size_t actionSerial,const ResistanceDomainAssessment& assessment,
    const std::string& unchangedModelRevision)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::ResistanceDomain || pending->serial!=actionSerial)
        throw std::logic_error("unrequested_resistance_domain");
    domains.push_back(assessment);
    if(assessment.modelMayHaveChanged || unchangedModelRevision!=revision || assessment.baseModelRevision!=revision ||
        assessment.inputIndex!=pending->inputIndex || assessment.geometry!=pending->parameters)
    {rejected.push_back({{},"resistance_domain_context_mismatch:"+assessment.reason});Stop(TrainingStatus::InvalidModel);return;}
    if(assessment.status==ResistanceDomainStatus::Unavailable)
    {rejected.push_back({{},"resistance_domain_unavailable:"+assessment.reason});pending.reset();AdvanceDomainGeometry();return;}
    if(assessment.status!=ResistanceDomainStatus::Available || assessment.previewModelRevision.empty())
    {rejected.push_back({{},"resistance_domain_unsupported:"+assessment.reason});Stop(TrainingStatus::InvalidModel);return;}
    const auto& bound=config.bounds.at(pending->inputIndex);
    const double low=std::max(bound.minimumResistance,assessment.minimumResistance);
    const double high=std::min(bound.maximumResistance,assessment.maximumResistance);
    if(!Finite(assessment.minimumResistance) || assessment.minimumResistance<=0. || !Finite(assessment.maximumResistance) ||
        assessment.maximumResistance<assessment.minimumResistance)
    {rejected.push_back({{},"invalid_resistance_domain_interval"});Stop(TrainingStatus::InvalidModel);return;}
    if(low>high)
    {rejected.push_back({{},"resistance_domain_outside_declared_box"});pending.reset();AdvanceDomainGeometry();return;}
    domainGeometry=pending->parameters;domainScope=assessment.previewModelRevision;resistances.clear();
    const double original=background[pending->inputIndex].resistance;
    if(original>=low && original<=high)UniquePush(resistances,original);
    UniquePush(resistances,low);UniquePush(resistances,high);
    for(unsigned level=1;level<=config.resistanceSubdivisionDepth && resistances.size()<config.maximumCandidatesPerInput;++level)
    {
        const std::size_t denominator=std::size_t(1)<<level;
        for(std::size_t numerator=1;numerator<denominator && resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
        {
            const double fraction=double(numerator)/double(denominator);
            const double value=std::exp(std::log(low)+fraction*(std::log(high)-std::log(low)));
            if(Finite(value) && value>=low && value<=high)UniquePush(resistances,value);
        }
    }
    domainNormalizer=0;domainReady=true;pending.reset();
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
        bool exhausted=candidates>=config.maximumCandidatesPerInput;
        if(!exhausted && requireResistanceDomains)
        {
            const auto next=NextDomainCandidate(candidate);
            if(next==DynamicCandidate::NeedDomain)return pending;
            exhausted=next==DynamicCandidate::Exhausted;
        }
        else if(!exhausted)exhausted=!NextCandidate(candidate);
        if(exhausted)
        {
            if(!best){Stop(TrainingStatus::SearchUnresolved);return {};}
            candidate=best->fullParameters;restoringBest=true;
            if(!PrepareCandidate(candidate))
            {rejected.push_back({{},"best_candidate_no_longer_legal"});Stop(TrainingStatus::InvalidModel);return {};}
        }
        else ++candidates;
        phase=Phase::Anchor;anchor.reset();anchorTrial.clear();candidateMeasurements.clear();
        // A compatibility graph can have the same L/Ns/Rs but different
        // effective stimulus bindings. Realize the assessed geometry explicitly
        // before treating its normalized preview scope as a measured point.
        const bool realizePreview=requireResistanceDomains && !restoringBest && domainReady &&
            candidate==domainGeometry && domainScope!=revision;
        if(candidate!=current || realizePreview)
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
    if(requireResistanceDomains)domainReady=false;
    // Every edit invalidates all current measurements, including Branch shared
    // cable dependencies. Earlier evidence remains historical, never reused.
    anchor.reset();anchorTrial.clear();
}

void BoundedTrainingSession::Reject(const std::string& reason)
{
    rejected.push_back({pending?pending->trialId:std::string{},reason});pending.reset();armed.reset();
    if(config.n0CandidateRepeats>1 && phase!=Phase::Verify)
    {RecordIncompleteCandidateRepeats(reason);anchor.reset();anchorTrial.clear();phase=Phase::Anchor;}
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
    if(requireResistanceDomains)domainReady=false;
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
    e.fullParameters=current;
    e.modelRevision=revision;e.trialId=trial;e.anchorTrialId=anchorTrial;
    e.timingResidualLow=measurement.arrival-anchor->plateauEnd;
    e.timingResidualHigh=measurement.plateauEnd-anchor->arrival;
    e.amplitudeResidual=measurement.rawPotential-reference.rawPotential;
    e.measuredRawPotential=measurement.rawPotential;
    for(std::size_t i=0;i<current.size();++i)
        e.parameterChangeCost+=std::fabs(double(current[i].length)-background[i].length)+
            std::fabs(double(current[i].synapses)-background[i].synapses)+
            std::fabs(std::log(current[i].resistance)-std::log(background[i].resistance));
    if(!Finite(e.timingResidualLow) || !Finite(e.timingResidualHigh) || !Finite(e.amplitudeResidual))
    {Reject("nonfinite_training_residual");return;}
    e.withinTiming=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh))<=config.tolerances.sync;
    e.withinAmplitude=std::fabs(e.amplitudeResidual)<=AmplitudeTolerance(config,reference);
    evidence.push_back(e);
    if(config.normalization==Normalization::SynapseCount && config.n0CandidateRepeats>1)
    {
        candidateMeasurements.push_back(e);
        if(candidateMeasurements.size()<config.n0CandidateRepeats)
        {phase=Phase::Anchor;anchor.reset();anchorTrial.clear();return;}
        CandidateRepeatEvidence group;group.inputIndex=index;group.modelEpoch=epoch;
        group.modelRevision=revision;group.fullParameters=current;
        group.minimumRawPotential=group.maximumRawPotential=e.measuredRawPotential;
        group.withinTiming=group.withinAmplitude=true;
        for(const auto& sample:candidateMeasurements)
        {
            if(sample.inputIndex!=index || sample.modelEpoch!=epoch || sample.modelRevision!=revision || sample.fullParameters!=current)
            {Stop(TrainingStatus::InvalidModel);return;}
            group.trialIds.push_back(sample.trialId);group.anchorTrialIds.push_back(sample.anchorTrialId);
            const double potential=sample.measuredRawPotential;
            group.rawPotentials.push_back(potential);
            group.minimumRawPotential=std::min(group.minimumRawPotential,potential);
            group.maximumRawPotential=std::max(group.maximumRawPotential,potential);
            group.withinTiming=group.withinTiming && sample.withinTiming;
            group.withinAmplitude=group.withinAmplitude && sample.withinAmplitude;
            if(std::fabs(sample.timingResidualLow)>std::fabs(e.timingResidualLow))e.timingResidualLow=sample.timingResidualLow;
            if(std::fabs(sample.timingResidualHigh)>std::fabs(e.timingResidualHigh))e.timingResidualHigh=sample.timingResidualHigh;
            if(std::fabs(sample.amplitudeResidual)>std::fabs(e.amplitudeResidual))e.amplitudeResidual=sample.amplitudeResidual;
        }
        e.withinTiming=group.withinTiming;e.withinAmplitude=group.withinAmplitude;
        candidateRepeats.push_back(std::move(group));candidateMeasurements.clear();
    }
    if(!best || Rank(e,config,reference)<Rank(*best,config,reference))best=e;
    if(restoringBest || (e.withinTiming && e.withinAmplitude))AdvanceInput();
    else {phase=Phase::Candidate;anchor.reset();anchorTrial.clear();}
}

void BoundedTrainingSession::RecordIncompleteCandidateRepeats(const std::string& reason)
{
    if(candidateMeasurements.empty())return;
    const auto& first=candidateMeasurements.front();
    CandidateRepeatEvidence group;group.inputIndex=first.inputIndex;group.modelEpoch=first.modelEpoch;
    group.modelRevision=first.modelRevision;group.fullParameters=first.fullParameters;
    group.minimumRawPotential=group.maximumRawPotential=first.measuredRawPotential;
    group.withinTiming=group.withinAmplitude=true;group.complete=false;group.reason=reason;
    for(const auto& sample:candidateMeasurements)
    {
        group.trialIds.push_back(sample.trialId);group.anchorTrialIds.push_back(sample.anchorTrialId);
        group.rawPotentials.push_back(sample.measuredRawPotential);
        group.minimumRawPotential=std::min(group.minimumRawPotential,sample.measuredRawPotential);
        group.maximumRawPotential=std::max(group.maximumRawPotential,sample.measuredRawPotential);
        group.withinTiming=group.withinTiming && sample.withinTiming;
        group.withinAmplitude=group.withinAmplitude && sample.withinAmplitude;
    }
    candidateRepeats.push_back(std::move(group));candidateMeasurements.clear();
}

void BoundedTrainingSession::Stop(TrainingStatus terminal)
{RecordIncompleteCandidateRepeats(TrainingStatusName(terminal));status=terminal;pending.reset();armed.reset();anchor.reset();}

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
