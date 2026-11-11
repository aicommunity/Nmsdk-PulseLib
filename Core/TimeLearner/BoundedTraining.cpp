#include "BoundedTraining.h"
#include "BoundedTrainingValidation.h"
#include "CandidateWindowCoreValidation.h"
#include "NumericChecks.h"
#include "MeasuredResistanceSearch.h"
#include "MeasuredLengthSearch.h"
#include "MeasuredCapacitanceSearch.h"
#include "CapacitanceRegion.h"
#include <algorithm>
#include <cmath>
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
    if(policy!=InvalidMeasurementRecovery::RetrySameCandidate && policy!=InvalidMeasurementRecovery::AdvanceUnassignedBurst)
        throw std::invalid_argument("invalid_candidate_recovery_policy");
    if(policy==InvalidMeasurementRecovery::AdvanceUnassignedBurst && invalidAllowance==0)
        throw std::invalid_argument("candidate_recovery_requires_invalid_observation_budget");
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
    if(config.invalidMeasurementRecovery!=InvalidMeasurementRecovery::RetrySameCandidate)
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
    capacitanceContexts.clear();activeCapacitanceContext=nextCapacitanceContext=0;issuedJointCandidates.clear();
    const auto index=order.at(position);const auto& b=config.bounds[index];const auto& p=current[index];
    background=current;cursor.lengths.clear();cursor.counts.clear();cursor.resistances.clear();best.reset();anchor.reset();anchorTrial.clear();
    backgroundCapacitance=currentCapacitance;
    candidateMeasurements.clear();cursor.lengthMeasurements.clear();cursor.n0Priority.clear();cursor.issuedN0Candidates.clear();
    cursor.lengthIndex=cursor.normalizerIndex=candidates=0;restoringBest=false;phase=Phase::Candidate;
    cursor.initialCandidatePending=requireResistanceDomains;cursor.domainReady=false;cursor.domainLength=cursor.domainNormalizer=cursor.domainVariant=0;
    cursor.domainYieldPending=cursor.domainPassIssuedCandidate=false;
    cursor.domainGeometry.clear();cursor.domainScope.clear();cursor.visitedDomainCandidates.clear();
    cursor.candidateResistanceScope.clear();cursor.resistanceMeasurements.clear();
    // Nearest integers first. Enumeration is bounded by the declared candidate
    // budget, even for a very large physical length range.
    UniquePush(cursor.lengths,p.length);
    for(long long distance=1;cursor.lengths.size()<config.maximumCandidatesPerInput;++distance)
    {
        const long long up=static_cast<long long>(p.length)+distance,down=static_cast<long long>(p.length)-distance;
        if(up>b.maximumLength && down<b.minimumLength)break;
        if(up<=b.maximumLength)UniquePush(cursor.lengths,int(up));
        if(down>=b.minimumLength && cursor.lengths.size()<config.maximumCandidatesPerInput)UniquePush(cursor.lengths,int(down));
    }
    if(config.normalization==Normalization::SynapseCount)
    {
        UniquePush(cursor.counts,p.synapses);
        for(int distance=1;cursor.counts.size()<std::size_t(b.maximumSynapses-b.minimumSynapses+1);++distance)
        {
            if(p.synapses+distance<=b.maximumSynapses)UniquePush(cursor.counts,p.synapses+distance);
            if(p.synapses-distance>=b.minimumSynapses)UniquePush(cursor.counts,p.synapses-distance);
        }
    }
    else if(!requireResistanceDomains)
    {
        UniquePush(cursor.resistances,p.resistance);
        UniquePush(cursor.resistances,b.minimumResistance);UniquePush(cursor.resistances,b.maximumResistance);
        for(unsigned level=1;level<=config.resistanceSubdivisionDepth && cursor.resistances.size()<config.maximumCandidatesPerInput;++level)
        {
            const std::size_t denominator=std::size_t(1)<<level;
            for(std::size_t numerator=1;numerator<denominator && cursor.resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
            {
                const double fraction=double(numerator)/double(denominator);
                const double value=std::exp(std::log(b.minimumResistance)+fraction*(std::log(b.maximumResistance)-std::log(b.minimumResistance)));
                if(Finite(value) && value>=b.minimumResistance && value<=b.maximumResistance)UniquePush(cursor.resistances,value);
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
            possible.first=std::int64_t(background[index+1].length)+1;
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
            if(!RegionScaleCanChange(*backgroundCapacitance,possible,scale))continue;
            if(scales.size()==policy.maximumContexts-1)break;
            scales.push_back(scale);
        }
        const auto reserve=policy.maximumCursorElements/(1+scales.size());
        if(cursor.lengths.size()>reserve || cursor.counts.size()>reserve-cursor.lengths.size() ||
            cursor.resistances.size()>reserve-cursor.lengths.size()-cursor.counts.size())
            throw std::invalid_argument("capacitance_cursor_inventory_exceeds_declared_budget");
        capacitanceContexts.push_back({}); // Preserve the unchanged full map.
        for(double scale:scales)
        {
            auto next=cursor;
            // A changed C requires a fresh N1 domain; it cannot use the
            // current-graph initial-candidate shortcut.
            if(requireResistanceDomains)next.initialCandidatePending=false;
            capacitanceContexts.push_back({std::move(next),scale,false});
        }
    }
}

void BoundedTrainingSession::SelectCapacitanceContext(std::size_t index)
{
    if(capacitanceContexts.empty() || index==activeCapacitanceContext)return;
    std::swap(cursor,capacitanceContexts.at(activeCapacitanceContext).suspended);
    std::swap(cursor,capacitanceContexts.at(index).suspended);
    activeCapacitanceContext=index;
}
void BoundedTrainingSession::AdvanceCapacitanceContext()
{
    if(!capacitanceContexts.empty())nextCapacitanceContext=(activeCapacitanceContext+1)%capacitanceContexts.size();
}
std::optional<TrainingCapacitance> BoundedTrainingSession::CandidateCapacitance(
    const std::vector<InputParameters>& parameters) const
{
    auto result=RestoreTrainingCapacitance(backgroundCapacitance,currentCapacitance);
    if(capacitanceContexts.empty() || !capacitanceContexts.at(activeCapacitanceContext).absoluteScale)return result;
    const auto region=SelectCapacitanceRegion(references.Protocol().routing,parameters,order.at(position));
    return MakeCapacitanceRegionRequest(*result,region,*capacitanceContexts[activeCapacitanceContext].absoluteScale,
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
            if(std::find(cursor.issuedN0Candidates.begin(),cursor.issuedN0Candidates.end(),candidate)!=cursor.issuedN0Candidates.end())return false;
            cursor.issuedN0Candidates.push_back(candidate);
        }
        return true;
    };
    // These are proposals from completed measurements, never assumed solutions.
    // Required repeats occur inside Consume and bypass candidate deduplication.
    while(!cursor.n0Priority.empty())
    {
        parameters=std::move(cursor.n0Priority.front());cursor.n0Priority.erase(cursor.n0Priority.begin());
        if(issue(parameters))return true;
    }
    const std::size_t normalizers=n0?cursor.counts.size():cursor.resistances.size();
    // First evaluate the current normalizer at each length; then the next
    // normalizer at each length. No monotonicity or bracket is inferred.
    while(cursor.normalizerIndex<normalizers)
    {
        if(cursor.lengthIndex==cursor.lengths.size()){cursor.lengthIndex=0;++cursor.normalizerIndex;continue;}
        parameters=background;parameters[index].length=cursor.lengths[cursor.lengthIndex++];
        if(config.normalization==Normalization::SynapseCount)parameters[index].synapses=cursor.counts[cursor.normalizerIndex];
        else parameters[index].resistance=cursor.resistances[cursor.normalizerIndex];
        if(issue(parameters))return true;
    }
    return false;
}

void BoundedTrainingSession::AdvanceDomainGeometry()
{cursor.domainReady=false;if(++cursor.domainVariant==2){cursor.domainVariant=0;++cursor.domainLength;}}

void BoundedTrainingSession::PrioritizeMeasuredLength(const CandidateEvidence& measurement)
{
    if(restoringBest)return;
    cursor.lengthMeasurements.push_back(measurement);
    const auto input=measurement.inputIndex;
    std::vector<MeasuredLengthPoint> points;
    for(const auto& previous:cursor.lengthMeasurements)
    {
        if(previous.inputIndex!=input || previous.fullParameters.size()!=measurement.fullParameters.size() ||
            !EquivalentTrainingCapacitance(previous.capacitance,measurement.capacitance))continue;
        auto geometry=previous.fullParameters;
        geometry[input].length=measurement.parameters.length;
        if(geometry!=measurement.fullParameters)continue;
        points.push_back({previous.parameters.length,previous.timingResidualLow,previous.timingResidualHigh});
    }
    const auto& bound=config.bounds[input];
    const auto proposal=ProposeMeasuredLength(std::move(points),bound.minimumLength,bound.maximumLength,config.tolerances.sync);
    if(!proposal)return;
    auto candidate=measurement.fullParameters;candidate[input].length=*proposal;
    if(!PrepareCandidate(candidate))return;
    // Do not interrupt an assessed N1 geometry or bypass its domain checks.
    // The next geometry is still assessed/applied through the native adapter.
    // N0 uses its existing full-vector deduplication and repeated probes.
    const std::size_t next=requireResistanceDomains?cursor.domainLength+1:cursor.lengthIndex;
    if(next>cursor.lengths.size() || std::find(cursor.lengths.begin(),cursor.lengths.begin()+next,*proposal)!=cursor.lengths.begin()+next)return;
    cursor.lengths.erase(std::remove(cursor.lengths.begin()+next,cursor.lengths.end(),*proposal),cursor.lengths.end());
    cursor.lengths.insert(cursor.lengths.begin()+next,*proposal);
}

BoundedTrainingSession::DynamicCandidate BoundedTrainingSession::NextDomainCandidate(std::vector<InputParameters>& parameters)
{
    const auto input=order.at(position);
    // A valid observation's search order must not depend on how an invalid
    // burst would be recovered. Give each legal geometry one unissued Rs per
    // pass: a dense Rs mesh at a mistimed length must not starve other lengths.
    // Every revisit still requires a fresh native domain assessment below.
    const bool interleave=config.normalization==Normalization::TipResistance;
    if(cursor.initialCandidatePending)
    {
        cursor.initialCandidatePending=false;parameters=background;
        cursor.candidateResistanceScope.clear();
        cursor.visitedDomainCandidates.push_back({revision,parameters});return DynamicCandidate::Candidate;
    }
    if(interleave && cursor.domainYieldPending)
    {
        cursor.domainYieldPending=false;AdvanceDomainGeometry();
    }
    for(;;)
    {
        if(cursor.domainLength==cursor.lengths.size())
        {
            if(!interleave || !cursor.domainPassIssuedCandidate)return DynamicCandidate::Exhausted;
            // Revisit every geometry for its next unissued Rs. Reassessment
            // stays mandatory: an old interval/scope is never a live bound.
            // A pass without an issued candidate proves only cursor exhaustion.
            cursor.domainLength=cursor.domainVariant=0;cursor.domainReady=false;
            cursor.domainPassIssuedCandidate=false;
        }
        if(!cursor.domainReady)
        {
            auto geometry=background;geometry[input].length=cursor.lengths[cursor.domainLength];
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
            if(cursor.domainVariant==1)
            {
                if(!branch || locked || input==0 || geometry[input].length==std::numeric_limits<int>::max())
                {AdvanceDomainGeometry();continue;}
                geometry[input-1].length=std::max(geometry[input-1].length,geometry[input].length+1);
                if(!PrepareCandidate(geometry) || geometry==ordinary){AdvanceDomainGeometry();continue;}
            }
            pending=TrainingAction{TrainingActionKind::ResistanceDomain,++serial,input,epoch,
                config.runId,{},revision,std::move(geometry),false};
            pending->capacitance=CandidateCapacitance(pending->parameters);
            return DynamicCandidate::NeedDomain;
        }
        while(cursor.domainNormalizer<cursor.resistances.size())
        {
            parameters=cursor.domainGeometry;parameters[input].resistance=cursor.resistances[cursor.domainNormalizer++];
            if(!PrepareCandidate(parameters))continue;
            const auto seen=std::find_if(cursor.visitedDomainCandidates.begin(),cursor.visitedDomainCandidates.end(),[&](const auto& item) {
                return item.first==cursor.domainScope && item.second==parameters;
            });
            if(seen!=cursor.visitedDomainCandidates.end())continue;
            cursor.visitedDomainCandidates.push_back({cursor.domainScope,parameters});
            if(interleave)cursor.domainYieldPending=cursor.domainPassIssuedCandidate=true;
            cursor.candidateResistanceScope=cursor.domainScope;return DynamicCandidate::Candidate;
        }
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
    {rejected.push_back({{},"resistance_domain_unavailable:"+assessment.reason});pending.reset();AdvanceDomainGeometry();AdvanceCapacitanceContext();return;}
    if(assessment.status!=ResistanceDomainStatus::Available || assessment.previewModelRevision.empty())
    {rejected.push_back({{},"resistance_domain_unsupported:"+assessment.reason});Stop(TrainingStatus::InvalidModel);return;}
    const auto& bound=config.bounds.at(pending->inputIndex);
    const double low=std::max(bound.minimumResistance,assessment.minimumResistance);
    const double high=std::min(bound.maximumResistance,assessment.maximumResistance);
    if(!Finite(assessment.minimumResistance) || assessment.minimumResistance<=0. || !Finite(assessment.maximumResistance) ||
        assessment.maximumResistance<assessment.minimumResistance)
    {rejected.push_back({{},"invalid_resistance_domain_interval"});Stop(TrainingStatus::InvalidModel);return;}
    if(low>high)
    {rejected.push_back({{},"resistance_domain_outside_declared_box"});pending.reset();AdvanceDomainGeometry();AdvanceCapacitanceContext();return;}
    cursor.domainGeometry=pending->parameters;cursor.domainScope=assessment.previewModelRevision;cursor.resistances.clear();
    const auto canAppend=[&] {
        if(capacitanceContexts.empty())return true;
        const auto reserve=config.capacitanceSearch->maximumCursorElements/capacitanceContexts.size();
        return cursor.lengths.size()+cursor.counts.size()+cursor.resistances.size()<reserve;
    };
    const auto add=[&](double value) {
        if(std::find(cursor.resistances.begin(),cursor.resistances.end(),value)!=cursor.resistances.end())return true;
        if(!canAppend()){Stop(TrainingStatus::BudgetExhausted,"capacitance_cursor_inventory_budget_exhausted");return false;}
        UniquePush(cursor.resistances,value);return true;
    };
    const double original=background[pending->inputIndex].resistance;
    if(original>=low && original<=high && !add(original))return;
    if(!add(low) || !add(high))return;
    for(unsigned level=1;level<=config.resistanceSubdivisionDepth && cursor.resistances.size()<config.maximumCandidatesPerInput;++level)
    {
        const std::size_t denominator=std::size_t(1)<<level;
        for(std::size_t numerator=1;numerator<denominator && cursor.resistances.size()<config.maximumCandidatesPerInput;numerator+=2)
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
        for(const auto& [scope,measurement]:cursor.resistanceMeasurements)
        {
            if(scope!=cursor.domainScope || measurement.fullParameters.size()!=cursor.domainGeometry.size() ||
                !EquivalentTrainingCapacitance(measurement.capacitance,pending->capacitance))continue;
            auto geometry=measurement.fullParameters;
            geometry[pending->inputIndex].resistance=cursor.domainGeometry[pending->inputIndex].resistance;
            if(geometry!=cursor.domainGeometry)continue;
            points.push_back({measurement.parameters.resistance,measurement.amplitudeResidual,measurement.withinTiming});
        }
        if(const auto refined=ProposeMeasuredResistanceRefinement(points,low,high,
                AmplitudeTolerance(config,references.Inputs().at(pending->inputIndex))))
        {
            cursor.resistances.erase(std::remove(cursor.resistances.begin(),cursor.resistances.end(),*refined),cursor.resistances.end());
            if(!canAppend()){Stop(TrainingStatus::BudgetExhausted,"capacitance_cursor_inventory_budget_exhausted");return;}
            cursor.resistances.insert(cursor.resistances.begin(),*refined);
        }
    }
    cursor.domainNormalizer=0;cursor.domainReady=true;pending.reset();
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
    if(phase==Phase::Candidate)
    {
        std::vector<InputParameters> candidate;
        auto candidateCapacitance=RestoreTrainingCapacitance(backgroundCapacitance,currentCapacitance);
        bool exhausted=candidates>=config.maximumCandidatesPerInput;
        if(capacitanceContexts.empty())
        {
            if(!exhausted && requireResistanceDomains)
            {
                const auto next=NextDomainCandidate(candidate);
                if(next==DynamicCandidate::NeedDomain)return pending;
                exhausted=next==DynamicCandidate::Exhausted;
            }
            else if(!exhausted)exhausted=!NextCandidate(candidate);
        }
        else if(!exhausted)
        {
            // One measured candidate per context turn, not an entire Ns/Rs
            // mesh at one C. Pending domains retain their issuing context.
            auto remainingContexts=std::count_if(capacitanceContexts.begin(),capacitanceContexts.end(),
                [](const auto& context){return !context.exhausted;});
            while(true)
            {
                if(!remainingContexts){exhausted=true;break;}
                SelectCapacitanceContext(nextCapacitanceContext);
                auto& context=capacitanceContexts[activeCapacitanceContext];
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
                const bool duplicate=std::any_of(issuedJointCandidates.begin(),issuedJointCandidates.end(),[&](const auto& issued) {
                    return issued.first==candidate && EquivalentTrainingCapacitance(issued.second,candidateCapacitance);
                });
                if(duplicate){AdvanceCapacitanceContext();continue;}
                issuedJointCandidates.push_back({candidate,candidateCapacitance});break;
            }
        }
        if(exhausted)
        {
            if(!best){Stop(TrainingStatus::SearchUnresolved,"no_measured_candidate_in_declared_search");return {};}
            candidate=best->fullParameters;restoringBest=true;
            candidateCapacitance=RestoreTrainingCapacitance(best->capacitance,currentCapacitance);
            if(!PrepareCandidate(candidate))
            {rejected.push_back({{},"best_candidate_no_longer_legal"});Stop(TrainingStatus::InvalidModel);return {};}
        }
        else ++candidates;
        phase=Phase::Anchor;anchor.reset();anchorTrial.clear();candidateMeasurements.clear();
        // A compatibility graph can have the same L/Ns/Rs but different
        // effective stimulus bindings. Realize the assessed geometry explicitly
        // before treating its normalized preview scope as a measured point.
        const bool realizePreview=requireResistanceDomains && !restoringBest && cursor.domainReady &&
            candidate==cursor.domainGeometry && cursor.domainScope!=revision;
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
    if(probes>=config.maximumProbes){Stop(TrainingStatus::BudgetExhausted,"maximum_probes_exhausted");return {};}
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
    if(requireResistanceDomains)cursor.domainReady=false;
    for(auto& context:capacitanceContexts)context.suspended.domainReady=false;
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
    if(unchangedModelRevision!=revision || unchangedModelRevision.empty() || restoringBest)
    {Stop(TrainingStatus::InvalidModel);return;}
    pending.reset();armed.reset();compositeArmed.reset();anchor.reset();anchorTrial.clear();phase=Phase::Candidate;
    AdvanceCapacitanceContext();
    if(requireResistanceDomains)cursor.domainReady=false;
    for(auto& context:capacitanceContexts)context.suspended.domainReady=false;
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
            if(restoringBest)
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
        e.parameterChangeCost+=std::fabs(double(current[i].length)-background[i].length)+
            std::fabs(double(current[i].synapses)-background[i].synapses)+
            std::fabs(std::log(current[i].resistance)-std::log(background[i].resistance));
    if(currentCapacitance)
        for(const auto& [path,scale]:currentCapacitance->membraneScales)
        {
            const auto before=backgroundCapacitance->membraneScales.find(path);
            const double previous=before==backgroundCapacitance->membraneScales.end()?1.:before->second;
            e.parameterChangeCost+=std::fabs(std::log(scale)-std::log(previous));
        }
    if(!Finite(e.timingResidualLow) || !Finite(e.timingResidualHigh) || !Finite(e.amplitudeResidual))
    {Reject("nonfinite_training_residual");return;}
    e.withinTiming=std::max(std::fabs(e.timingResidualLow),std::fabs(e.timingResidualHigh))<=config.tolerances.sync;
    e.withinAmplitude=std::fabs(e.amplitudeResidual)<=AmplitudeTolerance(config,reference);
    evidence.push_back(e);
    if(requireResistanceDomains && !restoringBest && !cursor.candidateResistanceScope.empty())
        cursor.resistanceMeasurements.push_back({cursor.candidateResistanceScope,e});
    if(config.normalization==Normalization::SynapseCount && config.n0CandidateRepeats>1)
    {
        candidateMeasurements.push_back(e);
        if(candidateMeasurements.size()<config.n0CandidateRepeats)
        {phase=Phase::Anchor;anchor.reset();anchorTrial.clear();return;}
        CandidateRepeatEvidence group;group.inputIndex=index;group.modelEpoch=epoch;
        group.modelRevision=revision;group.fullParameters=current;
        group.capacitance=currentCapacitance;
        group.minimumRawPotential=group.maximumRawPotential=e.measuredRawPotential;
        group.withinTiming=group.withinAmplitude=true;
        for(const auto& sample:candidateMeasurements)
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
        candidateRepeats.push_back(std::move(group));candidateMeasurements.clear();
    }
    PrioritizeMeasuredLength(e);
    const bool improved=!best || Rank(e,config,reference)<Rank(*best,config,reference);
    if(improved)best=e;
    if(improved && !restoringBest && config.normalization==Normalization::SynapseCount &&
        e.withinTiming && !e.withinAmplitude)
    {
        // Normalize near a measured timing-ready geometry before spending the
        // remaining budget on distant cursor.lengths. No amplitude monotonicity or
        // preservation of timing is inferred, and both neighbors remain legal
        // full-model candidates with fresh anchor/input measurements.
        const auto& bound=config.bounds[index];
        for(const int delta:{1,-1})
        {
            auto candidate=e.fullParameters;candidate[index].synapses+=delta;
            if(candidate[index].synapses<bound.minimumSynapses || candidate[index].synapses>bound.maximumSynapses ||
                !PrepareCandidate(candidate))continue;
            if(std::find(cursor.issuedN0Candidates.begin(),cursor.issuedN0Candidates.end(),candidate)==cursor.issuedN0Candidates.end() &&
                std::find(cursor.n0Priority.begin(),cursor.n0Priority.end(),candidate)==cursor.n0Priority.end())
                cursor.n0Priority.push_back(std::move(candidate));
        }
    }
    if(restoringBest || (e.withinTiming && e.withinAmplitude))AdvanceInput();
    else {phase=Phase::Candidate;anchor.reset();anchorTrial.clear();AdvanceCapacitanceContext();}
}

void BoundedTrainingSession::RecordIncompleteCandidateRepeats(const std::string& reason)
{
    if(candidateMeasurements.empty())return;
    const auto& first=candidateMeasurements.front();
    CandidateRepeatEvidence group;group.inputIndex=first.inputIndex;group.modelEpoch=first.modelEpoch;
    group.modelRevision=first.modelRevision;group.fullParameters=first.fullParameters;
    group.capacitance=first.capacitance;
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
