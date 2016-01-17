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
//#include "../BCL/NConnector.h"


namespace NMSDK {

// ������ NPulseLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZone::NPulseLTZone(void)
//: NADItem(name),
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
  NeuralPotential("NeuralPotential",this)
{
}

NPulseLTZone::~NPulseLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool NPulseLTZone::SetTimeConstant(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ��������� ���������
bool NPulseLTZone::SetPulseAmplitude(const double &value)
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
bool NPulseLTZone::CheckComponentType(UEPtr<UContainer> comp) const
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
 NPulseLTZoneCommon::ADefault();
 vector<size_t> size;

 SetNumOutputs(4);
 for(int i=0;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));

 // ��������� �������� ���� ����������
 TimeConstant=0.005;
 Threshold=0;//0.00001;

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
 NPulseLTZoneCommon::AReset();
 // ����� ��������� ����������
 NeuralPotential=0;

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
   if((inpsize=GetInputDataSize(i)[1]) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs/2.0;
 }

 PrePotential.v+=(NeuralPotential.v-PrePotential.v)/(TimeConstant.v*TimeStep);

 double current_time=GetTime().GetDoubleTime();
 if(CheckPulseOn())
 {
  POutputData[0].Double[0]=PulseAmplitude.v;
  POutputData[1].Double[0]=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(current_time);
  PulseFlag=true;
 }
 else
 if(CheckPulseOff())
 {
  PulseFlag=false;
  POutputData[0].Double[0]=0;
  POutputData[1].Double[0]=PrePotential.v;
 }
 else
  POutputData[1].Double[0]=PrePotential.v;

 list<double>::iterator I,J,K;
 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();

 if(AvgFrequencyCounter->size()>1)
 {
  while(I != J)
  {
   if(current_time-*I>AvgInterval)// && AvgFrequencyCounter->size()>3)
   {
	K=I;
	++I;
	AvgFrequencyCounter->erase(K);
   }
   else
	++I;
  }

  double frequency=0;
  frequency=static_cast<double>(AvgFrequencyCounter->size());//accumulate(AvgFrequencyCounter->begin(),AvgFrequencyCounter->end(),frequency);
  double interval=AvgFrequencyCounter->back()-AvgFrequencyCounter->front();
  if(interval>0 && frequency>2)
   POutputData[2].Double[0]=frequency/interval;
  else
  if(interval==0 && frequency>2);
  else
   POutputData[2].Double[0]=0;
 }
 else
  POutputData[2].Double[0]=0;

 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();
 SetOutputDataSize(3,MMatrixSize(1,AvgFrequencyCounter->size()),true);
 for(int i=0;i<POutputData[3].GetSize();i++,++I)
  POutputData[3].Double[i]=*I;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}

/// ���������� true ���� ������� ��� ��������� �������� ���������
bool NPulseLTZone::CheckPulseOn(void)
{
 return PrePotential.v>=Threshold.v;
}

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
bool NPulseLTZone::CheckPulseOff(void)
{
 return PrePotential.v<=0;
}
// --------------------------

// ������ NContinuesLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesLTZone::NContinuesLTZone(void)
//: NADItem(name),
 : TimeConstant("TimeConstant",this,&NContinuesLTZone::SetTimeConstant),
  PulseAmplitude("PulseAmplitude",this,&NContinuesLTZone::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  NeuralPotential("NeuralPotential",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
{
}

NContinuesLTZone::~NContinuesLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool NContinuesLTZone::SetTimeConstant(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ��������� ���������
bool NContinuesLTZone::SetPulseAmplitude(const double &value)
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
NContinuesLTZone* NContinuesLTZone::New(void)
{
 return new NContinuesLTZone;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NContinuesLTZone::CheckComponentType(UEPtr<UContainer> comp) const
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
bool NContinuesLTZone::ADefault(void)
{
 NLTZone::ADefault();

 SetNumOutputs(4);
 for(int i=0;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));

 // ��������� �������� ���� ����������
 TimeConstant=0.005;
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.0;
 AvgInterval=1;

// NumInputs=2;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NContinuesLTZone::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NContinuesLTZone::AReset(void)
{
 NLTZone::AReset();
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
bool NContinuesLTZone::ACalculate(void)
{
 // ������ �� ����
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)[1]) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs/2.0;
 }

 PrePotential.v=tanh(NeuralPotential.v);

 if(PrePotential.v>=Threshold.v)
 {
  POutputData[0].Double[0]=PrePotential.v;
  POutputData[1].Double[0]=PrePotential.v;
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

  POutputData[2].Double[0]=POutputData[0].Double[0];

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

// ������ NPulseSimpleLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSimpleLTZone::NPulseSimpleLTZone(void)
{
}

NPulseSimpleLTZone::~NPulseSimpleLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseSimpleLTZone* NPulseSimpleLTZone::New(void)
{
 return new NPulseSimpleLTZone;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseSimpleLTZone::CheckComponentType(UEPtr<UContainer> comp) const
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
bool NPulseSimpleLTZone::ADefault(void)
{
 NPulseLTZone::ADefault();
 generator.Default();

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSimpleLTZone::ABuild(void)
{
 generator.Build();
 return NPulseLTZone::ABuild();
}

// ����� �������� �����.
bool NPulseSimpleLTZone::AReset(void)
{
 generator.SetEnvironment(GetEnvironment());
 generator.SetActivity(true);
 generator.Reset();
 return NPulseLTZone::AReset();
}

// ��������� ������ ����� �������
bool NPulseSimpleLTZone::ACalculate(void)
{
 generator.SetEnvironment(GetEnvironment());
 // ������ �� ����
 NeuralPotential=0;

 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)[1]) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs;
 }

 generator.Amplitude=PulseAmplitude;
 if(NeuralPotential.v>200)
  NeuralPotential.v=200;
 if(NeuralPotential.v>0)
 {
  if(fabs(generator.Frequency.v-NeuralPotential.v)>0.001)
  {
   generator.Frequency=NeuralPotential.v;
//   generator.Reset();
  }
 }
 else
  generator.Frequency=0;

 generator.AvgInterval=AvgInterval;
 generator.Calculate();

 POutputData[0].Double[0]=generator.GetOutputData(0).Double[0];
 POutputData[1].Double[0]=generator.GetOutputData(0).Double[0];
 POutputData[2].Double[0]=NeuralPotential.v;

 POutputData[3]=generator.GetOutputData(3);

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

// ������ NContinuesSimpleLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesSimpleLTZone::NContinuesSimpleLTZone(void)
{
}

NContinuesSimpleLTZone::~NContinuesSimpleLTZone(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NContinuesSimpleLTZone* NContinuesSimpleLTZone::New(void)
{
 return new NContinuesSimpleLTZone;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NContinuesSimpleLTZone::CheckComponentType(UEPtr<UContainer> comp) const
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
bool NContinuesSimpleLTZone::ADefault(void)
{
 return NContinuesLTZone::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NContinuesSimpleLTZone::ABuild(void)
{
 return NContinuesLTZone::ABuild();
}

// ����� �������� �����.
bool NContinuesSimpleLTZone::AReset(void)
{
 return NContinuesLTZone::AReset();
}

// ��������� ������ ����� �������
bool NContinuesSimpleLTZone::ACalculate(void)
{
 // ������ �� ����
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)[1]) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs;
 }

 POutputData[0].Double[0]=NeuralPotential.v;
 POutputData[1].Double[0]=NeuralPotential.v;
 POutputData[2].Double[0]=NeuralPotential.v;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

}
#endif
