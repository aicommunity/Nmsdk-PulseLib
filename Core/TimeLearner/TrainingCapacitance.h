#pragma once
#include <map>
#include <optional>
#include <string>

namespace NMSDK::TimeLearner {
// Full absolute parameter map, including temporarily absent membrane paths.
// Native adapters retain the actual C0/image witnesses and partition active vs
// dormant paths. The pure controller must not reconstruct C0 or mutate storage.
struct TrainingCapacitance {
    std::string origin;
    double maximumRelativeChange=0.;
    std::map<std::string,double> membraneScales;
    bool operator==(const TrainingCapacitance&) const=default;
};
void ValidateTrainingCapacitance(const std::optional<TrainingCapacitance>& state);
// Unit scales of newly discovered native paths do not change a historical
// correction. Missing non-unit history, origin or policy changes do.
bool EquivalentTrainingCapacitance(const std::optional<TrainingCapacitance>& a,
    const std::optional<TrainingCapacitance>& b);
// Native growth may extend a requested complete map only with new unit scales.
// Every requested value must survive exact readback, including dormant paths.
bool TrainingCapacitanceReadbackMatches(const std::optional<TrainingCapacitance>& requested,
    const std::optional<TrainingCapacitance>& effective);
// Restore a historical candidate without losing the current C0 path inventory.
// Paths absent from that candidate return to C0, never inherit a later trim.
std::optional<TrainingCapacitance> RestoreTrainingCapacitance(
    const std::optional<TrainingCapacitance>& historical,const std::optional<TrainingCapacitance>& current);
}
