#ifndef NNEURONTIMELEARNERAMPREFERENCE_H
#define NNEURONTIMELEARNERAMPREFERENCE_H

namespace NMSDK {
namespace TimeLearnerAmpReference {

/// Initialize the amplitude reference once from the single-synapse unit cable.
/// Later structural or cross-input changes must not move the normalization target.
inline bool ShouldInitializeSingleSynapsePeakReference(
 bool training_phase_allows_capture,
 bool unit_length_cable,
 int synapse_count,
 double measured_peak,
 double current_reference)
{
 return training_phase_allows_capture
  && unit_length_cable
  && synapse_count == 1
  && current_reference <= 0.0
  && measured_peak > current_reference;
}

} // namespace TimeLearnerAmpReference
} // namespace NMSDK

#endif
