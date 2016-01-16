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

#ifndef NPULSE_MEMBRANE_COMMON_CPP
#define NPULSE_MEMBRANE_COMMON_CPP

#include <algorithm>
#include "NPulseMembraneCommon.h"
#include "NPulseChannel.h"



namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembraneCommon::NPulseMembraneCommon(void)
{
}

NPulseMembraneCommon::~NPulseMembraneCommon(void)
{
}
// --------------------------

// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы
size_t NPulseMembraneCommon::GetNumChannels(void) const
{
 return Channels.size();
}

NPulseChannelCommon* NPulseMembraneCommon::GetChannel(size_t i)
{
 return Channels[i];
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseMembraneCommon* NPulseMembraneCommon::New(void)
{
 return new NPulseMembraneCommon;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseMembraneCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp))
  return true;

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
bool NPulseMembraneCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 vector<NPulseChannelCommon* >::iterator I;

 if(channel)
 {
   if(find(Channels.begin(),Channels.end(),channel) == Channels.end())
	Channels.push_back(channel);
 }

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseMembraneCommon::ADelComponent(UEPtr<UContainer> comp)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  vector<NPulseChannelCommon*>::iterator I;
  I=find(Channels.begin(),Channels.end(),channel);
  if(I != Channels.end())
   Channels.erase(I);
 }
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseMembraneCommon::ADefault(void)
{
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembraneCommon::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPulseMembraneCommon::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool NPulseMembraneCommon::ACalculate(void)
{
 return true;
}
// --------------------------
}
#endif
