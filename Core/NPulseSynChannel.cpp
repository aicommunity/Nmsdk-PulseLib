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
#include "NPulseLifeNeuron.h"
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
 VSecretionTC=1;
 VDissociationTC=1;
 OutputConstData=0;
 syn_output=0;
}

NPulseSynChannel::~NPulseSynChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
int NPulseSynChannel::GetNumSynapses(void) const
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
bool NPulseSynChannel::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
/*bool NPulseSynChannel::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
/*bool NPulseSynChannel::ADelComponent(std::shared_ptr<UContainer> comp)
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
//	POutputData[0].Double[0]=channel_input; // TODO: ��� ��� �� ������ ������
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
  Output(0,0)=1;
 }
 else
 if(Type<1)
 {
  Output(0,0)=-1;
 }
 else
 {
  Output.ToZero();
 }

 SynapseInputFlagsList.resize(ChannelInputs->size(),true);
 for(int n=0;n<int(ChannelInputs->size());n++)
 {
  std::shared_ptr<UContainer> item=std::shared_ptr<UContainer>(ChannelInputs.GetItem(n)->GetOwner().get());
  if(dynamic_pointer_cast<NPulseChannel>(item) ||
	 dynamic_pointer_cast<NReceptor>(item) ||
	 dynamic_pointer_cast<NConstGenerator>(item))
   SynapseInputFlagsList[n]=false;
 }

 return true;
}

// ��������� ������ ����� �������
bool NPulseSynChannel::ACalculate2(void)
{
 /*double*/ channel_input=0;
 size_t num_connected_channels=0;
 double G=0;

 // ������ ��������
 double input=0;
 int num_connected_synapsis=0;
 /*double*/ syn_output=0;


 for(int n=0;n<int(ChannelInputs->size());n++)
 {
  int inpsize=ChannelInputs[n].GetCols();
  if(inpsize == 0)
   continue;

  if(!SynapseInputFlagsList[n])
  {
   double *data=ChannelInputs[n].Data;
   for(int j=0;j<inpsize;j++,++data)
    channel_input+=*data;
   ++num_connected_channels;
  }
  else // ��������� ������������ ���������� ������� ������� ��������
  {
   ++num_connected_synapsis;
   if(int(PreOutput->size())<num_connected_synapsis)
   {
    PreOutput->resize(num_connected_synapsis);
    PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=ChannelInputs[n](0,0);

   auto main_owner = MainOwner.lock();
   auto owner = Owner.lock();
   if(main_owner && owner)
   {
    if(Type() < 0)
     ++static_pointer_cast<NPulseNeuron>(main_owner)->NumActivePosInputs.v;
    else
     ++static_pointer_cast<NPulseNeuron>(main_owner)->NumActiveNegInputs.v;
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
 NumConnectedSynapsis=num_connected_synapsis;

 if(UseAveragePotential && num_connected_channels>0)
  channel_input/=num_connected_channels;
 SumChannelInput(0,0)=channel_input;

 if(UseAverageSynapsis && num_connected_synapsis>0)
  G/=num_connected_synapsis;

 auto owner_ptr = Owner.lock();
 double feedback = owner_ptr ? static_pointer_cast<NPulseMembrane>(owner_ptr)->Feedback : 0.0;
 // ��������� ���������� �� �������� �����
 if(owner_ptr)
  channel_input-=feedback;

 // ������
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);

 // ��������� ������������� ������
// if(RestingFlag)
// {
//  if(G<RestingThreshold)
//  {
//   *out=channel_input;
//  }
// }

 if(!feedback)
 {
  double resistance(0.0);
  if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
   resistance=RestingResistance.v;
  else
   resistance=Resistance.v;

  Ti=Capacity/(G+1.0/resistance);
  sum_u=(1.0+G*resistance);
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
 VSecretionTC=1;
 VDissociationTC=1;
 OutputConstData=0;
}

NContinuesSynChannel::~NContinuesSynChannel(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
int NContinuesSynChannel::GetNumSynapses(void) const
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
bool NContinuesSynChannel::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
/*bool NContinuesSynChannel::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
/*bool NContinuesSynChannel::ADelComponent(std::shared_ptr<UContainer> comp)
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

 //FillOutputData(); // TODO: ��� ��� �� ������ ������

 SynapseInputFlagsList.resize(ChannelInputs->size(),false);
 for(int n=0;n<int(ChannelInputs->size());n++)
 {
  std::shared_ptr<UContainer> item=std::shared_ptr<UContainer>(ChannelInputs.GetItem(n)->GetOwner().get());
  if(dynamic_pointer_cast<NPulseSynapseCommon>(item))
   SynapseInputFlagsList[n]=true;
 }

 return true;
}

// ��������� ������ ����� �������
bool NContinuesSynChannel::ACalculate2(void)
{
 double channel_input=0;
 int num_connected_channels=0;
 double G=0;

 // ������ ��������
 double input=0;
 int num_connected_synapsis=0;
 double syn_output=0;


 for(int n=0;n<int(ChannelInputs->size());n++)
 {
  if(ChannelInputs[n].GetCols() == 0)
   continue;
//  if(dynamic_cast<NPulseChannel*>(GetCItem(n).Item) ||
//     dynamic_cast<NReceptor*>(GetCItem(n).Item) ||
//	 dynamic_cast<NConstGenerator*>(GetCItem(n).Item))
  if(!SynapseInputFlagsList[n])
  {
   int inpsize=0;
   if((inpsize=ChannelInputs[n].GetCols()) >0)
   {
    double *data=ChannelInputs[n].Data;
    for(int j=0;j<inpsize;j++,++data)
     channel_input+=*data;
    ++num_connected_channels;
   }
  }
  else // ��������� ������������ ���������� ������� ������� ��������
  {
   ++num_connected_synapsis;
   if(int(PreOutput->size())<num_connected_synapsis)
   {
    PreOutput->resize(num_connected_synapsis);
    PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=ChannelInputs[n](0,0);

   auto main_owner = MainOwner.lock();
   auto owner = Owner.lock();
   if(main_owner && owner)
   {
    if(Type() < 0)
     ++static_pointer_cast<NPulseNeuron>(main_owner)->NumActivePosInputs.v;
    else
     ++static_pointer_cast<NPulseNeuron>(main_owner)->NumActiveNegInputs.v;
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
 NumConnectedSynapsis=num_connected_synapsis;

 if(UseAveragePotential && num_connected_channels>0)
  channel_input/=num_connected_channels;
 SumChannelInput(0,0)=channel_input;

 if(UseAverageSynapsis && num_connected_synapsis>0)
  G/=num_connected_synapsis;

 // ������
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);


 // ��������� ������������� ������
// if(RestingFlag)
// {
//  if(G<RestingThreshold)
//  {
//   *out=channel_input;
//  }
// }

 double resistance(0.0);
 if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
  resistance=RestingResistance.v;
 else
  resistance=Resistance.v;

 Ti=Capacity/(G+1.0/resistance);
 sum_u=(1.0+G*resistance);

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

