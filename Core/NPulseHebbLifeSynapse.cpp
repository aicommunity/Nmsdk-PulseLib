// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_HEBB_LIFE_SYNAPSE_CPP
#define NPULSE_HEBB_LIFE_SYNAPSE_CPP

#include "NPulseHebbLifeSynapse.h"
#include "NPulseLifeNeuron.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbLifeSynapse::NPulseHebbLifeSynapse(void)
{
}

NPulseHebbLifeSynapse::~NPulseHebbLifeSynapse(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseHebbLifeSynapse* NPulseHebbLifeSynapse::New(void)
{
 return new NPulseHebbLifeSynapse;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ���������� ������ ����� � ������ ���������������� �������
bool NPulseHebbLifeSynapse::InstallLifeConnection(void)
{
 bool res=true;
 UEPtr<NPulseLifeNeuron> mlowner=UEPtr<NPulseLifeNeuron>(dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner.lock()).get());

 if(mlowner && mlowner->GetNeuronLife())
 {
  RDK::UStringLinkSide item,conn;
  UEPtr<UConnector> connected_component=GetAConnectorByIndex("Output", 0);
  UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(connected_component);
  if(!channel)
   return false;

  Type=channel->Type;
  if(channel->Type.v>0)
   res&=mlowner->CreateLink(mlowner->GetNeuronLife()->GetLongName(mlowner),"Output7",GetLongName(mlowner), "InputNeuronLifeSignal");
  else
   res&=mlowner->CreateLink(mlowner->GetNeuronLife()->GetLongName(mlowner),"Output6",GetLongName(mlowner), "InputNeuronLifeSignal");
}
 return res;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseHebbLifeSynapse::ADefault(void)
{
 if(!NPulseHebbSynapse::ADefault())
  return false;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseHebbLifeSynapse::ABuild(void)
{
 Type=0;
 return NPulseHebbSynapse::ABuild();
}

// ����� �������� �����.
bool NPulseHebbLifeSynapse::AReset(void)
{
 if(!NPulseHebbSynapse::AReset())
  return false;
 InstallLifeConnection();

 return true;
}

// ��������� ������ ����� �������
bool NPulseHebbLifeSynapse::ACalculate2(void)
{
 if(!NPulseHebbSynapse::ACalculate2())
  return false;

 if(MainOwner.lock() && Owner.lock())
 {
  UEPtr<NPulseLifeNeuron> neuron=UEPtr<NPulseLifeNeuron>(dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner.lock()).get());
  if(neuron)
  {
   if(Type>0)
   {
	neuron->SummaryNegGs.v+=GsSum.v*GsGain.v;
	neuron->SummaryNegGd.v+=Gd.v*GdGain.v;
	neuron->SummaryNegG.v+=G;
   }
   else
   {
	neuron->SummaryPosGs.v+=GsSum.v*GsGain.v;
	neuron->SummaryPosGd.v+=Gd.v*GdGain.v;
	neuron->SummaryPosG.v+=G;
   }
  }
 }

 return true;
}
// --------------------------
}

#endif
