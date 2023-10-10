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

#ifndef NLOGICALNOT_CPP
#define NLOGICALNOT_CPP

#include "NNeuronTrainer.h"
#include "NLogicalNot.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include <QString>


// Класс, создающий группу нейронов для моделирования условного рефлекса
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLogicalNot::NLogicalNot(void)
: StructureBuildMode("StructureBuildMode",this,&NLogicalNot::SetStructureBuildMode),
  CalculateMode("CalculateMode",this,&NLogicalNot::SetCalculateMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NLogicalNot::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NLogicalNot::SetNeuronTrainerClassName),
  LogicalNotClassName("LogicalNotClassName",this,&NLogicalNot::SetLogicalNotClassName),
  NeuronClassName("NeuronClassName",this,&NLogicalNot::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NLogicalNot::SetSynapseClassName),
  IsNeedToTrain("IsNeedToTrain",this,&NLogicalNot::SetNeedToTrain),
  SpikesFrequency("SpikesFrequency",this,&NLogicalNot::SetSpikesFrequency),
  NumInputDendrite("NumInputDendrite",this,&NLogicalNot::SetNumInputDendrite),
  InputPattern("InputPattern",this,&NLogicalNot::SetInputPattern),
  DendriteLengthNGen1("DendriteLengthNGen1",this,&NLogicalNot::SetDendriteLengthNGen1),
  DendriteLengthNGen2("DendriteLengthNGen2",this,&NLogicalNot::SetDendriteLengthNGen2),
  NumExcSynapsesNGen1("NumExcSynapsesNGen1",this,&NLogicalNot::SetNumExcSynapsesNGen1),
  NumExcSynapsesNGen2("NumExcSynapsesNGen2",this,&NLogicalNot::SetNumExcSynapsesNGen2),
  PatternFrequency("PatternFrequency",this,&NLogicalNot::SetPatternFrequency)
{

}

NLogicalNot::~NLogicalNot(void)
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
bool NLogicalNot::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Режим расчёта нейрона
bool NLogicalNot::SetCalculateMode(const int &value)
{
 return true;
}

/// Имя класса, создающего генератор импульсов
bool NLogicalNot::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего модель логического "НЕ" (модель боли)
bool NLogicalNot::SetLogicalNotClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего учитель нейрона
bool NLogicalNot::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NLogicalNot::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NLogicalNot::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число входных дендритов
bool NLogicalNot::SetNumInputDendrite(const int &value)
{
 Ready=false;
 if(NeuronTrainer)
 {
    NeuronTrainer->NumInputDendrite = value;
    NeuronTrainer->Reset();
    InputPattern.Resize(value,1);
 }
 return true;
}

/// Признак необходимости обучения
bool NLogicalNot::SetNeedToTrain(const bool &value)
{
    if(NeuronTrainer)
    {
        if(NeuronTrainer->IsNeedToTrain)
        {
            Ready=false;
        }
        NeuronTrainer->IsNeedToTrain = value;
    }
    return true;
}


/// Частота генераторов (Гц)
bool NLogicalNot::SetSpikesFrequency(const double &value)
{
    Ready=false;
    if(NeuronTrainer && PatternGenerator)
    {
        NeuronTrainer->SpikesFrequency = value;
        double pulse_length = PatternGenerator->PulseLength;
        PatternGenerator->PatternDuration = (1/(NeuronTrainer->SpikesFrequency)) - pulse_length;
    }
 return true;
}


/// Паттерн для запоминания
bool NLogicalNot::SetInputPattern(const MDMatrix<double> &value)
{          
    if(NeuronTrainer)
    {
        if(NeuronTrainer->IsNeedToTrain)
        {
            Ready=false;
        }
        NeuronTrainer->InputPattern = value;
    }
  return true;
}

/// Частота генератора в режиме повышенной частоты (UsePatternOutput) (Гц)
bool NLogicalNot::SetPatternFrequency(const double &value)
{
    Ready=false;
    if(PatternGenerator)
    {
        PatternGenerator->PatternFrequency = value;
    }
 return true;
}

/// Длина дендрита на первом нейроне в генераторной связке (число сегментов)
bool NLogicalNot::SetDendriteLengthNGen1(const int &value)
{
    Ready=false;
    if (GeneratorNeuron1)
    {
        std::vector<int> dend_size;
        dend_size.resize(1);
        dend_size[0] = value;
        GeneratorNeuron1->NumDendriteMembranePartsVec = dend_size;

        //Удаляем лишние синапсы на соме
        UEPtr<NPulseMembrane> soma = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
        if(!soma)
         return true;
        soma->NumExcitatorySynapses = 1;
        soma->Reset();

        //Удаляем лишние синапсы на дендритах
        for (int i = 0; i< value; i++)
        {
            UEPtr<NPulseMembrane> dendrite = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
            if(!dendrite)
             return true;
            dendrite->NumExcitatorySynapses = 1;
            dendrite->Reset();
        }

        GeneratorNeuron1->Reset();

    }
 return true;
}

/// Длина дендрита на втором нейроне в генераторной связке (число сегментов)
bool NLogicalNot::SetDendriteLengthNGen2(const int &value)
{
   Ready=false;
   if (GeneratorNeuron2)
   {
       std::vector<int> dend_size;
       dend_size.resize(1);
       dend_size[0] = value;
       GeneratorNeuron2->NumDendriteMembranePartsVec = dend_size;

       //Удаляем лишние синапсы на соме
       UEPtr<NPulseMembrane> soma = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
       if(!soma)
        return true;
       soma->NumExcitatorySynapses = 1;
       soma->Reset();

       //Удаляем лишние синапсы на дендритах
       for (int i = 0; i< value; i++)
       {
           UEPtr<NPulseMembrane> dendrite = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
           if(!dendrite)
            return true;
           dendrite->NumExcitatorySynapses = 1;
           dendrite->Reset();
       }

       GeneratorNeuron2->Reset();
   }
 return true;
}

/// Число синапсов на дендрите первого нейрона в генераторной связке
bool NLogicalNot::SetNumExcSynapsesNGen1(const int &value)
{
    Ready=false;
    if (GeneratorNeuron1)
    {
        //Устанавливаем необходимое число синапсов на соме/окончании дендрита
        if(DendriteLengthNGen1 == 0)
        {
            UEPtr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma1)
             return true;
            soma1->NumExcitatorySynapses = value;
            soma1->Build();
        }
        else
        {
            int dend_length1 = DendriteLengthNGen1;
            UEPtr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length1),true);
            if(!dendrite1)
             return true;
            dendrite1->NumExcitatorySynapses = value;
            dendrite1->Build();
        }

        GeneratorNeuron1->Reset();
    }
 return true;
}

/// Число синапсов на дендрите второго нейрона в генераторной связке
bool NLogicalNot::SetNumExcSynapsesNGen2(const int &value)
{
    Ready=false;
    if (GeneratorNeuron2)
    {
        //Устанавливаем необходимое число синапсов на соме/окончании дендрита
        if(DendriteLengthNGen2 == 0)
        {
            UEPtr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma2)
             return true;
            soma2->NumExcitatorySynapses = value;
            soma2->Build();
        }
        else
        {
            int dend_length2 = DendriteLengthNGen2;
            UEPtr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length2),true);
            if(!dendrite2)
             return true;
            dendrite2->NumExcitatorySynapses = value;
            dendrite2->Build();
        }

        GeneratorNeuron2->Reset();
    }
  return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NLogicalNot* NLogicalNot::New(void)
{
 return new NLogicalNot;
}

UComponent* NLogicalNot::NewStatic(void)
{
 return new NLogicalNot;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NLogicalNot::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NLogicalNot::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NLogicalNot::BuildStructure(int structure_build_mode, const string &pulse_generator_class_name,
                                 const string &neuron_class_name, int num_input_dendrite,
                                 double spikes_frequency,
                                 int num_exc_synapses_ngen1,int num_exc_synapses_ngen2,
                                 int dendrite_length_ngen1, int dendrite_length_ngen2,
                                 const MDMatrix<double> &input_pattern)
{
    if(StructureBuildMode == 1)
    {
        bool res(true);


        //Создаем генератор стартового импульса для генераторной связки нейронов
        Generator = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source"), PulseGeneratorClassName);
        Generator->SetCoord(MVector<double,3>(4.3, 2.67, 0));
        Generator->Frequency = 0.001;
        Generator->Delay = 0.1;
        Generator->Reset();


        //Создаем нейроны генераторной связки
        //Neuron1
        GeneratorNeuron1 = AddMissingComponent<NPulseNeuron>(std::string("Neuron1"), NeuronClassName);
        GeneratorNeuron1->SetCoord(MVector<double,3>(11.6, 2.3, 0));
        std::vector<int> dend_size1;
        dend_size1.resize(1);
        dend_size1[0] = dendrite_length_ngen1;
        GeneratorNeuron1->NumDendriteMembranePartsVec = dend_size1;
        GeneratorNeuron1->Reset();

        if(dendrite_length_ngen1 == 0)
        {
            UEPtr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma1)
             return true;
            soma1->NumExcitatorySynapses = num_exc_synapses_ngen1;
            soma1->Build();
        }
        else
        {
            UEPtr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dendrite_length_ngen1),true);
            if(!dendrite1)
             return true;
            dendrite1->NumExcitatorySynapses = num_exc_synapses_ngen1;
            dendrite1->Build();
        }
        GeneratorNeuron1->Reset();


        //Создаем нейроны генераторной связки
        //Neuron2
        GeneratorNeuron2 = AddMissingComponent<NPulseNeuron>(std::string("Neuron2"), NeuronClassName);
        GeneratorNeuron2->SetCoord(MVector<double,3>(11.6, 4.6, 0));
        std::vector<int> dend_size2;
        dend_size2.resize(1);
        dend_size2[0] = dendrite_length_ngen2;
        GeneratorNeuron2->NumDendriteMembranePartsVec = dend_size2;
        GeneratorNeuron2->Reset();
        if(dendrite_length_ngen2 == 0)
        {
            UEPtr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma2)
             return true;
            soma2->NumExcitatorySynapses = num_exc_synapses_ngen2;
            soma2->Build();
        }
        else
        {
            UEPtr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dendrite_length_ngen2),true);
            if(!dendrite2)
             return true;
            dendrite2->NumExcitatorySynapses = num_exc_synapses_ngen2;
            dendrite2->Build();
        }
         GeneratorNeuron2->Reset();


        //Создаем обучаемый нейрон
        NeuronTrainer = AddMissingComponent<NNeuronTrainer>(std::string("NeuronTrainer"), NeuronTrainerClassName);
        NeuronTrainer->SetCoord(MVector<double,3>(4.3, 9.67, 0));
        NeuronTrainer->NumInputDendrite = num_input_dendrite;
        NeuronTrainer->SpikesFrequency = spikes_frequency;
        NeuronTrainer->InputPattern = input_pattern;
        NeuronTrainer->Reset();


        //Создем генератор, продлевающий тормозящее воздействие NeuronTrainer в схеме
        PatternGenerator = AddMissingComponent<NPulseGeneratorTransit>(std::string("PatternSource"), PulseGeneratorClassName);
        PatternGenerator->SetCoord(MVector<double,3>(11.67, 9.67, 0));
        PatternGenerator->UsePatternOutput = true;
        PatternGenerator->Frequency = 0;
        PatternGenerator->PatternFrequency = PatternFrequency;

        double pulse_length = PatternGenerator->PulseLength;
        PatternGenerator->PatternDuration = (1/(NeuronTrainer->SpikesFrequency)) - pulse_length;
        PatternGenerator->Reset();


        //Создаем выходной нейрон
        OutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
        OutputNeuron->SetCoord(MVector<double,3>(18.3, 7.0, 0));
        std::vector<int> dend_size3;
        dend_size3.resize(1);
        dend_size3[0] = 1;
        OutputNeuron->NumDendriteMembranePartsVec = dend_size3;
        OutputNeuron->Reset();


        //Создаем связь между генератором стартового импульса и первым нейроном из генераторной связки
        //Generator -> Neuron1
        UEPtr<NPulseSynapse> synapse1=GeneratorNeuron1->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse1",true);
        if(!synapse1)
         return true;
        res&=CreateLink("Source","Output",synapse1->GetLongName(this),"Input");
        if(!res)
         return true;
        Generator->Reset();


        //Создаем связь между нейронами из генераторной связки
        //Neuron1 -> Neuron2
        UEPtr<NLTZone> ltzone1 = GeneratorNeuron1->GetComponentL<NLTZone>("LTZone", true);
        if(!ltzone1)
            return true;

        synapses2.resize(num_exc_synapses_ngen2);
        for (int i = 0; i < num_exc_synapses_ngen2; i++)
        {
            if(dendrite_length_ngen2 == 0)
            {
                synapses2[i] = GeneratorNeuron2->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse"+sntoa(i+1),true);
            }
            else
            {
                synapses2[i] = GeneratorNeuron2->GetComponentL<NPulseSynapse>("Dendrite1_"+sntoa(dendrite_length_ngen2)+".ExcSynapse"+sntoa(i+1),true);
            }
            if(!synapses2[i])
                return true;

            res&=CreateLink(ltzone1->GetLongName(this),"Output",synapses2[i]->GetLongName(this),"Input");
            if(!res)
                return true;
        }
        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();


        //Создаем обратную связь между нейронами из генераторной связки
        //Neuron2 -> Neuron1
        UEPtr<NLTZone> ltzone2 = GeneratorNeuron2->GetComponentL<NLTZone>("LTZone", true);
        if(!ltzone2)
            return true;

        synapses1.resize(num_exc_synapses_ngen1);
        for (int i = 1; i < num_exc_synapses_ngen1; i++)
        {
            if(dendrite_length_ngen1 == 0)
            {
                synapses1[i] = GeneratorNeuron1->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse"+sntoa(i+1),true);
            }
            else
            {
                synapses1[i] = GeneratorNeuron1->GetComponentL<NPulseSynapse>("Dendrite1_"+sntoa(dendrite_length_ngen1)+".ExcSynapse"+sntoa(i+1),true);
            }
            if(!synapses1[i])
                return true;


            res&=CreateLink(ltzone2->GetLongName(this),"Output",synapses1[i]->GetLongName(this),"Input");
            if(!res)
                return true;
        }

        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();


        //Создаем связь между вторым нейрном из генераторной связки и выходным нейроном
        //Neuron2 -> OutputNeuron
        UEPtr<NPulseSynapse> synapse4 = OutputNeuron->GetComponentL<NPulseSynapse>("Dendrite1_1.ExcSynapse1",true);
        if(!synapse4)
            return true;
        res&=CreateLink(ltzone2->GetLongName(this),"Output",synapse4->GetLongName(this),"Input");
        if(!res)
            return true;
        GeneratorNeuron2->Reset();
        OutputNeuron->Reset();


        //Создаем связь между обучаемым нейроном и генератором, продлевающим его тормозящее воздействие
        //NeuronTrainer -> PatternGenerator
        UEPtr<NPulseNeuron> neuron = NeuronTrainer->GetComponentL<NPulseNeuron>("Neuron",true);
        if(!neuron)
         return true;
        UEPtr<NLTZone> ltzoneTr = neuron->GetComponentL<NLTZone>("LTZone", true);
        if(!ltzoneTr)
         return true;
        res&=CreateLink(ltzoneTr->GetLongName(this),"Output", "PatternSource","Input");
        if(!res)
         return true;
        NeuronTrainer->Reset();
        PatternGenerator->Reset();

        //Создаем связь между генератором, продлевающим тормозящее воздействие обучаемого нейрона, и выходным нейроном
        //PatternGenerator -> OutputNeuron
        UEPtr<NPulseSynapse> synapse5 = OutputNeuron->GetComponentL<NPulseSynapse>("Soma1.InhSynapse1",true);
        if(!synapse5)
            return true;
        res&=CreateLink("PatternSource","Output", synapse5->GetLongName(this),"Input");
        if(!res)
         return true;
        PatternGenerator->Reset();
        OutputNeuron->Reset();
      }
     return true;
}


// Сброс процесса счета.
bool NLogicalNot::AReset(void)
{
     bool res(true);

  //Настройка Neuron1
  std::vector<int> dend_size1;
  dend_size1.resize(1);
  dend_size1[0] = DendriteLengthNGen1;
  GeneratorNeuron1->NumDendriteMembranePartsVec = dend_size1;

  //Удаляем лишние синапсы на соме
  UEPtr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
  if(!soma1)
   return true;
  soma1->NumExcitatorySynapses = 1;
  soma1->Reset();

  //Удаляем лишние синапсы на дендритах
  for (int i = 0; i< (DendriteLengthNGen1-1); i++)
  {
      UEPtr<NPulseMembrane> dendrite = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
      if(!dendrite)
       return true;
      dendrite->NumExcitatorySynapses = 1;
      dendrite->Reset();

  }
  //Устанавливаем необходимое число синапсов на соме или окончании дендрита
  if(DendriteLengthNGen1 == 0)
  {
      UEPtr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
      if(!soma1)
       return true;
      soma1->NumExcitatorySynapses = NumExcSynapsesNGen1;
      soma1->Build();
  }
  else
  {
      int dend_length = DendriteLengthNGen1;
      UEPtr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length),true);
      if(!dendrite1)
       return true;
      dendrite1->NumExcitatorySynapses = NumExcSynapsesNGen1;
      dendrite1->Build();
  }
  GeneratorNeuron1->Reset();


  //Настройка Neuron2
  std::vector<int> dend_size2;
  dend_size2.resize(1);
  dend_size2[0] = DendriteLengthNGen2;
  GeneratorNeuron2->NumDendriteMembranePartsVec = dend_size2;

  //Удаляем лишние синапсы на соме
  UEPtr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
  if(!soma2)
   return true;
  soma2->NumExcitatorySynapses = 1;
  soma2->Reset();

  //Удаляем лишние синапсы на дендритах
  for (int i = 0; i< (DendriteLengthNGen2-1); i++)
  {
      UEPtr<NPulseMembrane> dendrite = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
      if(!dendrite)
       return true;
      dendrite->NumExcitatorySynapses = 1;
      dendrite->Reset();

  }

  //Устанавливаем необходимое число синапсов на соме или окончании дендрита
  if(DendriteLengthNGen2 == 0)
  {
      UEPtr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
      if(!soma2)
       return true;
      soma2->NumExcitatorySynapses = NumExcSynapsesNGen2;
      soma2->Build();
  }
  else
  {
      int dend_length2 = DendriteLengthNGen2;
      UEPtr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length2),true);
      if(!dendrite2)
       return true;
      dendrite2->NumExcitatorySynapses = NumExcSynapsesNGen2;
      dendrite2->Build();
  }
  GeneratorNeuron2->Reset();


  //Создаем заново связь между нейронами из генераторной связки
  //Neuron1 -> Neuron2
  UEPtr<NLTZone> ltzone1 = GeneratorNeuron1->GetComponentL<NLTZone>("LTZone", true);
  if(!ltzone1)
      return true;

  synapses2.resize(NumExcSynapsesNGen2);
  for (int i = 0; i < NumExcSynapsesNGen2; i++)
  {
      if(DendriteLengthNGen2 == 0)
      {
          synapses2[i] = GeneratorNeuron2->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse"+sntoa(i+1),true);
      }
      else
      {
          int dend_length2 = DendriteLengthNGen2;
          synapses2[i] = GeneratorNeuron2->GetComponentL<NPulseSynapse>("Dendrite1_"+sntoa(dend_length2)+".ExcSynapse"+sntoa(i+1),true);
      }
      if(!synapses2[i])
          return true;

      res&=CreateLink(ltzone1->GetLongName(this),"Output",synapses2[i]->GetLongName(this),"Input");
      if(!res)
          return true;
  }
        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();

  //Создаем обратную связь между нейронами из генераторной связки
  //Neuron2 -> Neuron1
  UEPtr<NLTZone> ltzone2 = GeneratorNeuron2->GetComponentL<NLTZone>("LTZone", true);
  if(!ltzone2)
      return true;

  synapses1.resize(NumExcSynapsesNGen1);
  for (int i = 1; i < NumExcSynapsesNGen1; i++)
  {
      if(DendriteLengthNGen1 == 0)
      {
          synapses1[i] = GeneratorNeuron1->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse"+sntoa(i+1),true);
      }
      else
      {
          int dend_length1 = DendriteLengthNGen1;
          synapses1[i] = GeneratorNeuron1->GetComponentL<NPulseSynapse>("Dendrite1_"+sntoa(dend_length1)+".ExcSynapse"+sntoa(i+1),true);
      }
      if(!synapses1[i])
          return true;


      res&=CreateLink(ltzone2->GetLongName(this),"Output",synapses1[i]->GetLongName(this),"Input");
      if(!res)
          return true;
  }

  GeneratorNeuron1->Reset();
  GeneratorNeuron2->Reset();


  //Настройка NeuronTrainer
  NeuronTrainer->InputPattern = InputPattern;
  NeuronTrainer->NumInputDendrite = NumInputDendrite;
  NeuronTrainer->SpikesFrequency = SpikesFrequency;
  NeuronTrainer->Reset();

  //PatternGenerator
  double pulse_length = PatternGenerator->PulseLength;
  PatternGenerator->PatternDuration = (1/(NeuronTrainer->SpikesFrequency)) - pulse_length;
  PatternGenerator->PatternFrequency = PatternFrequency;
  PatternGenerator->Reset();

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NLogicalNot::ADefault(void)
{
 StructureBuildMode=1;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronTrainerClassName="NNeuronTrainer";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio";
 IsNeedToTrain = true;
 SpikesFrequency = 1.5;
 NumInputDendrite = 4;
 InputPattern.Resize(NumInputDendrite,1);
 DendriteLengthNGen1 = 0;
 DendriteLengthNGen2 = 5;
 NumExcSynapsesNGen1 = 2;
 NumExcSynapsesNGen2 = 2;
 PatternFrequency = 500;

 return true;
}


// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NLogicalNot::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, PulseGeneratorClassName,
                          NeuronClassName, NumInputDendrite,
                          SpikesFrequency,
                          NumExcSynapsesNGen1,NumExcSynapsesNGen2,
                          DendriteLengthNGen1, DendriteLengthNGen2,
                          InputPattern);
  if(!res)
   return false;
 }

 return true;
}

// Выполняет расчет этого объекта
bool NLogicalNot::ACalculate(void)
{
    IsNeedToTrain = NeuronTrainer->IsNeedToTrain;

	return true;
}
// --------------------------


}
#endif
