// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NEYEMUSCLE_H
#define NEYEMUSCLE_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NEyeMuscle: public UNet
{
public: // Общедоступные свойства
/// Коэффициенты
UCLProperty<vector<double>,NEyeMuscle, ptPubParameter> MulCoeffs;

ULProperty<double,NEyeMuscle, ptPubParameter> K;

/// Постоянные времени
UCLProperty<vector<double>,NEyeMuscle, ptPubParameter> TC;

public: // Входы и выходы
UPropertyInputCData<MDMatrix<double>, NEyeMuscle> Inputs;

UPropertyOutputData<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputAcceleration;
UPropertyOutputData<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputLength;
UPropertyOutputData<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputSpeed;

public: // Временные переменные
vector<double> P1,P2,P3,L;

// Порог
vector<double> Threshold;

// Скорость
vector<double> Speed;

// Ускорение
vector<double> Acceleration;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NEyeMuscle(void);
virtual ~NEyeMuscle(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду синуса
bool SetAmplitude(double value);

// Устанавливает частоту генерации
bool SetFrequency(double value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NEyeMuscle* New(void);
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

// --------------------------
// Дополнительные скрытые методы управления счетом
// --------------------------
protected:
// мускульное сокращение
double MuscularReduction(size_t k,double in);

// Пороговая функция
void ThresholdCount(size_t k);
// --------------------------
};

}
#endif

