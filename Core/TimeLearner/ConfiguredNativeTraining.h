#pragma once
#include "NativeModelBuilder.h"
#include "NativeTrainingService.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
struct ConfiguredNativeEvidenceSinks {
    std::function<void(const PreparedVerificationProbe&,const ProbeSpec&)> onPrepared;
    std::function<void(const NativeProbeSample&)> onSample;
    std::function<void(const NativeProbeResult&)> onObservation;
};
// Portable cold-run declaration. Runtime callbacks, model hashes, references
// and certificates are deliberately not configuration or implicit checkpoints.
struct ConfiguredNativeTraining {
    NativeModelSpec model;
    StimulusProfile stimulusProfile;
    std::string runId,protocolRevision,sampleId="target";
    double duration=0.,maximumSampleGap=0.,minimumAmplitude=1e-6;
    std::size_t maximumSamples=0;
    bool recordSpikes=false;
    PeakSelectionPolicy peakSelection=PeakSelectionPolicy::IsolatedWindow;
    std::vector<std::pair<double,double>> windows;
    BoundedTrainingConfig search;
    std::size_t maximumReferenceProbes=0,maximumInvalidReferenceProbes=0;
    double maximumModelSeconds=0.,maximumWallSeconds=0.;
};
QJsonObject EncodeNativeModelSpec(const NativeModelSpec& model);
NativeModelSpec DecodeNativeModelSpec(const QJsonObject& object);
QJsonObject EncodeConfiguredNativeTraining(const ConfiguredNativeTraining& configuration);
ConfiguredNativeTraining DecodeConfiguredNativeTraining(const QJsonObject& object);
// Reject duplicate keys (including escaped spellings), unknown fields, invalid
// types/nonfinite numbers and unsupported schema. Parsing never touches Rdk.
ConfiguredNativeTraining ParseConfiguredNativeTraining(const std::string& text);
NativeTrainingPlan MakeConfiguredNativeTrainingPlan(const ConfiguredNativeTraining& configuration);
}
