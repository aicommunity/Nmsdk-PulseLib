#ifndef NMSDK_NPulseChannelIaFCPP
#define NMSDK_NPulseChannelIaFCPP

#include "NPulseChannelIaF.h"
#include "NPulseMembraneIzhikevich.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelIaF::NPulseChannelIaF(void)
 :
   Cm("Cm", this),
   EL("EL", this),
   TauM("TauM", this),
   TRef("TRef", this),
   VReset("VReset", this),
   Ie("Ie", this),
   VMin("VMin", this),
   VResetEnable("VResetEnable",this),
   Vm("Vm", this)
{
}

NPulseChannelIaF::~NPulseChannelIaF(void)
{
}
// --------------------------    

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseChannelIaF* NPulseChannelIaF::New(void)
{
 return new NPulseChannelIaF;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseChannelIaF::ADefault(void)
{
 if(!NPulseChannelClassic::ADefault())
  return false;

 Cm=250e-12;
 EL=-70*1e-3;
 TauM=1*1e-3;
 TRef=2*1e-3;
 VReset=-0.08;
 Ie=0.0;
 VMin=-10000; // нижний порог не задан
 SynapticR = 3000000;
 VResetEnable=false;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseChannelIaF::ABuild(void)
{
 if(!NPulseChannelClassic::ABuild())
  return false;

 Vm->Assign(1,1,0.0);
 return true;
}

// Сброс процесса счета без потери настроек
bool NPulseChannelIaF::AReset(void)
{
 if(!NPulseChannelClassic::AReset())
  return false;

 Vm(0,0)=EL;
 return true;
}

// Выполняет расчет этого объекта
bool NPulseChannelIaF::ACalculate2(void)
{
 double step=1./TimeStep;
 Vm(0,0)+=step*(-Vm(0,0)/TauM + EL/TauM + SynapticI/TauM+Ie/Cm);

 if(IsNeuronActivated && VResetEnable)
 {
  Vm(0,0)=VReset;
 }

 // Получение информации об обратной связи
 UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(Owner);
 if(IsNeuronActivated && membrane && fabs(membrane->Feedback)>1e-3)
  Vm(0,0)=VReset;

 Output(0,0)=Vm(0,0);
 return true;
}
// --------------------------

}
#endif



