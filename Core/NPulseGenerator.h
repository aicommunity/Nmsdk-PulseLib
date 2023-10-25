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

#ifndef NPULSE_GENERATOR_H
#define NPULSE_GENERATOR_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGenerator: public NSource
{
public: // Параметры
/// Частота генерации (Гц) (Математическое ожидание)
ULProperty<double,NPulseGenerator, ptPubParameter> Frequency;

/// Длительность импульса (с)
ULProperty<double,NPulseGenerator, ptPubParameter> PulseLength;

/// Амплитуда импульса
ULProperty<double,NPulseGenerator, ptPubParameter> Amplitude;

/// Задержка запуска генератора
ULProperty<double,NPulseGenerator, ptPubParameter> Delay;

/// Отклонение частоты
/// (Задает диапазон, не является стандартным отклонением!)
ULProperty<double,NPulseGenerator, ptPubParameter> FrequencyDeviation;

/// Интервал времени оценки частоты генерации
ULProperty<double,NPulseGenerator, ptPubParameter> AvgInterval;

public: // Входы и выходы
/// Потенциал (для этого компонента копирует PulseOutput) (1)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPotential;

/// Частота (2)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputFrequency;

/// Массив моментов времени начала импульсов (3)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPulseTimes;

public: // Переменные состояния
/// Счетчик импульса
ULProperty<int,NPulseGenerator,ptPubState> PulseCounter;

/// Случайное значение текущей частоты
ULProperty<double,NPulseGenerator,ptPubState> RandomFrequency;

/// Средняя частота за заданный интервал времени
UCLProperty<list<double>,NPulseGenerator,ptPubState> AvgFrequencyCounter;

//Флаг генерации сигнала в повышенной частоте (UsePatternOutput)
bool IsInPatternMode;

protected:

double OldFrequency;

// Добавил Корсаков ----->
// Время последнего вызова Reset
double ResetTime;
// <-----

// Добавила Демчева ----->
//Время начала работы генератора в режиме повышенной частоты (UsePatternOutput)
double PatternStartTime;

// <-----


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGenerator(void);
virtual ~NPulseGenerator(void);
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

// Устанавливает задержку запуска генератора
bool SetDelay(const double &value);

// Устанавливает случайное отклонение частоты
// (Задает диапазон, не является стандартным отклонением!)
bool SetFrequencyDeviation(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseGenerator* New(void);
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

class RDK_LIB_TYPE NPulseGeneratorTransit: public NPulseGenerator
{
public: // Параметры
/// Признак необходимости транзита сигнала от внешнего источника
ULProperty<bool,NPulseGeneratorTransit, ptPubParameter> UseTransitSignal;

/// Транзитный сигнал
UPropertyInputData<MDMatrix<double>,NPulseGeneratorTransit, ptInput | ptPubState> Input;

/// Признак необходимости генерации сигнала с повышенной частотой PatternFrequency по внешнему импульсу
/// Два формата:
/// 1. Генерация с повышенной частотой в течение заданного периода PatternDuration
/// 2. Генерация с повышенной частотой до получения повторного входного сигнала, прекращающего генерацию
ULProperty<bool,NPulseGeneratorTransit, ptPubParameter> UsePatternOutput;

/// Продолжительность периода работы в режиме повышенной частоты (UsePatternOutput) (c)
ULProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternDuration;

/// Частота генерации в режиме повышенной частоты (UsePatternOutput) (Гц)
ULProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternFrequency;


protected:

/// Метка, что на входе фиксируется тот же импульс, что и на предыдущем такте
/// За время импульса происходит несколько тактов, и поэтому входное значение > 0
/// на очередном такте не должно воприниматься как новый импульс, если TheSamePulse == true
bool TheSamePulse = false;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorTransit(void);
virtual ~NPulseGeneratorTransit(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает необходимость транзита сигнала от внешнего источника
bool SetUseTransitSignal(const bool &value);
// --------------------------
// Устанавливает необходимость увеличения частоты сигнала (на определенный период времени) по внешнему импульсу
bool SetUsePatternOutput(const bool &value);
// --------------------------
// Устанавливает продолжительность периода работы в режиме повышенной частоты (UsePatternOutput)
bool SetPatternDuration(const double &value);
// --------------------------
// Устанавливает частоту генерации в режиме повышенной частоты (UsePatternOutput)
bool SetPatternFrequency(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseGeneratorTransit* New(void);
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

