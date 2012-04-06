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

#ifndef NPULSE_LTZONE_CPP
#define NPULSE_LTZONE_CPP

#include <numeric>
#include "NPulseLTZone.h"
#include "NPulseChannel.h"
#include "NPulseNeuron.h"
#include "..\BCL\NConnector.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZone::NPulseLTZone(void)
//: NADItem(name),
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
  Threshold("Threshold",this,&NPulseLTZone::SetThreshold),
  PulseAmplitude("PulseAmplitude",this,&NPulseLTZone::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  NeuralPotential("NeuralPotential",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
{
 Neuron=0;
}

NPulseLTZone::~NPulseLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool NPulseLTZone::SetTimeConstant(real value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ����� �������
bool NPulseLTZone::SetThreshold(real value)
{
 return true;
}

// ������������� ��������� ���������
bool NPulseLTZone::SetPulseAmplitude(real value)
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
NPulseLTZone* NPulseLTZone::New(void)
{
 return new NPulseLTZone;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseLTZone::CheckComponentType(UEPtr<UAContainer> comp) const
{
// if(dynamic_cast<const NConnector*>(comp))
//  return true;

 return false;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseLTZone::ADefault(void)
{
 vector<size_t> size;

 SetNumOutputs(3);
 size.resize(NumOutputs);
 for(size_t i=0;i<size.size();i++)
  size[i]=1;

 SetOutputDataSize(size);

 // ��������� �������� ���� ����������
 TimeConstant=0.005;
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.00001;
 AvgInterval=1;

// NumInputs=2;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLTZone::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseLTZone::AReset(void)
{
 // ����� ��������� ����������
 NeuralPotential=0;
 PrePotential=0;
 PulseCounter=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 FillOutputData(0);

 return true;
}

// ��������� ������ ����� �������
bool NPulseLTZone::ACalculate(void)
{
 // ������ �� ����
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	real *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs/2.0;
 }

// real prev=PrePotential;
 PrePotential.v+=(NeuralPotential.v-PrePotential.v)/(TimeConstant.v*TimeStep);
// PrePotential=NeuralPotential.v;

// if(PulseCounter.v > 0)
//  --PulseCounter;

 if(PrePotential.v>=Threshold.v)
 {
  POutputData[0].Double[0]=PulseAmplitude.v;
  POutputData[1].Double[0]=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(GetDoubleTime());

  PulseFlag=true;
 }
 else
 if(PrePotential.v<=0)
 {
  PulseFlag=false;
  POutputData[0].Double[0]=0;
  POutputData[1].Double[0]=PrePotential.v;
 }
 else
  POutputData[1].Double[0]=PrePotential.v;

 list<real>::iterator I,J,K;
 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();

 if(AvgFrequencyCounter->size()>1)
 {
  while(I != J)
  {
   if(GetDoubleTime()-*I>AvgInterval)// && AvgFrequencyCounter->size()>3)
   {
	K=I;
	++I;
	AvgFrequencyCounter->erase(K);
   }
   else
	++I;
  }

  real frequency=0;
  frequency=static_cast<real>(AvgFrequencyCounter->size());//accumulate(AvgFrequencyCounter->begin(),AvgFrequencyCounter->end(),frequency);
  real interval=AvgFrequencyCounter->back()-AvgFrequencyCounter->front();
  if(interval>0 && frequency>2)
   POutputData[2].Double[0]=frequency/interval;
  else
  if(interval==0 && frequency>2);
  else
   POutputData[2].Double[0]=0;
 }
 else
  POutputData[2].Double[0]=0;


 if(MainOwner)
  static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveOutputs.v+=static_cast<real>(GetNumAConnectors(0));

 return true;
}
// --------------------------
}
#endif
