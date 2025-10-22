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

namespace NMSDK {

// ������ NPulseLTZoneThreshold
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZoneThreshold::NPulseLTZoneThreshold(void)
{
}

NPulseLTZoneThreshold::~NPulseLTZoneThreshold(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseLTZoneThreshold* NPulseLTZoneThreshold::New(void)
{
 return new NPulseLTZoneThreshold;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseLTZoneThreshold::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 return false;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseLTZoneThreshold::ADefault(void)
{
 NPulseLTZoneCommon::ADefault();

 // ��������� �������� ���� ����������
 Threshold=0.00001;

 NumChannelsInGroup=2;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLTZoneThreshold::ABuild(void)
{
 NPulseLTZoneCommon::ABuild();
 return true;
}

// ����� �������� �����.
bool NPulseLTZoneThreshold::AReset(void)
{
 NPulseLTZoneCommon::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NPulseLTZoneThreshold::ACalculate2(void)
{
 double current_time=GetTime().GetDoubleTime();
 if(CheckPulseOn())
 {
  // ----> �������
  //double eee = Potential.v;
  // < ----

  Output(0,0)=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(current_time);
  PulseFlag=true;
 }
 else
 if(CheckPulseOff())
 {
  PulseFlag=false;
  Output(0,0)=0;
 }

 OutputPotential(0,0)=Potential.v;

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
   OutputFrequency(0,0)=frequency/interval;
  else
  if(interval==0 && frequency>2);
  else
   OutputFrequency(0,0)=0;
 }
 else
  OutputFrequency(0,0)=0;

 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();
 OutputPulseTimes->Resize(1,int(AvgFrequencyCounter->size()));
 for(int i=0;i<int(AvgFrequencyCounter->size());i++,++I)
  OutputPulseTimes(0,i)=*I;

if(MainOwner.lock())
 dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner.lock())->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}

/// ���������� true ���� ������� ��� ��������� �������� ���������
bool NPulseLTZoneThreshold::CheckPulseOn(void)
{
 return Potential.v>=Threshold.v;
}

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
bool NPulseLTZoneThreshold::CheckPulseOff(void)
{
 return Potential.v<ThresholdOff;
}
// --------------------------


// ������ NPulseLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZone::NPulseLTZone(void)
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
  UseLTZIntegtation("UseLTZIntegtation",this,&NPulseLTZone::SetLTZIntegtation),
  UseSpikeStabilizer("UseSpikeStabilizer",this, &NPulseLTZone::SetUseSpikeStabilizer)
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
// ������������� ������� ������������� ���������� � LTZ
bool NPulseLTZone::SetLTZIntegtation(const bool &value)
{
 return true;
}

// ���� ��������� ������������ ������������ ��������
bool NPulseLTZone::SetUseSpikeStabilizer(const bool &value)
{
 TimeConstant=0.00144; // Force change time constant
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
bool NPulseLTZone::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
 NPulseLTZoneThreshold::ADefault();

 // ��������� �������� ���� ����������
 NumChannelsInGroup=2;
 TimeConstant=0.00144;//0.005;
 Threshold=0.00001;
 UseLTZIntegtation = true;
 UseSpikeStabilizer=true;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLTZone::ABuild(void)
{
 NPulseLTZoneThreshold::ABuild();
 return true;
}

// ����� �������� �����.
bool NPulseLTZone::AReset(void)
{
 NPulseLTZoneThreshold::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NPulseLTZone::ACalculate2(void)
{
 if(UseLTZIntegtation)
 {
  double input(0.0);
  if(UseSpikeStabilizer.v)
  {
   input=(Output(0,0)>0)?-Threshold.v:NeuralPotential.v;
  }
  PrePotential.v+=(input-PrePotential.v)/(TimeConstant.v*TimeStep);
 }
 else
  PrePotential.v = NeuralPotential.v;

 Potential.v=PrePotential.v;
 return NPulseLTZoneThreshold::ACalculate2();
}
// --------------------------

// ������ NContinuesLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesLTZone::NContinuesLTZone(void)
{
}

NContinuesLTZone::~NContinuesLTZone(void)
{
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
bool NContinuesLTZone::CheckComponentType(std::shared_ptr<UContainer> comp) const
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

 // ��������� �������� ���� ����������
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.0;
 AvgInterval=1;

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

 OutputPotential.ToZero();

 return true;
}

// ��������� ������ ����� �������
bool NContinuesLTZone::ACalculate2(void)
{
 Potential.v=tanh(NeuralPotential.v);

 if(Potential.v>=Threshold.v)
 {
  Output(0,0)=Potential.v;
  OutputPotential(0,0)=Potential.v;
  PulseFlag=true;
 }
 else
 if(Potential.v<=0)
 {
  PulseFlag=false;
  Output(0,0)=0;
  OutputPotential(0,0)=Potential.v;
 }
 else
  OutputPotential(0,0)=Potential.v;

if(MainOwner.lock())
 dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner.lock())->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

// ������ NPulseSimpleLTZone
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSimpleLTZone::NPulseSimpleLTZone(void)
: MaxFrequency("MaxFrequency",this)
{
 AddStaticComponent(
  "NPGenerator",
  "PGenerator",
  std::shared_ptr<UContainer>(&generator, RDK::NonOwningDeleter())
 );
}

NPulseSimpleLTZone::~NPulseSimpleLTZone(void)
{
}
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
bool NPulseSimpleLTZone::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseGenerator>(comp))
  return true;

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

 NumChannelsInGroup=1;
 MaxFrequency=100;
 //generator.Default();

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSimpleLTZone::ABuild(void)
{
 //generator.Build();
 return NPulseLTZone::ABuild();
}

// ����� �������� �����.
bool NPulseSimpleLTZone::AReset(void)
{
 //generator.SetEnvironment(GetEnvironment());
 //generator.SetActivity(true);
 //generator.Reset();
 return NPulseLTZone::AReset();
}

// ��������� ������ ����� �������
bool NPulseSimpleLTZone::ACalculate2(void)
{
 generator.Amplitude=PulseAmplitude;
 if(Potential.v>MaxFrequency)
  Potential.v=MaxFrequency;
 if(Potential.v>0)
 {
  if(fabs(generator.Frequency.v-Potential.v)>0.001)
  {
   generator.Frequency=Potential.v;
//   generator.Reset();
  }
 }
 else
  generator.Frequency=0;

 generator.AvgInterval=AvgInterval;
 generator.Calculate();

 Output(0,0)=generator.Output(0,0);
 OutputPotential(0,0)=generator.OutputPotential(0,0);
 OutputFrequency(0,0)=Potential.v;

 OutputPulseTimes(0,0)=generator.OutputPulseTimes(0,0);

if(MainOwner.lock())
 dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner.lock())->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

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
bool NContinuesSimpleLTZone::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
bool NContinuesSimpleLTZone::ACalculate2(void)
{
 Output(0,0)=Potential.v;
 OutputPotential(0,0)=Potential.v;

if(MainOwner.lock())
 dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner.lock())->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

}
#endif
