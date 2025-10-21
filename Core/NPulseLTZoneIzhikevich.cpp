#ifndef NMSDK_NPulseLTZoneIzhikevichCPP
#define NMSDK_NPulseLTZoneIzhikevichCPP

#include "NPulseLTZoneIzhikevich.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuronCommon.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZoneIzhikevich::NPulseLTZoneIzhikevich(void)
{
}

NPulseLTZoneIzhikevich::~NPulseLTZoneIzhikevich(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseLTZoneIzhikevich* NPulseLTZoneIzhikevich::New(void)
{
 return new NPulseLTZoneIzhikevich;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseLTZoneIzhikevich::ADefault(void)
{
 if(!NPulseLTZoneCommon::ADefault())
  return false;

 Threshold=30.0;
 NumChannelsInGroup=1;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLTZoneIzhikevich::ABuild(void)
{
 if(!NPulseLTZoneCommon::ABuild())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NPulseLTZoneIzhikevich::AReset(void)
{
 if(!NPulseLTZoneCommon::AReset())
  return false;
 PulseCounter=0;
 return true;
}

// ��������� ������ ����� �������
bool NPulseLTZoneIzhikevich::ACalculate(void)
{
 if(Inputs->size()<2)
  return true;
 PrePotential.v=Inputs[0](0,0);

 double current_time=GetTime().GetDoubleTime();
 OutputPotential(0,0)=PrePotential.v;
 if(PrePotential.v>=Threshold && !PulseFlag)
 {
  Output(0,0)=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(current_time);
  PulseFlag=true;
  PulseCounter=int(PulseLength*TimeStep);
 }
 else
 if(PulseFlag && PulseCounter<0)
 {
  PulseFlag=false;
  PulseCounter=0;
  Output(0,0)=0;
 }

 if(PulseFlag)
  --PulseCounter.v;

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
 OutputPulseTimes.Resize(1,int(AvgFrequencyCounter->size()));
 for(int i=0;i<OutputPulseTimes.GetCols();i++,++I)
  OutputPulseTimes(0,i)=*I;

if(MainOwner.lock())
 dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner.lock())->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}

/// ���������� true ���� ������� ��� ��������� �������� ���������
bool NPulseLTZoneIzhikevich::CheckPulseOn(void)
{
 return PrePotential.v>=Threshold;
}

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
bool NPulseLTZoneIzhikevich::CheckPulseOff(void)
{
 return false;
}
// --------------------------

}
#endif



