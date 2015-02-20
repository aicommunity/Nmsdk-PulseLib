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

#include "../../Kernel/NSupport.h"
#include "../BCL/NNet.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapse: public UNet
{
public: // Общедоступные свойства
// Амплитуда входных импульсов
RDK::ULProperty<double,NPulseSynapse> PulseAmplitude;

// Постоянная времени выделения медиатора
RDK::ULProperty<double,NPulseSynapse> SecretionTC;

// Постоянная времени распада медиатора
RDK::ULProperty<double,NPulseSynapse> DissociationTC;

// Коэффициент пресинаптического торможения
RDK::ULProperty<double,NPulseSynapse> InhibitionCoeff;

// Вес (эффективность синапса) синапса
RDK::ULProperty<double,NPulseSynapse> Resistance;

public: // Данные

protected: // Основные свойства

protected: // Временные переменные
// Промежуточное значение эффективности синапса
RDK::ULProperty<double,NPulseSynapse,ptPubState> PreOutput;

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
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
