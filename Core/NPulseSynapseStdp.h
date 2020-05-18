// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2020.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_STDP_H
#define NPULSE_SYNAPSE_STDP_H

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapseStdp: public NPulseSynapse
{
public: // Общедоступные свойства
ULProperty<double,NPulseSynapseStdp, ptPubParameter> XModCoeff;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> YModCoeff;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> APlus;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> AMinus;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> XTau;
ULProperty<double,NPulseSynapseStdp, ptPubParameter> YTau;

public: // Переменные состояния
/// Усреденение входного импульса
ULProperty<double,NPulseSynapseStdp, ptPubState> XAvg;

/// Усреденение выходного импульса
ULProperty<double,NPulseSynapseStdp, ptPubState> YAvg;

/// Промежуточная разность влияния X и Y компонент
ULProperty<double,NPulseSynapseStdp, ptPubState> XYDiff;


public: // Входы и выходы
/// Входной сигнал внешней постсинаптической активности
/// (например модулирующего нейрона)
UPropertyInputData<MDMatrix<double>, NPulseSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// Выходной сигнал влияния STDP
UPropertyOutputData<MDMatrix<double>, NPulseSynapseStdp, ptOutput | ptPubState> StdpInfluence;

protected: // Основные свойства

protected: // Временные переменные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapseStdp(void);
virtual ~NPulseSynapseStdp(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetXTau(const double &value);

bool SetYTau(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSynapseStdp* New(void);
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
