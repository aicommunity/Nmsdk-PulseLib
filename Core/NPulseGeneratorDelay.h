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

#ifndef NPULSE_GENERATOR_DELAY_H
#define NPULSE_GENERATOR_DELAY_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGeneratorDelay: public NSource
{
public: // Параметры
/// Длительность импульса (с)
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> PulseLength;

/// Амплитуда импульса
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> Amplitude;

/// Входной параметр данных (от 1 до 4)
ULProperty<int,NPulseGeneratorDelay, ptPubParameter> ParameterData;

/// Минимальное значение выборки
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> MinSample;

/// Максимальное значение выборки
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> MaxSample;

protected: // Вычисляемые переменные

/// Задержка запуска генератора
double Delay;

public: // Входы и выходы
/// Вход вещественного числа
ULProperty<double,NPulseGeneratorDelay, ptPubInput> Number;

/// Момент отсчёта задержки
ULProperty<int,NPulseGeneratorDelay,ptPubInput> FlagStart;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseGeneratorDelay(void);
virtual ~NPulseGeneratorDelay(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------

// Устанавливает длительность импульса
bool SetPulseLength(const double &value);

// Устанавливает амплитуду импульсов
bool SetAmplitude(const double &value);

// Устанавливает амплитуду импульсов
bool SetParameterData(const int &value);

// Устанавливает минимальное значение в семпле
bool SetMinSample(const double &value);

// Устанавливает максимальное значение в семпле
bool SetMaxSample(const double &value);

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseGeneratorDelay* New(void);
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

