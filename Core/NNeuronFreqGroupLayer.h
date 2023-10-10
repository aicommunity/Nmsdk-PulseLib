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

#ifndef NNEURONFREQGROUPLAYER_H
#define NNEURONFREQGROUPLAYER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, создающий слой групп афферентных нейронов, чувствительных к разным диапазонам частот
class RDK_LIB_TYPE NNeuronFreqGroupLayer: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// (В режимах >1) осуществляется автомтический выбор классов участков мембраны и низкопороговой зоны
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка производится
ULProperty<int,NNeuronFreqGroupLayer, ptPubParameter> StructureBuildMode;

/// Имя класса группы афферентных нейрона
ULProperty<std::string, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronGroupClassName;

/// Число групп афферентных нейронов по оси у
ULProperty<int, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronsGroupHeight;
/// Число групп афферентных нейронов по оси х
ULProperty<int, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronsGroupWidth;
/// Число афферентных нейронов в группе
ULProperty<int, NNeuronFreqGroupLayer, ptPubParameter> NumAffNeuronsInGroup;


protected:

// Старые значения количества участков сомы и дендритов
int OldAffNeuronsGroupHeight, OldAffNeuronsGroupWidth;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronFreqGroupLayer(void);
virtual ~NNeuronFreqGroupLayer(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса участка мембраны
bool SetAffNeuronGroupClassName(const std::string &value);

/// Число групп афферентных нейронов по оси у
bool SetAffNeuronsGroupHeight(const int &value);

/// Число групп афферентных нейронов по оси x
bool SetAffNeuronsGroupWidth(const int &value);

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
virtual NNeuronFreqGroupLayer* New(void);
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
bool BuildStructure(int structure_build_mode, const string &aff_neuron_group_class_name,
					int old_num_aff_neurons_group_height, int old_num_aff_neurons_group_width,
					int num_aff_neurons_group_height, int num_aff_neurons_group_width,
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
