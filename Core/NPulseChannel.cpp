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
#include "NPulseLifeNeuron.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannel::NPulseChannel(void)
: Capacity("Capacity",this,&NPulseChannel::SetCapacity),
Resistance("Resistance",this,&NPulseChannel::SetResistance),
FBResistance("FBResistance",this,&NPulseChannel::SetFBResistance),
RestingResistance("RestingResistance",this),
NumConnectedSynapsis("NumConnectedSynapsis",this),
TimeConstant("TimeConstant", this)
{
 channel_input=0;
}

NPulseChannel::~NPulseChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
int NPulseChannel::GetNumSynapses(void) const
{
 return GetNumComponents();
}

// ���������� ������ �� �������
std::shared_ptr<NPulseSynapse> NPulseChannel::GetSynapse(int i)
{
 return std::shared_ptr<NPulseSynapse>(dynamic_pointer_cast<NPulseSynapse>(SynapticInputs.GetItem(i)->GetOwner()).get());
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������� ��������
bool NPulseChannel::SetCapacity(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ��������
bool NPulseChannel::SetResistance(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ���������� ��������
bool NPulseChannel::SetFBResistance(const double &value)
{
 if(value <= 0)
  return false;

 return true;
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
bool NPulseChannel::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
bool NPulseChannel::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 if(!NPulseChannelCommon::AAddComponent(comp, pointer))
  return false;
 //InstallHebbSynapses(comp);
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseChannel::ADelComponent(std::shared_ptr<UContainer> comp)
{
 if(!NPulseChannelCommon::ADelComponent(comp))
  return false;

 return true;
}
// --------------------------
double NPulseChannel::GetSynOutput(void)
{
	 return 0;
}

bool NPulseChannel::ResetOut(void)
{
	 return false;
}
// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannel::ADefault(void)
{
 if(!NPulseChannelCommon::ADefault())
  return false;

 // ������� ��������
 Capacity=1.0e-9;

 // ������������� ��������
 Resistance=1.0e7;

 // ������������� ���������� ��������
 FBResistance=1.0e8;

 RestingResistance=1.0e7;

 Type=0;

 NumConnectedSynapsis = 0;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannel::ABuild(void)
{
 if(!NPulseChannelCommon::ABuild())
  return false;

 return true;
}

// ����� �������� �����.
bool NPulseChannel::AReset(void)
{
 if(!NPulseChannelCommon::AReset())
  return false;

 if(Type>0)
  Output(0,0)=1;
 else
 if(Type<0)
  Output(0,0)=-1;

 TimeConstant = Resistance * Capacity;

 return true;
}

// ��������� ������ ����� �������
bool NPulseChannel::ACalculate2(void)
{
 channel_input=0;
 double G=0;

 // ��������� ������� � ������ �������
 for(int i=0;i<int(SynapticInputs->size());i++)
 {
  if(SynapticInputs[i].GetSize() >0)
   G+=SynapticInputs[i](0,0);
 }

 if(UseAverageSynapsis && !SynapticInputs->empty())
  G/=SynapticInputs->size();

 // ��������� ������ ������
 int inp_size;
 int full_inp_data_size(0);
 for(int i=0;i<int(ChannelInputs->size());i++)
 {
  if((inp_size=ChannelInputs[i].GetSize()) >0)
  {
   full_inp_data_size+=inp_size;
   double *data=ChannelInputs[i].Data;
   for(int j=0;j<inp_size;j++,++data)
    channel_input+=*data;
  }
 }
 if(UseAveragePotential && full_inp_data_size>0)
  channel_input/=full_inp_data_size;

 SumChannelInput(0,0)=channel_input;

 // ��������� ���������� �� �������� �����
 std::shared_ptr<NPulseMembrane> membrane=std::shared_ptr<NPulseMembrane>(dynamic_pointer_cast<NPulseMembrane>(Owner.lock()).get());
 if(membrane)
  channel_input-=membrane->Feedback;

 // ������
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);

 if(membrane && fabs(membrane->Feedback)<1e-3)
 {
  double resistance(0.0);
  if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
   resistance=RestingResistance.v;
  else
   resistance=Resistance.v;

  Ti=Capacity.v/(G+1.0/resistance);
  sum_u=(1.0+G*resistance);
 }
 else
 {
  Ti=Capacity.v/(G+1.0/FBResistance.v);
  sum_u=(1.0+G*FBResistance.v);
 }

 TimeConstant = Ti;

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

