#pragma once
#include "NativeProbe.h"
#include "NativeThresholdSupport.h"
#include "ProbeEvidenceJson.h"
#include "ModelIdentity.h"
#include "NumericChecks.h"
#include <QJsonArray>
#include <algorithm>

namespace NMSDK::TimeLearner {
// Read-only supplement to the ordinary training-burst probe. It neither
// calculates components nor changes thresholds/routing/reset policy. The native
// executor remains the sole owner of the physical probe and its completeness.
class NativeSoloThresholdTrace {
public:
    template<class Owner> static std::shared_ptr<NativeSoloThresholdTrace> Attach(Owner& owner,NativeProbeRequest& request)
    {
        auto trace=std::shared_ptr<NativeSoloThresholdTrace>(new NativeSoloThresholdTrace);
        trace->requested=EncodeNativeProbeSpecification(request.specification,true);
        const auto life=owner.ObserveLifecycle();auto* pointer=&owner;
        const auto zone=[life,pointer]() {
            Require(life.IsCurrent(),"solo_threshold_owner_lifecycle_changed");
            auto neuron=pointer->template GetComponentL<NPulseNeuron>("Neuron",true);
            auto result=neuron?neuron->template GetComponentL<NPulseLTZoneCommon>("LTZone",true):RDK::UEPtr<NPulseLTZoneCommon>{};
            Require(bool(result),"missing_solo_threshold_source");return result;
        };
        const auto prepared=request.onPrepared;const auto sample=request.onSample;
        request.onPrepared=[trace,zone,prepared](const auto& probe,const auto& specification) {
            trace->Prepare(zone(),probe,specification);
            if(prepared)prepared(probe,specification);
        };
        request.onSample=[trace,zone,sample](const auto& frame) {
            trace->Observe(zone(),frame);
            if(sample)sample(frame);
        };
        return trace;
    }

    QJsonObject Finish(const NativeProbeResult& result)
    {
        Require(!closed,"solo_threshold_trace_already_closed");closed=true;
        const auto native=EncodeNativeProbeEvidence(result);
        const bool complete=armed && result.observation && result.observation->complete &&
            result.observation->sampleCount==std::size_t(frames.size()) && last && Equal(*last,spec.end) &&
            !frames.empty() && Equal(frames[0].toObject()["time"].toDouble(),spec.begin);
        const bool valid=complete && result.status==NativeProbeStatus::Completed && result.prepared &&
            result.observation->valid && !result.requiresFreshVerification && result.reasons.empty() &&
            result.requestedSpecification && EncodeNativeProbeSpecification(*result.requestedSpecification,true)==requested &&
            result.observation->specification.runId==spec.runId && result.observation->specification.trialId==spec.trialId &&
            result.observation->specification.modelRevision==spec.modelRevision &&
            EncodeNativeProbeSpecification(result.observation->specification)==EncodeNativeProbeSpecification(spec) &&
            result.prepared->routing==prepared.routing && result.prepared->inputId==prepared.inputId &&
            result.prepared->protocolRevision==prepared.protocolRevision && result.prepared->modelEpoch==prepared.modelEpoch &&
            result.prepared->selectedInputIndex==prepared.selectedInputIndex &&
            result.prepared->baseModelRevision==prepared.baseModelRevision &&
            result.prepared->effectiveModelRevision==prepared.effectiveModelRevision &&
            result.currentBaseRevision==prepared.baseModelRevision;
        finishedReport=Report(native,complete,valid,result.observation?result.observation->endReason:NativeProbeStatusName(result.status));
        return *finishedReport;
    }
    QJsonObject Interrupt(const std::string& reason)
    {
        Require(!closed && !reason.empty(),"invalid_solo_threshold_interruption");closed=true;
        // Reset may deliberately discard the executor result. Keep real sampled
        // values, but do not manufacture a cancelled NativeProbeResult.
        finishedReport=Report(QJsonValue(QJsonValue::Null),false,false,reason);
        return *finishedReport;
    }
    // Immutable value copy of a genuinely closed recorder. No JSON import or
    // owner access: a caller cannot turn an externally edited report into a
    // trusted production reference through this API.
    QJsonObject ClosedReport() const
    {
        Require(finishedReport.has_value(),"solo_threshold_trace_not_finished");
        return *finishedReport;
    }
private:
    QJsonObject Report(const QJsonValue& native,bool complete,bool valid,const std::string& reason) const
    {
        QJsonArray events;for(double t:spikes)events.append(t);
        QJsonObject report{{"kind","native_solo_threshold_trace"},{"schema_version",1},
            {"native_probe",native},{"armed",armed},{"complete",complete},{"valid",valid},
            {"requested_specification",requested},{"termination_reason",QString::fromStdString(reason)},
            {"nonspiking",valid?QJsonValue(spikes.empty()):QJsonValue(QJsonValue::Null)},
            {"proposal_eligible",valid && spikes.empty()},{"source",source},
            {"baseline_before_children",armed?QJsonValue(baseline):QJsonValue(QJsonValue::Null)},
            {"minimum_after_children",minimum?QJsonValue(*minimum):QJsonValue(QJsonValue::Null)},
            {"maximum_after_children",maximum?QJsonValue(*maximum):QJsonValue(QJsonValue::Null)},
            {"spike_times",events},{"samples",frames},{"calibrated",false},
            {"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
        report["sha256"]=NmsdkTools::CanonicalModelDataSha256(report);
        return report;
    }
    NativeSoloThresholdTrace()=default;
    bool armed=false,closed=false;
    NPulseLTZoneCommon* sourcePointer=nullptr;
    RDK::UModuleLifecycleObservation sourceLife;
    ProbeSpec spec;PreparedVerificationProbe prepared;
    QJsonObject source,requested;QJsonArray frames;
    std::optional<QJsonObject> finishedReport;
    double baseline=0.,threshold=0.;
    std::optional<double> minimum,maximum,last,lastSpike;
    std::vector<double> spikes;
    static void Require(bool value,const char* reason) {if(!value)throw std::logic_error(reason);}
    static bool Equal(double a,double b) {return !Detail::After(a,b) && !Detail::After(b,a);}
    void Prepare(RDK::UEPtr<NPulseLTZoneCommon> zone,const PreparedVerificationProbe& probe,const ProbeSpec& specification)
    {
        Require(!armed && !closed,"solo_threshold_trace_reused");
        RequireNativeScalarSpikeThreshold(*zone);
        Require(!zone->Potential.IsConnected() && !zone->Potential.UsesExternalDataSource(),"solo_threshold_potential_source_is_external");
        Require(zone->OutputPulseTimes.GetRows()==1 && zone->OutputPulseTimes.GetCols()==0,"solo_threshold_reset_retained_spikes");
        Require(specification.maximumSamples>=2 && specification.stimulusProfile.has_value(),"invalid_solo_threshold_sample_budget");
        baseline=zone->Potential.GetData();threshold=zone->Threshold.GetData();
        Require(Detail::Finite(baseline) && Detail::Finite(threshold),"nonfinite_solo_threshold_baseline");
        sourcePointer=zone.Get();sourceLife=zone->ObserveLifecycle();spec=specification;prepared=probe;armed=true;
        source={{"path","$.LTZone.Potential"},{"class",QString::fromStdString(zone->GetCompClassName())},
            {"threshold",threshold},{"frequency_hz",double(zone->GetTimeStep())},
            {"base_model_revision",QString::fromStdString(probe.baseModelRevision)},
            {"effective_model_revision",QString::fromStdString(probe.effectiveModelRevision)},
            {"sample_phase","native_after_children"},{"units","model_units_SI_unverified"}};
        Require(double(zone->GetTimeStep())==spec.stimulusProfile->frequencyHz,"solo_threshold_source_frequency_mismatch");
    }
    void Observe(RDK::UEPtr<NPulseLTZoneCommon> zone,const NativeProbeSample& frame)
    {
        Require(armed && !closed && zone.Get()==sourcePointer && sourceLife.IsCurrent(),"solo_threshold_source_lifecycle_changed");
        Require(!zone->Potential.IsConnected() && !zone->Potential.UsesExternalDataSource(),"solo_threshold_potential_source_is_external");
        Require(frame.effectiveModelRevision==prepared.effectiveModelRevision && zone->Threshold.GetData()==threshold,
            "solo_threshold_model_changed");
        Require(std::size_t(frames.size())<spec.maximumSamples,"solo_threshold_recording_budget_exhausted");
        Require(Detail::Finite(frame.time) && Detail::Finite(frame.stimulus) && !Detail::After(spec.begin,frame.time) &&
            !Detail::After(frame.time,spec.end) && (!last || (frame.time>*last &&
            !Detail::IntervalAfter(frame.time,*last,spec.maximumSampleGap))),"invalid_solo_threshold_sample_time");
        const double potential=zone->Potential.GetData();Require(Detail::Finite(potential),"nonfinite_solo_threshold_potential");
        const auto& history=zone->OutputPulseTimes.GetData();
        Require(history.GetRows()==1 && std::size_t(history.GetCols())<=spec.maximumSamples,"invalid_solo_threshold_spike_history");
        std::optional<double> previous;QJsonArray added;
        for(int i=0;i<history.GetCols();++i) {
            const double event=history(0,i);
            Require(Detail::Finite(event) && !Detail::After(spec.begin,event) && !Detail::After(event,frame.time) &&
                (!previous || event>*previous),"invalid_solo_threshold_spike_time");
            previous=event;
            if(!lastSpike || event>*lastSpike){spikes.push_back(event);added.append(event);}
        }
        if(previous && (!lastSpike || *previous>*lastSpike))lastSpike=previous;
        Require(spikes.size()<=spec.maximumSamples,"solo_threshold_spike_budget_exhausted");
        QJsonArray somas,stimuli;
        for(double v:frame.potentials){Require(Detail::Finite(v),"nonfinite_solo_threshold_soma");somas.append(v);}
        for(const auto& event:frame.newStimuli)stimuli.append(QJsonObject{{"id",QString::fromStdString(event.id)},{"time",event.time}});
        frames.append(QJsonObject{{"time",frame.time},{"stimulus",frame.stimulus},{"soma",somas},
            {"lt_potential",potential},{"new_stimuli",stimuli},{"new_spikes",added}});
        minimum=minimum?std::min(*minimum,potential):potential;maximum=maximum?std::max(*maximum,potential):potential;last=frame.time;
    }
};
}
