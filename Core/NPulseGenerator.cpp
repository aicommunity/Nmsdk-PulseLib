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

#ifndef NPULSE_GENERATOR_CPP
#define NPULSE_GENERATOR_CPP

#include <cstdlib>
#include <cstdio>
#include "NPulseGenerator.h"
//#include "NPulseChannel.h"



namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGenerator::NPulseGenerator(void)
//: NSource(name),
 : Frequency("Frequency",this,&NPulseGenerator::SetFrequency),
  PulseLength("PulseLength",this,&NPulseGenerator::SetPulseLength),
  Amplitude("Amplitude",this,&NPulseGenerator::SetAmplitude),
  Delay("Delay",this,&NPulseGenerator::SetDelay),
  FrequencyDeviation("FrequencyDeviation",this,&NPulseGenerator::SetFrequencyDeviation),
  AvgInterval("AvgInterval",this),

  OutputPotential("OutputPotential",this),
  OutputFrequency("OutputFrequency",this),
  OutputPulseTimes("OutputPulseTimes",this),

  PulseCounter("PulseCounter",this),
  RandomFrequency("RandomFrequency",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this)
{
 OldFrequency=0;
 ResetTime=0;
}

NPulseGenerator::~NPulseGenerator(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает частоту импульсов
bool NPulseGenerator::SetFrequency(const double &value)
{
 if(value <0)
  return false;
/*
 if(PulseCounter.v<=0 && value>Frequency.v)
 {
  Frequency.v=value;
  return Reset();
 }
  */
// return Reset();
 return true;
}

// Устанавливает длительность импульса
bool NPulseGenerator::SetPulseLength(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NPulseGenerator::SetAmplitude(const double &value)
{
 return true;
}

// Устанавливает задержку запуска генератора
bool NPulseGenerator::SetDelay(const double &value)
{
 return true;
}

// Устанавливает отклонение частоты
// (Задает диапазон, не является стандартным отклонением!)
bool NPulseGenerator::SetFrequencyDeviation(const double &value)
{
 if(value<0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseGenerator* NPulseGenerator::New(void)
{
 return new NPulseGenerator;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseGenerator::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NPulseChannel*>(comp))
//  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseGenerator::ADefault(void)
{
// SetNumOutputs(4);
// SetOutputDataSize(0,MMatrixSize(1,1));
// SetOutputDataSize(1,MMatrixSize(1,1));
 Frequency=0.0;
 PulseLength=0.001;
 Amplitude=1.0;
 Delay=0.0;
 FrequencyDeviation=0;
 AvgInterval=5;
 PulseCounter=0;

 Output.Assign(1,1,0.0);
 OutputPotential.Assign(1,1,0.0);
 OutputFrequency.Assign(1,1,0.0);
 OutputPulseTimes.Assign(1,1,0.0);

 return NSource::ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseGenerator::ABuild(void)
{
 AvgFrequencyCounter->clear();
 return true;
}

// Сброс процесса счета.
bool NPulseGenerator::AReset(void)
{
 time_t tm;
 time(&tm);
 srand(static_cast<unsigned>(tm));

// PulseCounter=static_cast<RDK::UTime>(PulseLength.v*TimeStep);
// if(Frequency.v > 0 && PulseCounter.v<-int(TimeStep/Frequency.v))
//  PulseCounter.v=-int(TimeStep/Frequency.v);
// else
  PulseCounter=0;
 RandomFrequency=Frequency;
 AvgFrequencyCounter->clear();

 OldFrequency=Frequency;
 Output.ToZero();
 OutputPotential.ToZero();
 OutputFrequency.ToZero();
 OutputPulseTimes.ToZero();

 if(Environment)
  ResetTime = Environment->GetTime().GetDoubleTime();
 else
  ResetTime = 0.0;

 return NSource::AReset();
}

// Выполняет расчет этого объекта
bool NPulseGenerator::ACalculate(void)
{
 if(Frequency<1e-8 || TimeStep/Frequency<1)
 {
  Output.ToZero();
  OutputPotential.ToZero();
  OutputFrequency.ToZero();
  OutputPulseTimes.ToZero();
  AvgFrequencyCounter->clear();
  OldFrequency=Frequency;
  PulseCounter = 0;
  return true;
 }

 if(Environment->GetTime().GetDoubleTime() - ResetTime < Delay)
  return true;

 if(OldFrequency != Frequency.v)
 {
  if(PulseCounter < 0 && PulseCounter < static_cast<int>(-int(TimeStep/Frequency)+PulseLength*TimeStep))
   PulseCounter = static_cast<int>(-int(TimeStep/Frequency));

  OldFrequency=Frequency;
 }

 if(FrequencyDeviation == 0)
 {
  if(PulseCounter>0) // Если импульс идет
  {
   --PulseCounter;
   if(PulseCounter <= 0) // Выключаем импульс и включаем ожидание
   {
	PulseCounter=static_cast<int>(-int(TimeStep/Frequency)+PulseLength*TimeStep);
	Output.ToZero();
    OutputPotential.ToZero();
   }
  }
  else
  {
   ++PulseCounter;
   if(PulseCounter >= 0) // Включаем импульс
   {
	PulseCounter=static_cast<RDK::UTime>(PulseLength*TimeStep);
	Output.Assign(1,1,Amplitude);
	OutputPotential.Assign(1,1,Amplitude);
	AvgFrequencyCounter->push_back(Environment->GetTime().GetDoubleTime());
   }
  }
  OutputFrequency.Assign(1,1,Frequency);
 }
 else
 {
  if(PulseCounter>0) // Если импульс идет
  {
   --PulseCounter;
   if(PulseCounter <= 0) // Выключаем импульс и включаем ожидание
   {
    RandomFrequency=double(rand()*FrequencyDeviation*2.0)/double(RAND_MAX)+
                           Frequency-FrequencyDeviation;
    if(RandomFrequency.v>0)
     PulseCounter=static_cast<int>(-int(TimeStep/RandomFrequency)+PulseLength*TimeStep);
    else
     PulseCounter=0;
     Output->ToZero();
     OutputPotential->ToZero();
   }
  }
  else
  {
   ++PulseCounter;
   if(PulseCounter >= 0) // Включаем импульс
   {
    PulseCounter=static_cast<RDK::UTime>(PulseLength*TimeStep);
    Output.Assign(1,1,Amplitude);
    OutputPotential.Assign(1,1,Amplitude);
    AvgFrequencyCounter->push_back(Environment->GetTime().GetDoubleTime());
   }
  }
  OutputFrequency.Assign(1,1,RandomFrequency);
 }

 list<double>::iterator I,J,K;
 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();

 if(AvgFrequencyCounter->size()>1)
 {
  while(I != J)
  {
   double diff=Environment->GetTime().GetDoubleTime()-*I;
   if(diff>AvgInterval)// && AvgFrequencyCounter->size()>3)
   {
    K=I;
    ++I;
    AvgFrequencyCounter->erase(K);
   }
   else
    ++I;
  }
 }

 I=AvgFrequencyCounter->begin();
 J=AvgFrequencyCounter->end();
 OutputPulseTimes.Resize(1,int(AvgFrequencyCounter->size()));
 for(int i=0;i<OutputPulseTimes.GetCols();i++,++I)
  OutputPulseTimes(0,i)=*I;

 return true;
}
// --------------------------


// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorTransit::NPulseGeneratorTransit(void)
//: NSource(name),
 : UseTransitSignal("UseTransitSignal",this,&NPulseGeneratorTransit::SetUseTransitSignal),
   UsePatternOutput("UsePatternOutput",this,&NPulseGeneratorTransit::SetUsePatternOutput),
   PatternDuration("PatternDuration",this,&NPulseGeneratorTransit::SetPatternDuration),
   PatternFrequency("PatternFrequency",this,&NPulseGeneratorTransit::SetPatternFrequency),
   Input("Input",this)
{

}

NPulseGeneratorTransit::~NPulseGeneratorTransit(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает необходимость транзита сигнала от внешнего источника
bool NPulseGeneratorTransit::SetUseTransitSignal(const bool &value)
{
 return true;
}
// --------------------------
// Устанавливает необходимость увеличения частоты сигнала (на определенный период времени) по внешнему импульсу
bool NPulseGeneratorTransit::SetUsePatternOutput(const bool &value)
{
 return true;
}
// --------------------------
// Устанавливает необходимость увеличения частоты сигнала (на определенный период времени) по внешнему импульсу
bool NPulseGeneratorTransit::SetPatternDuration(const double &value)
{
 return true;
}
// --------------------------
// Устанавливает необходимость увеличения частоты сигнала (на определенный период времени) по внешнему импульсу
bool NPulseGeneratorTransit::SetPatternFrequency(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseGeneratorTransit* NPulseGeneratorTransit::New(void)
{
 return new NPulseGeneratorTransit;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------

// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseGeneratorTransit::ADefault(void)
{
 UseTransitSignal = false;
 UsePatternOutput = false;
 IsInPatternMode  = false;
 TheSamePulse = false;
 PatternFrequency = 0; // (Гц)
 PatternDuration = 0; // (Гц)

 Input->Assign(1,1,0.0);

 return NPulseGenerator::ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseGeneratorTransit::ABuild(void)
{
 return NPulseGenerator::ABuild();
}

// Сброс процесса счета.
bool NPulseGeneratorTransit::AReset(void)
{
 // Останавливаем генерацию с повышенной частотой,
 // но сам режим остаётся включенным (UsePatternOutput)
 IsInPatternMode  = false;
 TheSamePulse = false;
 if (UsePatternOutput)
  Frequency = 0;

 return NPulseGenerator::AReset();
}

// Выполняет расчет этого объекта
bool NPulseGeneratorTransit::ACalculate(void)
{
 if(UseTransitSignal)
 {
  *Output = *Input;
  return true;
 }

 //режим работы с повышенной частотой в течение заданного периода времени
 else if(UsePatternOutput)
 {
  // Запускающий сигнал ещё не пришёл, повышенной генерации не происходит
  if (IsInPatternMode == false)
  {
   if ((Input()(0,0) >= 0.01) && (!TheSamePulse))  // Пришёл (первый) запускающий импульс
   {
    TheSamePulse = true;  // Ставим метку: импульс начался
    PatternStartTime = Environment->GetTime().GetDoubleTime();
    Frequency = PatternFrequency;
    IsInPatternMode = true;  // Переходим в режим генерации с повышенной частотой
   }
   else if ((Input()(0,0) <= 0.01) && (TheSamePulse))  // Сигнал на входе упал
   {
    TheSamePulse = false;  // Ставим метку: импульс закончился
   }
  }
  // Происходит генерация с повышенной частотой
  else
  {
   // 1: Прекращение генерации с повышенной частотой, так как вышло указанное в PatternDuration время
   if ((PatternDuration) && (Environment->GetTime().GetDoubleTime() - PatternStartTime >= PatternDuration))
   {
    Frequency = 0;
    IsInPatternMode = false;
   }
   // или 2: прекращение, так как пришёл (повторный) останавливающий сигнал
   if ((Input()(0,0) >= 0.01) && (!TheSamePulse))
   {
    Frequency = 0;
    IsInPatternMode = false;
    TheSamePulse = true;  // Ставим метку: импульс начался
   }
   // Сигнал на входе упал
   if ((Input()(0,0) <= 0.01) && (TheSamePulse))
   {
    TheSamePulse = false;  // Ставим метку: импульс закончился
   }
  }
 }

 return NPulseGenerator::ACalculate();
}
// --------------------------

}
#endif
