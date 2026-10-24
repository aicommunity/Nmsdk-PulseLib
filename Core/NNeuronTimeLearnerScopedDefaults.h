#ifndef NNEURONTIMELEARNER_SCOPED_DEFAULTS_H
#define NNEURONTIMELEARNER_SCOPED_DEFAULTS_H

#include "NPulseNeuron.h"
#include "NPulseChannel.h"
#include "NPulseSynapse.h"

namespace NMSDK {

/// Apply NPulseNeuron element defaults to one rebuilt membrane only.
/// NPulseNeuron::ApplyElementDefaults() walks every membrane and resets all
/// synaptic resistances, including state owned by other learner inputs.
inline void ApplyElementDefaultsToMembrane(NPulseNeuron *neuron, NPulseMembrane *membrane)
{
 if(!neuron || !membrane || !neuron->UseElementDefaults.GetData())
  return;

 const double cap = neuron->MembraneCapacity.GetData();
 const double dissoc = neuron->SynapseDissociationTC.GetData();
 const double secr = neuron->SynapseSecretionTC.GetData();
 const double tpd = neuron->SynapseTypicalPulseDuration.GetData();
 const double exc_rm = neuron->ExcMembraneResistance.GetData();
 const double inh_rm = neuron->InhMembraneResistance.GetData();
 const double exc_rsyn = neuron->ExcSynapseResistance.GetData();
 const double inh_rsyn = neuron->InhSynapseResistance.GetData();
 if(cap <= 0.0 && dissoc <= 0.0 && secr <= 0.0
    && exc_rm <= 0.0 && inh_rm <= 0.0
    && exc_rsyn <= 0.0 && inh_rsyn <= 0.0 && tpd <= 0.0)
  return;

 if(cap > 0.0 || exc_rm > 0.0 || inh_rm > 0.0)
 {
  UEPtr<UContainer> exc_c = membrane->GetComponent("ExcChannel", true);
  if(NPulseChannel *exc = dynamic_cast<NPulseChannel*>((UContainer*)exc_c))
  {
   if(cap > 0.0)
    exc->Capacity = cap;
   if(exc_rm > 0.0)
   {
    exc->Resistance = exc_rm;
    exc->RestingResistance = exc_rm;
   }
  }

  UEPtr<UContainer> inh_c = membrane->GetComponent("InhChannel", true);
  if(NPulseChannel *inh = dynamic_cast<NPulseChannel*>((UContainer*)inh_c))
  {
   if(cap > 0.0)
    inh->Capacity = cap;
   if(inh_rm > 0.0)
   {
    inh->Resistance = inh_rm;
    inh->RestingResistance = inh_rm;
   }
  }
 }

 if(dissoc > 0.0 || secr > 0.0 || exc_rsyn > 0.0 || inh_rsyn > 0.0 || tpd > 0.0)
 {
  const int nexc = int(membrane->NumExcitatorySynapses);
  const int ninh = int(membrane->NumInhibitorySynapses);
  for(int i = 0; i < nexc; ++i)
  {
   UEPtr<UContainer> syn_c = membrane->GetComponent(std::string("ExcSynapse") + sntoa(i + 1), true);
   if(NPulseSynapse *syn = dynamic_cast<NPulseSynapse*>((UContainer*)syn_c))
   {
    if(dissoc > 0.0)
     syn->DissociationTC = dissoc;
    if(secr > 0.0)
     syn->SecretionTC = secr;
    if(tpd > 0.0)
     syn->TypicalPulseDuration = tpd;
    if(exc_rsyn > 0.0)
     syn->Resistance = exc_rsyn;
   }
  }
  for(int i = 0; i < ninh; ++i)
  {
   UEPtr<UContainer> syn_c = membrane->GetComponent(std::string("InhSynapse") + sntoa(i + 1), true);
   if(NPulseSynapse *syn = dynamic_cast<NPulseSynapse*>((UContainer*)syn_c))
   {
    if(dissoc > 0.0)
     syn->DissociationTC = dissoc;
    if(secr > 0.0)
     syn->SecretionTC = secr;
    if(tpd > 0.0)
     syn->TypicalPulseDuration = tpd;
    if(inh_rsyn > 0.0)
     syn->Resistance = inh_rsyn;
   }
  }
 }
}

/// Reapply the structural-mode value to every additional synapse after a
/// membrane-local Build/default pass. ExcSynapse1 remains the configured base.
inline void SetAdditionalSynapseResistance(NPulseMembrane *membrane, double resistance)
{
 if(!membrane)
  return;
 for(int i = 2; i <= int(membrane->NumExcitatorySynapses); ++i)
 {
  if(NPulseSynapseCommon *syn = membrane->GetComponentL<NPulseSynapseCommon>(
       std::string("ExcSynapse") + sntoa(i), true))
   syn->Resistance = resistance;
 }
}

} // namespace NMSDK

#endif
