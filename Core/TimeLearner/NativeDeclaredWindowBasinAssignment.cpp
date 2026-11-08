#include "NativeDeclaredWindowBasinAssignment.h"
#include "MatchedOmissionValidation.h"
#include "MatchedOmissionEvidenceJson.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void NativeRequire(bool value,const char* reason)
{if(!value)throw std::invalid_argument(reason);}
std::string PatternHash(const std::vector<double>& isi) {
    QJsonArray values;for(double value:isi){NativeRequire(std::isfinite(value),"basin_native_nonfinite_source_isi");values.append(value);}
    return NmsdkTools::CanonicalModelDataSha256(values).toStdString();
}
void CheckNativeEnvelope(const NativeMatchedOmissionResult& group,const MatchedOmissionDetail::CheckedContext& checked) {
    const auto& s=group.specification;const auto count=s.stimulusIds.size();
    NativeRequire(group.reason=="omission_group_closed" && group.packets.size()==count+2 &&
        group.sourceIsi.size()==count && group.appliedSourceIsi==group.sourceIsi &&
        group.appliedPresentedIsi.size()==count+2,"basin_native_group_not_closed");
    NativeRequire(PatternHash(group.sourceIsi)==s.patternHash,"basin_native_original_isi_hash_changed");
    NativeRequire(std::isfinite(group.maximumModelSeconds) && group.maximumModelSeconds>0. &&
        std::isfinite(group.maximumWallSeconds) && group.maximumWallSeconds>0.,"basin_native_invalid_budget_declaration");
    std::vector<const MatchedOmissionTrace*> traces{checked.Before().trace};
    for(const auto& control:checked.Controls())traces.push_back(control.trace);traces.push_back(checked.After().trace);
    for(std::size_t i=0;i<count;++i)
        NativeRequire(s.stimulusIds[i]=="pulse"+std::to_string(i),"basin_native_source_identity_changed");
    for(std::size_t i=0;i<traces.size();++i) {
        const auto& trace=*traces[i];const auto& packet=group.packets[i];
        // This production codec entry point replays Arm/Append/CloseNative and
        // strictly roundtrips the closed packet. It has no v1 outcome whitelist.
        (void)EncodeMatchedOmissionPrimaryTrace(trace,packet);
        const auto planned=PlanStimulusSubset(group.sourceIsi,trace.presentedOrdinals).presentedIsi;
        const auto hash=PatternHash(planned);const auto& actual=packet.observation->specification;
        const auto& requested=*packet.requestedSpecification;
        NativeRequire(group.appliedPresentedIsi[i]==planned && actual.stimulusSubset &&
            actual.stimulusSubset->physicalInputCount==count && actual.stimulusSubset->sourceOrdinals==trace.presentedOrdinals &&
            actual.stimulusSubset->sourceTrainingPatternHash==s.patternHash &&
            actual.patternHash==hash && requested.patternHash==hash && actual.stimulusProfile==s.stimulusProfile &&
            requested.maximumSamples<=group.policy.maximumSamples && actual.maximumSamples<=group.policy.maximumSamples &&
            requested.end==s.duration,"basin_native_presentation_readback_changed");
        if(i)NativeRequire(actual.begin>group.packets[i-1].observation->specification.end,"basin_native_group_times_overlap");
    }
    const double span=group.packets.back().observation->specification.end-group.packets.front().observation->specification.begin;
    NativeRequire(std::isfinite(span) && span>0. && span<=group.maximumModelSeconds,"basin_native_model_span_exceeds_declaration");
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
        CheckNativeEnvelope(group,*checked);
        const auto& full=*checked->Before().trace;const auto& repeat=*checked->After().trace;
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
        for(std::size_t input=0;input<checked->Controls().size();++input)
            witness.omissions.push_back({input,project(*checked->Controls()[input].trace)});
        DeclaredWindowBasinSpecification geometry;geometry.inputCount=s.stimulusIds.size();geometry.assignedInput=s.assignedInput;
        geometry.windowBegin=full.observation.begin+spec.windowBeginOffset;
        geometry.windowEnd=full.observation.begin+spec.windowEndOffset;geometry.minimumAmplitude=s.minimumAmplitude;
        NativeRequire(std::isfinite(geometry.windowBegin) && std::isfinite(geometry.windowEnd),"basin_native_window_arithmetic_nonfinite");
        auto measured=SelectDeclaredWindowBasinMaximum(geometry,policy,witness);
        const double onset=full.observation.events[s.assignedInput].time;
        if(measured.validity==DeclaredWindowBasinValidity::Valid) {
            NativeRequire(bool(measured.measuredMaximum),"basin_native_valid_geometry_has_no_peak");
            if(measured.measuredMaximum->firstSample<checked->Before().eventFrames[s.assignedInput] ||
                measured.measuredMaximum->time<onset) {
                result.status=NativeDeclaredWindowBasinStatus::CausalityRejected;
                result.reasons.push_back("basin_native_selected_peak_precedes_actual_assigned_event");return result;
            }
        }
        result.geometry=std::move(measured);result.actualAssignedOnset=onset;result.status=NativeDeclaredWindowBasinStatus::Completed;
    }
    catch(const MatchedOmissionDetail::Failure& failure){result.reasons.push_back(failure.reason);}
    catch(const std::exception& error){result.reasons.push_back(error.what());}
    return result;
}
}
