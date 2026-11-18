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

#ifndef NNEURON_LIFE_H
#define NNEURON_LIFE_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "NSum.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNeuronLife: public UNet
{
public: //  
/// Статический порог нейрона
UProperty<double,NNeuronLife,ptPubParameter> Threshold;

/// Критическое значение энергии, используемое для расчета к-та мотивации
/// тормозных синапсов
UProperty<double,NNeuronLife,ptPubParameter> CriticalEnergy;

//переменные для расчета Износа

/// Ожидаемое значение максимального градиента выхода нейрона
UProperty<double,NNeuronLife,ptPubParameter> MaxPotentialGradient;

UProperty<double,NNeuronLife,ptPubParameter> WearOutConstPositive;

UProperty<double,NNeuronLife,ptPubParameter> WearOutConstNegative;

UProperty<double,NNeuronLife,ptPubParameter> Kw;

/// переменные для расчета самочувствия
UProperty<double,NNeuronLife,ptPubParameter> Emax;

UProperty<double,NNeuronLife,ptPubParameter> En;

UProperty<double,NNeuronLife,ptPubParameter> Kq;

/// переменные для расчета запаса энергии
UProperty<double,NNeuronLife,ptPubParameter> Ee0;
UProperty<double,NNeuronLife,ptPubParameter> Es;
UProperty<double,NNeuronLife,ptPubParameter> Eh0;
UProperty<double,NNeuronLife,ptPubParameter> Econst;
UProperty<double,NNeuronLife,ptPubParameter> Ecr;
UProperty<double,NNeuronLife, ptPubParameter> WearOutcr;
UProperty<double,NNeuronLife,ptPubParameter> EnergyWearOutCritical;
//RDK::UProperty<double,NNeuronLife, ptPubParameter> E;    //?
//RDK::UProperty<double,NNeuronLife, ptPubParameter> N;   //?
UProperty<double,NNeuronLife,ptPubParameter> EyConst;
UProperty<double,NNeuronLife,ptPubParameter> EyBonusPos;
UProperty<double,NNeuronLife,ptPubParameter> EyBonusNeg;

/// Номинальный предел усвояемости энергии
UProperty<double,NNeuronLife,ptPubParameter> EnergyComprehensibility;

/// Переменные для расчета порога
UProperty<double,NNeuronLife,ptPubParameter> Kdp;
UProperty<double,NNeuronLife,ptPubParameter> Pdmax;
UProperty<double,NNeuronLife,ptPubParameter> Qd;
UProperty<double,NNeuronLife,ptPubParameter> Khp0;
UProperty<double,NNeuronLife,ptPubParameter> Khp1;

UProperty<double,NNeuronLife,ptPubParameter> Qsummax;

// Ожидаемый бонус к подаче энергии
UProperty<double,NNeuronLife,ptPubParameter> EnergyBonus;

public: //    
// Запас Энергии
UProperty<double,NNeuronLife,ptPubState> Energy;

public: //   
///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input1;

///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input2;

///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input3;

///
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output1;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output2;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output3;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> OutputThreshold;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output5;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output6;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output7;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output8;

protected: //  
/// Износ нейрона
UProperty<double,NNeuronLife,ptPubState> WearOut;

/// Порог нейрона
UProperty<double,NNeuronLife,ptPubState> ThresholdLife;

UProperty<double,NNeuronLife,ptPubState> ThresholdWearOut;

UProperty<double,NNeuronLife,ptPubState> ThresholdFeel;

/// Самочувствие
UProperty<double,NNeuronLife,ptPubState> Feel;

UProperty<double,NNeuronLife,ptPubState> Qsum;

UProperty<double,NNeuronLife,ptPubState> Esum;

//RDK::UProperty<double,NNeuronLife,ptPubState> UsumOld;

//RDK::UProperty<double,NNeuronLife,ptPubState> UsumProizv;

UProperty<double,NNeuronLife,ptPubState> EsumOld;

UProperty<double,NNeuronLife,ptPubState> EsumProizv;

/// Прибавка энергии за шаг
UProperty<double,NNeuronLife,ptPubState> Ea;

/// Расход энергии за шаг на генерацию импульса
UProperty<double,NNeuronLife,ptPubState> Ey;

/// Расход энергии за шаг на поддержание запаса энергии
UProperty<double,NNeuronLife,ptPubState> Ee;

/// Расход энергии за шаг на противодействие износу
UProperty<double,NNeuronLife,ptPubState> Eh;

/// Производная - прибавка энергии за шаг
UProperty<double,NNeuronLife,ptPubState> dEa;

/// Производная - расход энергии за шаг на генерацию импульса
UProperty<double,NNeuronLife,ptPubState> dEy;

/// Производная - расход энергии за шаг на поддержание запаса энергии
UProperty<double,NNeuronLife,ptPubState> dEe;

/// Производная - расход энергии за шаг на противодействие износу
UProperty<double,NNeuronLife,ptPubState> dEh;

/// ПРоизводная текущего запаса энергии
UProperty<double,NNeuronLife,ptPubState> dE;

UProperty<double,NNeuronLife,ptPubState> Usum;

// Суммарный потенциал
//RDK::UProperty<double,NNeuronLife,ptPubState> NeuralPotential;

// Промежуточное значение потенциала
//RDK::UProperty<double,NNeuronLife,ptPubState> PrePotential;

// Флаг наличия генерации
//RDK::UProperty<int,NNeuronLife,ptPubState> PulseCounter;
public: //    
// Ожидаемый бонус к подаче энергии
UEPointer<NSum, NNeuronLife> ExternalEnergyBonus;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronLife(void);
virtual ~NNeuronLife(void);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetThreshold(const double &value);

// Устанавливает ожидаемое значение максимального градиента выхода нейрона
bool SetMaxPotentialGradient(const double &value);

bool SetEmax(const double &value);

bool SetEn(const double &value);

bool SetPdmax(const double &value);

bool SetQsummax(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NNeuronLife* New(void);
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

virtual bool ACalcWearOut(void);

virtual bool ACalcThresholdLife(void);

virtual bool ACalcEnergy(void);

virtual bool ACalcFeel(void);

double FeelDiff(double kq, double ediff, double en);

//virtual bool ACalcEnergy(void);
};

}
#endif
