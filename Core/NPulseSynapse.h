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
public: // Общедоступные свойства
/// Постоянная времени выделения медиатора
ULProperty<double,NPulseSynapse, ptPubParameter> SecretionTC;

/// Постоянная времени распада медиатора
ULProperty<double,NPulseSynapse, ptPubParameter> DissociationTC;

/// Коэффициент пресинаптического торможения
ULProperty<double,NPulseSynapse, ptPubParameter> InhibitionCoeff;

/// Флаг включения пресинаптического торможения
ULProperty<bool,NPulseSynapse, ptPubParameter> UsePresynapticInhibition;

public: // Входы и выходы

protected: // Основные свойства

protected: // Временные переменные
// Постоянные времени выделения и распада медиатора в единицах шага интегрирования
double VSecretionTC,VDissociationTC;

// Постоянная составляющая результатов вычислений
double OutputConstData;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapse(void);
virtual ~NPulseSynapse(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);

// Постоянная времени выделения медиатора
bool SetSecretionTC(const double &value);

// Постоянная времени распада медиатора
bool SetDissociationTC(const double &value);

// Коэффициент пресинаптического торможения
bool SetInhibitionCoeff(const double &value);

// Задание флага включения пресинаптического торомжения
bool SetUsePresynapticInhibition(const bool &value);

// Вес (эффективность синапса) синапса
bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSynapse* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
