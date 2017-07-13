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
Mode("Mode",this),
TCMode("TCMode",this),
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
bool NPac::SetPulseAmplitude(const vector<Real> &value)
{
 return true;
}

// ���������� ������� ��������� ���������
bool NPac::SetSecretionTC(const vector<Real> &value)
{
 return true;
}

// ���������� ������� ������� ���������
bool NPac::SetDissociationTC(const vector<Real> &value)
{
 return true;
}

// ��������

bool NPac::SetGain(const vector<Real> &value)
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

 Mode=0;
 TCMode=1;
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
 int min_input_data_size(0);
 if(NumInputs>0)
 {
  min_input_data_size=GetInputDataSize(0)[1];
  for(int i=1;i<NumInputs;i++)
   if(min_input_data_size>GetInputDataSize(i)[1])
	min_input_data_size=GetInputDataSize(i)[1];
 }

 int size=(int(min_input_data_size)<GetOutputDataSize(0)[1])?int(min_input_data_size):GetOutputDataSize(0)[1];
 double Ts;
 double input;

 PreOutput->resize(NumInputs);
 for(int i=0;i<NumInputs;i++)
  PreOutput[i].resize(size);

 if(int(SecretionTC->size())<NumInputs || int((*SecretionTC)[0].size())<size)
 {
  LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Input dimensions have incorrect size.");
  return true;
 }

 if(TCMode == 0)
 {
  for(int i=0;i<NumInputs;i++)
  {
   for(int j=0;j<size;j++)
   {
	if(!GetInputData(i))
	 continue;
	input=GetInputData(i)->Double[j];

	PreOutput[i][j]=input/PulseAmplitude[i][j];
   }
  }
 }
 else
 if(TCMode == 1)
 {
  for(int i=0;i<NumInputs;i++)
  {
   for(int j=0;j<size;j++)
   {
	if(!GetInputData(i))
	 continue;
	input=GetInputData(i)->Double[j];

	Ts=(fabs(input)>0)?SecretionTC[i][j]:DissociationTC[i][j];
	PreOutput[i][j]+=(input/PulseAmplitude[i][j]-PreOutput[i][j])/(Ts*TimeStep);
   }
  }
 }
  for(int j=0;j<size;j++)
   POutputData[0].Double[j]=0;

 if(Mode == 0)
 {
  for(int i=0;i<NumInputs;i++)
  {
   for(int j=0;j<size;j++)
   {
	POutputData[0].Double[j]+=PreOutput[i][j]*Gain[i][j];
   }
  }
 }
 else
 if(Mode == 1)
 {
  for(int j=0;j<size;j++)
  {
   double pos_values=0;
   double neg_values=0;
   for(int i=0;i<NumInputs;i++)
   {
	if(PreOutput[i][j]*Gain[i][j]>0)
	 pos_values+=PreOutput[i][j]*Gain[i][j];
	if(PreOutput[i][j]*Gain[i][j]<0)
	 neg_values+=PreOutput[i][j]*Gain[i][j];
   }
   if(pos_values>1e-5 && neg_values<-1e-5)
   {
	for(int i=0;i<NumInputs;i++)
	 PreOutput[i][j]=0;

	POutputData[0].Double[j]=0.0;
   }
   else
    POutputData[0].Double[j]=pos_values+neg_values;
  }
 }

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NCPac* NCPac::New(void)
{
 return new NCPac;
}
// --------------------------
}
#endif
