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

#ifndef NPULSE_SYNAPSE_CPP
#define NPULSE_SYNAPSE_CPP

#include "NPulseSynapse.h"
#include "NPulseNeuron.h"
#include "NPulseChannel.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapse::NPulseSynapse(void)
 //: NConnector(name),
: PulseAmplitude("PulseAmplitude",this,&NPulseSynapse::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NPulseSynapse::SetSecretionTC),
DissociationTC("DissociationTC",this,&NPulseSynapse::SetDissociationTC),
InhibitionCoeff("InhibitionCoeff",this,&NPulseSynapse::SetInhibitionCoeff),
Resistance("Resistance",this,&NPulseSynapse::SetResistance),

PreOutput("PreOutput",this)
{
}

NPulseSynapse::~NPulseSynapse(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NPulseSynapse::SetPulseAmplitude(real value)
{
 return true;
}

// ���������� ������� ��������� ���������
bool NPulseSynapse::SetSecretionTC(real value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ���������� ������� ������� ���������
bool NPulseSynapse::SetDissociationTC(real value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// ����������� ����������������� ����������
bool NPulseSynapse::SetInhibitionCoeff(real value)
{
 if(Resistance.v > 0)
  OutputConstData=4.0*(value+1)/Resistance.v;
 else
  OutputConstData=0;

 return true;
}

// ��� (������������� �������) �������
bool NPulseSynapse::SetResistance(real value)
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
NPulseSynapse* NPulseSynapse::New(void)
{
 return new NPulseSynapse;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseSynapse::ADefault(void)
{
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
 Resistance=1.0e8;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSynapse::ABuild(void)
{
 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;

 return true;
}

// ����� �������� �����.
bool NPulseSynapse::AReset(void)
{
 // ����� ��������� ����������
 PreOutput=0;

 return true;
}

// ��������� ������ ����� �������
bool NPulseSynapse::ACalculate(void)
{
 real input=0;

 if(NumInputs >0 && GetInputDataSize(0)>0)
 {
  input=GetInputData(0)->Double[0];
  if(MainOwner && Owner)
  {
   if(static_pointer_cast<NPulseChannel>(Owner)->Type() < 0)
	++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
   else
	++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
  }
 }

 if(input>0)
  PreOutput.v+=(input/PulseAmplitude.v-PreOutput.v)/VSecretionTC;
 else
  PreOutput.v-=PreOutput.v/VDissociationTC;

 POutputData[0].Double[0]=OutputConstData*
						(1.0-InhibitionCoeff.v*PreOutput.v)*PreOutput.v;

 if(POutputData[0].Double[0]<0)
  POutputData[0].Double[0]=0;

 return true;
}
// --------------------------
}
#endif
