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


// �����, ��������� ������ �������� ��� ������������� ��������� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NPainReflexSimple::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������, ���������� ��������� ���������
bool NPainReflexSimple::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������� �������
bool NPainReflexSimple::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NPainReflexSimple::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NPainReflexSimple::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ������ "��������" �������
/*
bool NPainReflexSimple::SetBigNeuronSize(const int &value)
{
 Ready=false;

 return true;
}
*/

/// ������� ������������� ��������
bool NPainReflexSimple::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  Ready=false;
 }

 return true;
}

/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool NPainReflexSimple::SetDelay(const double &value)
{
 return true;
}

/// ������� ����������� (��)
bool NPainReflexSimple::SetSpikesFrequency(const double &value)
{
 return true;
}

/// ����� ������� ��������� ��� ������� ������������ ������������
/*
bool NPainReflexSimple::SetNumUnconditionalStimulDendrite(const int &value)
{
 Ready=false;

 UnconditionalStimulPattern.Resize(value,1);

 return true;
}
*/

/// ����� ������� ��������� ��� ������� ��������� ������������
bool NPainReflexSimple::SetNumConditionalStimulDendrite(const int &value)
{
 Ready=false;

 //ConditionalStimulPattern.Resize(value,1);
 //TODO: Fix it here??

 return true;
}

/// ������������ ����� ��������
bool NPainReflexSimple::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// ������� ��� �����������  �������� ������������ ������������
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

/// ������� ��� �����������  �������� ��������� ������������
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

/// �������������� �����������
/// false - ���������� ������������
/// true - ���������� ������������
bool NPainReflexSimple::SetNegInfluence(const bool &value)
{

 return true;
}

/// ����� �������������� ���� �������
bool NPainReflexSimple::SetLTZThreshold(const double &value)
{

 return true;
}

/// ������������� ����� �������������� ���� �������
bool NPainReflexSimple::SetFixedLTZThreshold(const double &value)
{

 return true;
}

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPainReflexSimple::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPainReflexSimple::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ����� �������� �����.
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

 //��� �� ��� ������ ���� ???
 ConditionalStimul->SetActivity(false);
 ConditionalStimul->Reset();

 start_iter_time = 0;
 is_first_iter=true;

 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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

// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NPainReflexSimple::BuildStructure(void)
{
 // 1 - ������ ������ �������� ��� ������������� ��������� ��������
 if(StructureBuildMode == 1)
 {
     // ������� ���������� �������� ��������� ������������
     is_conditional_stimulus_trained = false;

     // ������ ������ "�������� ������������"
     ConditionalStimul = AddMissingComponent<NNeuronTrainer>("ConditionalStimulus", NeuronTrainerClassName);
     ConditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,1*2,0));
     ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;
     //std::shared_ptr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
     //if(!ltZone)
     //    return true;
     //ltZone->TimeConstant = 0.0005;
     ConditionalStimul->Reset();

     //���������� �� ��������� �������� � ����������� �� ��� ����������
     for(int i=0; i<NumConditionalStimulDendrite.v;i++)
     {
        std::shared_ptr<NPulseGeneratorTransit> src = ConditionalStimul->GetComponentL<NPulseGeneratorTransit>("Source"+sntoa(i+1),true);
        if(!src)
            return true;
        src->UseTransitSignal=true;
     }

     //������� ���������� ��������� ��� �������������� �����
     NormalInputGen = AddMissingComponent<NPulseGeneratorTransit>(std::string("NormInputGen"), PulseGeneratorClassName);
     NormalInputGen->SetCoord(MVector<double,3>(4.0+0*7,2*2,0));
     NormalInputGen->UseTransitSignal=true;

     //������� ���������� ��������� ��� ��������������� �����
     AltInputGen = AddMissingComponent<NPulseGeneratorTransit>(std::string("AltInputGen"), PulseGeneratorClassName);
     AltInputGen->SetCoord(MVector<double,3>(4.0+0*7,3*2,0));
     AltInputGen->UseTransitSignal=true;

     //������� ���������� ���������, ����������� ������������� ����
     NormalBlocker = AddMissingComponent<NPulseGeneratorTransit>(std::string("NormBlockerGen"), PulseGeneratorClassName);
     NormalBlocker->SetCoord(MVector<double,3>(4.0+1*7,1*2,0));
     //����������� ���������� ������� (? ��������� �� ��� ?)
     NormalBlocker->UseTransitSignal=false;
     NormalBlocker->Frequency=SpikesFrequency;
     NormalBlocker->UsePatternOutput=true;
     NormalBlocker->PatternDuration=0.6656; //??? ����������� � NLogicalNot
     NormalBlocker->PatternFrequency=500; //??? ����������� � NLogicalNot

     //������� ���������� ���������, ����������� �������������� ����
     AltBlocker = AddMissingComponent<NPulseGeneratorTransit>(std::string("AltBlockerGen"), PulseGeneratorClassName);
     AltBlocker->SetCoord(MVector<double,3>(4.0+2*7,2*2,0));
     //����������� ���������� ������� (? ��������� �� ��� ?)
     AltBlocker->UseTransitSignal=false;
     AltBlocker->Frequency=SpikesFrequency;
     AltBlocker->UsePatternOutput=true;
     AltBlocker->PatternDuration=0.6656; //??? ����������� � NLogicalNot
     AltBlocker->PatternFrequency=500; //??? ����������� � NLogicalNot

     // ������ ������, �������������� ������������� �����
     std::vector<int> dend_size;
     dend_size.resize(1);
     dend_size[0] = 1;

     NormalOutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("NormOutputNeuron"), NeuronClassName);
     NormalOutputNeuron->SetCoord(MVector<double,3>(4.0+1*7,2*2,0));
     NormalOutputNeuron->NumDendriteMembranePartsVec = dend_size;
     NormalOutputNeuron->Reset();

     // ������ ������, �������������� �������������� �����
     AltOutputNeuron = AddMissingComponent<NPulseNeuron>(std::string("AltOutputNeuron"), NeuronClassName);
     AltOutputNeuron->SetCoord(MVector<double,3>(4.0+2*7,3*2,0));
     AltOutputNeuron->NumDendriteMembranePartsVec = dend_size;
     AltOutputNeuron->Reset();

     //������� ����� ��������� ������������ � ����������� ����
     std::shared_ptr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
     //��� �� ��� ������ ����
     if(!ltZone)
         return true;

     // ��������� ����� ����� �������� � ����������� �����������
     // **�������� ����� ����������, ��� ��� ������ ������������, ��� �� ����� �������� ����� ���� �������� (?) **
     // ����� � ������, �� �� ����� �����
     //std::string input_name = NormalBlocker->GetLongName(this);
     bool res(true);
     //res&=CreateLink(ltZone->GetLongName(this),"Output",input_name,"Input");
     //if(!res)
     //    return true;

     //��������� ����� ����� ����������� ��������� ������ ����������� � ���������� ��������
     // �������������� ������� "��"
     std::shared_ptr<NPulseMembrane> soma = NormalOutputNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!soma)
         return true;
     std::shared_ptr<NPulseSynapse> inh_synapse_pos = soma->GetComponentL<NPulseSynapse>("InhSynapse1",true);
     if(!inh_synapse_pos)
         return true;
    res&=CreateLink(NormalBlocker->GetLongName(GetThisAsSharedContainer()),"Output",inh_synapse_pos->GetLongName(GetThisAsSharedContainer()),"Input");
     if(!res)
         return true;

     //��������� ����� ����� ������������� �������� ����������� � ������������
     // �������� �������������� ������� "��"
     std::shared_ptr<NPulseMembrane> dend_1_1 = NormalOutputNeuron->GetComponentL<NPulseMembrane>("Dendrite1_1",true);
     if(!dend_1_1)
         return true;
     std::shared_ptr<NPulseSynapse> exc_synapse_pos = dend_1_1->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
     if(!exc_synapse_pos)
         return true;
    res&=CreateLink(NormalInputGen->GetLongName(GetThisAsSharedContainer()),"Output",exc_synapse_pos->GetLongName(GetThisAsSharedContainer()),"Input");
     if(!res)
         return true;

     //��������� ����� ����� LTZone �������������� ������� "��" � �����������
     // ����������� ��� ���������� �������������� ����� ���� ��� ���������
     std::shared_ptr<NPulseLTZoneThreshold> pos_not_ltZone = NormalOutputNeuron->GetComponentL<NPulseLTZoneThreshold>("LTZone",true);
     if(!pos_not_ltZone)
         return true;
    res&=CreateLink(pos_not_ltZone->GetLongName(GetThisAsSharedContainer()),"Output",AltBlocker->GetLongName(GetThisAsSharedContainer()),"Input");
     if(!res)
         return true;

     //��������� ����� ����� ������� ����������� ������������� ����� ���������� �
     // ���������� ������ �������������� ������� "��"
     std::shared_ptr<NPulseMembrane> neg_soma = AltOutputNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!soma)
         return true;
     std::shared_ptr<NPulseSynapse> inh_synapse_neg = neg_soma->GetComponentL<NPulseSynapse>("InhSynapse1",true);
     if(!inh_synapse_pos)
         return true;
    res&=CreateLink(AltBlocker->GetLongName(GetThisAsSharedContainer()),"Output",inh_synapse_neg->GetLongName(GetThisAsSharedContainer()),"Input");
     if(!res)
         return true;

     //��������� ����� ����� ������� ����������� �������������� ���������� �
     // ������������ ������ ��������������� ������������� ����� ������� "��"
     std::shared_ptr<NPulseMembrane> dend_1_1_neg = AltOutputNeuron->GetComponentL<NPulseMembrane>("Dendrite1_1",true);
     if(!dend_1_1_neg)
         return true;
     std::shared_ptr<NPulseSynapse> exc_synapse_neg = dend_1_1_neg->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
     if(!exc_synapse_pos)
         return true;
    res&=CreateLink(AltInputGen->GetLongName(GetThisAsSharedContainer()),"Output",exc_synapse_neg->GetLongName(GetThisAsSharedContainer()),"Input");
     if(!res)
         return true;
 }

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

// ��������� ������ ����� �������
bool NPainReflexSimple::ACalculate(void)
{
    if(is_first_iter)
    {
        is_first_iter=false;
        start_iter_time = Environment.lock()->GetTime().GetDoubleTime();
    }

    // I. �������� ���� � �������� �������� ��������� ��������
    if(!is_conditional_stimulus_trained)
    {
        if(!ConditionalStimul->IsNeedToTrain)
        {
            is_conditional_stimulus_trained = true;

            //������� ����� ��������� ������������ � ����������� ����
            std::shared_ptr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
            //��� �� ��� ������ ����
            if(!ltZone)
                return true;

            // ��������� ����� ����� �������� � ����������� �����������
            std::string input_name = NormalBlocker->GetLongName(GetThisAsSharedContainer());
            bool res(true);
            res&=CreateLink(ltZone->GetLongName(GetThisAsSharedContainer()),"Output",input_name,"Input");
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

            //���� ������ ������� ������
            if((*PainInput)(0,0)>0)
            {
                //�������� ���������� (��������) ������� ��������� ������������
                ConditionalStimul->SetActivity(true);
            }


            double iter_time = Environment.lock()->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
            double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������

            if(iter_time>=iter_length)
            {
                //TODO:  ���-�� ���������� � ������ ��������� ��������?

                //���������� ���� ���������� (��������) � ������� ��������� (��������) �������
                ConditionalStimul->SetActivity(false);
            }
        }
    }

    /*
	// I. �������� ��������� � ������������ �������������
	if(!is_stimuls_trained)
	{
		if(!UnconditionalStimul->IsNeedToTrain && !ConditionalStimul->IsNeedToTrain)
		{
			is_stimuls_trained = true;

            // ������� ���������� �������� � ������� ��������� �� ��������� � ������������ �������������
			is_interval_found = false;
			is_first_spike = false;
			is_second_spike = false;

			return true;
		}
	}

	 // II. ���� �������� ����� �������� ����������
	 if(!is_interval_found)
	 {
		// ������� ������� ������� ������ �� ��������� ������������
		if(!is_first_spike)
		{
			std::shared_ptr<NLTZone> ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			double value = ltzone->Output(0,0);
			if(value > 0)
			{
				is_first_spike = true;
				first_spike_time = Environment.lock()->GetTime().GetDoubleTime();
				if(is_second_spike)
					ready_time = first_spike_time;
			}
		}

		// ������� ������� ������� ������ �� ������������ ������������
		if(!is_second_spike)
		{
			std::shared_ptr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			double value = ltzone->Output(0,0);
			if(value > 0)
			{
				is_second_spike = true;
				second_spike_time = Environment.lock()->GetTime().GetDoubleTime();
				if(is_first_spike)
					ready_time = second_spike_time;
			}
		}

		// ���� ��� ������:
		if(is_first_spike && is_second_spike)
		{
			double curr_time = Environment.lock()->GetTime().GetDoubleTime();
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

			// ����� ��������� �������� ��������
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

			// ������ ����� ��������� � ������������ ������������� � ��������, ����������� ������� "�"
			std::shared_ptr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// ������ "�"
			std::shared_ptr<NPulseGeneratorTransit> generator; // ��������������� ���� ������� � ������������ ��������
			generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source2",true);
			if(!generator)
				return true;
			// ���������
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",generator->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
				return true;
			// �������� ������������
			ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// ������ "�"
			generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source1",true);
			if(!generator)
				return true;
			// ���������
			res = true;
			res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",generator->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
				return true;

			// ���������� �������� ������� "�"
			LogicalAndNeuron->IsNeedToTrain = true;
            // ������� ���������� �������� ������� "�"
			is_and_neuron_trained = false;
            // ������� ���������� �������� � ������� ��������� �� ��������� � ������������ �������������
			is_interval_found = true;
		}

		return true;
	 }

	 // III. �������� �������, ������������ ������� "�"
	 if(!is_and_neuron_trained)
	 {
		if(!LogicalAndNeuron->IsNeedToTrain)
		{
			// ��������� ����� ����� ��������, ����������� ������� "�" � "�������" ��������
			// ������ "�"
			std::shared_ptr<NLTZone> ltzone = LogicalAndNeuron->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// "�������" ������
			std::shared_ptr<NPulseSynapse> synapse = BigNeuron->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse1",true);
			if(!synapse)
				return true;
			// ���������
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
				return true;

			// ������� ���������� �������� �������� �������
			is_big_neuron_trained = false;
			// ������� ������ ������ ��� ����������� �������� �� "�������" �������
			one_spike = false;
            // ������� ����� �������� �� "�������" �������
			synapses_num = 1;
			// ������� ���������� �������� ������� "�"
			is_and_neuron_trained = true;
		}

		return true;
	 }

	 // IV. �������� "��������" �������
	 if(!is_big_neuron_trained)
	 {
		// ���� "������� ������ �������, ��������� �������� � ��������� ����� ��������� ������������ � ����������� ���������
		std::shared_ptr<NLTZone> ltzone = BigNeuron->GetComponentL<NLTZone>("LTZone",true);
        if(!ltzone)
			return true;
		if(ltzone->Output(0,0) > 0)
		{
			// ��������� ����� ��������� ������������ � ����������� ���������
			// �������� ������������
			ltzone = ConditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone",true);
			if(!ltzone)
				return true;
			// ����������� �������
			std::shared_ptr<NPulseSynapse> synapse2;
			if(!IsNegInfluence)
				synapse2 = LogicalOrNeuron->GetComponentL<NPulseSynapse>("Soma1.ExcSynapse2",true);
			else
                synapse2 = LogicalOrNeuron->GetComponentL<NPulseSynapse>("Soma1.InhSynapse1",true);
			if(!synapse2)
				return true;
            // ���������
			bool res(true);
			res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse2->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
				return true;

			// ������� ���������� �������� �������� �������
			is_big_neuron_trained = true;

			return true;
		}

		// ���� ���� ���� � �������, ������������ ������� "�" - ��������� ������
		ltzone = LogicalAndNeuron->GetComponentL<NLTZone>("Neuron.LTZone",true);
        if(!ltzone)
			return true;

		if(ltzone->Output(0,0) > 0 && !one_spike)
		{
			one_spike = true;
			synapses_num++;

			// ��������� ������ �� "�������" ������
			std::shared_ptr<NPulseMembrane> soma = BigNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
			if(!soma)
				return true;
			std::shared_ptr<NPulseSynapse> synapse = soma->AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(synapses_num)), SynapseClassName);
			if(!synapse)
				return true;
			std::shared_ptr<NPulseSynapse> synapse1 = soma->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
			if(!synapse1)
				return true;
			RDK::MVector<double,3> coords = synapse1->GetCoord();
			synapse->SetCoord(MVector<double,3>(coords(0)+7.0*(synapses_num-1),coords(1),0));
			// ��������� ����� ����� ����� �������� � ������ �������
			std::string input_name = soma->GetLongName(GetThisAsSharedContainer()) + std::string(".ExcChannel");
			bool res(true);
			res&=CreateLink(synapse->GetLongName(GetThisAsSharedContainer()),"Output",input_name,"SynapticInputs");
			if(!res)
				return true;
			// ��������� ����� ����� ����� �������� � ������� "�" �������
			res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
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
