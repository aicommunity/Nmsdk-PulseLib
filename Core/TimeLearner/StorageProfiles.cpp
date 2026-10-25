#include "StorageProfiles.h"
#include "../NPulseChannel.h"
#include "../NPulseSynapse.h"
#include "../NPulseMembrane.h"
#include "../NPulseNeuron.h"
#include "../NConstGenerator.h"
#include "../../../../Rdk/Core/Engine/UEnvironment.h"
#include <cmath>
#include <functional>

namespace NMSDK { namespace TimeLearner {
namespace {
using namespace RDK;

struct Lease
{
 UEPtr<UStorage> storage;
 UEPtr<UContainer> object;
 ~Lease() { if(object) storage->ReturnObject(object); }
};

void Positive(double value, const std::string& field)
{
 if(!std::isfinite(value) || value <= 0.) throw ClassImageError("invalid_profile_value", field);
}

template<class T>
UEPtr<T> Take(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment,
             const std::string& name, unsigned int frequency)
{
 if(name.empty() || !storage->CheckClass(name)) throw ClassImageError("missing_profile_template", name);
 auto base = dynamic_pointer_cast<UContainer>(storage->TakeObject(name));
 auto typed = dynamic_pointer_cast<T>(base);
 if(!typed)
 {
  if(base) storage->ReturnObject(base);
  throw ClassImageError("unsupported_profile_template", name);
 }
 typed->SetEnvironment(environment);
 typed->SetGlobalTimeStep(frequency);
 return typed;
}

void CheckFrequency(UEPtr<UContainer> component, unsigned int frequency)
{
 if(component->GetTimeStep() != frequency)
  throw ClassImageError("profile_frequency_mismatch", component->GetName());
 std::vector<NameT> names;
 component->GetComponentsList(names);
 for(const auto& name : names) CheckFrequency(component->GetComponent(name, true), frequency);
}

template<class T, class Configure>
void Image(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment, unsigned int frequency,
           const std::string& name, const std::string& base, Configure configure,
           RegisteredStorageProfile& result)
{
 auto prototype = Take<T>(storage, environment, base, frequency);
 Lease lease{storage, prototype};
 configure(prototype);
 prototype->Build();
 // Construction can add children from template images with another frequency.
 // Apply the declared single rate once at this legal construction boundary.
 prototype->SetGlobalTimeStep(frequency);
 prototype->Init();
 CheckFrequency(prototype, frequency);
 RegisterClassImage(storage, name, prototype);
 result.classNames.push_back(name);
}

void Validate(const MembraneProfile& membrane)
{
 for(const auto* channel : {&membrane.excChannel, &membrane.inhChannel})
 {
  Positive(channel->capacity, "capacity"); Positive(channel->resistance, "resistance");
  Positive(channel->restingResistance, "restingResistance");
 }
 for(const auto* synapse : {&membrane.excSynapse, &membrane.inhSynapse})
 {
  Positive(synapse->resistance, "synapse resistance"); Positive(synapse->secretionTC, "secretionTC");
  Positive(synapse->dissociationTC, "dissociationTC"); Positive(synapse->pulseDuration, "pulseDuration");
  Positive(synapse->pulseAmplitude, "pulseAmplitude");
  if(!std::isfinite(synapse->inhibitionCoefficient) || synapse->inhibitionCoefficient < 0.)
   throw ClassImageError("invalid_profile_value", "inhibitionCoefficient");
 }
}

void PreflightMembrane(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment,
                       const MembraneProfile& specification, unsigned int frequency)
{
 Validate(specification);
 auto membrane = Take<NPulseMembrane>(storage, environment, specification.baseClass, frequency);
 Lease membraneLease{storage, membrane};
 for(const auto& role : {std::make_pair(&specification.excChannel, -1), std::make_pair(&specification.inhChannel, 1)})
 {
  auto channel = Take<NPulseChannel>(storage, environment, role.first->baseClass, frequency);
  Lease lease{storage, channel};
  if(channel->Type.GetData() != role.second) throw ClassImageError("wrong_profile_channel_role", role.first->baseClass);
 }
 for(const auto* synapse : {&specification.excSynapse, &specification.inhSynapse})
 {
  auto object = Take<NPulseSynapse>(storage, environment, synapse->baseClass, frequency);
  Lease lease{storage, object};
 }
}

void ChannelImage(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment, unsigned int frequency,
                  const std::string& name, const ChannelProfile& specification, RegisteredStorageProfile& result)
{
 Image<NPulseChannel>(storage, environment, frequency, name, specification.baseClass,
  [&](UEPtr<NPulseChannel> channel) {
   channel->Capacity = specification.capacity;
   channel->Resistance = specification.resistance;
   channel->RestingResistance = specification.restingResistance;
   if(channel->Capacity.GetData() != specification.capacity || channel->Resistance.GetData() != specification.resistance ||
      channel->RestingResistance.GetData() != specification.restingResistance)
    throw ClassImageError("profile_parameter_not_applied", name);
  }, result);
}

void SynapseImage(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment, unsigned int frequency,
                  const std::string& name, const SynapseProfile& specification, int type, RegisteredStorageProfile& result)
{
 Image<NPulseSynapse>(storage, environment, frequency, name, specification.baseClass,
  [&](UEPtr<NPulseSynapse> synapse) {
   synapse->Type = type;
   synapse->Resistance = specification.resistance;
   synapse->SecretionTC = specification.secretionTC;
   synapse->DissociationTC = specification.dissociationTC;
   synapse->TypicalPulseDuration = specification.pulseDuration;
   synapse->PulseAmplitude = specification.pulseAmplitude;
   synapse->UsePulseSignal = specification.usePulseSignal;
   synapse->InhibitionCoeff = specification.inhibitionCoefficient;
   synapse->UsePresynapticInhibition = specification.usePresynapticInhibition;
   if(synapse->Resistance.GetData() != specification.resistance || synapse->SecretionTC.GetData() != specification.secretionTC ||
      synapse->DissociationTC.GetData() != specification.dissociationTC || synapse->TypicalPulseDuration.GetData() != specification.pulseDuration ||
      synapse->PulseAmplitude.GetData() != specification.pulseAmplitude || synapse->UsePulseSignal.GetData() != specification.usePulseSignal ||
      synapse->InhibitionCoeff.GetData() != specification.inhibitionCoefficient ||
      synapse->UsePresynapticInhibition.GetData() != specification.usePresynapticInhibition)
    throw ClassImageError("profile_parameter_not_applied", name);
  }, result);
}

std::string MembraneImage(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment, unsigned int frequency,
                          const std::string& prefix, const MembraneProfile& specification, RegisteredStorageProfile& result)
{
 const auto excChannel = prefix + "_ExcChannel", inhChannel = prefix + "_InhChannel";
 const auto excSynapse = prefix + "_ExcSynapse", inhSynapse = prefix + "_InhSynapse";
 ChannelImage(storage, environment, frequency, excChannel, specification.excChannel, result);
 ChannelImage(storage, environment, frequency, inhChannel, specification.inhChannel, result);
 SynapseImage(storage, environment, frequency, excSynapse, specification.excSynapse, -1, result);
 SynapseImage(storage, environment, frequency, inhSynapse, specification.inhSynapse, 1, result);
 const auto name = prefix + "_Membrane";
 Image<NPulseMembrane>(storage, environment, frequency, name, specification.baseClass,
  [&](UEPtr<NPulseMembrane> membrane) {
   membrane->ExcChannelClassName = excChannel; membrane->InhChannelClassName = inhChannel;
   membrane->ExcSynapseClassName = excSynapse; membrane->InhSynapseClassName = inhSynapse;
   membrane->UseAveragePotential = specification.useAveragePotential;
  }, result);
 return name;
}
}

RegisteredStorageProfile RegisterStorageProfile(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment,
                                                const StorageProfileSpec& specification)
{
 if(!storage || !environment || specification.name.empty() || specification.frequencyHz == 0)
  throw ClassImageError("invalid_profile_specification", specification.name);
 Positive(specification.initialThreshold, "initialThreshold");
 PreflightMembrane(storage, environment, specification.body, specification.frequencyHz);
 if(specification.ltMembrane)
 {
  PreflightMembrane(storage, environment, *specification.ltMembrane, specification.frequencyHz);
  if(specification.ltMembrane->useAveragePotential != specification.body.useAveragePotential)
   throw ClassImageError("unsupported_heterogeneous_averaging", specification.name);
 }
 // Check all template capabilities before publishing the first dependency.
 auto neuron = Take<NPulseNeuron>(storage, environment, specification.neuronBaseClass, specification.frequencyHz);
 Lease neuronLease{storage, neuron};
 if(neuron->StructureBuildMode.GetData() != 1 && neuron->StructureBuildMode.GetData() != 2)
  throw ClassImageError("explicit_graph_requires_model_import", specification.neuronBaseClass);
 {
  auto zone = Take<NLTZone>(storage, environment, specification.ltZoneBaseClass, specification.frequencyHz);
  Lease lease{storage, zone};
 }
 for(const auto& role : {std::make_pair(specification.positiveGeneratorBaseClass, 1),
                         std::make_pair(specification.negativeGeneratorBaseClass, -1)})
 {
  auto generator = Take<NConstGenerator>(storage, environment, role.first, specification.frequencyHz);
  Lease lease{storage, generator};
  if(!std::isfinite(generator->Amplitude.GetData()) || generator->Amplitude.GetData() * role.second <= 0.)
   throw ClassImageError("wrong_profile_generator_role", role.first);
 }

 RegisteredStorageProfile result;
 result.bodyMembraneClass = MembraneImage(storage, environment, specification.frequencyHz, specification.name + "_Body", specification.body, result);
 if(specification.ltMembrane)
  result.ltMembraneClass = MembraneImage(storage, environment, specification.frequencyHz, specification.name + "_LT", *specification.ltMembrane, result);
 const auto zoneName = specification.name + "_LTZone";
 Image<NLTZone>(storage, environment, specification.frequencyHz, zoneName, specification.ltZoneBaseClass,
  [&](UEPtr<NLTZone> zone) {
   zone->Threshold = specification.initialThreshold;
   zone->UseAveragePotential = neuron->UseAverageLTZonePotential.GetData();
   if(zone->Threshold.GetData() != specification.initialThreshold)
    throw ClassImageError("profile_parameter_not_applied", zoneName);
  }, result);
 const auto positive = specification.name + "_PositiveGenerator", negative = specification.name + "_NegativeGenerator";
 Image<NConstGenerator>(storage, environment, specification.frequencyHz, positive, specification.positiveGeneratorBaseClass,
  [](UEPtr<NConstGenerator>) {}, result);
 Image<NConstGenerator>(storage, environment, specification.frequencyHz, negative, specification.negativeGeneratorBaseClass,
  [](UEPtr<NConstGenerator>) {}, result);

 neuron->UseElementDefaults = false;
 neuron->MembraneCapacity = 0.; neuron->SynapseDissociationTC = 0.; neuron->SynapseSecretionTC = 0.;
 neuron->ExcMembraneResistance = 0.; neuron->InhMembraneResistance = 0.;
 neuron->ExcSynapseResistance = 0.; neuron->InhSynapseResistance = 0.; neuron->SynapseTypicalPulseDuration = 0.;
 neuron->MembraneClassName = result.bodyMembraneClass;
 neuron->LTMembraneClassName = result.ltMembraneClass;
 neuron->LTZoneClassName = zoneName;
 neuron->ExcGeneratorClassName = positive; neuron->InhGeneratorClassName = negative;
 neuron->UseAverageDendritesPotential = specification.body.useAveragePotential;
 neuron->Build();
 neuron->SetGlobalTimeStep(specification.frequencyHz);
 neuron->Init();
 CheckFrequency(neuron, specification.frequencyHz);
 result.neuronClass = specification.name + "_Neuron";
 RegisterClassImage(storage, result.neuronClass, neuron);
 result.classNames.push_back(result.neuronClass);
 return result;
}
} }
