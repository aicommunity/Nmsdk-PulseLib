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

// Методы NPulseLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZone::NPulseLTZone(void)
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
   NeuralPotential("NeuralPotential",this)
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

// Устанавливает амплитуду импульсов
bool NPulseLTZone::SetPulseAmplitude(const double &value)
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
 NPulseLTZoneCommon::ADefault();

 // Начальные значения всем параметрам
 TimeConstant=0.005;
 Threshold=0.00001;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseLTZone::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPulseLTZone::AReset(void)
{
 NPulseLTZoneCommon::AReset();
 // Сброс временных переменных
 NeuralPotential=0;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(InputChannels->size()>0)
 {
  size_t inpsize;
  for(size_t i=0;i<InputChannels->size();i++)
  {
   if((inpsize=InputChannels[i]->GetCols()) >0)
   {
	double *data=InputChannels[i]->Data;
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=InputChannels->size()/2.0;
 }

 PrePotential.v+=(NeuralPotential.v-PrePotential.v)/(TimeConstant.v*TimeStep);

 double current_time=GetTime().GetDoubleTime();
 if(CheckPulseOn())
 {
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
  OutputPotential(0,0)=PrePotential.v;
 }
 else
  OutputPotential(0,0)=PrePotential.v;

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
bool NPulseLTZone::CheckPulseOn(void)
{
 return PrePotential.v>=Threshold.v;
}

/// Возвращает true если условие для генерации имульса не выполнено
bool NPulseLTZone::CheckPulseOff(void)
{
 return PrePotential.v<=0;
}
// --------------------------

// Методы NContinuesLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesLTZone::NContinuesLTZone(void)
//: NADItem(name),
 : TimeConstant("TimeConstant",this,&NContinuesLTZone::SetTimeConstant),
  PulseAmplitude("PulseAmplitude",this,&NContinuesLTZone::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  OutputPotential("OutputPotential",this),
  NeuralPotential("NeuralPotential",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
{
}

NContinuesLTZone::~NContinuesLTZone(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает значение постоянной времени
bool NContinuesLTZone::SetTimeConstant(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NContinuesLTZone::SetPulseAmplitude(const double &value)
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
 TimeConstant=0.005;
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.0;
 AvgInterval=1;

 OutputPotential.Assign(1,1,0.0);

// NumInputs=2;

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
bool NContinuesLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(InputChannels->size()>0)
 {
  size_t inpsize;
  for(size_t i=0;i<InputChannels->size();i++)
  {
   if((inpsize=InputChannels[i]->GetCols()) >0)
   {
	double *data=InputChannels[i]->Data;
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=InputChannels->size()/2.0;
 }

 PrePotential.v=tanh(NeuralPotential.v);

 if(PrePotential.v>=Threshold.v)
 {
  Output(0,0)=PrePotential.v;
  OutputPotential(0,0)=PrePotential.v;
  PulseFlag=true;
 }
 else
 if(PrePotential.v<=0)
 {
  PulseFlag=false;
  Output(0,0)=0;
  OutputPotential(0,0)=PrePotential.v;
 }
 else
  OutputPotential(0,0)=PrePotential.v;

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
{
}

NPulseSimpleLTZone::~NPulseSimpleLTZone(void)
{
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
// if(dynamic_cast<const NConnector*>(comp))
//  return true;

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
 generator.Default();

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseSimpleLTZone::ABuild(void)
{
 generator.Build();
 return NPulseLTZone::ABuild();
}

// Сброс процесса счета.
bool NPulseSimpleLTZone::AReset(void)
{
 generator.SetEnvironment(GetEnvironment());
 generator.SetActivity(true);
 generator.Reset();
 return NPulseLTZone::AReset();
}

// Выполняет расчет этого объекта
bool NPulseSimpleLTZone::ACalculate(void)
{
 generator.SetEnvironment(GetEnvironment());
 // расчет на шаге
 NeuralPotential=0;

 if(InputChannels.IsConnected())
 {
  int inpsize(0);
  for(int i=0;i<int(InputChannels->size());i++)
  {
   if(InputChannels[i])
   {
	if((inpsize=InputChannels[i]->GetSize(1)) >0)
	{
	 double *data=&(InputChannels[i]->Double[0]);
	 for(int j=0;j<inpsize;j++,++data)
	  NeuralPotential.v+=*data;
	}
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=InputChannels->size();
 }
 /*else
 if(NumInputs>0)
 {
  int inpsize(0);
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)[1]) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=NumInputs;
 }        */

 generator.Amplitude=PulseAmplitude;
 if(NeuralPotential.v>200)
  NeuralPotential.v=200;
 if(NeuralPotential.v>0)
 {
  if(fabs(generator.Frequency.v-NeuralPotential.v)>0.001)
  {
   generator.Frequency=NeuralPotential.v;
//   generator.Reset();
  }
 }
 else
  generator.Frequency=0;

 generator.AvgInterval=AvgInterval;
 generator.Calculate();

 Output(0,0)=generator.Output(0,0);
 OutputPotential(0,0)=generator.OutputPotential(0,0);
 OutputFrequency(0,0)=NeuralPotential.v;

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
// Методы управления общедоступными свойствами
// --------------------------
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
bool NContinuesSimpleLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

 if(InputChannels.IsConnected())
 {
  int inpsize(0);
  for(size_t i=0;i<InputChannels->size();i++)
  {
   if(InputChannels[i])
   {
	if((inpsize=InputChannels[i]->GetSize(1)) >0)
	{
	 double *data=&(InputChannels[i]->Double[0]);
	 for(int j=0;j<inpsize;j++,++data)
	  NeuralPotential.v+=*data;
	}
   }
  }
  if(UseAveragePotential)
   NeuralPotential.v/=InputChannels->size();
 }

 Output(0,0)=NeuralPotential.v;
 OutputPotential(0,0)=NeuralPotential.v;

 if(MainOwner)
  dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner)->NumActiveOutputs.v+=CachedNumAConnectors;//static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

}
#endif
