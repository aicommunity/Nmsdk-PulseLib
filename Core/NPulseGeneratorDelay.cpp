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

#ifndef NPULSE_GENERATOR_DELAY_CPP
#define NPULSE_GENERATOR_DELAY_CPP

#include <cstdlib>
#include <cstdio>
#include "NPulseGeneratorDelay.h"
//#include "NPulseChannel.h"



namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorDelay::NPulseGeneratorDelay(void)
//: NSource(name),
 : PulseLength("PulseLength",this,&NPulseGeneratorDelay::SetPulseLength),
  Amplitude("Amplitude",this,&NPulseGeneratorDelay::SetAmplitude),
  ParameterData("ParameterData",this,&NPulseGeneratorDelay::SetParameterData),
  MinSample("MinSample",this,&NPulseGeneratorDelay::SetMinSample),
  MaxSample("MaxSample",this,&NPulseGeneratorDelay::SetMaxSample),

  Number("Number",this),
  FlagStart("FlagStart",this)
{
    Delay=0;
}


NPulseGeneratorDelay::~NPulseGeneratorDelay(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------

// Устанавливает длительность импульса
bool NPulseGeneratorDelay::SetPulseLength(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Устанавливает амплитуду импульсов
bool NPulseGeneratorDelay::SetAmplitude(const double &value)
{
 return true;
}

// Устанавливает амплитуду импульсов
bool NPulseGeneratorDelay::SetParameterData(const int &value)
{
 if((value <=0)|(value >= 5))
  return false;
 return true;
}

// Устанавливает минимальное значение в семпле
bool NPulseGeneratorDelay::SetMinSample(const double &value)
{
 return true;
}

// Устанавливает максимальное значение в семпле
bool NPulseGeneratorDelay::SetMaxSample(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseGeneratorDelay* NPulseGeneratorDelay::New(void)
{
 return new NPulseGeneratorDelay;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseGeneratorDelay::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseGeneratorDelay::ADefault(void)
{
 PulseLength=0.001;
 Amplitude=1.0;
 ParameterData=1;
 MinSample=4.3;
 MaxSample=7.9;

 Output.Assign(1,1,0.0);

 return NSource::ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseGeneratorDelay::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPulseGeneratorDelay::AReset(void)
{
 time_t tm;
 time(&tm);
 srand(static_cast<unsigned>(tm));
 switch(ParameterData)
 {
  case 1:
   MinSample=4.3;
   MaxSample=7.9;
     break;
  case 2:
   MinSample=2;
   MaxSample=4.4;
     break;
  case 3:
   MinSample=1;
   MaxSample=6.9;
     break;
  case 4:
   MinSample=0.1;
   MaxSample=2.5;
     break;
 }
 Output.ToZero();

 return NSource::AReset();
}

// Выполняет расчет этого объекта
bool NPulseGeneratorDelay::ACalculate(void)
{
 double norm;
 //if ((*FlagStart)(0,0) > 0)
 {
  auto TimeStart = Environment->GetTime().GetDoubleTime();
  norm=(Number - MinSample)/(MaxSample - MinSample);
  Delay=norm*0.01;
  while(Environment->GetTime().GetDoubleTime() - TimeStart < Delay){;}
  Output.Assign(1,1,Amplitude);
  auto TimeStartPulse = Environment->GetTime().GetDoubleTime();
  while(Environment->GetTime().GetDoubleTime() - TimeStartPulse < PulseLength){;}
  Output.ToZero();
 }
 return true;
}
// --------------------------

}
#endif
