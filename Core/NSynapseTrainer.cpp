// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NSYNAPSE_TRAINER_CPP
#define NSYNAPSE_TRAINER_CPP

#include "NSynapseTrainer.h"
#include "NPulseNeuronCommon.h"

namespace NMSDK {

NSynapseTrainer::NSynapseTrainer(void)
: PostSynInput("PostSynInput",this),
  PreSynInput("PreSynInput",this),
  WeightOutput("WeightOutput",this)
{
}

NSynapseTrainer::~NSynapseTrainer(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NSynapseTrainer* NSynapseTrainer::New(void)
{
 return new NSynapseTrainer;
}
// --------------------------

void NSynapseTrainer::RebuildInternalLinks(void)
{
 if(MainOwner)
 {
  UEPtr<NPulseNeuronCommon> neuron=dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner);
  if(neuron)
  {
   bool res=true;
   res&=neuron->CreateLink(neuron->GetLTZone()->GetLongName(neuron),"Output",GetLongName(neuron),"PostSynInput");
   res&=neuron->CreateLink(GetOwner()->GetLongName(neuron),"OutInCopy",GetLongName(neuron),"PreSynInput");
   res&=neuron->CreateLink(GetLongName(neuron),"WeightOutput", GetOwner()->GetLongName(neuron),"WeightInput");
  }
 }
}
// --------------------------
}

#endif
