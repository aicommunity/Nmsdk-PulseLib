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

#ifndef NPULSE_MEMBRANE_COMMON_CPP
#define NPULSE_MEMBRANE_COMMON_CPP

#include <algorithm>
#include "NPulseMembraneCommon.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembraneCommon::NPulseMembraneCommon(void)
 : UseAveragePotential("UseAveragePotential",this,&NPulseMembraneCommon::SetUseAveragePotential),
  Feedback("Feedback",this),
  SumPotential("SumPotential",this)
{
}

NPulseMembraneCommon::~NPulseMembraneCommon(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ������� ������� ���������� � �������� ������ �������
bool NPulseMembraneCommon::SetUseAveragePotential(const bool &value)
{
 // SAFETY: Instead of using raw pointers from Channels vector,
 // get all channels directly from components to ensure they're still valid
 vector<NameT> channel_buffer;
 GetComponentsNameByClassType<NPulseChannelCommon>(channel_buffer, GetThisAsSharedContainer());
 for(size_t i=0; i<channel_buffer.size(); i++)
 {
  try {
   std::weak_ptr<RDK::UContainer> channel_weak = GetComponentL(channel_buffer[i], true);
   if(!channel_weak.expired())
   {
    std::shared_ptr<NPulseChannelCommon> channel = std::dynamic_pointer_cast<NPulseChannelCommon>(channel_weak.lock());
    if(channel)
    {
     channel->UseAveragePotential=value;
    }
   }
  } catch (...) {
   // Skip invalid or deleted components
   // Note: LOG may not be available here, so we silently skip
  }
 }
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ����������
// --------------------------
// ������ ���������
size_t NPulseMembraneCommon::GetNumChannels(void) const
{
 return Channels.size();
}

NPulseChannelCommon* NPulseMembraneCommon::GetChannel(size_t i)
{
 return Channels[i];
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseMembraneCommon* NPulseMembraneCommon::New(void)
{
 return new NPulseMembraneCommon;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseMembraneCommon::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp) ||
	dynamic_pointer_cast<NPulseSynapseCommon>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPulseMembraneCommon::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 std::shared_ptr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 vector<NPulseChannelCommon* >::iterator I;

 if(channel)
 {
   if(find(Channels.begin(),Channels.end(),channel.get()) == Channels.end())
    Channels.push_back(channel.get());
 }

 std::shared_ptr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
   if(find(Synapses.begin(),Synapses.end(),synapse.get()) == Synapses.end())
    Synapses.push_back(synapse.get());
 }

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseMembraneCommon::ADelComponent(std::shared_ptr<UContainer> comp)
{
 std::shared_ptr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  channel->UseAveragePotential=UseAveragePotential;
  vector<NPulseChannelCommon*>::iterator I;
  I=find(Channels.begin(),Channels.end(),channel.get());
  if(I != Channels.end())
   Channels.erase(I);
 }

 std::shared_ptr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  vector<NPulseSynapseCommon*>::iterator I;
  I=find(Synapses.begin(),Synapses.end(),synapse.get());
  if(I != Synapses.end())
   Synapses.erase(I);
 }

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseMembraneCommon::ADefault(void)
{
 UseAveragePotential=true;
 SumPotential->Assign(1,1,0.0);
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembraneCommon::ABuild(void)
{
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   Channels[i]->UseAveragePotential=UseAveragePotential;
 return true;
}

// ����� �������� �����.
bool NPulseMembraneCommon::AReset(void)
{
 Feedback=0;
 IsNeuronActive=false;
 SumPotential->ToZero();
 return true;
}

// ��������� ������ ����� �������
bool NPulseMembraneCommon::ACalculate(void)
{
 if(!ACalculate2())
  return false;

 if(Feedback>0 && !IsNeuronActive)
 {
  IsNeuronActive=true;
  for(size_t i=0;i<Channels.size();i++)
   if(Channels[i])
	Channels[i]->NeuronActivated();
 }

 if(IsNeuronActive && Feedback<=0)
  IsNeuronActive=false;
 SumPotential(0,0)=0;
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   SumPotential(0,0)+=Channels[i]->Output(0,0);
 return true;
}

bool NPulseMembraneCommon::ACalculate2(void)
{
 return true;
}
// --------------------------

void NPulseMembraneCommon::RebuildSynapticInternalLinks(void)
{
    for(int i=0; i<Synapses.size(); i++)
    {
        Synapses[i]->RebuildInternalLinks();
    }
}
}
#endif
