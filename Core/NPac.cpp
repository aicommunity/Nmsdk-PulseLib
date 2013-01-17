// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */
#ifndef NPAC_CPP
#define NPAC_CPP

#include "NPac.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPac::NPac(void)
//: NADItem(name),
: PulseAmplitude("PulseAmplitude",this,&NPac::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NPac::SetSecretionTC),
DissociationTC("DissociationTC",this,&NPac::SetDissociationTC),
Gain("Gain",this,&NPac::SetGain),
PreOutput("PreOutput",this)
{
}

NPac::~NPac(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NPac::SetPulseAmplitude(vector<Real> value)
{
 return true;
}

// ���������� ������� ��������� ���������
bool NPac::SetSecretionTC(vector<Real> value)
{
 return true;
}

// ���������� ������� ������� ���������
bool NPac::SetDissociationTC(vector<Real> value)
{
 return true;
}

// ��������
bool NPac::SetGain(vector<Real> value)
{
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPac* NPac::New(void)
{
 return new NPac;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPac::ADefault(void)
{
 // ��������� �������� ���� ����������
 // ��������� ������� ���������
 vector<Real> values;

 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,1);
 PulseAmplitude=values;

 // ���������� ������� ��������� ���������
 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,0.05);
 SecretionTC=values;

 // ���������� ������� ������� ���������
 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,0.5);
 DissociationTC=values;

 // ��������
 values.resize(4);
 values[0].resize(2);
 values[0][0]=30;
 values[0][1]=30;
 values[1].resize(2);
 values[1][0]=30;
 values[1][1]=30;
 values[2].resize(2);
 values[2][0]=-30;
 values[2][1]=-30;
 values[3].resize(2);
 values[3][0]=-30;
 values[3][1]=-30;
 Gain=values;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPac::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPac::AReset(void)
{
 // ����� ��������� ����������
 PreOutput->resize(0);

 return true;
}

// ��������� ������ ����� �������
bool NPac::ACalculate(void)
{
 size_t size=(MinInputDataSize<GetOutputDataSize(0))?MinInputDataSize:GetOutputDataSize(0);
 real Ts;
 real input;

 PreOutput->resize(NumInputs);
 for(int i=0;i<NumInputs;i++)
  PreOutput[i].resize(size);

 for(int i=0;i<NumInputs;i++)
 {
  for(size_t j=0;j<size;j++)
  {
   input=GetInputData(i)->Double[j];

   Ts=(fabs(input)>0)?SecretionTC[i][j]:DissociationTC[i][j];
   PreOutput[i][j]+=(input/PulseAmplitude[i][j]-PreOutput[i][j])/(Ts*TimeStep);
  }
 }

 for(size_t j=0;j<size;j++)
  POutputData[0].Double[j]=0;

 for(int i=0;i<NumInputs;i++)
 {
  for(size_t j=0;j<size;j++)
  {
   POutputData[0].Double[j]+=PreOutput[i][j]*Gain[i][j];
  }
 }

 return true;
}
// --------------------------
}
#endif
