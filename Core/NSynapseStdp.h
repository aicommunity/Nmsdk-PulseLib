#ifndef NMSDK_NSYNAPSESTDP_H
#define NMSDK_NSYNAPSESTDP_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseStdp: public NPulseSynapseCommon
{
public:
ULProperty<double,NSynapseStdp, ptPubParameter> XModCoeff;

ULProperty<double,NSynapseStdp, ptPubParameter> YModCoeff;

ULProperty<double,NSynapseStdp, ptPubParameter> APlus;

ULProperty<double,NSynapseStdp, ptPubParameter> AMinus;

ULProperty<double,NSynapseStdp, ptPubParameter> XTau;
ULProperty<double,NSynapseStdp, ptPubParameter> YTau;

public: // Переменные состояния
/// Усреденение входного импульса
ULProperty<double,NSynapseStdp, ptPubState> XAvg;

/// Усреденение выходного импульса
ULProperty<double,NSynapseStdp, ptPubState> YAvg;

/// Промежуточная разность влияния X и Y компонент
ULProperty<double,NSynapseStdp, ptPubState> XYDiff;


public: // Входы и выходы
/// Входной сигнал постсинаптической активности (либо этого же нейрона, либо внещнего
/// например модулирующего)
UPropertyInputData<MDMatrix<double>, NSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// Выходной сигнал влияния STDP
UPropertyOutputData<MDMatrix<double>, NSynapseStdp, ptOutput | ptPubState> StdpInfluence;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseStdp(void);
virtual ~NSynapseStdp(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseStdp* New(void);
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

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



