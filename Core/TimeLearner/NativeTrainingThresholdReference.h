#pragma once
#include "NativeSoloThresholdTrace.h"

namespace NMSDK::TimeLearner {
enum class NativeTrainingThresholdMode { GapFraction=0, PeakFraction=1 };
struct NativeTrainingThresholdSpec {
    NativeTrainingThresholdMode mode=NativeTrainingThresholdMode::GapFraction;
    double fraction=.7,minimum=0.,maximum=1.;
};

// A target-only Solo reference proposal, not the target/foil calibration
// criterion and not a threshold acceptance certificate. Legacy extrema select
// strictly positive after-children samples; baseline and post-spike recovery
// are never silently substituted into that mathematical definition.
inline QJsonObject ProposeNativeTrainingThreshold(const NativeSoloThresholdTrace& trace,
    const NativeTrainingThresholdSpec& spec,const std::string& currentBaseRevision)
{
    if((spec.mode!=NativeTrainingThresholdMode::GapFraction && spec.mode!=NativeTrainingThresholdMode::PeakFraction) ||
        !Detail::Finite(spec.fraction) || !Detail::Finite(spec.minimum) || !Detail::Finite(spec.maximum) ||
        spec.minimum>spec.maximum || currentBaseRevision.empty())
        throw std::invalid_argument("invalid_training_threshold_reference_specification");
    const auto source=trace.ClosedReport();
    const auto descriptor=source["source"].toObject();
    QJsonObject proposal{{"kind","native_training_threshold_reference"},{"schema_version",1},
        {"source_trace_sha256",source["sha256"]},{"source_model_revision",descriptor["base_model_revision"].toString()},
        {"current_model_revision",QString::fromStdString(currentBaseRevision)},
        {"mode",spec.mode==NativeTrainingThresholdMode::GapFraction?"gap_fraction":"peak_fraction"},
        {"fraction",spec.fraction},{"minimum_threshold",spec.minimum},{"maximum_threshold",spec.maximum},
        {"sample_selection","strictly_positive_after_children"},{"minimum_span",1e-9},
        {"positive_samples",0},{"positive_minimum",QJsonValue(QJsonValue::Null)},
        {"positive_maximum",QJsonValue(QJsonValue::Null)},{"unclamped_threshold",QJsonValue(QJsonValue::Null)},
        {"threshold",QJsonValue(QJsonValue::Null)},{"clipped",false},{"reason","source_not_eligible"},
        {"requires_spiking_confirmation",true},{"calibrated",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
    const auto finish=[&]() {proposal["sha256"]=NmsdkTools::CanonicalModelDataSha256(proposal);return proposal;};
    if(!source["proposal_eligible"].toBool())return finish();
    if(descriptor["base_model_revision"].toString()!=QString::fromStdString(currentBaseRevision)) {
        proposal["reason"]="source_model_changed";return finish();
    }
    std::optional<double> low,high;int count=0;
    for(const auto& frame:source["samples"].toArray()) {
        const double value=frame.toObject()["lt_potential"].toDouble();
        if(value>0.) {++count;low=low?std::min(*low,value):value;high=high?std::max(*high,value):value;}
    }
    proposal["positive_samples"]=count;
    if(low)proposal["positive_minimum"]=*low;
    if(high)proposal["positive_maximum"]=*high;
    if(!low || !high || *high<=*low+1e-9) {proposal["reason"]="insufficient_positive_span";return finish();}
    const double raw=spec.mode==NativeTrainingThresholdMode::PeakFraction?*high*spec.fraction:
        *low+spec.fraction*(*high-*low);
    if(!Detail::Finite(raw)) {proposal["reason"]="nonfinite_threshold_proposal";return finish();}
    const double threshold=std::max(spec.minimum,std::min(spec.maximum,raw));
    proposal["unclamped_threshold"]=raw;proposal["threshold"]=threshold;
    proposal["clipped"]=threshold!=raw;proposal["reason"]="proposed_unconfirmed";
    return finish();
}
}
