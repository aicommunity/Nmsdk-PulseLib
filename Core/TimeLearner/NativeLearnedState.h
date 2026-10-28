#pragma once
#include "ConfiguredNativeTraining.h"
#include "ModelArchive.h"
#include "ProbeEvidenceJson.h"

namespace NMSDK::TimeLearner {
struct PreparedNativeLearnedState {
    RDK::UEPtr<RDK::UStorage> storage;
    RDK::UEPtr<NPulseNeuron> neuron;
    RDK::UEPtr<NDatasetMatrix> dataset;
    ConfiguredNativeTraining configuration;
    std::vector<InputParameters> parameters;
    NativeTrainingService service;
    std::string configurationText,modelRevision;
    int controllerVersion=1,calculateMode=0,structureBuildMode=1;
    ~PreparedNativeLearnedState();
};
// Complete certified completed-stage boundary, distinct from physical archive,
// parameter overlay and a mid-probe dynamics checkpoint. Exact is recomputed.
QJsonObject ExportNativeLearnedState(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& parameters,int controllerVersion,int calculateMode,int structureBuildMode,
    const std::string& implementationId);
QJsonObject ParseNativeLearnedState(const std::string& text);
std::unique_ptr<PreparedNativeLearnedState> PrepareNativeLearnedState(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
}
