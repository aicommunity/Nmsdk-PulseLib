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

#include "NNet.h"
#include "NSynapseTrainer.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapseCommon: public UNet
{
public: // Общедоступные свойства
/// Тип синапса механизма
/// <0 - тормозит накопление отрицательного вклада в потенциал (или деполяризует мембрану) - возбуждающий синапс
/// >0 - тормозит накопление положительного вклада в потенциал (или гиперпорялизует мембрану) - тормозный синапс
ULProperty<double,NPulseSynapseCommon, ptPubParameter> Type;

/// Амплитуда входных импульсов
ULProperty<double,NPulseSynapseCommon, ptPubParameter> PulseAmplitude;

/// Сопротивление синапса
ULProperty<double,NPulseSynapseCommon, ptPubParameter> Resistance;

/// Вес синапса
ULProperty<double, NPulseSynapseCommon, ptPubParameter> Weight;

/// Имя класса-учителя, настраивающего вес синапса
ULProperty<std::string, NPulseSynapseCommon, ptPubParameter> TrainerClassName;

public: // Входы и выходы
/// Входной сигнал с нейрона
UPropertyInputData<MDMatrix<double>, NPulseSynapseCommon, ptInput | ptPubState> Input;

/// Внешний источник информации о весе. Перезаписывает параметр Weight если есть подключение
UPropertyInputData<MDMatrix<double>, NPulseSynapseCommon, ptInput | ptPubState> WeightInput;

/// Выходное влияние синапса на мембрану
UPropertyOutputData<MDMatrix<double>,NPulseSynapseCommon, ptOutput | ptPubState> Output;

/// Технологический выход, дублирующий входной сигнал на синапс
UPropertyOutputData<MDMatrix<double>,NPulseSynapseCommon, ptOutput | ptPubState> OutInCopy;

protected: // Временные переменные
/// Промежуточное значение эффективности синапса
ULProperty<double,NPulseSynapseCommon,ptPubState> PreOutput;

/// Сигнал наличия входного импульса
/// (сбрасывается автоматически в конце итерации)
ULProperty<bool, NPulseSynapseCommon, ptPubState> InputPulseSignal;

bool PulseSignalTemp;

NSynapseTrainer* Trainer;

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
virtual bool SetType(const double &value);

// Устанавливает амплитуду импульсов
virtual bool SetPulseAmplitude(const double &value);

// Вес (эффективность синапса) синапса
virtual bool SetResistance(const double &value);

/// Имя класса-учителя, настраивающего вес синапса
virtual bool SetTrainerClassName(const std::string &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSynapseCommon* New(void);

// Возвращает указатель на учителя
NSynapseTrainer* GetTrainer(void);

// Устанавливает связи тренера если он есть
virtual void RebuildInternalLinks(void);
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
virtual bool AAfterBuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
