#ifndef NMSDK_NSYNAPSECLASSICSLV_CPP
#define NMSDK_NSYNAPSECLASSICSLV_CPP

#include "NSynapseClassicSlv.h"

namespace NMSDK {

NSynapseSpikeData::NSynapseSpikeData(void)
 : SpikeTimeStamp(0.0), Weight(0.0)
{
}

NSynapseSpikeData::NSynapseSpikeData(const NSynapseSpikeData& value)
 : SpikeTimeStamp(value.SpikeTimeStamp), Weight(value.Weight)
{
}

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseClassicSlv::NSynapseClassicSlv(void)
 : SpikeBufferSize("SpikeBufferSize",this)
{
}

NSynapseClassicSlv::~NSynapseClassicSlv(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
bool NSynapseClassicSlv::SetSpikeBufferSize(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseClassicSlv* NSynapseClassicSlv::New(void)
{
 return new NSynapseClassicSlv;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������ 
// --------------------------
// ������ ���� ������� �� ��������� �����
/// time_value � �������� �� ������� ��������
double NSynapseClassicSlv::CalcCurrent(double tau_s, double current, double time_value, double weight)
{
 return weight*((time_value>0)?M_E*time_value*exp(-time_value/tau_s)*current/tau_s:0.0);
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseClassicSlv::ADefault(void)
{
 if(!NSynapseClassic::ADefault())
  return false;
 SpikeBufferSize=10;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NSynapseClassicSlv::ABuild(void)
{
 if(!NSynapseClassic::ABuild())
  return false;

 SpikeBuffer.resize(SpikeBufferSize);
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NSynapseClassicSlv::AReset(void)
{
 if(!NSynapseClassic::AReset())
  return false;

 FirstSpikeBufferIndex=LastSpikeBufferIndex=0;
 SpikeBuffer.clear();
 SpikeBuffer.resize(SpikeBufferSize);
 PreviousInput.ToZero();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseClassicSlv::ACalculate2(void)
{
 if(!Input.IsConnected() || Input->GetCols()<=0)
 {
  Output(0,0)=0.0;
  FirstSpikeBufferIndex=LastSpikeBufferIndex=0;
  return true;
 }

 // ��������� ����� ������� ���� ��� ����� �����
 if((*Input)(0,0)>0)
 {
  if(PreviousInput.GetCols()<=0 || PreviousInput(0,0)>0)
  {
   // �� ������ ������, ����� ��� ���
  }
  else // ������ ������
  {
   LastSpikeBufferIndex++;
   if(LastSpikeBufferIndex>=SpikeBuffer.size())
	LastSpikeBufferIndex=0;

   if(LastSpikeBufferIndex<=FirstSpikeBufferIndex)
   {
	++FirstSpikeBufferIndex;
	if(FirstSpikeBufferIndex>=SpikeBuffer.size())
     FirstSpikeBufferIndex=0;
   }
  }

  SpikeBuffer[LastSpikeBufferIndex].SpikeTimeStamp=GetTime().GetDoubleTime();
  SpikeBuffer[LastSpikeBufferIndex].Weight=Weight;
 }
 PreviousInput=*Input;


 // ������������ ������� �������
 double result(0.0);
 if(FirstSpikeBufferIndex<LastSpikeBufferIndex)
  for(size_t i=FirstSpikeBufferIndex;i<LastSpikeBufferIndex;i++)
  {
   double current_time=GetTime().GetDoubleTime()-SpikeBuffer[i].SpikeTimeStamp;
   result+=CalcCurrent(TauS, Current, current_time, SpikeBuffer[i].Weight);
  }
 else
 if(FirstSpikeBufferIndex>LastSpikeBufferIndex)
 {
  for(size_t i=FirstSpikeBufferIndex;i<SpikeBuffer.size();i++)
  {
   double current_time=GetTime().GetDoubleTime()-SpikeBuffer[i].SpikeTimeStamp;
   result+=CalcCurrent(TauS, Current, current_time, SpikeBuffer[i].Weight);
  }

  for(size_t i=0;i<LastSpikeBufferIndex;i++)
  {
   double current_time=GetTime().GetDoubleTime()-SpikeBuffer[i].SpikeTimeStamp;
   result+=CalcCurrent(TauS, Current, current_time, SpikeBuffer[i].Weight);
  }
 }

 Output(0,0)=result;
 return true;
}
// --------------------------

}
#endif



