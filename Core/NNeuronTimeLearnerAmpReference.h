#ifndef NNEURONTIMELEARNERAMPREFERENCE_H
#define NNEURONTIMELEARNERAMPREFERENCE_H

namespace NMSDK {
namespace TimeLearnerAmpReference {

/// The amplitude reference describes the configured single-synapse short cable.
/// Adding structural contacts must not raise the normalization target.
inline bool ShouldCaptureSingleSynapsePeak(
 bool training_phase_allows_capture,
 bool unit_length_cable,
 int synapse_count,
 double measured_peak,
 double current_reference)
{
 return training_phase_allows_capture
  && unit_length_cable
  && synapse_count == 1
  && measured_peak > current_reference;
}

} // namespace TimeLearnerAmpReference
} // namespace NMSDK

#endif
