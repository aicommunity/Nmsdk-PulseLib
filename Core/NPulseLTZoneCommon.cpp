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

namespace NMSDK {

// Методы NLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLTZone::NLTZone(void)
 : Threshold("Threshold",this,&NLTZone::SetThreshold),
   UseAveragePotential("UseAveragePotential",this),
   Output("Output",this),
   Inputs("Inputs",this)
{
 Neuron=0;
 CachedNumAConnectors=0;
// AddAlias("DataInput0","Inputs");
// AddAlias("DataInput1","Inputs");
// AddAlias("DataInput2","Inputs");
// AddAlias("DataInput3","Inputs");
// AddAlias("DataInput4","Inputs");
// AddAlias("DataInput5","Inputs");
// AddAlias("DataInput6","Inputs");
// AddAlias("DataInput7","Inputs");
// AddAlias("DataInput8","Inputs");
// AddAlias("DataInput9","Inputs");
// AddAlias("DataInput10","Inputs");
// AddAlias("DataInput11","Inputs");
// AddAlias("DataInput12","Inputs");
// AddAlias("DataInput13","Inputs");
// AddAlias("DataInput14","Inputs");
// AddAlias("DataInput15","Inputs");
// AddAlias("DataInput16","Inputs");
// AddAlias("DataInput17","Inputs");
// AddAlias("DataInput18","Inputs");
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
 UseAveragePotential=true;
 Output.Assign(1,1,0.0);

 return true;
}

// Сброс процесса счета.
bool NLTZone::AReset(void)
{
 CachedNumAConnectors=static_cast<int>(GetNumAConnectors(0));
 Output.ToZero();
 return true;
}
// --------------------------


// Методы NPulseLTZoneCommon
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZoneCommon::NPulseLTZoneCommon(void)
 :
  NumChannelsInGroup("NumChannelsInGroup",this),
  PulseAmplitude("PulseAmplitude",this,&NPulseLTZoneCommon::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  OutputPotential("OutputPotential",this),
  OutputFrequency("OutputFrequency",this),
  OutputPulseTimes("OutputPulseTimes",this),
  NeuralPotential("NeuralPotential",this),
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

 // Начальные значения всем параметрам
 NumChannelsInGroup=1;
 AvgInterval=1;
 PulseAmplitude=1;
 PulseLength=0.001;

 OutputPotential.Assign(1,1,0.0);
 OutputFrequency.Assign(1,1,0.0);
 OutputPulseTimes.Assign(1,1,0.0);

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
 NeuralPotential=0;

 PrePotential=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 OutputPotential.ToZero();
 OutputFrequency.ToZero();
 OutputPulseTimes.ToZero();

 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZoneCommon::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

 if(Inputs->size()>0)
 {
  size_t inpsize;
  for(size_t i=0;i<Inputs->size();i++)
  {
   if((inpsize=Inputs[i]->GetCols()) >0)
   {
	double *data=Inputs[i]->Data;
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=Inputs->size();
 }

 if(NumChannelsInGroup>0)
  NeuralPotential.v/=NumChannelsInGroup.v; // Делим пополам, чтобы учесть, что у нас по 2 ионных механизма на участок мембраны

 return ACalculate2();
}

bool NPulseLTZoneCommon::ACalculate2(void)
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
