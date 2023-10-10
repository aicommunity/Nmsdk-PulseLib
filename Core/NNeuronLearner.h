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

#ifndef NNEURONLEARNER_H
#define NNEURONLEARNER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNeuronLearner: public UNet
{
public:
 /// Режим сборки структуры нейрона
 /// 0 - автоматическая сборка не производится
 /// 1 - автоматически собирается нейрон, способный обучаться на каждом входном паттерне импульсов за счёт изменения внутренней структуры
 ULProperty<int, NNeuronLearner, ptPubParameter> StructureBuildMode;

 /// Имя класса генератора импульсов
 ULProperty<std::string, NNeuronLearner, ptPubParameter> PulseGeneratorClassName;

 /// Имя класса нейрона
 ULProperty<std::string, NNeuronLearner, ptPubParameter> NeuronClassName;

 /// Имя класса синапсов
 ULProperty<std::string, NNeuronLearner, ptPubParameter> SynapseClassName;

 /// Режим работы (обучение, распознование) нейрона:
 /// при поднятом флаге IsNeedToTrain происходит обучение, иначе - распознавание
 /// 0 - аналог работы NeuronTrainer: после того, как нейрон изучил пример,
 /// автоматически опускается флаг IsNeedToTrain, затем происходит распознавание
 /// 1 - флаг IsNeedToTrain автоматически не опускается, режим предполагает внешнее управление флагом
 ULProperty<int, NNeuronLearner, ptPubParameter> CalculateMode;

 /// Флаг необходимости обучения некоторому паттерну
 ULProperty<bool, NNeuronLearner, ptPubParameter> IsNeedToTrain;

 /// Время задержки начала обучения относительно старта системы (сек)
 ULProperty<double, NNeuronLearner, ptPubParameter> Delay;

 /// Частота генераторов (Гц)
 ULProperty<double, NNeuronLearner, ptPubParameter> SpikesFrequency;

 /// Число входных дендритов
 ULProperty<int, NNeuronLearner, ptPubParameter> NumInputDendrite;

 /// Максимальная длина дендрита
 ULProperty<int, NNeuronLearner, ptPubParameter> MaxDendriteLength;

 /// Паттерн для запоминания, закодированный по TTFS - содержит запаздывания входных сигналов для каждого дендрита
 ULProperty<MDMatrix<double>, NNeuronLearner, ptPubParameter> InputPattern;

 /// Дополнительный паттерн, который изучается в экспериментах для получения промежуточных структур
 ULProperty<MDMatrix<double>, NNeuronLearner, ptPubParameter> AdditionalInputPattern;

 /// Паттерн импульсов, который был на предыдущей итерации обучения
 MDMatrix<double> PrevInputPattern;

 /// Максимальное значение потенциала в нейроне, снятого на выходах первых сегментов дендритов
 /// 0 - DendriticSumPotential - сумарное значение максимумов потенциалов со всех дендритов
 /// i = 1..N - значение максимума потенциала на выходе первого сегмента i-ого дендрита
 UPropertyOutputData<MDMatrix<double>,NNeuronLearner,ptOutput | ptPubState> DendriteNeuronAmplitude;

 /// Максимальное значение потенциала в нейроне, снятого на выходах сегментов сомы
 /// 0 - сумарное значение максимумов потенциалов со всех сегментов сомы
 /// i = 1..N - значение максимума потенциала на выходе i-ого сегмента сомы
 UPropertyOutputData<MDMatrix<double>,NNeuronLearner,ptOutput | ptPubState> SomaNeuronAmplitude;

 /// Используемый порог низкопороговой зоны нейрона
 ULProperty<double, NNeuronLearner, ptPubParameter> LTZThreshold;

 /// Фиксированное значение (значение по умолчанию) порога низкопороговой зоны нейрона
 ULProperty<double, NNeuronLearner, ptPubParameter> FixedLTZThreshold;

 /// Значение порога низкопороговой зоны нейрона, устанавливаемое на время обучения
 ULProperty<double, NNeuronLearner, ptPubParameter> TrainingLTZThreshold;

 /// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
 ULProperty<bool, NNeuronLearner, ptPubParameter> UseFixedLTZThreshold;

 /// Выход нейрона
 UPropertyOutputData<MDMatrix<double>, NNeuronLearner, ptOutput | ptPubState> Output;

 /// Сопротивление добавляемых в процессе обучения возбуждающих синапсов
 ULProperty<double, NNeuronLearner, ptPubParameter> SynapseResistanceStep;

 /// Массив количеств сегментов в дендритах
 ULProperty<std::vector<int>, NNeuronLearner, ptPubParameter> DendriteLength;

 /// Массив количеств сегментов в дендритах, которые были до изменения.
 /// Параметр необходим, чтобы удалить лишние синапсы на сегментах дендритов,
 /// которые раньше были входными
 std::vector<int> OldDendriteLength;

 /// Массив значений максимумов потенциалов на выходах сегментов сомы, когда нейрон находится в исходном состоянии,
 /// то есть все дендриты состоят из одного сегмента и имеют только по одному тормозному и возбуждающему синапсу
 ULProperty<std::vector<double>, NNeuronLearner, ptPubParameter> InitialSomaPotential;

 /// Массив количеств возбуждающих синапсов на дендритах
 ULProperty<std::vector<int>, NNeuronLearner, ptPubParameter> NumSynapse;

 /// Номер эксперимента
 /// 2 - LearningSecondPattern() - изучение доп. паттерна
 ULProperty<int, NNeuronLearner, ptPubParameter> ExperimentNum;

 /// Режим проведения экспериментов
 ULProperty<bool, NNeuronLearner, ptPubParameter> ExperimentMode;

protected:
 /// Старое значение числа входных дендритов
 int OldNumInputDendrite;

 /// Генераторы импульсов
 std::vector<UEPtr<NPulseGeneratorTransit> > Generators;

 /// Настраиваемый нейрон
 UEPtr <NPulseNeuron> Neuron;

 /// Время начала текущей итерации (старт роста дендрита) (сек)
 double StartIterTime;


 // Параметры для работы с файлом

 /// Файл с входными данными
 ifstream Fin;

 /// Файл с выходными данными
 ofstream Fout;

 /// Входные данные из файла
 MDMatrix<double> Inputs;

 /// Выходные данные
 std::vector<int> Outputs;

 /// Выходные данные уже записаны в файл
 bool IsWritten;

 /// Флаг начала чтения информации из файла
 bool IsFirstFileStep;

 /// Счётчик итераций. Показывает кол-во итераций, на протяжении которых
 /// нейрон работает с текущим паттерном
 int CountIteration;


 // Параметры для синхронизации по времени

 /// Флаг первого такта в рамках одной итерации обучения нейрона
 /// На первом такте происходит замер времени длительности текущей итерации
 /// и изменение структуры нейрона
 bool IsFirstBeat;

 /// Временная длительность итерации
 double IterLength;

 /// Максимальные значения потенциалов на выходах сегментов сомы,
 /// измеренные в течение одной итерации
 std::vector<double> MaxIterSomaAmp;

 /// Время фиксации максимумов значений потенциалов на выходах сегментов сомы
 std::vector<double> TimeOfMaxIterSomaAmp;

 /// Текущая рассинхронизация сигналов на сомах - разница во времени
 /// для каждой сомы между максимумом амплитуды на ней и на калибровочной
 std::vector<double> Dissynchronization;

 /// Текущая разница в максимумах значений потенциалов на выходах сегментов сомы
 /// на данной итерации с их значениями при исходном состоянии нейрона
 std::vector<double> AmpDifference;

 /// Статусы дендритов - информация о том, как изменять число сегментов в дендритах:
 /// 0  - длина дендрита сформирована;
 /// 1  - необходимо увеличение числа сегментов в дендрите;
 /// -1 - необходимо уменьшение числа сегментов в дендрите
 std::vector<int> DendStatus;

 /// Статусы синапсов - информация о том, как изменять число возб. синапсов в дендритах:
 /// 0  - количество синапсов сформировано;
 /// 1  - необходимо увеличение числа синапсов на входном сегменте дендрита;
 /// -1 - необходимо уменьшение числа синапсов на входном сегменте дендрита
 std::vector<int> SynapseStatus;


 // Параметры для экспериментов

 /// Текущая эпоха обучения. Эпоха обучения - итерация обучения какому-либо примеру,
 /// которая может состоять из нескольких стандартных итераций обучения
 int EpochCur;

 /// Флаг, позволяющий менять длину дендрита
 bool CanChangeDendLength;

 /// Сумма максимальных значений потенциалов на выходах сегментов сомы у двух паттернов
 double SumMaxIterSomaAmp;

 /// Сумма максимальных значений потенциалов на выходах сегментов сомы у двух паттернов
 /// на при предыдущей структуре
 double PrevSumMaxIterSomaAmp;


public: // Методы
 // --------------------------
 // Конструкторы и деструкторы
 // --------------------------

 NNeuronLearner(void);
 virtual ~NNeuronLearner(void);

 // --------------------------



 // --------------------------
 // Методы упраления параметрами
 // --------------------------

 /// Установка режима сборки структуры нейрона
 bool SetStructureBuildMode(const int &value);

 /// Установка имени класса генераторов импульсов
 bool SetPulseGeneratorClassName(const std::string &value);

 /// Установка имени класса нейрона
 bool SetNeuronClassName(const std::string &value);

 /// Установка имени класса синапсов
 bool SetSynapseClassName(const std::string &value);

 /// Установка режима работы (обучение, распознование) нейрона:
 /// при поднятом флаге IsNeedToTrain происходит обучение, иначе - распознавание
 /// 0 - аналог работы NeuronTrainer: после того, как нейрон изучил пример,
 /// автоматически опускается флаг IsNeedToTrain, затем происходит распознавание
 /// 1 - флаг IsNeedToTrain автоматически не опускается, режим предполагает внешнее управление флагом
 bool SetCalculateMode(const int &value);

 /// Установка (поднятие или опускание) флага необходимости обучения некоторому паттерну
 bool SetIsNeedToTrain(const bool &value);

 /// Установка времени задержки начала обучения относительно старта системы (сек)
 bool SetDelay(const double &value);

 /// Установка частоты генераторов (Гц)
 bool SetSpikesFrequency(const double &value);

 /// Установка числа входных дендритов
 bool SetNumInputDendrite(const int &value);

 /// Установка значения максимальной длины дендритов (= числа сегментов в дендритах)
 bool SetMaxDendriteLength(const int &value);

 /// Установка паттерна для запоминания, закодированного по TTFS (содержит запаздывания входных сигналов для каждого дендрита)
 bool SetInputPattern(const MDMatrix<double> &value);

 /// Установка дополнительного паттерна, который изучается в экспериментах для получения промежуточных структур
 bool SetAdditionalInputPattern(const MDMatrix<double> &value);

 /// Паттерн импульсов, который был на предыдущей итерации обучения
 bool SetPrevInputPattern(const MDMatrix<double> &value);

 /// Установка используемого порога низкопороговой зоны нейрона
 bool SetLTZThreshold(const double &value);

 /// Установка значения порога низкопороговой зоны нейрона, устанавливаемого на время обучения
 bool SetTrainingLTZThreshold(const double &value);

 /// Установка фиксированного значения (значения по умолчанию) порога низкопороговой зоны нейрона
 bool SetFixedLTZThreshold(const double &value);

 /// Установка признака необходимости использования фиксированного порога низкопороговой зоны нейрона
 bool SetUseFixedLTZThreshold(const bool &value);

 /// Установка значения сопротивления добавляемых в процессе обучения возбуждающих синапсов
 bool SetSynapseResistanceStep(const double &value);

 /// Установка режима проведения экспериментов
 bool SetExperimentMode(const bool &value);

 /// Установка значения массива длин дендритов (количеств сегментов в дендритах)
 bool SetDendriteLength(const std::vector<int> &value);

 /// Массив значений максимумов потенциалов на выходах сегментов сомы,
 /// когда нейрон находится в исходном состоянии, то есть все дендриты
 /// состоят из одного сегмента и имеют только по одному тормозному и возбуждающему синапсу
 bool SetInitialSomaPotential(const std::vector<double> &value);

 /// Установка значения массива количеств возбуждающих синапсов на дендритах
 bool SetNumSynapse(const std::vector<int> &value);

 /// Установка номера эксперимента
 /// 1 - PatternRecognition()
 /// 2 - LearningSecondPattern()
 bool SetExperimentNum(const int &value);

 // --------------------------



 // --------------------------
 // Системные методы управления объектом
 // --------------------------

 /// Выделяет память для новой чистой копии объекта этого класса
 virtual NNeuronLearner* New(void);
 static UComponent* NewStatic(void);

 // --------------------------



protected:
 // --------------------------
 // Скрытые методы управления компонентами
 // --------------------------

 /// Выполняет завершающие пользовательские действия
 /// при добавлении дочернего компонента в этот объект
 // Метод будет вызван только если comp был успешно добавлен в список компонент
 virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

 /// Выполняет предварительные пользовательские действия
 /// при удалении дочернего компонента из этого объекта
 // Метод будет вызван только если comp существует в списке компонент
 virtual bool ADelComponent(UEPtr<UContainer> comp);

 // --------------------------



protected:
 // --------------------------
 // Скрытые методы управления счетом
 // --------------------------

 /// Осуществляет сборку структуры в соответствии с выбранными значениями параметров
 bool BuildStructure();

 /// Восстановление настроек по умолчанию и сброс процесса счета
 virtual bool ADefault(void);

 /// Обеспечивает сборку внутренней структуры объекта после настройки параметров
 // Автоматически вызывает метод Reset() и выставляет Ready в true
 // в случае успешной сборки
 virtual bool ABuild(void);

 /// Сброс процесса счета
 virtual bool AReset(void);

 /// Сравнивает поэлементно значения входных паттернов
 /// prev_input_pattern - входной паттерн на предыдущей итерации обучения
 /// input_pattern - входной паттерн на текущей итерации обучения
 /// e - допустимая погрешность различия элементов, при непревышении которой элементы считаются одинаковыми
 bool CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e);

 /// Обнуление информации о паттерне, которому обучен нейрон, длинах дендритов и числе синапсов
 bool ZeroingTrainingPattern(void);

 /// Изменение длины дендрита (числа сегментов в дендрите) num
 bool ChangeDendriteLength(int num);

 /// Изменение числа возбуждающих синапсов на входном сегменте дендрита num
 bool ChangeSynapseNumber(int num);

 /// Измерение значение потенциала на выходе сегментов сомы. Вызывается на каждом такте,
 /// чтобы определить максимальное значение потенциала и время его фиксации
 /// в течение текущей итерации
 bool MeasureMaxPotentialAndTime(void);

 /// Изменение статусов дендритов для изменения структуры на следующей итерации.
 /// Вызывается в конце каждой итерации
 bool ChangeDendriteStatus(int num);

 /// Изменение статусов синапсов для изменения структуры на следующей итерации.
 /// Вызывается в конце каждой итерации
 bool ChangeSynapseStatus(int num);

 /// Функция, реализующая распознавание выборки паттернов из входного файла
 bool PatternRecognition(void);

 /// Код, реализующий обучение по двум примерам
 /// До использования функции нейрон полностью обучается первому примеру. Далее в ф-ии последовательно изучает второй.
 /// Каждая эпоха обучения строится следующим образом: на первой итерации эпохи происходит изменение числа сегментов в дендритах и
 /// числа синапсов, на других 4-х итерациях происходит изменение только числа синапсов, чтобы в конце каждой эпохи
 /// число синапсов соответствовало числу сегментов в деднритах
 bool LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern);

 /// Реализуется инкрментное обучение:
 /// нейрон изучает дополнительный паттерн, ища оптимальную структуру по заданному критерию
 bool IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern);

 /// Функция для проведения экспериментов с моделью, в т.ч. происходит работа с файлами
 /// 1 эксперимент - PatternRecognition() - распознавание выборки из файла
 /// 2 эксперимент - LearningAdditionalPattern_1_4() - обучение дополнительному примеру
 bool Experiment(void);

 /// Завершающие обучение действия
 bool EndOfLearning(void);

 /// Часть процесса расчёта NeuronLearner - обучение модели
 bool Training(void);

 /// Выполняет расчёт NeuronLearner
 virtual bool ACalculate(void);

 // --------------------------
};
}

#endif // NNEURONLEARNER_H
