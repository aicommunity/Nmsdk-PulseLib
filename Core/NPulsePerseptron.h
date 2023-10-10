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

#ifndef NPULSEPERSEPTRON_H
#define NPULSEPERSEPTRON_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, формирующий импульсный персептрон
class RDK_LIB_TYPE NPulsePerseptron: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// (В режимах >1) осуществляется автомтический выбор классов участков мембраны и низкопороговой зоны
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка производится, 1 нейрон -> 1 дендрит, связь - каждый с каждым
/// 2 - автоматическая сборка без связей
ULProperty<int,NPulsePerseptron, ptPubParameter> StructureBuildMode;

/// Имя класса, создающего слой групп афферентных нейронов, чувствительных к разным диапазонам частот
ULProperty<std::string, NPulsePerseptron, ptPubParameter> NeuronFreqGroupLayerClassName;

/// Число признаков на входе перспептрона (размер вектора признаков)
ULProperty<int, NPulsePerseptron, ptPubParameter> NumInputFeatures;

/// Имя класса, создающего слой нейронов
ULProperty<std::string, NPulsePerseptron, ptPubParameter> NeuronsLayerClassName;

/// Число слоёв нейронов
ULProperty<int, NPulsePerseptron, ptPubParameter> NumNeuronsLayer;

/// Размер слоя афферентных нейронов.
ULProperty<MDMatrix<int>, NPulsePerseptron, ptPubParameter> AffNeuronsLayerSize;

/// Размер слоёв нейронов.
ULProperty<MDMatrix<int>, NPulsePerseptron, ptPubParameter> NeuronsLayerSize;

/// Число афферентных нейронов в группе
ULProperty<int, NPulsePerseptron, ptPubParameter> NumAffNeuronsInGroup;

/// Необходимость FreqGroupLayer
ULProperty<bool, NPulsePerseptron, ptPubState> FlagFreqGroupLayer;

protected:

// Старое значение числа слоёв нейронов
int OldNumNeuronsLayer;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulsePerseptron(void);
virtual ~NPulsePerseptron(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса, создающего слой групп афферентных нейронов, чувствительных к разным диапазонам частот
bool SetNeuronFreqGroupLayerClassName(const std::string &value);

/// Число признаков на входе перспептрона (размер вектора признаков)
bool SetNumInputFeatures(const int &value);

/// Имя класса, создающего слой нейронов
bool SetNeuronsLayerClassName(const std::string &value);

/// Число слоёв нейронов
bool SetNumNeuronsLayer(const int &value);

/// Размер слоя афферентных нейронов.
bool SetAffNeuronsLayerSize(const MDMatrix<int> &value);

/// Размер слоёв нейронов.
bool SetNeuronsLayerSize(const MDMatrix<int> &value);

/// Число афферентных нейронов в группе
bool SetNumAffNeuronsInGroup(const int &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulsePerseptron* New(void);
static UComponent* NewStatic(void);
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
// Скрытые методы управления счетом
// --------------------------
protected:
/// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool BuildStructure(int structure_build_mode, const string &neuron_freq_group_layer_class_name,
					const string &neurons_layer_class_name,
					int old_num_neurons_layer, int num_neurons_layer,
					const MDMatrix<int> &aff_neurons_layer_size, const MDMatrix<int> &neurons_layer_size,
					int num_aff_neurons_in_group);

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
