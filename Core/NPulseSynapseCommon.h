// ===========================================================
// Version:        3.0.2
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

#ifndef NPULSE_SYNAPSE_COMMON_H
#define NPULSE_SYNAPSE_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "../../Nmsdk-BasicLib/Core/NNet.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapseCommon: public UDynamicMatNet
{
public: // Общедоступные свойства
// Амплитуда входных импульсов
RDK::UProperty<double,NPulseSynapseCommon, ptPubParameter> PulseAmplitude;

// Вес (эффективность синапса) синапса
RDK::UProperty<double,NPulseSynapseCommon, ptPubParameter> Resistance;

protected: // Временные переменные
// Промежуточное значение эффективности синапса
RDK::UProperty<double,NPulseSynapseCommon,ptPubState> PreOutput;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapseCommon(void);
virtual ~NPulseSynapseCommon(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
virtual bool SetPulseAmplitude(const double &value);

// Вес (эффективность синапса) синапса
virtual bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSynapseCommon* New(void);
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
