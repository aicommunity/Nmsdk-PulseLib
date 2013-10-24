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
#include "NPulseChannel.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembrane::NPulseMembrane(void)
//: NADItem(name),
 : Feedback("Feedback",this),
  FeedbackGain("FeedbackGain",this,&NPulseMembrane::SetFeedbackGain)
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
 return PosChannels.size();
}

NPulseChannel* NPulseMembrane::GetPosChannel(size_t i)
{
 return PosChannels[i];
}

// ������ ��������� ����������������
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return NegChannels.size();
}

NPulseChannel* NPulseMembrane::GetNegChannel(size_t i)
{
 return NegChannels[i];
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
bool NPulseMembrane::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 vector<NPulseChannel* >::iterator I;

 if(channel)
 {
  if(channel->Type() < 0)
  {
   if(find(PosChannels.begin(),PosChannels.end(),channel) == PosChannels.end())
	PosChannels.push_back(channel);
   if((I=find(NegChannels.begin(),NegChannels.end(),channel)) != NegChannels.end())
	NegChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(NegChannels.begin(),NegChannels.end(),channel) == NegChannels.end())
	NegChannels.push_back(channel);
   if((I=find(PosChannels.begin(),PosChannels.end(),channel)) != PosChannels.end())
	PosChannels.erase(I);
  }
  else
   return false;
 }
 else
  return false;

 return true;
}

bool NPulseMembrane::UpdateChannelData(UEPtr<NPulseChannel> channel, UEPtr<UIPointer> pointer)
{
  vector<NPulseChannel* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(PosChannels.begin(),PosChannels.end(),channel) == PosChannels.end())
	PosChannels.push_back(channel);
   if((I=find(NegChannels.begin(),NegChannels.end(),channel)) != NegChannels.end())
	NegChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(NegChannels.begin(),NegChannels.end(),channel) == NegChannels.end())
	NegChannels.push_back(channel);
   if((I=find(PosChannels.begin(),PosChannels.end(),channel)) != PosChannels.end())
	PosChannels.erase(I);
  }

 else
  return false;

 return true;
}
// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseMembrane::ADelComponent(UEPtr<UContainer> comp)
{
 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 if(channel)
 {
  vector<NPulseChannel*>::iterator I;
  I=find(PosChannels.begin(),PosChannels.end(),channel);
  if(I != PosChannels.end())
   PosChannels.erase(I);
  I=find(NegChannels.begin(),NegChannels.end(),channel);
  if(I != NegChannels.end())
   NegChannels.erase(I);
 }
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseMembrane::ADefault(void)
{
 FeedbackGain=2;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembrane::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseMembrane::AReset(void)
{
 Feedback=0;
 return true;
}

// ��������� ������ ����� �������
bool NPulseMembrane::ACalculate(void)
{
if(!PosChannels.empty() && !NegChannels.empty())
{
	if(PosChannels[0]->GetSynOutput() > 1.0e-10)
	{
		NegChannels[0]->ResetOut();
	}
	else
	if(NegChannels[0]->GetSynOutput() > 1.0e-10)
	{
		PosChannels[0]->ResetOut();
	}
}
 // ��������� ������ ������
// Feedback=GetFullSumInput();
 Feedback=0;

 for(int i=0;i<NumInputs;i++)
  for(size_t j=0;j<GetInputDataSize(i);j++)
   Feedback.v+=GetInputData(i)->Double[j];

 Feedback.v*=FeedbackGain.v;

 return true;
}
// --------------------------
}
#endif
