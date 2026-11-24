#include "BoundedTraining.h"
#include "BoundedTrainingValidation.h"
#include "CandidateWindowCoreValidation.h"
#include "NumericChecks.h"
#include "MeasuredResistanceSearch.h"
#include "MeasuredSynapseCountSearch.h"
#include "MeasuredLengthSearch.h"
#include "MeasuredCapacitanceSearch.h"
#include "CapacitanceRegion.h"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string_view>
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
    const double amplitudeTolerance=AmplitudeTolerance(cfg,reference);
    // Zero is an exact requirement, not an implicit epsilon. Keep its measured
    // error as a separate ordered axis instead of making all nonzero errors
    // indistinguishable infinities. Timing precedes amplitude when both are zero.
    const double exactTime=cfg.tolerances.sync==0.?time:0.;
    const double exactAmplitude=amplitudeTolerance==0.?amplitude:0.;
    const double residual=(cfg.tolerances.sync>0.?time/cfg.tolerances.sync:0.)+
        (amplitudeTolerance>0.?amplitude/amplitudeTolerance:0.);
    const bool overflow=!Finite(residual);
    double magnitude=residual;
    if(overflow)
    {
        // Preserve the same positive normalized sum after double division/add
        // overflow. Do not rely on long double being wider on every platform.
        const double absent=-std::numeric_limits<double>::infinity();
        const double logTime=cfg.tolerances.sync>0. && time>0.?
            std::log(time)-std::log(cfg.tolerances.sync):absent;
        const double logAmplitude=amplitudeTolerance>0. && amplitude>0.?
            std::log(amplitude)-std::log(amplitudeTolerance):absent;
        const double high=std::max(logTime,logAmplitude),low=std::min(logTime,logAmplitude);
        magnitude=high+std::log1p(std::exp(low-high));
    }
    // Finite normalized sums precede overflowing ones; ordinary finite positive
    // tolerance ranks are unchanged, including their original cost tie-break.
    return std::tuple{!e.withinTiming,!e.withinAmplitude,exactTime,exactAmplitude,
        overflow,magnitude,e.parameterChangeCost};
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

bool RegionScaleCanChange(const TrainingCapacitance& owned,const CapacitanceRegion& region,double scale)
{
    if(region.first>region.last)return false;
    const auto prefix="Dendrite"+std::to_string(region.dendrite)+"_";
    std::uint64_t known=0;
    for(const auto& [path,currentScale]:owned.membraneScales)
    {
        if(!path.starts_with(prefix))continue;
        std::string_view suffix(path);suffix.remove_prefix(prefix.size());
        // The owner has already validated canonical positive decimal paths.
        // More than ten digits cannot fall in an int-bounded cable. Inspect
        // known paths only; do not expand a potentially enormous length box.
        if(suffix.size()>10)continue;
        std::int64_t segment=0;for(const char digit:suffix)segment=10*segment+(digit-'0');
        if(segment<region.first || segment>region.last)continue;
        if(currentScale!=scale)return true;
        ++known;
    }
    // Future paths acquire a unit multiplier of their own native storage C0.
    return scale!=1. && known<std::uint64_t(region.last-region.first)+1;
}
}

void ValidateCapacitanceSearchPolicy(const CapacitanceSearchPolicy& policy)
{
    if(!Finite(policy.minimumScale) || !Finite(policy.maximumScale) || policy.minimumScale<=0. ||
        policy.minimumScale>1. || policy.maximumScale<1. || policy.maximumScale<policy.minimumScale ||
        policy.subdivisionDepth>20 || policy.maximumContexts<2 || policy.maximumContexts>4096 ||
        !policy.maximumMembranes || !policy.maximumCursorElements)
        throw std::invalid_argument("invalid_capacitance_search_policy");
}
void ValidateCapacitanceSearchPolicy(const BoundedTrainingConfig& config,ProbeRouting routing)
{
    if(config.capacitanceSearch && routing!=ProbeRouting::ClassicBurst && routing!=ProbeRouting::BranchSoloBurst)
        throw std::invalid_argument("invalid_capacitance_search_routing");
    if(config.capacitanceSearch)
    {
        const auto& policy=*config.capacitanceSearch;
        ValidateCapacitanceSearchPolicy(policy);
        if(policy.maximumContexts>config.maximumCandidatesPerInput)
            throw std::invalid_argument("invalid_capacitance_search_policy");
        const auto contexts=1+CapacitanceSeedScales(policy.minimumScale,policy.maximumScale,1.,
            policy.subdivisionDepth,policy.maximumContexts-1).size();
        const auto reserve=policy.maximumCursorElements/contexts;
        for(std::size_t i=0;i+1<config.bounds.size();++i)
        {
            const auto& bound=config.bounds[i];
            if(bound.minimumLength<1 || bound.maximumLength<bound.minimumLength ||
                config.bounds[i+1].minimumLength<1 || bound.minimumSynapses<1 ||
                bound.maximumSynapses<bound.minimumSynapses || bound.maximumSynapses>128)
                throw std::invalid_argument("invalid_per_input_search_bounds");
            const auto lengths=std::min(config.maximumCandidatesPerInput,
                std::size_t(std::int64_t(bound.maximumLength)-bound.minimumLength+1));
            const auto counts=config.normalization==Normalization::SynapseCount?
                std::size_t(bound.maximumSynapses-bound.minimumSynapses+1):0;
            if(lengths>reserve || counts>reserve-lengths)
                throw std::invalid_argument("capacitance_cursor_inventory_exceeds_declared_budget");
            const auto maximum=std::int64_t(config.bounds[i].maximumLength)-
                (routing==ProbeRouting::BranchSoloBurst?config.bounds[i+1].minimumLength:0);
            if(maximum>0 && std::uint64_t(maximum)>policy.maximumMembranes)
                throw std::invalid_argument("capacitance_search_region_exceeds_declared_budget");
        }
    }
}

void ValidateInvalidMeasurementRecovery(InvalidMeasurementRecovery policy,std::size_t invalidAllowance)
{
    if(policy!=InvalidMeasurementRecovery::RetrySameCandidate && policy!=InvalidMeasurementRecovery::AdvanceUnassignedBurst &&
        policy!=InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow)
        throw std::invalid_argument("invalid_candidate_recovery_policy");
    if(policy!=InvalidMeasurementRecovery::RetrySameCandidate && invalidAllowance==0)
        throw std::invalid_argument("candidate_recovery_requires_invalid_observation_budget");
}

void ValidateN0LengthProposalBudget(const BoundedTrainingConfig& config)
{
    if(config.maximumN0LengthProposals &&
       (config.normalization!=Normalization::SynapseCount ||
        config.maximumN0LengthProposals>config.maximumCandidatesPerInput))
        throw std::invalid_argument("invalid_n0_length_proposal_budget");
}

namespace BoundedTrainingDetail {
void ValidateInputShape(std::size_t inputs,const BoundedTrainingConfig& config,
                        const std::vector<InputParameters>& initial)
{
    if(config.runId.empty() || config.initialModelRevision.empty() || !config.maximumProbes || !config.maximumEdits ||
        !config.maximumCandidatesPerInput || !config.maximumSweeps || config.resistanceSubdivisionDepth>20 ||
        config.n0CandidateRepeats<1 || config.n0CandidateRepeats>128 ||
        (config.normalization==Normalization::TipResistance && config.n0CandidateRepeats!=1) ||
        (config.normalization!=Normalization::SynapseCount && config.normalization!=Normalization::TipResistance) ||
        config.bounds.size()!=inputs || initial.size()!=inputs || initial.empty())
        throw std::invalid_argument("invalid_bounded_training_configuration");
    ValidateN0LengthProposalBudget(config);
}
void ValidateInputRanges(const BoundedTrainingConfig& config)
{
    if(!Finite(config.tolerances.amplitudeAbsolute) || config.tolerances.amplitudeAbsolute<0. ||
        !Finite(config.tolerances.amplitudeRelative) || config.tolerances.amplitudeRelative<0. ||
        !Finite(config.tolerances.sync) || config.tolerances.sync<0.)
        throw std::invalid_argument("invalid_verification_configuration");
    for(const auto& b:config.bounds)
        if(b.minimumLength<1 || b.maximumLength<b.minimumLength || b.minimumSynapses<1 ||
            b.maximumSynapses>128 || b.maximumSynapses<b.minimumSynapses ||
            !Finite(b.minimumResistance) || !Finite(b.maximumResistance) || b.minimumResistance<=0. || b.maximumResistance<b.minimumResistance)
            throw std::invalid_argument("invalid_per_input_search_bounds");
}
}

void ValidateBoundedTrainingInputs(const VerificationProtocol& protocol,const BoundedTrainingConfig& config,
                                   const std::vector<InputParameters>& initial)
{
    ValidateVerificationProtocol(protocol);
    if(protocol.schemaVersion==5)throw std::invalid_argument("candidate_window_training_requires_proposal_admission");
    ValidateInvalidMeasurementRecovery(config.invalidMeasurementRecovery,config.maximumInvalidObservations);
    if(config.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow)
        throw std::invalid_argument("candidate_window_recovery_requires_closed_reference_admission");
    BoundedTrainingDetail::ValidateInputShape(protocol.inputs.size(),config,initial);
    if(protocol.schemaVersion!=3 && protocol.schemaVersion!=4)
        throw std::invalid_argument("invalid_bounded_training_configuration");
    BoundedTrainingDetail::ValidateInputRanges(config);
    if(protocol.schemaVersion==4 && config.invalidMeasurementRecovery!=InvalidMeasurementRecovery::RetrySameCandidate)
        throw std::invalid_argument("composite_recovery_policy_not_implemented");
    ValidateCapacitanceSearchPolicy(config,protocol.routing);
    if(!ParametersLegal(protocol,config,initial))throw std::invalid_argument("initial_parameters_outside_search_domain");
}
void ValidateCandidateWindowTrainingConfiguration(const VerificationProtocol& protocol,const BoundedTrainingConfig& config,
    const std::vector<InputParameters>& initial) {
    ValidateVerificationProtocol(protocol);ValidateCandidateWindowMeasurementContract(protocol);
    ValidateInvalidMeasurementRecovery(config.invalidMeasurementRecovery,config.maximumInvalidObservations);
    BoundedTrainingDetail::ValidateInputShape(protocol.inputs.size(),config,initial);
    BoundedTrainingDetail::ValidateInputRanges(config);
    if(config.invalidMeasurementRecovery!=InvalidMeasurementRecovery::RetrySameCandidate &&
        config.invalidMeasurementRecovery!=InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow)
        throw std::invalid_argument("composite_recovery_policy_not_implemented");
    ValidateCapacitanceSearchPolicy(config,protocol.routing);
    if(!ParametersLegal(protocol,config,initial))throw std::invalid_argument("initial_parameters_outside_search_domain");
}

void ValidateBoundedTrainingCapacitance(const BoundedTrainingConfig& config,
    const std::optional<TrainingCapacitance>& owned)
{
    ValidateTrainingCapacitance(owned);
    if(config.capacitanceSearch && (!owned ||
        config.capacitanceSearch->minimumScale<1.-owned->maximumRelativeChange ||
        config.capacitanceSearch->maximumScale>1.+owned->maximumRelativeChange))
        throw std::invalid_argument("capacitance_search_requires_compatible_owned_C0");
}

BoundedTrainingSession::BoundedTrainingSession(CapturedReferences refs,BoundedTrainingConfig cfg,
                                             std::vector<InputParameters> initial,bool physicalDomains,
                                             std::optional<TrainingCapacitance> initialCapacitance)
    :references(std::move(refs)),config(std::move(cfg)),current(std::move(initial)),
     currentCapacitance(std::move(initialCapacitance)),
     epoch(config.initialModelEpoch),revision(config.initialModelRevision)
{
    ValidateBoundedTrainingCapacitance(config,currentCapacitance);
    requireResistanceDomains=physicalDomains && config.normalization==Normalization::TipResistance;
    ValidateBoundedTrainingInputs(references.Protocol(),config,current);
    InitializeDecisionState();
}
BoundedTrainingSession::BoundedTrainingSession(CapturedReferences refs,CandidateWindowReferenceSet closed,BoundedTrainingConfig cfg,
    std::vector<InputParameters> initial,bool physicalDomains,std::optional<TrainingCapacitance> initialCapacitance)
    :references(std::move(refs)),candidateWindowReferences(std::move(closed)),config(std::move(cfg)),current(std::move(initial)),
     currentCapacitance(std::move(initialCapacitance)),epoch(config.initialModelEpoch),revision(config.initialModelRevision) {
    CandidateWindowCoreDetail::RequireCapturedReferences(references,*candidateWindowReferences);
    ValidateBoundedTrainingCapacitance(config,currentCapacitance);
    requireResistanceDomains=physicalDomains && config.normalization==Normalization::TipResistance;
    ValidateCandidateWindowTrainingConfiguration(references.Protocol(),config,current);InitializeDecisionState();
}
void BoundedTrainingSession::InitializeDecisionState()
{
    if(current.size()!=references.Inputs().size())
        throw std::invalid_argument("invalid_bounded_training_configuration");
    // Use the exact same protocol/tolerance checks as the final gate.
    if(candidateWindowReferences) {
        FinalVerificationSession validate(references,*candidateWindowReferences,{config.runId,revision,config.maximumProbes,
            config.maximumInvalidObservations,epoch,config.tolerances});
    } else {
        FinalVerificationSession validate(references,{config.runId,revision,config.maximumProbes,
            config.maximumInvalidObservations,epoch,config.tolerances});
    }
    for(std::size_t i=0;i<config.bounds.size();++i)
    {
        if(!Finite(AmplitudeTolerance(config,references.Inputs()[i])))
            throw std::invalid_argument("invalid_per_input_search_bounds");
    }
    if(!Legal(current))throw std::invalid_argument("initial_parameters_outside_search_domain");
    // Last input is fixed. Local searches visit Classic in ascending order
    // and Branch in reverse order; several bursts may precede AdvanceInput().
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
    inputSearch.capacitanceContexts.clear();inputSearch.activeCapacitanceContext=inputSearch.nextCapacitanceContext=0;inputSearch.issuedJointCandidates.clear();
    const auto index=order.at(position);const auto& b=config.bounds[index];const auto& p=current[index];
    inputSearch.background=current;inputSearch.cursor.lengths.clear();inputSearch.cursor.counts.clear();inputSearch.cursor.resistances.clear();inputSearch.best.reset();anchor.reset();anchorTrial.clear();
    inputSearch.backgroundCapacitance=currentCapacitance;
    inputSearch.candidateMeasurements.clear();inputSearch.cursor.lengthMeasurements.clear();inputSearch.cursor.n0Priority.clear();inputSearch.cursor.issuedN0Candidates.clear();
    inputSearch.cursor.n0GeometryTurns.clear();
    inputSearch.cursor.n1RecoveryGeometry.reset();inputSearch.cursor.n1RejectedCandidates.clear();inputSearch.cursor.n1RecoveryLengths.clear();
    inputSearch.cursor.n1RecoveryCount=0;inputSearch.cursor.n1RecoveryPreferMinimum=inputSearch.cursor.n1RecoveryFallbackAllowed=false;
    inputSearch.cursor.lengthIndex=inputSearch.cursor.normalizerIndex=inputSearch.candidates=0;inputSearch.restoringBest=false;phase=Phase::Candidate;
    inputSearch.cursor.initialCandidatePending=requireResistanceDomains;inputSearch.cursor.domainReady=false;inputSearch.cursor.domainLength=inputSearch.cursor.domainNormalizer=inputSearch.cursor.domainVariant=0;
    inputSearch.cursor.domainYieldPending=inputSearch.cursor.domainPassIssuedCandidate=false;
    inputSearch.cursor.domainLocalTurnUsed=false;
    inputSearch.cursor.domainLocalTurnsRemaining=0;
    inputSearch.cursor.domainGeometry.clear();inputSearch.cursor.domainScope.clear();inputSearch.cursor.visitedDomainCandidates.clear();
    inputSearch.cursor.candidateResistanceScope.clear();inputSearch.cursor.resistanceMeasurements.clear();
    // Nearest integers first. Enumeration is bounded by the declared candidate
    // budget, even for a very large physical length range.
    UniquePush(inputSearch.cursor.lengths,p.length);
    for(long long distance=1;inputSearch.cursor.lengths.size()<config.maximumCandidatesPerInput;++distance)
    {
        const long long up=static_cast<long long>(p.length)+distance,down=static_cast<long long>(p.length)-distance;
        if(up>b.maximumLength && down<b.minimumLength)break;
        if(up<=b.maximumLength)UniquePush(inputSearch.cursor.lengths,int(up));
        if(down>=b.minimumLength && inputSearch.cursor.lengths.size()<config.maximumCandidatesPerInput)UniquePush(inputSearch.cursor.lengths,int(down));
    }
    if(config.normalization==Normalization::SynapseCount)
    {
        UniquePush(inputSearch.cursor.counts,p.synapses);
        for(int distance=1;inputSearch.cursor.counts.size()<std::size_t(b.maximumSynapses-b.minimumSynapses+1);++distance)
        {
            if(p.synapses+distance<=b.maximumSynapses)UniquePush(inputSearch.cursor.counts,p.synapses+distance);
            if(p.synapses-distance>=b.minimumSynapses)UniquePush(inputSearch.cursor.counts,p.synapses-distance);
        }
    }
    else if(!requireResistanceDomains)
    {
        UniquePush(inputSearch.cursor.resistances,p.resistance);
        UniquePush(inputSearch.cursor.resistances,b.minimumResistance);UniquePush(inputSearch.cursor.resistances,b.maximumResistance);
        for(unsigned level=1;level<=config.resistanceSubdivisionDepth && inputSearch.cursor.resistances.size()<config.maximumCandidatesPerInput;++level)
        {
            const std::size_t denominator=std::size_t(1)<<level;
            for(std::size_t numerator=1;numerator<denominator && inputSearch.cursor.resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
            {
                const double fraction=double(numerator)/double(denominator);
                const double value=std::exp(std::log(b.minimumResistance)+fraction*(std::log(b.maximumResistance)-std::log(b.minimumResistance)));
                if(Finite(value) && value>=b.minimumResistance && value<=b.maximumResistance)UniquePush(inputSearch.cursor.resistances,value);
            }
        }
    }
    if(config.capacitanceSearch)
    {
        const auto& policy=*config.capacitanceSearch;
        const bool branch=references.Protocol().routing==ProbeRouting::BranchSoloBurst;
        CapacitanceRegion possible{branch?1:index+1,1,b.maximumLength};
        if(branch)
        {
            possible.first=std::int64_t(inputSearch.background[index+1].length)+1;
            for(std::size_t upstream=0;upstream<index;++upstream)
                possible.last=std::min(possible.last,std::int64_t(config.bounds[upstream].maximumLength));
        }
        std::vector<double> scales;
        // One spare seed replaces a duplicate of the unchanged full state.
        // Retain scale1 whenever it actually resets a warm correction; dormant
        // corrections outside every possible selected path do not create a slot.
        for(double scale:CapacitanceSeedScales(policy.minimumScale,policy.maximumScale,1.,
                policy.subdivisionDepth,policy.maximumContexts))
        {
            if(!RegionScaleCanChange(*inputSearch.backgroundCapacitance,possible,scale))continue;
            if(scales.size()==policy.maximumContexts-1)break;
            scales.push_back(scale);
        }
        const auto reserve=policy.maximumCursorElements/(1+scales.size());
        if(inputSearch.cursor.lengths.size()>reserve || inputSearch.cursor.counts.size()>reserve-inputSearch.cursor.lengths.size() ||
            inputSearch.cursor.resistances.size()>reserve-inputSearch.cursor.lengths.size()-inputSearch.cursor.counts.size())
            throw std::invalid_argument("capacitance_cursor_inventory_exceeds_declared_budget");
        inputSearch.capacitanceContexts.push_back({}); // Preserve the unchanged full map.
        for(double scale:scales)
        {
            auto next=inputSearch.cursor;
            // A changed C requires a fresh N1 domain; it cannot use the
            // current-graph initial-candidate shortcut.
            if(requireResistanceDomains)next.initialCandidatePending=false;
            inputSearch.capacitanceContexts.push_back({std::move(next),scale,false});
        }
    }
}

void BoundedTrainingSession::SelectCapacitanceContext(std::size_t index)
{
    if(inputSearch.capacitanceContexts.empty() || index==inputSearch.activeCapacitanceContext)return;
    std::swap(inputSearch.cursor,inputSearch.capacitanceContexts.at(inputSearch.activeCapacitanceContext).suspended);
    std::swap(inputSearch.cursor,inputSearch.capacitanceContexts.at(index).suspended);
    inputSearch.activeCapacitanceContext=index;
}
void BoundedTrainingSession::AdvanceCapacitanceContext()
{
    if(!inputSearch.capacitanceContexts.empty())inputSearch.nextCapacitanceContext=(inputSearch.activeCapacitanceContext+1)%inputSearch.capacitanceContexts.size();
}
std::optional<TrainingCapacitance> BoundedTrainingSession::CandidateCapacitance(
    const std::vector<InputParameters>& parameters) const
{
    auto result=RestoreTrainingCapacitance(inputSearch.backgroundCapacitance,currentCapacitance);
    if(inputSearch.capacitanceContexts.empty() || !inputSearch.capacitanceContexts.at(inputSearch.activeCapacitanceContext).absoluteScale)return result;
    const auto region=SelectCapacitanceRegion(references.Protocol().routing,parameters,order.at(position));
    return MakeCapacitanceRegionRequest(*result,region,*inputSearch.capacitanceContexts[inputSearch.activeCapacitanceContext].absoluteScale,
        config.capacitanceSearch->maximumMembranes);
}

bool BoundedTrainingSession::NextCandidate(std::vector<InputParameters>& parameters)
{
    const auto index=order.at(position);
    const bool n0=config.normalization==Normalization::SynapseCount;
    const auto issue=[&](std::vector<InputParameters>& candidate) {
        if(!PrepareCandidate(candidate))return false;
        if(n0)
        {
            if(std::find(inputSearch.cursor.issuedN0Candidates.begin(),inputSearch.cursor.issuedN0Candidates.end(),candidate)!=inputSearch.cursor.issuedN0Candidates.end())return false;
            inputSearch.cursor.issuedN0Candidates.push_back(candidate);
        }
        return true;
    };
    // These are proposals from completed measurements, never assumed solutions.
    // Required repeats occur inside Consume and bypass candidate deduplication.
    while(!inputSearch.cursor.n0Priority.empty())
    {
        parameters=std::move(inputSearch.cursor.n0Priority.front());inputSearch.cursor.n0Priority.erase(inputSearch.cursor.n0Priority.begin());
        if(issue(parameters))return true;
    }
    const std::size_t normalizers=n0?inputSearch.cursor.counts.size():inputSearch.cursor.resistances.size();
    // First evaluate the current normalizer at each length; then the next
    // normalizer at each length. No monotonicity or bracket is inferred.
    while(inputSearch.cursor.normalizerIndex<normalizers)
    {
        if(inputSearch.cursor.lengthIndex==inputSearch.cursor.lengths.size()){inputSearch.cursor.lengthIndex=0;++inputSearch.cursor.normalizerIndex;continue;}
        parameters=inputSearch.background;parameters[index].length=inputSearch.cursor.lengths[inputSearch.cursor.lengthIndex++];
        if(config.normalization==Normalization::SynapseCount)parameters[index].synapses=inputSearch.cursor.counts[inputSearch.cursor.normalizerIndex];
        else parameters[index].resistance=inputSearch.cursor.resistances[inputSearch.cursor.normalizerIndex];
        if(issue(parameters))return true;
    }
    return false;
}

void BoundedTrainingSession::AdvanceDomainGeometry()
{
    inputSearch.cursor.domainReady=false;inputSearch.cursor.domainLocalTurnUsed=false;
    inputSearch.cursor.domainLocalTurnsRemaining=0;
    inputSearch.cursor.n1RecoveryGeometry.reset();inputSearch.cursor.n1RecoveryFallbackAllowed=false;
    inputSearch.cursor.n1RecoveryPreferMinimum=false;
    if(++inputSearch.cursor.domainVariant==2){inputSearch.cursor.domainVariant=0;++inputSearch.cursor.domainLength;}
}

void BoundedTrainingSession::PrioritizeMeasuredLength(const CandidateEvidence& measurement)
{
    if(inputSearch.restoringBest)return;
    inputSearch.cursor.lengthMeasurements.push_back(measurement);
    const auto input=measurement.inputIndex;
    const bool branchFamily=requireResistanceDomains && config.normalization==Normalization::TipResistance &&
        references.Protocol().routing==ProbeRouting::BranchSoloBurst;
    const bool n0BranchFamily=config.normalization==Normalization::SynapseCount && input>0 &&
        references.Protocol().routing==ProbeRouting::BranchSoloBurst;
    bool usedN0Family=false;
    std::vector<MeasuredLengthPoint> points;
    for(const auto& previous:inputSearch.cursor.lengthMeasurements)
    {
        if(previous.inputIndex!=input || previous.fullParameters.size()!=measurement.fullParameters.size() ||
            !EquivalentTrainingCapacitance(previous.capacitance,measurement.capacitance))continue;
        auto geometry=previous.fullParameters;
        geometry[input].length=measurement.parameters.length;
        if(geometry!=measurement.fullParameters)
        {
            if((!branchFamily && !n0BranchFamily) ||
               !SameBranchLengthProposalFamily(inputSearch.background,input,previous.fullParameters,measurement.fullParameters))continue;
            usedN0Family=usedN0Family || n0BranchFamily;
        }
        points.push_back({previous.parameters.length,previous.timingResidualLow,previous.timingResidualHigh});
    }
    const auto& bound=config.bounds[input];
    auto proposal=ProposeMeasuredLength(std::move(points),bound.minimumLength,bound.maximumLength,config.tolerances.sync);
    const std::size_t next=requireResistanceDomains?inputSearch.cursor.domainLength+1:inputSearch.cursor.lengthIndex;
    if(next>inputSearch.cursor.lengths.size())return;
    if(!proposal && requireResistanceDomains && config.normalization==Normalization::TipResistance &&
       measurement.withinTiming && !measurement.withinAmplitude)
    {
        // A timing-ready point is not an amplitude solution. After its finite
        // Rs turn, explore a nearby unvisited geometry rather than returning
        // directly to the distant initial length mesh. This is a proposal only:
        // the ordinary N1 cursor still assesses the new effective Rs domain,
        // applies the native edit and measures a fresh anchor/input pair.
        std::vector<int> issued(inputSearch.cursor.lengths.begin(),inputSearch.cursor.lengths.begin()+next);
        UniquePush(issued,measurement.parameters.length);
        const auto attempts=config.maximumCandidatesPerInput;
        for(std::size_t i=0;i<attempts;++i)
        {
            proposal=ProposeNearestUnvisitedLength(issued,measurement.parameters.length,
                bound.minimumLength,bound.maximumLength);
            if(!proposal)break;
            auto candidate=measurement.fullParameters;candidate[input].length=*proposal;
            if(PrepareCandidate(candidate))break;
            // A forbidden shared-bank geometry is not a measured failure or
            // proof of infeasibility. Skip it in this bounded proposal turn.
            issued.push_back(*proposal);proposal.reset();
        }
    }
    if(!proposal)return;
    if(branchFamily || usedN0Family)
    {
        // N0 borrows growth-family points only for a proposal, never for a
        // measured residual/certificate or a geometry-local amplitude turn.
        // Preserve the old N0 order when no additional family point was used.
        // Bound an extrapolated family proposal's displacement. This is a
        // search order policy, never a numerical tolerance or a measured peak.
        const auto length=std::int64_t(measurement.parameters.length);
        *proposal=int(std::clamp<std::int64_t>(*proposal,
            std::max<std::int64_t>(bound.minimumLength,(length+1)/2),
            std::min<std::int64_t>(bound.maximumLength,2*length)));
    }
    auto candidate=measurement.fullParameters;candidate[input].length=*proposal;
    if(!PrepareCandidate(candidate))return;
    // Do not interrupt an assessed N1 geometry or bypass its domain checks.
    // The next geometry is still assessed/applied through the native adapter.
    // N0 uses its existing full-vector deduplication and repeated probes.
    if(std::find(inputSearch.cursor.lengths.begin(),inputSearch.cursor.lengths.begin()+next,*proposal)!=inputSearch.cursor.lengths.begin()+next)return;
    inputSearch.cursor.lengths.erase(std::remove(inputSearch.cursor.lengths.begin()+next,inputSearch.cursor.lengths.end(),*proposal),inputSearch.cursor.lengths.end());
    inputSearch.cursor.lengths.insert(inputSearch.cursor.lengths.begin()+next,*proposal);
}

void BoundedTrainingSession::PrioritizeRejectedN0BranchLength()
{
    if(config.normalization!=Normalization::SynapseCount ||
        references.Protocol().routing!=ProbeRouting::BranchSoloBurst)return;
    const auto input=order.at(position);
    if(input+1>=inputSearch.background.size())return;
    // This recovery explores geometry at the input's original normalizer.
    // A failed local Ns proposal must not silently reset its normalizer or
    // lend its issued geometry to a different physical search family.
    for(std::size_t i=0;i<inputSearch.background.size();++i)
        if(current[i].synapses!=inputSearch.background[i].synapses ||
           current[i].resistance!=inputSearch.background[i].resistance)return;
    const auto& bound=config.bounds[input];
    std::int64_t minimum=std::max(bound.minimumLength,inputSearch.background[input+1].length);
    if(inputSearch.background[input].synapses!=inputSearch.background[input+1].synapses ||
       inputSearch.background[input].resistance!=inputSearch.background[input+1].resistance)
        minimum=std::max<std::int64_t>(minimum,std::int64_t(inputSearch.background[input+1].length)+1);
    std::int64_t maximum=bound.maximumLength;
    // PrepareCandidate may grow earlier banks, never a committed downstream
    // input. Different adjacent bank physics requires one extra segment.
    std::int64_t separation=0;
    for(std::size_t k=input;k>0;--k)
    {
        if(inputSearch.background[k-1].synapses!=inputSearch.background[k].synapses ||
           inputSearch.background[k-1].resistance!=inputSearch.background[k].resistance)++separation;
        maximum=std::min(maximum,std::int64_t(config.bounds[k-1].maximumLength)-separation);
    }
    if(minimum>maximum)return;
    std::vector<int> issued;
    for(const auto& candidate:inputSearch.cursor.issuedN0Candidates)
        if(SameBranchLengthProposalFamily(inputSearch.background,input,candidate,current))issued.push_back(candidate[input].length);
    const auto proposed=ProposeUnvisitedLength(std::move(issued),int(minimum),int(maximum));
    if(!proposed)return;
    auto candidate=inputSearch.background;candidate[input].length=*proposed;
    if(!PrepareCandidate(candidate))return;
    if(std::find(inputSearch.cursor.issuedN0Candidates.begin(),inputSearch.cursor.issuedN0Candidates.end(),candidate)!=inputSearch.cursor.issuedN0Candidates.end())return;
    // Reorder only. Normal edit/readback, fresh anchor/input observations and
    // the existing attempt-wide budgets remain mandatory. No rejected peak
    // becomes a scalar residual or a measured length trend.
    inputSearch.cursor.n0Priority.erase(std::remove(inputSearch.cursor.n0Priority.begin(),inputSearch.cursor.n0Priority.end(),candidate),inputSearch.cursor.n0Priority.end());
    inputSearch.cursor.n0Priority.insert(inputSearch.cursor.n0Priority.begin(),std::move(candidate));
}

bool BoundedTrainingSession::QueueRejectedN1Geometry(bool allowSameLength)
{
    if(!requireResistanceDomains || config.normalization!=Normalization::TipResistance)return false;
    const auto input=order.at(position);const auto& bound=config.bounds.at(input);
    const bool branch=references.Protocol().routing==ProbeRouting::BranchSoloBurst;
    std::int64_t minimum=bound.minimumLength,maximum=bound.maximumLength;
    if(branch)
    {
        if(input+1>=inputSearch.background.size())return false;
        // A freely adjustable Rs cannot share a committed downstream bank.
        minimum=std::max(minimum,std::int64_t(inputSearch.background[input+1].length)+1);
        std::int64_t separation=1;
        for(std::size_t k=input;k>0;--k)
        {
            maximum=std::min(maximum,std::int64_t(config.bounds[k-1].maximumLength)-separation);
            if(k>1 && (inputSearch.background[k-2].synapses!=inputSearch.background[k-1].synapses ||
                       inputSearch.background[k-2].resistance!=inputSearch.background[k-1].resistance))++separation;
        }
    }
    if(minimum>maximum)return false;
    std::vector<int> issued=inputSearch.cursor.n1RecoveryLengths;
    for(const auto& candidate:inputSearch.cursor.visitedDomainCandidates)
        if(candidate.second.size()==inputSearch.background.size())issued.push_back(candidate.second[input].length);
    std::optional<int> length;
    if(allowSameLength && current[input].length>=minimum && current[input].length<=maximum)
        length=current[input].length;
    else {
        // Keep the first recovery center fixed: do not double geometry after
        // every rejected observation. A costly endpoint can otherwise consume
        // the group deadline before its fresh anchor/input pair completes.
        const int center=inputSearch.cursor.n1RecoveryLengths.empty()?current[input].length:inputSearch.cursor.n1RecoveryLengths.front();
        length=ProposeModerateUnvisitedLength(std::move(issued),center,int(minimum),int(maximum));
    }
    if(!length)return false;
    auto geometry=inputSearch.background;geometry[input].length=*length;
    if(branch && input)
        geometry[input-1].length=std::max(geometry[input-1].length,*length+1);
    if(!PrepareCandidate(geometry))return false;
    inputSearch.cursor.n1RecoveryGeometry=std::move(geometry);
    UniquePush(inputSearch.cursor.n1RecoveryLengths,*length);++inputSearch.cursor.n1RecoveryCount;
    inputSearch.cursor.n1RecoveryPreferMinimum=true;
    // One same-length domain can be a locked singleton. Permit one new-length
    // domain-only fallback without spending/replenishing any observation budget.
    inputSearch.cursor.n1RecoveryFallbackAllowed=allowSameLength;
    inputSearch.cursor.domainReady=false;inputSearch.cursor.domainYieldPending=false;inputSearch.cursor.domainLocalTurnUsed=false;
    inputSearch.cursor.domainLocalTurnsRemaining=0;inputSearch.cursor.domainScope.clear();inputSearch.cursor.candidateResistanceScope.clear();
    return true;
}

bool BoundedTrainingSession::FallbackRejectedN1Geometry()
{
    if(!inputSearch.cursor.n1RecoveryGeometry || !inputSearch.cursor.n1RecoveryFallbackAllowed)return false;
    inputSearch.cursor.n1RecoveryFallbackAllowed=false;
    return QueueRejectedN1Geometry(false);
}

void BoundedTrainingSession::PrioritizeRejectedN1()
{
    if(!requireResistanceDomains || config.normalization!=Normalization::TipResistance)return;
    inputSearch.cursor.n1RejectedCandidates.push_back(current);
    QueueRejectedN1Geometry(inputSearch.cursor.n1RecoveryCount==0);
}

void BoundedTrainingSession::PrioritizeN0Neighbors(const CandidateEvidence& e)
{
    if(inputSearch.restoringBest || config.normalization!=Normalization::SynapseCount)return;
    if(!e.withinTiming) {PrioritizeN0LengthNeighborhood();return;}
    if(e.withinAmplitude)return;
    const auto index=e.inputIndex;
    auto turn=std::find_if(inputSearch.cursor.n0GeometryTurns.begin(),inputSearch.cursor.n0GeometryTurns.end(),[&](const auto& previous) {
        if(previous.best.inputIndex!=index || previous.best.fullParameters.size()!=e.fullParameters.size() ||
            !EquivalentTrainingCapacitance(previous.best.capacitance,e.capacitance))return false;
        auto geometry=previous.best.fullParameters;
        geometry[index].synapses=e.parameters.synapses;
        return geometry==e.fullParameters;
    });
    bool improved=true;
    if(turn==inputSearch.cursor.n0GeometryTurns.end())
    {
        const auto remainingCandidates=config.maximumCandidatesPerInput-inputSearch.candidates;
        const auto remainingProbePairs=(config.maximumProbes-probes)/2/config.n0CandidateRepeats;
        const auto remainingEdits=config.maximumEdits-edits;
        // Reserve work for other geometries without disabling useful small-
        // budget N0 searches. Each queued proposal consumes this finite turn;
        // invalid observations and revisits never replenish it. Required
        // repeats still use fresh anchor/input pairs through Consume.
        const auto allowance=std::min({std::size_t(12),remainingCandidates,remainingProbePairs,
            remainingEdits,std::max(std::size_t(4),remainingCandidates/4)});
        inputSearch.cursor.n0GeometryTurns.push_back({e,allowance});
        turn=std::prev(inputSearch.cursor.n0GeometryTurns.end());
    }
    else
    {
        improved=Rank(e,config,references.Inputs().at(index))<Rank(turn->best,config,references.Inputs().at(index));
        if(improved)turn->best=e;
    }
    const auto& bound=config.bounds[index];
    const double tolerance=AmplitudeTolerance(config,references.Inputs().at(index));
    std::vector<MeasuredSynapseCountPoint> countMeasurements;
    bool repeatable=true;
    for(const auto& previous:inputSearch.cursor.lengthMeasurements)
    {
        if(previous.inputIndex!=index || !previous.withinTiming || previous.fullParameters.size()!=e.fullParameters.size() ||
           !EquivalentTrainingCapacitance(previous.capacitance,e.capacitance))continue;
        auto geometry=previous.fullParameters;
        geometry[index].synapses=e.parameters.synapses;
        if(geometry!=e.fullParameters)continue;
        if(config.n0CandidateRepeats>1)
        {
            const auto group=std::find_if(candidateRepeats.begin(),candidateRepeats.end(),[&](const auto& g) {
                return g.inputIndex==index && g.modelRevision==previous.modelRevision &&
                    g.fullParameters==previous.fullParameters &&
                    std::find(g.trialIds.begin(),g.trialIds.end(),previous.trialId)!=g.trialIds.end();
            });
            if(group==candidateRepeats.end() || group->maximumRawPotential-group->minimumRawPotential>tolerance)
            {repeatable=false;break;}
        }
        countMeasurements.push_back({previous.parameters.synapses,previous.amplitudeResidual,true});
    }
    if(repeatable && turn->proposalsRemaining)
        if(const auto count=ProposeMeasuredSynapseCount(std::move(countMeasurements),
                bound.minimumSynapses,bound.maximumSynapses,tolerance))
        {
            auto candidate=e.fullParameters;candidate[index].synapses=*count;
            const auto requested=candidate;
            // A Branch fixup that moves another attachment is a different
            // geometry. Keep ordinary exploration, not this fitted proposal.
            if(PrepareCandidate(candidate) && candidate==requested &&
               std::find(inputSearch.cursor.issuedN0Candidates.begin(),inputSearch.cursor.issuedN0Candidates.end(),candidate)==inputSearch.cursor.issuedN0Candidates.end() &&
               std::find(inputSearch.cursor.n0Priority.begin(),inputSearch.cursor.n0Priority.end(),candidate)==inputSearch.cursor.n0Priority.end())
            {
                inputSearch.cursor.n0Priority.insert(inputSearch.cursor.n0Priority.begin(),std::move(candidate));
                --turn->proposalsRemaining;
                // Spend this finite turn on the fitted measurement first.
                // Reserving ordinary neighbours now can exhaust the turn
                // before that measurement can refine its still-unbracketed
                // root. Fallback remains available if no legal unissued fit
                // exists; no candidate/probe/edit allowance is replenished.
                return;
            }
        }
    // An adjacent measured count bracket has no interior integer. A finite
    // opt-in geometry proposal still needs a new edit and fresh observations.
    if(PrioritizeN0LengthNeighborhood())return;
    // A measured overshoot can add a useful bracket without improving the
    // incumbent. Only the fitted proposal may use that information; unchanged
    // ordinary neighbour exploration still requires an improving measurement.
    if(!improved)return;
    for(const int delta:{1,-1})
    {
        if(!turn->proposalsRemaining)break;
        auto candidate=e.fullParameters;candidate[index].synapses+=delta;
        if(candidate[index].synapses<bound.minimumSynapses || candidate[index].synapses>bound.maximumSynapses ||
            !PrepareCandidate(candidate))continue;
        if(std::find(inputSearch.cursor.issuedN0Candidates.begin(),inputSearch.cursor.issuedN0Candidates.end(),candidate)==inputSearch.cursor.issuedN0Candidates.end() &&
            std::find(inputSearch.cursor.n0Priority.begin(),inputSearch.cursor.n0Priority.end(),candidate)==inputSearch.cursor.n0Priority.end())
        {
            inputSearch.cursor.n0Priority.push_back(std::move(candidate));--turn->proposalsRemaining;
        }
    }
}

bool BoundedTrainingSession::PrioritizeN0LengthNeighborhood()
{
    if(inputSearch.restoringBest || config.normalization!=Normalization::SynapseCount ||
       !config.maximumN0LengthProposals)return false;
    const auto input=order.at(position);const auto& bound=config.bounds[input];
    auto& issued=n0LengthProposalsIssued[input];
    if(issued>=config.maximumN0LengthProposals)return false;
    const auto& reference=references.Inputs().at(input);
    const double tolerance=AmplitudeTolerance(config,reference);
    struct Family {std::vector<const CandidateEvidence*> points;bool repeatable=true;};
    std::vector<Family> families;
    for(const auto& e:inputSearch.cursor.lengthMeasurements)
    {
        if(e.inputIndex!=input || !e.withinTiming)continue;
        auto family=std::find_if(families.begin(),families.end(),[&](const auto& f) {
            const auto& old=*f.points.front();auto geometry=old.fullParameters;
            geometry[input].synapses=e.parameters.synapses;
            return geometry==e.fullParameters && EquivalentTrainingCapacitance(old.capacitance,e.capacitance);
        });
        if(family==families.end()) {families.push_back({});family=std::prev(families.end());}
        family->points.push_back(&e);
        if(config.n0CandidateRepeats>1)
        {
            const auto group=std::find_if(candidateRepeats.begin(),candidateRepeats.end(),[&](const auto& g) {
                return g.inputIndex==input && g.modelRevision==e.modelRevision &&
                    g.fullParameters==e.fullParameters &&
                    EquivalentTrainingCapacitance(g.capacitance,e.capacitance) &&
                    std::find(g.trialIds.begin(),g.trialIds.end(),e.trialId)!=g.trialIds.end();
            });
            family->repeatable=family->repeatable && group!=candidateRepeats.end() &&
                group->maximumRawPotential-group->minimumRawPotential<=tolerance;
        }
    }
    const CandidateEvidence* bestSeed=nullptr;
    std::vector<InputParameters> bestProposal;
    for(const auto& family:families)
    {
        if(!family.repeatable)continue;
        std::vector<MeasuredSynapseCountPoint> counts;
        for(const auto* e:family.points)counts.push_back({e->parameters.synapses,e->amplitudeResidual,true});
        for(const auto& bracket:MeasuredAdjacentSynapseCountBrackets(std::move(counts),
                bound.minimumSynapses,bound.maximumSynapses,tolerance))
        {
            // Rank real measured endpoints across every retained local family:
            // a worse recent geometry must not pull the search away from a
            // better measured bracket. Repeated evidence is conservative.
            const CandidateEvidence* seed=nullptr;
            for(const int n:{bracket.lower,bracket.upper})
            {
                const CandidateEvidence* endpoint=nullptr;
                for(const auto* e:family.points)
                    if(e->parameters.synapses==n &&
                       (!endpoint || Rank(*endpoint,config,reference)<Rank(*e,config,reference)))endpoint=e;
                if(endpoint && (!seed || Rank(*endpoint,config,reference)<Rank(*seed,config,reference)))seed=endpoint;
            }
            if(!seed || (bestSeed && !(Rank(*seed,config,reference)<Rank(*bestSeed,config,reference))))continue;
            std::vector<int> considered{seed->parameters.length};
            for(std::size_t attempt=0;attempt<config.maximumCandidatesPerInput;++attempt)
            {
                const auto length=ProposeNearestUnvisitedLength(considered,seed->parameters.length,
                    bound.minimumLength,bound.maximumLength);
                if(!length)break;
                considered.push_back(*length);auto candidate=seed->fullParameters;candidate[input].length=*length;
                if(!PrepareCandidate(candidate) ||
                   std::find(inputSearch.cursor.issuedN0Candidates.begin(),inputSearch.cursor.issuedN0Candidates.end(),candidate)!=inputSearch.cursor.issuedN0Candidates.end() ||
                   std::find(inputSearch.cursor.n0Priority.begin(),inputSearch.cursor.n0Priority.end(),candidate)!=inputSearch.cursor.n0Priority.end())continue;
                bestSeed=seed;bestProposal=std::move(candidate);break;
            }
        }
    }
    if(!bestSeed)return false;
    inputSearch.cursor.n0Priority.insert(inputSearch.cursor.n0Priority.begin(),std::move(bestProposal));
    ++issued;return true;
}

BoundedTrainingSession::DynamicCandidate BoundedTrainingSession::NextDomainCandidate(std::vector<InputParameters>& parameters)
{
    const auto input=order.at(position);
    // A valid observation's search order must not depend on how an invalid
    // burst would be recovered. Give each legal geometry one unissued Rs per
    // pass, with one bounded measured timing-ready normalization turn: a dense
    // Rs mesh must not starve other lengths. Every edit still invalidates the
    // domain and both anchor/input observations; no old interval is reused.
    // Every revisit still requires a fresh native domain assessment below.
    const bool interleave=config.normalization==Normalization::TipResistance;
    if(inputSearch.cursor.initialCandidatePending)
    {
        inputSearch.cursor.initialCandidatePending=false;parameters=inputSearch.background;
        inputSearch.cursor.candidateResistanceScope.clear();
        inputSearch.cursor.visitedDomainCandidates.push_back({revision,parameters});return DynamicCandidate::Candidate;
    }
    if(interleave && inputSearch.cursor.domainYieldPending)
    {
        inputSearch.cursor.domainYieldPending=false;AdvanceDomainGeometry();
    }
    for(;;)
    {
        if(inputSearch.cursor.domainLength==inputSearch.cursor.lengths.size())
        {
            if(!interleave || !inputSearch.cursor.domainPassIssuedCandidate)return DynamicCandidate::Exhausted;
            // Revisit every geometry for its next unissued Rs. Reassessment
            // stays mandatory: an old interval/scope is never a live bound.
            // A pass without an issued candidate proves only cursor exhaustion.
            inputSearch.cursor.domainLength=inputSearch.cursor.domainVariant=0;inputSearch.cursor.domainReady=false;
            inputSearch.cursor.domainLocalTurnUsed=false;
            inputSearch.cursor.domainLocalTurnsRemaining=0;
            inputSearch.cursor.domainPassIssuedCandidate=false;
        }
        if(!inputSearch.cursor.domainReady)
        {
            std::vector<InputParameters> geometry;
            if(inputSearch.cursor.n1RecoveryGeometry)geometry=*inputSearch.cursor.n1RecoveryGeometry;
            else {
            geometry=inputSearch.background;geometry[input].length=inputSearch.cursor.lengths[inputSearch.cursor.domainLength];
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
            if(inputSearch.cursor.domainVariant==1)
            {
                if(!branch || locked || input==0 || geometry[input].length==std::numeric_limits<int>::max())
                {AdvanceDomainGeometry();continue;}
                geometry[input-1].length=std::max(geometry[input-1].length,geometry[input].length+1);
                if(!PrepareCandidate(geometry) || geometry==ordinary){AdvanceDomainGeometry();continue;}
            }
            }
            pending=TrainingAction{TrainingActionKind::ResistanceDomain,++serial,input,epoch,
                config.runId,{},revision,std::move(geometry),false};
            pending->capacitance=CandidateCapacitance(pending->parameters);
            return DynamicCandidate::NeedDomain;
        }
        while(inputSearch.cursor.domainNormalizer<inputSearch.cursor.resistances.size())
        {
            parameters=inputSearch.cursor.domainGeometry;parameters[input].resistance=inputSearch.cursor.resistances[inputSearch.cursor.domainNormalizer++];
            if(!PrepareCandidate(parameters))continue;
            const auto seen=std::find_if(inputSearch.cursor.visitedDomainCandidates.begin(),inputSearch.cursor.visitedDomainCandidates.end(),[&](const auto& item) {
                return item.first==inputSearch.cursor.domainScope && item.second==parameters;
            });
            if(seen!=inputSearch.cursor.visitedDomainCandidates.end())continue;
            if(inputSearch.cursor.n1RecoveryGeometry && std::find(inputSearch.cursor.n1RejectedCandidates.begin(),
                inputSearch.cursor.n1RejectedCandidates.end(),parameters)!=inputSearch.cursor.n1RejectedCandidates.end())continue;
            inputSearch.cursor.n1RecoveryPreferMinimum=false;
            inputSearch.cursor.visitedDomainCandidates.push_back({inputSearch.cursor.domainScope,parameters});
            // A proposal can still be rejected by cross-C-context dedup below.
            // Charge an issued pass only after actual candidate admission.
            if(interleave)inputSearch.cursor.domainYieldPending=true;
            inputSearch.cursor.candidateResistanceScope=inputSearch.cursor.domainScope;return DynamicCandidate::Candidate;
        }
        if(FallbackRejectedN1Geometry())continue;
        AdvanceDomainGeometry();
    }
}

void BoundedTrainingSession::AssessedDomain(std::size_t actionSerial,const ResistanceDomainAssessment& assessment,
    const std::string& unchangedModelRevision)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::ResistanceDomain || pending->serial!=actionSerial)
        throw std::logic_error("unrequested_resistance_domain");
    domains.push_back(assessment);
    if(assessment.modelMayHaveChanged || unchangedModelRevision!=revision || assessment.baseModelRevision!=revision ||
        assessment.inputIndex!=pending->inputIndex || assessment.geometry!=pending->parameters ||
        assessment.capacitance!=pending->capacitance)
    {rejected.push_back({{},"resistance_domain_context_mismatch:"+assessment.reason});Stop(TrainingStatus::InvalidModel);return;}
    if(assessment.status==ResistanceDomainStatus::Unavailable)
    {rejected.push_back({{},"resistance_domain_unavailable:"+assessment.reason});pending.reset();if(!FallbackRejectedN1Geometry())AdvanceDomainGeometry();AdvanceCapacitanceContext();return;}
    if(assessment.status!=ResistanceDomainStatus::Available || assessment.previewModelRevision.empty())
    {rejected.push_back({{},"resistance_domain_unsupported:"+assessment.reason});Stop(TrainingStatus::InvalidModel);return;}
    const auto& bound=config.bounds.at(pending->inputIndex);
    const double low=std::max(bound.minimumResistance,assessment.minimumResistance);
    const double high=std::min(bound.maximumResistance,assessment.maximumResistance);
    if(!Finite(assessment.minimumResistance) || assessment.minimumResistance<=0. || !Finite(assessment.maximumResistance) ||
        assessment.maximumResistance<assessment.minimumResistance)
    {rejected.push_back({{},"invalid_resistance_domain_interval"});Stop(TrainingStatus::InvalidModel);return;}
    if(low>high)
    {rejected.push_back({{},"resistance_domain_outside_declared_box"});pending.reset();if(!FallbackRejectedN1Geometry())AdvanceDomainGeometry();AdvanceCapacitanceContext();return;}
    inputSearch.cursor.domainGeometry=pending->parameters;inputSearch.cursor.domainScope=assessment.previewModelRevision;inputSearch.cursor.resistances.clear();
    const auto canAppend=[&] {
        if(inputSearch.capacitanceContexts.empty())return true;
        const auto reserve=config.capacitanceSearch->maximumCursorElements/inputSearch.capacitanceContexts.size();
        return inputSearch.cursor.lengths.size()+inputSearch.cursor.counts.size()+inputSearch.cursor.resistances.size()<reserve;
    };
    const auto add=[&](double value) {
        if(std::find(inputSearch.cursor.resistances.begin(),inputSearch.cursor.resistances.end(),value)!=inputSearch.cursor.resistances.end())return true;
        if(!canAppend()){Stop(TrainingStatus::BudgetExhausted,"capacitance_cursor_inventory_budget_exhausted");return false;}
        UniquePush(inputSearch.cursor.resistances,value);return true;
    };
    const double original=inputSearch.background[pending->inputIndex].resistance;
    if(original>=low && original<=high && !add(original))return;
    if(!add(low) || !add(high))return;
    for(unsigned level=1;level<=config.resistanceSubdivisionDepth && inputSearch.cursor.resistances.size()<config.maximumCandidatesPerInput;++level)
    {
        const std::size_t denominator=std::size_t(1)<<level;
        for(std::size_t numerator=1;numerator<denominator && inputSearch.cursor.resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
        {
            const double fraction=double(numerator)/double(denominator);
            const double value=std::exp(std::log(low)+fraction*(std::log(high)-std::log(low)));
            if(Finite(value) && value>=low && value<=high && !add(value))return;
        }
    }
    // All retained observations were validated against this session's fixed
    // stimulus profile. A different effective source amplitude cannot borrow
    // their bracket even when the normalized neuron graph hash is unchanged.
    const auto& stimulus=references.Protocol().stimulusProfile;
    if(config.resistanceSubdivisionDepth>0 && stimulus && Finite(assessment.sourceAmplitude) &&
       assessment.sourceAmplitude==stimulus->amplitude)
    {
        std::vector<MeasuredResistancePoint> points;
        for(const auto& [scope,measurement]:inputSearch.cursor.resistanceMeasurements)
        {
            if(scope!=inputSearch.cursor.domainScope || measurement.fullParameters.size()!=inputSearch.cursor.domainGeometry.size() ||
                !EquivalentTrainingCapacitance(measurement.capacitance,pending->capacitance))continue;
            auto geometry=measurement.fullParameters;
            geometry[pending->inputIndex].resistance=inputSearch.cursor.domainGeometry[pending->inputIndex].resistance;
            if(geometry!=inputSearch.cursor.domainGeometry)continue;
            points.push_back({measurement.parameters.resistance,measurement.amplitudeResidual,measurement.withinTiming});
        }
        if(const auto refined=ProposeMeasuredResistanceRefinement(points,low,high,
                AmplitudeTolerance(config,references.Inputs().at(pending->inputIndex))))
        {
            inputSearch.cursor.resistances.erase(std::remove(inputSearch.cursor.resistances.begin(),inputSearch.cursor.resistances.end(),*refined),inputSearch.cursor.resistances.end());
            if(!canAppend()){Stop(TrainingStatus::BudgetExhausted,"capacitance_cursor_inventory_budget_exhausted");return;}
            inputSearch.cursor.resistances.insert(inputSearch.cursor.resistances.begin(),*refined);
        }
    }
    if(inputSearch.cursor.n1RecoveryGeometry && inputSearch.cursor.n1RecoveryPreferMinimum)
    {
        // Try the fresh lower Rs endpoint first after a rejected point.
        // This is a proposal, not a monotonicity assumption or evidence. Ordinary
        // valid-observation scheduling and later measured refinements stay intact.
        inputSearch.cursor.resistances.erase(std::remove(inputSearch.cursor.resistances.begin(),inputSearch.cursor.resistances.end(),low),inputSearch.cursor.resistances.end());
        inputSearch.cursor.resistances.insert(inputSearch.cursor.resistances.begin(),low);
    }
    inputSearch.cursor.domainNormalizer=0;inputSearch.cursor.domainReady=true;pending.reset();
}

void BoundedTrainingSession::AdvanceInput()
{
    anchor.reset();anchorTrial.clear();++position;
    if(position==order.size())BeginVerification();else BeginInput();
}

void BoundedTrainingSession::BeginVerification()
{
    if(probes>=config.maximumProbes){Stop(TrainingStatus::BudgetExhausted,"maximum_probes_exhausted");return;}
    phase=Phase::Verify;status=TrainingStatus::Verifying;
    verificationInvalidAccounted=0;
    const VerificationConfig finalConfig{
        config.runId+"/final/"+std::to_string(sweep),revision,config.maximumProbes-probes,
        config.maximumInvalidObservations-std::min(invalid,config.maximumInvalidObservations),epoch,config.tolerances};
    if(candidateWindowReferences)verification=std::make_unique<FinalVerificationSession>(references,*candidateWindowReferences,finalConfig);
    else verification=std::make_unique<FinalVerificationSession>(references,finalConfig);
}

void BoundedTrainingSession::FinishVerification()
{
    // Charge every rejection immediately, including a retryable failed Arm.
    // Rejected().size() also includes model invalidation records and is not
    // an observation counter. Never replenish this budget at a new sweep.
    const auto observed=verification->InvalidObservations();
    const auto additional=observed-verificationInvalidAccounted;
    verificationInvalidAccounted=observed;
    const auto remaining=config.maximumInvalidObservations-std::min(invalid,config.maximumInvalidObservations);
    invalid+=std::min(additional,std::numeric_limits<std::size_t>::max()-invalid);
    if(additional>remaining)
    {
        const auto& failures=verification->Rejected();
        Stop(TrainingStatus::InvalidObservation,failures.empty()?"final_verification_invalid_observation":
            "final_verification:"+failures.back().reason);return;
    }
    switch(verification->Status())
    {
    case VerificationStatus::Collecting:return;
    case VerificationStatus::Exact:Stop(TrainingStatus::Exact);return;
    case VerificationStatus::BudgetExhausted:Stop(TrainingStatus::BudgetExhausted,"final_verification_probe_budget_exhausted");return;
    case VerificationStatus::ExternalStop:Stop(TrainingStatus::ExternalStop);return;
    case VerificationStatus::InvalidObservation:
    {
        const auto& failures=verification->Rejected();
        Stop(TrainingStatus::InvalidObservation,failures.empty()?"final_verification_invalid_observation":
            "final_verification:"+failures.back().reason);return;
    }
    case VerificationStatus::OutsideTolerance:
        // A measured negative final pass is not convergence. With no adjustable
        // inputs it is unresolved; otherwise a required next sweep prevented by
        // the declared iteration budget is explicitly budget exhaustion.
        if(order.empty()){Stop(TrainingStatus::SearchUnresolved,"no_adjustable_inputs_outside_tolerance");return;}
        if(++sweep>=config.maximumSweeps){Stop(TrainingStatus::BudgetExhausted,"maximum_sweeps_exhausted");return;}
        verification.reset();status=TrainingStatus::Searching;position=0;BeginInput();return;
    }
}

std::optional<TrainingAction> BoundedTrainingSession::NextAction()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return {};
    if(armed || compositeArmed)return {};
    if(pending)return pending;
    // Do not issue another geometry/domain edit when no observation can be
    // afforded. Otherwise the terminal export contains a newly staged,
    // unmeasured candidate instead of the state at the last completed probe.
    if(probes>=config.maximumProbes){Stop(TrainingStatus::BudgetExhausted,"maximum_probes_exhausted");return {};}
    if(phase==Phase::Candidate)
    {
        // Every new search point needs a fresh anchor/input pair. An odd last
        // probe cannot measure that point, so it must not buy a domain/edit
        // that leaves an unmeasured graph in the terminal export. Repeated N0
        // measurements already in progress and final verification have their
        // existing accounting; this does not reserve or replenish a budget.
        if(config.maximumProbes-probes<2)
        {Stop(TrainingStatus::BudgetExhausted,"candidate_probe_pair_budget_exhausted");return {};}
        std::vector<InputParameters> candidate;
        auto candidateCapacitance=RestoreTrainingCapacitance(inputSearch.backgroundCapacitance,currentCapacitance);
        bool exhausted=inputSearch.candidates>=config.maximumCandidatesPerInput;
        if(inputSearch.capacitanceContexts.empty())
        {
            if(!exhausted && requireResistanceDomains)
            {
                const auto next=NextDomainCandidate(candidate);
                if(next==DynamicCandidate::NeedDomain)return pending;
                if(next==DynamicCandidate::Candidate && inputSearch.cursor.domainYieldPending)
                    inputSearch.cursor.domainPassIssuedCandidate=true;
                exhausted=next==DynamicCandidate::Exhausted;
            }
            else if(!exhausted)exhausted=!NextCandidate(candidate);
        }
        else if(!exhausted)
        {
            // One measured candidate per context turn, not an entire Ns/Rs
            // mesh at one C. Pending domains retain their issuing context.
            auto remainingContexts=std::count_if(inputSearch.capacitanceContexts.begin(),inputSearch.capacitanceContexts.end(),
                [](const auto& context){return !context.exhausted;});
            while(true)
            {
                if(!remainingContexts){exhausted=true;break;}
                SelectCapacitanceContext(inputSearch.nextCapacitanceContext);
                auto& context=inputSearch.capacitanceContexts[inputSearch.activeCapacitanceContext];
                if(context.exhausted)
                {
                    AdvanceCapacitanceContext();continue;
                }
                const auto next=requireResistanceDomains?NextDomainCandidate(candidate):
                    (NextCandidate(candidate)?DynamicCandidate::Candidate:DynamicCandidate::Exhausted);
                if(next==DynamicCandidate::NeedDomain)return pending;
                if(next==DynamicCandidate::Exhausted)
                {context.exhausted=true;--remainingContexts;AdvanceCapacitanceContext();continue;}
                candidateCapacitance=CandidateCapacitance(candidate);
                const std::string modelScope=requireResistanceDomains?
                    (inputSearch.cursor.candidateResistanceScope.empty()?revision:inputSearch.cursor.candidateResistanceScope):std::string{};
                const bool duplicate=std::any_of(inputSearch.issuedJointCandidates.begin(),inputSearch.issuedJointCandidates.end(),[&](const auto& issued) {
                    return issued.parameters==candidate && issued.modelScope==modelScope &&
                        EquivalentTrainingCapacitance(issued.capacitance,candidateCapacitance);
                });
                if(duplicate){AdvanceCapacitanceContext();continue;}
                if(requireResistanceDomains && inputSearch.cursor.domainYieldPending)inputSearch.cursor.domainPassIssuedCandidate=true;
                inputSearch.issuedJointCandidates.push_back({candidate,candidateCapacitance,modelScope});break;
            }
        }
        if(exhausted)
        {
            if(!inputSearch.best){Stop(TrainingStatus::SearchUnresolved,"no_measured_candidate_in_declared_search");return {};}
            candidate=inputSearch.best->fullParameters;inputSearch.restoringBest=true;
            candidateCapacitance=RestoreTrainingCapacitance(inputSearch.best->capacitance,currentCapacitance);
            if(!PrepareCandidate(candidate))
            {rejected.push_back({{},"best_candidate_no_longer_legal"});Stop(TrainingStatus::InvalidModel);return {};}
        }
        else ++inputSearch.candidates;
        phase=Phase::Anchor;anchor.reset();anchorTrial.clear();inputSearch.candidateMeasurements.clear();
        // A compatibility graph can have the same L/Ns/Rs but different
        // effective stimulus bindings. Realize the assessed geometry explicitly
        // before treating its normalized preview scope as a measured point.
        const bool realizePreview=requireResistanceDomains && !inputSearch.restoringBest && inputSearch.cursor.domainReady &&
            candidate==inputSearch.cursor.domainGeometry && inputSearch.cursor.domainScope!=revision;
        if(candidate!=current || candidateCapacitance!=currentCapacitance || realizePreview)
        {
            if(edits>=config.maximumEdits || epoch==std::numeric_limits<std::size_t>::max())
            {Stop(TrainingStatus::BudgetExhausted,epoch==std::numeric_limits<std::size_t>::max()?
                "model_epoch_exhausted":"maximum_edits_exhausted");return {};}
            pending=TrainingAction{TrainingActionKind::Edit,++serial,order.at(position),epoch,
                config.runId,{},revision,std::move(candidate),false};
            pending->capacitance=std::move(candidateCapacitance);return pending;
        }
    }
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
    pending->capacitance=currentCapacitance;
    return pending;
}

void BoundedTrainingSession::Applied(std::size_t actionSerial,const std::vector<InputParameters>& effective,
                                   const std::string& actualModelRevision,std::optional<TrainingCapacitance> effectiveCapacitance)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::Edit || pending->serial!=actionSerial)
        throw std::logic_error("unrequested_training_edit");
    bool capacitanceMatches=false;
    try {capacitanceMatches=TrainingCapacitanceReadbackMatches(pending->capacitance,effectiveCapacitance);}
    catch(const std::invalid_argument&) {}
    if(!capacitanceMatches)
    {rejected.push_back({{},"effective_capacitance_readback_mismatch"});Stop(TrainingStatus::InvalidModel);return;}
    if(effective!=pending->parameters || !Legal(effective) || actualModelRevision.empty() || actualModelRevision==revision)
    {rejected.push_back({{},"effective_edit_readback_mismatch"});Stop(TrainingStatus::InvalidModel);return;}
    current=effective;currentCapacitance=std::move(effectiveCapacitance);
    revision=actualModelRevision;++epoch;++edits;pending.reset();
    if(requireResistanceDomains)inputSearch.cursor.domainReady=false;
    for(auto& context:inputSearch.capacitanceContexts)context.suspended.domainReady=false;
    // Every edit invalidates all current measurements, including Branch shared
    // cable dependencies. Earlier evidence remains historical, never reused.
    anchor.reset();anchorTrial.clear();
}

void BoundedTrainingSession::Reject(const std::string& reason)
{
    rejected.push_back({pending?pending->trialId:std::string{},reason});pending.reset();armed.reset();compositeArmed.reset();
    if(config.n0CandidateRepeats>1 && phase!=Phase::Verify)
    {RecordIncompleteCandidateRepeats(reason);anchor.reset();anchorTrial.clear();phase=Phase::Anchor;}
    const bool retry=invalid<config.maximumInvalidObservations;
    if(invalid<std::numeric_limits<std::size_t>::max())++invalid;
    if(!retry)Stop(TrainingStatus::InvalidObservation);
}

void BoundedTrainingSession::DeclineEdit(std::size_t actionSerial,const std::string& reason,const std::string& unchangedModelRevision)
{
    if(status!=TrainingStatus::Searching)return;
    if(!pending || pending->kind!=TrainingActionKind::Edit || pending->serial!=actionSerial || reason.empty())
        throw std::logic_error("unrequested_training_edit_rejection");
    rejected.push_back({{},"edit_rejected:"+reason});
    if(unchangedModelRevision!=revision || unchangedModelRevision.empty() || inputSearch.restoringBest)
    {Stop(TrainingStatus::InvalidModel);return;}
    pending.reset();armed.reset();compositeArmed.reset();anchor.reset();anchorTrial.clear();phase=Phase::Candidate;
    AdvanceCapacitanceContext();
    if(requireResistanceDomains)inputSearch.cursor.domainReady=false;
    for(auto& context:inputSearch.capacitanceContexts)context.suspended.domainReady=false;
}

void BoundedTrainingSession::Arm(std::size_t actionSerial,const PreparedVerificationProbe& prepared)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(references.Protocol().schemaVersion==4 || references.Protocol().schemaVersion==5)throw std::invalid_argument("composite_measurement_requires_typed_admission");
    if(!pending || pending->kind!=TrainingActionKind::Probe || pending->serial!=actionSerial || armed)
        throw std::logic_error("unrequested_training_probe");
    ++probes;
    if(phase==Phase::Verify)
    {
        verification->Arm(prepared);
        FinishVerification();
        if(status!=TrainingStatus::Verifying)return;
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
    if(references.Protocol().schemaVersion==4 || references.Protocol().schemaVersion==5)throw std::invalid_argument("composite_measurement_requires_typed_admission");
    if(actualModelRevision!=revision || actualModelRevision.empty())
    {rejected.push_back({pending?pending->trialId:std::string{},"unexpected_training_model_revision"});Stop(TrainingStatus::InvalidModel);return;}
    if(!pending || !armed){Reject("unrequested_training_observation");return;}
    if(phase==Phase::Verify)
    {
        verification->Consume(observation,actualModelRevision);pending.reset();armed.reset();compositeArmed.reset();FinishVerification();return;
    }
    if(observation.specification.runId!=pending->runId || observation.specification.trialId!=pending->trialId ||
        !seenTrials.insert(observation.specification.trialId).second)
    {Reject("unexpected_or_replayed_training_trial");return;}
    ValidatedProbeMeasurement measurement;
    try {measurement=ValidateProbeMeasurement(references.Protocol(),pending->inputIndex,*armed,observation);}
    catch(const std::invalid_argument& error)
    {
        const bool advance=config.invalidMeasurementRecovery==InvalidMeasurementRecovery::AdvanceUnassignedBurst &&
            CanAdvanceUnassignedBurstCandidate(references.Protocol(),pending->inputIndex,*armed,observation);
        std::optional<CandidateRecoveryEvidence> recovery;
        if(advance)
        {
            CandidateRecoveryEvidence e;
            e.actionSerial=pending->serial;e.inputIndex=pending->inputIndex;e.modelEpoch=epoch;
            e.candidateInputIndex=order.at(position);e.sweep=sweep;
            e.invalidBefore=invalid;e.runId=pending->runId;e.trialId=pending->trialId;
            e.baseModelRevision=revision;e.effectiveModelRevision=armed->effectiveModelRevision;
            e.anchor=phase==Phase::Anchor;e.validity=observation.burstAssignments.front().validity;
            e.fullParameters=current;e.capacitance=currentCapacitance;recovery=std::move(e);
        }
        Reject(advance?std::string("unassigned_burst_candidate:")+
            PeakValidityName(observation.burstAssignments.front().validity):error.what());
        if(advance && status==TrainingStatus::Searching)
        {
            // The failed point is not a measured candidate. Retire any partial
            // repeats/anchor and continue the existing finite native cursor.
            // The common Reject already consumed the shared attempt budget.
            anchor.reset();anchorTrial.clear();
            if(inputSearch.restoringBest)
            {
                Stop(TrainingStatus::SearchUnresolved,"restored_best_candidate_has_unassigned_burst");
                recovery->disposition=CandidateRecoveryDisposition::RestoredBestStop;
            }
            else
            {
                phase=Phase::Candidate;AdvanceCapacitanceContext();
                recovery->disposition=CandidateRecoveryDisposition::CursorAdvanced;
            }
        }
        if(recovery)
        {
            recovery->invalidAfter=invalid;recovery->statusAfter=status;recovery->reasonAfter=stopReason;
            recoveries.push_back(std::move(*recovery));
        }
        return;
    }
    AcceptValidatedScalar(measurement);
}

void BoundedTrainingSession::ArmComposite(std::size_t actionSerial,const PreparedCompositeMeasurement& prepared)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(references.Protocol().schemaVersion==5)throw std::invalid_argument("candidate_window_training_requires_proposal_admission");
    ArmTyped(actionSerial,prepared,nullptr);
}
void BoundedTrainingSession::ArmCandidateWindow(std::size_t serial,const PreparedCompositeMeasurement& prepared) {
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(references.Protocol().schemaVersion!=5 || !candidateWindowReferences)
        throw std::invalid_argument("candidate_window_closed_reference_admission_required");
    ArmTyped(serial,prepared,&*candidateWindowReferences);
}
void BoundedTrainingSession::ArmTyped(std::size_t actionSerial,const PreparedCompositeMeasurement& prepared,const CandidateWindowReferenceSet* closed)
{
    if(!pending || pending->kind!=TrainingActionKind::Probe || pending->serial!=actionSerial || armed || compositeArmed)
        throw std::logic_error("unrequested_training_probe");
    ++probes;
    try {
        if(!TrainingCapacitanceReadbackMatches(currentCapacitance,prepared.identity->capacitance))
            throw std::invalid_argument("composite_training_capacitance_readback_mismatch");
    }
    catch(const CompositeTrainingBudgetExhausted&){ExhaustBudget();return;}
    catch(const CompositeTrainingStopped&){ExternalStop();return;}
    catch(const std::exception& error){if(closed)RejectCandidateWindow(error.what());else RejectComposite(error.what());return;}
    if(phase==Phase::Verify) {
        if(closed)verification->ArmCandidateWindow(prepared);else verification->ArmComposite(prepared);FinishVerification();
        if(status!=TrainingStatus::Verifying)return;
        if(!verification->WaitingForObservation()){pending.reset();return;}
    }
    try {
        if(closed)prepared.RequireCandidateWindow(references.Protocol(),*closed,pending->runId,pending->trialId,pending->inputIndex,revision,epoch,
            phase==Phase::Verify?CompositeMeasurementPurpose::Final:CompositeMeasurementPurpose::Search);
        else prepared.Require(references.Protocol(),pending->runId,pending->trialId,pending->inputIndex,revision,epoch,
            phase==Phase::Verify?CompositeMeasurementPurpose::Final:CompositeMeasurementPurpose::Search,
            references.CompositeOwnerScope(),references.CompositeSourceOrigin());
        if(!seenCompositeGroups.insert(prepared.GroupId()).second)throw std::invalid_argument("replayed_composite_training_group");
        compositeArmed=prepared;
    }
    catch(const CompositeTrainingBudgetExhausted&){ExhaustBudget();return;}
    catch(const CompositeTrainingStopped&){ExternalStop();return;}
    catch(const std::exception& error){Reject(error.what());}
}
void BoundedTrainingSession::ConsumeComposite(const ValidatedCompositeMeasurement& measured,const std::string& actualRevision)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(references.Protocol().schemaVersion==5)throw std::invalid_argument("candidate_window_training_requires_proposal_admission");
    ConsumeTyped(measured,actualRevision,nullptr);
}
void BoundedTrainingSession::ConsumeCandidateWindow(const ValidatedCompositeMeasurement& measured,const std::string& actualRevision) {
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(references.Protocol().schemaVersion!=5 || !candidateWindowReferences)
        throw std::invalid_argument("candidate_window_closed_reference_admission_required");
    ConsumeTyped(measured,actualRevision,&*candidateWindowReferences);
}
void BoundedTrainingSession::ConsumeTyped(const ValidatedCompositeMeasurement& measured,const std::string& actualRevision,const CandidateWindowReferenceSet* closed)
{
    if(actualRevision!=revision || actualRevision.empty()) {
        rejected.push_back({pending?pending->trialId:std::string{},"unexpected_training_model_revision"});Stop(TrainingStatus::InvalidModel);return;
    }
    if(!pending || !compositeArmed){Reject("unrequested_composite_training_observation");return;}
    try {
        if(!TrainingCapacitanceReadbackMatches(currentCapacitance,measured.prepared.identity->capacitance))
            throw std::invalid_argument("composite_training_capacitance_readback_mismatch");
    }
    catch(const CompositeTrainingBudgetExhausted&){ExhaustBudget();return;}
    catch(const CompositeTrainingStopped&){ExternalStop();return;}
    catch(const std::exception& error){if(closed)RejectCandidateWindow(error.what());else RejectComposite(error.what());return;}
    if(phase==Phase::Verify) {
        const auto before=verification->AcceptedCompositeObservations();
        if(closed)verification->ConsumeCandidateWindow(measured,actualRevision);else verification->ConsumeComposite(measured,actualRevision);
        acceptedComposite+=verification->AcceptedCompositeObservations()-before;
        pending.reset();compositeArmed.reset();FinishVerification();return;
    }
    try {
        if(closed)measured.prepared.RequireCandidateWindow(references.Protocol(),*closed,pending->runId,pending->trialId,pending->inputIndex,revision,epoch,
            CompositeMeasurementPurpose::Search,true);
        else measured.prepared.Require(references.Protocol(),pending->runId,pending->trialId,pending->inputIndex,revision,epoch,
            CompositeMeasurementPurpose::Search,references.CompositeOwnerScope(),references.CompositeSourceOrigin(),true);
        measured.RequirePrepared(*compositeArmed);
        if(!seenTrials.insert(pending->trialId).second || !seenCompositeProofs.insert(measured.semanticProofId).second)
            throw std::invalid_argument("unexpected_or_replayed_composite_training_proof");
        if(phase==Phase::Anchor)compositeAnchorWitness=measured.prepared.identity->candidateWitnessId;
        else if(compositeAnchorWitness!=measured.prepared.identity->candidateWitnessId)
            throw std::invalid_argument("mixed_composite_anchor_candidate_witness");
    }
    catch(const CompositeTrainingBudgetExhausted&){ExhaustBudget();return;}
    catch(const CompositeTrainingStopped&){ExternalStop();return;}
    catch(const std::exception& error){Reject(error.what());return;}
    const auto before=evidence.size();const bool wasAnchor=phase==Phase::Anchor;
    AcceptValidatedScalar({measured.rawPotential,measured.arrival,measured.plateauEnd});
    if((wasAnchor && anchor && anchorTrial==measured.prepared.LogicalTrialId()) || evidence.size()>before)++acceptedComposite;
}
void BoundedTrainingSession::RejectComposite(const std::string& reason)
{
    if(references.Protocol().schemaVersion!=4 || reason.empty())throw std::invalid_argument("invalid_composite_rejection");
    RejectTyped(reason,nullptr);
}
void BoundedTrainingSession::RejectCandidateWindow(const std::string& reason) {
    if(references.Protocol().schemaVersion!=5 || !candidateWindowReferences || reason.empty())throw std::invalid_argument("invalid_candidate_window_rejection");
    RejectTyped(reason,&*candidateWindowReferences);
}
void BoundedTrainingSession::RejectCandidateWindow(const RejectedCompositeMeasurement& failed,const std::string& actualRevision) {
    if(references.Protocol().schemaVersion!=5 || !candidateWindowReferences ||
        config.invalidMeasurementRecovery!=InvalidMeasurementRecovery::AdvanceRejectedCandidateWindow)
        throw std::invalid_argument("typed_candidate_window_recovery_policy_required");
    if(status!=TrainingStatus::Searching || phase==Phase::Verify)
        throw std::invalid_argument("candidate_window_recovery_requires_active_search");
    // A second delivery must not consume another invalid/probe allowance or
    // move the cursor, even while the original native group remains current.
    if(seenRejectedCompositeProofs.count(failed.ContentId()))
        throw std::invalid_argument("replayed_candidate_window_rejection");
    if(!pending || pending->kind!=TrainingActionKind::Probe || !compositeArmed)
        throw std::logic_error("unrequested_candidate_window_rejection");
    if(actualRevision.empty() || actualRevision!=revision) {
        rejected.push_back({pending->trialId,"unexpected_training_model_revision"});
        Stop(TrainingStatus::InvalidModel);return;
    }
    try {
        failed.RequirePrepared(*compositeArmed);
        failed.Prepared().RequireCandidateWindow(references.Protocol(),*candidateWindowReferences,
            pending->runId,pending->trialId,pending->inputIndex,revision,epoch,CompositeMeasurementPurpose::Search,true);
    }
    catch(const CompositeTrainingBudgetExhausted&){ExhaustBudget();return;}
    catch(const CompositeTrainingStopped&){ExternalStop();return;}
    if(seenTrials.count(pending->trialId) || seenCompositeProofs.count(failed.ContentId()))
        throw std::invalid_argument("unexpected_or_replayed_candidate_window_rejection");
    CandidateWindowRecoveryEvidence event;auto& e=event.transition;
    e.actionSerial=pending->serial;e.inputIndex=pending->inputIndex;e.candidateInputIndex=order.at(position);
    e.sweep=sweep;e.modelEpoch=epoch;e.invalidBefore=invalid;e.runId=pending->runId;e.trialId=pending->trialId;
    e.baseModelRevision=revision;e.effectiveModelRevision=failed.Prepared().identity->effectiveRevision;
    e.anchor=phase==Phase::Anchor;
    e.validity=failed.Validity()==DeclaredWindowBasinValidity::Missing?PeakValidity::Missing:PeakValidity::Ambiguous;
    e.fullParameters=current;e.capacitance=currentCapacitance;
    event.groupId=failed.Prepared().GroupId();event.rejectedContentId=failed.ContentId();
    event.sourceOrigin=failed.Prepared().identity->sourceOrigin;event.ownerScope=failed.Prepared().identity->ownerScope;
    event.candidateWitnessId=failed.Prepared().identity->candidateWitnessId;
    event.issuedProbes=probes;event.appliedEdits=edits;event.acceptedCompositeObservations=acceptedComposite;
    seenTrials.insert(pending->trialId);seenRejectedCompositeProofs.insert(failed.ContentId());
    const auto reason=std::string("rejected_candidate_window:")+PeakValidityName(e.validity);
    RecordIncompleteCandidateRepeats(reason);
    // ArmCandidateWindow charged the native logical probe. Reject charges the
    // same attempt-wide invalid budget used by every other observation path.
    Reject(reason);
    anchor.reset();anchorTrial.clear();compositeAnchorWitness.clear();
    if(status==TrainingStatus::Searching) {
        if(inputSearch.restoringBest) {
            Stop(TrainingStatus::SearchUnresolved,"restored_best_candidate_has_rejected_window");
            e.disposition=CandidateRecoveryDisposition::RestoredBestStop;
        } else {
            if(!PrioritizeN0LengthNeighborhood())PrioritizeRejectedN0BranchLength();
            PrioritizeRejectedN1();
            phase=Phase::Candidate;AdvanceCapacitanceContext();
            e.disposition=CandidateRecoveryDisposition::CursorAdvanced;
        }
    }
    e.invalidAfter=invalid;e.statusAfter=status;e.reasonAfter=stopReason;
    candidateWindowRecoveries.push_back(std::move(event));
}
void BoundedTrainingSession::RejectTyped(const std::string& reason,const CandidateWindowReferenceSet* closed)
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(phase==Phase::Verify) {
        if(closed)verification->RejectCandidateWindow(reason);else verification->RejectComposite(reason);
        pending.reset();compositeArmed.reset();FinishVerification();return;
    }
    Reject(reason);
}

void BoundedTrainingSession::AcceptValidatedScalar(const ValidatedProbeMeasurement& measurement)
{
    const auto trial=pending->trialId;pending.reset();armed.reset();compositeArmed.reset();
    if(phase==Phase::Anchor){anchor=measurement;anchorTrial=trial;phase=Phase::Input;return;}
    if(!anchor)throw std::logic_error("missing_same_revision_anchor");
    const auto index=order.at(position);const auto& reference=references.Inputs()[index];
    CandidateEvidence e;e.inputIndex=index;e.modelEpoch=epoch;e.parameters=current[index];
    e.fullParameters=current;
    e.capacitance=currentCapacitance;
    e.modelRevision=revision;e.trialId=trial;e.anchorTrialId=anchorTrial;
    e.timingResidualLow=measurement.arrival-anchor->plateauEnd;
    e.timingResidualHigh=measurement.plateauEnd-anchor->arrival;
    e.amplitudeResidual=measurement.rawPotential-reference.rawPotential;
    e.measuredRawPotential=measurement.rawPotential;
    for(std::size_t i=0;i<current.size();++i)
        e.parameterChangeCost+=std::fabs(double(current[i].length)-inputSearch.background[i].length)+
            std::fabs(double(current[i].synapses)-inputSearch.background[i].synapses)+
            std::fabs(std::log(current[i].resistance)-std::log(inputSearch.background[i].resistance));
    if(currentCapacitance)
        for(const auto& [path,scale]:currentCapacitance->membraneScales)
        {
            const auto before=inputSearch.backgroundCapacitance->membraneScales.find(path);
            const double previous=before==inputSearch.backgroundCapacitance->membraneScales.end()?1.:before->second;
            e.parameterChangeCost+=std::fabs(std::log(scale)-std::log(previous));
        }
    if(!Finite(e.timingResidualLow) || !Finite(e.timingResidualHigh) || !Finite(e.amplitudeResidual))
    {Reject("nonfinite_training_residual");return;}
    e.withinTiming=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh))<=config.tolerances.sync;
    e.withinAmplitude=std::fabs(e.amplitudeResidual)<=AmplitudeTolerance(config,reference);
    evidence.push_back(e);
    if(requireResistanceDomains && !inputSearch.restoringBest && !inputSearch.cursor.candidateResistanceScope.empty())
    {
        inputSearch.cursor.resistanceMeasurements.push_back({inputSearch.cursor.candidateResistanceScope,e});
        if(e.withinTiming && !e.withinAmplitude && inputSearch.cursor.domainYieldPending && !inputSearch.cursor.domainLocalTurnUsed)
        {
            // Enough local points can establish/refine a measured bracket,
            // while reserving most remaining work for other geometries. Probe
            // allowance counts fresh anchor/input pairs, not raw native waves.
            inputSearch.cursor.domainLocalTurnUsed=true;
            inputSearch.cursor.domainLocalTurnsRemaining=std::min({std::size_t(12),
                (config.maximumCandidatesPerInput-inputSearch.candidates)/4,
                (config.maximumProbes-probes)/8,(config.maximumEdits-edits)/4});
        }
        if(inputSearch.cursor.domainYieldPending && inputSearch.cursor.domainLocalTurnsRemaining && !(e.withinTiming && e.withinAmplitude))
        {
            // A valid endpoint can lose timing; it is not a certificate or a
            // reason to discard the remaining finite mesh. Invalid observations
            // never replenish this turn. Every candidate gets a fresh domain,
            // edit readback and anchor/input measurement through the usual path.
            --inputSearch.cursor.domainLocalTurnsRemaining;inputSearch.cursor.domainYieldPending=false;inputSearch.cursor.domainReady=false;
        }
    }
    if(config.normalization==Normalization::SynapseCount && config.n0CandidateRepeats>1)
    {
        inputSearch.candidateMeasurements.push_back(e);
        if(inputSearch.candidateMeasurements.size()<config.n0CandidateRepeats)
        {phase=Phase::Anchor;anchor.reset();anchorTrial.clear();return;}
        CandidateRepeatEvidence group;group.inputIndex=index;group.modelEpoch=epoch;
        group.modelRevision=revision;group.fullParameters=current;
        group.capacitance=currentCapacitance;
        group.minimumRawPotential=group.maximumRawPotential=e.measuredRawPotential;
        group.withinTiming=group.withinAmplitude=true;
        for(const auto& sample:inputSearch.candidateMeasurements)
        {
            if(sample.inputIndex!=index || sample.modelEpoch!=epoch || sample.modelRevision!=revision || sample.fullParameters!=current ||
                sample.capacitance!=currentCapacitance)
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
        candidateRepeats.push_back(std::move(group));inputSearch.candidateMeasurements.clear();
    }
    PrioritizeMeasuredLength(e);
    const bool improved=!inputSearch.best || Rank(e,config,reference)<Rank(*inputSearch.best,config,reference);
    if(improved)
    {
        const auto previous=selectedMeasuredBoundary?selectedMeasuredBoundary->serial:0;
        if(previous==std::numeric_limits<std::size_t>::max())
        {Stop(TrainingStatus::InvalidModel,"measured_boundary_selection_serial_exhausted");return;}
        MeasuredCandidateSelection selected{previous+1,sweep,position,e};
        inputSearch.best=e;selectedMeasuredBoundary=std::move(selected);
    }
    // A different timing-ready geometry may start far from the amplitude
    // target. Its local search must not be gated by another geometry's best.
    // Global ranking and the independent final-verification gate are unchanged.
    PrioritizeN0Neighbors(e);
    if(inputSearch.restoringBest || (e.withinTiming && e.withinAmplitude))AdvanceInput();
    else {phase=Phase::Candidate;anchor.reset();anchorTrial.clear();AdvanceCapacitanceContext();}
}

void BoundedTrainingSession::RecordIncompleteCandidateRepeats(const std::string& reason)
{
    if(inputSearch.candidateMeasurements.empty())return;
    const auto& first=inputSearch.candidateMeasurements.front();
    CandidateRepeatEvidence group;group.inputIndex=first.inputIndex;group.modelEpoch=first.modelEpoch;
    group.modelRevision=first.modelRevision;group.fullParameters=first.fullParameters;
    group.capacitance=first.capacitance;
    group.minimumRawPotential=group.maximumRawPotential=first.measuredRawPotential;
    group.withinTiming=group.withinAmplitude=true;group.complete=false;group.reason=reason;
    for(const auto& sample:inputSearch.candidateMeasurements)
    {
        group.trialIds.push_back(sample.trialId);group.anchorTrialIds.push_back(sample.anchorTrialId);
        group.rawPotentials.push_back(sample.measuredRawPotential);
        group.minimumRawPotential=std::min(group.minimumRawPotential,sample.measuredRawPotential);
        group.maximumRawPotential=std::max(group.maximumRawPotential,sample.measuredRawPotential);
        group.withinTiming=group.withinTiming && sample.withinTiming;
        group.withinAmplitude=group.withinAmplitude && sample.withinAmplitude;
    }
    candidateRepeats.push_back(std::move(group));inputSearch.candidateMeasurements.clear();
}

void BoundedTrainingSession::Stop(TrainingStatus terminal,const std::string& diagnostic)
{
    stopReason=diagnostic.empty()?TrainingStatusName(terminal):diagnostic;
    if(diagnostic.empty() && (terminal==TrainingStatus::InvalidModel || terminal==TrainingStatus::InvalidObservation) && !rejected.empty())
        stopReason=rejected.back().reason;
    RecordIncompleteCandidateRepeats(stopReason);status=terminal;pending.reset();armed.reset();compositeArmed.reset();anchor.reset();
}

void BoundedTrainingSession::ExternalStop()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(verification){verification->ExternalStop();}
    Stop(TrainingStatus::ExternalStop);
}

void BoundedTrainingSession::ExhaustBudget()
{
    if(status!=TrainingStatus::Searching && status!=TrainingStatus::Verifying)return;
    if(verification){verification->ExhaustBudget();}
    Stop(TrainingStatus::BudgetExhausted,"adapter_execution_budget_exhausted");
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
