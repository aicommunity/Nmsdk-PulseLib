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

#ifndef NPULSE_MEMBRANE_IZHIKEVICH_CPP
#define NPULSE_MEMBRANE_IZHIKEVICH_CPP

#include <algorithm>
#include "NPulseMembraneIzhikevich.h"
#include "NPulseChannelIzhikevich.h"


namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembraneIzhikevich::NPulseMembraneIzhikevich(void)
{
}

NPulseMembraneIzhikevich::~NPulseMembraneIzhikevich(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseMembraneIzhikevich* NPulseMembraneIzhikevich::New(void)
{
 return new NPulseMembraneIzhikevich;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseMembraneIzhikevich::CheckComponentType(UEPtr<UContainer> comp) const
{
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseMembraneIzhikevich::ADefault(void)
{
 if(!NPulseMembrane::ADefault())
  return false;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembraneIzhikevich::ABuild(void)
{
 if(!NPulseMembrane::ABuild())
  return false;

 AddMissingComponent<NPulseChannelIzhikevich>("PosChannel", "NPulseChannelIzhikevich");

 return true;
}

// Сброс процесса счета.
bool NPulseMembraneIzhikevich::AReset(void)
{
 if(!NPulseMembrane::AReset())
  return false;
 return true;
}

// Выполняет расчет этого объекта
bool NPulseMembraneIzhikevich::ACalculate2(void)
{
 if(!NPulseMembrane::ACalculate())
  return false;

 return true;
}
// --------------------------
}
#endif
