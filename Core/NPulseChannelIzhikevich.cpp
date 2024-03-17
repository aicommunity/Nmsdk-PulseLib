#ifndef NMSDK_NPulseChannelIzhikevichCPP
#define NMSDK_NPulseChannelIzhikevichCPP

#include "NPulseChannelIzhikevich.h"
#include "NPulseMembraneIzhikevich.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelIzhikevich::NPulseChannelIzhikevich(void)
 : A("A",this),
   B("B",this),
   C("C", this),
   D("D", this),
   OutputU("OutputU",this),
   V("V",this),
   U("U",this)
{
}

NPulseChannelIzhikevich::~NPulseChannelIzhikevich(void)
{
}
// --------------------------    

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannelIzhikevich* NPulseChannelIzhikevich::New(void)
{
 return new NPulseChannelIzhikevich;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
/// ��������� ������ � ��������� ������� �������������� �����
/// (���������� �� ������ ��������)
void NPulseChannelIzhikevich::NeuronActivated(void)
{

}

// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannelIzhikevich::ADefault(void)
{
 if(!NPulseChannelClassic::ADefault())
  return false;

 A=0.02;
 B=0.2;
 D=2.0;
 C=-65.0;
 OutputU.Assign(1,1,0.0);
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannelIzhikevich::ABuild(void)
{
 if(!NPulseChannelClassic::ABuild())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NPulseChannelIzhikevich::AReset(void)
{
 if(!NPulseChannelClassic::AReset())
  return false;

 U=0;
 V=-70;
 Output.ToZero();
 OutputU.ToZero();
 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelIzhikevich::ACalculate2(void)
{
 if(IsNeuronActivated)
 {
  V=C;
  U=U+D;
 }
 else
 {
  V=V+(0.04*V*V+5*V+140-U+SumSynapticInput(0,0))/TimeStep;
  U=U+(A*(B*V-U))/TimeStep;
 }

 Output(0,0)=V;
 OutputU(0,0)=U;
 return true;
}
// --------------------------

}
#endif



