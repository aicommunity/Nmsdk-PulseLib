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

#ifndef NNEURONLEARNER_CPP
#define NNEURONLEARNER_CPP

#define NOMINMAX
#include <algorithm>
#include "NNeuronLearner.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"


namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------

NNeuronLearner::NNeuronLearner(void):
 StructureBuildMode("StructureBuildMode",this,&NNeuronLearner::SetStructureBuildMode),
 PulseGeneratorClassName("PulseGeneratorClassName",this,&NNeuronLearner::SetPulseGeneratorClassName),
 NeuronClassName("NeuronClassName",this,&NNeuronLearner::SetNeuronClassName),
 SynapseClassName("SynapseClassName",this,&NNeuronLearner::SetSynapseClassName),
 CalculateMode("CalculateMode",this, &NNeuronLearner::SetCalculateMode),
 IsNeedToTrain("IsNeedToTrain",this,&NNeuronLearner::SetIsNeedToTrain),
 Delay("Delay",this,&NNeuronLearner::SetDelay),
 SpikesFrequency("SpikesFrequency",this,&NNeuronLearner::SetSpikesFrequency),
 NumInputDendrite("NumInputDendrite",this,&NNeuronLearner::SetNumInputDendrite),
 MaxDendriteLength("MaxDendriteLength",this,&NNeuronLearner::SetMaxDendriteLength),
 InputPattern("InputPattern",this,&NNeuronLearner::SetInputPattern),
 AdditionalInputPattern("AdditionalInputPattern",this,&NNeuronLearner::SetAdditionalInputPattern),
 DendriteNeuronAmplitude("DendriteNeuronAmplitude",this),
 SomaNeuronAmplitude("SomaNeuronAmplitude",this),
 LTZThreshold("LTZThreshold",this,&NNeuronLearner::SetLTZThreshold),
 FixedLTZThreshold("FixedLTZThreshold",this,&NNeuronLearner::SetFixedLTZThreshold),
 TrainingLTZThreshold("TrainingLTZThreshold",this,&NNeuronLearner::SetTrainingLTZThreshold),
 UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NNeuronLearner::SetUseFixedLTZThreshold),
 Output("Output",this),
 SynapseResistanceStep("SynapseResistanceStep", this, &NNeuronLearner::SetSynapseResistanceStep),
 DendriteLength("DendriteLength", this, &NNeuronLearner::SetDendriteLength),
 InitialSomaPotential("InitialSomaPotential", this, &NNeuronLearner::SetInitialSomaPotential),
 NumSynapse("NumSynapse", this, &NNeuronLearner::SetNumSynapse),
 ExperimentNum("ExperimentNum", this, &NNeuronLearner::SetExperimentNum),
 ExperimentMode("ExperimentMode", this, &NNeuronLearner::SetExperimentMode)
{
 OldNumInputDendrite = 0;
 Generators.clear();
 Neuron = NULL;

 // Флаг необходимости открыть файл с входными данными
 IsFirstFileStep = true;
}


NNeuronLearner::~NNeuronLearner(void)
{
 OldNumInputDendrite = 0;
}

// --------------------------



// --------------------------
// Методы упраления параметрами
// --------------------------

/// Установка режима сборки структуры нейрона
bool NNeuronLearner::SetStructureBuildMode(const int &value)
{
 if(value > 0)
  Ready = false;
 return true;
}

/// Установка имени класса генераторов импульсов
bool NNeuronLearner::SetPulseGeneratorClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// Установка имени класса нейрона
bool NNeuronLearner::SetNeuronClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// Установка имени класса синапсов
bool NNeuronLearner::SetSynapseClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// Обнуление информации о паттерне, которому обучен нейрон, длинах дендритов и числе синапсов
bool NNeuronLearner::ZeroingTrainingPattern(void)
{
 // Паттерн, которому обучен нейрон
 MDMatrix<double> zero;
 zero.Assign(NumInputDendrite, 1, 0.0);
 if (Neuron)
  Neuron->TrainingPattern = zero;

 // Информация о структуре
 MDMatrix<int> temp;
 temp.Assign(NumInputDendrite, 1, 1);
 if (Neuron)
 {
  // Количество сегментов в дендритах
  Neuron->TrainingDendIndexes = temp;
  // Количество синапсов на входных сегментах дендритов
  Neuron->TrainingSynapsisNum = temp;
 }


 return true;
}

/// Установка используемого порога низкопороговой зоны нейрона
bool NNeuronLearner::SetLTZThreshold(const double &value)
{
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
 {
  UseFixedLTZThreshold = false;
 }

 return true;
}

/// Установка (поднятие или опускание) флага необходимости обучения некоторому паттерну
bool NNeuronLearner::SetIsNeedToTrain(const bool &value)
{
 if(CalculateMode == 1 && !value)
 {
  ZeroingTrainingPattern();
 }

 CountIteration = 0;  // Обнуляем счетчик итераций
 IsFirstBeat = true;

 // При поднятии флага надо установить порогу LTZone учебное значение
 if(value)
 {
  SetLTZThreshold(TrainingLTZThreshold.v);
  LTZThreshold.v = TrainingLTZThreshold.v;
 }
 // При опускании флага надо установить фиксированное значение
 else
 {
  SetLTZThreshold(FixedLTZThreshold.v);
  LTZThreshold.v = FixedLTZThreshold.v;
 }
 return true;
}

/// Установка режима работы (обучение, распознование) нейрона:
/// при поднятом флаге IsNeedToTrain происходит обучение, иначе - распознавание
/// 0 - аналог работы NeuronTrainer: после того, как нейрон изучил пример,
/// автоматически опускается флаг IsNeedToTrain, затем происходит распознавание
/// 1 - флаг IsNeedToTrain автоматически не опускается, режим предполагает внешнее управление флагом
bool NNeuronLearner::SetCalculateMode(const int &value)
{
 // Проверка корректности введённого значения
 if((value != 0) && (value != 1))
  return false;

// // Поднимаем флаг необходимости обучения
// SetIsNeedToTrain(true);
// IsNeedToTrain = true;

 CountIteration = 0;  // Обнуляем счетчик итераций
 IsFirstBeat = true;

 return true;
}

/// Установка времени задержки начала обучения относительно старта системы (сек)
bool NNeuronLearner::SetDelay(const double &value)
{
 for(int i = 0; i < int(Generators.size()); i++)
 {
  Generators[i]->Delay = value + InputPattern[i];
  Generators[i]->Reset();
 }
 return true;
}

/// Установка частоты генераторов (Гц)
bool NNeuronLearner::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < Generators.size(); i++)
 {
  Generators[i]->Frequency = value;
  Generators[i]->Reset();
 }

 return true;
 }

/// Установка числа входных дендритов
bool NNeuronLearner::SetNumInputDendrite(const int &value)
{
 if(value < 1)
  return false;
 Ready = false;
 OldNumInputDendrite = NumInputDendrite;
 return true;
}

/// Установка значения максимальной длины дендритов (= числа сегментов в дендритах)
bool NNeuronLearner::SetMaxDendriteLength(const int &value)
{
 if(value < 1)
  return false;

 // Проверяем в нейроне соответствие новому условию
 for (int i = 0; i < NumInputDendrite; i++)
 {
  if (DendriteLength.empty())
   return true;

  // Если найдётся дендрит длинее, чем теперь разрешено,
  // то укоротим дендрит до текущей максимальной возможной длины
  if (DendriteLength[i] > value)
  {
   DendriteLength[i] = value;

   // Удаляем связи генератора
   Generators[i]->DisconnectAll("Output");

   if (!Neuron)
    return true;

   // Обновляем информацию в нейроне о длинах дендритов
   if(Neuron->StructureBuildMode != 2)
   {
    Neuron->NumDendriteMembraneParts = 1;
   }
   else
   {
    vector<int> temp;
    temp = Neuron->NumDendriteMembranePartsVec;
    temp[i] = value;
    Neuron->NumDendriteMembranePartsVec = temp;
   }
   // Neuron->GetStorage()->FreeObjectsStorage();
   Neuron->Reset();
   // Neuron->GetStorage()->FreeObjectsStorage();

   // Находим последний фрагмент дендрита
   UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                             + "_" + sntoa(DendriteLength[i]), true);
   if(!dendrite)
    return true;

   // Меняем число синапсов на дендрите. Устанавливается то же, что и было
   dendrite->NumExcitatorySynapses = NumSynapse[i];
//   dendrite->GetStorage()->FreeObjectsStorage();
   dendrite->Build();
//   dendrite->GetStorage()->FreeObjectsStorage();

   // Для каждого синапса, кроме нулевого, так как он сразу создаётся с нужными настройками,
   // добавляем связь с генератором и меняем сопротивление
   for(int k = 0; k < NumSynapse[i]; k++)
   {
    NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(k);

    // Добаляем связь между текущим генератором и синапсом
    bool res = CreateLink("Source" + sntoa(i + 1), "Output", synapse->GetLongName(this), "Input");

    if(!res || k == 0)
     continue;

    synapse->Resistance = SynapseResistanceStep;
   }
   Neuron->Reset();
  }
 }

 return true;
}

/// Установка паттерна для запоминания, закодированного по TTFS (содержит запаздывания входных сигналов для каждого дендрита)
bool NNeuronLearner::SetInputPattern(const MDMatrix<double> &value)
{
 for(int i = 0; i < int(Generators.size()); i++)
 {
  Generators[i]->Delay = Delay + value[i];
  Generators[i]->Reset();
 }

 // Поднимаем флаг первого такта итерации
 IsFirstBeat = true;
 // При смене паттерна обнуляем счётчик итераций
 CountIteration = 0;
 return true;
}

 /// Установка дополнительного паттерна, который изучается в экспериментах для получения промежуточных структур
bool NNeuronLearner::SetAdditionalInputPattern(const MDMatrix<double> &value)
{
 return true;
}

/// Установка значения порога низкопороговой зоны нейрона, устанавливаемого на время обучения
bool NNeuronLearner::SetTrainingLTZThreshold(const double &value)
{
 return true;
}

/// Установка фиксированного значения (значения по умолчанию) порога низкопороговой зоны нейрона
bool NNeuronLearner::SetFixedLTZThreshold(const double &value)
{
 return true;
}

/// Установка признака необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NNeuronLearner::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  SetLTZThreshold(FixedLTZThreshold.v);
  LTZThreshold.v = FixedLTZThreshold.v;
 }

 return true;
}

/// Установка значения сопротивления добавляемых в процессе обучения возбуждающих синапсов
bool NNeuronLearner::SetSynapseResistanceStep(const double &value)
{

 // Меняем сопротивления у существующих нарощенных синапсов
 // Предполгается, что такие синапсы есть только на входных сегментах дендритов
 if (NumSynapse.empty() || !Neuron)
  return true;

 for(int numdend = 1; numdend <= NumInputDendrite; numdend++)
 {
  for(int numsyn = 2; numsyn <= NumSynapse[numdend - 1]; numsyn++)
  {
   UEPtr<NPulseSynapse> synapse = Neuron->GetComponentL<NPulseSynapse>("Dendrite" +
                                                                       sntoa(numdend) + "_" + sntoa(DendriteLength[numdend - 1]) +
                                                                       ".ExcSynapse" + sntoa(numsyn), true);
   if(!synapse)
    continue;

   synapse->Resistance = value;
  }
 }
 return true;
}

/// Установка режима проведения экспериментов
bool NNeuronLearner::SetExperimentMode(const bool &value)
{
 if(!value)
 {
  Fin.close();
  Fout.close();
  CanChangeDendLength = true;
 }
 else
 {
  ExperimentNum = 1;  // По умлочанию включаем 1-й эксперимент
  EpochCur = 1;
 }
 IsFirstFileStep = true;
 return true;
}

/// Установка значения массива длин дендритов (количеств сегментов в дендритах)
bool NNeuronLearner::SetDendriteLength(const std::vector<int> &value)
{
 OldDendriteLength = DendriteLength;
 Ready = false;
 return true;
}

/// Массив значений максимумов потенциалов на выходах сегментов сомы,
/// когда нейрон находится в исходном состоянии, то есть все дендриты
/// состоят из одного сегмента и имеют только по одному тормозному и возбуждающему синапсу
bool NNeuronLearner::SetInitialSomaPotential(const std::vector<double> &value)
{
 return true;
}

/// Установка значения массива количеств возбуждающих синапсов на дендритах
bool NNeuronLearner::SetNumSynapse(const std::vector<int> &value)
{
 Ready = false;
 return true;
}

/// Установка номера эксперимента
/// 1 - PatternRecognition()
/// 2 - LearningSecondPattern()
bool NNeuronLearner::SetExperimentNum(const int &value)
{
 CalculateMode = 1;
 SetCalculateMode(1);
 IsFirstFileStep = true;
 if (value == 1)
 {
  IsNeedToTrain = false;
  SetIsNeedToTrain(false);
 }
 if (value == 2)
 {
  IsNeedToTrain = true;
  SetIsNeedToTrain(true);
 }
 return true;
}

// --------------------------



// --------------------------
// Системные методы управления объектом
// --------------------------

/// Выделяет память для новой чистой копии объекта этого класса
NNeuronLearner* NNeuronLearner::New(void)
{
 return new NNeuronLearner;
}

UComponent* NNeuronLearner::NewStatic(void)
{
 return new NNeuronLearner;
}

// --------------------------



// --------------------------
// Скрытые методы управления компонентами
// --------------------------

/// Выполняет завершающие пользовательские действия
/// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был успешно добавлен в список компонент
bool NNeuronLearner::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}

/// Выполняет предварительные пользовательские действия
/// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp существует в списке компонент
bool NNeuronLearner::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}

// --------------------------



// --------------------------
// Скрытые методы управления счетом
// --------------------------

/// Осуществляет сборку структуры в соответствии с выбранными значениями параметров
bool NNeuronLearner::BuildStructure()
{
 bool res(true);

 // Cоздаём нейрон
 Neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
 Neuron->SetCoord(MVector<double,3>(8.7 + 1 * 7, 1.67, 0));

 // Устанавливаем количество сом
 Neuron->NumSomaMembraneParts = NumInputDendrite;

 // Устанавливаем количество и длину дендритов
 if(Neuron->StructureBuildMode != 2)
  Neuron->NumDendriteMembraneParts = 1;
 else
 {
  // Обновляем информацию в NeuronLearner
  DendriteLength.resize(NumInputDendrite, 1);
  OldDendriteLength.resize(NumInputDendrite, 1);
  // Обновляем информацию в нейроне
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
 }
 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
// if(Neuron && Neuron->GetStorage())
  // Neuron->GetStorage()->FreeObjectsStorage();

 // Удаляем лишние генераторы
 for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
 {
  DelComponent(std::string("Source") + sntoa(i + 1));
 }
 Storage->FreeObjectsStorage();

 // Инициализируем генераторы импульсов
 Generators.resize(NumInputDendrite);
 for(int i = 0; i < NumInputDendrite; i++)
 {
  Generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source") + sntoa(i + 1), PulseGeneratorClassName);
  Generators[i]->SetCoord(MVector<double,3>(6.7, 1.67 + i * 2, 0));
  Generators[i]->Frequency = SpikesFrequency;
 }

 // Разрываем выходные связи генераторов
 for(int i = 0; i < NumInputDendrite; i++)
 {
  Generators[i]->DisconnectAll("Output");
 }

 // Массив количества синапсов на дендрите
 // При необходимости меняем размер
 if (NumInputDendrite != NumSynapse.size())
  NumSynapse.resize(NumInputDendrite, 1);

 // Устанавливаем на дендритах необходиме количество синапсов
 for (int i = 0; i < NumInputDendrite; i++)
 {
  // Удаляем избыточные синапсы на сегменте дендрита, который раньше был входным
  if ((i < OldDendriteLength.size()) && (i < DendriteLength.size()) && (OldDendriteLength[i] < DendriteLength[i]))
  {
   UEPtr<NPulseMembrane> previnputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                              "_" + sntoa(OldDendriteLength[i]), true);
   if(previnputsegmentofdendrite)
   {
    previnputsegmentofdendrite->NumExcitatorySynapses = 1;
    previnputsegmentofdendrite->Build();
   }
  }

  // Находим входной сегмент дендрита
  UEPtr<NPulseMembrane> inputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                            + "_" + sntoa(DendriteLength[i]), true);
  if(!inputsegmentofdendrite)
   continue;

  // Меняем число синапсов на дендрите. Устанавливается то же, что и было
  inputsegmentofdendrite->NumExcitatorySynapses = NumSynapse[i];
//  dendrite->GetStorage()->FreeObjectsStorage();
  inputsegmentofdendrite->Build();
//  dendrite->GetStorage()->FreeObjectsStorage();
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 // Добаляем связи между генераторами и синапсами, меняем сопротивление синапсов
 for(int numdend = 0; numdend < NumInputDendrite; numdend++)
 {
  // Находим входной сегмент дендрита
  UEPtr<NPulseMembrane> segmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(numdend + 1) +
                                                                            + "_" + sntoa(DendriteLength[numdend]), true);
  // Для каждого синапса
  for(int numsyn = 0; numsyn < NumSynapse[numdend]; numsyn++)
  {
   NPulseSynapseCommon *synapse = segmentofdendrite->GetExcitatorySynapses(numsyn);

   if(!synapse)
   {
    LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Can't create link because synapse not exists: "));
    continue;
   }

   // Меняем сопротивление
   if(numsyn != 0)
    synapse->Resistance = SynapseResistanceStep;

   // Устанавливаем связь
   if (!CheckLink("Source" + sntoa(numdend + 1), "Output", synapse->GetLongName(this), "Input"))
    res &= CreateLink("Source" + sntoa(numdend + 1), "Output", synapse->GetLongName(this), "Input");

   if(!res)
    continue;
  }
 }
 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 // Задаём размеры и начальные значения параметров обучения
 IsFirstBeat = true; // Флаг первого такта в рамках итерации расчёта нейрона
 // Максимальная суммарная амплитуда нейрона, снятая на дендрите
 DendriteNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 // Максимальная суммарная амплитуда нейрона, снятая на соме
 SomaNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);

 // Для первой итерации устанавливаем,
 // что все дендриты не должны расти
 DendStatus.assign(NumInputDendrite, 0);
 // как и синапсы
 SynapseStatus.assign(NumInputDendrite, 0);

 // Массив значений потенциалов на выходах дендритов при их единичной длине
 if (NumInputDendrite != InitialSomaPotential.size())
 {
  std::vector<double> newinitialsomapotential;
  newinitialsomapotential.assign(NumInputDendrite, 0.0);
  // Копирование старой информации
  for (int i = 0; i < std::min(NumInputDendrite.v, int(InitialSomaPotential.size())); i++)
  {
   newinitialsomapotential[i] = InitialSomaPotential[i];
  }
  InitialSomaPotential = newinitialsomapotential;
 }

 // Время для каждой сомы, когда амплитуда выходного сигнала на ней максимальна в течение итерации
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 double period = 1.0 / SpikesFrequency + 0.001;
 // Текущая рассинхронизация сигналов на сомах
 Dissynchronization.assign(NumInputDendrite, period);
 Dissynchronization[NumInputDendrite - 1] = 0.0;
 // Разница между текущей амплитудой на соме и исходной
 AmpDifference.assign(NumInputDendrite, 0.0);


 InputPattern.Resize(NumInputDendrite, 1, 0.0);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);  // Начальное значение предыдущего паттерна
 AdditionalInputPattern.Resize(NumInputDendrite, 1, 0.0);
 return true;
}


/// Восстановление настроек по умолчанию и сброс процесса счета
bool NNeuronLearner::ADefault(void)
{
 Generators.clear();
 StructureBuildMode = 1;
 CalculateMode = 0;
 PulseGeneratorClassName = "NPulseGeneratorTransit";
 NeuronClassName = "NSPNeuronGen";
 SynapseClassName = "NPSynapseBio";
 IsNeedToTrain = true;
 ExperimentMode = false;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 OldNumInputDendrite = 0;
 MaxDendriteLength = 100;

 // Настройки порога
 LTZThreshold = 100;
 FixedLTZThreshold = 0.0115;
 TrainingLTZThreshold = 100;
 UseFixedLTZThreshold = false;

 // Паттерны
 InputPattern.Resize(NumInputDendrite, 1);
 AdditionalInputPattern.Resize(NumInputDendrite, 1);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);

 // Для графиков
 DendriteNeuronAmplitude.Assign(NumInputDendrite, 1, 0.0);
 SomaNeuronAmplitude.Assign(NumInputDendrite, 1, 0.0);
 Output.Assign(NumInputDendrite, 1, 0.0);

 SynapseResistanceStep = 1.0e9;
 DendriteLength.assign(NumInputDendrite, 1);
 OldDendriteLength.assign(NumInputDendrite, 1);
 NumSynapse.assign(NumInputDendrite, 1);

 CountIteration = 0;
 ExperimentNum = 0;  // номер по умлочанию, эксперимента нет
 EpochCur = 0;
 CanChangeDendLength = true;

 IsWritten = false; // Флаг позволяющий записать выходные данные в файл

 // Задаём размеры и начальные значения параметров обучения
 IsFirstBeat = true; // Флаг первого такта в рамках итерации расчёта нейрона

 // Максимальная суммарная амплитуда нейрона, снятая на дендрите
 DendriteNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 // Максимальная суммарная амплитуда нейрона, снятая на соме
 SomaNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 InitialSomaPotential.assign(NumInputDendrite, 0.0);
 // Максимальные амплитуды выходного сигнала на сомах для данной итерации
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 // Время для каждой сомы, когда амплитуда выходного сигнала на ней максимальна в течение итерации
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 // Текущая рассинхронизация сигналов на сомах
 Dissynchronization.assign(NumInputDendrite, 1.0 / SpikesFrequency + 0.001);
 // Разница между текущей амплитудой на соме и исходной
 AmpDifference.assign(NumInputDendrite, 0.0);

 // Для первой итерации устанавливаем, что все дендриты не должны расти
 DendStatus.assign(NumInputDendrite, 0);
 // как и синапсы
 SynapseStatus.assign(NumInputDendrite, 0);

 return true;
}


/// Обеспечивает сборку внутренней структуры объекта после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NNeuronLearner::ABuild(void)
{
 bool res(true);
 if(StructureBuildMode == 1)
 {
  res = BuildStructure();
  if(!res)
   return false;
 }

 return true;
}


/// Сброс процесса счета
bool NNeuronLearner::AReset(void)
{
 // Устанавливаем значение порога по умолчанию
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = LTZThreshold;

 // Задаём воходной паттерн на генераторах
 for(size_t i = 0; i < Generators.size(); i++)
 {
  Generators[i]->Delay = Delay + InputPattern[i];
  Generators[i]->Frequency = SpikesFrequency;
  Generators[i]->Reset();
 }
 Output.ToZero();

 return true;
}


/// Сравнивает поэлементно значения входных паттернов
/// prev_input_pattern - входной паттерн на предыдущей итерации обучения
/// input_pattern - входной паттерн на текущей итерации обучения
/// e - допустимая погрешность различия элементов, при непревышении которой элементы считаются одинаковыми
bool NNeuronLearner::CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e)
{
 bool res(true);
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // Если хотя бы для одного входа разница превышает допустимую погрешность,
  // то считаем, что паттерны разные
  if(fabs(prev_input_pattern[i] - input_pattern[i]) > e)
  {
   res = false;
   break;
  }
 }
 return res;
}


/// Изменение длины дендрита (числа сегментов в дендрите) num
bool NNeuronLearner::ChangeDendriteLength(int num)
{
 bool res(true); // Переменная для возвращения результата функции

 // Если не нужно менять длину дендритов - ничего не делаем
 if(!DendStatus[num])
  return true;


 // Если требуется удалить единственный оставшийся блок дендрита, то
 // считаем, что достигнута оптимальная длина дендрита
 if((DendStatus[num] == -1) && (DendriteLength[num] < 2))
 {
  DendStatus[num] = 0;
  return true;
 }


 // Необходимо увеличить длину дендрита
 if(DendStatus[num] == 1)
 {
  // Проверяем условие на максимальную длину дендритов
  if(DendriteLength[num] >= MaxDendriteLength)
  {
   DendStatus[num] = 0;
   return true;
  }
 }


 // Изменяем информацию о числе дендритов в NeuronLearner
 OldDendriteLength[num] = DendriteLength[num];
 DendriteLength[num] += DendStatus[num];


 // Удаляем связи генератора с синапсами
 Generators[num]->DisconnectAll("Output");


 // Изменяем информацию о числе участков дендритов в модели нейрона
 if(Neuron->StructureBuildMode != 2)
 {
  if(Neuron->NumDendriteMembraneParts != DendriteLength[num])
  {
   Neuron->NumDendriteMembraneParts = DendriteLength[num];
   // Neuron->GetStorage()->FreeObjectsStorage();
   Neuron->Reset();
   // Neuron->GetStorage()->FreeObjectsStorage();
  }
 }
 else
 {
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
  // Neuron->GetStorage()->FreeObjectsStorage();
  Neuron->Reset();
  // Neuron->GetStorage()->FreeObjectsStorage();

  // Убеждаемся, что длина изменилась корректно
  int templength = Neuron->NumDendriteMembranePartsVec[num];
  if(DendriteLength[num] != templength)
  {
   LogMessageEx(RDK_EX_WARNING, __FUNCTION__, "Incorrect dendtic length after INCREASE. Need " +
                sntoa(DendriteLength[num]) + " current length " + sntoa(templength));
  }
 }


 // Если добавился дендрит, то на предыдущем нужно оставить только 1 синапс
 if((DendStatus[num] == 1) && (DendriteLength[num] > 1))
 {
  UEPtr<NPulseMembrane> prevdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                             "_" + sntoa(DendriteLength[num] - 1), true);
  if(!prevdendrite)
   return true;

  prevdendrite->NumExcitatorySynapses = 1;
//  prevdendrite->GetStorage()->FreeObjectsStorage();
  prevdendrite->Build();
//  prevdendrite->GetStorage()->FreeObjectsStorage();
 }


 // Изменяем количество синапсов в новом дендрите
 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                         "_" + sntoa(DendriteLength[num]), true);
 if(!dendrite)
  return true;
 dendrite->NumExcitatorySynapses = NumSynapse[num];
// dendrite->GetStorage()->FreeObjectsStorage();
 dendrite->Build();
// dendrite->GetStorage()->FreeObjectsStorage();


 // Добаляем связи между текущим генератором и синапсами на новом дендрите
 for (int i = 0; i < NumSynapse[num]; i++)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(i);

  res &= CreateLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(this), "Input");
  if(!res)
   return true;

  if(i > 0)
   synapse->Resistance = SynapseResistanceStep;
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 return true;
}


/// Изменение числа возбуждающих синапсов на входном сегменте дендрита num
bool NNeuronLearner::ChangeSynapseNumber(int num)
{
 bool res(true); // Переменная для возвращения результата функции

 // Если не нужно менять число синапсов - ничего не делаем
 if(!SynapseStatus[num])
  return true;

 // Если требуется удалить единственный оставшийся синапс, то
 // считаем, что достигнуто оптимальное количество синапсов
 if((SynapseStatus[num] == -1) && (NumSynapse[num] < 2))
 {
  SynapseStatus[num] = 0;
  return true;
 }

 // Изменяем информацию о количестве синапсов в NeuronLearner
 NumSynapse[num] += SynapseStatus[num];

 // Находим последний фрагмент дендрита
 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                        "_" + sntoa(DendriteLength[num]), true);
 if(!dendrite)
  return true;

 // Если надо удалить синапс, то прежде разрываем его связи
 if (SynapseStatus[num] == -1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(NumSynapse[num]);
  synapse->DisconnectAll();
 }

 // Изменяем количество синапсов в дендрите
 dendrite->NumExcitatorySynapses = NumSynapse[num];
// dendrite->GetStorage()->FreeObjectsStorage();
 dendrite->Build();
// dendrite->GetStorage()->FreeObjectsStorage();

 // Если добавился синапс
 if (SynapseStatus[num] == 1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(NumSynapse[num] - 1);
  if(!synapse)
   return true;

  // Меняем у него сопротивление
  synapse->Resistance = SynapseResistanceStep;

  // Создаём связь
  if (!CheckLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(this), "Input"))
   res &= CreateLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(this), "Input");

  if(!res)
   return true;
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 return true;
}


/// Измерение значение потенциала на выходе сегментов сомы. Вызывается на каждом такте,
/// чтобы определить максимальное значение потенциала и время его фиксации
/// в течение текущей итерации
bool NNeuronLearner::MeasureMaxPotentialAndTime(void)
{
 // Текущее значение амплитуды сомы
 double currentsomaamp;

 // Для каждой сомы ищем максимальную амплитуду
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // Находим сому
  UEPtr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);

  if(!soma)
   return true;

  // Снимаем амплитуду
  currentsomaamp = soma->SumPotential(0,0);

  // Проверяем условие на максимум
  if(currentsomaamp >= MaxIterSomaAmp[i])
  {
   // Запоминаем новые значения максимума и его времени
   MaxIterSomaAmp[i] = currentsomaamp;
   TimeOfMaxIterSomaAmp[i] = Environment->GetTime().GetDoubleTime();

   // Для начального участка дендрита запоминаем выходную амплитуду
   if(DendriteLength[i] == 1 && NumSynapse[i] && currentsomaamp > InitialSomaPotential[i])
    InitialSomaPotential[i] = currentsomaamp;
  }
 }
 return true;
}


/// Изменение статусов дендритов для изменения структуры на следующей итерации.
/// Вызывается в конце каждой итерации
bool NNeuronLearner::ChangeDendriteStatus(int num)
{
 // Текущий рассинхронизация сигналов на калибровочном дендрите и текущем
 double dt = TimeOfMaxIterSomaAmp[NumInputDendrite - 1] - TimeOfMaxIterSomaAmp[num];

 // Считаем, что длина оптимальна, если:
 // 1. рассинхронизация нулевая
 // 2. при одинаковых предыдущем и текущем паттернах рассинхронизация сменила знак
 // и уменьшилась по модулю
 // 3. при одинаковых предыдущем и текущем паттернах уже стоит нулевой статус
 if(dt == 0.0)
 {
  DendStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001)
          && ((((dt / fabs(dt)) * (Dissynchronization[num] / fabs(Dissynchronization[num])) < 0)
          && (fabs(dt) <= fabs(Dissynchronization[num]))) || (DendStatus[num] == 0)))
 {
  DendStatus[num] = 0;
 }
 else
 {
  // Текущий импульс пришёл раньше калибровочного, нужно растить дендрит
  if(dt > 0)
   DendStatus[num] = 1;
  // Текущий импульс пришёл позже калибровочного, нужно укорачивать дендрит
  else if(dt < 0)
   DendStatus[num] = -1;
 }

 // Запоминаем рассинхронизацию
 Dissynchronization[num] = dt;

 return true;
}


/// Изменение статусов синапсов для изменения структуры на следующей итерации.
/// Вызывается в конце каждой итерации
bool NNeuronLearner::ChangeSynapseStatus(int num)
{
 // Разница исходной амплитуды на соме и текущей
 double dt = InitialSomaPotential[num] - MaxIterSomaAmp[num];

 // Надстройка для эксперимента с обучением второму примеру - необходимо, чтобы
 // определялось соответствующее число синапсов для каждого промежуточного числа дендритов
 int dendstatus = DendStatus[num];
 if (ExperimentNum == 2 && !CanChangeDendLength)
  DendStatus[num] = 0;

 // Считаем, что количество синапсов оптимально, если
 // при одинаковых предыдущем и текущем паттернах и уже нулевом статусе дендрита:
 // 1. уже стоит нулевой статус у синапса
 // 2. разница в амплитудах не больше 0.000005
 // 3. разница в амплитудах сменила знак и уменьшилась по модулю
 if((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
    && !SynapseStatus[num])
 {
  SynapseStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
          && (fabs(dt) <= 0.000005))
 {
  SynapseStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
          && ((dt / fabs(dt)) * (AmpDifference[num] / fabs(AmpDifference[num])) < 0)
          && (fabs(dt) <= fabs(AmpDifference[num])))
 {
  SynapseStatus[num] = 0;
 }
 else
 {
  // Текущая амплитуда меньше исходной, поэтому добавляем синапсы
  if(dt > 0.0)
   SynapseStatus[num] = 1;
  // Текущая амплитуда больше исходной, поэтому удаляем синапсы
  else if(dt < 0.0)
   SynapseStatus[num] = -1;
 }

 // Запоминаем рассинхронизацию
 AmpDifference[num] = dt;

 DendStatus[num] = dendstatus;

 return true;
}

/// Функция, реализующая распознавание выборки паттернов из входного файла
bool NNeuronLearner::PatternRecognition(void)
{
 // Первое открытие файла с тестовыми примерами
 if(IsFirstFileStep)
 {
  // Подготовка к работе с файлами
  Fin.open(Environment->GetCurrentDataDir()+"input_data.txt");
  Fout.open(Environment->GetCurrentDataDir()+"output_data"+sntoa(EpochCur)+".txt");
  Inputs.Assign(NumInputDendrite, 1, 0.0);  // Входные данные
  Outputs.assign(NumInputDendrite, 0);  // Выходные данные

  // Записываем информацию о структуре нейрона, которой осуществляется распознавание
  Fout << "Структура нейрона, при которой происходит распознавание:" << std::endl;
  for(int i = 0; i < NumInputDendrite; i++)  // длины дендритов
  {
   Fout << DendriteLength[i] << "\t";
  }
  for(int i = 0; i < NumInputDendrite; i++)  // число синапсов
  {
   Fout << NumSynapse[i] << "\t";
  }
  Fout << std::endl;
  Fout << "Результаты распознавания:" << std::endl;
  IsFirstFileStep = false;
 }

 // Установка нового входного примера для распознавания
 // Обязательно устанавливаем при поднятом флаге первого такта. Так паттерн
 // будет заменён после полного окончания предыдущей итерации и перед началом новой
 if(IsFirstBeat)
 {
  StartIterTime = Environment->GetTime().GetDoubleTime();
  IterLength = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep));

  // Если достигли конца файла - завершаем режим распознавания
  if(Fin.eof())
  {
   Fin.close();
   Fout.close();
   IsFirstFileStep = true;
  }

  // (конец файла не достигнут - продолжение распознавания)
  // Считываем входной пример из файла
  for(int i = 0; i < NumInputDendrite; i++)
  {
   double in;
   Fin >> in;
   Inputs[i] = in;
  }

  // Устанавливаем пример для распознавания
  InputPattern = Inputs;
  SetInputPattern(Inputs);
  Outputs[0] = 0;
  IsWritten = false;
  IsFirstBeat = false;
 }

 // Снимаем результат распознавания входного примера
 UEPtr<NPulseNeuron> neuron = this->GetComponent<NPulseNeuron>(std::string("Neuron"), true);
 if(!neuron)
  return true;
 UEPtr<NLTZone> ltzone = neuron->GetComponent<NLTZone>("LTZone");
 if(!ltzone)
  return true;
 if(ltzone->Output(0,0) > 0.001)
  Outputs[0]= 1;

 // Записываем результат распознавания в выходной файл
 double currentitertime = Environment->GetTime().GetDoubleTime() - StartIterTime;
 if((currentitertime - IterLength >= 0) && !IsWritten)
 {
  Fout << Outputs[0] << std::endl;
  IsWritten = true;
  IsFirstBeat = true;
 }

 return true;
}

/// Код, реализующий обучение дополнительному примеру
/// До использования функции нейрон полностью обучается первому примеру. Далее в ф-ии последовательно изучает дополнительный.
/// Каждая эпоха обучения строится следующим образом: на первой итерации эпохи происходит изменение числа сегментов в дендритах и
/// числа синапсов, на других 4-х итерациях происходит изменение только числа синапсов, чтобы в конце каждой эпохи
/// число синапсов соответствовало числу сегментов в деднритах, далее распознавание примеров из файла input_data.txt
bool NNeuronLearner::LearningAdditionalPattern_1_4(MDMatrix<double> additional_pattern)
{
 // Обучение дополнительному примеру
 if (IsNeedToTrain)
 {
  if(IsFirstBeat)
  {
   // Установка паттерна обучения при первом вхождении в функцию
   if (CountIteration == 0 && EpochCur == 1)
   {
    InputPattern = additional_pattern;
    SetInputPattern(InputPattern);
   }
   // После 5 итерации переходим к распознаванию
   if (CountIteration >= 5)
   {
    IsFirstFileStep = true;
    IsNeedToTrain = false;
   }
   // После 1 итреции обучения не меняем длины дендритов
   else if(CountIteration >= 1)
   {
    CanChangeDendLength = false;
   }
  }
 }
 // Распознавание тестовых примеров из файла input_data.txt в конце эпохи
 else
 {
  // Распознавание паттернов из файла
  PatternRecognition();

  // Когда закончено распознавание паттернов из файла (поднятый IsFirstFileStep
  // - косвенный признак этого), необходимо либо перейти к обучению
  // дополнительному примеру, либо завершить эксперимент
  if (IsFirstFileStep)
  {
   // Если полностью изучили дополнительный пример - завершаем эксперимент
   if (EndOfLearning())
   {
    ExperimentMode = false;
    SetExperimentMode(false);
   }
   // Иначе продолжаем изучение дополнительного примера
   else
   {
    IsNeedToTrain = true;
    CountIteration = 0;
    CanChangeDendLength = true;
    EpochCur++;
    PrevInputPattern = additional_pattern;
    InputPattern = additional_pattern;
    SetInputPattern(additional_pattern);
    for (int i = 0; i < NumInputDendrite - 1; i++)
    {
     SynapseStatus[i] = 1;
    }
    return true;
   }
  }
 }

 return true;
}

/// Реализуется инкрментное обучение:
/// нейрон изучает дополнительный паттерн, ища оптимальную структуру по заданному критерию
bool NNeuronLearner::IncrementalLearning(MDMatrix<double> initial_pattern, MDMatrix<double> additional_pattern)
{
 if(!IsFirstBeat)
  return true;

 // Установка паттерна для обучения при первом вхождении в функцию
 if (CountIteration == 0 && EpochCur == 1)
 {
  InputPattern = additional_pattern;
  SetInputPattern(additional_pattern);
  Fout.open(Environment->GetCurrentDataDir()+"output_data"+sntoa(EpochCur)+".txt");
 }

 // После 5 итерации меняем входной паттерн на исходный,
 // чтобы вычислить для него сумму потенциалов
 if (CountIteration == 5)
 {
  // (пока ещё не была обнулена MaxIterSomaAmp)
  // Вычисляем сумму максимумов потенциалов для предыдущей итерации,
  // те для паттерна additional_pattern
  for (int k = 0; k < NumInputDendrite; k++)
  {
   SumMaxIterSomaAmp += MaxIterSomaAmp[k];
  }

  InputPattern = initial_pattern;
  SetInputPattern(initial_pattern);
  CountIteration = 5;  // устанавливаем значение, тк SetInputPattern() его обнулила
 }

 // Проверка условия оптимальности
 else if (CountIteration == 6)
 {
  // Добавляем сумму максимумов потенциалов для предыдущей итерации,
  // те для паттерна initial_pattern
  double suminitialpattern = 0;
  for (int k = 0; k < NumInputDendrite; k++)
  {
   suminitialpattern += MaxIterSomaAmp[k];
  }
  SumMaxIterSomaAmp += suminitialpattern;

  // Записываем измерения для additional_pattern, initial_pattern и суммы
  Fout << "Итерация обучения" + sntoa(EpochCur) + ":" << "\t" << SumMaxIterSomaAmp;
  Fout << "\t" << suminitialpattern << "\t" << SumMaxIterSomaAmp << std::endl;

  // Структура лучше описывает оба примера, продолжаем в том же направлении
  if (SumMaxIterSomaAmp > PrevSumMaxIterSomaAmp)
  {
   PrevInputPattern = additional_pattern;
  InputPattern = additional_pattern;
   SetInputPattern(InputPattern);
   CanChangeDendLength = true;
   EpochCur++;
   for (int i = 0; i < NumInputDendrite - 1; i++)
   {
    SynapseStatus[i] = 1;
   }
  }
  // Стало хуже - возращаем предыдущую структуру и заканчиваем обучение
  else
  {
   Fout.close();
  }

 PrevSumMaxIterSomaAmp = SumMaxIterSomaAmp;
  SumMaxIterSomaAmp = 0;
 }

 // После 1 итерации обучения не меняем длины дендритов
 else if(CountIteration >= 1)
 {
  CanChangeDendLength = false;
 }

 return true;
}

/// Функция для проведения экспериментов с моделью, в т.ч. происходит работа с файлами
/// 1 эксперимент - PatternRecognition() - распознавание выборки из файла
/// 2 эксперимент - LearningAdditionalPattern_1_4() - обучение дополнительному примеру
/// 3 эксперимент - IncrementalLearning() - Инкрементное обучение
bool NNeuronLearner::Experiment(void)
{
 switch (ExperimentNum) {
 case 1:  // Распознавание данных из файла
 {
  PatternRecognition();
  // Когда закончено распознавание паттернов из файла (поднятый IsFirstFileStep
  // - косвенный признак этого), завершаем эксперимент
  if (IsFirstFileStep)
  {
   ExperimentMode = false;
   SetExperimentMode(false);
  }
  break;
 }
 case 2:  // Обучение дополнительному примеру
 {
  // Нейрон должен быть обучен некоторому примеру,
  // после чего может использоваться данная функция
  LearningAdditionalPattern_1_4(AdditionalInputPattern);
  break;
 }
 case 3:  // Инкрементное обучение путём нахождения оптимального положения между двумя паттернами
 {
  // Нейрон должен быть обучен некоторому примеру,
  // после чего может использоваться данная функция
  IncrementalLearning(InputPattern, AdditionalInputPattern);
  break;
 }
 default:
  break;
 }
 return true;
}


/// Завершающие обучение действия
bool NNeuronLearner::EndOfLearning(void)
{
 // Проверка обученности нейрона предыдущему импульсу
 bool istrained = true;
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // Если есть хотя бы один дендрит или сома, длину кот-х надо изменить,
  // то нейрон ещё не изучил паттерн
  if(DendStatus[i] || SynapseStatus[i])
  {
   istrained = false;
   break;
  }
 }

 // Если нейрон уже изучил паттерн, то обновляем информацию
 if(istrained)
 {
  // Паттерн, которому обучен нейрон
  Neuron->TrainingPattern = InputPattern;

  // Длины дендритов
  MDMatrix<int> temp;
  temp.Resize(NumInputDendrite, 1);
  for(int i = 0; i < NumInputDendrite; i++)
  {
   temp(i, 0) = DendriteLength[i];
  }
  Neuron->TrainingDendIndexes.Resize(NumInputDendrite, 1);
  Neuron->TrainingDendIndexes = temp;

  // Количество синапсов на входных участках дендритов
  for(int i = 0; i < NumInputDendrite; i++)
  {
   temp(i, 0) = NumSynapse[i];
  }
  Neuron->TrainingSynapsisNum.Resize(NumInputDendrite, 1);
  Neuron->TrainingSynapsisNum = temp;

  SetIsNeedToTrain(false);
  IsNeedToTrain = false;

  return true;
 }

 return false;
}


/// Часть процесса расчёта NeuronLearner - обучение модели
// Под итерацией подразумевается промежуток времени между приходами паттернов
// За время итерации вызов функции происходит многократно
// Каждый такой вызов функции называем тактом
bool NNeuronLearner::Training(void)
{
 // Если 0 режим обучения и не 0 итерация, то можем завершить обучение
 if(!CalculateMode && (CountIteration > 0))
 {
  if (EndOfLearning())
   return true;
 }


 // Действия только для первого такта итерации
 if(IsFirstBeat)
 {
  // Измеряем время начала текущей итерации и вычисляем её длительность
  StartIterTime = Environment->GetTime().GetDoubleTime();
  IterLength = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep));

  // Обнуляем максимальные амплитуды выходного сигнала на сомах для данной итерации
  MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
  TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);

  // Изменяем структуру нейрона
  for(int i = 0; i < NumInputDendrite - 1; i++)
  {
   // Изменяем длину дендрита по значению его статуса
   if(CanChangeDendLength) // запрет на изменение длины используется в LearningSecondPattern
    ChangeDendriteLength(i);
   // Изменяем число синапсов по значению статуса
   ChangeSynapseNumber(i);
  }

  IsFirstBeat = false;

  // Досрочно завершаем такт
  return true;
 }


 // Вычисляем амплитуду потенциала на соме, чтобы найти максимум и его время
 MeasureMaxPotentialAndTime();


 // Действия при завершении итерации
 // Текущая длительность итерации - сколько прошло времени от начала итерации
 double currentitertime = Environment->GetTime().GetDoubleTime() - StartIterTime;

 if(currentitertime - IterLength >= 0)
 {
  // Меняем статусы всех дендритов и синапсов, кроме калибровочных
  for(int i = 0; i < NumInputDendrite; i++)
  {
   ChangeDendriteStatus(i);
   ChangeSynapseStatus(i);
   // Запоминаем текущий паттерн перед началом следующей итерации
   PrevInputPattern[i] = InputPattern[i];
  }

  // Поднимаем для следующей итерации флаг первого такта
  IsFirstBeat = true;
  // Увеличиваем счётчик итераций.
  CountIteration++;
 }

 return true;
}


/// Выполняет расчет этого объекта
// Под итерацией подразумевается промежуток времени между приходами паттернов
// За время итерации вызов функции происходит многократно
// Каждый такой вызов функции называем тактом
bool NNeuronLearner::ACalculate(void)
{
 // Если нет нейрона, то ничего делать не надо
 if(!Neuron)
  return true;


 // Вычисляем значения потенциалов на дендритах и сомах
 DendriteNeuronAmplitude(0, 0) = 0;
 for(int i = 0; i < NumInputDendrite; i++)
 {
     UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i + 1)+"_1",true);
     if(!dendrite)
      return true;
     DendriteNeuronAmplitude(i + 1, 0) = dendrite->SumPotential(0, 0);
     DendriteNeuronAmplitude(0, 0) += dendrite->SumPotential(0, 0);
 }

 SomaNeuronAmplitude(0, 0) = 0;
 for(int i = 0; i < NumInputDendrite; i++)
 {
     UEPtr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i + 1),true);
     if(!soma)
      return true;
     SomaNeuronAmplitude(i + 1, 0) = soma->SumPotential(0, 0);
     SomaNeuronAmplitude(0, 0) += soma->SumPotential(0, 0);
 }


 // Если 0 режим обучения и не 0 итерация, то можем завершить обучение
 if(!CalculateMode && (CountIteration > 0))
 {
  // Проверяем обученность нейрона предыдущему импульсу
  bool istrained = true;
  for(int i = 0; i < NumInputDendrite; i++)
  {
   // Если есть хотя бы один дендрит или сома, длину кот-х надо изменить,
   // то нейрон ещё не изучил паттерн
   if(DendStatus[i] || SynapseStatus[i])
   {
    istrained = false;
    break;
   }
  }

  // Если нейрон уже изучил паттерн, то обновляем информацию
  if(istrained)
  {
   // Паттерн, которому обучен нейрон
   Neuron->TrainingPattern = InputPattern;

   // Длины дендритов
   MDMatrix<int> temp;
   temp.Resize(NumInputDendrite, 1);
   for(int i = 0; i < NumInputDendrite; i++)
   {
    temp(i, 0) = DendriteLength[i];
   }
   Neuron->TrainingDendIndexes.Resize(NumInputDendrite, 1);
   Neuron->TrainingDendIndexes = temp;

   // Количество синапсов на входных участках дендритов
   for(int i = 0; i < NumInputDendrite; i++)
   {
    temp(i, 0) = NumSynapse[i];
   }
   Neuron->TrainingSynapsisNum.Resize(NumInputDendrite, 1);
   Neuron->TrainingSynapsisNum = temp;

   SetIsNeedToTrain(false);
   IsNeedToTrain = false;

   // Досрочно завершаем этот такт
   return true;
  }
 }


 // Функция для работы с файлами.
 // Осуществляет чтение входных данных из файла,
 // Обработку результатов и запись результатов в файл
 if(ExperimentMode)
     Experiment();

 // В функции происходит обучение нейрона
 if (IsNeedToTrain)
  Training();


 // Подаём информацию с выхода нейрона на выход NNeuronLearner
 if(Neuron)
  Output = Neuron->Output;

 return true;
}

// --------------------------

}

#endif
