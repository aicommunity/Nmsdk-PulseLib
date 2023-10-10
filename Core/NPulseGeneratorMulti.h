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

#ifndef NPULSE_GENERATOR_MULTI_H
#define NPULSE_GENERATOR_MULTI_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGeneratorMulti: public NSource
{
public: // Параметры
/// Частота генерации (Гц) (Математическое ожидание)
ULProperty<double,NPulseGeneratorMulti, ptPubParameter> Frequency;

/// Длительность импульса (с)
ULProperty<double,NPulseGeneratorMulti, ptPubParameter> PulseLength;

/// Амплитуда импульса
ULProperty<double,NPulseGeneratorMulti, ptPubParameter> Amplitude;

/// Задержка запуска генератора
ULProperty<int, NPulseGeneratorMulti, ptPubParameter> PulseCount;

/// Задержка запуска генератора
ULProperty<MDMatrix<double>, NPulseGeneratorMulti, ptPubParameter> Delays;

/// Отклонение частоты
/// (Задает диапазон, не является стандартным отклонением!)
ULProperty<double,NPulseGeneratorMulti, ptPubParameter> FrequencyDeviation;

/// Интервал времени оценки частоты генерации
ULProperty<double,NPulseGeneratorMulti, ptPubParameter> AvgInterval;

public: // Входы и выходы
/// Потенциал (для этого компонента копирует PulseOutput) (1)
UPropertyOutputData<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputPotential;

/// Частота (2)
UPropertyOutputData<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputFrequency;

/// Массив моментов времени начала импульсов (3)
UPropertyOutputData<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputPulseTimes;

public: // Переменные состояния
/// Счетчик импульса
ULProperty<vector<int>,NPulseGeneratorMulti,ptPubState> PulseCounters;

/// Случайное значение текущей частоты
ULProperty<double,NPulseGeneratorMulti,ptPubState> RandomFrequency;

/// Средняя частота за заданный интервал времени
UCLProperty<list<double>,NPulseGeneratorMulti,ptPubState> AvgFrequencyCounter;

protected:

//Отсчет состояния для каждой задержки по отдельности (чтобы в конце собрать в Output)
std::vector<bool> pulses;

double OldFrequency;

// Добавил Корсаков ----->
// Время последнего вызова Reset
double ResetTime;
// <-----

// Добавила Демчева ----->
//Время начала работы генератора в режиме повышенной частоты (UsePatternOutput)
double PatternStartTime;
//Флаг режима повышенной частоты (UsePatternOutput)
bool IsInPatternMode;
// <-----


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorMulti(void);
virtual ~NPulseGeneratorMulti(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает частоту импульсов
bool SetFrequency(const double &value);

// Устанавливает длительность импульса
bool SetPulseLength(const double &value);

// Устанавливает амплитуду импульсов
bool SetAmplitude(const double &value);

// Устанавливает число импульсов
bool SetPulseCount(const int &value);

// Устанавливает задержку запуска генератора
bool SetDelays(const MDMatrix<double> &value);

// Устанавливает случайное отклонение частоты
// (Задает диапазон, не является стандартным отклонением!)
bool SetFrequencyDeviation(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseGeneratorMulti* New(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}
#endif

