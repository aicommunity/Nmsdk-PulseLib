#pragma once
#include "NativeProbe.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
// Full internal measurement evidence, distinct from the recognition CSV schema.
// Nonfinite numbers and integers not exactly representable in JSON are errors;
// they must never become an apparently valid null/rounded measurement.
QJsonObject EncodeNativeProbeEvidence(const NativeProbeResult& result);
}
