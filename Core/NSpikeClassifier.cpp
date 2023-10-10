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

#ifndef NSPIKECLASSIFIER_CPP
#define NSPIKECLASSIFIER_CPP

#include "NSpikeClassifier.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"


// Класс, создающий группу обученных нейронов для распознавания заданного паттерна импульсов
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSpikeClassifier::NSpikeClassifier(void)
: StructureBuildMode("StructureBuildMode",this,&NSpikeClassifier::SetStructureBuildMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NSpikeClassifier::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NSpikeClassifier::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NSpikeClassifier::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NSpikeClassifier::SetSynapseClassName),
  NumNeurons("NumNeurons",this,&NSpikeClassifier::SetNumNeurons),
  IsNeedToTrain("IsNeedToTrain",this,&NSpikeClassifier::SetNeedToTrain),
  Delay("Delay",this,&NSpikeClassifier::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NSpikeClassifier::SetSpikesFrequency),
  NumInputDendrite("NumInputDendrite",this,&NSpikeClassifier::SetNumInputDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NSpikeClassifier::SetMaxDendriteLength),
  TrainingPatterns("TrainingPatterns",this,&NSpikeClassifier::SetTrainingPatterns),
  InputPattern("InputPattern",this,&NSpikeClassifier::SetInputPattern),
  LTZThreshold("LTZThreshold",this,&NSpikeClassifier::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NSpikeClassifier::SetFixedLTZThreshold),
  TrainingLTZThreshold("TrainingLTZThreshold",this,&NSpikeClassifier::SetTrainingLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NSpikeClassifier::SetUseFixedLTZThreshold),
  UseTransitSignal("UseTransitSignal",this,&NSpikeClassifier::SetUseTransitSignal),
  DataFromFile("DataFromFile",this,&NSpikeClassifier::SetDataFromFile)
{
 OldNumNeurons=0;
 OldNumInputDendrite=0;
 generators.clear();
 trainers.clear();

 // Переменные для чтение и записи данных в файл
 // ------->
 // Признак первой итерации
 IsFirstFileStep = true;
 // <-------
}

NSpikeClassifier::~NSpikeClassifier(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool NSpikeClassifier::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NSpikeClassifier::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего учитель нейрона
bool NSpikeClassifier::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NSpikeClassifier::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NSpikeClassifier::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число нейронов
bool NSpikeClassifier::SetNumNeurons(const int &value)
{
 Ready=false;

 OldNumNeurons = NumNeurons;
 TrainingPatterns.Resize(value,NumInputDendrite);

 return true;
}

/// Признак необходимости обучения
bool NSpikeClassifier::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  OldNumNeurons=0;
  OldNumInputDendrite=0;
  generators.clear();
  trainers.clear();
  DelAllComponents();
  Ready=false;
 }
 else
 {
  UEPtr<NNeuronTrainer> trainer;
  UEPtr<NPulseGeneratorTransit> generator;
  for(int i = 0; i < NumNeurons; i++)
  {
   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
   if(!trainer)
	return true;
   for(int j = 0; j < NumInputDendrite; j++)
   {
	generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
	if(!generator)
	 return true;

	generator->UseTransitSignal = (value)? false : true;
   }
  }
 }
 return true;
}

/// Время задержки начала обучения относительно старта системы (сек)
bool NSpikeClassifier::SetDelay(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->Delay = value;

 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = value + InputPattern[i];
  generators[i]->Reset();
 }
 return true;
}

/// Частота генераторов (Гц)
bool NSpikeClassifier::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->SpikesFrequency = value;

 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Frequency = value;
  generators[i]->Reset();
 }

 return true;
}

/// Число входных дендритов
bool NSpikeClassifier::SetNumInputDendrite(const int &value)
{
 Ready=false;

 OldNumInputDendrite = NumInputDendrite;
 InputPattern.Resize(value,1);
 /*for(size_t i = 0; i < trainers.size(); i++)
 {
	trainers[i]->NumInputDendrite = value;
	trainers[i]->Reset();
 } */
 TrainingPatterns.Resize(NumNeurons,value);

 return true;
}

/// Максимальная длина дендрита
bool NSpikeClassifier::SetMaxDendriteLength(const int &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->MaxDendriteLength = value;

 return true;
}

/// Паттерн для запоминания
bool NSpikeClassifier::SetTrainingPatterns(const MDMatrix<double> &value)
{
 if(IsNeedToTrain)
 {
  Ready=false;
 }
 else
 {
  MDMatrix<double> pattern;
  pattern.Resize(NumInputDendrite,1);
  for(int i = 0; i < int(trainers.size()); i++)
  {
   for(int j = 0; j < NumInputDendrite; j++)
	pattern[j] = value(i,j);
   trainers[i]->InputPattern = pattern;
   trainers[i]->IsNeedToTrain=IsNeedToTrain;
  }
 }
 return true;
}

/// Паттерн для распознавания
bool NSpikeClassifier::SetInputPattern(const MDMatrix<double> &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = Delay + value[int(i)];
  generators[i]->Reset();
 }

 return true;
}

/// Порог низкопороговой зоны нейрона
bool NSpikeClassifier::SetLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->LTZThreshold = value;

 return true;
}

/// Порог низкопороговой зоны нейрона для этапа обучения
bool NSpikeClassifier::SetTrainingLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->TrainingLTZThreshold = value;

 return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NSpikeClassifier::SetFixedLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->FixedLTZThreshold = value;

 return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NSpikeClassifier::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  double threshold = FixedLTZThreshold;
  LTZThreshold = threshold;
 }

 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->UseFixedLTZThreshold = value;

 return true;
}

// Устанавливает необходимость транзита сигнала от внешнего источника
bool NSpikeClassifier::SetUseTransitSignal(const bool &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->UseTransitSignal = value;
 }

 return true;
}

// Устанавливает необходимость транзита сигнала от внешнего источника
bool NSpikeClassifier::SetDataFromFile(const bool &value)
{
 if(!value)
 {
  fin.close();
  fout.close();
  IsFirstFileStep = true;
 }

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NSpikeClassifier* NSpikeClassifier::New(void)
{
 return new NSpikeClassifier;
}

UComponent* NSpikeClassifier::NewStatic(void)
{
 return new NSpikeClassifier;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NSpikeClassifier::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Признак необходимости чтения и записи данных из файла
bool NSpikeClassifier::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NSpikeClassifier::AReset(void)
{
 for(size_t i = 0; i < trainers.size(); i++)
 {
//  trainers[i]->IsNeedToTrain = IsNeedToTrain;

/*  UEPtr<NPulseGeneratorTransit> generator;
  for(int j = 0; j < NumInputDendrite; j++)
  {
   generator = trainers[i]->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
   if(!generator)
	return true;

   generator->UseTransitSignal = (IsNeedToTrain)? false : true;
  }       */

 // trainers[i]->Delay = Delay;
 // trainers[i]->SpikesFrequency = SpikesFrequency;
 // trainers[i]->NumInputDendrite = NumInputDendrite;
 // trainers[i]->MaxDendriteLength = MaxDendriteLength;
  //trainers[i]->LTZThreshold = LTZThreshold;
  //trainers[i]->FixedLTZThreshold = FixedLTZThreshold;
  //trainers[i]->UseFixedLTZThreshold = UseFixedLTZThreshold;
	 /*
  MDMatrix<double> pattern;
  pattern.Resize(NumInputDendrite,1);
  for(int j = 0; j < NumInputDendrite; j++)
   pattern[j] = TrainingPatterns(i,j);
  trainers[i]->InputPattern = pattern;
       */
 // trainers[i]->Reset();
 }
	 /*
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = Delay + InputPattern[i];
  generators[i]->Frequency = SpikesFrequency;
//  generators[i]->UseTransitSignal = UseTransitSignal;
  generators[i]->Reset();
 }   */

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NSpikeClassifier::ADefault(void)
{
 StructureBuildMode=1;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronTrainerClassName="NNeuronTrainer";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio";    // NPSynapse
 NumNeurons = 1;
 IsNeedToTrain = true;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 MaxDendriteLength = 100;
 LTZThreshold = 0.0117;
 TrainingLTZThreshold=100;
 FixedLTZThreshold = 0.0117;
 UseFixedLTZThreshold = true;
 UseTransitSignal = false;
 TrainingPatterns.Assign(1,1,0.0);
 InputPattern.Assign(1,1,0.0);
 DataFromFile = false;

 return true;
}

// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NSpikeClassifier::BuildStructure(void)
{
 // 1 - создаём группу нейронов, структурно адаптирующихся для распознавания заданного паттерна
 if(StructureBuildMode == 1)
 {
	// Удаляем старые нейроны с возможностью обучения
	for(int i = NumNeurons; i < OldNumNeurons; i++)
	{
		DelComponent(std::string("NeuronTrainer")+sntoa(i+1));
	}
	// Инициализируем нейроны с возможностью обучения
	OldNumNeurons = int(trainers.size());
	trainers.resize(NumNeurons);
	for(int i = 0; i < NumNeurons; i++)
	{
		trainers[i] = AddMissingComponent<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1), NeuronTrainerClassName);
		trainers[i]->SetCoord(MVector<double,3>(8.7+1*7,1.67+i*2,0));
		trainers[i]->NumInputDendrite = NumInputDendrite;
        trainers[i]->Reset();
		trainers[i]->LTZThreshold = LTZThreshold;
		trainers[i]->TrainingLTZThreshold = TrainingLTZThreshold;
        trainers[i]->FixedLTZThreshold = FixedLTZThreshold;

		MDMatrix<double> pattern;
		pattern.Resize(NumInputDendrite,1);
		for(int j = 0; j < NumInputDendrite; j++)
		 pattern[j] = TrainingPatterns(i,j);
		trainers[i]->InputPattern = pattern;
		trainers[i]->IsNeedToTrain = IsNeedToTrain;
		trainers[i]->Delay = Delay;
		trainers[i]->SpikesFrequency = SpikesFrequency;
 		trainers[i]->MaxDendriteLength = MaxDendriteLength;
		trainers[i]->Reset();
	}

	// Удаляем старые генераторы
	for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));
	}
	// Инициализируем генераторы импульсов
    //OldNumInputDendrite = int(generators.size());
	generators.resize(NumInputDendrite);
	for(int i = 0; i < NumInputDendrite; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), PulseGeneratorClassName);
		generators[i]->SetCoord(MVector<double,3>(6.7,1.67+i*2,0));
		generators[i]->UseTransitSignal = UseTransitSignal;
		generators[i]->Delay = Delay + InputPattern[i];
		generators[i]->Frequency = SpikesFrequency;
  		generators[i]->Reset();
	}
	// Разрываем выходные связи генераторов
	for(int i = 0; i < NumInputDendrite; i++)
	{
		generators[i]->DisconnectAll("Output");
	}

	// Добаляем связи между генераторами и нейронами с возможностью обучения
	UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
	UEPtr<NPulseGeneratorTransit> gen_in; // соответствующий вход нейрона с возможностью обучения
	for(int i = 0; i < NumInputDendrite; i++)
	{
	 for(int j = 0; j < NumNeurons; j++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(j+1)),true);
	  if(!trainer)
	   return true;
	  gen_in = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(i+1)),true);
	  if(!gen_in)
	   return true;

	  gen_in->UseTransitSignal=(IsNeedToTrain.v)? false : true;


	  bool res(true);
	  res&=CreateLink("Source"+sntoa(i+1),"Output",gen_in->GetLongName(this),"Input");
	  if(!res)
	   return true;
	 }
	}
 }

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NSpikeClassifier::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure();
  if(!res)
   return false;
 }

 OldNumNeurons=0;
 OldNumInputDendrite=0;
 return true;
}

// Функция для работы с файлами.
// Осуществляет чтение входных данных из файла,
// Обработку результатов и запись результатов в файл
bool NSpikeClassifier::TreatDataFromFile(void)
{
	// Признак первого входа в функцию
	if(IsFirstFileStep)
	{
        // Открываем файл для чтения данных
        fin.open(Environment->GetCurrentDataDir()+"input_data.txt");
		// Открываем файл для записи данных
        fout.open(Environment->GetCurrentDataDir()+"output_data.txt");
        // Флаг начала итерации
		is_first_iter = true;
		// Входные данные
		inputs.Assign(NumInputDendrite, 1, 0.0);

		IsFirstFileStep = false;
	}

	// Признак начала новой итерации
	if(is_first_iter)
	{
		// Время начала текущей итерации (сек)
		start_iter_time = Environment->GetTime().GetDoubleTime();
		// Ответы нейронов
		outputs.assign(NumNeurons,0);
		// Считываем входной вектор
		if(fin.eof())
		{
			fin.close();
			fout.close();
			IsFirstFileStep = true;
			DataFromFile = false;
			return true;
		}
		for(int i = 0; i < NumInputDendrite; i++)
		{
			double in;
			fin >> in;
			/*if(fin.eof())
			{
				fin.close();
                fout.close();
				DataFromFile = false;
				return true;
			} */
			inputs(i,0) = in;
		}
        InputPattern = inputs;

		is_first_iter = false;
	}

	// Фиксируем выходы нейронов
	UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
	UEPtr<NPulseNeuron> neuron;
	UEPtr<NLTZone> ltzone;
	for(int i= 0; i < NumNeurons; i++)
	{
		trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
		if(!trainer)
			return true;

		neuron  = trainer->GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
		if(!neuron)
			return true;

		ltzone = neuron->GetComponentL<NLTZone>("LTZone");
		if(!ltzone)
			return true;

		if(ltzone->Output(0,0) > 0.00001)
			outputs[i] = 1;
	}

	// Проверяем окончание итерации
    double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Записываем результаты в файл
		for(int i = 0; i < NumNeurons; i++)
		{
			fout << outputs[i] << "\t";
		}
		fout << std::endl;

		is_first_iter = true;
	}

	return true;
}

// Выполняет расчет этого объекта
bool NSpikeClassifier::ACalculate(void)
{
	UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
	UEPtr<NPulseGeneratorTransit> generator;  // генератор

	// Процесс обучения
	if(IsNeedToTrain)
	{
	 bool is_trained(true);

	 // Проверяем обученность всех нейронов
	 for(int i = 0; i < NumNeurons; i++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	  if(!trainer)
	   return true;

	  if(trainer->IsNeedToTrain)
	  {
	   is_trained = false;
	   break;
	  }
	 }

	 if(is_trained)
	 {
	  IsNeedToTrain = false;

	  // Переводим все генераторы в режим транзита
      for(int i = 0; i < NumNeurons; i++)
	  {
	   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	   if(!trainer)
		return true;

       trainer->LTZThreshold=FixedLTZThreshold;

	   for(int j = 0; j < NumInputDendrite; j++)
	   {
		generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
		if(!generator)
		 return true;

		generator->UseTransitSignal = true;
	   }

       // Обратные связи между нейронами с возможностью обучения
       UEPtr<NPulseNeuron> neuron;
       UEPtr<NPulseMembrane> soma;
       UEPtr<NPulseSynapse> synapse;
       UEPtr<NLTZone> ltzone;
       std::vector<int> inh_synapse_counter;
       inh_synapse_counter.assign(NumNeurons,1);
       for(int i = 0; i < NumNeurons; i++)
       {
        trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
        if(!trainer)
         return true;

        neuron  = trainer->GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
        if(!neuron)
         return true;

        ltzone = neuron->GetComponentL<NLTZone>("LTZone");
        if(!ltzone)
         return true;

        for(int j = 0; j < NumNeurons; j++)
        {
         if(i==j)
          continue;

         neuron  = GetComponentL<NPulseNeuron>(std::string("NeuronTrainer"+sntoa(j+1)+".Neuron"),true);
         if(!neuron)
          return true;

         for(int n = 0; n < NumInputDendrite; n++)
         {
          soma = neuron->GetComponentL<NPulseMembrane>(std::string("Soma"+sntoa(n+1)),true);
          if(!soma)
           return true;

          soma->NumInhibitorySynapses=NumNeurons-1;
          soma->Reset();
          synapse = soma->GetComponentL<NPulseSynapse>(std::string("InhSynapse")+RDK::sntoa(inh_synapse_counter[j]),true);
          if(!synapse)
           return true;

          bool res(true);
          res&=CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");
          if(!res)
           return true;
         }
         ++inh_synapse_counter[j];
        }
       }
	  }
	 }

	 return true;
	}
    // Процесс распознавания
	else
	{
	 bool is_trained(true);

	 // Проверяем обученность всех нейронов
	 for(int i = 0; i < NumNeurons; i++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	  if(!trainer)
	   return true;

	  if(trainer->IsNeedToTrain)
	  {
	   is_trained = false;
	   break;
	  }
	 }

	 if(!is_trained)
	 {
	  IsNeedToTrain = true;

	  // Переводим все генераторы в режим генерации
      for(int i = 0; i < NumNeurons; i++)
	  {
	   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	   if(!trainer)
		return true;

	   for(int j = 0; j < NumInputDendrite; j++)
	   {
		generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
		if(!generator)
		 return true;

		generator->UseTransitSignal = false;
	   }
	  }

	  return true;
	 }

		// Функция для работы с файлами.
		// Осуществляет чтение входных данных из файла,
		// Обработку результатов и запись результатов в файл
		if(DataFromFile)
			TreatDataFromFile();
	}

	return true;
}

// Устанавливает компоненты в требуемый порядок расчета
void NSpikeClassifier::UpdateComputationOrder(void)
{
 int position=0;
 for(size_t i=0;i<generators.size();i++)
  SetComponentPosition(generators[i]->GetName(),position++);
 for(size_t i=0;i<trainers.size();i++)
  SetComponentPosition(trainers[i]->GetName(),position++);
}
// --------------------------


}
#endif
