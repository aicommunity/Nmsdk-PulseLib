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

namespace NMSDK {

// ������ NLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NLTZone::NLTZone(void)
 : Threshold("Threshold",this,&NLTZone::SetThreshold),
   UseAveragePotential("UseAveragePotential",this),
   Output("Output",this),
   Inputs("Inputs",this)
{
 Neuron=0;
 CachedNumAConnectors=0;
// AddAlias("DataInput0","Inputs");
// AddAlias("DataInput1","Inputs");
// AddAlias("DataInput2","Inputs");
// AddAlias("DataInput3","Inputs");
// AddAlias("DataInput4","Inputs");
// AddAlias("DataInput5","Inputs");
// AddAlias("DataInput6","Inputs");
// AddAlias("DataInput7","Inputs");
// AddAlias("DataInput8","Inputs");
// AddAlias("DataInput9","Inputs");
// AddAlias("DataInput10","Inputs");
// AddAlias("DataInput11","Inputs");
// AddAlias("DataInput12","Inputs");
// AddAlias("DataInput13","Inputs");
// AddAlias("DataInput14","Inputs");
// AddAlias("DataInput15","Inputs");
// AddAlias("DataInput16","Inputs");
// AddAlias("DataInput17","Inputs");
// AddAlias("DataInput18","Inputs");
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
 UseAveragePotential=true;
 Output.Assign(1,1,0.0);

 return true;
}

// ����� �������� �����.
bool NLTZone::AReset(void)
{
 CachedNumAConnectors=static_cast<int>(GetNumAConnectors(0));
 Output.ToZero();
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
  OutputPotential("OutputPotential",this),
  OutputFrequency("OutputFrequency",this),
  OutputPulseTimes("OutputPulseTimes",this),
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

 // ��������� �������� ���� ����������
 AvgInterval=1;
 PulseAmplitude=1;
 PulseLength=0.001;

 OutputPotential.Assign(1,1,0.0);
 OutputFrequency.Assign(1,1,0.0);
 OutputPulseTimes.Assign(1,1,0.0);

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
 OutputPotential.ToZero();
 OutputFrequency.ToZero();
 OutputPulseTimes.ToZero();

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
