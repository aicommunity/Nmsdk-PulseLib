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

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbSynapse: public NPulseSynapse
{
public: // Общедоступные свойства
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// К-т усиления динамической связи
RDK::ULProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// К-т усиления статической связи
RDK::ULProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;

public: // Входы и выходы
/// Входной сигнал от низкопороговой зоны
UPropertyInputData<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputLTZoneFeedbackSignal;

/// Сигнал "мотивации"
UPropertyInputData<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputMotivation;

UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output4;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output6;

protected: // Временные переменные
ULProperty<double,NPulseHebbSynapse,ptPubState> G;
ULProperty<double,NPulseHebbSynapse,ptPubState> Gd;
UCLProperty<vector<double>,NPulseHebbSynapse,ptPubState> Gs;
ULProperty<double,NPulseHebbSynapse,ptPubState> Win;
ULProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
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
