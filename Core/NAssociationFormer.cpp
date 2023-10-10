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

#ifndef NASSOCIATIONFORMER_CPP
#define NASSOCIATIONFORMER_CPP

#include "NAssociationFormer.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"


// Класс, создающий группу нейронов для моделирования формирования ассоциативных связей
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NAssociationFormer::NAssociationFormer(void)
: StructureBuildMode("StructureBuildMode",this,&NAssociationFormer::SetStructureBuildMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NAssociationFormer::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NAssociationFormer::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NAssociationFormer::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NAssociationFormer::SetSynapseClassName),
  BigNeuronSize("BigNeuronSize",this,&NAssociationFormer::SetBigNeuronSize),
  IsNeedToTrain("IsNeedToTrain",this,&NAssociationFormer::SetNeedToTrain),
  Delay("Delay",this,&NAssociationFormer::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NAssociationFormer::SetSpikesFrequency),
  NumUnconditionalStimulDendrite("NumStimulus2Dendrite",this,&NAssociationFormer::SetNumUnconditionalStimulDendrite),
  NumConditionalStimulDendrite("NumStimulus1Dendrite",this,&NAssociationFormer::SetNumConditionalStimulDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NAssociationFormer::SetMaxDendriteLength),
  UnconditionalStimulPattern("Stimulus2Pattern",this,&NAssociationFormer::SetUnconditionalStimulPattern),
  ConditionalStimulPattern("Stimulus1Pattern",this,&NAssociationFormer::SetConditionalStimulPattern),
  LTZThreshold("LTZThreshold",this,&NAssociationFormer::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NAssociationFormer::SetFixedLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NAssociationFormer::SetUseFixedLTZThreshold)
{

}

NAssociationFormer::~NAssociationFormer(void)
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
bool NAssociationFormer::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NAssociationFormer::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего учитель нейрона
bool NAssociationFormer::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NAssociationFormer::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NAssociationFormer::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Размер "большого" нейрона
bool NAssociationFormer::SetBigNeuronSize(const int &value)
{
 Ready=false;

 return true;
}

/// Признак необходимости обучения
bool NAssociationFormer::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  Ready=false;
 }

 return true;
}

/// Время задержки начала обучения относительно старта системы (сек)
bool NAssociationFormer::SetDelay(const double &value)
{
 return true;
}

/// Частота генераторов (Гц)
bool NAssociationFormer::SetSpikesFrequency(const double &value)
{
 return true;
}

/// Число входных дендритов для нейрона, формирующего второй стимул
bool NAssociationFormer::SetNumUnconditionalStimulDendrite(const int &value)
{
 Ready=false;

 UnconditionalStimulPattern.Resize(value,1);

 return true;
}

/// Число входных дендритов для нейрона, формирующего первый стимул
bool NAssociationFormer::SetNumConditionalStimulDendrite(const int &value)
{
 Ready=false;

 ConditionalStimulPattern.Resize(value,1);

 return true;
}

/// Максимальная длина дендрита
bool NAssociationFormer::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// Паттерн для запоминания  нейроном, формирующим второй стимул
bool NAssociationFormer::SetUnconditionalStimulPattern(const MDMatrix<double> &value)
{
 if(IsNeedToTrain)
 {
  Ready=false;
 }

 MDMatrix<double> pattern;
 pattern.Resize(NumUnconditionalStimulDendrite,1);

 for(int j = 0; j < NumUnconditionalStimulDendrite; j++)
	pattern[j] = value(j,0);
 UnconditionalStimul->InputPattern = pattern;
 UnconditionalStimul->Reset();

 return true;
}

/// Паттерн для запоминания  нейроном, формирующим первый стимул
bool NAssociationFormer::SetConditionalStimulPattern(const MDMatrix<double> &value)
{
 if(IsNeedToTrain)
 {
  Ready=false;
 }

 MDMatrix<double> pattern;
 pattern.Resize(NumConditionalStimulDendrite,1);

 for(int j = 0; j < NumConditionalStimulDendrite; j++)
	pattern[j] = value(j,0);
 ConditionalStimul->InputPattern = pattern;
 ConditionalStimul->Reset();

 return true;
}

/// Порог низкопороговой зоны нейрона
bool NAssociationFormer::SetLTZThreshold(const double &value)
{

 return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NAssociationFormer::SetFixedLTZThreshold(const double &value)
{

 return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NAssociationFormer::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  double threshold = FixedLTZThreshold;
  LTZThreshold = threshold;
 }

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NAssociationFormer* NAssociationFormer::New(void)
{
 return new NAssociationFormer;
}

UComponent* NAssociationFormer::NewStatic(void)
{
 return new NAssociationFormer;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NAssociationFormer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NAssociationFormer::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NAssociationFormer::AReset(void)
{

 UnconditionalStimul->Delay = Delay;
 UnconditionalStimul->SpikesFrequency = SpikesFrequency;
 UnconditionalStimul->MaxDendriteLength = MaxDendriteLength;
 UnconditionalStimul->LTZThreshold = LTZThreshold;
 UnconditionalStimul->FixedLTZThreshold = FixedLTZThreshold;
 UnconditionalStimul->UseFixedLTZThreshold = UseFixedLTZThreshold;
 UnconditionalStimul->NumInputDendrite = NumUnconditionalStimulDendrite;

 ConditionalStimul->Delay = Delay;
 ConditionalStimul->SpikesFrequency = SpikesFrequency;
 ConditionalStimul->MaxDendriteLength = MaxDendriteLength;
 ConditionalStimul->LTZThreshold = LTZThreshold;
 ConditionalStimul->FixedLTZThreshold = FixedLTZThreshold;
 ConditionalStimul->UseFixedLTZThreshold = UseFixedLTZThreshold;
 ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;

 LogicalAndNeuron->Delay = Delay;
 LogicalAndNeuron->SpikesFrequency = SpikesFrequency;
 LogicalAndNeuron->MaxDendriteLength = MaxDendriteLength;
 LogicalAndNeuron->LTZThreshold = LTZThreshold;
 LogicalAndNeuron->FixedLTZThreshold = FixedLTZThreshold;
 LogicalAndNeuron->UseFixedLTZThreshold = UseFixedLTZThreshold;
 LogicalAndNeuron->NumInputDendrite = 2;


 MDMatrix<double> pattern;

 pattern.Resize(NumUnconditionalStimulDendrite,1);
 for(int j = 0; j < NumUnconditionalStimulDendrite; j++)
	pattern[j] = UnconditionalStimulPattern(j,0);
 UnconditionalStimul->InputPattern = pattern;
 UnconditionalStimul->Reset();

 pattern.Resize(NumConditionalStimulDendrite,1);
 for(int j = 0; j < NumConditionalStimulDendrite; j++)
	pattern[j] = ConditionalStimulPattern(j,0);
 ConditionalStimul->InputPattern = pattern;
 ConditionalStimul->Reset();

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NAssociationFormer::ADefault(void)
{
 StructureBuildMode=1;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronTrainerClassName="NNeuronTrainer";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio";
 BigNeuronSize = 10;
 IsNeedToTrain = true;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumUnconditionalStimulDendrite = 1;
 NumConditionalStimulDendrite = 1;
 MaxDendriteLength = 100;
 LTZThreshold = 0.0115;
 FixedLTZThreshold = 0.0115;
 UseFixedLTZThreshold = true;
 UnconditionalStimulPattern.Assign(1,1,0.0);
 ConditionalStimulPattern.Assign(1,1,0.0);

 return true;
}

// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NAssociationFormer::BuildStructure(void)
{
 // 1 - Создаём группу нейронов для моделирования формирования ассоциативных связей
 if(StructureBuildMode == 1)
 {
	// Признак завершения обучения первому и второму раздражителям
	is_stimuls_trained = false;
	// Признак нахождения разности в приходе импульсов от первого и второго раздражителей
	is_interval_found = true;
	// Признак завершения обучения нейрона "И"
	is_and_neuron_trained = true;
	// Признак завершения обучения большого нейрона
	is_big_neuron_trained = true;


	// Создаём нейрон, формирующий первый стимул
	ConditionalStimul = AddMissingComponent<NNeuronTrainer>("Stimulus1", NeuronTrainerClassName);
	ConditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,1*2,0));
	ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;
    UEPtr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
	if(!ltZone)
		return true;
    //ltZone->TimeConstant = 0.0005;
	ConditionalStimul->Reset();

	// Создаём нейрон, формирующий второй стимул
	UnconditionalStimul = AddMissingComponent<NNeuronTrainer>("Stimulus2", NeuronTrainerClassName);
	UnconditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,5*2,0));
	UnconditionalStimul->NumInputDendrite = NumUnconditionalStimulDendrite;
    ltZone = UnconditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
	if(!ltZone)
		return true;
    //ltZone->TimeConstant = 0.0005;
	UnconditionalStimul->Reset();

	// Создаём нейрон, реализующий функцию "И"
	LogicalAndNeuron = AddMissingComponent<NNeuronTrainer>("AndNeuron", NeuronTrainerClassName);
	LogicalAndNeuron->SetCoord(MVector<double,3>(4.0+0.5*7,3*2,0));
	LogicalAndNeuron->NumInputDendrite = 2;
	LogicalAndNeuron->Reset();
	LogicalAndNeuron->IsNeedToTrain = false;
	// Переводим генераторы в режим транзита
	UEPtr<NPulseGeneratorTransit> generator;
	generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source1",true);
	if(!generator)
		return true;
	generator->UseTransitSignal = true;
	generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source2",true);
	if(!generator)
		return true;
	generator->UseTransitSignal = true;

	// Создаём "большой" нейрон
	BigNeuron = AddMissingComponent<NPulseNeuron>(std::string("TrainingNeuron"), NeuronClassName);
	BigNeuron->SetCoord(MVector<double,3>(4.0+1.5*7,3*2,0));
	BigNeuron->NumSomaMembraneParts = BigNeuronSize;
    ltZone = BigNeuron->GetComponentL<NPulseLTZoneThreshold>("LTZone",true);
	if(!ltZone)
		return true;
	double threshold = FixedLTZThreshold;
	ltZone->Threshold = threshold;
    BigNeuron->Reset();

	// Создаём нейрон, реагирующий на второй стимул
	LogicalOrNeuron = AddMissingComponent<NPulseNeuron>(std::string("Reaction2"), NeuronClassName);
	LogicalOrNeuron->SetCoord(MVector<double,3>(3.5+2.5*7,5*2,0));
	// Добавляем синапс для реализации функции "ИЛИ"
	UEPtr<NPulseMembrane> soma = LogicalOrNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
	if(!soma)
		return true;
	UEPtr<NPulseSynapse> synapse2 = soma->AddMissingComponent<NPulseSynapse>("ExcSynapse2", SynapseClassName);
	if(!synapse2)
		return true;
	UEPtr<NPulseSynapse> synapse1 = soma->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
	if(!synapse1)
		return true;
	RDK::MVector<double,3> coords = synapse1->GetCoord();
	synapse2->SetCoord(MVector<double,3>(coords(0)+7.0,coords(1),0));
	// Добавляем связь между новым синапсом и ионным каналом
	std::string input_name = soma->GetLongName(this) + std::string(".ExcChannel");
	bool res(true);
	res&=CreateLink(synapse2->GetLongName(this),"Output",input_name,"SynapticInputs");
	if(!res)
		return true;
	LogicalOrNeuron->Reset();

 	// Связываем нейрон, формирующий второй стимул, с нейроном, реагирующим на второй стимул
	UEPtr<NPulseNeuron> neuron;
	// Второй стимул
    UEPtr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone", true);
	if(!ltzone)
		return true;
	// Связываем c реагирующшим нейроном
	res&=CreateLink(ltzone->GetLongName(this),"Output",synapse1->GetLongName(this),"Input");
	if(!res)
		return true;

	// Создаём нейрон, реагирующий на первый стимул
	LogicalOrNeuron1 = AddMissingComponent<NPulseNeuron>(std::string("Reaction1"), NeuronClassName);
	LogicalOrNeuron1->SetCoord(MVector<double,3>(3.5+2.5*7,1*2,0));
	// Добавляем синапс для реализации функции "ИЛИ"
	soma = LogicalOrNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
	if(!soma)
		return true;
	synapse2 = soma->AddMissingComponent<NPulseSynapse>("ExcSynapse2", SynapseClassName);
	if(!synapse2)
		return true;
	synapse1 = soma->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
	if(!synapse1)
		return true;
	coords = synapse1->GetCoord();
	synapse2->SetCoord(MVector<double,3>(coords(0)+7.0,coords(1),0));
	// Добавляем связь между новым синапсом и ионным каналом
	input_name = soma->GetLongName(this) + std::string(".ExcChannel");
	res&=CreateLink(synapse2->GetLongName(this),"Output",input_name,"SynapticInputs");
	if(!res)
		return true;
	LogicalOrNeuron1->Reset();

	// Связываем нейрон, формирующий первый стимул, с нейроном, реагирующим на первый стимул
	// Первый стимул
	ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone", true);
	if(!ltzone)
		return true;
	// Связываем c реагирующшим нейроном
	res&=CreateLink(ltzone->GetLongName(this),"Output",synapse1->GetLongName(this),"Input");
	if(!res)
		return true;
 }

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NAssociationFormer::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure();
  if(!res)
   return false;
 }

 return true;
}

// Выполняет расчет этого объекта
bool NAssociationFormer::ACalculate(void)
{
	// I. Обучение первому и второму раздражителям
	if(!is_stimuls_trained)
	{
		if(!UnconditionalStimul->IsNeedToTrain && !ConditionalStimul->IsNeedToTrain)
		{
			is_stimuls_trained = true;

			// Признак нахождения разности в приходе импульсов от первого и второго раздражителей
			is_interval_found = false;
			is_first_spike = false;
			is_second_spike = false;

			return true;
		}
	}

	 // II. Ищем интервал между входными импульсами
	 if(!is_interval_found)
	 {
		// Ожидаем прихода первого спайка от первого раздражителя
		if(!is_first_spike)
		{
			UEPtr<NLTZone> ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			double value = ltzone->Output(0,0);
			if(value > 0)
			{
				is_first_spike = true;
				first_spike_time = Environment->GetTime().GetDoubleTime();
				if(is_second_spike)
					ready_time = first_spike_time;
			}
		}

		// Ожидаем прихода первого спайка от второго раздражителя
		if(!is_second_spike)
		{
			UEPtr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			double value = ltzone->Output(0,0);
			if(value > 0)
			{
				is_second_spike = true;
				second_spike_time = Environment->GetTime().GetDoubleTime();
				if(is_first_spike)
					ready_time = second_spike_time;
			}
		}

		// Если оба пришли:
		if(is_first_spike && is_second_spike)
		{
			double curr_time = Environment->GetTime().GetDoubleTime();
			if(curr_time - ready_time < Delay)
                return true;

			int type;
			if(second_spike_time > first_spike_time)
			{
				difference = second_spike_time - first_spike_time;
				type = 2;
			}
			else
			{
				difference = first_spike_time - second_spike_time;
				type = 1;
			}

			if(difference > ((1.0 / SpikesFrequency) / 2.0))
			{
				type = (type == 1)? 2 : 1;
				difference = (1.0 / SpikesFrequency) - difference;
			}

			// Задаём интервалы входного паттерна
			MDMatrix<double> pattern;
			pattern.Resize(LogicalAndNeuron->NumInputDendrite,1);
			if(type == 1)
			{
				pattern(0,0) = difference;
				pattern(1,0) = 0;
			}
			else
			{
				pattern(0,0) = 0;
				pattern(1,0) = difference;
			}
			LogicalAndNeuron->InputPattern = pattern;

			// Создаём связи первого и второго раздражителей с нейроном, реализующим функцию "И"
			UEPtr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// Нейрон "И"
			UEPtr<NPulseGeneratorTransit> generator; // соответствующий вход нейрона с возможностью обучения
			generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source2",true);
			if(!generator)
				return true;
			// Связываем
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(this),"Output",generator->GetLongName(this),"Input");
			if(!res)
				return true;
			// Первый раздражитель
			ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// Нейрон "И"
			generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source1",true);
			if(!generator)
				return true;
			// Связываем
			res = true;
			res&=CreateLink(ltzone->GetLongName(this),"Output",generator->GetLongName(this),"Input");
			if(!res)
				return true;

			// Активируем обучение нейрона "И"
			LogicalAndNeuron->IsNeedToTrain = true;
            // Признак завершения обучения нейрона "И"
			is_and_neuron_trained = false;
			// Признак нахождения разности в приходе импульсов от первого и второго раздражителей
			is_interval_found = true;
		}

		return true;
	 }

	 // III. Обучение нейрона, реализующего функцию "И"
	 if(!is_and_neuron_trained)
	 {
		if(!LogicalAndNeuron->IsNeedToTrain)
		{
			// Формируем связь между нейроном, реализующим функцию "И" и "большим" нейроном
			// Нейрон "И"
			UEPtr<NLTZone> ltzone = LogicalAndNeuron->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// "Большой" нейрон
			UEPtr<NPulseSynapse> synapse = BigNeuron->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse1",true);
			if(!synapse)
				return true;
			// Связываем
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");
			if(!res)
				return true;

			// Признак завершения обучения большого нейрона
			is_big_neuron_trained = false;
			// Признак одного спайка при наращивании синапсов на "большом" нейроне
			one_spike = false;
            // Текущее число синапсов на "большом" нейроне
			synapses_num = 1;
			// Признак завершения обучения нейрона "И"
			is_and_neuron_trained = true;
		}

		return true;
	 }

	 // IV. Обучение "большого" нейрона
	 if(!is_big_neuron_trained)
	 {
		// Если "большой нейрон ответил, завершаем обучение и формируем связь первого раздражителя с нейроном, реализующим функцию "ИЛИ" для второго стимула
		UEPtr<NLTZone> ltzone = BigNeuron->GetComponentL<NLTZone>("LTZone",true);
        if(!ltzone)
			return true;
		if(ltzone->Output(0,0) > 0)
		{
			// Формируем связь первого раздражителя с нейроном, реализующим функцию "ИЛИ" для второго стимула
			// Первый раздражитель
			ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// Нейрон, реализующий функцию "ИЛИ" для второго стимула
			UEPtr<NPulseSynapse> synapse2;
			synapse2 = LogicalOrNeuron->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse2",true);
            if(!synapse2)
				return true;
            // Связываем
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(this),"Output",synapse2->GetLongName(this),"Input");
			if(!res)
				return true;

			// Формируем связь второго раздражителя с нейроном, реализующим функцию "ИЛИ" для первого стимула
			// Второй раздражитель
			ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// Нейрон, реализующий функцию "ИЛИ" для первого стимула
			synapse2 = LogicalOrNeuron1->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse2",true);
            if(!synapse2)
				return true;
            // Связываем
			res&=CreateLink(ltzone->GetLongName(this),"Output",synapse2->GetLongName(this),"Input");
			if(!res)
				return true;

			// Признак завершения обучения большого нейрона
			is_big_neuron_trained = true;

			return true;
		}

		// Если есть вход с нейрона, реализующего функцию "И" - добавляем синапс
		ltzone = LogicalAndNeuron->GetComponentL<NLTZone>("Neuron.LTZone",true);
        if(!ltzone)
			return true;

		if(ltzone->Output(0,0) > 0 && !one_spike)
		{
			one_spike = true;
			synapses_num++;

			// Добавляем синапс на "большой" нейрон
			UEPtr<NPulseMembrane> soma = BigNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
			if(!soma)
				return true;
			UEPtr<NPulseSynapse> synapse = soma->AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(synapses_num)), SynapseClassName);
			if(!synapse)
				return true;
			UEPtr<NPulseSynapse> synapse1 = soma->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
			if(!synapse1)
				return true;
			RDK::MVector<double,3> coords = synapse1->GetCoord();
			synapse->SetCoord(MVector<double,3>(coords(0)+7.0*(synapses_num-1),coords(1),0));
			// Добавляем связь между новым синапсом и ионным каналом
			std::string input_name = soma->GetLongName(this) + std::string(".ExcChannel");
			bool res(true);
			res&=CreateLink(synapse->GetLongName(this),"Output",input_name,"SynapticInputs");
			if(!res)
				return true;
			// Добавляем связь между новым синапсом и выходом "И" нейрона
			res&=CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");
			if(!res)
				return true;
		}
		else if(one_spike && ltzone->Output(0,0) < 0.0000001)
		{
			one_spike = false;
		}

		return true;
	 }

	return true;
}
// --------------------------


}
#endif
