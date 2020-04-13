// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_CHANNEL_COMMON_CPP
#define NPULSE_CHANNEL_COMMON_CPP

#include "NPulseChannelCommon.h"
#include "NPulseMembrane.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelCommon::NPulseChannelCommon(void)
 : Type("Type", this, &NPulseChannelCommon::SetType),
   UseAveragePotential("UseAveragePotential",this),
   Output("Output",this)
{
}

NPulseChannelCommon::~NPulseChannelCommon(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
int NPulseChannelCommon::GetNumSynapses(void) const
{
 return 0;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
//
bool NPulseChannelCommon::SetType(const double &value)
{
 Type.v=value;

 UEPtr<NPulseMembrane> membr=dynamic_pointer_cast<NPulseMembrane>(Owner);
 if(membr)
 {
  membr->UpdateChannelData(dynamic_cast<NPulseChannel*>(this));
 }

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannelCommon* NPulseChannelCommon::New(void)
{
 return new NPulseChannelCommon;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseChannelCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
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
bool NPulseChannelCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseChannelCommon::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannelCommon::ADefault(void)
{
 Type=0;
 UseAveragePotential=true;
 Output.Assign(1,1,0.0);
// RestingThreshold=100;
// RestingFlag=false;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannelCommon::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseChannelCommon::AReset(void)
{
 Output.ToZero();

 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelCommon::ACalculate(void)
{
 return true;
}
// --------------------------
}

#endif

