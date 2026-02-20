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
public: //  
UProperty<double,NPulseSynapseStdp, ptPubParameter> XModCoeff;

UProperty<double,NPulseSynapseStdp, ptPubParameter> YModCoeff;

UProperty<double,NPulseSynapseStdp, ptPubParameter> APlus;

UProperty<double,NPulseSynapseStdp, ptPubParameter> AMinus;

UProperty<double,NPulseSynapseStdp, ptPubParameter> XTau;
UProperty<double,NPulseSynapseStdp, ptPubParameter> YTau;

public: //  
/// Усреденение входного импульса
UProperty<double,NPulseSynapseStdp, ptPubState> XAvg;

/// Усреденение выходного импульса
UProperty<double,NPulseSynapseStdp, ptPubState> YAvg;

/// Промежуточная разность влияния X и Y компонент
UProperty<double,NPulseSynapseStdp, ptPubState> XYDiff;


public: //   
/// Входной сигнал внешней постсинаптической активности
/// (например модулирующего нейрона)
UProperty<MDMatrix<double>, NPulseSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// Выходной сигнал влияния STDP
UProperty<MDMatrix<double>, NPulseSynapseStdp, ptOutput | ptPubState> StdpInfluence;

protected: //  

protected: //  

public: // 
// --------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
NPulseSynapseStdp(void);
virtual ~NPulseSynapseStdp(void);
// --------------------------

protected:
// --------------------------
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
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
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// --------------------------
virtual NPulseSynapseStdp* New(void);
// --------------------------

// --------------------------
// --------------------------
// Скрытые методы управления счетом
// --------------------------
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
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
