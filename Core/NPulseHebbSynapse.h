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

#include "../../Kernel/NSupport.h"
#include "NPulseSynapse.h"


namespace NMSDK {

class NPulseHebbSynapse: public NPulseSynapse
{
public: // Общедоступные свойства
RDK::ULProperty<real,NPulseHebbSynapse> Min;
RDK::ULProperty<real,NPulseHebbSynapse> Mout;
RDK::ULProperty<real,NPulseHebbSynapse> Md;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> ActiveMs;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> PassiveMs;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> Kmot;
RDK::ULProperty<real,NPulseHebbSynapse> Kin;
RDK::ULProperty<real,NPulseHebbSynapse> Kout;

// К-т усиления динамической связи
RDK::ULProperty<real,NPulseHebbSynapse> GdGain;

// К-т усиления статической связи
RDK::ULProperty<real,NPulseHebbSynapse> GsGain;


protected: // Основные свойства

protected: // Временные переменные
RDK::ULProperty<real,NPulseHebbSynapse,ptPubState> G;
RDK::ULProperty<real,NPulseHebbSynapse,ptPubState> Gd;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse,ptPubState> Gs;
RDK::ULProperty<real,NPulseHebbSynapse,ptPubState> Win;
RDK::ULProperty<real,NPulseHebbSynapse,ptPubState> Wout;

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
//bool SetPulseAmplitude(real value);

// Постоянная времени выделения медиатора
//bool SetSecretionTC(real value);

// Постоянная времени распада медиатора
//bool SetDissociationTC(real value);

// Коэффициент пресинаптического торможения
//bool SetInhibitionCoeff(real value);

// Вес (эффективность синапса) синапса
//bool SetResistance(real value);
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
