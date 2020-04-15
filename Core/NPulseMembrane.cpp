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

#ifndef NPULSE_MEMBRANE_CPP
#define NPULSE_MEMBRANE_CPP

#include <algorithm>
#include "NPulseMembrane.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembrane::NPulseMembrane(void)
 : FeedbackGain("FeedbackGain",this,&NPulseMembrane::SetFeedbackGain),
  ResetAvailable("ResetAvailable",this),
  SynapseClassName("SynapseClassName",this),
  ChannelClassName("ChannelClassName",this),
  NumExcitatorySynapses("NumExcitatorySynapses",this),
  NumInhibitorySynapses("NumInhibitorySynapses",this),
  InputFeedbackSignal("InputFeedbackSignal",this),
  Feedback("Feedback",this)

{
}

NPulseMembrane::~NPulseMembrane(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� ���������� ����������
// --------------------------
// ������ ��������� �������������
size_t NPulseMembrane::GetNumPosChannels(void) const
{
 return ExcitatoryChannels.size();
}

NPulseChannel* NPulseMembrane::GetPosChannel(size_t i)
{
 return ExcitatoryChannels[i];
}

// ������ ��������� ����������������
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return InhibitoryChannels.size();
}

NPulseChannel* NPulseMembrane::GetNegChannel(size_t i)
{
 return InhibitoryChannels[i];
}

bool NPulseMembrane::UpdateChannelData(UEPtr<NPulseChannel> channel, UEPtr<UIPointer> pointer)
{
  vector<NPulseChannel* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel);
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel)) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel);
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel)) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }

 else
  return false;

 return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ����������� �������� �����
bool NPulseMembrane::SetFeedbackGain(const double &value)
{
 if(value < 0)
  return false;

 return true;
}

bool NPulseMembrane::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetChannelClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumExcitatorySynapses(const int &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumInhibitorySynapses(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseMembrane* NPulseMembrane::New(void)
{
 return new NPulseMembrane;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseMembrane::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannel>(comp))
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
/*
bool NPulseMembrane::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NPulseMembraneCommon::AAddComponent(comp, pointer))
  return false;

 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 vector<NPulseChannel* >::iterator I;

 if(channel)
 {
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel);
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel)) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel);
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel)) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }
  else
   LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Channel->Type is undefined");
 }



 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseMembrane::ADelComponent(UEPtr<UContainer> comp)
{
 if(!NPulseMembraneCommon::ADelComponent(comp))
  return false;

 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 if(channel)
 {
  vector<NPulseChannel*>::iterator I;
  I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel);
  if(I != ExcitatoryChannels.end())
   ExcitatoryChannels.erase(I);
  I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel);
  if(I != InhibitoryChannels.end())
   InhibitoryChannels.erase(I);
 }
 return true;
}               */
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseMembrane::ADefault(void)
{
 if(!NPulseMembraneCommon::ADefault())
  return false;
 FeedbackGain=2;
 ResetAvailable=true;
 SynapseClassName="NPSynapse";
 ChannelClassName="NPChannel";
 NumExcitatorySynapses=1;
 NumInhibitorySynapses=1;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembrane::ABuild(void)
{
 if(!Storage)
  return true;
 UEPtr<NPulseChannel> exc_channel=AddMissingComponent<NPulseChannel>("ExcChannel", ChannelClassName);
 UEPtr<NPulseChannel> inh_channel=AddMissingComponent<NPulseChannel>("InhChannel", ChannelClassName);

 ExcitatoryChannels.resize(1);
 ExcitatoryChannels[0]=exc_channel;
 exc_channel->SetCoord(MVector<double,3>(30,100,0));
 InhibitoryChannels.resize(1);
 InhibitoryChannels[0]=inh_channel;
 inh_channel->SetCoord(MVector<double,3>(30,200,0));

 bool res=true;

 size_t old_ex_synapses=ExcitatorySynapses.size();

 for(size_t i=old_ex_synapses;i<NumExcitatorySynapses;i++)
  ExcitatorySynapses[i]->Free();
 ExcitatorySynapses.resize(NumExcitatorySynapses);

 for(size_t i=old_ex_synapses;i<NumExcitatorySynapses;i++)
 {
  UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>("ExcSynapse", SynapseClassName);
  ExcitatorySynapses.push_back(synapse);
  res&=CreateLink(synapse->GetName(),"Output","ExcChannel","SynapticInput");
  synapse->SetCoord(MVector<double,3>(30+i*100,50,0));
 }

 size_t old_in_synapses=InhibitorySynapses.size();
 for(size_t i=old_in_synapses;i<NumInhibitorySynapses;i++)
  InhibitorySynapses[i]->Free();
 InhibitorySynapses.resize(NumInhibitorySynapses);

 for(size_t i=old_in_synapses;i<NumInhibitorySynapses;i++)
 {
  UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>("InhSynapse", SynapseClassName);
  InhibitorySynapses.push_back(synapse);
  res&=CreateLink(synapse->GetName(),"Output","InhChannel","SynapticInput");
  synapse->SetCoord(MVector<double,3>(30+i*100,250,0));
 }

 if(!NPulseMembraneCommon::ABuild())
  return false;
 return true;
}

// ����� �������� �����.
bool NPulseMembrane::AReset(void)
{
 if(!NPulseMembraneCommon::AReset())
  return false;
 Feedback=0;
 return true;
}

// ��������� ������ ����� �������
bool NPulseMembrane::ACalculate(void)
{
 if(!NPulseMembraneCommon::ACalculate())
  return false;
 if(!ExcitatoryChannels.empty() && !InhibitoryChannels.empty() && ResetAvailable)
 {
	if(ExcitatoryChannels[0]->GetSynOutput() > 1.0e-10)
	{
		InhibitoryChannels[0]->ResetOut();
	}
	else
	if(InhibitoryChannels[0]->GetSynOutput() > 1.0e-10)
	{
		ExcitatoryChannels[0]->ResetOut();
	}
 }

 Feedback=0;

  for(int j=0;j<InputFeedbackSignal->GetRows();j++)
   for(int i=0;i<InputFeedbackSignal->GetCols();i++)
    Feedback.v+=(*InputFeedbackSignal)(j,i);

 Feedback.v*=FeedbackGain.v;

 return true;
}
// --------------------------
}
#endif
