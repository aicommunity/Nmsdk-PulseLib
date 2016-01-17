// ===========================================================
// Version:        3.0.2
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

#ifndef NPULSE_LTZONE_COMMON_CPP
#define NPULSE_LTZONE_COMMON_CPP

#include <numeric>
#include "NPulseLTZoneCommon.h"
#include "NPulseChannel.h"
#include "NPulseNeuron.h"
//#include "../BCL/NConnector.h"


namespace NMSDK {

// ������ NLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NLTZone::NLTZone(void)
 : Threshold("Threshold",this,&NLTZone::SetThreshold)
{
 Neuron=0;
}

NLTZone::~NLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ����� �������
bool NLTZone::SetThreshold(const double &value)
{
 return true;
}

// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NLTZone::ADefault(void)
{
 Threshold=0.0;

 return true;
}

// ����� �������� �����.
bool NLTZone::AReset(void)
{
 CachedNumAConnectors=static_cast<double>(GetNumAConnectors(0));
 return true;
}
// --------------------------


// ������ NPulseLTZoneCommon
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZoneCommon::NPulseLTZoneCommon(void)
 :
  PulseAmplitude("PulseAmplitude",this,&NPulseLTZoneCommon::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
{
}

NPulseLTZoneCommon::~NPulseLTZoneCommon(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NPulseLTZoneCommon::SetPulseAmplitude(const double &value)
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
NPulseLTZoneCommon* NPulseLTZoneCommon::New(void)
{
 return new NPulseLTZoneCommon;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseLTZoneCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseLTZoneCommon::ADefault(void)
{
 NLTZone::ADefault();
 vector<size_t> size;

 SetNumOutputs(4);
 for(int i=0;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));

 // ��������� �������� ���� ����������
 AvgInterval=1;
 PulseAmplitude=1;
 PulseLength=0.001;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLTZoneCommon::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseLTZoneCommon::AReset(void)
{
 NLTZone::AReset();
 // ����� ��������� ����������
 PulseCounter=0;
 PrePotential=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 FillOutputData(0);

 return true;
}

// ��������� ������ ����� �������
bool NPulseLTZoneCommon::ACalculate(void)
{

 return true;
}

/// ���������� true ���� ������� ��� ��������� �������� ���������
bool NPulseLTZoneCommon::CheckPulseOn(void)
{
 return false;
}

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
bool NPulseLTZoneCommon::CheckPulseOff(void)
{
 return false;
}
// --------------------------

}
#endif
