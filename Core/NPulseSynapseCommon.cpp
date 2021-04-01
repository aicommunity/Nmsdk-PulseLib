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

#ifndef NPULSE_SYNAPSE_COMMON_CPP
#define NPULSE_SYNAPSE_COMMON_CPP

#include "NPulseSynapseCommon.h"
#include "NPulseNeuron.h"
#include "NPulseChannel.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapseCommon::NPulseSynapseCommon(void)
: PulseAmplitude("PulseAmplitude",this,&NPulseSynapseCommon::SetPulseAmplitude),
  Resistance("Resistance",this,&NPulseSynapseCommon::SetResistance),
  Input("Input",this),
  Output("Output",this),
  PreOutput("PreOutput",this),
  InputPulseSignal("InputPulseSignal",this)
{
}

NPulseSynapseCommon::~NPulseSynapseCommon(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NPulseSynapseCommon::SetPulseAmplitude(const double &value)
{
 return true;
}

// ��� (������������� �������) �������
bool NPulseSynapseCommon::SetResistance(const double &value)
{
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseSynapseCommon* NPulseSynapseCommon::New(void)
{
 return new NPulseSynapseCommon;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseSynapseCommon::ADefault(void)
{
 // ��������� �������� ���� ����������
 // ��������� ������� ���������
 PulseAmplitude=1;

 // ��� (������������� �������) �������
 Resistance=10.0;

 Output.Assign(1,1,0.0);
 Input->Assign(1,1,0.0);
 InputPulseSignal=false;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSynapseCommon::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseSynapseCommon::AReset(void)
{
 // ����� ��������� ����������
 PreOutput=0;
 InputPulseSignal=false;
 PulseSignalTemp=false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseSynapseCommon::ACalculate(void)
{
 if((*Input)(0,0)>0 && !PulseSignalTemp)
 {
  InputPulseSignal=true;
  PulseSignalTemp=true;
 }

 if((*Input)(0,0)<=0 && PulseSignalTemp)
  PulseSignalTemp=false;

 bool res=ACalculate2();
 InputPulseSignal=false;
 return res;
}

bool NPulseSynapseCommon::ACalculate2(void)
{
 Output(0,0)=PreOutput;
 return true;
}
// --------------------------
}
#endif
