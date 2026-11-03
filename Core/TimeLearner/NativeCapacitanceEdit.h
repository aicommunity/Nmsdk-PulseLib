#pragma once
#include "NativeModelEditor.h"
#include <QJsonObject>
#include <map>

namespace NMSDK::TimeLearner {
struct NativeModelSpec;
struct NativeCapacitanceCorrection {
    QJsonObject baseline;
    std::map<std::string,double> membraneScales;
    double maximumRelativeChange=0.;
    // Schema 2 parameter state retains learned scales of removed membranes.
    // They are never applied until native geometry contains that path again.
    std::map<std::string,double> dormantMembraneScales;
};
struct NativeCapacitanceEditResult {
    NativeEditStatus status=NativeEditStatus::Rejected;
    std::string reason,beforeRevision,afterRevision;
    bool modelMayHaveChanged=false;
    QJsonObject appliedChannels;
    // Prepared and validated on the preview before changing the live graph.
    QJsonObject capacitanceState;
};
// Immutable per-channel effective C0, including independently configured LT,
// soma, Exc and Inh channels. No parent default or arbitrary fallback.
QJsonObject CaptureNativeCapacitanceBaseline(const NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings);
// Version 2 binds effective C0 to the declared model, original semantic graph
// and immutable storage image closure. Version 1 remains a legacy primitive.
QJsonObject CaptureBoundNativeCapacitanceBaseline(const NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings,const NativeModelSpec& declaration);
QJsonObject EncodeNativeCapacitanceState(const NativeCapacitanceCorrection& correction,
    const std::string& appliedModelRevision);
NativeCapacitanceCorrection ParseNativeCapacitanceState(const std::string& text,
    std::string& appliedModelRevision);
// Read-only validation against an already constructed/imported physical graph;
// this is parameter state, not a controller or mid-probe dynamics checkpoint.
void ValidateAppliedNativeCapacitanceState(const NativeProbeBindings& bindings,
    const NativeModelSpec& declaration,const QJsonObject& state);
// Absolute multipliers relative to the supplied baseline, not current C.
// Each selected dendritic membrane scales its existing channels separately.
// A new segment without an explicit baseline is rejected by the standalone
// edit. A bound joint structural edit may extend C0 from a verified image.
// The caller owns controller/frozen-state exclusion and persistence of baseline.
NativeCapacitanceEditResult ApplyNativeCapacitanceScale(NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings,const QJsonObject& baseline,
    const std::map<std::string,double>& membraneScales,double maximumRelativeChange);
NativeCapacitanceEditResult ApplyNativeCapacitanceScale(NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings,const NativeCapacitanceCorrection& correction);
}
