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

#ifndef NPATTERN_CPP
#define NPATTERN_CPP

#include <cstdlib>
#include <cstdio>
#include "NPattern.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPattern::NPattern(void)
 :
	PulseGeneratorClassName("PulseGeneratorClassName",this,&NPattern::SetPulseGeneratorClassName),
	NumGenerators("NumGenerators",this,&NPattern::SetNumGenerators),
	InputPattern("InputPattern",this,&NPattern::SetInputPattern),
	Frequency("Frequency",this,&NPattern::SetFrequency),
	PulseLength("PulseLength",this,&NPattern::SetPulseLength),
	Amplitude("Amplitude",this,&NPattern::SetAmplitude),
	Delay("Delay",this,&NPattern::SetDelay),
	FrequencyDeviation("FrequencyDeviation",this,&NPattern::SetFrequencyDeviation),
	UseTransitSignal("UseTransitSignal",this,&NPattern::SetUseTransitSignal)
{
 OldNumGenerators = 0;
 generators.clear();
}

NPattern::~NPattern(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Имя класса, создающего генератор импульсов
bool NPattern::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}


// Устанавливает размер входного паттерна
bool NPattern::SetNumGenerators(const int &value)
{
 Ready = false;

 OldNumGenerators = NumGenerators;
 InputPattern.Resize(value,1);

 return true;
}

// Устанавливает генерируемый паттерн импульсов
bool NPattern::SetInputPattern(const MDMatrix<double> &value)
{
 Ready = false;

 for(int i = 0; i < int(generators.size()); i++)
 {
  generators[i]->Delay = Delay + value[i];
  generators[i]->Reset();
 }

 return true;
}

// Устанавливает частоту импульсов
bool NPattern::SetFrequency(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Frequency = Frequency;
    generators[i]->Reset();
 }

 return true;
}

// Устанавливает длительность импульса
bool NPattern::SetPulseLength(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->PulseLength = PulseLength;
 }

 return true;
}

// Устанавливает амплитуду импульсов
bool NPattern::SetAmplitude(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Amplitude = Amplitude;
 }

 return true;
}

// Устанавливает задержку запуска генератора
bool NPattern::SetDelay(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Delay = Delay;
 }

 return true;
}

// Устанавливает отклонение частоты
// (Задает диапазон, не является стандартным отклонением!)
bool NPattern::SetFrequencyDeviation(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->FrequencyDeviation = FrequencyDeviation;
 }

 return true;
}

// Устанавливает необходимость транзита сигнала от внешнего источника
bool NPattern::SetUseTransitSignal(const bool &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->UseTransitSignal = UseTransitSignal;
 }

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPattern* NPattern::New(void)
{
 return new NPattern;
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
bool NPattern::ADefault(void)
{
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NumGenerators = 1;
 InputPattern.Resize(1,1);
 Frequency = 0.5;
 PulseLength = 0.001;
 Amplitude = 1.0;
 Delay = 0.1;
 FrequencyDeviation = 0;
 UseTransitSignal = false;

 return NSource::ADefault();
}

// Сброс процесса счета.
bool NPattern::AReset(void)
{
 OldNumGenerators = NumGenerators;

 for(int i = 0; i < int(generators.size()); i++)
 {
	generators[i]->Frequency = Frequency;
	generators[i]->PulseLength = PulseLength;
	generators[i]->Amplitude = Amplitude;
	generators[i]->Delay = Delay + InputPattern(i,0);
	generators[i]->FrequencyDeviation = FrequencyDeviation;
	generators[i]->UseTransitSignal = UseTransitSignal;
    generators[i]->Reset();
 }

 return NSource::AReset();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPattern::ABuild(void)
{
	// Удаляем старые генераторы
	for(int i = NumGenerators; i < OldNumGenerators; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));
	}
	// Инициализируем генераторы импульсов
	generators.resize(NumGenerators);
	for(int i = 0; i < NumGenerators; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), PulseGeneratorClassName);
		generators[i]->SetCoord(MVector<double,3>(7.0,2.0+i*2,0));
	}

	OldNumGenerators = 0;

	return true;
}

// Выполняет расчет этого объекта
bool NPattern::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
