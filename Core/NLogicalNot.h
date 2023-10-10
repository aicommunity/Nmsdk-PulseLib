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

#ifndef NLOGICALNOT_H
#define NLOGICALNOT_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// Класс, создающий группу нейронов для моделирования условного рефлекса
class RDK_LIB_TYPE NLogicalNot: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// 0 - автоматическая сборка не производится
/// 1 - Создаём группу нейронов для моделирования условного рефлекса.
/// Нейроны организованы в качестве 2 каналов информации +
/// промежуточный слой, реализующий возникновение условного рефлекса.
/// Первый канал: безусловный раздражитель -> безусловный рефлекс
/// Второй канал: условный раздражитель
/// На первом этапе обучаются входные нейроны 2-х каналов, каждый своему паттерну импульсов.
/// На втором этапе обучается элемент "И" промежуточного слоя, который
/// должен реагировать единичным выходным импульсом при активации двух входов одновременно.
/// На третьем этапе происходит наращивание синапсов на большом нейроне до тех пор,
/// пока большой нейрон не начинает генерировать сигналы. Генерация сигнала большим
/// нейроном вызывает формирование связи выхода нейрона 2-ого канала на элемент
/// "ИЛИ" 1-ого канала.
ULProperty<int,NLogicalNot, ptPubParameter> StructureBuildMode;

/// Режим рвсчёта обучения нейрона
/// 0 - расчёт не производится
/// 1 - обучение путём наращивания длины дендритов путём максимизации амплитуды нейрона
/// 2 (НЕ ОТЛАЖЕНО!!!) - обучение путём наращивания длины дендритов по амплитуде и количества синапсов на дендрите
/// 3 - обучение путём наращивания длины дендритов путём максимального выравнивания времени прихода сигналов на сому
/// 4 - обучение путём наращивания длины дендритов по времени и количества синапсов на дендрите
/// 5 - автоматический подбор порога срабатывания нейрона для каждого автоматичеки сгенерированного паттерна

ULProperty<int,NLogicalNot, ptPubParameter> CalculateMode;

/// Имя класса, создающего генератор импульсов
ULProperty<std::string, NLogicalNot, ptPubParameter> PulseGeneratorClassName;

/// Имя класса, создающего модель логического "НЕ" (модель боли)
ULProperty<std::string, NLogicalNot, ptPubParameter> LogicalNotClassName;

/// Имя класса, создающего учитель нейрона
ULProperty<std::string, NLogicalNot, ptPubParameter> NeuronTrainerClassName;

/// Имя класса нейрона
ULProperty<std::string, NLogicalNot, ptPubParameter> NeuronClassName;

/// Имя класса синапса
ULProperty<std::string, NLogicalNot, ptPubParameter> SynapseClassName;

/// Число входных дендритов
ULProperty<int, NLogicalNot, ptPubParameter> NumInputDendrite;

/// Признак необходимости обучения
ULProperty<bool, NLogicalNot, ptPubParameter> IsNeedToTrain;

///// Частота генераторов (Гц)
ULProperty<double, NLogicalNot, ptPubParameter> SpikesFrequency;

/// Паттерн для запоминания. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
ULProperty<MDMatrix<double>, NLogicalNot, ptPubParameter> InputPattern;

/// Длина дендрита на первом нейроне в генераторной связке (число сегментов)
ULProperty<int, NLogicalNot, ptPubParameter> DendriteLengthNGen1;

/// Длина дендрита на втором нейроне в генераторной связке (число сегментов)
ULProperty<int, NLogicalNot, ptPubParameter> DendriteLengthNGen2;

/// Число синапсов на дендрите первого нейрона в генераторной связке
ULProperty<int, NLogicalNot, ptPubParameter> NumExcSynapsesNGen1;

/// Число синапсов на дендрите второго нейрона в генераторной связке
ULProperty<int, NLogicalNot, ptPubParameter> NumExcSynapsesNGen2;

/// Частота генератора в режиме повышенной частоты (UsePatternOutput) (Гц)
ULProperty<double, NLogicalNot, ptPubParameter> PatternFrequency;

//Обучаемый нейрон
UEPtr<NNeuronTrainer> NeuronTrainer;

//Выходной нейрон
UEPtr<NPulseNeuron> OutputNeuron;



protected:

// Продолжительность работы генератора в режиме повышенной частоты (UsePatternOutput) (с)
double PatternDuration;

//Нейроны генераторной связки
UEPtr<NPulseNeuron> GeneratorNeuron1;
UEPtr<NPulseNeuron> GeneratorNeuron2;

//Текущее число синапсов на нейронах генераторной связки
 std::vector<UEPtr<NPulseSynapse>> synapses1;
 std::vector<UEPtr<NPulseSynapse>> synapses2;

//Генератор стартового импульса для генераторной связки нейронов
UEPtr<NPulseGeneratorTransit> Generator;

//Обучаемый нейрон
//UEPtr<NNeuronTrainer> NeuronTrainer;

//Генератор, продлевающий тормозящее воздействие NeuronTrainer в схеме
UEPtr<NPulseGeneratorTransit> PatternGenerator;

//Выходной нейрон
//UEPtr<NPulseNeuron> OutputNeuron;



public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLogicalNot(void);
virtual ~NLogicalNot(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Режим расчёта нейрона
bool SetCalculateMode(const int &value);

/// Имя класса, создающего генератор импульсов
bool SetPulseGeneratorClassName(const std::string &value);

/// Имя класса, создающего учитель нейрона
bool SetNeuronTrainerClassName(const std::string &value);

/// Имя класса, создающего модель логического "НЕ" (модель боли)
bool SetLogicalNotClassName(const std::string &value);

/// Имя класса, создающего нейрон
bool SetNeuronClassName(const std::string &value);

/// Имя класса, создающего синапс
bool SetSynapseClassName(const std::string &value);

/// Признак необходимости обучения
bool SetNeedToTrain(const bool &value);

/// Число входных дендритов
bool SetNumInputDendrite(const int &value);

/// Частота генераторов (Гц)
bool SetSpikesFrequency(const double &value);

/// Паттерн для запоминания
bool SetInputPattern(const MDMatrix<double> &value);

/// Длина дендрита на первом нейроне в генераторной связке (число сегментов)
bool SetDendriteLengthNGen1(const int &value);

/// Длина дендрита на втором нейроне в генераторной связке (число сегментов)
bool SetDendriteLengthNGen2(const int &value);

/// Число синапсов на дендрите первого нейрона в генераторной связке
bool SetNumExcSynapsesNGen1(const int &value);

/// Число синапсов на дендрите второго нейрона в генераторной связке
bool SetNumExcSynapsesNGen2(const int &value);

/// Частота генератора в режиме повышенной частоты (UsePatternOutput) (Гц)
bool SetPatternFrequency(const double &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NLogicalNot* New(void);
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
bool BuildStructure(int structure_build_mode, const string &pulse_generator_class_name,
                    const string &neuron_class_name, int num_input_dendrite,
                    double spikes_frequency,
                    int num_exc_synapses_ngen1,int num_exc_synapses_ngen2,
                    int dendrite_length_ngen1, int dendrite_length_ngen2,
                    const MDMatrix<double> &input_pattern);

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
