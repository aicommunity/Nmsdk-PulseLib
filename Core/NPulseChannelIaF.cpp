#ifndef NMSDK_NPulseChannelIaFCPP
#define NMSDK_NPulseChannelIaFCPP

#include "NPulseChannelIaF.h"
#include "NPulseMembraneIzhikevich.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelIaF::NPulseChannelIaF(void)
 :
   Cm("Cm", this),
   EL("EL", this),
   TauM("TauM", this),
   TRef("TRef", this),
   VReset("VReset", this),
   VMin("VMin", this),
   VResetEnable("VResetEnable",this),
   Vm("Vm", this),
   Rm("Rm", this)
{
}

NPulseChannelIaF::~NPulseChannelIaF(void)
{
}
// --------------------------    

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannelIaF* NPulseChannelIaF::New(void)
{
 return new NPulseChannelIaF;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannelIaF::ADefault(void)
{
 if(!NPulseChannelClassic::ADefault())
  return false;

 Cm=100e-12;
 EL=-70*1e-3;
 TauM=1*1e-3;
 TRef=2*1e-3;
 VReset=-0.08;
 VMin=-0.01;
 SynapticR = 1;
 VResetEnable=false;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannelIaF::ABuild(void)
{
 if(!NPulseChannelClassic::ABuild())
  return false;

 Vm->Assign(1,1,0.0);
 Rm->Assign(1,1,0.0);
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NPulseChannelIaF::AReset(void)
{
 if(!NPulseChannelClassic::AReset())
  return false;

 Vm(0,0)=EL;
 if(Cm > 0.0)
  Rm(0,0)=TauM/Cm;
 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelIaF::ACalculate2(void)
{
 if(Cm > 0.0)
  Rm(0,0)=TauM/Cm;

 double step=1./TimeStep;
 Vm(0,0)+=step*(-Vm(0,0)/TauM + EL/TauM + SynapticI/Cm+CompartmentI/Cm);

 if(IsNeuronActivated && VResetEnable)
 {
  Vm(0,0)=VReset;
 }

 // ��������� ���������� �� �������� �����
 UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(Owner);
 if(IsNeuronActivated && membrane && fabs(membrane->Feedback)>1e-3)
  Vm(0,0)=VReset;

 Output(0,0)=Vm(0,0);
 return true;
}
// --------------------------

}
#endif



