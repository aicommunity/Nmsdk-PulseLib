#pragma once
#include "NativeLearnedState.h"

namespace NMSDK::TimeLearner {
// Complete, freshly initialized physical children and their native declaration.
// Deliberately has no training service, certificate, search cursor or callbacks.
// Source-boundary metadata is provenance DATA, not restored proof authority.
struct PreparedNativePhysicalModelData {
    RDK::UEPtr<RDK::UStorage> storage;
    RDK::UEPtr<NPulseNeuron> neuron;
    RDK::UEPtr<NDatasetMatrix> dataset;
    ConfiguredNativeTraining configuration;
    std::string configurationText,modelRevision,sourceSoloRevision;
    std::vector<InputParameters> parameters;
    QJsonObject sourceBoundary,protocol;
    int controllerVersion=2,calculateMode=0,structureBuildMode=1;
    ~PreparedNativePhysicalModelData();
};

// Validates the complete existing solo/recognition/candidate/calibrated source
// through its normal native importer ONCE, then captures the complete physical
// model separately from observation/search history. The original evidence must
// be published separately and linked by source_boundary.sha256; this document
// alone must never satisfy a completed-training/calibration proof validator.
QJsonObject ExportNativePhysicalModelData(const QJsonObject& source,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment,
    const std::string& implementationId);
// Constructor preflight only; does not validate history or grant authority.
unsigned NativePhysicalModelSourceFrequency(const QJsonObject& source);

// Native image registration -> Dataset import -> port-bound neuron import.
// Verifies effective graph, input parameters, frequency and stimulus/target.
// No live owner replacement, Calculate, shared-prototype mutation, controller
// continuation or certificate/calibration authority is installed.
std::unique_ptr<PreparedNativePhysicalModelData> PrepareNativePhysicalModelData(
    const QJsonObject& document,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const std::string& implementationId);
}
