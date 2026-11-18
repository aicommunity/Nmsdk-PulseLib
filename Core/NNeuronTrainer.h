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

#ifndef NNEURONTRAINER_H
#define NNEURONTRAINER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// Класс, создающий нейрон со структурой, оптимизованной для распознавания заданного паттерна импульсов
class RDK_LIB_TYPE NNeuronTrainer: public UNet
{
public: // 
/// Режим сборки структуры нейрона
/// 0 - автоматическая сборка не производится
/// 1 - создаём нейрон, настраивающий длину своих дендритов для максимального отклика по амплетуде на заданный паттерн импульсов
/// создаётся нейрон, набор генераторов и дендритов, связывается _один_ генератор с дендритом, сигнал на который приходит позже всего
UProperty<int,NNeuronTrainer, ptPubParameter> StructureBuildMode;

/// Режим рвсчёта обучения нейрона
/// 0 - расчёт не производится
/// 1 - обучение путём наращивания длины дендритов путём максимизации амплитуды нейрона
/// 2 (НЕ ОТЛАЖЕНО!!!) - обучение путём наращивания длины дендритов по амплитуде и количества синапсов на дендрите
/// 3 - обучение путём наращивания длины дендритов путём максимального выравнивания времени прихода сигналов на сому
/// 4 - обучение путём наращивания длины дендритов по времени и количества синапсов на дендрите
/// 5 - автоматический подбор порога срабатывания нейрона для каждого автоматичеки сгенерированного паттерна

UProperty<int,NNeuronTrainer, ptPubParameter> CalculateMode;

/// Имя класса, создающего генератор импульсов
UProperty<std::string, NNeuronTrainer, ptPubParameter> PulseGeneratorClassName;

/// Имя класса, создающего нейрон
UProperty<std::string, NNeuronTrainer, ptPubParameter> NeuronClassName;

/// Имя класса синапса
UProperty<std::string, NNeuronTrainer, ptPubParameter> SynapseClassName;

/// Признак необходимости обучения
UProperty<bool, NNeuronTrainer, ptPubParameter> IsNeedToTrain;

/// Время задержки начала обучения относительно старта системы (сек)
UProperty<double, NNeuronTrainer, ptPubParameter> Delay;

/// Частота генераторов (Гц)
UProperty<double, NNeuronTrainer, ptPubParameter> SpikesFrequency;

/// Число входных дендритов
UProperty<int, NNeuronTrainer, ptPubParameter> NumInputDendrite;

/// Максимальная длина дендрита
UProperty<int, NNeuronTrainer, ptPubParameter> MaxDendriteLength;

/// Паттерн для запоминания. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
UProperty<MDMatrix<double>, NNeuronTrainer, ptPubParameter> InputPattern;

/// Максимальная суммарная амплитуда нейрона
// 0 - DendriticSumPotential - сумарная амплитуда всех дендритов
// N - выход N-ого дендрита
UProperty<MDMatrix<double>,NNeuronTrainer,ptOutput | ptPubState> NeuronAmplitude;

UProperty<MDMatrix<double>,NNeuronTrainer,ptOutput | ptPubState> SomaNeuronAmplitude;

/// Порог низкопороговой зоны нейрона
UProperty<double, NNeuronTrainer, ptPubParameter> LTZThreshold;

/// Фиксированный порог низкопороговой зоны нейрона
UProperty<double, NNeuronTrainer, ptPubParameter> FixedLTZThreshold;

/// Порог низкопороговой зоны нейрона для обучения
UProperty<double, NNeuronTrainer, ptPubParameter> TrainingLTZThreshold;

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
UProperty<bool, NNeuronTrainer, ptPubParameter> UseFixedLTZThreshold;

/// Выход нейрона
UProperty<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> Output;

/// Сопротивление нарастающих синапсов
UProperty<double, NNeuronTrainer, ptPubParameter> SynapseResistanceStep;

/// Флаг включения DEBUG-логирования работы тренера:
/// при активации выводит подробную информацию о процессе обучения.
UProperty<bool, NNeuronTrainer, ptPubParameter> EnableDebug;


protected:

 // Старое значение числа входных дендритов
 int OldNumInputDendrite;

 // Генераторы импульсов
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;
 // Настраиваемый нейрон
 UEPtr <NPulseNeuron> neuron;


 // Флаг первой итерации (необходим для замера времени начала расчёта)
 bool is_first_iter;
 // Время начала текущей итерации (старт роста дендрита) (сек)
 double start_iter_time;

 // Максимальная амплитуда выходного сигнала на одной итерации
 double max_iter_neuron_amp;
 // Максимальная суммарная амплитуда нейрона  и время её замера
 double max_neuron_amp;
 double max_neuron_amp_time;
 // Массив длин дендритов
 std::vector<int> DendriteLength;
 // Массив выходов дендритов при единичной их длине
 std::vector<double> InitialDendritePotential;
 // Массив количества синапсов на дендрите
 std::vector<int> SynapseNum;

 // Обрабатываемый по счёту дендрит
 int dend_counter;
 // Индекс обрабатываемого дендрита
 int dend_index;
 // Признак расчёта нового дендрита
 bool is_new_dend;
 // Признак новой итерации (добавления участка дендрита)
 bool is_new_iteration;
 // Обрабатываемый по счёту синапс = индекc+1 дендрита, на котором растим синапсы
 int syn_counter;

 bool thresh_first_iter;
 ////////////////////////////////////////////
 // Параметры для синхронизации по времени
 /////////////////////////////////////////////
 // Флаг необходимости досабрать нейрон
 bool is_need_to_build;
 // Максимальная амплитуда выходного сигнала дендритов на одной итерации
 std::vector<double> max_iter_dend_amp;
 // Время нахождения максимальной амплитуды выходного сигнала дендритов на одной итерации
 std::vector<double> max_dend_amp_time;
 // Текущая рассинхронизация дендрита
 std::vector<double> dissynchronization;
 // Статус дендритов:
 // 0 - длина дендрита сформирована
 // 1 - необходим рост
 // -1 - необходимо укорочение
 // 2 - добавить синапс
 // -2 - удалить синапс
 std::vector<int> dend_status;
 // Признак завершения синхронизации
 bool is_synchronizated;


public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronTrainer(void);
virtual ~NNeuronTrainer(void);
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

/// Имя класса, создающего нейрон
bool SetNeuronClassName(const std::string &value);

/// Имя класса синапса
bool SetSynapseClassName(const std::string &value);

/// Время задержки начала обучения относительно старта системы (сек)
bool SetDelay(const double &value);

/// Частота генераторов (Гц)
bool SetSpikesFrequency(const double &value);

/// Число входных дендритов
bool SetNumInputDendrite(const int &value);

/// Максимальная длина дендрита
bool SetMaxDendriteLength(const int &value);

/// Паттерн для запоминания
bool SetInputPattern(const MDMatrix<double> &value);

/// Признак необходимости обучения
bool SetNeedToTrain(const bool &value);

/// Порог низкопороговой зоны нейрона
bool SetLTZThreshold(const double &value);

/// Порог низкопороговой зоны нейрона для обучения
bool SetTrainingLTZThreshold(const double &value);

/// Фиксированный порог низкопороговой зоны нейрона
bool SetFixedLTZThreshold(const double &value);

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool SetUseFixedLTZThreshold(const bool &value);

/// Сопротивление нарастающих синапсов
bool SetSynapseResistanceStep(const double &value);

/// Установка флага включения DEBUG-логирования обучения
bool SetEnableDebug(const bool &value);


// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NNeuronTrainer* New(void);
static UComponent* NewStatic(void);
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
 /// Safely returns LTZone for the current neuron.
 UEPtr<NLTZone> GetTrainerLTZone(void) const;

 /// Applies LTZ threshold consistently to property and LTZone.
 bool ApplyLTZThresholdValue(const double value, const bool update_fixed_usage_flag);

 /// Canonical component naming helpers used in training logic.
 static std::string MakeSourceName(const int index1);
 static std::string MakeSomaName(const int index1);
 static std::string MakeDendriteName(const int dendrite_index1, const int segment_index1);
 static std::string MakeExcSynapsePath(const int dendrite_index1, const int segment_index1, const int synapse_index1);

 /// Computes safe iteration length for training loop timing.
 double GetSafeIterationLength(void) const;

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
					const string &neuron_class_name,
					int old_num_input_dendrite, int num_input_dendrite,
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

// Выполняется обучение путём наращивания длины дендритов
// путём максимизации амплитуды нейрона
//bool MaximazeAmplitude(void);
// Выполняется обучение путём наращивания длины дендритов
// путём синхронизации входного паттерна на входе сомы
//bool SynchronizePattern(void);
// Добавляем синапсы для компенсации потерь амплитуды потенциала на входе сомы при наращивании
//bool SynapseNormalization(void);

// Выполняется обучение путём наращивания длины дендритов
// путём синхронизации на выходе сомы
bool SomaSynchronizePattern(void);
// Добавляем синапсы для компенсации потерь амплитуды на выходе сомы при наращивании дендритов
bool SomaSynapseNormalization(void);

// Процесс расчёта
bool CalculateProcess(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);

// Устанавливает компоненты в требуемый порядок расчета
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif
