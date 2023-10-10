// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_LIFE_NEURON_H
#define NPULSE_LIFE_NEURON_H

#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "NNeuronLife.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseLifeNeuron: public NPulseNeuron
{
public: // Данные
/// Суммарный динамический вес возбуждающих связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGd;

/// Суммарный статический вес возбуждающих связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGs;

/// Суммарный результирующий вес возбуждающих связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosG;

/// Суммарный динамический вес тормозных связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGd;

/// Суммарный статический вес тормозных связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGs;

/// Суммарный результирующий вес тормозных связей
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegG;

public: // Входы и выходы
/// Выход: суммарный динамический вес возбуждающих связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGd;

/// Выход: суммарный динамический вес возбуждающих связей отнесенный к числу активных возбуждающих входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGdNorm;

/// Выход: суммарный статический вес возбуждающих связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGs;

/// Выход: суммарный статический вес возбуждающих связей отнесенный к числу активных возбуждающих входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGsNorm;

/// Суммарный результирующий вес возбуждающих связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosG;

/// Суммарный результирующий вес возбуждающих связей отнесенный к числу активных возбуждающих входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGNorm;

/// Выход: суммарный динамический вес тормозных связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGd;

/// Выход: суммарный динамический вес тормозных связей отнесенный к числу активных тормозных входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGdNorm;

/// Выход: суммарный статический вес тормозных связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGs;

/// Выход: суммарный статический вес тормозных связей отнесенный к числу активных тормозных входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGsNorm;

/// Суммарный результирующий вес тормозных связей
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegG;

/// Суммарный результирующий вес тормозных связей отнесенный к числу активных тормозных входов
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGNorm;

protected: // Временные переменные

UEPointer<NNeuronLife,NPulseLifeNeuron> NeuronLife;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLifeNeuron(void);
virtual ~NPulseLifeNeuron(void);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// Возвращает указатель на модель жизнеобеспечения
NNeuronLife* GetNeuronLife(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseLifeNeuron* New(void);
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
