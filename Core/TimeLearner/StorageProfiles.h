#pragma once

#include "../../../../Rdk/Core/Engine/UClassImageRegistry.h"
#include <optional>

namespace NMSDK { namespace TimeLearner {

// Uniform physical recipes. Heterogeneous body/LT physics uses separate recipes;
// individual learned changes remain in model state, never in these prototypes.
struct ChannelProfile
{
 std::string baseClass;
 double capacity = 1e-9;
 double resistance = 1e7;
 double restingResistance = 1e7;
};

struct SynapseProfile
{
 std::string baseClass = "NPSynapseBio";
 double resistance = 86e6;
 double secretionTC = .001;
 double dissociationTC = .001;
 double pulseDuration = .001;
 double pulseAmplitude = 1.;
 bool usePulseSignal = true;
 bool usePresynapticInhibition = false;
 double inhibitionCoefficient = 0.;
};

struct MembraneProfile
{
 std::string baseClass = "NPMembraneBio";
 ChannelProfile excChannel{"NPExcChannelBio", 1e-9, 1e7, 1e7};
 ChannelProfile inhChannel{"NPInhChannelBio", 1e-9, 1e8, 1e8};
 SynapseProfile excSynapse;
 SynapseProfile inhSynapse{"NPSynapseBio", 86e7};
 bool useAveragePotential = true;
};

struct StorageProfileSpec
{
 std::string name;
 unsigned int frequencyHz = 2000;
 std::string neuronBaseClass = "NSPNeuronGenAsymRmD001C1e9";
 std::string ltZoneBaseClass = "NPulseLTZoneThreshold";
 std::string positiveGeneratorBaseClass = "NPNeuronPosCGenerator";
 std::string negativeGeneratorBaseClass = "NPNeuronNegCGenerator";
 double initialThreshold = .0117;
 MembraneProfile body;
 std::optional<MembraneProfile> ltMembrane;
};

struct RegisteredStorageProfile
{
 std::string neuronClass;
 std::string bodyMembraneClass;
 std::string ltMembraneClass;
 std::vector<std::string> classNames; // dependency order, for native export.
};

// Native storage setup before constructing/running a learner. Throws a typed
// ClassImageError on unsupported templates, collisions or inconsistent physics.
// Templates retain parameters not explicitly overridden by the recipe.
RDK_LIB_TYPE RegisteredStorageProfile RegisterStorageProfile(
 RDK::UEPtr<RDK::UStorage> storage, RDK::UEPtr<RDK::UEnvironment> environment,
 const StorageProfileSpec& specification);

} }
