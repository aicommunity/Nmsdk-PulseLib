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
 : CompartmentR("CompartmentR", this),
   SynapticR("SynapticR",this),
   SynapticI("SynapticI",this),
   CompartmentI("CompartmentI",this)
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

 CompartmentR = 1.0;
 SynapticR = 1.0;
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

 SynapticI=0;
 CompartmentI=0;
 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelClassic::ACalculate(void)
{
 SynapticI=0;
 for(int i=0;i<int(SynapticInputs->size());i++)
  for(int j=0;j<SynapticInputs[i]->GetCols();j++)
   SynapticI.v+=(*SynapticInputs[i])(0,j);
 SynapticI.v *= SynapticR.v;

 CompartmentI=0;
 for(int i=0;i<int(ChannelInputs->size());i++)
  for(int j=0;j<ChannelInputs[i]->GetCols();j++)
   CompartmentI.v+=(*ChannelInputs[i])(0,j);
 CompartmentI.v/=CompartmentR.v;

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



