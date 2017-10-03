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

#ifndef NPULSE_HEBB_SYNAPSE_H
#define NPULSE_HEBB_SYNAPSE_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbSynapse: public NPulseSynapse
{
public: // Общедоступные свойства
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// К-т усиления динамической связи
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// К-т усиления статической связи
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;


protected: // Основные свойства

protected: // Временные переменные
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> G;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Gd;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse,ptPubState> Gs;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Win;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
//bool SetPulseAmplitude(double value);

// Постоянная времени выделения медиатора
//bool SetSecretionTC(double value);

// Постоянная времени распада медиатора
//bool SetDissociationTC(double value);

// Коэффициент пресинаптического торможения
//bool SetInhibitionCoeff(double value);

// Вес (эффективность синапса) синапса
//bool SetResistance(double value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseHebbSynapse* New(void);
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
