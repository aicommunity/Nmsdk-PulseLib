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

#include <fstream>
#include <sstream>
#include <limits>

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
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   Channels[i]->UseAveragePotential=value;
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
bool NPulseMembraneCommon::CheckComponentType(UEPtr<UContainer> comp) const
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
bool NPulseMembraneCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 vector<NPulseChannelCommon* >::iterator I;

 if(channel)
 {
   if(find(Channels.begin(),Channels.end(),channel) == Channels.end())
	Channels.push_back(channel);
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
   if(find(Synapses.begin(),Synapses.end(),synapse) == Synapses.end())
	Synapses.push_back(synapse);
 }

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseMembraneCommon::ADelComponent(UEPtr<UContainer> comp)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  channel->UseAveragePotential=UseAveragePotential;
  vector<NPulseChannelCommon*>::iterator I;
  I=find(Channels.begin(),Channels.end(),channel);
  if(I != Channels.end())
   Channels.erase(I);
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  vector<NPulseSynapseCommon*>::iterator I;
  I=find(Synapses.begin(),Synapses.end(),synapse);
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

 //
 std::stringstream ss;
 ss<<"/home/ivan/rtv/log/"<<GetName().c_str()<<".txt";

 std::ofstream ofs;
 ofs.open(ss.str(), std::ios_base::app);
 typedef std::numeric_limits< double > dbl;
 std::cout.precision(dbl::max_digits10 - 1);
 ofs<<GetTime().GetDoubleTime()<<" "<<std::scientific<<SumPotential(0,0)<<" "<<Feedback.v<<" "<<(int)(IsNeuronActive)<<"\n";
 ofs.close();
 return true;
}

bool NPulseMembraneCommon::ACalculate2(void)
{
 return true;
}
// --------------------------
}
#endif
