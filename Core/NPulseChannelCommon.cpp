// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_CHANNEL_COMMON_CPP
#define NPULSE_CHANNEL_COMMON_CPP

#include "NPulseChannelCommon.h"
#include "NPulseMembrane.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelCommon::NPulseChannelCommon(void)
 : Type("Type", this, &NPulseChannelCommon::SetType),
   UseAveragePotential("UseAveragePotential",this),
   UseAverageSynapsis("UseAverageSynapsis",this),
   ChannelInputs("ChannelInputs",this),
   SynapticInputs("SynapticInputs",this),
   Output("Output",this),
   SumChannelInputs("SumChannelInputs",this),
   IsNeuronActivated("IsNeuronActivated",this)
{
}

NPulseChannelCommon::~NPulseChannelCommon(void)
{
}
// --------------------------


// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
int NPulseChannelCommon::GetNumSynapses(void) const
{
 return int(SynapticInputs->size());
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
//
bool NPulseChannelCommon::SetType(const double &value)
{
 Type.v=value;

 UEPtr<NPulseMembrane> membr=dynamic_pointer_cast<NPulseMembrane>(Owner);
 if(membr)
 {
  membr->UpdateChannelData(dynamic_cast<NPulseChannelCommon*>(this));
 }

 for(int i=0;i<int(SynapticInputs->size());i++)
 {
  UEPtr<NPulseSynapseCommon> synapse = dynamic_cast<NPulseSynapseCommon*>(SynapticInputs.GetItem(i));
  if(synapse)
   synapse->Type = value;
 }

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseChannelCommon* NPulseChannelCommon::New(void)
{
 return new NPulseChannelCommon;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseChannelCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPulseChannelCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseChannelCommon::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
/// Принимает сигнал о генерации сигнала низкопороговой зоной
/// (однократно по началу импульса)
void NPulseChannelCommon::NeuronActivated(void)
{
 IsNeuronActivated=true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseChannelCommon::ADefault(void)
{
 Type=0;
 UseAveragePotential=true;
 UseAverageSynapsis=false;
 Output.Assign(1,1,0.0);
 SumChannelInputs.Assign(1,1,0.0);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseChannelCommon::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPulseChannelCommon::AReset(void)
{
 Output.ToZero();
 SumChannelInputs.ToZero();
 IsNeuronActivated=false;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseChannelCommon::ACalculate(void)
{
 ACalculate2();
 IsNeuronActivated=false;
 return true;
}

bool NPulseChannelCommon::ACalculate2(void)
{
 return true;
}

// --------------------------
}

#endif

