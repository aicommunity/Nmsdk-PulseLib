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

#ifndef NCLASSIFIER_CPP
#define NCLASSIFIER_CPP

#include "NClassifier.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseMembrane.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
//#include <QString>
//#include <QDateTime>
//#include <QFile>

// Класс, создающий группу обученных нейронов для распознавания заданного паттерна импульсов
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NClassifier::NClassifier(void)
: StructureBuildMode("StructureBuildMode",this,&NClassifier::SetStructureBuildMode),
  CalculateMode("CalculateMode",this,&NClassifier::SetCalculateMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NClassifier::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NClassifier::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NClassifier::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NClassifier::SetSynapseClassName),
  IsNeedToTrain("IsNeedToTrain",this,&NClassifier::SetNeedToTrain),
  Delay("Delay",this,&NClassifier::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NClassifier::SetSpikesFrequency),
  NumInputDendrite("NumInputDendrite",this,&NClassifier::SetNumInputDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NClassifier::SetMaxDendriteLength),
  TrainingPatterns("TrainingPatterns",this,&NClassifier::SetTrainingPatterns),
  InputPattern("InputPattern",this,&NClassifier::SetInputPattern),
  LTZThreshold("LTZThreshold",this,&NClassifier::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NClassifier::SetFixedLTZThreshold),
  TrainingLTZThreshold("TrainingLTZThreshold",this,&NClassifier::SetTrainingLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NClassifier::SetUseFixedLTZThreshold),
  UseTransitSignal("UseTransitSignal",this,&NClassifier::SetUseTransitSignal),
  NumClasses("NumClasses",this,&NClassifier::SetNumClasses),
  SizeTrainingSet("SizeTrainingSet",this,&NClassifier::SetSizeTrainingSet),
  DataFromFile("DataFromFile",this,&NClassifier::SetDataFromFile),
  UseLateralInhibition("UseLateralInhibition",this,&NClassifier::SetUseLateralInhibition)
{
 OldNumClasses=0;
 OldNumInputDendrite=0;
 OldSizeTrainingSet=0;
 generators.clear();
 groups_trainers.clear();

 // Переменные для чтение и записи данных в файл
 // ------->
 // Признак первой итерации
 IsFirstFileStep = true;
 // <-------
}

NClassifier::~NClassifier(void)
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
bool NClassifier::SetStructureBuildMode(const int &value)
{
	if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
		Ready=false;
	return true;
}

/// Режим расчёта нейрона
bool NClassifier::SetCalculateMode(const int &value)
{
    for(size_t i = 0; i < groups_trainers.size(); i++)
	{
        for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			 groups_trainers[i][j]->CalculateMode = value;
		}
	}
	return true;
}

/// Имя класса, создающего генератор импульсов
bool NClassifier::SetPulseGeneratorClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// Имя класса, создающего учитель нейрона
bool NClassifier::SetNeuronTrainerClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// Имя класса, создающего нейрон
bool NClassifier::SetNeuronClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// Имя класса, создающего синапс
bool NClassifier::SetSynapseClassName(const std::string &value)
{
	Ready=false;
	return true;
}


/// Признак необходимости обучения
bool NClassifier::SetNeedToTrain(const bool &value)
{
	if(value)
	{
	 OldNumClasses=0;
	 OldNumInputDendrite=0;
	 OldSizeTrainingSet=0;
	 generators.clear();
	 groups_trainers.clear();
     DelAllComponents();
	 Ready=false;
	}
	else
	{
	 UEPtr<NNeuronTrainer> trainer;
	 UEPtr<NPulseGeneratorTransit> generator;

	 for(int i = 0; i < NumClasses; i++)
	 {
		for(int j = 0; j < SizeTrainingSet; j++)
		{
			trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1)),true);
			if(!trainer)
				return true;
			trainer->IsNeedToTrain = value;
			for(int k = 0; k < NumInputDendrite; k++)
			{
				generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
				if(!generator)
					return true;

				generator->UseTransitSignal = (value)? false : true;
			}
		}
	 }
	}
	return true;
}

/// Время задержки начала обучения относительно старта системы (сек)
bool NClassifier::SetDelay(const double &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->Delay = value;
		}
	}

    for(size_t i = 0; i < generators.size(); i++)
	{
     generators[i]->Delay = Delay + InputPattern[i];
	 generators[i]->Reset();
	}
	return true;
}

/// Частота генераторов (Гц)
bool NClassifier::SetSpikesFrequency(const double &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->SpikesFrequency = value;
		}
	}

   	for(size_t i = 0; i < generators.size(); i++)
	{
		generators[i]->Frequency = value;
		generators[i]->Reset();
	}

	return true;
}

/// Число входных дендритов
bool NClassifier::SetNumInputDendrite(const int &value)
{
	Ready=false;

	OldNumInputDendrite = NumInputDendrite;
	InputPattern.Resize(value,1);
/*
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->NumInputDendrite = value;
			groups_trainers[i][j]->Reset();
		}
	}
	generators.resize(NumInputDendrite);
	*/
	TrainingPatterns.Resize(NumClasses*SizeTrainingSet,value);

	return true;
}

/// Максимальная длина дендрита
bool NClassifier::SetMaxDendriteLength(const int &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->MaxDendriteLength = value;
		}
	}
	return true;
}

/// Паттерн для запоминания
bool NClassifier::SetTrainingPatterns(const MDMatrix<double> &value)
{
	if(IsNeedToTrain)
	 Ready=false;
	else
	{
	 MDMatrix<double> pattern;
	 pattern.Resize(NumInputDendrite,1);
     for(int i = 0; i < int(groups_trainers.size()); i++)
	 {
        for(int j = 0; j < int(groups_trainers[i].size()); j++)
		{
			for(int k = 0; k < NumInputDendrite; k++)
			{
                pattern[k] = value(j+int(groups_trainers[i].size())*i,k);
				groups_trainers[i][j]->InputPattern = pattern;
			}
		}
	 }
	}

	return true;
}

/// Паттерн для распознавания
bool NClassifier::SetInputPattern(const MDMatrix<double> &value)
{
    for(int i = 0; i < int(generators.size()); i++)
	{
		generators[i]->Delay = Delay + value[i];
		generators[i]->Reset();
	}

	return true;
}

/// Порог низкопороговой зоны нейрона
bool NClassifier::SetLTZThreshold(const double &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->LTZThreshold = value;
		}
	}

	return true;
}

/// Порог низкопороговой зоны нейрона для этапа обучения
bool NClassifier::SetTrainingLTZThreshold(const double &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->TrainingLTZThreshold = value;
		}
	}

	return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NClassifier::SetFixedLTZThreshold(const double &value)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
			groups_trainers[i][j]->FixedLTZThreshold = value;
		}
	}

	return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NClassifier::SetUseFixedLTZThreshold(const bool &value)
{
	if(value)
	{
		double threshold = FixedLTZThreshold;
		LTZThreshold.SetDataDirect(threshold);
	}

	return true;
}

// Устанавливает необходимость транзита сигнала от внешнего источника
bool NClassifier::SetUseTransitSignal(const bool &value)
{
	for(size_t i = 0; i < generators.size(); i++)
	{
		generators[i]->UseTransitSignal = value;
	}

	return true;
}

/// Число классов
bool NClassifier::SetNumClasses(const int &value)
{
	Ready=false;
	OldNumClasses = NumClasses;
	TrainingPatterns.Resize(SizeTrainingSet*value,NumInputDendrite);
	return true;
}

/// Число каждого класса обучающей выборки
bool NClassifier::SetSizeTrainingSet(const int &value)
{
	Ready=false;
	OldSizeTrainingSet = SizeTrainingSet;
	TrainingPatterns.Resize(NumClasses*value,NumInputDendrite);
	return true;
}

// Устанавливает необходимость транзита сигнала от внешнего источника
bool NClassifier::SetDataFromFile(const bool &value)
{
 if(!value)
 {
  fin.close();
  fout.close();
  IsFirstFileStep = true;
 }

 return true;
}

bool NClassifier::SetUseLateralInhibition(const bool &value)
{
	Ready=false;
	return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NClassifier* NClassifier::New(void)
{
	return new NClassifier;
}

UComponent* NClassifier::NewStatic(void)
{
	return new NClassifier;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NClassifier::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
	return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NClassifier::ADelComponent(UEPtr<UContainer> comp)
{
	return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NClassifier::AReset(void)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
 // Serialized classifiers may already have IsNeedToTrain=false, so the
 // child trainer setter need not run while loading. Restore its inference
 // threshold after the trainer's own reset has completed.
 if(!IsNeedToTrain)
  groups_trainers[i][j]->LTZThreshold = FixedLTZThreshold;

 //			groups_trainers[i][j]->IsNeedToTrain = IsNeedToTrain;

 /*			UEPtr<NPulseGeneratorTransit> generator;
			for(int k = 0; k < NumInputDendrite; k++)
			{
				generator = groups_trainers[i][j]->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
				if(!generator)
					return true;

				generator->UseTransitSignal = (IsNeedToTrain)? false : true;
			}    */

 //			groups_trainers[i][j]->Delay = Delay;
 //			groups_trainers[i][j]->SpikesFrequency = SpikesFrequency;
 //			groups_trainers[i][j]->NumInputDendrite = NumInputDendrite;
 //			groups_trainers[i][j]->MaxDendriteLength = MaxDendriteLength;
 //			groups_trainers[i][j]->LTZThreshold = LTZThreshold;
 //			groups_trainers[i][j]->FixedLTZThreshold = FixedLTZThreshold;
 //			groups_trainers[i][j]->UseFixedLTZThreshold = UseFixedLTZThreshold;
		   /*
			MDMatrix<double> pattern;
			pattern.Resize(NumInputDendrite,1);

			for(int k = 0; k < NumInputDendrite; k++)
			{
				pattern[k] = TrainingPatterns(j+groups_trainers[i].size()*i,k);
			}
			groups_trainers[i][j]->InputPattern = pattern;
			groups_trainers[i][j]->Reset();   */
		}
	}

	for(size_t i = 0; i < generators.size(); i++)
	{
//		generators[i]->Delay = Delay + InputPattern[i];
//		generators[i]->Frequency = SpikesFrequency;
//		generators[i]->UseTransitSignal = UseTransitSignal;
//		generators[i]->Reset();
	}

	if(StructureBuildMode != 2)
		return true;

	// Child neurons are reset before this method runs. Reapply the classifier
	// output-layer topology after those resets, which can restore saved soma
	// defaults and discard the class-to-class inhibitory synapses.
	return RebuildOutputLayer();
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NClassifier::ADefault(void)
{
	groups_trainers.clear();
	generators.clear();

	StructureBuildMode=2;
	CalculateMode = 6;
	PulseGeneratorClassName="NPulseGeneratorTransit";
	NeuronTrainerClassName="NNeuronTrainer";
	NeuronClassName="NSPNeuronGen";
	SynapseClassName="NPSynapseBio";   //NPSynapse
	NumClasses = 1;
	SizeTrainingSet = 1;
	IsNeedToTrain = true;
	Delay = 0.1;
	SpikesFrequency = 1.5;
	NumInputDendrite = 1;
	//OldNumInputDendrite = NumInputDendrite;
	MaxDendriteLength = 100;
    LTZThreshold = 0.0117;
    FixedLTZThreshold = 0.0117;
	TrainingLTZThreshold = 100;
	UseFixedLTZThreshold = true;
	UseTransitSignal = false;
	TrainingPatterns.Assign(1,1,0.0);
	InputPattern.Assign(1,1,0.0);
	DataFromFile = false;
	UseLateralInhibition = true;

	return true;
}

// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NClassifier::BuildStructure(void)
{
	if(StructureBuildMode == 2)
	{
		// Удаляем старые нейроны
		for(int i = NumClasses; i < OldNumClasses; i++)
		{
			DelComponent(std::string("OrNeuron"+sntoa(i+1)));

			for(int j = 0; j < SizeTrainingSet; j++)
			{
				DelComponent(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1));
			}
		}

		for(int j = SizeTrainingSet; j < OldSizeTrainingSet; j++)
		{
			for(int i = 0; i < NumClasses; i++)
			{
				DelComponent(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1));
			}
		}

		// Инициализируем нейроны с возможностью обучения
		groups_trainers.resize(NumClasses);
		for(size_t i = 0; i < groups_trainers.size(); i++)
		{
			groups_trainers[i].resize(SizeTrainingSet);
		}

		for(int i = 0; i < NumClasses; i++)
		{
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				groups_trainers[i][j] = AddMissingComponent<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1), NeuronTrainerClassName);
				groups_trainers[i][j]->SetCoord(MVector<double,3>(8.7+1*7+i*20,1.67+j*2,0));
				groups_trainers[i][j]->CalculateMode = CalculateMode;
				groups_trainers[i][j]->NumInputDendrite = NumInputDendrite;
				groups_trainers[i][j]->LTZThreshold = LTZThreshold;
				groups_trainers[i][j]->TrainingLTZThreshold = TrainingLTZThreshold;
				groups_trainers[i][j]->FixedLTZThreshold = FixedLTZThreshold;				groups_trainers[i][j]->Reset();
				groups_trainers[i][j]->IsNeedToTrain = IsNeedToTrain;
				groups_trainers[i][j]->SpikesFrequency = SpikesFrequency;
				groups_trainers[i][j]->Delay = Delay;
				groups_trainers[i][j]->MaxDendriteLength = MaxDendriteLength;
				MDMatrix<double> pattern;
	 			pattern.Resize(NumInputDendrite,1);
				for(int k = 0; k < NumInputDendrite; k++)
				{
                 pattern[k] = TrainingPatterns(j+int(groups_trainers[i].size())*i,k);
				 groups_trainers[i][j]->InputPattern = pattern;
				 UEPtr<NPulseGeneratorTransit> generator = groups_trainers[i][j]->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
				 if(!generator)
				  continue;

				 generator->UseTransitSignal = (IsNeedToTrain.GetData())? false : true;
				}
    groups_trainers[i][j]->Reset();
			}
		}

		// Удаляем старые генераторы
		for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
		{
			DelComponent(std::string("Source")+sntoa(i+1));
		}

		// Инициализируем генераторы импульсов
		generators.resize(NumInputDendrite);
		for(int i = 0; i < NumInputDendrite; i++)
		{
			generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), PulseGeneratorClassName);
			generators[i]->SetCoord(MVector<double,3>(6.7,1.67+i*2,0));
			generators[i]->Frequency = SpikesFrequency;
			generators[i]->Delay = Delay + InputPattern[i];
			generators[i]->UseTransitSignal = UseTransitSignal;
			generators[i]->Reset();
		}

		// Разрываем выходные связи генераторов
		for(int i = 0; i < NumInputDendrite; i++)
		{
			generators[i]->DisconnectAll("Output");
		}

		// Добаляем связи между генераторами и нейронами с возможностью обучения
		for(int k = 0; k < NumInputDendrite; k++)
		{
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
					UEPtr<NPulseGeneratorTransit> gen_in; // соответствующий вход нейрона с возможностью обучения

					trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1)),true);
					if(!trainer)
						return false;
					gen_in = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
					if(!gen_in)
						return false;

					bool res(true);
					res&=CreateLink("Source"+sntoa(k+1),"Output",gen_in->GetLongName(this),"Input");
					if(!res)
						return false;
				}
			}
		}


		// Создаём нейроны ИЛИ
		for(int i = 0; i < NumClasses; i++)
		{
			LogicalOrNeuron = AddMissingComponent<NPulseNeuron>(std::string("OrNeuron"+sntoa(i+1)), NeuronClassName);
			LogicalOrNeuron->SetCoord(MVector<double,3>(8.7+1*7+i*20+10,(1.67+SizeTrainingSet)/2,0));
			// Finish the neuron's own structure before changing its soma synapse
			// counts; resetting the whole neuron afterwards can rebuild that soma.
			LogicalOrNeuron->Reset();

			UEPtr<NPulseMembrane> soma = LogicalOrNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
			if(!soma)
				return false;

			// Build the complete OR soma once, after the parent neuron is ready.
			soma->NumExcitatorySynapses=*SizeTrainingSet;//soma->NumExcitatorySynapses+1;
			soma->NumInhibitorySynapses = NumClasses - 1;
			soma->Reset();

			// Rebuilding from a saved model can preserve connections to old
			// same-name synapses. Replace those links for both OR input groups.
			for(int j = 1; j <= SizeTrainingSet; j++)
			{
				UEPtr<NPulseSynapseCommon> excitatory_synapse = soma->GetComponentL<NPulseSynapseCommon>(
				    std::string("ExcSynapse")+sntoa(j), true);
				if(!excitatory_synapse)
					return false;

				excitatory_synapse->Input.DetachFrom();
				excitatory_synapse->DisconnectAllItems();
			}

			// A saved classifier may already contain the older lateral links.
			// Detach them before adding the current class-to-class connections.
			for(int j = 1; j < NumClasses; j++)
			{
				UEPtr<NPulseSynapseCommon> inhibitory_synapse = soma->GetComponentL<NPulseSynapseCommon>(
				    std::string("InhSynapse")+sntoa(j), true);
				if(!inhibitory_synapse)
					return false;

				inhibitory_synapse->Input.DetachFrom();
				inhibitory_synapse->DisconnectAllItems();
			}

			// Connect the class-example neurons only after the OR soma has its
			// final, reset structure.
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				UEPtr<NNeuronTrainer> trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1)),true);
				if(!trainer)
					return false;

				UEPtr<NLTZone> ltzone = trainer->GetComponentL<NLTZone>("Neuron.LTZone", true);
				if(!ltzone)
					return false;

				NPulseSynapseCommon *synapse=soma->GetComponentL<NPulseSynapseCommon>(
				    std::string("ExcSynapse")+sntoa(j+1), true);
				if(!synapse)
					return false;

				bool res(true);
				res &= CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");
				if(!res)
					return false;
			}
		}

		// Обратные связи между нейронами ИЛИ
        for(int i = 0; UseLateralInhibition && i < NumClasses; i++)
		{
            UEPtr<NPulseNeuron> logical_or_neuron = GetComponentL<NPulseNeuron>(std::string("OrNeuron"+sntoa(i+1)),true);
            if(!logical_or_neuron)
				return false;

            UEPtr<NPulseMembrane> soma = logical_or_neuron->GetComponentL<NPulseMembrane>(std::string("Soma1"),true);
			if(!soma)
				return false;

			int indexSynapse = 0;

			for(int j = 0; j < NumClasses; j++)
			{
				if (i == j)
					continue;

				LogicalOrNeuron = GetComponentL<NPulseNeuron>(std::string("OrNeuron" + sntoa(j + 1)), true);
				if(!LogicalOrNeuron)
					return false;

				UEPtr<NLTZone> ltzone = LogicalOrNeuron->GetComponentL<NLTZone>("LTZone");
				 if(!ltzone)
					return false;

				UEPtr<NPulseSynapse> synapse = soma->GetComponentL<NPulseSynapse>(std::string("InhSynapse" + sntoa(indexSynapse + 1)), true);
				if(!synapse)
					return false;

				bool res(true);
				res &= CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");
				if(!res)
					return false;

				indexSynapse++;
			}
		}

		if(!RebuildOutputLayer())
			return false;

		OldNumClasses = int(groups_trainers.size());
		OldSizeTrainingSet = int(SizeTrainingSet);
	}

	return true;
}

// Rebuild the output layer after child components have been reset. Doing the
// work here makes it safe to call both during initial construction and from
// NClassifier::AReset(), which runs after child resets.
bool NClassifier::RebuildOutputLayer(void)
{
	for(int class_idx = 0; class_idx < NumClasses; class_idx++)
	{
		UEPtr<NPulseNeuron> or_neuron = GetComponentL<NPulseNeuron>(
		    std::string("OrNeuron")+sntoa(class_idx+1), true);
		if(!or_neuron)
			return false;

		UEPtr<NPulseMembrane> soma = or_neuron->GetComponentL<NPulseMembrane>("Soma1", true);
		if(!soma)
			return false;

		soma->NumExcitatorySynapses = SizeTrainingSet;
		soma->NumInhibitorySynapses = NumClasses - 1;
		if(!soma->Reset())
			return false;
		if(!InitializeOutputSoma(soma))
			return false;

		// Remove links restored from serialized classifiers before reconnecting
		// the class-example inputs and the lateral inhibition fan-in.
		for(int synapse_idx = 1; synapse_idx <= SizeTrainingSet; synapse_idx++)
		{
			UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
			    std::string("ExcSynapse")+sntoa(synapse_idx), true);
			if(!synapse)
				return false;
			synapse->Input.DetachFrom();
			synapse->DisconnectAllItems();
		}

		for(int synapse_idx = 1; synapse_idx < NumClasses; synapse_idx++)
		{
			UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
			    std::string("InhSynapse")+sntoa(synapse_idx), true);
			if(!synapse)
				return false;
			synapse->Input.DetachFrom();
			synapse->DisconnectAllItems();
		}

		for(int example_idx = 0; example_idx < SizeTrainingSet; example_idx++)
		{
			UEPtr<NNeuronTrainer> trainer = GetComponentL<NNeuronTrainer>(
			    std::string("NeuronTrainer")+sntoa(class_idx+1)+"_"+sntoa(example_idx+1), true);
			if(!trainer)
				return false;

			UEPtr<NLTZone> ltzone = trainer->GetComponentL<NLTZone>("Neuron.LTZone", true);
			if(!ltzone)
				return false;

			UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
			    std::string("ExcSynapse")+sntoa(example_idx+1), true);
			if(!synapse || !CreateLink(ltzone->GetLongName(this), "Output", synapse->GetLongName(this), "Input"))
				return false;
		}
	}

	for(int target_idx = 0; UseLateralInhibition && target_idx < NumClasses; target_idx++)
	{
		UEPtr<NPulseNeuron> target = GetComponentL<NPulseNeuron>(
		    std::string("OrNeuron")+sntoa(target_idx+1), true);
		if(!target)
			return false;
		UEPtr<NPulseMembrane> soma = target->GetComponentL<NPulseMembrane>("Soma1", true);
		if(!soma)
			return false;

		int inhibitory_synapse_idx = 1;
		for(int source_idx = 0; source_idx < NumClasses; source_idx++)
		{
			if(source_idx == target_idx)
				continue;

			UEPtr<NPulseNeuron> source = GetComponentL<NPulseNeuron>(
			    std::string("OrNeuron")+sntoa(source_idx+1), true);
			if(!source)
				return false;
			UEPtr<NLTZone> ltzone = source->GetComponentL<NLTZone>("LTZone", true);
			if(!ltzone)
				return false;

			UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
			    std::string("InhSynapse")+sntoa(inhibitory_synapse_idx++), true);
			if(!synapse || !CreateLink(ltzone->GetLongName(this), "Output", synapse->GetLongName(this), "Input"))
				return false;
		}
	}

	return true;
}

// Soma::Reset() can rebuild its channels and synapses after the application's
// initial component Init() pass. Explicitly initialize the complete output
// soma subtree before the classifier starts driving its inhibitory synapses.
bool NClassifier::InitializeOutputSoma(UEPtr<NPulseMembrane> soma)
{
	if(!soma)
		return false;

	if(!soma->IsInit())
		soma->Init();
	if(!soma->IsInit())
	{
		LogMessageEx(RDK_EX_ERROR, __FUNCTION__, "Failed to initialize classifier output soma.");
		return false;
	}

	const char *channel_names[] = {"ExcChannel", "InhChannel"};
	for(size_t i = 0; i < sizeof(channel_names) / sizeof(channel_names[0]); ++i)
	{
		UEPtr<NPulseChannelCommon> channel = soma->GetComponentL<NPulseChannelCommon>(channel_names[i], true);
		if(channel && !channel->IsInit())
			channel->Init();
		if(channel && !channel->IsInit())
		{
			LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
			             std::string("Failed to initialize output channel ") + channel_names[i] + ".");
			return false;
		}
	}

	for(int synapse_index = 1; synapse_index <= SizeTrainingSet; ++synapse_index)
	{
		UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
		    std::string("ExcSynapse") + sntoa(synapse_index), true);
		if(!synapse)
			return false;
		if(!synapse->IsInit())
			synapse->Init();
		if(!synapse->IsInit())
		{
			LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
			             std::string("Failed to initialize output synapse ") + synapse->GetLongName(this));
			return false;
		}
	}

	for(int synapse_index = 1; synapse_index < NumClasses; ++synapse_index)
	{
		UEPtr<NPulseSynapseCommon> synapse = soma->GetComponentL<NPulseSynapseCommon>(
		    std::string("InhSynapse") + sntoa(synapse_index), true);
		if(!synapse)
			return false;
		if(!synapse->IsInit())
			synapse->Init();
		if(!synapse->IsInit())
		{
			LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
			             std::string("Failed to initialize lateral synapse ") + synapse->GetLongName(this));
			return false;
		}
	}

	return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NClassifier::ABuild(void)
{
	if(StructureBuildMode>0)
	{
		bool res=BuildStructure();
		if(!res)
			return false;
	}

	OldNumClasses=0;
	OldNumInputDendrite=0;
	OldSizeTrainingSet=0;
	return true;
}


// Функция для работы с файлами.
// Осуществляет чтение входных данных из файла,
// Обработку результатов и запись результатов в файл
bool NClassifier::TreatDataFromFile(void)
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
        outputs.assign(NumClasses,0);
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
            double in = 0.0;
            if(!(fin >> in))
            {
                fin.close();
                fout.close();
                IsFirstFileStep = true;
                DataFromFile = false;
                return true;
            }
            inputs(i,0) = in;
        }
        InputPattern = inputs;

        is_first_iter = false;
    }

    // Фиксируем выходы нейронов
    //UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
    UEPtr<NPulseNeuron> neuron;
    UEPtr<NLTZone> ltzone;
    for(int i= 0; i < NumClasses; i++)
    {
        //trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
        //if(!trainer)
        //	return true;

        neuron  = GetComponentL<NPulseNeuron>(std::string("OrNeuron"+sntoa(i+1)),true);
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
        for(int i = 0; i < NumClasses; i++)
        {
            fout << outputs[i] << "\t";
        }
        fout << std::endl;

        is_first_iter = true;
    }

    return true;
}


// Выполняет расчет этого объекта
bool NClassifier::ACalculate(void)
{
	UEPtr<NNeuronTrainer> trainer; // нейрон с возможностью обучения
	UEPtr<NPulseGeneratorTransit> generator;  // генератор

	if(IsNeedToTrain)
	{
		bool is_trained(true);

		// Проверяем обученность всех нейронов
		for(int i = 0; i < NumClasses; i++)
		{
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1),true);
				if(!trainer)
					return true;

				if(trainer->IsNeedToTrain)
				{
					is_trained = false;
					break;
				}
			}
		}

		if(is_trained)
		{
			IsNeedToTrain = false;

			// Переводим все генераторы в режим транзита
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1),true);
					if(!trainer)
						return true;

		  			trainer->LTZThreshold=FixedLTZThreshold;
					for(int k = 0; k < NumInputDendrite; k++)
					{
						generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
						if(!generator)
							return true;

						generator->UseTransitSignal = true;
					}
				}
			}
		}

		return true;
	}
	else
	{
		bool is_trained(true);

		// Проверяем обученность всех нейронов
		for(int i = 0; i < NumClasses; i++)
		{
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1),true);
				if(!trainer)
					return true;

				if(trainer->IsNeedToTrain)
				{
					is_trained = false;
					break;
				}
			}
		}

		if(!is_trained)
		{
			IsNeedToTrain = true;

			// Переводим все генераторы в режим транзита
			// Проверяем обученность всех нейронов
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1)+"_"+sntoa(j+1),true);
					if(!trainer)
						return true;

					for(int k = 0; k < NumInputDendrite; k++)
					{
						generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(k+1)),true);
						if(!generator)
							return true;

						generator->UseTransitSignal = false;
					}
				}
			}
		}
		// Функция для работы с файлами.
		// Осуществляет чтение входных данных из файла,
		// Обработку результатов и запись результатов в файл
		if(DataFromFile)
			TreatDataFromFile();
		return true;
	}

	return true;
}

// --------------------------


}
#endif
