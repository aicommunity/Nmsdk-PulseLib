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

#ifndef NPULSE_MEMBRANE_IZHIKEVICH_CPP
#define NPULSE_MEMBRANE_IZHIKEVICH_CPP

#include <algorithm>
#include "NPulseMembraneIzhikevich.h"
#include "NPulseChannelIzhikevich.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembraneIzhikevich::NPulseMembraneIzhikevich(void)
{
}

NPulseMembraneIzhikevich::~NPulseMembraneIzhikevich(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseMembraneIzhikevich* NPulseMembraneIzhikevich::New(void)
{
 return new NPulseMembraneIzhikevich;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseMembraneIzhikevich::CheckComponentType(UEPtr<UContainer> comp) const
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseMembraneIzhikevich::ADefault(void)
{
 if(!NPulseMembrane::ADefault())
  return false;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembraneIzhikevich::ABuild(void)
{
 if(!NPulseMembrane::ABuild())
  return false;

 AddMissingComponent<NPulseChannelIzhikevich>("PosChannel", "NPulseChannelIzhikevich");

 return true;
}

// ����� �������� �����.
bool NPulseMembraneIzhikevich::AReset(void)
{
 if(!NPulseMembrane::AReset())
  return false;
 return true;
}

// ��������� ������ ����� �������
bool NPulseMembraneIzhikevich::ACalculate2(void)
{
 if(!NPulseMembrane::ACalculate())
  return false;

 return true;
}
// --------------------------
}
#endif
