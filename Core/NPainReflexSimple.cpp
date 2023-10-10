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

#ifndef NPainReflexSimple_CPP
#define NPainReflexSimple_CPP

#include "NPainReflexSimple.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"


// Класс, создающий группу нейронов для моделирования условного рефлекса
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPainReflexSimple::NPainReflexSimple(void)
: StructureBuildMode("StructureBuildMode",this,&NPainReflexSimple::SetStructureBuildMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NPainReflexSimple::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NPainReflexSimple::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NPainReflexSimple::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NPainReflexSimple::SetSynapseClassName),
  //BigNeuronSize("BigNeuronSize",this,&NPainReflexSimple::SetBigNeuronSize),
  IsNeedToTrain("IsNeedToTrain",this,&NPainReflexSimple::SetNeedToTrain),
  Delay("Delay",this,&NPainReflexSimple::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NPainReflexSimple::SetSpikesFrequency),
  //NumUnconditionalStimulDendrite("NumUnconditionalStimulDendrite",this,&NPainReflexSimple::SetNumUnconditionalStimulDendrite),
  NumConditionalStimulDendrite("NumConditionalStimulDendrite",this,&NPainReflexSimple::SetNumConditionalStimulDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NPainReflexSimple::SetMaxDendriteLength),
  //UnconditionalStimulPattern("UnconditionalStimulPattern",this,&NPainReflexSimple::SetUnconditionalStimulPattern),
  //ConditionalStimulPattern("ConditionalStimulPattern",this,&NPainReflexSimple::SetConditionalStimulPattern),
  IsNegInfluence("IsNegInfluence",this,&NPainReflexSimple::SetNegInfluence),
  LTZThreshold("LTZThreshold",this,&NPainReflexSimple::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NPainReflexSimple::SetFixedLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NPainReflexSimple::SetUseFixedLTZThreshold),
  PainInput("PainInput",this)
{
 //OldNumNeurons=0;
 //OldNumInputDendrite=0;
}

NPainReflexSimple::~NPainReflexSimple(void)
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
bool NPainReflexSimple::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NPainReflexSimple::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего учитель нейрона
bool NPainReflexSimple::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NPainReflexSimple::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NPainReflexSimple::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Размер "большого" нейрона
/*
bool NPainReflexSimple::SetBigNeuronSize(const int &value)
{
 Ready=false;

 return true;
}
*/

/// Признак необходимости обучения
bool NPainReflexSimple::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  Ready=false;
 }

 return true;
}

/// Время задержки начала обучения относительно старта системы (сек)
bool NPainReflexSimple::SetDelay(const double &value)
{
 return true;
}

/// Частота генераторов (Гц)
bool NPainReflexSimple::SetSpikesFrequency(const double &value)
{
 return true;
}

/// Число входных дендритов для нейрона безусловного раздражителя
/*
bool NPainReflexSimple::SetNumUnconditionalStimulDendrite(const int &value)
{
 Ready=false;

 UnconditionalStimulPattern.Resize(value,1);

 return true;
}
*/

/// Число входных дендритов для нейрона условного раздражителя
bool NPainReflexSimple::SetNumConditionalStimulDendrite(const int &value)
{
 Ready=false;

 //ConditionalStimulPattern.Resize(value,1);
 //TODO: Fix it here??

 return true;
}

/// Максимальная длина дендрита
bool NPainReflexSimple::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// Паттерн для запоминания  нейроном безусловного раздражителя
/*
bool NPainReflexSimple::SetUnconditionalStimulPattern(const MDMatrix<double> &value)
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
*/

/// Паттерн для запоминания  нейроном условного раздражителя
/*
bool NPainReflexSimple::SetConditionalStimulPattern(const MDMatrix<double> &value)
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
*/

/// Характеристика воздействия
/// false - позитивное подкрепление
/// true - негативное подкрепление
bool NPainReflexSimple::SetNegInfluence(const bool &value)
{

 return true;
}

/// Порог низкопороговой зоны нейрона
bool NPainReflexSimple::SetLTZThreshold(const double &value)
{

 return true;
}

/// Фиксированный порог низкопороговой зоны нейрона
bool NPainReflexSimple::SetFixedLTZThreshold(const double &value)
{

 return true;
}

/// Признак необходимости использования фиксированного порога низкопороговой зоны нейрона
bool NPainReflexSimple::SetUseFixedLTZThreshold(const bool &value)
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
NPainReflexSimple* NPainReflexSimple::New(void)
{
 return new NPainReflexSimple;
}

UComponent* NPainReflexSimple::NewStatic(void)
{
 return new NPainReflexSimple;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPainReflexSimple::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPainReflexSimple::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Сброс процесса счета.
bool NPainReflexSimple::AReset(void)
{

 /*
 UnconditionalStimul->Delay = Delay;
 UnconditionalStimul->SpikesFrequency = SpikesFrequency;
 UnconditionalStimul->MaxDendriteLength = MaxDendriteLength;
 UnconditionalStimul->LTZThreshold = LTZThreshold;
 UnconditionalStimul->FixedLTZThreshold = FixedLTZThreshold;
 UnconditionalStimul->UseFixedLTZThreshold = UseFixedLTZThreshold;
 UnconditionalStimul->NumInputDendrite = NumUnconditionalStimulDendrite;
 */

 ConditionalStimul->Delay = Delay;
 ConditionalStimul->SpikesFrequency = SpikesFrequency;
 ConditionalStimul->MaxDendriteLength = MaxDendriteLength;
 ConditionalStimul->LTZThreshold = LTZThreshold;
 ConditionalStimul->FixedLTZThreshold = FixedLTZThreshold;
 ConditionalStimul->UseFixedLTZThreshold = UseFixedLTZThreshold;
 ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;

 /*
 LogicalAndNeuron->Delay = Delay;
 LogicalAndNeuron->SpikesFrequency = SpikesFrequency;
 LogicalAndNeuron->MaxDendriteLength = MaxDendriteLength;
 LogicalAndNeuron->LTZThreshold = LTZThreshold;
 LogicalAndNeuron->FixedLTZThreshold = FixedLTZThreshold;
 LogicalAndNeuron->UseFixedLTZThreshold = UseFixedLTZThreshold;
 LogicalAndNeuron->NumInputDendrite = 2;
 */

 MDMatrix<double> pattern;

 /*
 pattern.Resize(NumUnconditionalStimulDendrite,1);
 for(int j = 0; j < NumUnconditionalStimulDendrite; j++)
	pattern[j] = UnconditionalStimulPattern(j,0);
 UnconditionalStimul->InputPattern = pattern;
 UnconditionalStimul->Reset();
 */


 //pattern.Resize(NumConditionalStimulDendrite,1);
 //for(int j = 0; j < NumConditionalStimulDendrite; j++)
 //	pattern[j] = ConditionalStimulPattern(j,0);

 //Так ли оно должно быть ???
 ConditionalStimul->SetActivity(false);
 ConditionalStimul->Reset();

 start_iter_time = 0;
 is_first_iter=true;

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPainReflexSimple::ADefault(void)
{
 StructureBuildMode=1;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronTrainerClassName="NNeuronTrainer";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio";
 //BigNeuronSize = 10;
 IsNeedToTrain = true;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 //NumUnconditionalStimulDendrite = 1;
 NumConditionalStimulDendrite = 1;
 MaxDendriteLength = 100;
 IsNegInfluence = false;
 LTZThreshold = 0.0115;
 FixedLTZThreshold = 0.0115;
 UseFixedLTZThreshold = true;
 //UnconditionalStimulPattern.Assign(1,1,0.0);
 //ConditionalStimulPattern.Assign(1,1,0.0);
 return true;
}

// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NPainReflexSimple::BuildStructure(void)
{
 // 1 - Создаём группу нейронов для моделирования условного рефлекса
 if(StructureBuildMode == 1)
 {
     // Признак завершения обучения условному раздражителю
     is_conditional_stimulus_trained = false;

     // Создаём нейрон "Условный раздражитель"
     ConditionalStimul = AddMissingComponent<NNeuronTrainer>("ConditionalStimulus", NeuronTrainerClassName);
     ConditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,1*2,0));
     ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;
     //UEPtr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
     //if(!ltZone)
     //    return true;
     //ltZone->TimeConstant = 0.0005;
     ConditionalStimul->Reset();

     //Добираемся до условного рефлекса и настраиваем их как транзитные
     for(int i=0; i<NumConditionalStimulDendrite.v;i++)
     {
        UEPtr<NPulseGeneratorTransit> src = ConditionalStimul->GetComponentL<NPulseGeneratorTransit>("Source"+sntoa(i+1),true);
        if(!src)
            return true;
        src->UseTransitSignal=true;
     }

     //Создаем транзитный генератор для положительного входа
     NormalInputGen = AddMissingComponent<NPulseGeneratorTransit>(std::string("NormInputGen"), PulseGeneratorClassName);
     NormalInputGen->SetCoord(MVector<double,3>(4.0+0*7,2*2,0));
     NormalInputGen->UseTransitSignal=true;

     //Создаем транзитный генератор для альтернативного входа
     AltInputGen = AddMissingComponent<NPulseGeneratorTransit>(std::string("AltInputGen"), PulseGeneratorClassName);
     AltInputGen->SetCoord(MVector<double,3>(4.0+0*7,3*2,0));
     AltInputGen->UseTransitSignal=true;

     //Создаем транзитный генератор, блокирующий положительный вход
     NormalBlocker = AddMissingComponent<NPulseGeneratorTransit>(std::string("NormBlockerGen"), PulseGeneratorClassName);
     NormalBlocker->SetCoord(MVector<double,3>(4.0+1*7,1*2,0));
     //Настраиваем умноженный импульс (? правильно ли так ?)
     NormalBlocker->UseTransitSignal=false;
     NormalBlocker->Frequency=SpikesFrequency;
     NormalBlocker->UsePatternOutput=true;
     NormalBlocker->PatternDuration=0.6656; //??? скопировано с NLogicalNot
     NormalBlocker->PatternFrequency=500; //??? скопировано с NLogicalNot

     //Создаем транзитный генератор, блокирующий альтернативный вход
     AltBlocker = AddMissingComponent<NPulseGeneratorTransit>(std::string("AltBlockerGen"), PulseGeneratorClassName);
     AltBlocker->SetCoord(MVector<double,3>(4.0+2*7,2*2,0));
     //Настраиваем умноженный импульс (? правильно ли так ?)
     AltBlocker->UseTransitSignal=false;
     AltBlocker->Frequency=SpikesFrequency;
     AltBlocker->UsePatternOutput=true;
     AltBlocker->PatternDuration=0.6656; //??? скопировано с NLogicalNot
     AltBlocker->PatternFrequency=500; //??? скопировано с NLogicalNot

     // Создаём нейрон, контролирующий положительный выход
     std::vector<int> dend_size;
     dend_size.resize(1);
     dend_size[0] = 1;

     NormalOutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("NormOutputNeuron"), NeuronClassName);
     NormalOutputNeuron->SetCoord(MVector<double,3>(4.0+1*7,2*2,0));
     NormalOutputNeuron->NumDendriteMembranePartsVec = dend_size;
     NormalOutputNeuron->Reset();

     // Создаём нейрон, контролирующий альтернативный выход
     AltOutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("AltOutputNeuron"), NeuronClassName);
     AltOutputNeuron->SetCoord(MVector<double,3>(4.0+2*7,3*2,0));
     AltOutputNeuron->NumDendriteMembranePartsVec = dend_size;
     AltOutputNeuron->Reset();

     //Связать выход условного раздражителя и замедляющий вход
     UEPtr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
     //Так ли оно должно быть
     if(!ltZone)
         return true;

     // Добавляем связь между нейроном и разгоняющим генератором
     // **Временно убрал связывание, так как логика подсказывает, что во время обучения могут быть эксцессы (?) **
     // вынес в расчет, но мб потом верну
     //std::string input_name = NormalBlocker->GetLongName(this);
     bool res(true);
     //res&=CreateLink(ltZone->GetLongName(this),"Output",input_name,"Input");
     //if(!res)
     //    return true;

     //Добавляем связь между разгоняющим тормозный сигнал генератором и тормозящим синапсом
     // положительного нейрона "НЕ"
     UEPtr<NPulseMembrane> soma = NormalOutputNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!soma)
         return true;
     UEPtr<NPulseSynapse> inh_synapse_pos = soma->GetComponentL<NPulseSynapse>("InhSynapse1",true);
     if(!inh_synapse_pos)
         return true;
     res&=CreateLink(NormalBlocker->GetLongName(this),"Output",inh_synapse_pos->GetLongName(this),"Input");
     if(!res)
         return true;

     //Добавляем связь между положительным входящим генератором и возбуждающим
     // синапсом положительного нейрона "НЕ"
     UEPtr<NPulseMembrane> dend_1_1 = NormalOutputNeuron->GetComponentL<NPulseMembrane>("Dendrite1_1",true);
     if(!dend_1_1)
         return true;
     UEPtr<NPulseSynapse> exc_synapse_pos = dend_1_1->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
     if(!exc_synapse_pos)
         return true;
     res&=CreateLink(NormalInputGen->GetLongName(this),"Output",exc_synapse_pos->GetLongName(this),"Input");
     if(!res)
         return true;

     //Добавляем связь между LTZone положительного нейрона "НЕ" и разгоняющим
     // генератором для торможения отрицательного входа пока все нормально
     UEPtr<NPulseLTZoneThreshold> pos_not_ltZone = NormalOutputNeuron->GetComponentL<NPulseLTZoneThreshold>("LTZone",true);
     if(!pos_not_ltZone)
         return true;
     res&=CreateLink(pos_not_ltZone->GetLongName(this),"Output",AltBlocker->GetLongName(this),"Input");
     if(!res)
         return true;

     //Добавляем связь между выходом тормозящего отрицательный выход генератора и
     // тормозящим входом отрицательного нейрона "НЕ"
     UEPtr<NPulseMembrane> neg_soma = AltOutputNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!soma)
         return true;
     UEPtr<NPulseSynapse> inh_synapse_neg = neg_soma->GetComponentL<NPulseSynapse>("InhSynapse1",true);
     if(!inh_synapse_pos)
         return true;
     res&=CreateLink(AltBlocker->GetLongName(this),"Output",inh_synapse_neg->GetLongName(this),"Input");
     if(!res)
         return true;

     //Добавляем связь между выходом транзитного отрицательного генератора и
     // возбуждающим входом контролирующего отрицательный выход нейрона "НЕ"
     UEPtr<NPulseMembrane> dend_1_1_neg = AltOutputNeuron->GetComponentL<NPulseMembrane>("Dendrite1_1",true);
     if(!dend_1_1_neg)
         return true;
     UEPtr<NPulseSynapse> exc_synapse_neg = dend_1_1_neg->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
     if(!exc_synapse_pos)
         return true;
     res&=CreateLink(AltInputGen->GetLongName(this),"Output",exc_synapse_neg->GetLongName(this),"Input");
     if(!res)
         return true;
 }

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPainReflexSimple::ABuild(void)
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
bool NPainReflexSimple::ACalculate(void)
{
    if(is_first_iter)
    {
        is_first_iter=false;
        start_iter_time = Environment->GetTime().GetDoubleTime();
    }

    // I. Контроль боли и ожидание обучения условного рефлекса
    if(!is_conditional_stimulus_trained)
    {
        if(!ConditionalStimul->IsNeedToTrain)
        {
            is_conditional_stimulus_trained = true;

            //Связать выход условного раздражителя и замедляющий вход
            UEPtr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
            //Так ли оно должно быть
            if(!ltZone)
                return true;

            // Добавляем связь между нейроном и разгоняющим генератором
            std::string input_name = NormalBlocker->GetLongName(this);
            bool res(true);
            res&=CreateLink(ltZone->GetLongName(this),"Output",input_name,"Input");
            if(!res)
                return true;

            return true;
        }
        else
        {
            if(!PainInput.IsConnected())
            {
                return true;
            }

            //Если пришел болевой сигнал
            if((*PainInput)(0,0)>0)
            {
                //Включаем активность (обучение) нейрона условного раздражителя
                ConditionalStimul->SetActivity(true);
            }


            double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // Текущее время итерации
            double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // Длина одной итерации

            if(iter_time>=iter_length)
            {
                //TODO:  Что-то необходимо в рамках окончания итерации?

                //Сбрасываем флаг активности (обучения) у нейрона условного (болевого) стимула
                ConditionalStimul->SetActivity(false);
            }
        }
    }

    /*
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
    */
	return true;
}
// --------------------------


}
#endif
