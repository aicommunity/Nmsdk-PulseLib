#include "NativeBasinValidation.h"
#include "MatchedOmissionEvidenceJson.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <cmath>
#include <stdexcept>

namespace NMSDK::TimeLearner::NativeBasinDetail {
namespace {
void NativeRequire(bool value,const char* reason)
{if(!value)throw std::invalid_argument(reason);}
std::string PatternHash(const std::vector<double>& isi) {
    QJsonArray values;for(double value:isi){NativeRequire(std::isfinite(value),"basin_native_nonfinite_source_isi");values.append(value);}
    return NmsdkTools::CanonicalModelDataSha256(values).toStdString();
}
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
