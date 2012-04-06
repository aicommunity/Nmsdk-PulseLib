// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_CHANNEL_CPP
#define NPULSE_CHANNEL_CPP

#include "NPulseChannel.h"
#include "NPulseSynapse.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "NPulseHebbSynapse.h"
#include "../NeuronLifeLib/NPulseLifeNeuron.h"
#include "../BCL/NConnector.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannel::NPulseChannel(void)
//: NADItem(name),
: Capacity("Capacity",this,&NPulseChannel::SetCapacity),
Resistance("Resistance",this,&NPulseChannel::SetResistance),
FBResistance("FBResistance",this,&NPulseChannel::SetFBResistance),
Type("Type",this)
{
}

NPulseChannel::~NPulseChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
size_t NPulseChannel::GetNumSynapses(void) const
{
 return GetNumComponents();
}

// ���������� ������ �� �������
UEPtr<NPulseSynapse> NPulseChannel::GetSynapse(size_t i)
{
 return static_pointer_cast<NPulseSynapse>(GetComponentByIndex(i));
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������� ��������
bool NPulseChannel::SetCapacity(real value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ��������
bool NPulseChannel::SetResistance(real value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ���������� ��������
bool NPulseChannel::SetFBResistance(real value)
{
 if(value <= 0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// ���������� ������ ����� synapse � �������������� ���� �������-���������
// ���������� false ������ ���� ��������� ������ ��������� �����
// ���� synapse == 0, �� ���������� ��� ������� �����
bool NPulseChannel::InstallHebbSynapses(UEPtr<NAContainer> synapse)
{
 bool res=true;
 UEPtr<NPulseNeuron> mowner=dynamic_pointer_cast<NPulseNeuron>(MainOwner);

 if(mowner && mowner->GetLTZone())
 {
  if(synapse)
  {
   UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(synapse);
   if(hsynapse)
   {
	RDK::ULinkSide item,conn;
	item.Id=mowner->GetLTZone()->GetLongId(mowner);
	item.Index=0;
	conn.Id=hsynapse->GetLongId(mowner);
	conn.Index=1;
	res&=mowner->CreateLink(item,conn);
   }
  }
  else
  {
   RDK::ULinkSide item,conn;
   for(size_t i=0;i<GetNumSynapses();i++)
   {
	UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(GetSynapse(i));
	if(hsynapse)
	{
	 item.Id=mowner->GetLTZone()->GetLongId(mowner);
     item.Index=0;
	 conn.Id=hsynapse->GetLongId(mowner);
	 conn.Index=1;
	 res&=mowner->CreateLink(item,conn);
	}
   }
  }
 }

 UEPtr<NPulseLifeNeuron> mlowner=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);

 if(mlowner && mlowner->GetNeuronLife())
 {
  if(synapse)
  {
   UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(synapse);
   if(hsynapse)
   {
	RDK::ULinkSide item,conn;
	item.Id=mlowner->GetNeuronLife()->GetLongId(mlowner);
	if(Type.v>0)
	 item.Index=6;
	else
	 item.Index=5;
	conn.Id=hsynapse->GetLongId(mlowner);
	conn.Index=2;
	res&=mlowner->CreateLink(item,conn);
   }
  }
  else
  {
   RDK::ULinkSide item,conn;
   for(size_t i=0;i<GetNumSynapses();i++)
   {
	UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(GetSynapse(i));
	if(hsynapse)
	{
	 item.Id=mlowner->GetNeuronLife()->GetLongId(mlowner);
	 if(Type.v>0)
	  item.Index=6;
	 else
	  item.Index=5;
	 conn.Id=hsynapse->GetLongId(mlowner);
	 conn.Index=2;
	 res&=mlowner->CreateLink(item,conn);
	}
   }
  }
 }
 return res;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannel* NPulseChannel::New(void)
{
 return new NPulseChannel;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseChannel::CheckComponentType(UEPtr<NAContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseSynapse>(comp))
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
bool NPulseChannel::AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseChannel::ADelComponent(UEPtr<UAContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannel::ADefault(void)
{
 // ������� ��������
 Capacity=1.0e-9;

 // ������������� ��������
 Resistance=1.0e7;

 // ������������� ���������� ��������
 FBResistance=1.0e8;

 Type=0;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannel::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseChannel::AReset(void)
{
 FillOutputData();
 return true;
}

// ��������� ������ ����� �������
bool NPulseChannel::ACalculate(void)
{
 real channel_input=0;
 real G=0;

 // ��������� ������� � ������ �������
 for(int i=0;i<NumComponents;i++)
  G+=static_pointer_cast<NADItem>(PComponents[i])->GetOutputData(0).Double[0];
 
 // ��������� ������ ������
 if(FullInputDataSize>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	real *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 channel_input+=*data;
   }
  }
  channel_input/=FullInputDataSize;
 }

 real feedback=static_pointer_cast<NPulseMembrane>(Owner)->Feedback;
 // ��������� ���������� �� �������� �����
 if(Owner)
  channel_input-=feedback;

 // ������
 real *out=&POutputData[0].Double[0];
 real Ti,sum_u;

 if(!feedback)
 {
  Ti=Capacity.v/(G+1.0/Resistance.v);
  sum_u=(1.0+G*Resistance.v);
 }
 else
 {
  Ti=Capacity.v/(G+1.0/FBResistance.v);
  sum_u=(1.0+G*FBResistance.v);
 }

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

