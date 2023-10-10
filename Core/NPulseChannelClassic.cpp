#ifndef NMSDK_NPulseSynChannelIzhikevichCPP
#define NMSDK_NPulseChannelClassicCPP

#include "NPulseChannelClassic.h"
#include "NPulseMembraneIzhikevich.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
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
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseChannelClassic* NPulseChannelClassic::New(void)
{
 return new NPulseChannelClassic;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseChannelClassic::ADefault(void)
{
 if(!NPulseChannelCommon::ADefault())
  return false;

 CompartmentR = 1.0;
 SynapticR = 1.0;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseChannelClassic::ABuild(void)
{
 if(!NPulseChannelCommon::ABuild())
  return false;

 return true;
}

// Сброс процесса счета без потери настроек
bool NPulseChannelClassic::AReset(void)
{
 if(!NPulseChannelCommon::AReset())
  return false;

 SynapticI=0;
 CompartmentI=0;
 return true;
}

// Выполняет расчет этого объекта
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



