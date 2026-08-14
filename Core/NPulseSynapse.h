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

#ifndef NPULSE_SYNAPSE_H
#define NPULSE_SYNAPSE_H

#include "NPulseSynapseCommon.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapse: public NPulseSynapseCommon
{
public: //  
/// Постоянная времени выделения медиатора
UProperty<double,NPulseSynapse, ptPubParameter> SecretionTC;

/// Постоянная времени распада медиатора
UProperty<double,NPulseSynapse, ptPubParameter> DissociationTC;

/// Коэффициент пресинаптического торможения
UProperty<double,NPulseSynapse, ptPubParameter> InhibitionCoeff;

/// Типовая длительность импульса, с
UProperty<double,NPulseSynapse, ptPubParameter> TypicalPulseDuration;

/// Флаг работы в режиме симуляции пришедшего импульса по сигналу
UProperty<bool,NPulseSynapse, ptPubParameter> UsePulseSignal;

/// Флаг включения пресинаптического торможения
UProperty<bool,NPulseSynapse, ptPubParameter> UsePresynapticInhibition;

public: //   

protected: //  

protected: //  
// Постоянные времени выделения и распада медиатора в единицах шага интегрирования
double VSecretionTC,VDissociationTC;

// Постоянная составляющая результатов вычислений
double OutputConstData;

/// Счетчик числа итераций в течении которых на входет присутствует импульс
int PulseCounter;

public: // 
// --------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
NPulseSynapse(void);
virtual ~NPulseSynapse(void);
// --------------------------

protected:
// --------------------------
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
// --------------------------
bool SetPulseAmplitude(const double &value);

// Постоянная времени выделения медиатора
bool SetSecretionTC(const double &value);

// Постоянная времени распада медиатора
bool SetDissociationTC(const double &value);

/// Типовая длительность импульса, с
bool SetTypicalPulseDuration(const double &value);

// Коэффициент пресинаптического торможения
bool SetInhibitionCoeff(const double &value);

/// Флаг работы в режиме симуляции пришедшего импульса по сигналу
bool SetUsePulseSignal(const bool &value);

// Задание флага включения пресинаптического торомжения
bool SetUsePresynapticInhibition(const bool &value);

// Вес (эффективность синапса) синапса
bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// --------------------------
virtual NPulseSynapse* New(void);
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

/// PSI conductance C*(1-k*p)*p.
/// The parabola is <= 0 for p >= 1/k. With k=2 a shared burst keeps Pre above
/// 0.5 after pulse 1, so later dendrite windows saw no peak and length sync
/// never started. Apply a small residual only while input is active — flooring
/// the gate during dissociation leaked DC and inflated soma amp at long L.
inline double PresynapticInhibitionConductance(
 double output_const, double inhibition_coeff, double pre, bool input_active)
{
 if(inhibition_coeff <= 0.0)
  return output_const * pre;
 double gate = 1.0 - inhibition_coeff * pre;
 if(gate >= 0.0)
  return output_const * gate * pre;
 if(!input_active)
  return 0.0;
 constexpr double kPsiMinGate = 0.05;
 return output_const * kPsiMinGate * pre;
}

}
#endif
