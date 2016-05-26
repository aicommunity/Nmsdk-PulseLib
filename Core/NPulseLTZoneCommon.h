// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_LTZONE_COMMON_H
#define NPULSE_LTZONE_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NLTZone: public UNet
{
public: // Общедоступные свойства
/// Порог нейрона
RDK::ULProperty<double,NLTZone> Threshold;

/// Признак наличия усреднения в выходных данных нейрона
ULProperty<bool,NLTZone> UseAveragePotential;

public: // Данные
/// Нейрон владелец мембраны канала
NPulseNeuron* Neuron;

protected: // Основные свойства

protected: // Временные переменные
/// Число подключенных синапсов к этой низкопороговой зоне
int CachedNumAConnectors;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLTZone(void);
virtual ~NLTZone(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает порог нейрона
bool SetThreshold(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Сброс процесса счета.
virtual bool AReset(void);
// --------------------------
};


class RDK_LIB_TYPE NPulseLTZoneCommon: public NLTZone
{
public: // Общедоступные свойства
// Амплитуда импульсов
RDK::ULProperty<double,NPulseLTZoneCommon> PulseAmplitude;

// Длительность импульса
RDK::ULProperty<double,NPulseLTZoneCommon> PulseLength;

// Интервал времени оценки частоты генерации
RDK::ULProperty<double,NPulseLTZoneCommon> AvgInterval;

public: // Данные
// Промежуточное значение потенциала
// Следует использовать эту переменную для сохранения состояния мембранного потенциала
RDK::ULProperty<double,NPulseLTZoneCommon,ptPubState> PrePotential;

protected: // Основные свойства

protected: // Временные переменные
// Флаг наличия генерации
RDK::ULProperty<int,NPulseLTZoneCommon,ptPubState> PulseCounter;

// Средняя частота за заданный интервал времени
RDK::UCLProperty<list<double>,NPulseLTZoneCommon,ptPubState> AvgFrequencyCounter;

// Признак текущей генерации импульса
RDK::ULProperty<bool,NPulseLTZoneCommon,ptPubState> PulseFlag;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZoneCommon(void);
virtual ~NPulseLTZoneCommon(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseLTZoneCommon* New(void);
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


}
#endif
