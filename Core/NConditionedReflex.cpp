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

#ifndef NCONDITIONEDREFLEX_CPP
#define NCONDITIONEDREFLEX_CPP

#include "NConditionedReflex.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"


// Класс, создающий группу нейронов для моделирования условного рефлекса
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NConditionedReflex::NConditionedReflex(void)
: StructureBuildMode("StructureBuildMode",this,&NConditionedReflex::SetStructureBuildMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NConditionedReflex::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NConditionedReflex::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NConditionedReflex::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NConditionedReflex::SetSynapseClassName),
  BigNeuronSize("BigNeuronSize",this,&NConditionedReflex::SetBigNeuronSize),
  IsNeedToTrain("IsNeedToTrain",this,&NConditionedReflex::SetNeedToTrain),
  Delay("Delay",this,&NConditionedReflex::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NConditionedReflex::SetSpikesFrequency),
  NumUnconditionalStimulDendrite("NumUnconditionalStimulDendrite",this,&NConditionedReflex::SetNumUnconditionalStimulDendrite),
  NumConditionalStimulDendrite("NumConditionalStimulDendrite",this,&NConditionedReflex::SetNumConditionalStimulDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NConditionedReflex::SetMaxDendriteLength),
  UnconditionalStimulPattern("UnconditionalStimulPattern",this,&NConditionedReflex::SetUnconditionalStimulPattern),
  ConditionalStimulPattern("ConditionalStimulPattern",this,&NConditionedReflex::SetConditionalStimulPattern),
  IsNegInfluence("IsNegInfluence",this,&NConditionedReflex::SetNegInfluence),
  LTZThreshold("LTZThreshold",this,&NConditionedReflex::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NConditionedReflex::SetFixedLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NConditionedReflex::SetUseFixedLTZThreshold)
{
 //OldNumNeurons=0;
 //OldNumInputDendrite=0;
}

NConditionedReflex::~NConditionedReflex(void)
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
bool NConditionedReflex::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NConditionedReflex::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего учитель нейрона
bool NConditionedReflex::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NConditionedReflex::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NConditionedReflex::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Размер "большого" нейрона
bool NConditionedReflex::SetBigNeuronSize(const int &value)
{
 Ready=false;

 return true;
}

/// Признак необходимости обучения
bool NConditionedReflex::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  Ready=false;
 }

 return true;
}

/// Время задержки начала обучения относительно старта системы (сек)
bool NConditionedReflex::SetDelay(const double &value)
{
 return true;
}

/// Частота генераторов (Гц)
bool NConditionedReflex::SetSpikesFrequency(const double &value)
{
 return true;
}

/// Число входных дендритов для нейрона безусловного раздражителя
bool NConditionedReflex::SetNumUnconditionalStimulDendrite(const int &value)
{
 Ready=false;

 UnconditionalStimulPattern.Resize(value,1);

 return true;
}

/// Число входных дендритов для нейрона условного раздражителя
bool NConditionedReflex::SetNumConditionalStimulDendrite(const int &value)
{
 Ready=false;

 ConditionalStimulPattern.Resize(value,1);

 return true;
}

/// Максимальная длина дендрита
bool NConditionedReflex::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// Паттерн для запоминания  нейроном безусловного раздражителя
bool NConditionedReflex::SetUnconditionalStimulPattern(const MDMatrix<double> &value)
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

/// Паттерн для запоминания  нейроном условного раздражителя
bool NConditionedReflex::SetConditionalStimulPattern(const MDMatrix<double> &value)
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

/// Характеристика воздействия
/// false - позитивное подкрепление
/// true - негативное подкрепление
bool NConditionedReflex::SetNegInfluence(const bool &value)
{

 return true;
}

/// Порог низкопороговой зоны нейрона
bool NConditionedReflex::SetLTZThreshold(const double &value)
{

 return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NConditionedReflex::SetFixedLTZThreshold(const double &value)
{

 return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NConditionedReflex::SetUseFixedLTZThreshold(const bool &value)
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
NConditionedReflex* NConditionedReflex::New(void)
{
 return new NConditionedReflex;
}

UComponent* NConditionedReflex::NewStatic(void)
{
 return new NConditionedReflex;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NConditionedReflex::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NConditionedReflex::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NConditionedReflex::AReset(void)
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
bool NConditionedReflex::ADefault(void)
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
 IsNegInfluence = false;
 LTZThreshold = 0.0115;
 FixedLTZThreshold = 0.0115;
 UseFixedLTZThreshold = true;
 UnconditionalStimulPattern.Assign(1,1,0.0);
 ConditionalStimulPattern.Assign(1,1,0.0);

 return true;
}

// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NConditionedReflex::BuildStructure(void)
{
 // 1 - Создаём группу нейронов для моделирования условного рефлекса
 if(StructureBuildMode == 1)
 {
	// Признак завершения обучения условному и безусловному раздражителям
	is_stimuls_trained = false;
	// Признак нахождения разности в приходе импульсов от условного и безусловного раздражителей
	is_interval_found = true;
	// Признак завершения обучения нейрона "И"
	is_and_neuron_trained = true;
	// Признак завершения обучения большого нейрона
	is_big_neuron_trained = true;


	// Создаём нейрон "Условный раздражитель"
	ConditionalStimul = AddMissingComponent<NNeuronTrainer>("ConditionalStimulus", NeuronTrainerClassName);
	ConditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,1*2,0));
	ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;
    UEPtr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
	if(!ltZone)
		return true;
    //ltZone->TimeConstant = 0.0005;
	ConditionalStimul->Reset();

	// Создаём нейрон "Безсловный раздражитель"
	UnconditionalStimul = AddMissingComponent<NNeuronTrainer>("UnconditionalStimulus", NeuronTrainerClassName);
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
	BigNeuron = AddMissingComponent<NPulseNeuron>(std::string("ConditionalNeuron"), NeuronClassName);
	BigNeuron->SetCoord(MVector<double,3>(4.0+1.5*7,3*2,0));
	BigNeuron->NumSomaMembraneParts = BigNeuronSize;
    ltZone = BigNeuron->GetComponentL<NPulseLTZoneThreshold>("LTZone",true);
	if(!ltZone)
		return true;
	double threshold = FixedLTZThreshold;
	ltZone->Threshold = threshold;
    BigNeuron->Reset();

	// Создаём нейрон, реализующий "Безусловный рефлекс"
	LogicalOrNeuron = AddMissingComponent<NPulseNeuron>(std::string("UnconditionalNeuron"), NeuronClassName);
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

	// Связываем "Безусловный раздражитель" с "Безусловным рефлексом"
	UEPtr<NPulseNeuron> neuron;
	// Безусловный раздражитель
	UEPtr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone", true);
	if(!ltzone)
		return true;
	// Связываем c безусловным рефлексом
	res = true;
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
bool NConditionedReflex::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure();
  if(!res)
   return false;
 }

 //OldNumNeurons=0;
 //OldNumInputDendrite=0;
 return true;
}

// Выполняет расчет этого объекта
bool NConditionedReflex::ACalculate(void)
{
	// I. Обучение условному и безусловному раздражителям
	if(!is_stimuls_trained)
	{
		if(!UnconditionalStimul->IsNeedToTrain && !ConditionalStimul->IsNeedToTrain)
		{
			is_stimuls_trained = true;

            // Признак нахождения разности в приходе импульсов от условного и безусловного раздражителей
			is_interval_found = false;
			is_first_spike = false;
			is_second_spike = false;

			return true;
		}
	}

	 // II. Ищем интервал между входными импульсами
	 if(!is_interval_found)
	 {
		// Ожидаем прихода первого спайка от условного раздражителя
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

		// Ожидаем прихода первого спайка от безусловного раздражителя
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

			// Создаём связи условного и безусловного раздражителей с нейроном, реализующим функцию "И"
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
			// Условный раздражитель
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
            // Признак нахождения разности в приходе импульсов от условного и безусловного раздражителей
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
		// Если "большой нейрон ответил, завершаем обучение и формируем связь условного раздражителя с безусловным рефлексом
		UEPtr<NLTZone> ltzone = BigNeuron->GetComponentL<NLTZone>("LTZone",true);
        if(!ltzone)
			return true;
		if(ltzone->Output(0,0) > 0)
		{
			// Формируем связь условного раздражителя с безусловным рефлексом
			// Условный раздражитель
			ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// Безусловный рефлекс
			UEPtr<NPulseSynapse> synapse2;
			if(!IsNegInfluence)
				synapse2 = LogicalOrNeuron->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse2",true);
			else
                synapse2 = LogicalOrNeuron->GetComponentL<NPulseSynapse>("Soma1.InhSynapse1",true);
			if(!synapse2)
				return true;
            // Связываем
			bool res(true);
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
