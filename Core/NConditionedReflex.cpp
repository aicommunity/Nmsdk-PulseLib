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


// �����, ��������� ������ �������� ��� ������������� ��������� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NConditionedReflex::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������, ���������� ��������� ���������
bool NConditionedReflex::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������� �������
bool NConditionedReflex::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NConditionedReflex::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NConditionedReflex::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ������ "��������" �������
bool NConditionedReflex::SetBigNeuronSize(const int &value)
{
 Ready=false;

 return true;
}

/// ������� ������������� ��������
bool NConditionedReflex::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  Ready=false;
 }

 return true;
}

/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool NConditionedReflex::SetDelay(const double &value)
{
 return true;
}

/// ������� ����������� (��)
bool NConditionedReflex::SetSpikesFrequency(const double &value)
{
 return true;
}

/// ����� ������� ��������� ��� ������� ������������ ������������
bool NConditionedReflex::SetNumUnconditionalStimulDendrite(const int &value)
{
 Ready=false;

 UnconditionalStimulPattern.Resize(value,1);

 return true;
}

/// ����� ������� ��������� ��� ������� ��������� ������������
bool NConditionedReflex::SetNumConditionalStimulDendrite(const int &value)
{
 Ready=false;

 ConditionalStimulPattern.Resize(value,1);

 return true;
}

/// ������������ ����� ��������
bool NConditionedReflex::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// ������� ��� �����������  �������� ������������ ������������
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

/// ������� ��� �����������  �������� ��������� ������������
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

/// �������������� �����������
/// false - ���������� ������������
/// true - ���������� ������������
bool NConditionedReflex::SetNegInfluence(const bool &value)
{

 return true;
}

/// ����� �������������� ���� �������
bool NConditionedReflex::SetLTZThreshold(const double &value)
{

 return true;
}

/// ������������� ����� �������������� ���� �������
bool NConditionedReflex::SetFixedLTZThreshold(const double &value)
{

 return true;
}

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NConditionedReflex::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NConditionedReflex::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ����� �������� �����.
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

// �������������� �������� �� ��������� � ����� �������� �����
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

// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NConditionedReflex::BuildStructure(void)
{
 // 1 - ������ ������ �������� ��� ������������� ��������� ��������
 if(StructureBuildMode == 1)
 {
	// ������� ���������� �������� ��������� � ������������ �������������
	is_stimuls_trained = false;
	// ������� ���������� �������� � ������� ��������� �� ��������� � ������������ �������������
	is_interval_found = true;
	// ������� ���������� �������� ������� "�"
	is_and_neuron_trained = true;
	// ������� ���������� �������� �������� �������
	is_big_neuron_trained = true;


	// ������ ������ "�������� ������������"
	ConditionalStimul = AddMissingComponent<NNeuronTrainer>("ConditionalStimulus", NeuronTrainerClassName);
	ConditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,1*2,0));
	ConditionalStimul->NumInputDendrite = NumConditionalStimulDendrite;
    std::shared_ptr<NPulseLTZoneThreshold> ltZone = ConditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
	if(!ltZone)
		return true;
    //ltZone->TimeConstant = 0.0005;
	ConditionalStimul->Reset();

	// ������ ������ "���������� ������������"
	UnconditionalStimul = AddMissingComponent<NNeuronTrainer>("UnconditionalStimulus", NeuronTrainerClassName);
	UnconditionalStimul->SetCoord(MVector<double,3>(4.0+0*7,5*2,0));
	UnconditionalStimul->NumInputDendrite = NumUnconditionalStimulDendrite;
    ltZone = UnconditionalStimul->GetComponentL<NPulseLTZoneThreshold>("Neuron.LTZone",true);
	if(!ltZone)
		return true;
    //ltZone->TimeConstant = 0.0005;
	UnconditionalStimul->Reset();

	// ������ ������, ����������� ������� "�"
	LogicalAndNeuron = AddMissingComponent<NNeuronTrainer>("AndNeuron", NeuronTrainerClassName);
	LogicalAndNeuron->SetCoord(MVector<double,3>(4.0+0.5*7,3*2,0));
	LogicalAndNeuron->NumInputDendrite = 2;
	LogicalAndNeuron->Reset();
	LogicalAndNeuron->IsNeedToTrain = false;
	// ��������� ���������� � ����� ��������
	std::shared_ptr<NPulseGeneratorTransit> generator;
	generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source1",true);
	if(!generator)
		return true;
	generator->UseTransitSignal = true;
	generator = LogicalAndNeuron->GetComponentL<NPulseGeneratorTransit>("Source2",true);
	if(!generator)
		return true;
	generator->UseTransitSignal = true;

	// ������ "�������" ������
	BigNeuron = AddMissingComponent<NPulseNeuron>(std::string("ConditionalNeuron"), NeuronClassName);
	BigNeuron->SetCoord(MVector<double,3>(4.0+1.5*7,3*2,0));
	BigNeuron->NumSomaMembraneParts = BigNeuronSize;
    ltZone = BigNeuron->GetComponentL<NPulseLTZoneThreshold>("LTZone",true);
	if(!ltZone)
		return true;
	double threshold = FixedLTZThreshold;
	ltZone->Threshold = threshold;
    BigNeuron->Reset();

	// ������ ������, ����������� "����������� �������"
	LogicalOrNeuron = AddMissingComponent<NPulseNeuron>(std::string("UnconditionalNeuron"), NeuronClassName);
	LogicalOrNeuron->SetCoord(MVector<double,3>(3.5+2.5*7,5*2,0));
	// ��������� ������ ��� ���������� ������� "���"
	std::shared_ptr<NPulseMembrane> soma = LogicalOrNeuron->GetComponentL<NPulseMembrane>("Soma1",true);
	if(!soma)
		return true;
	std::shared_ptr<NPulseSynapse> synapse2 = soma->AddMissingComponent<NPulseSynapse>("ExcSynapse2", SynapseClassName);
	if(!synapse2)
		return true;
	std::shared_ptr<NPulseSynapse> synapse1 = soma->GetComponentL<NPulseSynapse>("ExcSynapse1",true);
	if(!synapse1)
		return true;
	RDK::MVector<double,3> coords = synapse1->GetCoord();
	synapse2->SetCoord(MVector<double,3>(coords(0)+7.0,coords(1),0));
	// ��������� ����� ����� ����� �������� � ������ �������
	std::string input_name = soma->GetLongName(GetThisAsSharedContainer()) + std::string(".ExcChannel");
	bool res(true);
	res&=CreateLink(synapse2->GetLongName(GetThisAsSharedContainer()),"Output",input_name,"SynapticInputs");
	if(!res)
		return true;
	LogicalOrNeuron->Reset();

	// ��������� "����������� ������������" � "����������� ���������"
	std::shared_ptr<NPulseNeuron> neuron;
	// ����������� ������������
	std::shared_ptr<NLTZone> ltzone = UnconditionalStimul->GetComponentL<NLTZone>("Neuron.LTZone", true);
	if(!ltzone)
		return true;
	// ��������� c ����������� ���������
	res = true;
	res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse1->GetLongName(GetThisAsSharedContainer()),"Input");
	if(!res)
		return true;
 }

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

// ��������� ������ ����� �������
bool NConditionedReflex::ACalculate(void)
{
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

	return true;
}
// --------------------------


}
#endif
