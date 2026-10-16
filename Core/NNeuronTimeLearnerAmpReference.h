#ifndef NNEURONTIMELEARNERAMPREFERENCE_H
#define NNEURONTIMELEARNERAMPREFERENCE_H

#include <cmath>

namespace NMSDK {
namespace TimeLearnerAmpReference {

/// Capture the completed first peak window from the single-synapse unit cable.
/// Capturing while the signal is rising would freeze a sub-peak reference.
/// Later structural or cross-input changes must not move the normalization target.
inline bool ShouldCaptureFinalizedSingleSynapsePeakReference(
 bool training_phase_allows_capture,
 bool unit_length_cable,
 int synapse_count,
 bool measurement_window_closed,
 bool valid_peak_seen,
 double finalized_peak,
 double current_reference)
{
 return training_phase_allows_capture
  && unit_length_cable
  && synapse_count == 1
  && measurement_window_closed
  && valid_peak_seen
  && current_reference <= 0.0
  && std::isfinite(finalized_peak)
  && finalized_peak > 0.0;
}

} // namespace TimeLearnerAmpReference
} // namespace NMSDK

#endif
