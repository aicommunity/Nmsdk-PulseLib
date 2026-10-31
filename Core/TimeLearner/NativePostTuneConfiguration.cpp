#include "NativePostTuneConfiguration.h"
#include <QJsonArray>
#include <cmath>
#include <utility>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
void Keys(const QJsonObject& object,std::initializer_list<const char*> names)
{
    Require(object.size()==int(names.size()),"job_field_set_mismatch");
    for(auto name:names)Require(object.contains(name),"job_missing_field");
}
double Number(const QJsonValue& value)
{Require(value.isDouble() && std::isfinite(value.toDouble()),"job_finite_number_required");return value.toDouble();}
double Numeric(double value)
{Require(std::isfinite(value),"job_finite_number_required");return value;}
std::size_t Count(const QJsonValue& value,std::size_t maximum)
{
    const auto n=Number(value);
    Require(n>=1. && n<=double(maximum) && std::floor(n)==n,"job_count_out_of_range");return std::size_t(n);
}
const char* Mode(NativePostTuneControllerMode mode)
{
    switch(mode){case NativePostTuneControllerMode::Off:return "off";
    case NativePostTuneControllerMode::KeepDone:return "keep_done";
    case NativePostTuneControllerMode::CanonRmin:return "canon_rmin";
    case NativePostTuneControllerMode::FlatLastR:return "flat_last_r";
    case NativePostTuneControllerMode::SearchSynthetic:return "search_synthetic";}
    throw std::invalid_argument("unknown_post_tune_job_mode");
}
const char* Metric(NativePostTuneMetric metric)
{
    switch(metric){case NativePostTuneMetric::Auto:return "auto";
    case NativePostTuneMetric::LTZone:return "ltzone";
    case NativePostTuneMetric::SomaSum:return "soma_sum";}
    throw std::invalid_argument("unknown_post_tune_job_metric");
}
}
NativePostTuneTransactionSpec DecodeNativePostTuneControls(const QJsonObject& p,NativeThresholdGridSpec calibration)
{
    Keys(p,{"mode","metric","canonical_floor","canonical_last","resistance_bounds","search_passes",
        "maximum_candidates","maximum_recorded_samples","minimum_positive_gap"});
    NativePostTuneTransactionSpec spec;bool found=false;
    for(auto mode:{NativePostTuneControllerMode::Off,NativePostTuneControllerMode::KeepDone,
        NativePostTuneControllerMode::CanonRmin,NativePostTuneControllerMode::FlatLastR,NativePostTuneControllerMode::SearchSynthetic})
        if(p["mode"]==QJsonValue(Mode(mode))){spec.mode=mode;found=true;}
    Require(found,"unknown_post_tune_job_mode");found=false;
    for(auto metric:{NativePostTuneMetric::Auto,NativePostTuneMetric::LTZone,NativePostTuneMetric::SomaSum})
        if(p["metric"]==QJsonValue(Metric(metric))){spec.metric=metric;found=true;}
    Require(found,"unknown_post_tune_job_metric");
    spec.canonicalFloor=Number(p["canonical_floor"]);spec.canonicalLast=Number(p["canonical_last"]);
    spec.minimumPositiveGap=Number(p["minimum_positive_gap"]);
    spec.searchPasses=unsigned(Count(p["search_passes"],128));
    spec.maximumCandidates=Count(p["maximum_candidates"],4096);
    spec.maximumRecordedSamples=Count(p["maximum_recorded_samples"],100000000);
    Require(p["resistance_bounds"].isArray(),"job_array_required");
    for(const auto& value:p["resistance_bounds"].toArray())
    {
        Require(value.isObject(),"job_object_required");const auto bounds=value.toObject();
        Keys(bounds,{"minimum","maximum"});
        spec.resistanceBounds.push_back({Number(bounds["minimum"]),Number(bounds["maximum"])});
    }
    spec.calibration=std::move(calibration);return spec;
}
QJsonObject EncodeNativePostTuneControls(const NativePostTuneTransactionSpec& spec)
{
    Require(!spec.weightProbeThreshold,"post_tune_controls_requires_explicit_weight_threshold_schema");
    Require(spec.finalization==NativePostTuneFinalization::Calibrate,"post_tune_controls_v1_requires_calibration");
    QJsonArray bounds;
    for(const auto& b:spec.resistanceBounds)bounds.append(QJsonObject{{"minimum",Numeric(b.minimum)},{"maximum",Numeric(b.maximum)}});
    QJsonObject result{{"mode",Mode(spec.mode)},{"metric",Metric(spec.metric)},
        {"canonical_floor",Numeric(spec.canonicalFloor)},{"canonical_last",Numeric(spec.canonicalLast)},
        {"resistance_bounds",bounds},{"search_passes",double(spec.searchPasses)},
        {"maximum_candidates",double(spec.maximumCandidates)},{"maximum_recorded_samples",double(spec.maximumRecordedSamples)},
        {"minimum_positive_gap",Numeric(spec.minimumPositiveGap)}};
    DecodeNativePostTuneControls(result,{});return result;
}
QJsonObject EncodeNativePostTuneControlsV2(const NativePostTuneTransactionSpec& spec)
{
    Require(spec.finalization==NativePostTuneFinalization::Calibrate ||
        spec.finalization==NativePostTuneFinalization::KeepFixedThreshold,"unknown_post_tune_finalization");
    auto legacy=spec;legacy.finalization=NativePostTuneFinalization::Calibrate;
    auto result=EncodeNativePostTuneControls(legacy);result["schema_version"]=2;
    result["finalization"]=spec.finalization==NativePostTuneFinalization::Calibrate?"calibrate":"keep_fixed_threshold";
    return result;
}
NativePostTuneTransactionSpec DecodeNativePostTuneControlsV2(const QJsonObject& controls,NativeThresholdGridSpec calibration)
{
    Keys(controls,{"schema_version","finalization","mode","metric","canonical_floor","canonical_last",
        "resistance_bounds","search_passes","maximum_candidates","maximum_recorded_samples","minimum_positive_gap"});
    Require(Number(controls["schema_version"])==2.,"unsupported_post_tune_controls_schema");
    Require(controls["finalization"]==QJsonValue("calibrate") ||
        controls["finalization"]==QJsonValue("keep_fixed_threshold"),"unknown_post_tune_finalization");
    auto legacy=controls;legacy.remove("schema_version");legacy.remove("finalization");
    auto result=DecodeNativePostTuneControls(legacy,std::move(calibration));
    result.finalization=controls["finalization"]==QJsonValue("calibrate")?
        NativePostTuneFinalization::Calibrate:NativePostTuneFinalization::KeepFixedThreshold;
    return result;
}
}
