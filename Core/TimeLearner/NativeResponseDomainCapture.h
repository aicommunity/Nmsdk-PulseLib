#pragma once
#include "NativeProbe.h"
#include "ResponseDomainAssociation.h"
#include "NumericChecks.h"
#include <algorithm>
#include <set>

namespace NMSDK::TimeLearner {
// Read-only sink for native callbacks. Physical paths are supplied in the same
// order as NativeProbeBindings::somas and must be checked by the model adapter.
// Capturing them is not an independent proof of the physical graph.
class NativeResponseDomainCapture {
public:
    void Arm(const PreparedVerificationProbe& prepared,const ProbeSpec& actual,
             std::vector<std::string> physicalChannelPaths) {
        Require(!armed && !closed && !failed,"domain_capture_already_armed");failed=true;
        const ProbeRecorder validateActual(actual);
        Require(!prepared.baseModelRevision.empty() && !prepared.protocolRevision.empty() &&
                prepared.effectiveModelRevision==actual.modelRevision && !actual.modelRevision.empty(),
                "domain_capture_prepared_context_invalid");
        Require(!actual.runId.empty() && !actual.trialId.empty() && !actual.sampleId.empty() &&
                !actual.patternHash.empty() && actual.maximumSamples>0 &&
                Detail::Finite(actual.begin) && Detail::Finite(actual.end) && actual.end>actual.begin &&
                Detail::Finite(actual.maximumSampleGap) && actual.maximumSampleGap>0. &&
                Detail::Finite(actual.minimumAmplitude) && actual.minimumAmplitude>0.,
                "domain_capture_specification_invalid");
        Require(!physicalChannelPaths.empty() && physicalChannelPaths.size()==actual.baseline.size() &&
                std::set<std::string>(physicalChannelPaths.begin(),physicalChannelPaths.end()).size()==physicalChannelPaths.size() &&
                std::none_of(physicalChannelPaths.begin(),physicalChannelPaths.end(),[](const auto& p){return p.empty();}),
                "domain_capture_physical_channels_invalid");
        Require(prepared.routing==ProbeRouting::BranchSoloBurst ? bool(prepared.selectedInputIndex) :
                !prepared.selectedInputIndex,"domain_capture_selector_invalid");
        context=prepared;specification=actual;paths=std::move(physicalChannelPaths);
        observation.runId=actual.runId;observation.trialId=actual.trialId;observation.sampleId=actual.sampleId;
        observation.patternHash=actual.patternHash;observation.modelRevision=actual.modelRevision;
        observation.protocolRevision=prepared.protocolRevision;observation.begin=actual.begin;observation.end=actual.end;
        observation.maximumSampleGap=actual.maximumSampleGap;observation.minimumAmplitude=actual.minimumAmplitude;
        for(std::size_t i=0;i<actual.baseline.size();++i) {
            Require(Detail::Finite(actual.baseline[i]),"domain_capture_baseline_invalid");
            observation.channels.push_back({i,actual.baseline[i],{}});
        }
        armed=true;failed=false;
    }
    void Append(const NativeProbeSample& sample) {
        Require(armed && !closed && !failed,"domain_capture_not_running");failed=true;
        Require(sample.effectiveModelRevision==context.effectiveModelRevision,"domain_capture_revision_changed");
        Require(samples<specification.maximumSamples && sample.potentials.size()==paths.size(),
                "domain_capture_sample_dimensions_or_budget");
        Require(Detail::Finite(sample.time) && Detail::Finite(sample.stimulus) &&
                std::all_of(sample.potentials.begin(),sample.potentials.end(),Detail::Finite),
                "domain_capture_nonfinite_sample");
        if(samples)Require(sample.time>observation.channels.front().samples.back().time &&
                           !Detail::IntervalAfter(sample.time,observation.channels.front().samples.back().time,
                                                  specification.maximumSampleGap),
                           "domain_capture_nonmonotonic_time");
        for(std::size_t i=0;i<sample.potentials.size();++i)
            Require(Detail::Finite(sample.potentials[i]-specification.baseline[i]),"domain_capture_nonfinite_amplitude");
        // Validate the entire callback before changing the captured record.
        auto events=observation.events;
        for(const auto& event:sample.newStimuli) {
            const auto ordinal=events.size();
            Require(ordinal<specification.expectedStimulusIds.size() &&
                    event.id==specification.expectedStimulusIds[ordinal] && Detail::Finite(event.time) &&
                    !Detail::After(observation.begin,event.time) && !Detail::After(event.time,sample.time) &&
                    (events.empty() || event.time>events.back().time),"domain_capture_actual_stimulus_invalid");
            events.push_back({event.id,event.time});
        }
        observation.events=std::move(events);
        for(std::size_t i=0;i<paths.size();++i)observation.channels[i].samples.push_back({sample.time,sample.potentials[i]});
        stimulusSamples.push_back(sample.stimulus);++samples;failed=false;
    }
    ResponseDomainResult Close(const NativeProbeResult& native,const ResponseDomainSpec& domains) {
        Require(armed && !closed && !failed,"domain_capture_not_running");closed=true;
        Require(native.status==NativeProbeStatus::Completed && native.reasons.empty() &&
                !native.requiresFreshVerification && native.prepared && native.observation &&
                native.currentBaseRevision==context.baseModelRevision,"domain_capture_native_not_completed");
        const auto& p=*native.prepared;const auto& o=*native.observation;const auto& s=o.specification;
        Require(p.routing==context.routing && p.inputId==context.inputId && p.protocolRevision==context.protocolRevision &&
                p.baseModelRevision==context.baseModelRevision && p.effectiveModelRevision==context.effectiveModelRevision &&
                p.modelEpoch==context.modelEpoch && p.selectedInputIndex==context.selectedInputIndex,
                "domain_capture_closure_context_changed");
        Require(s.runId==specification.runId && s.trialId==specification.trialId &&
                s.sampleId==specification.sampleId && s.patternHash==specification.patternHash &&
                s.modelRevision==specification.modelRevision && s.schemaVersion==specification.schemaVersion &&
                s.begin==specification.begin && s.end==specification.end && s.baseline==specification.baseline &&
                s.maximumSampleGap==specification.maximumSampleGap && s.minimumAmplitude==specification.minimumAmplitude &&
                s.maximumSamples==specification.maximumSamples && s.expectedStimulusIds==specification.expectedStimulusIds &&
                s.recordSpikes==specification.recordSpikes && s.characterization==specification.characterization &&
                s.stimulusSubset==specification.stimulusSubset && s.stimulusProfile==specification.stimulusProfile &&
                s.waveAssignment==specification.waveAssignment && s.peakSelection==specification.peakSelection &&
                s.windows.size()==specification.windows.size() && o.schemaVersion==s.schemaVersion,
                "domain_capture_closure_specification_changed");
        for(std::size_t i=0;i<s.windows.size();++i) {
            const auto& a=s.windows[i];const auto& b=specification.windows[i];
            Require(a.inputId==b.inputId && a.stimulusId==b.stimulusId && a.channel==b.channel &&
                    a.begin==b.begin && a.end==b.end,"domain_capture_closure_windows_changed");
        }
        Require(o.complete && o.endReason=="window_closed" && o.sampleCount==samples &&
                o.stimuli.size()==observation.events.size(),"domain_capture_incomplete_closure");
        for(std::size_t i=0;i<o.stimuli.size();++i)
            Require(o.stimuli[i].id==observation.events[i].id && o.stimuli[i].time==observation.events[i].time,
                    "domain_capture_closure_stimuli_changed");
        Require(observation.events.size()==specification.expectedStimulusIds.size() && samples>0,
                "domain_capture_missing_stimulus_or_samples");
        // Replay the production recorder on the actual captured frames. This
        // permits reproducible legacy assignment diagnostics, not arbitrary
        // integrity errors hidden behind observation.valid=false.
        ProbeRecorder replay(specification);std::size_t event=0;
        for(std::size_t frame=0;frame<samples;++frame) {
            const auto time=observation.channels.front().samples[frame].time;
            while(event<observation.events.size() && !Detail::After(observation.events[event].time,time)) {
                replay.RecordStimulus(observation.events[event].id,observation.events[event].time);++event;
            }
            std::vector<double> values;
            for(const auto& channel:observation.channels)values.push_back(channel.samples[frame].rawPotential);
            replay.Observe(time,values,context.effectiveModelRevision);
        }
        const auto reproduced=replay.Close(observation.channels.front().samples.back().time,
                                           context.effectiveModelRevision,"window_closed");
        Require(reproduced.complete && reproduced.valid==o.valid && reproduced.reasons==o.reasons,
                "domain_capture_native_diagnostics_not_reproduced");
        Require(reproduced.peaks.size()==o.peaks.size() && reproduced.burstAssignments.size()==o.burstAssignments.size(),
                "domain_capture_native_measurement_dimensions_changed");
        for(std::size_t i=0;i<o.peaks.size();++i) {
            const auto& a=reproduced.peaks[i];const auto& b=o.peaks[i];
            Require(a.inputId==b.inputId && a.stimulusId==b.stimulusId && a.validity==b.validity &&
                    a.amplitude==b.amplitude && a.rawPotential==b.rawPotential && a.time==b.time &&
                    a.plateauEnd==b.plateauEnd && a.delayFromStimulus==b.delayFromStimulus &&
                    a.candidateCount==b.candidateCount,"domain_capture_native_peak_not_reproduced");
        }
        for(std::size_t i=0;i<o.burstAssignments.size();++i) {
            const auto& a=reproduced.burstAssignments[i];const auto& b=o.burstAssignments[i];
            Require(a.channel==b.channel && a.validity==b.validity && a.waves.size()==b.waves.size(),
                    "domain_capture_native_assignment_not_reproduced");
            for(std::size_t j=0;j<a.waves.size();++j) {
                const auto& x=a.waves[j];const auto& y=b.waves[j];
                Require(x.stimulusId==y.stimulusId && x.amplitude==y.amplitude && x.rawPotential==y.rawPotential &&
                        x.time==y.time && x.plateauEnd==y.plateauEnd,"domain_capture_native_wave_not_reproduced");
            }
        }
        // Legacy peak/whole-burst assignment validity is deliberately separate.
        // The diagnostic analyzer independently checks waveform coverage and
        // stimuli; it can never produce a training certificate.
        observation.complete=true;
        return AnalyzeDeclaredResponseDomains(domains,observation);
    }
    const ResponseDomainObservation& Observation() const {return observation;}
    const PreparedVerificationProbe& Context() const {return context;}
    const std::vector<std::string>& PhysicalChannelPaths() const {return paths;}
    const std::vector<double>& StimulusSamples() const {return stimulusSamples;}
private:
    static void Require(bool condition,const char* reason) {if(!condition)throw std::invalid_argument(reason);}
    bool armed=false,closed=false,failed=false;
    std::size_t samples=0;
    ProbeSpec specification;
    PreparedVerificationProbe context;
    std::vector<std::string> paths;
    std::vector<double> stimulusSamples;
    ResponseDomainObservation observation;
};
}
