// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYN_CHANNEL_CPP
#define NPULSE_SYN_CHANNEL_CPP

#include "NPulseSynChannel.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "../NeuronLifeLib/NPulseLifeNeuron.h"
//#include "../BCL/NConnector.h"
#include "NReceptor.h"


namespace NMSDK {

// ������ NPulseSynChannel
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynChannel::NPulseSynChannel(void)
//: NPulseChannel(name),
: PulseAmplitude("PulseAmplitude",this,&NPulseSynChannel::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NPulseSynChannel::SetSecretionTC),
DissociationTC("DissociationTC",this,&NPulseSynChannel::SetDissociationTC),
InhibitionCoeff("InhibitionCoeff",this,&NPulseSynChannel::SetInhibitionCoeff),
SynapseResistance("SynapseResistance",this,&NPulseSynChannel::SetSynapseResistance),

PreOutput("PreOutput",this)
{
}

NPulseSynChannel::~NPulseSynChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
size_t NPulseSynChannel::GetNumSynapses(void) const
{
 return 0;
// return GetNumComponents();
}

// ���������� ������ �� �������
NPulseSynapse* NPulseSynChannel::GetSynapse(size_t i)
{
// return static_cast<NPulseSynapse*>(GetComponentByIndex(i));
 return 0;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NPulseSynChannel::SetPulseAmplitude(const double &value)
{
 return true;
}

// ���������� ������� ��������� ���������
bool NPulseSynChannel::SetSecretionTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ���������� ������� ������� ���������
bool NPulseSynChannel::SetDissociationTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ����������� ����������������� ����������
bool NPulseSynChannel::SetInhibitionCoeff(const double &value)
{
 if(SynapseResistance.v > 0)
 {
  if(value>0)
   OutputConstData=4.0*(value+1)/SynapseResistance.v;
  else
   OutputConstData=1.0/SynapseResistance.v;
 }
 else
  OutputConstData=0;

 return true;
}

// ��� (������������� �������) �������
bool NPulseSynChannel::SetSynapseResistance(const double &value)
{
 if(value<=0)
  return false;

 if(InhibitionCoeff.v>0)
  OutputConstData=4.0*(InhibitionCoeff.v+1)/value;
 else
  OutputConstData=1.0/value;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseSynChannel* NPulseSynChannel::New(void)
{
 return new NPulseSynChannel;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseSynChannel::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NPulseSynapse*>(comp))
//  return true;

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
/*bool NPulseSynChannel::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
/*bool NPulseSynChannel::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}                     */
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
double NPulseSynChannel::GetSynOutput()
{
    return syn_output;
}

bool NPulseSynChannel::ResetOut()
{
	POutputData[0].Double[0]=channel_input;
    return true;
}
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseSynChannel::ADefault(void)
{
 if(!NPulseChannel::ADefault())
  return false;

 // ��������� �������� ���� ����������
 // ��������� ������� ���������
 PulseAmplitude=1;

 // ���������� ������� ��������� ���������
 SecretionTC=0.001;

 // ���������� ������� ������� ���������
 DissociationTC=0.01;

 // ����������� ����������������� ����������
 InhibitionCoeff=0;

 // ��� (������������� �������) �������
 SynapseResistance=1.e8;

// SetNumInputs(1);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSynChannel::ABuild(void)
{
 if(!NPulseChannel::ABuild())
  return false;

 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;
 return true;
}

// ����� �������� �����.
bool NPulseSynChannel::AReset(void)
{
 if(!NPulseChannel::AReset())
  return false;

 // ����� ��������� ����������
 if(Type>0)
 {
  POutputData[0].Double[0]=1;
 }
 else
 if(Type<1)
 {
  POutputData[0].Double[0]=-1;
 }
 else
 {
  FillOutputData();
 }

 return true;
}

// ��������� ������ ����� �������
bool NPulseSynChannel::ACalculate(void)
{
 /*double*/ channel_input=0;
 size_t num_connected_channels=0;
 double G=0;

 // ������ ��������
 double input=0;
 size_t num_connected_synapsis=0;
 /*double*/ syn_output=0;


 for(int n=0;n<NumInputs;n++)
 {
  if(GetInputDataSize(n) == 0)
   continue;
  if(dynamic_cast<NPulseChannel*>(GetCItem(n).Item) ||
     dynamic_cast<NReceptor*>(GetCItem(n).Item) ||
	 dynamic_cast<NConstGenerator*>(GetCItem(n).Item))
  {
   size_t inpsize=0;
   if((inpsize=GetInputDataSize(n)) >0)
   {
    double *data=&(GetInputData(n)->Double[0]);
    for(size_t j=0;j<inpsize;j++,++data)
     channel_input+=*data;
    ++num_connected_channels;
   }
  }
  else // ��������� ������������ ���������� ������� ������� ��������
  {
   ++num_connected_synapsis;
   if(PreOutput->size()<num_connected_synapsis)
   {
	PreOutput->resize(num_connected_synapsis);
    PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=GetInputData(n)->Double[0];

   if(MainOwner && Owner)
   {
	if(Type() < 0)
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
	else
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
   }

   if(input>0)
    PreOutput.v[num_connected_synapsis-1]+=(input/PulseAmplitude.v-PreOutput.v[num_connected_synapsis-1])/VSecretionTC;
   else
    PreOutput.v[num_connected_synapsis-1]-=PreOutput.v[num_connected_synapsis-1]/VDissociationTC;

   syn_output=OutputConstData*(1.0-InhibitionCoeff.v*PreOutput.v[num_connected_synapsis-1])*PreOutput.v[num_connected_synapsis-1];

   if(syn_output>0)
	G+=syn_output;
  }
 }

 // ��������� ������ ������
  switch(PotentialSummaryMode)
  {
  case 0:
   if(num_connected_channels)
    channel_input/=num_connected_channels;
  break;

  case 1:
  break;

  default:
   ;
  }

 double feedback=static_pointer_cast<NPulseMembrane>(Owner)->Feedback;
 // ��������� ���������� �� �������� �����
 if(Owner)
  channel_input-=feedback;

 // ������
 double *out=&POutputData[0].Double[0];
 double Ti,sum_u;

 if(!feedback)
 {
  Ti=Capacity/(G+1.0/Resistance);
  sum_u=(1.0+G*Resistance);
 }
 else
 {
  Ti=Capacity/(G+1.0/FBResistance);
  sum_u=(1.0+G*FBResistance);
 }

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------


// ������ NContinuesSynChannel
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesSynChannel::NContinuesSynChannel(void)
//: NPulseChannel(name),
: PulseAmplitude("PulseAmplitude",this,&NContinuesSynChannel::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NContinuesSynChannel::SetSecretionTC),
DissociationTC("DissociationTC",this,&NContinuesSynChannel::SetDissociationTC),
InhibitionCoeff("InhibitionCoeff",this,&NContinuesSynChannel::SetInhibitionCoeff),
SynapseResistance("SynapseResistance",this,&NContinuesSynChannel::SetSynapseResistance),

PreOutput("PreOutput",this)
{
}

NContinuesSynChannel::~NContinuesSynChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
size_t NContinuesSynChannel::GetNumSynapses(void) const
{
 return 0;
// return GetNumComponents();
}

// ���������� ������ �� �������
NPulseSynapse* NContinuesSynChannel::GetSynapse(size_t i)
{
// return static_cast<NPulseSynapse*>(GetComponentByIndex(i));
 return 0;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NContinuesSynChannel::SetPulseAmplitude(const double &value)
{
 return true;
}

// ���������� ������� ��������� ���������
bool NContinuesSynChannel::SetSecretionTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ���������� ������� ������� ���������
bool NContinuesSynChannel::SetDissociationTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ����������� ����������������� ����������
bool NContinuesSynChannel::SetInhibitionCoeff(const double &value)
{
 if(SynapseResistance.v > 0)
  OutputConstData=4.0*(value+1)/SynapseResistance.v;
 else
  OutputConstData=0;

 return true;
}

// ��� (������������� �������) �������
bool NContinuesSynChannel::SetSynapseResistance(const double &value)
{
 if(value<=0)
  return false;

 OutputConstData=4.0*InhibitionCoeff.v/value;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NContinuesSynChannel* NContinuesSynChannel::New(void)
{
 return new NContinuesSynChannel;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NContinuesSynChannel::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NPulseSynapse*>(comp))
//  return true;

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
/*bool NContinuesSynChannel::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
/*bool NContinuesSynChannel::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}                     */
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NContinuesSynChannel::ADefault(void)
{
 if(!NPulseChannel::ADefault())
  return false;

 // ��������� �������� ���� ����������
 // ��������� ������� ���������
 PulseAmplitude=1;

 // ���������� ������� ��������� ���������
 SecretionTC=0.001;

 // ���������� ������� ������� ���������
 DissociationTC=0.01;

 // ����������� ����������������� ����������
 InhibitionCoeff=1;

 // ��� (������������� �������) �������
 SynapseResistance=1.0e8;

// SetNumInputs(1);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NContinuesSynChannel::ABuild(void)
{
 if(!NPulseChannel::ABuild())
  return false;

 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;
 return true;
}

// ����� �������� �����.
bool NContinuesSynChannel::AReset(void)
{
 if(!NPulseChannel::AReset())
  return false;

 // ����� ��������� ����������
// PreOutput=0;
 PreOutput->assign(PreOutput->size(),0);

 FillOutputData();
 return true;
}

// ��������� ������ ����� �������
bool NContinuesSynChannel::ACalculate(void)
{
 double channel_input=0;
 size_t num_connected_channels=0;
 double G=0;

 // ������ ��������
 double input=0;
 size_t num_connected_synapsis=0;
 double syn_output=0;


 for(int n=0;n<NumInputs;n++)
 {
  if(GetInputDataSize(n) == 0)
   continue;
  if(dynamic_cast<NPulseChannel*>(GetCItem(n).Item) ||
     dynamic_cast<NReceptor*>(GetCItem(n).Item) ||
	 dynamic_cast<NConstGenerator*>(GetCItem(n).Item))
  {
   size_t inpsize=0;
   if((inpsize=GetInputDataSize(n)) >0)
   {
    double *data=&(GetInputData(n)->Double[0]);
    for(size_t j=0;j<inpsize;j++,++data)
     channel_input+=*data;
    ++num_connected_channels;
   }
  }
  else // ��������� ������������ ���������� ������� ������� ��������
  {
   ++num_connected_synapsis;
   if(PreOutput->size()<num_connected_synapsis)
   {
	PreOutput->resize(num_connected_synapsis);
    PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=GetInputData(n)->Double[0];

   if(MainOwner && Owner)
   {
	if(Type() < 0)
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
	else
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
   }

   if(input>0)
    PreOutput.v[num_connected_synapsis-1]+=(input/PulseAmplitude.v-PreOutput.v[num_connected_synapsis-1])/VSecretionTC;
   else
	PreOutput.v[num_connected_synapsis-1]-=PreOutput.v[num_connected_synapsis-1]/VDissociationTC;

   syn_output=PreOutput.v[num_connected_synapsis-1]/SynapseResistance.v;//OutputConstData*(1.0-InhibitionCoeff.v*PreOutput.v[num_connected_synapsis-1])*PreOutput.v[num_connected_synapsis-1];

   if(syn_output>0)
	G+=syn_output;
  }
 }

 // ��������� ������ ������
  switch(PotentialSummaryMode)
  {
  case 0:
   if(num_connected_channels)
    channel_input/=num_connected_channels;
  break;

  case 1:
  break;

  default:
   ;
  }

 // ������
 double *out=&POutputData[0].Double[0];
 double Ti,sum_u;

 Ti=Capacity/(G+1.0/Resistance);
 sum_u=(1.0+G*Resistance);

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

