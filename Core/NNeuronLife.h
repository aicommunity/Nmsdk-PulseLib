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
public: // Общедоступные свойства
/// Статический порог нейрона
ULProperty<double,NNeuronLife,ptPubParameter> Threshold;

/// Критическое значение энергии, используемое для расчета к-та мотивации
/// тормозных синапсов
ULProperty<double,NNeuronLife,ptPubParameter> CriticalEnergy;

//переменные для расчета Износа

/// Ожидаемое значение максимального градиента выхода нейрона
ULProperty<double,NNeuronLife,ptPubParameter> MaxPotentialGradient;

ULProperty<double,NNeuronLife,ptPubParameter> WearOutConstPositive;

ULProperty<double,NNeuronLife,ptPubParameter> WearOutConstNegative;

ULProperty<double,NNeuronLife,ptPubParameter> Kw;

/// переменные для расчета самочувствия
ULProperty<double,NNeuronLife,ptPubParameter> Emax;

ULProperty<double,NNeuronLife,ptPubParameter> En;

ULProperty<double,NNeuronLife,ptPubParameter> Kq;

/// переменные для расчета запаса энергии
ULProperty<double,NNeuronLife,ptPubParameter> Ee0;
ULProperty<double,NNeuronLife,ptPubParameter> Es;
ULProperty<double,NNeuronLife,ptPubParameter> Eh0;
ULProperty<double,NNeuronLife,ptPubParameter> Econst;
ULProperty<double,NNeuronLife,ptPubParameter> Ecr;
ULProperty<double,NNeuronLife> WearOutcr;
ULProperty<double,NNeuronLife,ptPubParameter> EnergyWearOutCritical;
//RDK::ULProperty<double,NNeuronLife> E;    //?
//RDK::ULProperty<double,NNeuronLife> N;   //?
ULProperty<double,NNeuronLife,ptPubParameter> EyConst;
ULProperty<double,NNeuronLife,ptPubParameter> EyBonusPos;
ULProperty<double,NNeuronLife,ptPubParameter> EyBonusNeg;

/// Номинальный предел усвояемости энергии
ULProperty<double,NNeuronLife,ptPubParameter> EnergyComprehensibility;

/// Переменные для расчета порога
ULProperty<double,NNeuronLife,ptPubParameter> Kdp;
ULProperty<double,NNeuronLife,ptPubParameter> Pdmax;
ULProperty<double,NNeuronLife,ptPubParameter> Qd;
ULProperty<double,NNeuronLife,ptPubParameter> Khp0;
ULProperty<double,NNeuronLife,ptPubParameter> Khp1;

ULProperty<double,NNeuronLife,ptPubParameter> Qsummax;

// Ожидаемый бонус к подаче энергии
ULProperty<double,NNeuronLife,ptPubParameter> EnergyBonus;

public: // Данные с внешним управлением
// Запас Энергии
ULProperty<double,NNeuronLife,ptPubState> Energy;

public: // Входы и выходы
///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input1;

///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input2;

///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input3;

///
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> OutputThreshold;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output6;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output7;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output8;

protected: // Временные переменные
/// Износ нейрона
ULProperty<double,NNeuronLife,ptPubState> WearOut;

/// Порог нейрона
ULProperty<double,NNeuronLife,ptPubState> ThresholdLife;

ULProperty<double,NNeuronLife,ptPubState> ThresholdWearOut;

ULProperty<double,NNeuronLife,ptPubState> ThresholdFeel;

/// Самочувствие
ULProperty<double,NNeuronLife,ptPubState> Feel;

ULProperty<double,NNeuronLife,ptPubState> Qsum;

ULProperty<double,NNeuronLife,ptPubState> Esum;

//RDK::ULProperty<double,NNeuronLife,ptPubState> UsumOld;

//RDK::ULProperty<double,NNeuronLife,ptPubState> UsumProizv;

ULProperty<double,NNeuronLife,ptPubState> EsumOld;

ULProperty<double,NNeuronLife,ptPubState> EsumProizv;

/// Прибавка энергии за шаг
ULProperty<double,NNeuronLife,ptPubState> Ea;

/// Расход энергии за шаг на генерацию импульса
ULProperty<double,NNeuronLife,ptPubState> Ey;

/// Расход энергии за шаг на поддержание запаса энергии
ULProperty<double,NNeuronLife,ptPubState> Ee;

/// Расход энергии за шаг на противодействие износу
ULProperty<double,NNeuronLife,ptPubState> Eh;

/// Производная - прибавка энергии за шаг
ULProperty<double,NNeuronLife,ptPubState> dEa;

/// Производная - расход энергии за шаг на генерацию импульса
ULProperty<double,NNeuronLife,ptPubState> dEy;

/// Производная - расход энергии за шаг на поддержание запаса энергии
ULProperty<double,NNeuronLife,ptPubState> dEe;

/// Производная - расход энергии за шаг на противодействие износу
ULProperty<double,NNeuronLife,ptPubState> dEh;

/// ПРоизводная текущего запаса энергии
ULProperty<double,NNeuronLife,ptPubState> dE;

ULProperty<double,NNeuronLife,ptPubState> Usum;

// Суммарный потенциал
//RDK::ULProperty<double,NNeuronLife,ptPubState> NeuralPotential;

// Промежуточное значение потенциала
//RDK::ULProperty<double,NNeuronLife,ptPubState> PrePotential;

// Флаг наличия генерации
//RDK::ULProperty<int,NNeuronLife,ptPubState> PulseCounter;
public: // Указатели на внутренние компоненты
// Ожидаемый бонус к подаче энергии
UEPointer<NSum, NNeuronLife> ExternalEnergyBonus;

public: // Методы
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
