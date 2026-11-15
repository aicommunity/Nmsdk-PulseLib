#include "DeclaredWindowBasinAssignment.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <tuple>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
using Validity=DeclaredWindowBasinValidity;
struct Failure {Validity validity;const char* reason;};
void Require(bool value,Validity validity,const char* reason)
{if(!value)throw Failure{validity,reason};}
struct Extremum {
    std::size_t first=0,last=0;
    double value=0.;
    bool maximum=false,minimum=false,boundary=false;
};
struct Geometry {
    std::vector<double> values;
    std::vector<Extremum> extrema;
};
Geometry Features(const BasinGeometryTrace& trace) {
    Geometry result;
    for(double raw:trace.rawPotential) {
        const double value=raw-trace.baseline;
        Require(std::isfinite(value),Validity::InvalidSpecification,"nonfinite_basin_amplitude");
        result.values.push_back(value);
    }
    const auto size=result.values.size();
    for(std::size_t first=0;first<size;) {
        std::size_t last=first;while(last+1<size && result.values[last+1]==result.values[first])++last;
        const double value=result.values[first];const bool left=first>0,right=last+1<size;
        const bool maximum=(!left || value>result.values[first-1]) && (!right || value>result.values[last+1]);
        const bool minimum=(!left || value<result.values[first-1]) && (!right || value<result.values[last+1]);
        if(maximum || minimum)result.extrema.push_back({first,last,value,maximum,minimum,!left || !right});
        first=last+1;
    }
    return result;
}
bool Intersects(const Extremum& e,const DeclaredWindowBasinSpecification& s,const std::vector<double>& times)
{return times[e.last]>=s.windowBegin && times[e.first]<=s.windowEnd;}
using Topology=std::vector<std::tuple<std::size_t,std::size_t,bool,bool,bool>>;
Topology WindowTopology(const Geometry& geometry,const DeclaredWindowBasinSpecification& s,const std::vector<double>& times) {
    Topology result;
    for(const auto& e:geometry.extrema)if(Intersects(e,s,times))
        result.emplace_back(e.first,e.last,e.maximum,e.minimum,e.boundary);
    return result;
}
std::pair<BasinAnchor,BasinAnchor> Anchors(const Geometry& geometry,std::size_t first,std::size_t last) {
    BasinAnchor left{0,0,true},right{geometry.values.size()-1,geometry.values.size()-1,true};
    for(const auto& e:geometry.extrema)if(e.minimum) {
        if(e.last<first)left={e.first,e.last,e.boundary};
        else if(e.first>last){right={e.first,e.last,e.boundary};break;}
    }
    return {left,right};
}
std::vector<ResponseMaximum> Candidates(const Geometry& geometry,const BasinGeometryTrace& trace,
    const std::vector<double>& times,double floor) {
    std::vector<ResponseSample> samples;for(std::size_t i=0;i<times.size();++i)samples.push_back({times[i],trace.rawPotential[i]});
    const auto measured=ExtractMeasuredResponse(samples,trace.baseline,floor);
    Require(measured.validity==ResponseFeatureValidity::Valid,Validity::Incomplete,"invalid_basin_full_features");
    auto result=measured.positiveMaxima;
    for(const auto& e:geometry.extrema)if(e.maximum && e.boundary && e.value>=floor)
        result.push_back({e.first,e.last,e.value,trace.rawPotential[e.first],times[e.first],times[e.last]});
    std::sort(result.begin(),result.end(),[](const auto& a,const auto& b){return a.firstSample<b.firstSample;});return result;
}
BasinControlPresence Presence(const Geometry& control,const DeclaredBasinCandidate& candidate,double floor) {
    const auto left=candidate.left.lastSample,right=candidate.right.firstSample;
    std::size_t found=0;bool unknown=false;
    for(const auto& e:control.extrema)if(e.maximum && e.last>=left && e.first<=right) {
        // A constant complete record has no interior extremum. A positive
        // above-floor constant record is instead a clipped/unknown response.
        if(e.first==0 && e.last+1==control.values.size()) {
            unknown=unknown || e.value>=floor;continue;
        }
        if(e.boundary || e.first<=left || e.last>=right) {unknown=true;continue;}
        ++found;
        // A geometrical vertex must not be called absent merely because it
        // crossed the measurable floor in this omission control.
        if(e.value<floor)unknown=true;
    }
    if(unknown || found>1)return BasinControlPresence::Unknown;
    return found==1?BasinControlPresence::Preserved:BasinControlPresence::AbsentWithinFixedBasin;
}
bool IsAmbiguous(BasinCandidateStatus status)
{return status==BasinCandidateStatus::SharedNecessary || status==BasinCandidateStatus::NoNecessaryInput ||
        status==BasinCandidateStatus::Unknown;}
bool PotentiallyAssigned(const DeclaredBasinCandidate& candidate,std::size_t assigned)
{return candidate.status==BasinCandidateStatus::Clipped || candidate.controls.size()<=assigned ||
        candidate.controls[assigned]!=BasinControlPresence::Preserved;}
}

DeclaredWindowBasinResult SelectDeclaredWindowBasinMaximum(const DeclaredWindowBasinSpecification& s,
    const DeclaredWindowBasinPolicy& p,const DeclaredWindowBasinWitness& w) {
    DeclaredWindowBasinResult result;
    try {
        Require(s.schemaVersion==1 && p.schemaVersion==1 && s.inputCount>0 && s.assignedInput<s.inputCount &&
            std::isfinite(s.windowBegin) && std::isfinite(s.windowEnd) && s.windowEnd>s.windowBegin &&
            std::isfinite(s.minimumAmplitude) && s.minimumAmplitude>0. &&
            std::isfinite(p.repeatabilityAbsolute) && p.repeatabilityAbsolute>=0.,
            Validity::InvalidSpecification,"invalid_declared_basin_specification_or_policy");
        Require(s.inputCount>=2,Validity::Unsupported,"single_input_uses_existing_single_impulse_path");
        Require(p.maximumInputs>0 && p.maximumSamples>0 && p.maximumTotalSamples>0,
            Validity::InvalidSpecification,"missing_declared_basin_budget");
        Require(s.inputCount<=p.maximumInputs,Validity::BudgetExhausted,"basin_input_budget_exhausted");
        Require(w.omissions.size()==s.inputCount,Validity::InvalidSpecification,"basin_omission_inventory_changed");
        const auto frames=w.sampleTimes.size();
        Require(frames>=3,Validity::Incomplete,"insufficient_basin_samples");
        Require(frames<=p.maximumSamples && s.inputCount<=std::numeric_limits<std::size_t>::max()-2 &&
            frames<=p.maximumTotalSamples/(s.inputCount+2),Validity::BudgetExhausted,"basin_sample_budget_exhausted");
        for(std::size_t i=0;i<frames;++i)
            Require(std::isfinite(w.sampleTimes[i]) && (!i || w.sampleTimes[i]>w.sampleTimes[i-1]),
                Validity::InvalidSpecification,"invalid_declared_basin_grid");
        Require(s.windowBegin>=w.sampleTimes.front() && s.windowEnd<=w.sampleTimes.back(),
            Validity::Incomplete,"declared_basin_window_not_covered");
        const auto checkTrace=[&](const BasinGeometryTrace& t) {
            Require(t.rawPotential.size()==frames && std::isfinite(t.baseline),
                Validity::InvalidSpecification,"basin_trace_dimensions_or_baseline_changed");
            for(double value:t.rawPotential)Require(std::isfinite(value),Validity::InvalidSpecification,"nonfinite_basin_sample");
        };
        checkTrace(w.fullBefore);checkTrace(w.fullAfter);
        std::vector<const BasinGeometryTrace*> omitted(s.inputCount,nullptr);
        for(const auto& control:w.omissions) {
            Require(control.omittedInput<s.inputCount && !omitted[control.omittedInput],
                Validity::InvalidSpecification,"duplicate_or_unknown_basin_omission");
            checkTrace(control.trace);omitted[control.omittedInput]=&control.trace;
        }
        const auto full=Features(w.fullBefore),repeat=Features(w.fullAfter);
        for(std::size_t i=0;i<frames;++i) {
            const double difference=std::fabs(full.values[i]-repeat.values[i]);
            Require(std::isfinite(difference),Validity::InvalidSpecification,"nonfinite_basin_repeat_difference");
            Require(difference<=p.repeatabilityAbsolute,Validity::NonRepeatable,"basin_full_response_not_repeatable");
        }
        Require(WindowTopology(full,s,w.sampleTimes)==WindowTopology(repeat,s,w.sampleTimes),
            Validity::NonRepeatable,"basin_window_topology_not_repeated");
        std::vector<Geometry> controls;for(const auto* trace:omitted)controls.push_back(Features(*trace));
        const auto maxima=Candidates(full,w.fullBefore,w.sampleTimes,s.minimumAmplitude);
        const auto repeatMaxima=Candidates(repeat,w.fullAfter,w.sampleTimes,s.minimumAmplitude);
        const auto inventory=[&](const std::vector<ResponseMaximum>& peaks) {
            std::vector<std::pair<std::size_t,std::size_t>> ranges;
            for(const auto& peak:peaks)if(w.sampleTimes[peak.lastSample]>=s.windowBegin && w.sampleTimes[peak.firstSample]<=s.windowEnd)
                ranges.emplace_back(peak.firstSample,peak.lastSample);
            return ranges;
        };
        Require(inventory(maxima)==inventory(repeatMaxima),Validity::NonRepeatable,"basin_observable_peak_inventory_not_repeated");
        for(const auto& maximum:maxima) {
            if(w.sampleTimes[maximum.lastSample]<s.windowBegin || w.sampleTimes[maximum.firstSample]>s.windowEnd)continue;
            DeclaredBasinCandidate candidate;candidate.maximum=maximum;
            const auto anchors=Anchors(full,maximum.firstSample,maximum.lastSample);
            candidate.left=anchors.first;candidate.right=anchors.second;
            const auto repeated=std::find_if(repeatMaxima.begin(),repeatMaxima.end(),[&](const auto& peak) {
                return peak.firstSample==maximum.firstSample && peak.lastSample==maximum.lastSample;
            });
            Require(repeated!=repeatMaxima.end() && Anchors(repeat,maximum.firstSample,maximum.lastSample)==anchors,
                Validity::NonRepeatable,"basin_peak_or_valley_range_not_repeated");
            if(maximum.firstSample==0 || maximum.lastSample+1==frames ||
                w.sampleTimes[maximum.firstSample-1]<s.windowBegin || w.sampleTimes[maximum.lastSample+1]>s.windowEnd) {
                candidate.status=BasinCandidateStatus::Clipped;candidate.reasons.push_back("basin_full_peak_not_interior_to_declared_window");
            }
            else {
                bool unknown=false;
                for(std::size_t input=0;input<s.inputCount;++input) {
                    const auto state=Presence(controls[input],candidate,s.minimumAmplitude);candidate.controls.push_back(state);
                    unknown=unknown || state==BasinControlPresence::Unknown;
                    if(state==BasinControlPresence::AbsentWithinFixedBasin)candidate.necessaryInputs.push_back(input);
                }
                if(unknown){candidate.status=BasinCandidateStatus::Unknown;candidate.reasons.push_back("basin_control_geometry_unknown");}
                else if(candidate.necessaryInputs.size()==1)candidate.status=BasinCandidateStatus::UniqueNecessary;
                else if(candidate.necessaryInputs.empty()) {
                    candidate.status=BasinCandidateStatus::NoNecessaryInput;candidate.reasons.push_back("basin_has_no_unique_necessary_input");
                }
                else {candidate.status=BasinCandidateStatus::SharedNecessary;candidate.reasons.push_back("basin_requires_multiple_inputs");}
            }
            result.candidates.push_back(std::move(candidate));
        }
        std::optional<std::size_t> best;bool tied=false;
        for(std::size_t i=0;i<result.candidates.size();++i) {
            const auto& c=result.candidates[i];
            if(c.status!=BasinCandidateStatus::UniqueNecessary || c.necessaryInputs.front()!=s.assignedInput)continue;
            if(!best || c.maximum.amplitude>result.candidates[*best].maximum.amplitude){best=i;tied=false;}
            else if(c.maximum.amplitude==result.candidates[*best].maximum.amplitude)tied=true;
        }
        for(const auto& c:result.candidates)if(!best || c.maximum.amplitude>=result.candidates[*best].maximum.amplitude) {
            Require(c.status!=BasinCandidateStatus::Clipped,Validity::Clipped,"highest_window_basin_peak_is_clipped");
            Require(!IsAmbiguous(c.status) || (best && !PotentiallyAssigned(c,s.assignedInput)),
                Validity::Ambiguous,"highest_window_basin_peak_has_ambiguous_necessity");
        }
        Require(bool(best),Validity::Missing,"no_unique_assigned_basin_peak_in_declared_window");
        Require(!tied,Validity::Ambiguous,"highest_assigned_window_basin_peaks_tie");
        const double repeatedBest=repeat.values[result.candidates[*best].maximum.firstSample];
        for(std::size_t i=0;i<result.candidates.size();++i)if(i!=*best) {
            const auto& c=result.candidates[i];const double amplitude=repeat.values[c.maximum.firstSample];
            const bool assigned=c.status==BasinCandidateStatus::UniqueNecessary && c.necessaryInputs.front()==s.assignedInput;
            Require(!(amplitude>=repeatedBest && (assigned ||
                ((IsAmbiguous(c.status) || c.status==BasinCandidateStatus::Clipped) && PotentiallyAssigned(c,s.assignedInput)))),
                Validity::NonRepeatable,"highest_assigned_basin_peak_not_repeated");
        }
        result.selectedCandidate=best;result.measuredMaximum=result.candidates[*best].maximum;result.validity=Validity::Valid;
    }
    catch(const Failure& failure){result.validity=failure.validity;result.reasons.push_back(failure.reason);}
    return result;
}
const char* DeclaredWindowBasinValidityName(DeclaredWindowBasinValidity v) {
    switch(v) {
        case Validity::Valid:return "valid";case Validity::Missing:return "missing";case Validity::Ambiguous:return "ambiguous";
        case Validity::Clipped:return "clipped";case Validity::NonRepeatable:return "nonrepeatable";case Validity::Incomplete:return "incomplete";
        case Validity::InvalidSpecification:return "invalid_specification";case Validity::BudgetExhausted:return "budget_exhausted";
        case Validity::Unsupported:return "unsupported";
    }return "unknown";
}
const char* BasinControlPresenceName(BasinControlPresence s) {
    switch(s){case BasinControlPresence::Preserved:return "preserved";
        case BasinControlPresence::AbsentWithinFixedBasin:return "absent_within_fixed_basin";case BasinControlPresence::Unknown:return "unknown";}
    return "unknown";
}
const char* BasinCandidateStatusName(BasinCandidateStatus s) {
    switch(s){case BasinCandidateStatus::UniqueNecessary:return "unique_necessary";case BasinCandidateStatus::SharedNecessary:return "shared_necessary";
        case BasinCandidateStatus::NoNecessaryInput:return "no_necessary_input";case BasinCandidateStatus::Unknown:return "unknown";
        case BasinCandidateStatus::Clipped:return "clipped";}
    return "unknown";
}
}
