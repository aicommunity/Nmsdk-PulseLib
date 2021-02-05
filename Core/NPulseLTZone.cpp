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

#ifndef NPULSE_LTZONE_CPP
#define NPULSE_LTZONE_CPP

#include <numeric>
#include "NPulseLTZone.h"
#include "NPulseChannel.h"
#include "NPulseNeuron.h"

namespace NMSDK {

// Методы NPulseLTZoneThreshold
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZoneThreshold::NPulseLTZoneThreshold(void)
{
}

NPulseLTZoneThreshold::~NPulseLTZoneThreshold(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseLTZoneThreshold* NPulseLTZoneThreshold::New(void)
{
 return new NPulseLTZoneThreshold;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseLTZoneThreshold::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseLTZoneThreshold::ADefault(void)
{
 NPulseLTZoneCommon::ADefault();

 // Начальные значения всем параметрам
 Threshold=0.00001;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseLTZoneThreshold::ABuild(void)
{
 NPulseLTZoneCommon::ABuild();
 return true;
}

// Сброс процесса счета.
bool NPulseLTZoneThreshold::AReset(void)
{
 NPulseLTZoneCommon::AReset();
 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZoneThreshold::ACalculate2(void)
{
 double current_time=GetTime().GetDoubleTime();
 if(CheckPulseOn())
 {
  // ----> ОТЛАДКА
  //double eee = Potential.v;
  // < ----

  Output(0,0)=PulseAmplitude.v;
  OutputPotential(0,0)=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(current_time);
  PulseFlag=true;
 }
 else
 if(CheckPulseOff())
 {
  PulseFlag=false;
  Output(0,0)=0;
  OutputPotential(0,0)=Potential.v;
 }
 else
  OutputPotential(0,0)=Potential.v;

 list<double>::iterator I,J,K;
 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();

 if(AvgFrequencyCounter->size()>1)
 {
  while(I != J)
  {
   if(current_time-*I>AvgInterval)// && AvgFrequencyCounter->size()>3)
   {
	K=I;
	++I;
	AvgFrequencyCounter->erase(K);
   }
   else
	++I;
  }

  double frequency=0;
  frequency=static_cast<double>(AvgFrequencyCounter->size());//accumulate(AvgFrequencyCounter->begin(),AvgFrequencyCounter->end(),frequency);
  double interval=AvgFrequencyCounter->back()-AvgFrequencyCounter->front();
  if(interval>0 && frequency>2)
   OutputFrequency(0,0)=frequency/interval;
  else
  if(interval==0 && frequency>2);
  else
   OutputFrequency(0,0)=0;
 }
 else
  OutputFrequency(0,0)=0;

 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();
 OutputPulseTimes->Resize(1,AvgFrequencyCounter->size());
 for(size_t i=0;i<AvgFrequencyCounter->size();i++,++I)
  OutputPulseTimes(0,i)=*I;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}

/// Возвращает true если условие для генерации импульса выполнено
bool NPulseLTZoneThreshold::CheckPulseOn(void)
{
 return Potential.v>=Threshold.v;
}

/// Возвращает true если условие для генерации имульса не выполнено
bool NPulseLTZoneThreshold::CheckPulseOff(void)
{
 return Potential.v<=0;
}
// --------------------------


// Методы NPulseLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZone::NPulseLTZone(void)
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
  UseLTZIntegtation("UseLTZIntegtation",this,&NPulseLTZone::SetLTZIntegtation),
  UseSpikeStabilizer("UseSpikeStabilizer",this, &NPulseLTZone::SetUseSpikeStabilizer)
{
}

NPulseLTZone::~NPulseLTZone(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает значение постоянной времени
bool NPulseLTZone::SetTimeConstant(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}
// Устанавливает признак необходимости интеграции в LTZ
bool NPulseLTZone::SetLTZIntegtation(const bool &value)
{
 return true;
}

// Флаг включения стабилизации длительности импульса
bool NPulseLTZone::SetUseSpikeStabilizer(const bool &value)
{
 TimeConstant=0.00144; // Force change time constant
 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseLTZone* NPulseLTZone::New(void)
{
 return new NPulseLTZone;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseLTZone::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NConnector*>(comp))
//  return true;

 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseLTZone::ADefault(void)
{
 NPulseLTZoneThreshold::ADefault();

 // Начальные значения всем параметрам
 NumChannelsInGroup=2;
 TimeConstant=0.00144;//0.005;
 Threshold=0.00001;
 UseLTZIntegtation = true;
 UseSpikeStabilizer=true;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseLTZone::ABuild(void)
{
 NPulseLTZoneThreshold::ABuild();
 return true;
}

// Сброс процесса счета.
bool NPulseLTZone::AReset(void)
{
 NPulseLTZoneThreshold::AReset();
 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZone::ACalculate2(void)
{
 if(UseLTZIntegtation)
 {
  double input(0.0);
  if(UseSpikeStabilizer.v)
  {
   input=(Output(0,0)>0)?-Threshold.v:NeuralPotential.v;
  }
  PrePotential.v+=(input-PrePotential.v)/(TimeConstant.v*TimeStep);
 }
 else
  PrePotential.v = NeuralPotential.v;

 Potential.v=PrePotential.v;
 return NPulseLTZoneThreshold::ACalculate2();
}
// --------------------------

// Методы NContinuesLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesLTZone::NContinuesLTZone(void)
{
}

NContinuesLTZone::~NContinuesLTZone(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NContinuesLTZone* NContinuesLTZone::New(void)
{
 return new NContinuesLTZone;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NContinuesLTZone::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NConnector*>(comp))
//  return true;

 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NContinuesLTZone::ADefault(void)
{
 NLTZone::ADefault();

 // Начальные значения всем параметрам
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.0;
 AvgInterval=1;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NContinuesLTZone::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NContinuesLTZone::AReset(void)
{
 NLTZone::AReset();
 // Сброс временных переменных
 NeuralPotential=0;
 PrePotential=0;
 PulseCounter=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;

 OutputPotential.ToZero();

 return true;
}

// Выполняет расчет этого объекта
bool NContinuesLTZone::ACalculate2(void)
{
 Potential.v=tanh(NeuralPotential.v);

 if(Potential.v>=Threshold.v)
 {
  Output(0,0)=Potential.v;
  OutputPotential(0,0)=Potential.v;
  PulseFlag=true;
 }
 else
 if(Potential.v<=0)
 {
  PulseFlag=false;
  Output(0,0)=0;
  OutputPotential(0,0)=Potential.v;
 }
 else
  OutputPotential(0,0)=Potential.v;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

// Методы NPulseSimpleLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSimpleLTZone::NPulseSimpleLTZone(void)
: MaxFrequency("MaxFrequency",this)
{
 AddStaticComponent("NPGenerator","PGenerator",&generator);
}

NPulseSimpleLTZone::~NPulseSimpleLTZone(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseSimpleLTZone* NPulseSimpleLTZone::New(void)
{
 return new NPulseSimpleLTZone;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseSimpleLTZone::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseGenerator>(comp))
  return true;

 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseSimpleLTZone::ADefault(void)
{
 NPulseLTZone::ADefault();

 NumChannelsInGroup=1;
 MaxFrequency=100;
 //generator.Default();

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseSimpleLTZone::ABuild(void)
{
 //generator.Build();
 return NPulseLTZone::ABuild();
}

// Сброс процесса счета.
bool NPulseSimpleLTZone::AReset(void)
{
 //generator.SetEnvironment(GetEnvironment());
 //generator.SetActivity(true);
 //generator.Reset();
 return NPulseLTZone::AReset();
}

// Выполняет расчет этого объекта
bool NPulseSimpleLTZone::ACalculate2(void)
{
 generator.Amplitude=PulseAmplitude;
 if(Potential.v>MaxFrequency)
  Potential.v=MaxFrequency;
 if(Potential.v>0)
 {
  if(fabs(generator.Frequency.v-Potential.v)>0.001)
  {
   generator.Frequency=Potential.v;
//   generator.Reset();
  }
 }
 else
  generator.Frequency=0;

 generator.AvgInterval=AvgInterval;
 generator.Calculate();

 Output(0,0)=generator.Output(0,0);
 OutputPotential(0,0)=generator.OutputPotential(0,0);
 OutputFrequency(0,0)=Potential.v;

 OutputPulseTimes(0,0)=generator.OutputPulseTimes(0,0);

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

// Методы NContinuesSimpleLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesSimpleLTZone::NContinuesSimpleLTZone(void)
{
}

NContinuesSimpleLTZone::~NContinuesSimpleLTZone(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NContinuesSimpleLTZone* NContinuesSimpleLTZone::New(void)
{
 return new NContinuesSimpleLTZone;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NContinuesSimpleLTZone::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NConnector*>(comp))
//  return true;

 return false;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NContinuesSimpleLTZone::ADefault(void)
{
 return NContinuesLTZone::ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NContinuesSimpleLTZone::ABuild(void)
{
 return NContinuesLTZone::ABuild();
}

// Сброс процесса счета.
bool NContinuesSimpleLTZone::AReset(void)
{
 return NContinuesLTZone::AReset();
}

// Выполняет расчет этого объекта
bool NContinuesSimpleLTZone::ACalculate2(void)
{
 Output(0,0)=Potential.v;
 OutputPotential(0,0)=Potential.v;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

}
#endif
