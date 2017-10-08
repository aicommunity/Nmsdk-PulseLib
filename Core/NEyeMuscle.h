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

#include "../../Nmsdk-BasicLib/Core/NSupport.h"

namespace NMSDK {

//using namespace MySDK;

class RDK_LIB_TYPE NEyeMuscle: public UDynamicMatNet
{
public: // Общедоступные свойства
// Коэффициенты
RDK::UPropertyRange<double,Real,NEyeMuscle,ptPubParameter> MulCoeffs;

RDK::UProperty<double,NEyeMuscle,ptPubParameter> K;

// Постоянные времени
RDK::UPropertyRange<double,Real,NEyeMuscle,ptPubParameter> TC;

public: // Временные переменные
Real P1,P2,P3,L;

// Порог
Real Threshold;

// Скорость
Real Speed;

// Ускорение
Real Acceleration;

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

