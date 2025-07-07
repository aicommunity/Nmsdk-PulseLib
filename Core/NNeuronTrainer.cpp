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

#ifndef NNEURONTRAINER_CPP
#define NNEURONTRAINER_CPP

#include "NNeuronTrainer.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
//#include <QString>
//#include <QDateTime>
//#include <QFile>

// Класс, создающий нейрон со структурой, оптимизованной для распознавания заданного паттерна импульсов
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronTrainer::NNeuronTrainer(void)
: StructureBuildMode("StructureBuildMode",this,&NNeuronTrainer::SetStructureBuildMode),
  CalculateMode("CalculateMode",this,&NNeuronTrainer::SetCalculateMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NNeuronTrainer::SetPulseGeneratorClassName),
  NeuronClassName("NeuronClassName",this,&NNeuronTrainer::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NNeuronTrainer::SetSynapseClassName),
  IsNeedToTrain("IsNeedToTrain",this,&NNeuronTrainer::SetNeedToTrain),
  Delay("Delay",this,&NNeuronTrainer::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NNeuronTrainer::SetSpikesFrequency),
  NumInputDendrite("NumInputDendrite",this,&NNeuronTrainer::SetNumInputDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NNeuronTrainer::SetMaxDendriteLength),
  InputPattern("InputPattern",this,&NNeuronTrainer::SetInputPattern),
  NeuronAmplitude("NeuronAmplitude",this),
  SomaNeuronAmplitude("SomaNeuronAmplitude",this),
  LTZThreshold("LTZThreshold",this,&NNeuronTrainer::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NNeuronTrainer::SetFixedLTZThreshold),
  TrainingLTZThreshold("TrainingLTZThreshold",this,&NNeuronTrainer::SetTrainingLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NNeuronTrainer::SetUseFixedLTZThreshold),
  Output("Output",this),
  SynapseResistanceStep("SynapseResistanceStep", this, &NNeuronTrainer::SetSynapseResistanceStep)
{
 OldNumInputDendrite=0;
 generators.clear();
 is_first_iter=true;
 neuron = NULL;
 thresh_first_iter=true;
 //dendrite = NULL;
 //synapse = NULL;
}

NNeuronTrainer::~NNeuronTrainer(void)
{
 OldNumInputDendrite=0;
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
bool NNeuronTrainer::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Режим расчёта нейрона
bool NNeuronTrainer::SetCalculateMode(const int &value)
{
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NNeuronTrainer::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NNeuronTrainer::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NNeuronTrainer::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Признак необходимости обучения
bool NNeuronTrainer::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  is_need_to_build=true;
  is_synchronizated=false;
  OldNumInputDendrite=0;
  generators.clear();
  is_first_iter=true;
  neuron = NULL;
  thresh_first_iter=true;
  DelAllComponents();
  Ready=false;
 }

 return true;
}


/// Время задержки начала обучения относительно старта системы (сек)
bool NNeuronTrainer::SetDelay(const double &value)
{
 return true;
}

/// Частота генераторов (Гц)
bool NNeuronTrainer::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Frequency = value;
  generators[i]->Reset();
 }

 return true;
}

/// Число входных дендритов
bool NNeuronTrainer::SetNumInputDendrite(const int &value)
{
 Ready=false;
 OldNumInputDendrite = NumInputDendrite;
 InputPattern.Resize(value,1);
 return true;
}

/// Максимальная длина дендрита
bool NNeuronTrainer::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// Паттерн для запоминания
bool NNeuronTrainer::SetInputPattern(const MDMatrix<double> &value)
{
 if(IsNeedToTrain)
 {
  is_need_to_build=true;
  is_synchronizated=false;
  Ready=false;
 }

 for(int i = 0; i < int(generators.size()); i++)
 {
  generators[i]->Delay = Delay + value[i];
  generators[i]->Reset();
 }

 return true;
}

/// Порог низкопороговой зоны нейрона
bool NNeuronTrainer::SetLTZThreshold(const double &value)
{
 UEPtr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
  UseFixedLTZThreshold = false;

 return true;
}

/// Порог низкопороговой зоны нейрона для этапа обучения
bool NNeuronTrainer::SetTrainingLTZThreshold(const double &value)
{
// UEPtr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
// if(!n_in)
//  return true;
//
// UEPtr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
// if(!ltzone)
//  return true;
//
// ltzone->Threshold = value;

 return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NNeuronTrainer::SetFixedLTZThreshold(const double &value)
{
 return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NNeuronTrainer::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  double threshold = FixedLTZThreshold;
  LTZThreshold = threshold;
 }

 return true;
}
/// Сопротивление нарастающих синапсов
bool NNeuronTrainer::SetSynapseResistanceStep(const double &value)
{
 return true;
}

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NNeuronTrainer* NNeuronTrainer::New(void)
{
 return new NNeuronTrainer;
}

UComponent* NNeuronTrainer::NewStatic(void)
{
 return new NNeuronTrainer;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NNeuronTrainer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 /*
 if(!NPulseNeuronCommon::AAddComponent(comp,pointer))
  return false;

 comp->SetMainOwner(this,-1);

  UEPtr<NConstGenerator> temp=dynamic_pointer_cast<NConstGenerator>(comp);

  if(temp && temp->Amplitude()>0)
  {
   if(PosGenerator)
	return false;
   PosGenerator=temp;
   return true;
  }
  else
  if(temp && temp->Amplitude()<0)
  {
   if(NegGenerator)
	return false;
   NegGenerator=temp;
   return true;
  }
				*/
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NNeuronTrainer::ADelComponent(UEPtr<UContainer> comp)
{                     /*
 if(comp == PosGenerator)
  PosGenerator=0;
 else
 if(comp == NegGenerator)
  NegGenerator=0;

 return NNeuron::ADelComponent(comp); */
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NNeuronTrainer::BuildStructure(int structure_build_mode, const string &pulse_generator_class_name,
									const string &neuron_class_name,
									int old_num_input_dendrite, int num_input_dendrite,
									const MDMatrix<double> &input_pattern)
{
 // 1 - создаём нейрон, настраивающий длину своих дендритов для максимального отклика по амплетуде на заданный паттерн импульсов
 if(StructureBuildMode == 1)
 {
	bool res(true);
    is_first_iter = true; // Флаг первой итерации (необходим для замера времени начала расчёта)
	max_iter_neuron_amp = 0; // Максимальная амплитуда выходного сигнала на одной итерации
	max_neuron_amp = 0; // Максимальная суммарная амплитуда
	dend_counter = 1; // Обрабатываемый по счёту дендрит
	is_new_dend = false; // Признак расчёта нового дендрита
	is_new_iteration = false; // Признак новой итерации (добавления участка дендрита)
	dend_index = -1; // Индекс обрабатываемого дендрит
	syn_counter = 0; // Обрабатываемый по счёту синапс = индекc+1 дендрита, на котором растим синапсы

	// Параметры для синхронизации по времени
	is_need_to_build = true; // Флаг необходимости досабрать нейрон

	NeuronAmplitude.Assign(1+num_input_dendrite,1,0.0);
	SomaNeuronAmplitude.Assign(1+num_input_dendrite,1,0.0);

	// Инициализация массива длин дендритов
	DendriteLength.assign(num_input_dendrite,0);
	// Массив выходов дендритов при единичной их длине
	InitialDendritePotential.assign(num_input_dendrite,0);
	// Массив количества синапсов на дендрите
	SynapseNum.assign(num_input_dendrite,1);

	// Вычисляем индекс дендрита, на который входной сигнал пришёл последним
	// и устанавливаем длину данного дендрита равной 1
	double max_delay = -1; // максимальное время задержки дендрита, относительно первого импулься
	for(int i = 0; i < num_input_dendrite; i++)
	{
		if(input_pattern[i] > max_delay)
		{
			max_delay = input_pattern[i];
			dend_index = i;
		}
	}
	DendriteLength[dend_index] = 1;

	// Удаляем старые генераторы и старые дендриты
	UEPtr<NPulseNeuron> neuron1 = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
	for(int i = num_input_dendrite; i < old_num_input_dendrite; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));

		neuron1->DelComponent(std::string("Dendrite")+sntoa(i+1)+"_1")  ;
	}

	// Инициализируем генераторы импульсов
    //OldNumInputDendrite = int(generators.size());
	generators.resize(num_input_dendrite);
	for(int i = 0; i < num_input_dendrite; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), pulse_generator_class_name);
		generators[i]->SetCoord(MVector<double,3>(6.7,1.67+i*2,0));
	}
	// Разрываем выходные связи генераторов
	for(int i = 0; i < num_input_dendrite; i++)
	{
		generators[i]->DisconnectAll("Output");
	}

	// Инициализируем нейрон
    if(IsNeedToTrain)
	{
		// создаём новый нейрон
		neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), neuron_class_name);
		neuron->SetCoord(MVector<double,3>(8.7+1*7,1.67,0));
		// устанавливаем количество дендритов
		neuron->NumSomaMembraneParts = num_input_dendrite;
		// устанавливаем длину дендритов
		if(neuron->StructureBuildMode != 2)
			neuron->NumDendriteMembraneParts = 1;
		else
		{
			std::vector<int> dend_length;
			dend_length.resize(num_input_dendrite);
			for(int i = 0; i < num_input_dendrite; i++)
				dend_length[i] = 1;
			neuron->NumDendriteMembranePartsVec = dend_length;
		}
		neuron->Reset();
	}

	// Добаляем связи между генератором и синапсами
	if(!neuron)
	 return true;
	UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_1.ExcSynapse1",true);
	if(!synapse)
	 return true;
	res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
	if(!res)
	 return true;
	neuron->Reset();
 }

 return true;
}

// Сброс процесса счета.
bool NNeuronTrainer::AReset(void)
{
 UEPtr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = LTZThreshold;

 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = Delay + InputPattern[i];
  generators[i]->Frequency = SpikesFrequency;
  generators[i]->Reset();
 }
 Output.ToZero();

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NNeuronTrainer::ADefault(void)
{
 generators.clear();
 StructureBuildMode=1;
 CalculateMode = 6;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio"; //NPSynapseBio
 IsNeedToTrain = true;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 OldNumInputDendrite = 0;
 MaxDendriteLength = 100;
 LTZThreshold = 0.0117;
 FixedLTZThreshold = 0.0117;
 TrainingLTZThreshold = 100;
 UseFixedLTZThreshold = true;
 InputPattern.Resize(1,1);
 NeuronAmplitude.Assign(1,1,0.0);
 SomaNeuronAmplitude.Assign(1,1,0.0);
 Output.Assign(1,1,0.0);
 SynapseResistanceStep = 1.0e9;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NNeuronTrainer::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, PulseGeneratorClassName,
						  NeuronClassName, OldNumInputDendrite, NumInputDendrite,
						  InputPattern);
  if(!res)
   return false;
 }

 OldNumInputDendrite=0;

 return true;
}

// Выполняется обучение путём наращивания длины дендритов
// путём максимизации амплитуды нейрона
/*bool NNeuronTrainer::MaximazeAmplitude(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 1 - обучение путём наращивания длины дендритов путём максимизации амплитуды нейрона
	// Если нейрон обучен - возвращаемся
	if(NumInputDendrite < dend_counter && CalculateMode < 2)
	{
		IsNeedToTrain = false;
		return true;
	}

	// Проверяем необходимость перейти к следующему дендриту
	if(is_new_dend)
	{
		// Вычисляем индекс дендрита, на который входной сигнал пришёл последним,
		// из ещё не обработанных
		double max_delay = -1;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			if(InputPattern[i] > max_delay && DendriteLength[i] == 0)
			{
				max_delay = InputPattern[i];
				dend_index = i;
			}
		}
		DendriteLength[dend_index] = 1;

		if(!neuron)
		 return true;
		UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_1.ExcSynapse1",true);
		if(!synapse)
		 return true;
		res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
		if(!res)
		 return true;
		neuron->Reset();

		is_new_dend = false;
		is_new_iteration = false;

		return true;
	}

	// Проверяем необходимость нарастить текущий дендрит
	if(is_new_iteration)
	{
		// Если возможно, наращиваем дендрит
		if(DendriteLength[dend_index] < MaxDendriteLength)
			DendriteLength[dend_index]++;
		// Если невозможно нарастить дендрит - переходим к следующему
		else
		{
			is_new_dend = true;
			dend_counter++;
			return true;
		}

		// Если необходимо, устанавливаем новуй длину дендритов
		bool is_need_to_increase = true;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			if(i==dend_index)
			 continue;
			if(DendriteLength[dend_index] <= DendriteLength[i])
			{
				is_need_to_increase = false;
				break;
			}
		}
		if(is_need_to_increase)
		{
			neuron->NumDendriteMembraneParts = DendriteLength[dend_index];
			neuron->Reset();
		}

		// Добаляем связь между текущим генератором и новым синапсом
		if(!neuron)
		 return true;

		UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index])+".ExcSynapse1",true);
		if(!synapse)
		 return true;

		res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
		if(!res)
		 return true;

		// Удаляем связь между текущим генератором и старым синапсом
		synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index]-1)+".ExcSynapse1",true);
		if(!synapse)
		 return true;

		res&=BreakLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
		if(!res)
		 return true;

		neuron->Reset();

		is_new_iteration = false;

		return true;
	}

	// Флаг первой итерации (необходим для замера времени начала расчёта)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();
		is_first_iter = false;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//max_neuron_amp_time = 0;
		//InitialDendritePotential[dend_index] = 0;
	}

	// Вычисляем максимальную амплитуду выходного сигнала
	{
		if(!neuron)
		 return true;
		// Амплитуда нейрона
		double neuron_amp;
		neuron_amp = neuron->DendriticSumPotential(0,0);
		if(neuron_amp > max_iter_neuron_amp)
		{
			max_iter_neuron_amp = neuron_amp;
			// !!!!!!!!!!!!!!!!!!!!!!!!!
			//max_neuron_amp_time = Environment->GetTime().GetDoubleTime();
		}

		// Для начального участка дендрита запоминаем выходную амплитуду
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if(DendriteLength[dend_index] == 1)
		{
			UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(dend_index+1)+"_1",true);
			if(!dendrite)
				return true;

			if(dendrite->SumPotential(0,0) > InitialDendritePotential[dend_index])
			{
				InitialDendritePotential[dend_index] = dendrite->SumPotential(0,0);
                // !!!!!!!!!!!!!!!!!!!!!!!
				//max_neuron_amp_time = Environment->GetTime().GetDoubleTime();
			}
		}
	}

	// Признак окончания итерации (роста дендрита)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Если выходная амплитуда нейрона увеличилась - растим дендрит дальше
		if(max_iter_neuron_amp > max_neuron_amp - 0.000005)
		{
			max_neuron_amp = max_iter_neuron_amp;
			NeuronAmplitude(0,0) = max_neuron_amp;
			is_new_iteration = true;
		}
		// Если нет - удаляем текущий участок и переходим к следующиму дендриту
		else
		{
			is_new_dend = true;
			dend_counter++;
			is_new_iteration = false;

			// Переключаем вход на предыдущий синапс
			// Добаляем связь между текущим генератором и старым синапсом
			if(!neuron)
			 return true;

			UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index]-1)+".ExcSynapse1",true);
			if(!synapse)
			 return true;

			res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
			if(!res)
			 return true;

			// Удаляем связь между текущим генератором и новым синапсом
			synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index])+".ExcSynapse1",true);
			if(!synapse)
			 return true;

			res&=BreakLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(this),"Input");
			if(!res)
			 return true;

			neuron->Reset();

			DendriteLength[dend_index]--;
		}

        // Флаг первой итерации (необходим для замера времени начала расчёта)
		is_first_iter = true;
		max_iter_neuron_amp = 0;

		return true;
	}
	else
		return true;

	////////////////////////////////////////////////////////////////////////////
	// 2 - обучение путём наращивания длины дендритов по амплитуде и количества синапсов на дендрите
*/	/*if(NumInputDendrite < dend_counter && CalculateMode == 2)
	{
        // Если нейрон обучен - возвращаемся
		if(NumInputDendrite < syn_counter)
		{
			IsNeedToTrain = false;
			return true;
		}

		// Проверяем необходимость перейти к следующему дендриту
		if(is_new_dend)
		{
			// Обрабатываемый по счёту синапс = индекc+1 дендрита, на котором растим синапсы
			syn_counter++;
			SynapseNum[syn_counter-1] = 1;

			is_new_dend = false;
			is_new_iteration = false;

			return true;
		}

		// Проверяем необходимость добавить синапс на дендрит
		if(is_new_iteration)
		{
			// Если возможно, наращиваем дендрит
			if(SynapseNum[syn_counter-1] < MaxDendriteLength)
				SynapseNum[syn_counter-1]++;
			// Если невозможно наростить дендрит - переходим к следующему
			else
			{
				is_new_dend = true;
				return true;
			}

			// Добавляем новый синапс
			if(!neuron)
			 return true;
			dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(syn_counter)+"_"+sntoa(DendriteLength[syn_counter-1]),true);
			if(!dendrite)
			 return true;
			synapse=AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[syn_counter-1])), SynapseClassName);
			if(!synapse)
			 return true;

			// Добаляем связь между текущим генератором и новым синапсом
			res&=CreateLink("Source"+sntoa(syn_counter),"Output",synapse->GetLongName(this),"Input");
			if(!res)
			 return true;

			neuron->Reset();

			is_new_iteration = false;

			return true;
		}

		// Флаг первой итерации (необходим для замера времени начала расчёта)
		if(is_first_iter)
		{
			start_iter_time = Environment->GetTime().GetDoubleTime();
			is_first_iter = false;
		}

		// Вычисляем максимальную амплитуду выходного сигнала
		{
			if(!neuron)
			 return true;
			dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(syn_counter)+"_1",true);
			if(!dendrite)
			 return true;

			// Амплитуда нейрона
			double neuron_amp;
			neuron_amp = dendrite->SumPotential(0,0);
			if(neuron_amp > max_iter_neuron_amp)
				max_iter_neuron_amp = neuron_amp;
		}

		// Признак окончания итерации (роста дендрита)
		double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
		double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
		if(iter_time >= iter_length)
		{
			// Если выходная амплитуда нейрона меньше начальной - добавляем синапсы дальше
			if(max_iter_neuron_amp < InitialDendritePotential[syn_counter-1] - 0.000005)
			{
				is_new_iteration = true;
			}
			// Если нет - удаляем текущий участок и переходим к следующиму дендриту
			else
			{
				is_new_dend = true;
				is_new_iteration = false;

				// Удаляем синапс
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(syn_counter)+"_"+sntoa(DendriteLength[syn_counter-1])+".ExcSynapse"+sntoa(SynapseNum[syn_counter-1]),true);
				if(!synapse)
				 return true;
				DelComponent(synapse->GetLongName(this));
				synapse = NULL;

				neuron->Reset();

				SynapseNum[dend_index]--;
			}

			// Флаг первой итерации (необходим для замера времени начала расчёта)
			is_first_iter = true;
			max_iter_neuron_amp = 0;

			return true;
		}
		else
			return true;
	}*/

/*	return true;
}*/

// Добавляем синапсы для компенсации потерь амплитуды при наращивании дендритов
/*bool NNeuronTrainer::SynapseNormalization(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 4 - Обучение путём наращивания длины дендритов по времени и количества синапсов на дендрите
	////////////////////////////////////////////////////////////////////////////

	//  Проверяем обученность нейрона
	bool is_trained = true;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		if(dend_status[i])
		{
			is_trained = false;
			break;
		}
	}
	if(is_trained)
	{
		IsNeedToTrain = false;
		// Паттерн, которому обучен нейрон
		neuron->TrainingPattern = InputPattern;
		// Индексы входных участков на дендритах
		neuron->TrainingDendIndexes.Resize(NumInputDendrite,1);
		for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingDendIndexes(i,0) = DendriteLength[i];
		}
		// Количество синапсов на входных участков на дендритах
		neuron->TrainingSynapsisNum.Resize(NumInputDendrite,1);
        for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingSynapsisNum(i,0) = SynapseNum[i];
		}

		return true;
	}

	// Проверяем необходимость добавить синапс на дендрит
	if(is_new_iteration)
	{
		// Для всех дендритов
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// Если амплитуда дендрита уже нормализована - переходим к следующиму
			if(!dend_status[i])
			 continue;
			// Добавляем синапс
			else if(dend_status[i] == 2)
			{
				// Проверяем условие на максимальное количество синапсов
				// ПАРАМЕТР НЕ ЗАВЕДЁН. НУЖЕН ЛИ?
				//if(DendriteLength[i] >= MaxDendriteLength)
				//{
				//	dend_status[i] = 0;
				//	continue;
				//}

				// Добавляем синапс
				SynapseNum[i]++;
				// Добавляем синапс
				if(!neuron)
				 return true;
				UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
				if(!dendrite)
				 return true;
				dendrite->NumExcitatorySynapses=SynapseNum[i]++;
                dendrite->Build();
				UEPtr<NPulseSynapse> synapse=dendrite->GetComponentL<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[i])), true);
				if(!synapse)
				 return true;

				synapse->Resistance = SynapseResistanceStep;
				// Добаляем связь между текущим генератором и новым синапсом
				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;
				// Добавляем связь между новым синапсом и ионным каналом
				std::string input_name = dendrite->GetLongName(this) + std::string(".ExcChannel");
				res&=CreateLink(synapse->GetLongName(this),"Output",input_name,"SynapticInputs");
				if(!res)
				 return true;

				neuron->Reset();
			}
			// Удаляем синапс
			else if(dend_status[i] == -2)
			{
				// Удаляем синапс
				SynapseNum[i]--;
				// Помечаем как посчитанный
				dend_status[i] = 0;

				// Удаляем синапс
				UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse"+sntoa(SynapseNum[i]+1),true);
				if(!synapse)
				 return true;
				DelComponent(synapse->GetLongName(this));
				synapse = NULL;

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// Флаг первой итерации (необходим для замера времени начала расчёта)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// Максимальная амплитуда выходного сигнала дендритов на одной итерации
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// Вычисляем максимальную амплитуду выходного сигнала каждого дендрита
	if(!neuron)
	 return true;

	// Амплитуда дендрита
	double dendrite_amp;
	// Ищем максимальные амплитуды дендритов
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		if(!dendrite)
			return true;

		dendrite_amp = dendrite->SumPotential(0,0);
		if(dendrite_amp > max_iter_dend_amp[i])
			max_iter_dend_amp[i] = dendrite_amp;
	}

	// Признак окончания итерации
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Для всех дендритов
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//double aaa = max_iter_dend_amp[i];
			//double aaaa = InitialDendritePotential[i];
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// Если амплитуда меньше начальной - добавляем синапсы дальше
			if(max_iter_dend_amp[i] < InitialDendritePotential[i] + 0.000005)
			{
				dend_status[i] = 2;
			}
			// В противном случае - удаляем синапс
			else
			{
				dend_status[i] = -2;
			}

			is_new_iteration = true;
		}

		// Флаг первой итерации (необходим для замера времени начала расчёта)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}

// Выполняется обучение путём наращивания длины дендритов
// путём синхронизации входного паттерна
bool NNeuronTrainer::SynchronizePattern(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 3 - Обучение путём наращивания длины дендритов путём максимального выравнивания времени прихода сигналов на сому
	// Если нейрон обучен - возвращаемся

	// При первом входе дособираем нейрон
	if(is_need_to_build)
	{
		// Вычисляем индекс дендрита, на который входной сигнал пришёл последним
    */	/*double max_delay = -1;
		for(int i = 0; i < NumInputDendrite; i++)
            DendriteLength[i] = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			if(!generators[i]->GetActivity())
				continue;
			if(InputPattern[i] > max_delay && DendriteLength[i] == 0)
			{
				max_delay = InputPattern[i];
				dend_index = i;
			}
		}
		if(max_delay == -1)
		{
		 is_synchronizated = false;
		 return true;
		}
		DendriteLength[dend_index] = 1; */

		// Добаляем связи между генератором и синапсами
        /*if(!neuron)
		 return true;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_1.ExcSynapse1",true);
			if(!synapse)
			 return true;
			res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
			if(!res)
			 return true;

			DendriteLength[i] = 1;
		}
		neuron->Reset();

		// Параметры для синхронизации по времени
		is_need_to_build = false; // Флаг необходимости досабрать нейрон
		max_dend_amp_time.assign(NumInputDendrite, 0); // Время нахождения максимальной амплитуды выходного сигнала дендритов на одной итерации
		double pepiod = 1.0 / SpikesFrequency + 0.001; // период итерации
		dissynchronization.assign(NumInputDendrite, pepiod); // Текущая рассинхронизация дендрита
		dissynchronization[dend_index] = 0;
		dend_status.assign(NumInputDendrite, 1); // Статус дендритов
		dend_status[dend_index] = 0;
		is_synchronizated = false; // Признак завершения синхронизации

		return true;
	}

	//  Проверяем обученность нейрона
	bool is_trained = true;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		if(dend_status[i])
		{
			is_trained = false;
			break;
		}
	}
	if(is_trained)
	{
		dend_status.assign(NumInputDendrite, 2); // Статус дендритов
		dend_status[dend_index] = 0;
		is_synchronizated = true; // Признак завершения синхронизации
		if(CalculateMode == 3)
		 IsNeedToTrain = false;
		return true;
	}

	// Проверяем необходимость наростить текущий дендрит
	if(is_new_iteration)
	{
		// Для всех дендритов
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// Если дендрит уже вырос - переходим к следующиму
			if(!dend_status[i])
			 continue;
			// Наращиваем дендрит
			else if(dend_status[i] == 1)
			{
				// Проверяем условие на максимальную длину дендритов
				if(DendriteLength[i] >= MaxDendriteLength)
				{
					dend_status[i] = 0;
					continue;
				}

				// Наращиваем дендрит
				DendriteLength[i]++;
				// Добавляем дендрит
				if(!neuron)
				 return true;

				if(neuron->StructureBuildMode != 2)
				{
					if(neuron->NumDendriteMembraneParts < DendriteLength[i])
					{
						neuron->NumDendriteMembraneParts = DendriteLength[i];
						neuron->Reset();
					}
				}
				else
				{
					std::vector<int> dend_length;
					dend_length.resize(NumInputDendrite);
					for(size_t n = 0; n < dend_length.size(); n++)
						dend_length[n] = neuron->NumDendriteMembranePartsVec[n];
					dend_length[i]++;
					neuron->NumDendriteMembranePartsVec = dend_length;
					neuron->Reset();
				}

				// Добаляем связь между текущим генератором и новым синапсом
				UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;

				// Удаляем связь между текущим генератором и старым синапсом
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]-1)+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;

				neuron->Reset();
			}
			// Укорачиваем дендрит
			else if(dend_status[i] == -1)
			{
                if(!neuron)
				 return true;

				// Укорачиваем дендрит
				DendriteLength[i]--;
				if(neuron->StructureBuildMode == 2)
				{
					if(neuron->NumDendriteMembranePartsVec[i] > 1)
					{
						std::vector<int> dend_length;
						dend_length.resize(NumInputDendrite);
						for(size_t n = 0; n < dend_length.size(); n++)
							dend_length[n] = neuron->NumDendriteMembranePartsVec[n];
						dend_length[i]--;
						neuron->NumDendriteMembranePartsVec = dend_length;
					}
					neuron->Reset();
				}

				// Помечаем как посчитанный
				dend_status[i] = 0;

				// Добаляем связь между текущим генератором и старым синапсом
				UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;

				// Удаляем связь между текущим генератором и новым синапсом
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]+1)+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// Флаг первой итерации (необходим для замера времени начала расчёта)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// Максимальная амплитуда выходного сигнала дендритов на одной итерации
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// Вычисляем максимальную амплитуду выходного сигнала каждого дендрита
	if(!neuron)
	 return true;

	// Амплитуда дендрита
	double dendrite_amp;
	// Ищем максимальные амплитуды дендритов
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		if(!dendrite)
			return true;

		dendrite_amp = dendrite->SumPotential(0,0);
		if(dendrite_amp > max_iter_dend_amp[i])
		{
			max_iter_dend_amp[i] = dendrite_amp;
			max_dend_amp_time[i] = Environment->GetTime().GetDoubleTime();

			// Для начального участка дендрита запоминаем выходную амплитуду
			if(DendriteLength[i] == 1)
			 InitialDendritePotential[i] = dendrite_amp;
		}
	}

	// Признак окончания итерации (роста дендрита)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Для всех дендритов
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// Текущий рассинхрон
			double dt = fabs(max_dend_amp_time[dend_index] - max_dend_amp_time[i]);

			// Рассинхрон уменьшается - растим дендрит дальше
			if(dt < dissynchronization[i])
			{
				dissynchronization[i] = dt;
				dend_status[i] = 1;
			}
			// В противном случае - укарачиваем дендрит
			else
			{
				dend_status[i] = -1;
			}

			is_new_iteration = true;
		}

		// Флаг первой итерации (необходим для замера времени начала расчёта)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}*/

// Добавляем синапсы для компенсации потерь амплитуды при наращивании дендритов
bool NNeuronTrainer::SomaSynapseNormalization(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// Обучение путём наращивания длины дендритов по времени и количества синапсов на дендрите
	//     в зависимости от потенциала на выходе с сомы
	////////////////////////////////////////////////////////////////////////////

	//  Проверяем обученность нейрона
	bool is_trained = true;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		if(dend_status[i])
		{
			is_trained = false;
			break;
		}
	}
	if(is_trained)
	{
		IsNeedToTrain = false;
//        const QString dateTime = QDateTime::currentDateTime().toString("hh:mm dd.MM.yyyy");
//        QFile file("test.txt");
//        if (file.open(QIODevice::WriteOnly))
//        {
//            file.write(dateTime.toUtf8());
//            file.close();
//        }
		// Паттерн, которому обучен нейрон
		neuron->TrainingPattern = InputPattern;
		// Индексы входных участков на дендритах
		neuron->TrainingDendIndexes.Resize(NumInputDendrite,1);
		for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingDendIndexes(i,0) = DendriteLength[i];
		}
		// Количество синапсов на входных участков на дендритах
		neuron->TrainingSynapsisNum.Resize(NumInputDendrite,1);
        for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingSynapsisNum(i,0) = SynapseNum[i];
		}

        double threshold = FixedLTZThreshold;
        LTZThreshold = threshold;
        //LTZThreshold=FixedLTZThreshold.v;
		return true;
	}

	// Проверяем необходимость добавить синапс на дендрит
	if(is_new_iteration)
	{
		// Для всех дендритов
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// Если амплитуда дендрита уже нормализована - переходим к следующиму
			if(!dend_status[i])
			 continue;
			// Добавляем синапс
			else if(dend_status[i] == 2)
			{
				// Проверяем условие на максимальное количество синапсов
				// ПАРАМЕТР НЕ ЗАВЕДЁН. НУЖЕН ЛИ?
				//if(DendriteLength[i] >= MaxDendriteLength)
				//{
				//	dend_status[i] = 0;
				//	continue;
				//}

				// Добавляем синапс
				SynapseNum[i]++;

				// Добавляем синапс
				if(!neuron)
				 return true;
				UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
				if(!dendrite)
				 return true;
				dendrite->NumExcitatorySynapses = dendrite->NumExcitatorySynapses + 1;
				dendrite->Build();
//				UEPtr<NPulseSynapse> synapse=dendrite->AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[i])), SynapseClassName);
//				if(!synapse)
//				 return true;

				// Добаляем связь между текущим генератором и новым синапсом
				NPulseSynapseCommon *synapse=dendrite->GetExcitatorySynapses(dendrite->NumExcitatorySynapses - 1);
				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
				if(!res)
				 return true;

				synapse->Resistance = SynapseResistanceStep;

				neuron->Reset();
			}
			// Удаляем синапс
			else if(dend_status[i] == -2)
			{
				// Удаляем синапс
				SynapseNum[i]--;
				// Помечаем как посчитанный
				dend_status[i] = 0;

				UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
				if(!dendrite)
				 return true;
				dendrite->NumExcitatorySynapses = dendrite->NumExcitatorySynapses - 1;
				dendrite->Build();


				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// Флаг первой итерации (необходим для замера времени начала расчёта)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// Максимальная амплитуда выходного сигнала дендритов на одной итерации
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// Вычисляем максимальную амплитуду выходного сигнала каждого дендрита
	if(!neuron)
	 return true;

	// Амплитуда дендрита
	double soma_amp;
	// Ищем максимальные амплитуды дендритов
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		if(!soma)
			return true;

		soma_amp = soma->SumPotential(0,0);
		if(soma_amp > max_iter_dend_amp[i])
			max_iter_dend_amp[i] = soma_amp;
	}

	// Признак окончания итерации
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Для всех дендритов
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
//			double aaa = max_iter_dend_amp[i];
//			double aaaa = InitialDendritePotential[i];
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// Если амплитуда меньше начальной - добавляем синапсы дальше
			if(max_iter_dend_amp[i] < InitialDendritePotential[i] + 0.000005)
			{
				dend_status[i] = 2;
			}
			// В противном случае - удаляем синапс
			else
			{
				dend_status[i] = -2;
			}

			is_new_iteration = true;
		}

		// Флаг первой итерации (необходим для замера времени начала расчёта)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}

// Выполняется обучение путём наращивания длины дендритов
// путём синхронизации входного паттерна на выходе сомы
bool NNeuronTrainer::SomaSynchronizePattern(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 6 - Обучение путём наращивания длины дендритов по времени и количества синапсов на дендрите
	//     в зависимости от потенциала на выходе сомы
	////////////////////////////////////////////////////////////////////////////
	// Если нейрон обучен - возвращаемся

	// При первом входе дособираем нейрон
	if(is_need_to_build)
	{
		// Добаляем связи между генератором и синапсами
		if(!neuron)
		 return true;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_1.ExcSynapse1",true);
			if(!synapse)
			 return true;
			res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
			if(!res)
			 return true;

			DendriteLength[i] = 1;
		}
		neuron->Reset();

		// Параметры для синхронизации по времени
		is_need_to_build = false; // Флаг необходимости досабрать нейрон
		max_dend_amp_time.assign(NumInputDendrite, 0); // Время нахождения максимальной амплитуды выходного сигнала с сом на одной итерации
		double period = 1.0 / SpikesFrequency + 0.001; // период итерации
		dissynchronization.assign(NumInputDendrite, period); // Текущая рассинхронизация сомы
		dissynchronization[dend_index] = 0;
		dend_status.assign(NumInputDendrite, 1); // Статус дендритов
		dend_status[dend_index] = 0;
		is_synchronizated = false; // Признак завершения синхронизации

		return true;
	}

	//  Проверяем обученность нейрона
	bool is_trained = true;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		if(dend_status[i])
		{
			is_trained = false;
			break;
		}
	}
	if(is_trained)
	{
		dend_status.assign(NumInputDendrite, 2); // Статус дендритов
		dend_status[dend_index] = 0;
		is_synchronizated = true; // Признак завершения синхронизации
		if(CalculateMode == 3)
		 IsNeedToTrain = false;
		return true;
	}

	// Проверяем необходимость наростить текущий дендрит
	if(is_new_iteration)
	{
		// Для всех дендритов
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// Если дендрит уже вырос - переходим к следующиму
			if(!dend_status[i])
			 continue;
			// Наращиваем дендрит
			else if(dend_status[i] == 1)
			{
				// Проверяем условие на максимальную длину дендритов
				if(DendriteLength[i] >= MaxDendriteLength)
				{
					dend_status[i] = 0;
					continue;
				}

				// Наращиваем дендрит
				DendriteLength[i]++;
				// Добавляем участок дендрита
				if(!neuron)
				 return true;

				if(neuron->StructureBuildMode != 2)
				{
					if(neuron->NumDendriteMembraneParts < DendriteLength[i])
					{
						neuron->NumDendriteMembraneParts = DendriteLength[i];
						neuron->Reset();
					}
				}
				else
				{
					std::vector<int> dend_length;
					dend_length.resize(NumInputDendrite);
					for(size_t n = 0; n < dend_length.size(); n++)
						dend_length[n] = neuron->NumDendriteMembranePartsVec[n];
					dend_length[i]++;
					neuron->NumDendriteMembranePartsVec = dend_length;
					neuron->Reset();

                    int temp_length2=neuron->NumDendriteMembranePartsVec[i];
                    if(dend_length[i] != temp_length2)
                    {
                     LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Incorrect dendtic length after INCREASE. Need "+sntoa(dend_length[i])+" current length "+sntoa(temp_length2));
                    }

//                    std::stringstream sss;
//                    for(size_t k=0;k<neuron->NumDendriteMembranePartsVec.size();k++)
//                     sss<<neuron->NumDendriteMembranePartsVec[k]<<",";
//                    LogMessageEx(RDK_EX_INFO,__FUNCTION__,"Dendritic lengths : "+sss.str());
                }

				// Добаляем связь между текущим генератором и новым синапсом
				UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
                 LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Synapse: Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1"+" not found");
                else
                {
                 res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
                 if(!res)
                  LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Falied create link: Source"+sntoa(i+1)+":Output -> "+synapse->GetLongName(this)+":Input");
                }
				// Удаляем связь между текущим генератором и старым синапсом
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]-1)+".ExcSynapse1",true);
                if(synapse)
                 BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");

				neuron->Reset();
			}
			// Укорачиваем дендрит
			else if(dend_status[i] == -1)
			{
                if(!neuron)
				 return true;

				// Укорачиваем дендрит
				DendriteLength[i]--;
				if(neuron->StructureBuildMode == 2)
				{
                    int temp_length1=0;
					if(neuron->NumDendriteMembranePartsVec[i] > 1)
					{
						std::vector<int> dend_length;
						dend_length.resize(NumInputDendrite);
						for(size_t n = 0; n < dend_length.size(); n++)
							dend_length[n] = neuron->NumDendriteMembranePartsVec[n];
						dend_length[i]--;
                        temp_length1=dend_length[i];
						neuron->NumDendriteMembranePartsVec = dend_length;
					}
					neuron->Reset();
                    int temp_length2=neuron->NumDendriteMembranePartsVec[i];
                    if(temp_length1 != temp_length2)
                    {
                     LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Incorrect dendtic length after DECREASE. Need "+sntoa(temp_length1)+" current length "+sntoa(temp_length2));
                    }
                }


				// Помечаем как посчитанный
				dend_status[i] = 0;

				// Добаляем связь между текущим генератором и старым синапсом
				UEPtr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
                 LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Synapse: Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1"+" not found");
                else
                {
                 res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");
                 if(!res)
                  LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Falied create link: Source"+sntoa(i+1)+":Output -> "+synapse->GetLongName(this)+":Input");
                }
				// Удаляем связь между текущим генератором и новым синапсом
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]+1)+".ExcSynapse1",true);
                if(synapse)
                 BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(this),"Input");

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// Флаг первой итерации (необходим для замера времени начала расчёта)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// Максимальная амплитуда выходного сигнала сом на одной итерации
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// Вычисляем максимальную амплитуду выходного сигнала каждой сомы
	if(!neuron)
	 return true;

	// Амплитуда сомы
	double soma_amp;
	// Ищем максимальные амплитуды на сомах
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		if(!soma)
			return true;

		soma_amp = soma->SumPotential(0,0);
		if(soma_amp > max_iter_dend_amp[i])
		{
			max_iter_dend_amp[i] = soma_amp;
			max_dend_amp_time[i] = Environment->GetTime().GetDoubleTime();

			// Для начального участка дендрита запоминаем выходную амплитуду
			if(DendriteLength[i] == 1)
			 InitialDendritePotential[i] = soma_amp;
		}
	}

	// Признак окончания итерации (роста дендрита)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации
	if(iter_time >= iter_length)
	{
		// Для всех дендритов
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// Текущий рассинхрон
			double dt = fabs(max_dend_amp_time[dend_index] - max_dend_amp_time[i]);

			// Рассинхрон уменьшается - растим дендрит дальше
			if(dt < dissynchronization[i])
			{
				dissynchronization[i] = dt;
				dend_status[i] = 1;
			}
			// В противном случае - укарачиваем дендрит
			else
			{
				dend_status[i] = -1;
			}

			is_new_iteration = true;
		}

		// Флаг первой итерации (необходим для замера времени начала расчёта)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}


// Процесс расчёта
bool NNeuronTrainer::CalculateProcess(void)
{
	bool res(true);
	//static bool first_iter = 1;//теперь флаг в конструкторе
	static MDMatrix<double> auto_puttern;
	static ofstream fout("C:\\Test_out2.txt"); // брать оттуда где проект
	static ifstream fin("C:\\Test.txt");// сохранять туда же где и проект?
	static double local_trainigLTZtresh = 0;

	//Меняем порог для этапа обучения
 if (IsNeedToTrain && thresh_first_iter)
	{
		local_trainigLTZtresh = TrainingLTZThreshold;
		LTZThreshold = local_trainigLTZtresh;
		thresh_first_iter = 0;
	}

	// выход N-ого дендрита
	// Для расчёта не нужно, может быть полезно для отладки и/или отображения
	NeuronAmplitude(0,0) = 0;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		NeuronAmplitude(i+1,0) = dendrite->SumPotential(0,0);
		NeuronAmplitude(0,0) = NeuronAmplitude(0,0) + /*8.0**/dendrite->SumPotential(0,0);
	}

	SomaNeuronAmplitude(0,0) = 0;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		UEPtr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		SomaNeuronAmplitude(i+1,0) = soma->SumPotential(0,0);
		SomaNeuronAmplitude(0,0) = SomaNeuronAmplitude(0,0) + /*8.0**/soma->SumPotential(0,0);
	}

	// Проверяем необходимость обучения
	if(CalculateMode==5)
	{
		static double time_spike = 0;
		if(!IsNeedToTrain)
		{
			UEPtr<NPulseLTZone> ltzone=neuron->GetComponentL<NPulseLTZone>("LTZone");
			if(!neuron)
				return true;
			if(!ltzone)
				return true;
			double time_now=Environment->GetTime().GetDoubleTime();
			const double step_thresh=0.0001;
			const double diff_time = 3;     //почему 3? 2 не работает
			if(ltzone->CheckPulseOn())
			{
				time_spike = Environment->GetTime().GetDoubleTime();
				LTZThreshold=LTZThreshold+step_thresh;
			}
			else if(((time_now - time_spike)>diff_time)&& time_spike)
			{
				for(int i=0;i<NumInputDendrite;++i)
				{
					fout << auto_puttern(i,0)<<" ";
				}
				double thresh = LTZThreshold - step_thresh;
				fout << thresh;
				fout << endl;
				time_spike=0;

				IsNeedToTrain=1;
				thresh_first_iter=1;

			}
			return true;
		}
	}
	else
	{
		if(!IsNeedToTrain)
		{
		//  порог после обучения надо возвращать вручную
			return true;
		}
	}


	// Выбираем способ расчёта
	switch(CalculateMode)
	{
	 case 0:
		return true;
		break;
	 case 1: case 2:
        // Выполняется обучение путём наращивания длины дендритов
		// путём максимизации амплитуды нейрона
        //res = MaximazeAmplitude();
        //return res;
        //break;
	 case 3:
		// Выполняется обучение путём наращивания длины дендритов
		// путём синхронизации входного паттерна
        //res = SynchronizePattern();
        //return res;
        //break;
	 case 4:
	 {
		// Выполняется обучение путём наращивания длины дендритов
		// путём синхронизации входного паттерна

        /*if(!is_synchronizated)
		 res = SynchronizePattern();
		// Добавляем синапсы для компенсации потерь амплитуды при наращивании дендритов
		if(is_synchronizated)
		 res = SynapseNormalization();
        return res;*/
        //break;
	 }
	 case 5:
		//Выполняется автоматический подбор порога срабатывания нейрона
		//для каждого автоматичеки сгенерированного паттерна.
		//В header подключены станд. библиотеки <iostream><fstream><cstring>
     {/*
		//позиция в файле
        static std::streamoff pos_file = 0;
		auto_puttern.Resize(NumInputDendrite,1);
		//проверка на открытие файлов и первую итерацию, чтобы задать паттерн из файла
		if (fin.is_open()&&thresh_first_iter&&fout.is_open())
		{
			//переход к запоненной позиции
			fin.seekg(pos_file);
			for(int i=0;i<NumInputDendrite;++i)
			{
				fin >> auto_puttern(i,0);
			}
			//запоминаем позицию, чтобы потом перейти к следующему паттерну
			pos_file = fin.tellg();
			thresh_first_iter=0;
		}
		InputPattern = auto_puttern;
		//начальное приближение порога
		LTZThreshold = 0.003; //если сразу порог будет превышен то не будет подбора!!!!!!!!!!
		if(!is_synchronizated)
		 res = SynchronizePattern();
		// Добавляем синапсы для компенсации потерь амплитуды при наращивании дендритов
		if(is_synchronizated)
		 res = SynapseNormalization();
        return res;*/
        //break;
	 }
	 case 6:
	 {
		// Выполняется обучение путём наращивания длины дендритов
		// путём синхронизации входного паттерна на выходе сомы

		if(!is_synchronizated)
		 res = SomaSynchronizePattern();
		// Добавляем синапсы для компенсации потерь амплитуды на выходе сомы при наращивании дендритов
		if(is_synchronizated)
		 res = SomaSynapseNormalization();
		return res;
		break;
	 }

	 default:
		return true;
	}

	return true;
}

// Выполняет расчет этого объекта
bool NNeuronTrainer::ACalculate(void)
{
	CalculateProcess();

 	// выход нейрона
	if(neuron)
	{
	 UEPtr<NLTZone> ltzone=neuron->GetComponentL<NLTZone>("LTZone");//GetLTZone();
	 if(!ltzone)
	  return true;

	 Output = ltzone->Output;
	}

	return true;
}

// Устанавливает компоненты в требуемый порядок расчета
void NNeuronTrainer::UpdateComputationOrder(void)
{
 int position=0;
 for(size_t i=0;i<generators.size();i++)
  SetComponentPosition(generators[i]->GetName(),position++);

 if(neuron)
  SetComponentPosition(neuron->GetName(),position++);
}
// --------------------------


}
#endif
