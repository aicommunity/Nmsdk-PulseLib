// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_NEURON_COMMON_H
#define NPULSE_NEURON_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NNeuron.h"
#include "NPulseLTZone.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"
#include "NPulseMembraneCommon.h"
//#include "NAfferentNeuron.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseNeuronCommon: public NNeuron
{
public: // Параметры
/// Признак наличия усреднения в ветвлении дендритов
ULProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageDendritesPotential;

/// Признак наличия усреднения в ветвлении дендритов
ULProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageLTZonePotential;

public: // Входы и выходы
/// Число связей организованных этим нейроном на других (и себе)
UPropertyOutputData<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActiveOutputs;

/// Число возбуждающих связей организованных другими нейронами на этом
ULProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActivePosInputs;

/// Число тормозных связей организованных другими нейронами на этом
ULProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActiveNegInputs;

public: // Статистика
/// Число связей организованных этим нейроном на других (и себе)
ULProperty<double,NPulseNeuronCommon,ptPubState> NumActiveOutputs;

/// Число возбуждающих связей организованных другими нейронами на этом
ULProperty<double,NPulseNeuronCommon,ptPubState> NumActivePosInputs;

/// Число тормозных связей организованных другими нейронами на этом
ULProperty<double,NPulseNeuronCommon,ptPubState> NumActiveNegInputs;

protected: // Временные переменные
//NPulseLTZone *LTZone;

public:
RDK::UEPointer<NPulseLTZone,NPulseNeuronCommon> LTZone;

vector<NPulseMembraneCommon*> Membranes;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseNeuronCommon(void);
virtual ~NPulseNeuronCommon(void);
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Признак наличия усреднения в выходных данных нейрона
bool SetUseAverageDendritesPotential(const bool &value);

/// Признак наличия усреднения в выходных данных нейрона
bool SetUseAverageLTZonePotential(const bool &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// Возвращает указатель на модель низкопороговой зоны
NLTZone* GetLTZone(void);
// --------------------------

// --------------------------
// Методы управления структурой объекта
// --------------------------
//// Удлинняет заданный участок мембраны, добавляя к нему новый участок мембраны,
//// и переключая входы заданного участка на входы нового
//// Возвращает указатель на созданный участок
//NPulseMembrane* ElongateDendrite(const UId &id, bool feedback=false);
//
//// Разветвляет заданный участок мембраны, добавляя к точке его подключения
//// дополнительно новый участок мембраны
//// Возвращает указатель на созданный участок
//NPulseMembrane* BranchDendrite(const UId &id, bool feedback=false);
//
//// Удаляет заданный участок мембраны
//// Если full == true, то удаляет и все другие участки, подключенные к нему
//// Иначе перенаправляет связи со входов на свои выходы
//bool EraseDendrite(const UId &id);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseNeuronCommon* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);
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
};

}
#endif
