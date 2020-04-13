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
   InputChannels("InputChannels",this)
{
 Neuron=0;
 CachedNumAConnectors=0;
// AddAlias("DataInput0","InputChannels");
// AddAlias("DataInput1","InputChannels");
// AddAlias("DataInput2","InputChannels");
// AddAlias("DataInput3","InputChannels");
// AddAlias("DataInput4","InputChannels");
// AddAlias("DataInput5","InputChannels");
// AddAlias("DataInput6","InputChannels");
// AddAlias("DataInput7","InputChannels");
// AddAlias("DataInput8","InputChannels");
// AddAlias("DataInput9","InputChannels");
// AddAlias("DataInput10","InputChannels");
// AddAlias("DataInput11","InputChannels");
// AddAlias("DataInput12","InputChannels");
// AddAlias("DataInput13","InputChannels");
// AddAlias("DataInput14","InputChannels");
// AddAlias("DataInput15","InputChannels");
// AddAlias("DataInput16","InputChannels");
// AddAlias("DataInput17","InputChannels");
// AddAlias("DataInput18","InputChannels");
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
