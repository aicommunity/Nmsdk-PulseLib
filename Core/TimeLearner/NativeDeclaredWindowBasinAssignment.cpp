#include "NativeDeclaredWindowBasinAssignment.h"
#include "NativeBasinValidation.h"
#include "CandidateWindowTrainingBoundary.h"
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void NativeRequire(bool value,const char* reason)
{if(!value)throw std::invalid_argument(reason);}

NativeDeclaredWindowBasinResult MeasureChecked(const DeclaredWindowBasinPolicy& policy,
    const NativeMatchedOmissionResult& group,const MatchedOmissionDetail::CheckedContext& checked,
    double windowBegin,double windowEnd) {
    NativeDeclaredWindowBasinResult result;const auto& s=group.specification;
    const auto& full=*checked.Before().trace;const auto& repeat=*checked.After().trace;
    const auto project=[&](const MatchedOmissionTrace& trace) {
        BasinGeometryTrace geometry;const auto& channel=trace.observation.channels[s.channel];geometry.baseline=channel.baseline;
        for(const auto& sample:channel.samples)geometry.rawPotential.push_back(sample.rawPotential);return geometry;
    };
    DeclaredWindowBasinWitness witness;
    // The common validator proved actual frame count/order/relative grid
    // and retained event indices. Preserve fullBefore raw timestamps; do
    // not interpolate, shift by a tick or synthesize a different waveform.
    for(const auto& sample:full.observation.channels[s.channel].samples)witness.sampleTimes.push_back(sample.time);
    witness.fullBefore=project(full);witness.fullAfter=project(repeat);
    for(std::size_t input=0;input<checked.Controls().size();++input)
        witness.omissions.push_back({input,project(*checked.Controls()[input].trace)});
    DeclaredWindowBasinSpecification geometry;geometry.inputCount=s.stimulusIds.size();geometry.assignedInput=s.assignedInput;
    geometry.windowBegin=windowBegin;
    geometry.windowEnd=windowEnd;geometry.minimumAmplitude=s.minimumAmplitude;
    NativeRequire(std::isfinite(geometry.windowBegin) && std::isfinite(geometry.windowEnd),"basin_native_window_arithmetic_nonfinite");
    auto measured=SelectDeclaredWindowBasinMaximum(geometry,policy,witness);
    const double onset=full.observation.events[s.assignedInput].time;
    if(measured.validity==DeclaredWindowBasinValidity::Valid) {
        NativeRequire(bool(measured.measuredMaximum),"basin_native_valid_geometry_has_no_peak");
        if(measured.measuredMaximum->firstSample<checked.Before().eventFrames[s.assignedInput] ||
            measured.measuredMaximum->time<onset) {
            result.status=NativeDeclaredWindowBasinStatus::CausalityRejected;
            result.reasons.push_back("basin_native_selected_peak_precedes_actual_assigned_event");return result;
        }
    }
    result.geometry=std::move(measured);result.actualAssignedOnset=onset;result.status=NativeDeclaredWindowBasinStatus::Completed;
    return result;
}

}
NativeDeclaredWindowBasinResult SelectNativeDeclaredWindowBasinMaximum(const NativeDeclaredWindowBasinSpecification& spec,
    const DeclaredWindowBasinPolicy& policy,const NativeMatchedOmissionResult& group) {
    NativeDeclaredWindowBasinResult result;
    try {
        const auto& s=group.specification;
        NativeRequire(spec.schemaVersion==1 && spec.methodId==NativeDeclaredWindowBasinResult::MethodId() &&
            spec.groupId==s.groupId && spec.inputId==s.inputId && spec.assignedInput==s.assignedInput && spec.channel==s.channel &&
            std::isfinite(spec.windowBeginOffset) && std::isfinite(spec.windowEndOffset) &&
            spec.windowBeginOffset>=0. && spec.windowEndOffset>spec.windowBeginOffset && spec.windowEndOffset<=s.duration,
            "basin_native_method_or_window_binding_changed");
        const auto checked=MatchedOmissionDetail::CheckMatchedOmissionContext(s,group.policy,group.witness);
        NativeBasinDetail::CheckNativeEnvelope(group,*checked);
        return MeasureChecked(policy,group,*checked,checked->Before().trace->observation.begin+spec.windowBeginOffset,
            checked->Before().trace->observation.begin+spec.windowEndOffset);
    }
    catch(const MatchedOmissionDetail::Failure& failure){result.reasons.push_back(failure.reason);}
    catch(const std::exception& error){result.reasons.push_back(error.what());}
    return result;
}
NativeDeclaredWindowBasinResult SelectNativeDeclaredWindowBasinMaximum(const CandidateWindowBasinSpecification& spec,
    const DeclaredWindowBasinPolicy& policy,const NativeMatchedOmissionResult& group) {
    NativeDeclaredWindowBasinResult result;
    try {
        const auto& s=group.specification;
        NativeRequire(spec.schemaVersion==2 && spec.methodId==NativeDeclaredWindowBasinResult::MethodId() &&
            spec.groupId==s.groupId && spec.inputId==s.inputId && spec.assignedInput==s.assignedInput && spec.channel==s.channel,
            "basin_native_method_or_window_binding_changed");
        const bool reference=spec.coordinateMode==CandidateWindowCoordinateMode::ReferenceTime;
        const bool candidate=spec.coordinateMode==CandidateWindowCoordinateMode::CandidateFrame;
        NativeRequire((reference && spec.referenceTime && !spec.candidateFrame) ||
            (candidate && spec.candidateFrame && !spec.referenceTime),"basin_native_spec2_coordinate_binding");
        if(reference)NativeRequire(std::isfinite(spec.referenceTime->beginOffset) && std::isfinite(spec.referenceTime->endOffset) &&
            spec.referenceTime->beginOffset>=0. && spec.referenceTime->endOffset>spec.referenceTime->beginOffset &&
            spec.referenceTime->endOffset<=s.duration,"basin_native_method_or_window_binding_changed");
        else NativeRequire(spec.candidateFrame->firstFrame<spec.candidateFrame->lastFrame &&
            spec.candidateFrame->lastFrame<=CandidateWindowMaximumExactCount,"basin_native_spec2_frame_declaration");
        const auto checked=MatchedOmissionDetail::CheckMatchedOmissionContext(s,group.policy,group.witness);
        NativeBasinDetail::CheckNativeEnvelope(group,*checked);
        const auto& full=checked->Before().trace->observation;
        double begin=0.,end=0.;
        if(reference) {
            begin=full.begin+spec.referenceTime->beginOffset;end=full.begin+spec.referenceTime->endOffset;
        } else {
            const auto& samples=full.channels[s.channel].samples;
            NativeRequire(spec.candidateFrame->lastFrame<samples.size(),"window_frames_not_captured");
            // The caller/private proposal must prove prequeue declaration;
            // this diagnostic selector cannot attest it. Projection uses ONLY
            // retained timestamps, no estimated dt, rounding or padding.
            begin=samples[std::size_t(spec.candidateFrame->firstFrame)].time;
            end=samples[std::size_t(spec.candidateFrame->lastFrame)].time;
        }
        return MeasureChecked(policy,group,*checked,begin,end);
    }
    catch(const MatchedOmissionDetail::Failure& failure){result.reasons.push_back(failure.reason);}
    catch(const std::exception& error){result.reasons.push_back(error.what());}
    return result;
}
}
