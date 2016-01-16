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
#include "NPulseChannel.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembraneCommon::NPulseMembraneCommon(void)
{
}

NPulseMembraneCommon::~NPulseMembraneCommon(void)
{
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
// ������ ���������� �������������� ����������
// --------------------------
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
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp))
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
  vector<NPulseChannelCommon*>::iterator I;
  I=find(Channels.begin(),Channels.end(),channel);
  if(I != Channels.end())
   Channels.erase(I);
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
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembraneCommon::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseMembraneCommon::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool NPulseMembraneCommon::ACalculate(void)
{
 return true;
}
// --------------------------
}
#endif
