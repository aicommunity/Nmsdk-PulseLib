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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainer* NSynapseTrainer::New(void)
{
 return new NSynapseTrainer;
}
// --------------------------

void NSynapseTrainer::RebuildInternalLinks(void)
{
 auto main_owner = MainOwner.lock();
 if(main_owner)
 {
  std::shared_ptr<NPulseNeuronCommon> neuron=std::shared_ptr<NPulseNeuronCommon>(dynamic_pointer_cast<NPulseNeuronCommon>(main_owner).get());
  std::weak_ptr<RDK::UContainer> owner_weak=GetOwner();
  if(neuron && neuron->GetLTZone() && !owner_weak.expired())
  {
   std::shared_ptr<RDK::UContainer> owner=owner_weak.lock();
   if(owner)
   {
    bool res=true;
    res&=neuron->CreateLink(neuron->GetLTZone()->GetLongName(neuron),"Output",GetLongName(neuron),"PostSynInput");
    res&=neuron->CreateLink(owner->GetLongName(neuron),"OutInCopy",GetLongName(neuron),"PreSynInput");
    res&=neuron->CreateLink(GetLongName(neuron),"WeightOutput", owner->GetLongName(neuron),"WeightInput");
   }
  }
 }
}
// --------------------------
}

#endif
