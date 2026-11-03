#pragma once
#include "NativeCapacitanceEdit.h"
#include "NativeModelBuilder.h"
#include "TrainingCapacitance.h"

namespace NMSDK::TimeLearner {
// Strict signed native parameter-state codec; does not claim live readback.
TrainingCapacitance DecodeNativeTrainingCapacitance(const QJsonObject& state);
// Read the actual native owner at a legal physical boundary. Absence is
// unknown/unbound, never an invented unit baseline. No capture or live writes.
std::optional<TrainingCapacitance> ReadNativeTrainingCapacitance(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned);
// Translate the complete absolute controller map back to the existing joint
// native editor. Current C0 witnesses stay owned by the component; the editor
// partitions active/dormant paths on its actual prospective graph and captures
// new channels from verified images. This function never changes the model.
std::optional<NativeCapacitanceCorrection> ResolveNativeTrainingCapacitance(
    const NativeProbeExecutor& executor,const NativeProbeBindings& bindings,
    const std::optional<NativeModelSpec>& declaration,const QJsonObject& owned,
    const std::optional<TrainingCapacitance>& requested);
}
