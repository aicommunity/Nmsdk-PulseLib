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

#ifndef NEYEMUSCLE_CPP
#define NEYEMUSCLE_CPP

//#pragma hdrstop

#include "NEyeMuscle.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NEyeMuscle::NEyeMuscle(void)
//: NADItem(name),
 : MulCoeffs("MulCoeffs",this),
  K("K",this),
  TC("TC",this)
{
 // ������������� ����� ���������
// AddLookupParameter("MulCoeffs",MulCoeffs);
// AddLookupParameter("K",K);
// AddLookupParameter("TC",TC);
}

NEyeMuscle::~NEyeMuscle(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ������
bool NEyeMuscle::SetAmplitude(real value)
{
 return true;
}

// ������������� �������
bool NEyeMuscle::SetFrequency(real value)
{
 if(value <=0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NEyeMuscle* NEyeMuscle::New(void)
{
 return new NEyeMuscle;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NEyeMuscle::ADefault(void)
{
 Real value;
 value.resize(3);
 value[0]=0.74;//0.4;
 value[1]=0.75;//0.12;
 value[2]=0.5;//0.0015;
 MulCoeffs=value;

 K=0.2;

 value.resize(3);
 value[0]=0.003;//1;
 value[1]=0.0092;//1;
 value[2]=0.0327;//1;
 TC=value;
 SetTimeStep(1000);
 SetNumInputs(1);
 SetNumOutputs(3);
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NEyeMuscle::ABuild(void)
{
 P1.resize(GetOutputDataSize(0));
 P2.resize(GetOutputDataSize(0));
 P3.resize(GetOutputDataSize(0));
 L.resize(GetOutputDataSize(0));

 Threshold.assign(GetOutputDataSize(0),0.5);

 return true;
}

// ����� �������� �����.
bool NEyeMuscle::AReset(void)
{
 P1.assign(GetOutputDataSize(0),0);
 P2.assign(GetOutputDataSize(0),0);
 P3.assign(GetOutputDataSize(0),0);
 L.assign(GetOutputDataSize(0),0);
 Speed.assign(GetOutputDataSize(0),0);
 Acceleration.assign(GetOutputDataSize(0),0);

 // �����
 Threshold.assign(GetOutputDataSize(0),0.5);

 return true;
}

// ��������� ������ ����� �������
bool NEyeMuscle::ACalculate(void)
{
 size_t k=0;
 SetOutputDataSize(0,NumInputs);
 SetOutputDataSize(1,NumInputs);
 SetOutputDataSize(2,NumInputs);
 for(int i=0;i<NumInputs;i++)
 {
  P1.resize(GetOutputDataSize(0),0);
  P2.resize(GetOutputDataSize(0),0);
  P3.resize(GetOutputDataSize(0),0);
  L.resize(GetOutputDataSize(0),0);
  Speed.resize(GetOutputDataSize(0),0);
  Acceleration.resize(GetOutputDataSize(0),0);
  Threshold.resize(GetOutputDataSize(0),0.5);

  for(size_t j=0;j<GetInputDataSize(i);j++)
  {
   if(k >= GetOutputDataSize(0))
	break;

   real in=GetInputData(i)->Double[j];
   ThresholdCount(k);
   in*=Threshold[k];

   real leng=MuscularReduction(k,in)*K;
   real speed=(leng-L[k])*TimeStep;
   Acceleration[k]=(speed-Speed[k])*TimeStep;
   Speed[k]=speed;
   L[k]=leng;
   POutputData[0].Double[k]=Acceleration[k];
   POutputData[1].Double[k]=L[k];
   POutputData[2].Double[k]=Speed[k];
   ++k;
  }
 }

 return true;
}
// --------------------------


// --------------------------
// �������������� ������� ������ ���������� ������
// --------------------------
// ���������� ����������
real NEyeMuscle::MuscularReduction(size_t k,real in)
{
 double p[3];
 p[0]=P1[k]+(in-P3[k]*MulCoeffs[2]-P1[k]*MulCoeffs[0])/(TC[0]*TimeStep);
 p[1]=P2[k]+(p[0]-P2[k]*MulCoeffs[1])/(TC[1]*TimeStep);
 p[2]=P3[k]+(p[1])/(TC[2]*TimeStep);
 P1[k]=p[0];
 P2[k]=p[1];
 P3[k]=p[2];

 return P3[k];
}


// ��������� �������
void NEyeMuscle::ThresholdCount(size_t k)
{
 Threshold[k]+=0.4*L[k]-0.005*L[k]*L[k];
 if(Threshold[k]>1.) Threshold[k]=1.;
 if(Threshold[k]<0.) Threshold[k]=0.;
}
// --------------------------
}
//---------------------------------------------------------------------------
#endif
