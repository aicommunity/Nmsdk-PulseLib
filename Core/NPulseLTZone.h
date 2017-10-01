// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_LTZONE_H
#define NPULSE_LTZONE_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"
#include "NPulseLTZoneCommon.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseLTZone: public NPulseLTZoneCommon
{
public: // Общедоступные свойства
// Постоянная времени
RDK::UProperty<double,NPulseLTZone,ptPubParameter> TimeConstant;

public: // Данные

protected: // Основные свойства

protected: // Временные переменные
// Суммарный потенциал
RDK::UProperty<double,NPulseLTZone,ptPubState> NeuralPotential;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZone(void);
virtual ~NPulseLTZone(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает значение постоянной времени
bool SetTimeConstant(const double &value);

// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseLTZone* New(void);
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

/// Возвращает true если условие для генерации импульса выполнено
virtual bool CheckPulseOn(void);

/// Возвращает true если условие для генерации имульса не выполнено
virtual bool CheckPulseOff(void);
// --------------------------
};

class RDK_LIB_TYPE NContinuesLTZone: public NLTZone
{
public: // Общедоступные свойства
// Постоянная времени
RDK::UProperty<double,NContinuesLTZone,ptPubParameter> TimeConstant;

// Амплитуда импульсов
RDK::UProperty<double,NContinuesLTZone,ptPubParameter> PulseAmplitude;

// Длительность импульса
RDK::UProperty<double,NContinuesLTZone,ptPubParameter> PulseLength;

// Интервал времени оценки частоты генерации
RDK::UProperty<double,NContinuesLTZone,ptPubParameter> AvgInterval;

public: // Данные

protected: // Основные свойства

protected: // Временные переменные
// Суммарный потенциал
RDK::UProperty<double,NContinuesLTZone,ptPubState> NeuralPotential;

// Промежуточное значение потенциала
RDK::UProperty<double,NContinuesLTZone,ptPubState> PrePotential;

// Флаг наличия генерации
RDK::UProperty<int,NContinuesLTZone,ptPubState> PulseCounter;

// Средняя частота за заданный интервал времени
RDK::UPropertyRange<double,list<double>,NContinuesLTZone,ptPubState> AvgFrequencyCounter;

// Признак текущей генерации импульса
RDK::UProperty<bool,NContinuesLTZone,ptPubState> PulseFlag;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesLTZone(void);
virtual ~NContinuesLTZone(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает значение постоянной времени
bool SetTimeConstant(const double &value);

// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NContinuesLTZone* New(void);
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

class RDK_LIB_TYPE NPulseSimpleLTZone: public NPulseLTZone
{
public: // Общедоступные свойства

public: // Данные

protected: // Основные свойства

protected: // Временные переменные
NPulseGenerator generator;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSimpleLTZone(void);
virtual ~NPulseSimpleLTZone(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSimpleLTZone* New(void);
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

class RDK_LIB_TYPE NContinuesSimpleLTZone: public NContinuesLTZone
{
public: // Общедоступные свойства

public: // Данные

protected: // Основные свойства

protected: // Временные переменные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesSimpleLTZone(void);
virtual ~NContinuesSimpleLTZone(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает значение постоянной времени
bool SetTimeConstant(const double &value);

// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NContinuesSimpleLTZone* New(void);
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
