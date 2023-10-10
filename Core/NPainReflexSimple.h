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

#ifndef NPainReflexSimple_H
#define NPainReflexSimple_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, создающий группу нейронов для моделирования условного рефлекса
class RDK_LIB_TYPE NPainReflexSimple: public UNet
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
ULProperty<int,NPainReflexSimple, ptPubParameter> StructureBuildMode;

/// Имя класса, создающего генератор импульсов
ULProperty<std::string, NPainReflexSimple, ptPubParameter> PulseGeneratorClassName;

/// Имя класса, создающего учитель нейрона
ULProperty<std::string, NPainReflexSimple, ptPubParameter> NeuronTrainerClassName;

/// Имя класса нейрона
ULProperty<std::string, NPainReflexSimple, ptPubParameter> NeuronClassName;

/// Имя класса синапса
ULProperty<std::string, NPainReflexSimple, ptPubParameter> SynapseClassName;

/// Размер "большого" нейрона
//ULProperty<int, NPainReflexSimple, ptPubParameter> BigNeuronSize;

/// Признак необходимости обучения
ULProperty<bool, NPainReflexSimple, ptPubParameter> IsNeedToTrain;

/// Время задержки начала обучения относительно старта системы (сек)
ULProperty<double, NPainReflexSimple, ptPubParameter> Delay;

/// Частота генераторов (Гц)
ULProperty<double, NPainReflexSimple, ptPubParameter> SpikesFrequency;

/// Число входных дендритов для нейрона безусловного раздражителя
//ULProperty<int, NPainReflexSimple, ptPubParameter> NumUnconditionalStimulDendrite;

/// Число входных дендритов для нейрона условного раздражителя
ULProperty<int, NPainReflexSimple, ptPubParameter> NumConditionalStimulDendrite;

/// Максимальная длина дендрита для каждого нейрона
ULProperty<int, NPainReflexSimple, ptPubParameter> MaxDendriteLength;

/// Паттерн для запоминания нейроном безусловного раздражителя.
/// Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
/// Задержки для одного нейрона располагаются в одном СТОЛБЦЕ!!!
//ULProperty<MDMatrix<double>, NPainReflexSimple, ptPubParameter> UnconditionalStimulPattern;

/// Паттерн для запоминания нейроном условного раздражителя.
/// Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
/// Задержки для одного нейрона располагаются в одном СТОЛБЦЕ!!!
//ULProperty<MDMatrix<double>, NPainReflexSimple, ptPubParameter> ConditionalStimulPattern;

/// Характеристика воздействия
/// false - позитивное подкрепление
/// true - негативное подкрепление
ULProperty<bool, NPainReflexSimple, ptPubParameter> IsNegInfluence;

/// Порог низкопороговой зоны нейрона
ULProperty<double, NPainReflexSimple, ptPubParameter> LTZThreshold;

/// Фиксированный порог низкопороговой зоны нейрона
ULProperty<double, NPainReflexSimple, ptPubParameter> FixedLTZThreshold;

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
ULProperty<bool, NPainReflexSimple, ptPubParameter> UseFixedLTZThreshold;

/// "Болевой" вход, если сигнал есть - нейрон активен и будет обучаться
UPropertyInputData<MDMatrix<double>, NPainReflexSimple, ptInput | ptPubState> PainInput;


/// "Нормальный" вход-выход (основной, задающий сигнал)
//UPropertyInputData<MDMatrix<double>, NNeuronTrainer, ptInput | ptPubState> NormalInput;

/// "Альтернативный" вход (болевой сигнал, который будет передаваться, если больно, и
///  блокироваться, если не больно)
//UPropertyInputData<MDMatrix<double>, NNeuronTrainer, ptInput | ptPubState> AlternativeInput;

/// Выход (результат работы) рефлекса - основной
//UPropertyOutputData<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> NormalOutput;

/// Выход (результат работы) рефлекса - болевой, если распознана боль
//UPropertyOutputData<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> PainOutput;

protected:

 // Старое значение числа входных нейронов
 //int OldNumNeurons;
 // Старое значение числа входных дендритов
 //int OldNumInputDendrite;

 // Безусловный раздражитель
 //UEPtr<NNeuronTrainer> UnconditionalStimul;
 // Условный раздражитель
 UEPtr<NNeuronTrainer> ConditionalStimul;
 // Нейрон, реализующий функцию "И"
 //UEPtr<NNeuronTrainer> LogicalAndNeuron;
 // "Большой" нейрон
 //UEPtr<NPulseNeuron> BigNeuron;

 // Нейрон, пропускающий/блокирующий "нормальный" выход
 UEPtr<NPulseNeuron> NormalOutputNeuron;

 // Нейрон, пропускающий/блокирующий "альтернативный" выход
 UEPtr<NPulseNeuron> AltOutputNeuron;

 //Генератор, блокирующий NormalOutputNeuron серией  импульсов, если есть выход с условного раздражителя
 UEPtr<NPulseGeneratorTransit> NormalBlocker;

 //Генератор, блокирующий PainOutputNeuron серией  импульсов, если есть выход с "нормального" нейрона
 UEPtr<NPulseGeneratorTransit> AltBlocker;

 //Транзитный генератор, к которому подключается "нормальный" вход (основной)
 UEPtr<NPulseGeneratorTransit> NormalInputGen;

 //Транзитный генератор, к которому подключается "альтернативный" вход (болевой)
 UEPtr<NPulseGeneratorTransit> AltInputGen;

 // Флаги, определяющие стадию формирования рефлекса
 bool is_conditional_stimulus_trained; //Признак завершения обучения условному раздражителю

 /*
 // Флаги, определяющие стадию формирования рефлекса
 bool is_stimuls_trained;   	// Признак завершения обучения условному и безусловному раздражителям
 bool is_interval_found;		// Признак нахождения разности в приходе импульсов от условного и безусловного раздражителей
 bool is_and_neuron_trained;	// Признак завершения обучения нейрона "И"
 bool is_big_neuron_trained;	// Признак завершения обучения большого нейрона

 // Время между спайками, приходящими на нейрон, реализующий функцию "И"
 double first_spike_time;	// От условного раздражителя
 bool is_first_spike;		// Спайк пришёл?
 double second_spike_time;  // От безусловного раздражителя
 bool is_second_spike;		// Спайк пришёл?
 double difference;			// Разность
 // Время для организации Delay для нейрона, реализующего функцию "И"
 double ready_time;

 // Признак одного спайка при наращивании синапсов на "большом" нейроне
 bool one_spike;
 // Текущее число синапсов на "большом" нейроне
 int synapses_num;
 */

 double start_iter_time;
 bool is_first_iter;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPainReflexSimple(void);
virtual ~NPainReflexSimple(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса, создающего генератор импульсов
bool SetPulseGeneratorClassName(const std::string &value);

/// Имя класса, создающего учитель нейрона
bool SetNeuronTrainerClassName(const std::string &value);

/// Имя класса, создающего нейрон
bool SetNeuronClassName(const std::string &value);

/// Имя класса, создающего синапс
bool SetSynapseClassName(const std::string &value);

// Размер "большого" нейрона
//bool SetBigNeuronSize(const int &value);

/// Признак необходимости обучения
bool SetNeedToTrain(const bool &value);

/// Время задержки начала обучения относительно старта системы (сек)
bool SetDelay(const double &value);

/// Частота генераторов (Гц)
bool SetSpikesFrequency(const double &value);

/// Число входных дендритов для нейрона безусловного раздражителя
//bool SetNumUnconditionalStimulDendrite(const int &value);

/// Число входных дендритов для нейрона условного раздражителя
bool SetNumConditionalStimulDendrite(const int &value);

/// Максимальная длина дендрита
bool SetMaxDendriteLength(const int &value);

/// Паттерн для запоминания  нейроном безусловного раздражителя
//bool SetUnconditionalStimulPattern(const MDMatrix<double> &value);

/// Паттерн для запоминания  нейроном условного раздражителя
bool SetConditionalStimulPattern(const MDMatrix<double> &value);

/// Характеристика воздействия
bool SetNegInfluence(const bool &value);

/// Порог низкопороговой зоны нейрона
bool SetLTZThreshold(const double &value);

/// Фиксированный порог низкопороговой зоны нейрона
bool SetFixedLTZThreshold(const double &value);

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool SetUseFixedLTZThreshold(const bool &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPainReflexSimple* New(void);
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
bool BuildStructure(void);

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
