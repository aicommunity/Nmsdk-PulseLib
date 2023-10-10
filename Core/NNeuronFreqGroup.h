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

#ifndef NNEURONFREQGROUP_H
#define NNEURONFREQGROUP_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, создающий группу афферентных нейронов, чувствительных к разным диапазонам частот
class RDK_LIB_TYPE NNeuronFreqGroup: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// (В режимах >1) осуществляется автомтический выбор классов участков мембраны и низкопороговой зоны
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка производится с равномерным разбиением диапазона въодных частот
ULProperty<int,NNeuronFreqGroup, ptPubParameter> StructureBuildMode;

/// Имя класса афферентного нейрона
ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> AffNeuronClassName;

/// Число афферентных нейронов
ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumAffNeurons;

/// Диапазон входных частот
/// Минимальная входная частота
ULProperty<int, NNeuronFreqGroup, ptPubParameter> MinInputFreq;
/// Максимальная входная частота
ULProperty<int, NNeuronFreqGroup, ptPubParameter> MaxInputFreq;

/// Имя класса выделенного участка мембраны для генераторной зоны
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> LTMembraneClassName;

/// Имя класса генераторной зоны
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> LTZoneClassName;

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> ExcGeneratorClassName;

/// Имя класса источника задающего сигнала для тормозного ионного механизма
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> InhGeneratorClassName;

/// Число участков мембраны тела нейрона
//ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumSomaMembraneParts;

/// Число участков мембраны дендритов (исключая участок тела)
//ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumDendriteMembraneParts;

protected:
//NConstGenerator *PosGenerator,*NegGenerator;

// Старые значения количества участков сомы и дендритов
int OldNumAffNeurons;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronFreqGroup(void);
virtual ~NNeuronFreqGroup(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса участка мембраны
bool SetAffNeuronClassName(const std::string &value);

/// Число афферентных нейронов
bool SetNumAffNeurons(const int &value);

/// Минимальная входная частота
bool SetMinInputFreq(const int &value);

/// Максимальная входная частота
bool SetMaxInputFreq(const int &value);

/// Имя класса выделенного участка мембраны для генераторной зоны
//bool SetLTMembraneClassName(const std::string &value);

/// Имя класса генераторной зоны
//bool SetLTZoneClassName(const std::string &value);

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
//bool SetExcGeneratorClassName(const std::string &value);

/// Имя класса источника задающего сигнала для тормозного ионного механизма
//bool SetInhGeneratorClassName(const std::string &value);

/// Число участков мембраны тела нейрона
//bool SetNumSomaMembraneParts(const int &value);

/// Число участков мембраны дендритов (исключая участок тела)
//bool SetNumDendriteMembraneParts(const int &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// Возвращает указатель на модель источника возбуждаюшего потенциала
//NConstGenerator* GetPosGenerator(void);

// Возвращает указатель на модель источника тормозного потенциала
//NConstGenerator* GetNegGenerator(void);

/// Доступ к участкам мембраны
//size_t GetNumMembranes(void) const;
//NPulseMembrane* GetMembrane(size_t i);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NNeuronFreqGroup* New(void);
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
bool BuildStructure(int structure_build_mode, const string &aff_neuron_class_name,
					int old_num_aff_neurons, int num_aff_neurons, int min_inp_freq, int max_inp_freq);

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
