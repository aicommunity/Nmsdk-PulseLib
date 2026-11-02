#pragma once
#include "NativeModelEditor.h"
#include <QJsonObject>
#include <map>

namespace NMSDK::TimeLearner {
struct NativeCapacitanceCorrection {
    QJsonObject baseline;
    std::map<std::string,double> membraneScales;
    double maximumRelativeChange=0.;
};
struct NativeCapacitanceEditResult {
    NativeEditStatus status=NativeEditStatus::Rejected;
    std::string reason,beforeRevision,afterRevision;
    bool modelMayHaveChanged=false;
    QJsonObject appliedChannels;
};
// Immutable per-channel effective C0, including independently configured LT,
// soma, Exc and Inh channels. No parent default or arbitrary fallback.
QJsonObject CaptureNativeCapacitanceBaseline(const NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings);
// Absolute multipliers relative to the supplied baseline, not current C.
// Each selected dendritic membrane scales its existing channels separately.
// A new segment without an explicit baseline is rejected, never guessed.
// The caller owns controller/frozen-state exclusion and persistence of baseline.
NativeCapacitanceEditResult ApplyNativeCapacitanceScale(NativeProbeExecutor& executor,
    const NativeProbeBindings& bindings,const QJsonObject& baseline,
    const std::map<std::string,double>& membraneScales,double maximumRelativeChange);
}
