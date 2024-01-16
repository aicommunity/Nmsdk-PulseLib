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

#ifndef NPULSE_GENERATOR_MULTI_CPP
#define NPULSE_GENERATOR_MULTI_CPP

#include <cstdlib>
#include <cstdio>
#include "NPulseGeneratorMulti.h"
//#include "NPulseChannel.h"



namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorMulti::NPulseGeneratorMulti(void)
//: NSource(name),
 : Frequency("Frequency",this,&NPulseGeneratorMulti::SetFrequency),
  PulseLength("PulseLength",this,&NPulseGeneratorMulti::SetPulseLength),
  Amplitude("Amplitude",this,&NPulseGeneratorMulti::SetAmplitude),
  PulseCount("PulseCount",this,&NPulseGeneratorMulti::SetPulseCount),
  Delays("Delays",this,&NPulseGeneratorMulti::SetDelays),
  FrequencyDeviation("FrequencyDeviation",this,&NPulseGeneratorMulti::SetFrequencyDeviation),
  AvgInterval("AvgInterval",this),

  OutputPotential("OutputPotential",this),
  OutputFrequency("OutputFrequency",this),
  OutputPulseTimes("OutputPulseTimes",this),

  PulseCounters("PulseCounters",this),
  RandomFrequency("RandomFrequency",this),
  AvgFrequencyCounter("AvgFrequencyCounter",this)
{
 OldFrequency=0;
 ResetTime=0;
}

NPulseGeneratorMulti::~NPulseGeneratorMulti(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает частоту импульсов
bool NPulseGeneratorMulti::SetFrequency(const double &value)
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
bool NPulseGeneratorMulti::SetPulseLength(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NPulseGeneratorMulti::SetAmplitude(const double &value)
{
 return true;
}

// Устанавливает число импульсов
bool NPulseGeneratorMulti::SetPulseCount(const int &value)
{
    if(value<0)
        return false;

    Delays.Resize(1,value);
    return true;
}

// Устанавливает задержку запуска генератора
bool NPulseGeneratorMulti::SetDelays(const MDMatrix<double> &value)
{
 if(value.GetCols()<=0)
    return false;
 if(value.GetRows()!=1)
     return false;

 //int c = value.GetCols();
 //int r = value.GetRows();
 int num_pulses = value.GetCols();
 PulseCounters.resize(num_pulses);
 return true;
}

// Устанавливает отклонение частоты
// (Задает диапазон, не является стандартным отклонением!)
bool NPulseGeneratorMulti::SetFrequencyDeviation(const double &value)
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
NPulseGeneratorMulti* NPulseGeneratorMulti::New(void)
{
 return new NPulseGeneratorMulti;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseGeneratorMulti::CheckComponentType(UEPtr<UContainer> comp) const
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
bool NPulseGeneratorMulti::ADefault(void)
{
// SetNumOutputs(4);
// SetOutputDataSize(0,MMatrixSize(1,1));
// SetOutputDataSize(1,MMatrixSize(1,1));
 Frequency=0.0;
 PulseLength=0.001;
 Amplitude=1.0;
 //Проверить, надо ли это
 Delays.Assign(1,1,0.0);
 FrequencyDeviation=0;
 AvgInterval=5;
 PulseCounters.resize(0);

 //PulseCount=0;

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
bool NPulseGeneratorMulti::ABuild(void)
{
 AvgFrequencyCounter->clear();

 int num_pulses = Delays.GetCols();
 pulses.resize(num_pulses, false);
 PulseCounters.resize(num_pulses, 0);
 return true;
}

// Сброс процесса счета.
bool NPulseGeneratorMulti::AReset(void)
{
 time_t tm;
 time(&tm);
 srand(static_cast<unsigned>(tm));

// PulseCounter=static_cast<RDK::UTime>(PulseLength.v*TimeStep);
// if(Frequency.v > 0 && PulseCounter.v<-int(TimeStep/Frequency.v))
//  PulseCounter.v=-int(TimeStep/Frequency.v);
// else

 pulses.resize(PulseCounters.size(), false);
 for(size_t i=0; i<PulseCounters.size();i++)
 {
     PulseCounters[i]=0;
     pulses[i]=false;
 }


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
bool NPulseGeneratorMulti::ACalculate(void)
{
 if(Frequency<1e-8 || TimeStep/Frequency<1)
 {
  Output.ToZero();
  OutputPotential.ToZero();
  OutputFrequency.ToZero();
  OutputPulseTimes.ToZero();
  AvgFrequencyCounter->clear();
  OldFrequency=Frequency;

  pulses.resize(PulseCounters.size(), false);
  for(size_t i=0; i<PulseCounters.size();i++)
  {
      PulseCounters[i]=0;
      pulses[i]=false;
  }


  return true;
 }


 //Это теперь переезжает в обработку соответствующих импульсов, так как Delay по логике у них разный
 //if(Environment->GetTime().GetDoubleTime() - ResetTime < Delay)
 // return true;

 if(OldFrequency != Frequency.v)
 {
    for(int i=0; i<int(PulseCounters.size());i++)
    {
        double Delay = Delays(0,i);
        if(Environment->GetTime().GetDoubleTime() - ResetTime < Delay)
            continue;

        int &PulseCounter = PulseCounters[i];
        if(PulseCounter < 0 && PulseCounter < static_cast<int>(-int(TimeStep/Frequency)+PulseLength*TimeStep))
         PulseCounter = static_cast<int>(-int(TimeStep/Frequency));
    }
    OldFrequency=Frequency;
 }

 if(FrequencyDeviation == 0)
 {
     //bool output_active = false;
     for(int i=0; i<int(PulseCounters.size());i++)
     {
      double Delay = Delays(0,i);
      if(Environment->GetTime().GetDoubleTime() - ResetTime < Delay)
       continue;

      int &PulseCounter = PulseCounters[i];
      if(PulseCounter>0) // Если импульс идет
      {
       --PulseCounter;
       if(PulseCounter <= 0) // Выключаем импульс и включаем ожидание
       {
        PulseCounter=static_cast<int>(-int(TimeStep/Frequency)+PulseLength*TimeStep);
        pulses[i] = false;
//        Output.ToZero();
//        OutputPotential.ToZero();
       }
      }
      else
      {
       ++PulseCounter;
       if(PulseCounter >= 0) // Включаем импульс
       {
        PulseCounter=static_cast<RDK::UTime>(PulseLength*TimeStep);
        pulses[i] = true;
//        Output.Assign(1,1,Amplitude);
//        OutputPotential.Assign(1,1,Amplitude);
        AvgFrequencyCounter->push_back(Environment->GetTime().GetDoubleTime());
       }
      }
     }

     bool pulse = false;
     for(size_t i=0; i<pulses.size(); i++)
     {
         if(pulses[i]==true)
         {
            pulse = true;
            break;
         }
     }
     if(pulse)
     {
         Output.Assign(1,1,Amplitude);
         OutputPotential.Assign(1,1,Amplitude);
     }
     else
     {
         Output.ToZero();
         OutputPotential.ToZero();
     }



     //ХЗ зачем это надо (и как причесывать и надо ли для мультивыходов)
     OutputFrequency.Assign(1,1,Frequency);
 }
 else
 {
  /*
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
  */
 }

 /*
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
 */

 return true;
}
// --------------------------
}
#endif
