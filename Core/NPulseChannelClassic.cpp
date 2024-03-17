#ifndef NMSDK_NPulseSynChannelIzhikevichCPP
#define NMSDK_NPulseChannelClassicCPP

#include "NPulseChannelClassic.h"
#include "NPulseMembraneIzhikevich.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelClassic::NPulseChannelClassic(void)
 : ChannelInputCoeff("ChannelInputCoeff", this),
   SynapticInputCoeff("SynapticInputCoeff",this)
{
}

NPulseChannelClassic::~NPulseChannelClassic(void)
{
}
// --------------------------    

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannelClassic* NPulseChannelClassic::New(void)
{
 return new NPulseChannelClassic;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannelClassic::ADefault(void)
{
 if(!NPulseChannelCommon::ADefault())
  return false;

 ChannelInputCoeff = 1.0;
 SynapticInputCoeff = 1.0;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannelClassic::ABuild(void)
{
 if(!NPulseChannelCommon::ABuild())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NPulseChannelClassic::AReset(void)
{
 if(!NPulseChannelCommon::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelClassic::ACalculate(void)
{
 SumSynapticInput.ToZero();
 for(int i=0;i<int(SynapticInputs->size());i++)
  for(int j=0;j<SynapticInputs[i].GetCols();j++)
   SumSynapticInput(0,0)+=SynapticInputs[i](0,j);
 SumSynapticInput.v *= SynapticInputCoeff.v;

 SumChannelInput.ToZero();
 for(int i=0;i<int(ChannelInputs->size());i++)
  for(int j=0;j<ChannelInputs[i].GetCols();j++)
   SumChannelInput(0,0)+=ChannelInputs[i](0,j);
 SumChannelInput.v *= ChannelInputCoeff.v;

 ACalculate2();
 IsNeuronActivated=false;
 return true;
}

bool NPulseChannelClassic::ACalculate2(void)
{
 return true;
}
// --------------------------

}
#endif



