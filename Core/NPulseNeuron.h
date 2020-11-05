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

#ifndef NPULSE_NEURON_H
#define NPULSE_NEURON_H

#include "NPulseNeuronCommon.h"
#include "NPulseMembrane.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseNeuron: public NPulseNeuronCommon
{
public: // Параметры
/// Режим сборки структуры нейрона
/// (В режимах >1) осуществляется автомтический выбор классов участков мембраны и низкопороговой зоны
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка производится с заданными вручную параметрами
/// 2 - автоматическая сборка с заданными вручную параметрами и дендритами заданной длины

/// 3 - сборка классической модели без выделенного участка мембраны для генераторной зоны
/// 4 - сборка классической модели без выделенного участка мембраны для генераторной зоны
/// и встроенными синапсами
/// 5 - сборка модели с выделенным участком мембраны для генераторной зоны
/// 6 - сборка модели с выделенным участком мембраны для генераторной зоны
/// и встроенными синапсами
/// 7 - сборка классической модели без выделенного участка мембраны для генераторной зоны
/// и дендритами заданной длины
/// 8 - сборка классической модели без выделенного участка мембраны для генераторной зоны,
/// дендритами заданной длины и встроенными синапсами
/// 9 - сборка модели с выделенным участком мембраны для генераторной зоны,
/// и дендритами заданной длины
/// 10 - сборка модели с выделенным участком мембраны для генераторной зоны,
/// встроенными синапсами и дендритами заданной длины
ULProperty<int,NPulseNeuron, ptPubParameter> StructureBuildMode;

/// Имя класса участка мембраны
ULProperty<std::string, NPulseNeuron, ptPubParameter> MembraneClassName;

/// Имя класса выделенного участка мембраны для генераторной зоны
ULProperty<std::string, NPulseNeuron, ptPubParameter> LTMembraneClassName;

/// Имя класса генераторной зоны
ULProperty<std::string, NPulseNeuron, ptPubParameter> LTZoneClassName;

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
ULProperty<std::string, NPulseNeuron, ptPubParameter> ExcGeneratorClassName;

/// Имя класса источника задающего сигнала для тормозного ионного механизма
ULProperty<std::string, NPulseNeuron, ptPubParameter> InhGeneratorClassName;

/// Число участков мембраны тела нейрона
ULProperty<int, NPulseNeuron, ptPubParameter> NumSomaMembraneParts;

/// Число участков мембраны дендритов (исключая участок тела)
ULProperty<int, NPulseNeuron, ptPubParameter> NumDendriteMembraneParts;

/// Число участков мембраны дендритов (исключая участок тела)
ULProperty<std::vector<int>, NPulseNeuron, ptPubParameter> NumDendriteMembranePartsVec;

/// Паттерн, которому обучен нейрон
/// (если нейрон не обучен Size = 0)
/// НАЗНАЧАЕТСЯ ПРОГРАММНО! НЕ ДОЛЖЕН МЕНЯТЬСЯ ПОЛЬЗОВАТЕЛЕМ!!!
ULProperty<MDMatrix<double>, NPulseNeuron, ptPubParameter> TrainingPattern;

/// Индексы входных участков на дендритах
/// (если нейрон не обучен Size = 0)
/// НАЗНАЧАЕТСЯ ПРОГРАММНО! НЕ ДОЛЖЕН МЕНЯТЬСЯ ПОЛЬЗОВАТЕЛЕМ!!!
ULProperty<MDMatrix<int>, NPulseNeuron, ptPubParameter> TrainingDendIndexes;

/// Количество синапсов на входных участков на дендритах
/// (если нейрон не обучен Size = 0)
/// НАЗНАЧАЕТСЯ ПРОГРАММНО! НЕ ДОЛЖЕН МЕНЯТЬСЯ ПОЛЬЗОВАТЕЛЕМ!!!
ULProperty<MDMatrix<int>, NPulseNeuron, ptPubParameter> TrainingSynapsisNum;


protected:
NConstGenerator *PosGenerator,*NegGenerator;

vector<UEPtr<NPulseMembraneCommon> > Soma;

// Старые значения количества участков сомы и дендритов
int OldNumDendrites, OldNumSoma;

vector<int> OldNumDendritesVec;

int OldStructureBuildMode;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseNeuron(void);
virtual ~NPulseNeuron(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса участка мембраны
bool SetMembraneClassName(const std::string &value);

/// Имя класса выделенного участка мембраны для генераторной зоны
bool SetLTMembraneClassName(const std::string &value);

/// Имя класса генераторной зоны
bool SetLTZoneClassName(const std::string &value);

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
bool SetExcGeneratorClassName(const std::string &value);

/// Имя класса источника задающего сигнала для тормозного ионного механизма
bool SetInhGeneratorClassName(const std::string &value);

/// Число участков мембраны тела нейрона
bool SetNumSomaMembraneParts(const int &value);

/// Число участков мембраны дендритов (исключая участок тела)
bool SetNumDendriteMembraneParts(const int &value);

/// Число участков мембраны дендритов (исключая участок тела)
bool SetNumDendriteMembranePartsVec(const std::vector<int> &value);

/// Паттерн, которому обучен нейрон
bool SetTrainingPattern(const MDMatrix<double> &value);

/// Индексы входных участков на дендритах
bool SetTrainingDendIndexes(const MDMatrix<int> &value);

/// Количество синапсов на входных участков на дендритах
bool SetTrainingSynapsisNum(const MDMatrix<int> &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// Возвращает указатель на модель источника возбуждаюшего потенциала
NConstGenerator* GetPosGenerator(void);

// Возвращает указатель на модель источника тормозного потенциала
NConstGenerator* GetNegGenerator(void);

/// Доступ к участкам мембраны
size_t GetNumMembranes(void) const;
NPulseMembrane* GetMembrane(size_t i);
// --------------------------

// --------------------------
// Методы управления структурой объекта
// --------------------------
// Удлинняет заданный участок мембраны, добавляя к нему новый участок мембраны,
// и переключая входы заданного участка на входы нового
// Возвращает указатель на созданный участок
NPulseMembraneCommon* ElongateDendrite(const string &name, bool feedback=false);

// Разветвляет заданный участок мембраны, добавляя к точке его подключения
// дополнительно новый участок мембраны
// Возвращает указатель на созданный участок
NPulseMembraneCommon* BranchDendrite(const string &name, bool feedback=false);

// Удаляет заданный участок мембраны
// Если full == true, то удаляет и все другие участки, подключенные к нему
// Иначе перенаправляет связи со входов на свои выходы
bool EraseDendrite(const string &name);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseNeuron* New(void);
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
/// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
/// dendrite_mode принимает 2 значения:
/// 1 - все дендриты имеют одинаковую длину (используется dendrite_length)
/// 2 - все дендриты имеют свою длину (используется dendrite_length_vec, dendrite_length
/// меняется внутри и хранит максимальную длину)
bool BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
					const string &ltzone_class, const string &pos_gen_class,
					const string &neg_gen_class, int num_soma_membranes, int dendrite_mode,
					int dendrite_length, const vector<int> &dendrite_length_vec, int num_stimulates, int num_arresting);

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
public:
int GetNumOfConnectedSynToPosCh(NPulseMembrane* membr);
int GetNumOfConnectedSynToNegCh(NPulseMembrane* membr);
};

}
#endif
