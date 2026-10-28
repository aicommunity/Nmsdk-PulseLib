#pragma once
namespace NMSDK::TimeLearner {
enum class Normalization { SynapseCount, TipResistance };
struct InputParameters {
    // Effective physical banks, including shared Branch attachment aliases.
    int length=1,synapses=1;
    double resistance=0.;
    bool operator==(const InputParameters&) const=default;
};
}
