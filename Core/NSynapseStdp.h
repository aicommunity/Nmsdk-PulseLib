#ifndef NMSDK_NSYNAPSESTDP_H
#define NMSDK_NSYNAPSESTDP_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseStdp: public NPulseSynapseCommon
{
public:
UProperty<double,NSynapseStdp, ptPubParameter> XModCoeff;

UProperty<double,NSynapseStdp, ptPubParameter> YModCoeff;

UProperty<double,NSynapseStdp, ptPubParameter> APlus;

UProperty<double,NSynapseStdp, ptPubParameter> AMinus;

UProperty<double,NSynapseStdp, ptPubParameter> XTau;
UProperty<double,NSynapseStdp, ptPubParameter> YTau;

public: //  
/// Усреденение входного импульса
UProperty<double,NSynapseStdp, ptPubState> XAvg;

/// Усреденение выходного импульса
UProperty<double,NSynapseStdp, ptPubState> YAvg;

/// Промежуточная разность влияния X и Y компонент
UProperty<double,NSynapseStdp, ptPubState> XYDiff;


public: //   
/// Входной сигнал постсинаптической активности (либо этого же нейрона, либо внещнего
/// например модулирующего)
UProperty<MDMatrix<double>, NSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// Выходной сигнал влияния STDP
UProperty<MDMatrix<double>, NSynapseStdp, ptOutput | ptPubState> StdpInfluence;


public: // 
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



