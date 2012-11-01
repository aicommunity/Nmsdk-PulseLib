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
#include "../BCL/NConnector.h"


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
bool NLTZone::SetThreshold(double value)
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

// --------------------------


// Методы NPulseLTZone
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZone::NPulseLTZone(void)
//: NADItem(name),
 : TimeConstant("TimeConstant",this,&NPulseLTZone::SetTimeConstant),
  PulseAmplitude("PulseAmplitude",this,&NPulseLTZone::SetPulseAmplitude),
  PulseLength("PulseLength",this),
  AvgInterval("AvgInterval",this),
  NeuralPotential("NeuralPotential",this),
  PrePotential("PrePotential",this),
  PulseCounter("PulseCounter",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this),
  PulseFlag("PulseFlag",this)
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
bool NPulseLTZone::SetTimeConstant(double value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NPulseLTZone::SetPulseAmplitude(double value)
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
bool NPulseLTZone::CheckComponentType(UEPtr<UAContainer> comp) const
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
 NLTZone::ADefault();
 vector<size_t> size;

 SetNumOutputs(4);
 size.resize(NumOutputs);
 for(size_t i=0;i<size.size();i++)
  size[i]=1;

 SetOutputDataSize(size);

 // Начальные значения всем параметрам
 TimeConstant=0.005;
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.00001;
 AvgInterval=1;

// NumInputs=2;

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
 // Сброс временных переменных
 NeuralPotential=0;
 PrePotential=0;
 PulseCounter=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 FillOutputData(0);

 return true;
}

// Выполняет расчет этого объекта
bool NPulseLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs/2.0;
 }

// double prev=PrePotential;
 PrePotential.v+=(NeuralPotential.v-PrePotential.v)/(TimeConstant.v*TimeStep);
// PrePotential=NeuralPotential.v;

// if(PulseCounter.v > 0)
//  --PulseCounter;

 if(PrePotential.v>=Threshold.v)
 {
  POutputData[0].Double[0]=PulseAmplitude.v;
  POutputData[1].Double[0]=PulseAmplitude.v;
  if(!PulseFlag)
   AvgFrequencyCounter->push_back(GetDoubleTime());
  PulseFlag=true;
 }
 else
 if(PrePotential.v<=0)
 {
  PulseFlag=false;
  POutputData[0].Double[0]=0;
  POutputData[1].Double[0]=PrePotential.v;
 }
 else
  POutputData[1].Double[0]=PrePotential.v;

 list<double>::iterator I,J,K;
 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();

 if(AvgFrequencyCounter->size()>1)
 {
  while(I != J)
  {
   if(GetDoubleTime()-*I>AvgInterval)// && AvgFrequencyCounter->size()>3)
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
   POutputData[2].Double[0]=frequency/interval;
  else
  if(interval==0 && frequency>2);
  else
   POutputData[2].Double[0]=0;
 }
 else
  POutputData[2].Double[0]=0;

 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();
 SetOutputDataSize(3,AvgFrequencyCounter->size(),true);
 for(int i=0;i<POutputData[3].GetSize();i++,++I)
  POutputData[3].Double[i]=*I;

 if(MainOwner)
  static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveOutputs.v+=static_cast<double>(GetNumAConnectors(0));

 return true;
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
bool NContinuesLTZone::SetTimeConstant(double value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NContinuesLTZone::SetPulseAmplitude(double value)
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
bool NContinuesLTZone::CheckComponentType(UEPtr<UAContainer> comp) const
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
 vector<size_t> size;

 SetNumOutputs(4);
 size.resize(NumOutputs);
 for(size_t i=0;i<size.size();i++)
  size[i]=1;

 SetOutputDataSize(size);

 // Начальные значения всем параметрам
 TimeConstant=0.005;
 PulseAmplitude=1;
 PulseLength=0.001;
 Threshold=0.0;
 AvgInterval=1;

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
 // Сброс временных переменных
 NeuralPotential=0;
 PrePotential=0;
 PulseCounter=0;
 AvgFrequencyCounter->clear();
 PulseFlag=false;
 FillOutputData(0);

 return true;
}

// Выполняет расчет этого объекта
bool NContinuesLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs/2.0;
 }

 PrePotential.v=tanh(NeuralPotential.v);

 if(PrePotential.v>=Threshold.v)
 {
  POutputData[0].Double[0]=PrePotential.v;
  POutputData[1].Double[0]=PrePotential.v;
  PulseFlag=true;
 }
 else
 if(PrePotential.v<=0)
 {
  PulseFlag=false;
  POutputData[0].Double[0]=0;
  POutputData[1].Double[0]=PrePotential.v;
 }
 else
  POutputData[1].Double[0]=PrePotential.v;

  POutputData[2].Double[0]=POutputData[0].Double[0];

 if(MainOwner)
  static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveOutputs.v+=static_cast<double>(GetNumAConnectors(0));

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
bool NPulseSimpleLTZone::CheckComponentType(UEPtr<UAContainer> comp) const
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
 generator.SetActivity(true);
 generator.Reset();
 return NPulseLTZone::AReset();
}

// Выполняет расчет этого объекта
bool NPulseSimpleLTZone::ACalculate(void)
{
 // расчет на шаге
 NeuralPotential=0;

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs;
 }

 generator.Amplitude=PulseAmplitude;
 if(NeuralPotential.v>200)
  NeuralPotential.v=200;
 if(NeuralPotential.v>0)
 {
  if(fabs(generator.Frequency.v-NeuralPotential.v)>0.001)
  {
   generator.Frequency=NeuralPotential.v;
 //  generator.PulseCounter.v=-TimeStep/generator.Frequency.v;
   generator.Reset();
  }
 }
 else
  generator.Frequency=0;
 generator.Calculate();

 POutputData[0].Double[0]=generator.GetOutputData(0).Double[0];
 POutputData[1].Double[0]=generator.GetOutputData(0).Double[0];
 POutputData[2].Double[0]=NeuralPotential.v;

 POutputData[3]=generator.GetOutputData(3);

 if(MainOwner)
  static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveOutputs.v+=static_cast<double>(GetNumAConnectors(0));

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
bool NContinuesSimpleLTZone::CheckComponentType(UEPtr<UAContainer> comp) const
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

// NeuralPotential=GetFullSumInput();
 if(NumInputs>0)
 {
  size_t inpsize;
  for(int i=0;i<NumInputs;i++)
  {
   if((inpsize=GetInputDataSize(i)) >0)
   {
	double *data=&(GetInputData(i)->Double[0]);
	for(size_t j=0;j<inpsize;j++,++data)
	 NeuralPotential.v+=*data;
   }
  }
  NeuralPotential.v/=NumInputs;
 }

 POutputData[0].Double[0]=NeuralPotential.v;
 POutputData[1].Double[0]=NeuralPotential.v;
 POutputData[2].Double[0]=NeuralPotential.v;

 if(MainOwner)
  static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveOutputs.v+=static_cast<double>(GetNumAConnectors(0));

 return true;
}
// --------------------------

}
#endif
