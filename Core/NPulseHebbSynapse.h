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
public: //  
UProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// -   
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// -   
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;

public: //   
/// Входной сигнал от низкопороговой зоны
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputLTZoneFeedbackSignal;

/// Сигнал "мотивации"
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputMotivation;

UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output1;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output2;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output3;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output4;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output5;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output6;

protected: //  
UProperty<double,NPulseHebbSynapse,ptPubState> G;
UProperty<double,NPulseHebbSynapse,ptPubState> Gd;
UProperty<vector<double>,NPulseHebbSynapse,ptPubState> Gs;
UProperty<double,NPulseHebbSynapse,ptPubState> GsSum;
UProperty<double,NPulseHebbSynapse,ptPubState> Win;
UProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // 
// --------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
// --------------------------

public:
// --------------------------
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// --------------------------
virtual NPulseHebbSynapse* New(void);
// --------------------------

// --------------------------
// --------------------------
// Скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
//       -
// Подключает синапс хебба к низкопороговой зоне нейрона-владельца
// Возвращает false только если произошла ошибка установки связи
// Если synapse == 0, то подключает все синапсы хебба
bool InstallHebbianConnection(void);

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
