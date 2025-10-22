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


// �����, ��������� ������ �������� ��� ������������� ��������� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NLogicalNot::NLogicalNot(void)
: StructureBuildMode("StructureBuildMode",this,&NLogicalNot::SetStructureBuildMode),
  CalculateMode("CalculateMode",this,&NLogicalNot::SetCalculateMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NLogicalNot::SetPulseGeneratorClassName),
  LogicalNotClassName("LogicalNotClassName",this,&NLogicalNot::SetLogicalNotClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NLogicalNot::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NLogicalNot::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NLogicalNot::SetSynapseClassName),
  NumInputDendrite("NumInputDendrite",this,&NLogicalNot::SetNumInputDendrite),
  IsNeedToTrain("IsNeedToTrain",this,&NLogicalNot::SetNeedToTrain),
  SpikesFrequency("SpikesFrequency",this,&NLogicalNot::SetSpikesFrequency),
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NLogicalNot::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ����� ������� �������
bool NLogicalNot::SetCalculateMode(const int &value)
{
 return true;
}

/// ��� ������, ���������� ��������� ���������
bool NLogicalNot::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������ ����������� "��" (������ ����)
bool NLogicalNot::SetLogicalNotClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������� �������
bool NLogicalNot::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NLogicalNot::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NLogicalNot::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ����� ������� ���������
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

/// ������� ������������� ��������
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


/// ������� ����������� (��)
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


/// ������� ��� �����������
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

/// ������� ���������� � ������ ���������� ������� (UsePatternOutput) (��)
bool NLogicalNot::SetPatternFrequency(const double &value)
{
    Ready=false;
    if(PatternGenerator)
    {
        PatternGenerator->PatternFrequency = value;
    }
 return true;
}

/// ����� �������� �� ������ ������� � ������������ ������ (����� ���������)
bool NLogicalNot::SetDendriteLengthNGen1(const int &value)
{
    Ready=false;
    if (GeneratorNeuron1)
    {
        std::vector<int> dend_size;
        dend_size.resize(1);
        dend_size[0] = value;
        GeneratorNeuron1->NumDendriteMembranePartsVec = dend_size;

        //������� ������ ������� �� ����
        std::shared_ptr<NPulseMembrane> soma = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
        if(!soma)
         return true;
        soma->NumExcitatorySynapses = 1;
        soma->Reset();

        //������� ������ ������� �� ���������
        for (int i = 0; i< value; i++)
        {
            std::shared_ptr<NPulseMembrane> dendrite = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
            if(!dendrite)
             return true;
            dendrite->NumExcitatorySynapses = 1;
            dendrite->Reset();
        }

        GeneratorNeuron1->Reset();

    }
 return true;
}

/// ����� �������� �� ������ ������� � ������������ ������ (����� ���������)
bool NLogicalNot::SetDendriteLengthNGen2(const int &value)
{
   Ready=false;
   if (GeneratorNeuron2)
   {
       std::vector<int> dend_size;
       dend_size.resize(1);
       dend_size[0] = value;
       GeneratorNeuron2->NumDendriteMembranePartsVec = dend_size;

       //������� ������ ������� �� ����
       std::shared_ptr<NPulseMembrane> soma = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
       if(!soma)
        return true;
       soma->NumExcitatorySynapses = 1;
       soma->Reset();

       //������� ������ ������� �� ���������
       for (int i = 0; i< value; i++)
       {
           std::shared_ptr<NPulseMembrane> dendrite = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
           if(!dendrite)
            return true;
           dendrite->NumExcitatorySynapses = 1;
           dendrite->Reset();
       }

       GeneratorNeuron2->Reset();
   }
 return true;
}

/// ����� �������� �� �������� ������� ������� � ������������ ������
bool NLogicalNot::SetNumExcSynapsesNGen1(const int &value)
{
    Ready=false;
    if (GeneratorNeuron1)
    {
        //������������� ����������� ����� �������� �� ����/��������� ��������
        if(DendriteLengthNGen1 == 0)
        {
            std::shared_ptr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma1)
             return true;
            soma1->NumExcitatorySynapses = value;
            soma1->Build();
        }
        else
        {
            int dend_length1 = DendriteLengthNGen1;
            std::shared_ptr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length1),true);
            if(!dendrite1)
             return true;
            dendrite1->NumExcitatorySynapses = value;
            dendrite1->Build();
        }

        GeneratorNeuron1->Reset();
    }
 return true;
}

/// ����� �������� �� �������� ������� ������� � ������������ ������
bool NLogicalNot::SetNumExcSynapsesNGen2(const int &value)
{
    Ready=false;
    if (GeneratorNeuron2)
    {
        //������������� ����������� ����� �������� �� ����/��������� ��������
        if(DendriteLengthNGen2 == 0)
        {
            std::shared_ptr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma2)
             return true;
            soma2->NumExcitatorySynapses = value;
            soma2->Build();
        }
        else
        {
            int dend_length2 = DendriteLengthNGen2;
            std::shared_ptr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length2),true);
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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NLogicalNot::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NLogicalNot::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
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


        //������� ��������� ���������� �������� ��� ������������ ������ ��������
        Generator = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source"), PulseGeneratorClassName);
        Generator->SetCoord(MVector<double,3>(4.3, 2.67, 0));
        Generator->Frequency = 0.001;
        Generator->Delay = 0.1;
        Generator->Reset();


        //������� ������� ������������ ������
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
            std::shared_ptr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma1)
             return true;
            soma1->NumExcitatorySynapses = num_exc_synapses_ngen1;
            soma1->Build();
        }
        else
        {
            std::shared_ptr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dendrite_length_ngen1),true);
            if(!dendrite1)
             return true;
            dendrite1->NumExcitatorySynapses = num_exc_synapses_ngen1;
            dendrite1->Build();
        }
        GeneratorNeuron1->Reset();


        //������� ������� ������������ ������
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
            std::shared_ptr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
            if(!soma2)
             return true;
            soma2->NumExcitatorySynapses = num_exc_synapses_ngen2;
            soma2->Build();
        }
        else
        {
            std::shared_ptr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dendrite_length_ngen2),true);
            if(!dendrite2)
             return true;
            dendrite2->NumExcitatorySynapses = num_exc_synapses_ngen2;
            dendrite2->Build();
        }
         GeneratorNeuron2->Reset();


        //������� ��������� ������
        NeuronTrainer = AddMissingComponent<NNeuronTrainer>(std::string("NeuronTrainer"), NeuronTrainerClassName);
        NeuronTrainer->SetCoord(MVector<double,3>(4.3, 9.67, 0));
        NeuronTrainer->NumInputDendrite = num_input_dendrite;
        NeuronTrainer->SpikesFrequency = spikes_frequency;
        NeuronTrainer->InputPattern = input_pattern;
        NeuronTrainer->Reset();


        //������ ���������, ������������ ���������� ����������� NeuronTrainer � �����
        PatternGenerator = AddMissingComponent<NPulseGeneratorTransit>(std::string("PatternSource"), PulseGeneratorClassName);
        PatternGenerator->SetCoord(MVector<double,3>(11.67, 9.67, 0));
        PatternGenerator->UsePatternOutput = true;
        PatternGenerator->Frequency = 0;
        PatternGenerator->PatternFrequency = PatternFrequency;

        double pulse_length = PatternGenerator->PulseLength;
        PatternGenerator->PatternDuration = (1/(NeuronTrainer->SpikesFrequency)) - pulse_length;
        PatternGenerator->Reset();


        //������� �������� ������
        OutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
        OutputNeuron->SetCoord(MVector<double,3>(18.3, 7.0, 0));
        std::vector<int> dend_size3;
        dend_size3.resize(1);
        dend_size3[0] = 1;
        OutputNeuron->NumDendriteMembranePartsVec = dend_size3;
        OutputNeuron->Reset();


        //������� ����� ����� ����������� ���������� �������� � ������ �������� �� ������������ ������
        //Generator -> Neuron1
        std::shared_ptr<NPulseSynapse> synapse1=GeneratorNeuron1->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse1",true);
        if(!synapse1)
         return true;
        res&=CreateLink("Source","Output",synapse1->GetLongName(GetThisAsSharedContainer()),"Input");
        if(!res)
         return true;
        Generator->Reset();


        //������� ����� ����� ��������� �� ������������ ������
        //Neuron1 -> Neuron2
        std::shared_ptr<NLTZone> ltzone1 = GeneratorNeuron1->GetComponentL<NLTZone>("LTZone", true);
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

            res&=CreateLink(ltzone1->GetLongName(GetThisAsSharedContainer()),"Output",synapses2[i]->GetLongName(GetThisAsSharedContainer()),"Input");
            if(!res)
                return true;
        }
        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();


        //������� �������� ����� ����� ��������� �� ������������ ������
        //Neuron2 -> Neuron1
        std::shared_ptr<NLTZone> ltzone2 = GeneratorNeuron2->GetComponentL<NLTZone>("LTZone", true);
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


            res&=CreateLink(ltzone2->GetLongName(GetThisAsSharedContainer()),"Output",synapses1[i]->GetLongName(GetThisAsSharedContainer()),"Input");
            if(!res)
                return true;
        }

        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();


        //������� ����� ����� ������ ������� �� ������������ ������ � �������� ��������
        //Neuron2 -> OutputNeuron
        std::shared_ptr<NPulseSynapse> synapse4 = OutputNeuron->GetComponentL<NPulseSynapse>("Dendrite1_1.ExcSynapse1",true);
        if(!synapse4)
            return true;
        res&=CreateLink(ltzone2->GetLongName(GetThisAsSharedContainer()),"Output",synapse4->GetLongName(GetThisAsSharedContainer()),"Input");
        if(!res)
            return true;
        GeneratorNeuron2->Reset();
        OutputNeuron->Reset();


        //������� ����� ����� ��������� �������� � �����������, ������������ ��� ���������� �����������
        //NeuronTrainer -> PatternGenerator
        std::shared_ptr<NPulseNeuron> neuron = NeuronTrainer->GetComponentL<NPulseNeuron>("Neuron",true);
        if(!neuron)
         return true;
        std::shared_ptr<NLTZone> ltzoneTr = neuron->GetComponentL<NLTZone>("LTZone", true);
        if(!ltzoneTr)
         return true;
        res&=CreateLink(ltzoneTr->GetLongName(GetThisAsSharedContainer()),"Output", "PatternSource","Input");
        if(!res)
         return true;
        NeuronTrainer->Reset();
        PatternGenerator->Reset();

        //������� ����� ����� �����������, ������������ ���������� ����������� ���������� �������, � �������� ��������
        //PatternGenerator -> OutputNeuron
        std::shared_ptr<NPulseSynapse> synapse5 = OutputNeuron->GetComponentL<NPulseSynapse>("Soma1.InhSynapse1",true);
        if(!synapse5)
            return true;
        res&=CreateLink("PatternSource","Output", synapse5->GetLongName(GetThisAsSharedContainer()),"Input");
        if(!res)
         return true;
        PatternGenerator->Reset();
        OutputNeuron->Reset();
      }
     return true;
}


// ����� �������� �����.
bool NLogicalNot::AReset(void)
{
     bool res(true);

  //��������� Neuron1
  std::vector<int> dend_size1;
  dend_size1.resize(1);
  dend_size1[0] = DendriteLengthNGen1;
  GeneratorNeuron1->NumDendriteMembranePartsVec = dend_size1;

  //������� ������ ������� �� ����
  std::shared_ptr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
  if(!soma1)
   return true;
  soma1->NumExcitatorySynapses = 1;
  soma1->Reset();

  //������� ������ ������� �� ���������
  for (int i = 0; i< (DendriteLengthNGen1-1); i++)
  {
      std::shared_ptr<NPulseMembrane> dendrite = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
      if(!dendrite)
       return true;
      dendrite->NumExcitatorySynapses = 1;
      dendrite->Reset();

  }
  //������������� ����������� ����� �������� �� ���� ��� ��������� ��������
  if(DendriteLengthNGen1 == 0)
  {
      std::shared_ptr<NPulseMembrane> soma1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Soma1",true);
      if(!soma1)
       return true;
      soma1->NumExcitatorySynapses = NumExcSynapsesNGen1;
      soma1->Build();
  }
  else
  {
      int dend_length = DendriteLengthNGen1;
      std::shared_ptr<NPulseMembrane> dendrite1 = GeneratorNeuron1->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length),true);
      if(!dendrite1)
       return true;
      dendrite1->NumExcitatorySynapses = NumExcSynapsesNGen1;
      dendrite1->Build();
  }
  GeneratorNeuron1->Reset();


  //��������� Neuron2
  std::vector<int> dend_size2;
  dend_size2.resize(1);
  dend_size2[0] = DendriteLengthNGen2;
  GeneratorNeuron2->NumDendriteMembranePartsVec = dend_size2;

  //������� ������ ������� �� ����
  std::shared_ptr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
  if(!soma2)
   return true;
  soma2->NumExcitatorySynapses = 1;
  soma2->Reset();

  //������� ������ ������� �� ���������
  for (int i = 0; i< (DendriteLengthNGen2-1); i++)
  {
      std::shared_ptr<NPulseMembrane> dendrite = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(i+1),true);
      if(!dendrite)
       return true;
      dendrite->NumExcitatorySynapses = 1;
      dendrite->Reset();

  }

  //������������� ����������� ����� �������� �� ���� ��� ��������� ��������
  if(DendriteLengthNGen2 == 0)
  {
      std::shared_ptr<NPulseMembrane> soma2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Soma1",true);
      if(!soma2)
       return true;
      soma2->NumExcitatorySynapses = NumExcSynapsesNGen2;
      soma2->Build();
  }
  else
  {
      int dend_length2 = DendriteLengthNGen2;
      std::shared_ptr<NPulseMembrane> dendrite2 = GeneratorNeuron2->GetComponentL<NPulseMembrane>("Dendrite1_"+sntoa(dend_length2),true);
      if(!dendrite2)
       return true;
      dendrite2->NumExcitatorySynapses = NumExcSynapsesNGen2;
      dendrite2->Build();
  }
  GeneratorNeuron2->Reset();


  //������� ������ ����� ����� ��������� �� ������������ ������
  //Neuron1 -> Neuron2
  std::shared_ptr<NLTZone> ltzone1 = GeneratorNeuron1->GetComponentL<NLTZone>("LTZone", true);
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

      res&=CreateLink(ltzone1->GetLongName(GetThisAsSharedContainer()),"Output",synapses2[i]->GetLongName(GetThisAsSharedContainer()),"Input");
      if(!res)
          return true;
  }
        GeneratorNeuron1->Reset();
        GeneratorNeuron2->Reset();

  //������� �������� ����� ����� ��������� �� ������������ ������
  //Neuron2 -> Neuron1
  std::shared_ptr<NLTZone> ltzone2 = GeneratorNeuron2->GetComponentL<NLTZone>("LTZone", true);
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


      res&=CreateLink(ltzone2->GetLongName(GetThisAsSharedContainer()),"Output",synapses1[i]->GetLongName(GetThisAsSharedContainer()),"Input");
      if(!res)
          return true;
  }

  GeneratorNeuron1->Reset();
  GeneratorNeuron2->Reset();


  //��������� NeuronTrainer
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

// �������������� �������� �� ��������� � ����� �������� �����
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


// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

// ��������� ������ ����� �������
bool NLogicalNot::ACalculate(void)
{
    IsNeedToTrain = NeuronTrainer->IsNeedToTrain;

	return true;
}
// --------------------------


}
#endif
