#pragma once
#include "ConfiguredNativeTraining.h"
#include "ModelArchive.h"
#include "ProbeEvidenceJson.h"
#include "NativeCapacitanceEdit.h"

namespace NMSDK::TimeLearner {
struct PreparedNativeLearnedState {
    RDK::UEPtr<RDK::UStorage> storage;
    RDK::UEPtr<NPulseNeuron> neuron;
    RDK::UEPtr<NDatasetMatrix> dataset;
    ConfiguredNativeTraining configuration;
    std::vector<InputParameters> parameters;
    NativeTrainingService service;
    std::string configurationText,modelRevision;
    QJsonObject capacitanceState;
    int controllerVersion=1,calculateMode=0,structureBuildMode=1;
    ~PreparedNativeLearnedState();
};
// Certified completed or explicitly stopped stage boundary. A stopped package
// has diagnostic state_kind and null certificate, preserving incomplete capture
// and observations without a success claim. Never a mid-probe dynamics checkpoint.
QJsonObject ExportNativeLearnedState(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& parameters,int controllerVersion,int calculateMode,int structureBuildMode,
    const std::string& implementationId);
// Schema 2 adds the immutable per-channel C0 and full absolute correction map.
// The physical archive and certificate remain bound to the current graph.
QJsonObject ExportNativeLearnedState(RDK::UNet& owner,const NativeModelSpec& declaration,
    const NativeTrainingService& service,const std::vector<InputParameters>& parameters,int controllerVersion,int calculateMode,int structureBuildMode,
    const std::string& implementationId,const NativeCapacitanceCorrection& capacitance);
QJsonObject ParseNativeLearnedState(const std::string& text);
std::unique_ptr<PreparedNativeLearnedState> PrepareNativeLearnedState(const QJsonObject& package,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
}
