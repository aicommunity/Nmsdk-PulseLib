// ===========================================================
// Version:        3.0.2
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

#ifndef NPULSE_LTZONE_COMMON_CPP
#define NPULSE_LTZONE_COMMON_CPP

#include <numeric>
#include "NPulseLTZoneCommon.h"
#include "NPulseChannel.h"
#include "NPulseNeuron.h"
//#include "../BCL/NConnector.h"


namespace NMSDK {

// Методы NLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLTZone::NLTZone(void)
 : Threshold("Threshold",this,&NLTZone::SetThreshold)
{
 Neuron=0;
}

NLTZone::~NLTZone(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает порог нейрона
bool NLTZone::SetThreshold(const double &value)
{
 return true;
}

// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NLTZone::ADefault(void)
{
 Threshold=0.0;

 return true;
}

// Сброс процесса счета.
bool NLTZone::AReset(void)
{
 CachedNumAConnectors=static_cast<double>(GetNumAConnectors(0));
 return true;
}
// --------------------------


// Методы NPulseLTZoneCommon
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZoneCommon::NPulseLTZoneCommon(void)
 :
  PulseAmplitude("PulseAmplitude",this,&NPulseLTZoneCommon::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
{
}

NPulseLTZoneCommon::~NPulseLTZoneCommon(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NPulseLTZoneCommon::SetPulseAmplitude(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseLTZoneCommon* NPulseLTZoneCommon::New(void)
{
 return new NPulseLTZoneCommon;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseLTZoneCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseLTZoneCommon::ADefault(void)
{
 NLTZone::ADefault();
 vector<size_t> size;

 SetNumOutputs(4);
 for(int i=0;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));

 // Начальные значения всем параметрам
 AvgInterval=1;
 PulseAmplitude=1;
 PulseLength=0.001;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseLTZoneCommon::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPulseLTZoneCommon::AReset(void)
{
 NLTZone::AReset();
 // Сброс временных переменных
 PulseCounter=0;
 PrePotential=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 FillOutputData(0);

 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZoneCommon::ACalculate(void)
{

 return true;
}

/// Возвращает true если условие для генерации импульса выполнено
bool NPulseLTZoneCommon::CheckPulseOn(void)
{
 return false;
}

/// Возвращает true если условие для генерации имульса не выполнено
bool NPulseLTZoneCommon::CheckPulseOff(void)
{
 return false;
}
// --------------------------

}
#endif
