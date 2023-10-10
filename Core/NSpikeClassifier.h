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

#ifndef NSPIKECLASSIFIER_H
#define NSPIKECLASSIFIER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// Класс, создающий группу обученных нейронов для распознавания заданного паттерна импульсов
class RDK_LIB_TYPE NSpikeClassifier: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// 0 - автоматическая сборка не производится
/// 1 - создаём группу нейронов, структурно адаптирующихся для распознавания заданного паттерна
/// Нейроны связываются каждый с каждым по принципу "выход n-ого нейрона -> тормозный вход m-ого нейрона" n != m
/// На вход обученной группе подаётся паттерн. Группа осуществляет его распознавание.
/// Признаком распознавания является активность одного (или нескольких) из обученных нейронов
/// Результатом распознавания является паттерн(-ы), по которому обучался активный нейрон
ULProperty<int,NSpikeClassifier, ptPubParameter> StructureBuildMode;

/// Имя класса, создающего генератор импульсов
ULProperty<std::string, NSpikeClassifier, ptPubParameter> PulseGeneratorClassName;

/// Имя класса, создающего учитель нейрона
ULProperty<std::string, NSpikeClassifier, ptPubParameter> NeuronTrainerClassName;

/// Имя класса нейрона
ULProperty<std::string, NSpikeClassifier, ptPubParameter> NeuronClassName;

/// Имя класса синапса
ULProperty<std::string, NSpikeClassifier, ptPubParameter> SynapseClassName;

/// Число нейронов
ULProperty<int, NSpikeClassifier, ptPubParameter> NumNeurons;

/// Признак необходимости обучения
ULProperty<bool, NSpikeClassifier, ptPubParameter> IsNeedToTrain;

/// Время задержки начала обучения относительно старта системы (сек)
ULProperty<double, NSpikeClassifier, ptPubParameter> Delay;

/// Частота генераторов (Гц)
ULProperty<double, NSpikeClassifier, ptPubParameter> SpikesFrequency;

/// Число входных дендритов для каждого нейрона
ULProperty<int, NSpikeClassifier, ptPubParameter> NumInputDendrite;

/// Максимальная длина дендрита для каждого нейрона
ULProperty<int, NSpikeClassifier, ptPubParameter> MaxDendriteLength;

/// Паттерны для запоминания нейронами. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
/// Задержки для одного нейрона располагаются в одной СТРОКЕ!!!
ULProperty<MDMatrix<double>, NSpikeClassifier, ptPubParameter> TrainingPatterns;

/// Паттерн для распознавания. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
/// Задержки паттерна для распознавания располагаются в одном СТОЛБЦЕ!!!
ULProperty<MDMatrix<double>, NSpikeClassifier, ptPubParameter> InputPattern;

/// Порог низкопороговой зоны нейрона
ULProperty<double, NSpikeClassifier, ptPubParameter> LTZThreshold;

/// Фиксированный порог низкопороговой зоны нейрона
ULProperty<double, NSpikeClassifier, ptPubParameter> FixedLTZThreshold;

/// Порог низкопороговой зоны нейрона для обучения
ULProperty<double, NSpikeClassifier, ptPubParameter> TrainingLTZThreshold;

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
ULProperty<bool, NSpikeClassifier, ptPubParameter> UseFixedLTZThreshold;

/// Признак необходимости транзита сигнала от внешнего источника
ULProperty<bool,NSpikeClassifier, ptPubParameter> UseTransitSignal;

/// Признак необходимости чтения и записи данных из файла
ULProperty<bool, NSpikeClassifier, ptPubParameter> DataFromFile;


protected:

 // Старое значение числа нейронов
 int OldNumNeurons;
 // Старое значение числа входных дендритов
 int OldNumInputDendrite;

 // Генераторы импульсов
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;
 // Обученные (обучающиеся) нейроны
 std::vector<UEPtr<NNeuronTrainer> > trainers;

 // Переменные для чтение и записи данных в файл
 // ------->
 // Признак первой итерации
 bool IsFirstFileStep;
 // Файл с входными данными
 ifstream fin;
 // Файл с выходными данными
 ofstream fout;
 // Входной паттерн импульсов
 MDMatrix<double> inputs;
 // Ответы нейронов
 std::vector<int> outputs;
 // Флаг начала итерации
 bool is_first_iter;
 // Время начала текущей итерации (сек)
 double start_iter_time;
 // <-------


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSpikeClassifier(void);
virtual ~NSpikeClassifier(void);
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

/// Число нейронов
bool SetNumNeurons(const int &value);

/// Признак необходимости обучения
bool SetNeedToTrain(const bool &value);

/// Время задержки начала обучения относительно старта системы (сек)
bool SetDelay(const double &value);

/// Частота генераторов (Гц)
bool SetSpikesFrequency(const double &value);

/// Число входных дендритов
bool SetNumInputDendrite(const int &value);

/// Максимальная длина дендрита
bool SetMaxDendriteLength(const int &value);

/// Паттерны для запоминания
bool SetTrainingPatterns(const MDMatrix<double> &value);

/// Паттерн для распознавания.
bool SetInputPattern(const MDMatrix<double> &value);

/// Порог низкопороговой зоны нейрона
bool SetLTZThreshold(const double &value);

/// Фиксированный порог низкопороговой зоны нейрона
bool SetFixedLTZThreshold(const double &value);

/// Порог низкопороговой зоны нейрона для обучения
bool SetTrainingLTZThreshold(const double &value);

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool SetUseFixedLTZThreshold(const bool &value);

// Устанавливает необходимость транзита сигнала от внешнего источника
bool SetUseTransitSignal(const bool &value);

// Устанавливает необходимости чтения и записи данных из файла
bool SetDataFromFile(const bool &value);
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSpikeClassifier* New(void);
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

// Функция для работы с файлами.
// Осуществляет чтение входных данных из файла,
// Обработку результатов и запись результатов в файл
bool TreatDataFromFile(void);

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

// Устанавливает компоненты в требуемый порядок расчета
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif
