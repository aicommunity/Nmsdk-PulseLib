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

#ifndef NPATTERN_H
#define NPATTERN_H

#include "NSource.h"
#include "NPulseGenerator.h"

namespace NMSDK {

class RDK_LIB_TYPE NPattern: public NSource
{
public: // Параметры
/// Имя класса, создающего генератор импульсов
ULProperty<std::string, NPattern, ptPubParameter> PulseGeneratorClassName;

/// Размер входного паттерна
ULProperty<int,NPattern, ptPubParameter> NumGenerators;

/// Генерируемый паттерн импульсов. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
ULProperty<MDMatrix<double>, NPattern, ptPubParameter> InputPattern;

/// Частота генерации (Гц) (Математическое ожидание)
ULProperty<double,NPattern, ptPubParameter> Frequency;

/// Длительность импульса (с)
ULProperty<double,NPattern, ptPubParameter> PulseLength;

/// Амплитуда импульса
ULProperty<double,NPattern, ptPubParameter> Amplitude;

/// Задержка запуска генератора
ULProperty<double,NPattern, ptPubParameter> Delay;

/// Отклонение частоты
/// (Задает диапазон, не является стандартным отклонением!)
ULProperty<double,NPattern, ptPubParameter> FrequencyDeviation;

/// Признак необходимости транзита сигнала от внешнего источника
ULProperty<bool,NPattern, ptPubParameter> UseTransitSignal;


public: // Входны и выходы


public: // Переменные состояния


protected:

double OldNumGenerators;

// Генераторы импульсов
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPattern(void);
virtual ~NPattern(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Имя класса, создающего генератор импульсов
bool SetPulseGeneratorClassName(const std::string &value);

// Устанавливает размер входного паттерна
bool SetNumGenerators(const int &value);

/// Устанавливает генерируемый паттерн импульсов
bool SetInputPattern(const MDMatrix<double> &value);

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

// Устанавливает необходимость транзита сигнала от внешнего источника
bool SetUseTransitSignal(const bool &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPattern* New(void);
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------

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

