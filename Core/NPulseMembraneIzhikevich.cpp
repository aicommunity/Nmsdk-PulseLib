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

namespace {
// Keep in sync with NPulseMembrane::ABuild diagram layout.
constexpr double kMemSynX0     = 0.3;
constexpr double kMemBlockW    = 7.0;
constexpr double kMemChannelX  = kMemSynX0 + kMemBlockW + 1.5 * kMemBlockW; // 17.8
constexpr double kMemExcRowY0  = 1.6;
constexpr double kMemInhRowY0  = 5.25;
constexpr double kMemSynPitchY = 3.5;

inline MVector<double,3> izhChannelCoord(double rowY0, int numSynapses)
{
 const int n = numSynapses > 0 ? numSynapses : 1;
 return MVector<double,3>(kMemChannelX, rowY0 + (n - 1) * kMemSynPitchY * 0.5, 0.0);
}

inline double izhInhRowY0(int numExcSynapses)
{
 const int nExc = numExcSynapses > 0 ? numExcSynapses : 0;
 return std::max(kMemInhRowY0, kMemExcRowY0 + nExc * kMemSynPitchY);
}
} // namespace

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

 const int num_exc = int(NumExcitatorySynapses);
 const int num_inh = int(NumInhibitorySynapses);
 const MVector<double,3> exc_coord = izhChannelCoord(kMemExcRowY0, num_exc);
 const MVector<double,3> inh_coord = izhChannelCoord(izhInhRowY0(num_exc), num_inh);

 auto pos = AddMissingComponent<NPulseChannelIzhikevich>("PosChannel", "NPulseChannelIzhikevich");
 if(pos)
  pos->SetCoord(exc_coord);

 // Align Exc/Inh channels with NPulseMembrane diagram layout (Pos shares Exc slot)
 if(auto exc = GetComponent("ExcChannel", true))
  exc->SetCoord(exc_coord);
 if(auto inh = GetComponent("InhChannel", true))
  inh->SetCoord(inh_coord);

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
