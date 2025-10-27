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

// �����, ��������� ������ �� ����������, �������������� ��� ������������� ��������� �������� ���������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NNeuronTrainer::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ����� ������� �������
bool NNeuronTrainer::SetCalculateMode(const int &value)
{
 return true;
}

/// ��� ������, ���������� ��������� ���������
bool NNeuronTrainer::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NNeuronTrainer::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NNeuronTrainer::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ������� ������������� ��������
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


/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool NNeuronTrainer::SetDelay(const double &value)
{
 return true;
}

/// ������� ����������� (��)
bool NNeuronTrainer::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Frequency = value;
  generators[i]->Reset();
 }

 return true;
}

/// ����� ������� ���������
bool NNeuronTrainer::SetNumInputDendrite(const int &value)
{
 Ready=false;
 OldNumInputDendrite = NumInputDendrite;
 InputPattern.Resize(value,1);
 return true;
}

/// ������������ ����� ��������
bool NNeuronTrainer::SetMaxDendriteLength(const int &value)
{
 return true;
}

/// ������� ��� �����������
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

/// ����� �������������� ���� �������
bool NNeuronTrainer::SetLTZThreshold(const double &value)
{
 std::shared_ptr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 std::shared_ptr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
  UseFixedLTZThreshold = false;

 return true;
}

/// ����� �������������� ���� ������� ��� ����� ��������
bool NNeuronTrainer::SetTrainingLTZThreshold(const double &value)
{
// std::shared_ptr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
// if(!n_in)
//  return true;
//
// std::shared_ptr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
// if(!ltzone)
//  return true;
//
// ltzone->Threshold = value;

 return true;
}

/// ������������� ����� �������������� ���� �������
bool NNeuronTrainer::SetFixedLTZThreshold(const double &value)
{
 return true;
}

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
bool NNeuronTrainer::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  double threshold = FixedLTZThreshold;
  LTZThreshold = threshold;
 }

 return true;
}
/// ������������� ����������� ��������
bool NNeuronTrainer::SetSynapseResistanceStep(const double &value)
{
 return true;
}

// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NNeuronTrainer::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 /*
 if(!NPulseNeuronCommon::AAddComponent(comp,pointer))
  return false;

 comp->SetMainOwner(this,-1);

  std::shared_ptr<NConstGenerator> temp=dynamic_pointer_cast<NConstGenerator>(comp);

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

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NNeuronTrainer::ADelComponent(std::shared_ptr<UContainer> comp)
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
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NNeuronTrainer::BuildStructure(int structure_build_mode, const string &pulse_generator_class_name,
									const string &neuron_class_name,
									int old_num_input_dendrite, int num_input_dendrite,
									const MDMatrix<double> &input_pattern)
{
 // 1 - ������ ������, ������������� ����� ����� ��������� ��� ������������� ������� �� ��������� �� �������� ������� ���������
 if(StructureBuildMode == 1)
 {
	bool res(true);
    is_first_iter = true; // ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	max_iter_neuron_amp = 0; // ������������ ��������� ��������� ������� �� ����� ��������
	max_neuron_amp = 0; // ������������ ��������� ���������
	dend_counter = 1; // �������������� �� ����� �������
	is_new_dend = false; // ������� ������� ������ ��������
	is_new_iteration = false; // ������� ����� �������� (���������� ������� ��������)
	dend_index = -1; // ������ ��������������� �������
	syn_counter = 0; // �������������� �� ����� ������ = �����c+1 ��������, �� ������� ������ �������

	// ��������� ��� ������������� �� �������
	is_need_to_build = true; // ���� ������������� ��������� ������

	NeuronAmplitude.Assign(1+num_input_dendrite,1,0.0);
	SomaNeuronAmplitude.Assign(1+num_input_dendrite,1,0.0);

	// ������������� ������� ���� ���������
	DendriteLength.assign(num_input_dendrite,0);
	// ������ ������� ��������� ��� ��������� �� �����
	InitialDendritePotential.assign(num_input_dendrite,0);
	// ������ ���������� �������� �� ��������
	SynapseNum.assign(num_input_dendrite,1);

	// ��������� ������ ��������, �� ������� ������� ������ ������ ���������
	// � ������������� ����� ������� �������� ������ 1
	double max_delay = -1; // ������������ ����� �������� ��������, ������������ ������� ��������
	for(int i = 0; i < num_input_dendrite; i++)
	{
		if(input_pattern[i] > max_delay)
		{
			max_delay = input_pattern[i];
			dend_index = i;
		}
	}
	DendriteLength[dend_index] = 1;

	// ������� ������ ���������� � ������ ��������
	std::shared_ptr<NPulseNeuron> neuron1 = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
	for(int i = num_input_dendrite; i < old_num_input_dendrite; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));

		neuron1->DelComponent(std::string("Dendrite")+sntoa(i+1)+"_1")  ;
	}

	// �������������� ���������� ���������
    //OldNumInputDendrite = int(generators.size());
	generators.resize(num_input_dendrite);
	for(int i = 0; i < num_input_dendrite; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), pulse_generator_class_name);
		generators[i]->SetCoord(MVector<double,3>(6.7,1.67+i*2,0));
	}
	// ��������� �������� ����� �����������
	for(int i = 0; i < num_input_dendrite; i++)
	{
		generators[i]->DisconnectAll("Output");
	}

	// �������������� ������
    if(IsNeedToTrain)
	{
		// ������ ����� ������
		neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), neuron_class_name);
		neuron->SetCoord(MVector<double,3>(8.7+1*7,1.67,0));
		// ������������� ���������� ���������
		neuron->NumSomaMembraneParts = num_input_dendrite;
		// ������������� ����� ���������
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

	// �������� ����� ����� ����������� � ���������
	if(!neuron)
	 return true;
	std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_1.ExcSynapse1",true);
	if(!synapse)
	 return true;
res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
	if(!res)
	 return true;
	neuron->Reset();
 }

 return true;
}

// ����� �������� �����.
bool NNeuronTrainer::AReset(void)
{
 std::shared_ptr<NPulseNeuron> n_in=GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 std::shared_ptr<NLTZone> ltzone=n_in->GetComponentL<NLTZone>("LTZone");
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

// �������������� �������� �� ��������� � ����� �������� �����
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

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

// ����������� �������� ���� ����������� ����� ���������
// ���� ������������ ��������� �������
/*bool NNeuronTrainer::MaximazeAmplitude(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 1 - �������� ���� ����������� ����� ��������� ���� ������������ ��������� �������
	// ���� ������ ������ - ������������
	if(NumInputDendrite < dend_counter && CalculateMode < 2)
	{
		IsNeedToTrain = false;
		return true;
	}

	// ��������� ������������� ������� � ���������� ��������
	if(is_new_dend)
	{
		// ��������� ������ ��������, �� ������� ������� ������ ������ ���������,
		// �� ��� �� ������������
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
		std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_1.ExcSynapse1",true);
		if(!synapse)
		 return true;
		res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
		if(!res)
		 return true;
		neuron->Reset();

		is_new_dend = false;
		is_new_iteration = false;

		return true;
	}

	// ��������� ������������� ��������� ������� �������
	if(is_new_iteration)
	{
		// ���� ��������, ���������� �������
		if(DendriteLength[dend_index] < MaxDendriteLength)
			DendriteLength[dend_index]++;
		// ���� ���������� ��������� ������� - ��������� � ����������
		else
		{
			is_new_dend = true;
			dend_counter++;
			return true;
		}

		// ���� ����������, ������������� ����� ����� ���������
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

		// �������� ����� ����� ������� ����������� � ����� ��������
		if(!neuron)
		 return true;

		std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index])+".ExcSynapse1",true);
		if(!synapse)
		 return true;

		res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
		if(!res)
		 return true;

		// ������� ����� ����� ������� ����������� � ������ ��������
		synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index]-1)+".ExcSynapse1",true);
		if(!synapse)
		 return true;

		res&=BreakLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
		if(!res)
		 return true;

		neuron->Reset();

		is_new_iteration = false;

		return true;
	}

	// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();
		is_first_iter = false;
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//max_neuron_amp_time = 0;
		//InitialDendritePotential[dend_index] = 0;
	}

	// ��������� ������������ ��������� ��������� �������
	{
		if(!neuron)
		 return true;
		// ��������� �������
		double neuron_amp;
		neuron_amp = neuron->DendriticSumPotential(0,0);
		if(neuron_amp > max_iter_neuron_amp)
		{
			max_iter_neuron_amp = neuron_amp;
			// !!!!!!!!!!!!!!!!!!!!!!!!!
			//max_neuron_amp_time = Environment->GetTime().GetDoubleTime();
		}

		// ��� ���������� ������� �������� ���������� �������� ���������
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if(DendriteLength[dend_index] == 1)
		{
			std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(dend_index+1)+"_1",true);
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

	// ������� ��������� �������� (����� ��������)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
	if(iter_time >= iter_length)
	{
		// ���� �������� ��������� ������� ����������� - ������ ������� ������
		if(max_iter_neuron_amp > max_neuron_amp - 0.000005)
		{
			max_neuron_amp = max_iter_neuron_amp;
			NeuronAmplitude(0,0) = max_neuron_amp;
			is_new_iteration = true;
		}
		// ���� ��� - ������� ������� ������� � ��������� � ���������� ��������
		else
		{
			is_new_dend = true;
			dend_counter++;
			is_new_iteration = false;

			// ����������� ���� �� ���������� ������
			// �������� ����� ����� ������� ����������� � ������ ��������
			if(!neuron)
			 return true;

			std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index]-1)+".ExcSynapse1",true);
			if(!synapse)
			 return true;

			res&=CreateLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
			 return true;

			// ������� ����� ����� ������� ����������� � ����� ��������
			synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(dend_index+1)+"_"+sntoa(DendriteLength[dend_index])+".ExcSynapse1",true);
			if(!synapse)
			 return true;

			res&=BreakLink("Source"+sntoa(dend_index+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
			 return true;

			neuron->Reset();

			DendriteLength[dend_index]--;
		}

        // ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		is_first_iter = true;
		max_iter_neuron_amp = 0;

		return true;
	}
	else
		return true;

	////////////////////////////////////////////////////////////////////////////
	// 2 - �������� ���� ����������� ����� ��������� �� ��������� � ���������� �������� �� ��������
*/	/*if(NumInputDendrite < dend_counter && CalculateMode == 2)
	{
        // ���� ������ ������ - ������������
		if(NumInputDendrite < syn_counter)
		{
			IsNeedToTrain = false;
			return true;
		}

		// ��������� ������������� ������� � ���������� ��������
		if(is_new_dend)
		{
			// �������������� �� ����� ������ = �����c+1 ��������, �� ������� ������ �������
			syn_counter++;
			SynapseNum[syn_counter-1] = 1;

			is_new_dend = false;
			is_new_iteration = false;

			return true;
		}

		// ��������� ������������� �������� ������ �� �������
		if(is_new_iteration)
		{
			// ���� ��������, ���������� �������
			if(SynapseNum[syn_counter-1] < MaxDendriteLength)
				SynapseNum[syn_counter-1]++;
			// ���� ���������� ��������� ������� - ��������� � ����������
			else
			{
				is_new_dend = true;
				return true;
			}

			// ��������� ����� ������
			if(!neuron)
			 return true;
			dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(syn_counter)+"_"+sntoa(DendriteLength[syn_counter-1]),true);
			if(!dendrite)
			 return true;
			synapse=AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[syn_counter-1])), SynapseClassName);
			if(!synapse)
			 return true;

			// �������� ����� ����� ������� ����������� � ����� ��������
			res&=CreateLink("Source"+sntoa(syn_counter),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
			 return true;

			neuron->Reset();

			is_new_iteration = false;

			return true;
		}

		// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		if(is_first_iter)
		{
			start_iter_time = Environment->GetTime().GetDoubleTime();
			is_first_iter = false;
		}

		// ��������� ������������ ��������� ��������� �������
		{
			if(!neuron)
			 return true;
			dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(syn_counter)+"_1",true);
			if(!dendrite)
			 return true;

			// ��������� �������
			double neuron_amp;
			neuron_amp = dendrite->SumPotential(0,0);
			if(neuron_amp > max_iter_neuron_amp)
				max_iter_neuron_amp = neuron_amp;
		}

		// ������� ��������� �������� (����� ��������)
		double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
		double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
		if(iter_time >= iter_length)
		{
			// ���� �������� ��������� ������� ������ ��������� - ��������� ������� ������
			if(max_iter_neuron_amp < InitialDendritePotential[syn_counter-1] - 0.000005)
			{
				is_new_iteration = true;
			}
			// ���� ��� - ������� ������� ������� � ��������� � ���������� ��������
			else
			{
				is_new_dend = true;
				is_new_iteration = false;

				// ������� ������
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(syn_counter)+"_"+sntoa(DendriteLength[syn_counter-1])+".ExcSynapse"+sntoa(SynapseNum[syn_counter-1]),true);
				if(!synapse)
				 return true;
				DelComponent(synapse->GetLongName(GetThisAsSharedContainer()));
				synapse = NULL;

				neuron->Reset();

				SynapseNum[dend_index]--;
			}

			// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
			is_first_iter = true;
			max_iter_neuron_amp = 0;

			return true;
		}
		else
			return true;
	}*/

/*	return true;
}*/

// ��������� ������� ��� ����������� ������ ��������� ��� ����������� ���������
/*bool NNeuronTrainer::SynapseNormalization(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 4 - �������� ���� ����������� ����� ��������� �� ������� � ���������� �������� �� ��������
	////////////////////////////////////////////////////////////////////////////

	//  ��������� ����������� �������
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
		// �������, �������� ������ ������
		neuron->TrainingPattern = InputPattern;
		// ������� ������� �������� �� ���������
		neuron->TrainingDendIndexes.Resize(NumInputDendrite,1);
		for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingDendIndexes(i,0) = DendriteLength[i];
		}
		// ���������� �������� �� ������� �������� �� ���������
		neuron->TrainingSynapsisNum.Resize(NumInputDendrite,1);
        for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingSynapsisNum(i,0) = SynapseNum[i];
		}

		return true;
	}

	// ��������� ������������� �������� ������ �� �������
	if(is_new_iteration)
	{
		// ��� ���� ���������
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// ���� ��������� �������� ��� ������������� - ��������� � ����������
			if(!dend_status[i])
			 continue;
			// ��������� ������
			else if(dend_status[i] == 2)
			{
				// ��������� ������� �� ������������ ���������� ��������
				// �������� �� ����Ĩ�. ����� ��?
				//if(DendriteLength[i] >= MaxDendriteLength)
				//{
				//	dend_status[i] = 0;
				//	continue;
				//}

				// ��������� ������
				SynapseNum[i]++;
				// ��������� ������
				if(!neuron)
				 return true;
				std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
				if(!dendrite)
				 return true;
				dendrite->NumExcitatorySynapses=SynapseNum[i]++;
                dendrite->Build();
				std::shared_ptr<NPulseSynapse> synapse=dendrite->GetComponentL<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[i])), true);
				if(!synapse)
				 return true;

				synapse->Resistance = SynapseResistanceStep;
				// �������� ����� ����� ������� ����������� � ����� ��������
				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;
				// ��������� ����� ����� ����� �������� � ������ �������
				std::string input_name = dendrite->GetLongName(GetThisAsSharedContainer()) + std::string(".ExcChannel");
				res&=CreateLink(synapse->GetLongName(GetThisAsSharedContainer()),"Output",input_name,"SynapticInputs");
				if(!res)
				 return true;

				neuron->Reset();
			}
			// ������� ������
			else if(dend_status[i] == -2)
			{
				// ������� ������
				SynapseNum[i]--;
				// �������� ��� �����������
				dend_status[i] = 0;

				// ������� ������
				std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse"+sntoa(SynapseNum[i]+1),true);
				if(!synapse)
				 return true;
				DelComponent(synapse->GetLongName(GetThisAsSharedContainer()));
				synapse = NULL;

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// ������������ ��������� ��������� ������� ��������� �� ����� ��������
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// ��������� ������������ ��������� ��������� ������� ������� ��������
	if(!neuron)
	 return true;

	// ��������� ��������
	double dendrite_amp;
	// ���� ������������ ��������� ���������
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		if(!dendrite)
			return true;

		dendrite_amp = dendrite->SumPotential(0,0);
		if(dendrite_amp > max_iter_dend_amp[i])
			max_iter_dend_amp[i] = dendrite_amp;
	}

	// ������� ��������� ��������
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
	if(iter_time >= iter_length)
	{
		// ��� ���� ���������
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//double aaa = max_iter_dend_amp[i];
			//double aaaa = InitialDendritePotential[i];
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// ���� ��������� ������ ��������� - ��������� ������� ������
			if(max_iter_dend_amp[i] < InitialDendritePotential[i] + 0.000005)
			{
				dend_status[i] = 2;
			}
			// � ��������� ������ - ������� ������
			else
			{
				dend_status[i] = -2;
			}

			is_new_iteration = true;
		}

		// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}

// ����������� �������� ���� ����������� ����� ���������
// ���� ������������� �������� ��������
bool NNeuronTrainer::SynchronizePattern(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 3 - �������� ���� ����������� ����� ��������� ���� ������������� ������������ ������� ������� �������� �� ����
	// ���� ������ ������ - ������������

	// ��� ������ ����� ���������� ������
	if(is_need_to_build)
	{
		// ��������� ������ ��������, �� ������� ������� ������ ������ ���������
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

		// �������� ����� ����� ����������� � ���������
        /*if(!neuron)
		 return true;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_1.ExcSynapse1",true);
			if(!synapse)
			 return true;
			res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
			 return true;

			DendriteLength[i] = 1;
		}
		neuron->Reset();

		// ��������� ��� ������������� �� �������
		is_need_to_build = false; // ���� ������������� ��������� ������
		max_dend_amp_time.assign(NumInputDendrite, 0); // ����� ���������� ������������ ��������� ��������� ������� ��������� �� ����� ��������
		double pepiod = 1.0 / SpikesFrequency + 0.001; // ������ ��������
		dissynchronization.assign(NumInputDendrite, pepiod); // ������� ���������������� ��������
		dissynchronization[dend_index] = 0;
		dend_status.assign(NumInputDendrite, 1); // ������ ���������
		dend_status[dend_index] = 0;
		is_synchronizated = false; // ������� ���������� �������������

		return true;
	}

	//  ��������� ����������� �������
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
		dend_status.assign(NumInputDendrite, 2); // ������ ���������
		dend_status[dend_index] = 0;
		is_synchronizated = true; // ������� ���������� �������������
		if(CalculateMode == 3)
		 IsNeedToTrain = false;
		return true;
	}

	// ��������� ������������� ��������� ������� �������
	if(is_new_iteration)
	{
		// ��� ���� ���������
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// ���� ������� ��� ����� - ��������� � ����������
			if(!dend_status[i])
			 continue;
			// ���������� �������
			else if(dend_status[i] == 1)
			{
				// ��������� ������� �� ������������ ����� ���������
				if(DendriteLength[i] >= MaxDendriteLength)
				{
					dend_status[i] = 0;
					continue;
				}

				// ���������� �������
				DendriteLength[i]++;
				// ��������� �������
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

				// �������� ����� ����� ������� ����������� � ����� ��������
				std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;

				// ������� ����� ����� ������� ����������� � ������ ��������
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]-1)+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;

				neuron->Reset();
			}
			// ����������� �������
			else if(dend_status[i] == -1)
			{
                if(!neuron)
				 return true;

				// ����������� �������
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

				// �������� ��� �����������
				dend_status[i] = 0;

				// �������� ����� ����� ������� ����������� � ������ ��������
				std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;

				// ������� ����� ����� ������� ����������� � ����� ��������
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]+1)+".ExcSynapse1",true);
				if(!synapse)
				 return true;

				res&=BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// ������������ ��������� ��������� ������� ��������� �� ����� ��������
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// ��������� ������������ ��������� ��������� ������� ������� ��������
	if(!neuron)
	 return true;

	// ��������� ��������
	double dendrite_amp;
	// ���� ������������ ��������� ���������
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		if(!dendrite)
			return true;

		dendrite_amp = dendrite->SumPotential(0,0);
		if(dendrite_amp > max_iter_dend_amp[i])
		{
			max_iter_dend_amp[i] = dendrite_amp;
			max_dend_amp_time[i] = Environment->GetTime().GetDoubleTime();

			// ��� ���������� ������� �������� ���������� �������� ���������
			if(DendriteLength[i] == 1)
			 InitialDendritePotential[i] = dendrite_amp;
		}
	}

	// ������� ��������� �������� (����� ��������)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
	if(iter_time >= iter_length)
	{
		// ��� ���� ���������
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// ������� ����������
			double dt = fabs(max_dend_amp_time[dend_index] - max_dend_amp_time[i]);

			// ���������� ����������� - ������ ������� ������
			if(dt < dissynchronization[i])
			{
				dissynchronization[i] = dt;
				dend_status[i] = 1;
			}
			// � ��������� ������ - ����������� �������
			else
			{
				dend_status[i] = -1;
			}

			is_new_iteration = true;
		}

		// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}*/

// ��������� ������� ��� ����������� ������ ��������� ��� ����������� ���������
bool NNeuronTrainer::SomaSynapseNormalization(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// �������� ���� ����������� ����� ��������� �� ������� � ���������� �������� �� ��������
	//     � ����������� �� ���������� �� ������ � ����
	////////////////////////////////////////////////////////////////////////////

	//  ��������� ����������� �������
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
		// �������, �������� ������ ������
		neuron->TrainingPattern = InputPattern;
		// ������� ������� �������� �� ���������
		neuron->TrainingDendIndexes.Resize(NumInputDendrite,1);
		for(int i = 0; i < NumInputDendrite; i++)
		{
		 neuron->TrainingDendIndexes(i,0) = DendriteLength[i];
		}
		// ���������� �������� �� ������� �������� �� ���������
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

	// ��������� ������������� �������� ������ �� �������
	if(is_new_iteration)
	{
		// ��� ���� ���������
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// ���� ��������� �������� ��� ������������� - ��������� � ����������
			if(!dend_status[i])
			 continue;
			// ��������� ������
			else if(dend_status[i] == 2)
			{
				// ��������� ������� �� ������������ ���������� ��������
				// �������� �� ����Ĩ�. ����� ��?
				//if(DendriteLength[i] >= MaxDendriteLength)
				//{
				//	dend_status[i] = 0;
				//	continue;
				//}

				// ��������� ������
				SynapseNum[i]++;

				// ��������� ������
				if(!neuron)
				 return true;
				std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
				if(!dendrite)
				 return true;
				dendrite->NumExcitatorySynapses = dendrite->NumExcitatorySynapses + 1;
				dendrite->Build();
//				std::shared_ptr<NPulseSynapse> synapse=dendrite->AddMissingComponent<NPulseSynapse>(std::string("ExcSynapse"+sntoa(SynapseNum[i])), SynapseClassName);
//				if(!synapse)
//				 return true;

				// �������� ����� ����� ������� ����������� � ����� ��������
				NPulseSynapseCommon *synapse=dendrite->GetExcitatorySynapses(dendrite->NumExcitatorySynapses - 1);
				res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
				 return true;

				synapse->Resistance = SynapseResistanceStep;

				neuron->Reset();
			}
			// ������� ������
			else if(dend_status[i] == -2)
			{
				// ������� ������
				SynapseNum[i]--;
				// �������� ��� �����������
				dend_status[i] = 0;

				std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]),true);
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

	// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// ������������ ��������� ��������� ������� ��������� �� ����� ��������
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// ��������� ������������ ��������� ��������� ������� ������� ��������
	if(!neuron)
	 return true;

	// ��������� ��������
	double soma_amp;
	// ���� ������������ ��������� ���������
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		if(!soma)
			return true;

		soma_amp = soma->SumPotential(0,0);
		if(soma_amp > max_iter_dend_amp[i])
			max_iter_dend_amp[i] = soma_amp;
	}

	// ������� ��������� ��������
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
	if(iter_time >= iter_length)
	{
		// ��� ���� ���������
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
//			double aaa = max_iter_dend_amp[i];
//			double aaaa = InitialDendritePotential[i];
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// ���� ��������� ������ ��������� - ��������� ������� ������
			if(max_iter_dend_amp[i] < InitialDendritePotential[i] + 0.000005)
			{
				dend_status[i] = 2;
			}
			// � ��������� ������ - ������� ������
			else
			{
				dend_status[i] = -2;
			}

			is_new_iteration = true;
		}

		// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}

// ����������� �������� ���� ����������� ����� ���������
// ���� ������������� �������� �������� �� ������ ����
bool NNeuronTrainer::SomaSynchronizePattern(void)
{
	bool res(true);

	////////////////////////////////////////////////////////////////////////////
	// 6 - �������� ���� ����������� ����� ��������� �� ������� � ���������� �������� �� ��������
	//     � ����������� �� ���������� �� ������ ����
	////////////////////////////////////////////////////////////////////////////
	// ���� ������ ������ - ������������

	// ��� ������ ����� ���������� ������
	if(is_need_to_build)
	{
		// �������� ����� ����� ����������� � ���������
		if(!neuron)
		 return true;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_1.ExcSynapse1",true);
			if(!synapse)
			 return true;
			res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
			if(!res)
			 return true;

			DendriteLength[i] = 1;
		}
		neuron->Reset();

		// ��������� ��� ������������� �� �������
		is_need_to_build = false; // ���� ������������� ��������� ������
		max_dend_amp_time.assign(NumInputDendrite, 0); // ����� ���������� ������������ ��������� ��������� ������� � ��� �� ����� ��������
		double period = 1.0 / SpikesFrequency + 0.001; // ������ ��������
		dissynchronization.assign(NumInputDendrite, period); // ������� ���������������� ����
		dissynchronization[dend_index] = 0;
		dend_status.assign(NumInputDendrite, 1); // ������ ���������
		dend_status[dend_index] = 0;
		is_synchronizated = false; // ������� ���������� �������������

		return true;
	}

	//  ��������� ����������� �������
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
		dend_status.assign(NumInputDendrite, 2); // ������ ���������
		dend_status[dend_index] = 0;
		is_synchronizated = true; // ������� ���������� �������������
		if(CalculateMode == 3)
		 IsNeedToTrain = false;
		return true;
	}

	// ��������� ������������� ��������� ������� �������
	if(is_new_iteration)
	{
		// ��� ���� ���������
		for(int i = 0; i < NumInputDendrite; i++)
		{
			// ���� ������� ��� ����� - ��������� � ����������
			if(!dend_status[i])
			 continue;
			// ���������� �������
			else if(dend_status[i] == 1)
			{
				// ��������� ������� �� ������������ ����� ���������
				if(DendriteLength[i] >= MaxDendriteLength)
				{
					dend_status[i] = 0;
					continue;
				}

				// ���������� �������
				DendriteLength[i]++;
				// ��������� ������� ��������
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

				// �������� ����� ����� ������� ����������� � ����� ��������
				std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
                 LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Synapse: Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1"+" not found");
                else
                {
                res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
                 if(!res)
                  LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Falied create link: Source"+sntoa(i+1)+":Output -> "+synapse->GetLongName(GetThisAsSharedContainer())+":Input");
                }
				// ������� ����� ����� ������� ����������� � ������ ��������
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]-1)+".ExcSynapse1",true);
                if(synapse)
                 BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");

				neuron->Reset();
			}
			// ����������� �������
			else if(dend_status[i] == -1)
			{
                if(!neuron)
				 return true;

				// ����������� �������
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


				// �������� ��� �����������
				dend_status[i] = 0;

				// �������� ����� ����� ������� ����������� � ������ ��������
				std::shared_ptr<NPulseSynapse> synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1",true);
				if(!synapse)
                 LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Synapse: Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i])+".ExcSynapse1"+" not found");
                else
                {
                 res&=CreateLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
                 if(!res)
                  LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Falied create link: Source"+sntoa(i+1)+":Output -> "+synapse->GetLongName(GetThisAsSharedContainer())+":Input");
                }
				// ������� ����� ����� ������� ����������� � ����� ��������
				synapse=neuron->GetComponentL<NPulseSynapse>("Dendrite"+sntoa(i+1)+"_"+sntoa(DendriteLength[i]+1)+".ExcSynapse1",true);
                if(synapse)
                 BreakLink("Source"+sntoa(i+1),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");

				neuron->Reset();
			}
		}

		is_new_iteration = false;

		return true;
	}

	// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
	if(is_first_iter)
	{
		start_iter_time = Environment->GetTime().GetDoubleTime();

		// ������������ ��������� ��������� ������� ��� �� ����� ��������
		max_iter_dend_amp.assign(NumInputDendrite, 0);

		is_first_iter = false;
	}

	// ��������� ������������ ��������� ��������� ������� ������ ����
	if(!neuron)
	 return true;

	// ��������� ����
	double soma_amp;
	// ���� ������������ ��������� �� �����
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		if(!soma)
			return true;

		soma_amp = soma->SumPotential(0,0);
		if(soma_amp > max_iter_dend_amp[i])
		{
			max_iter_dend_amp[i] = soma_amp;
			max_dend_amp_time[i] = Environment->GetTime().GetDoubleTime();

			// ��� ���������� ������� �������� ���������� �������� ���������
			if(DendriteLength[i] == 1)
			 InitialDendritePotential[i] = soma_amp;
		}
	}

	// ������� ��������� �������� (����� ��������)
	double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
	double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
	if(iter_time >= iter_length)
	{
		// ��� ���� ���������
		//NeuronAmplitude(0,0) = 0;
		for(int i = 0; i < NumInputDendrite; i++)
		{
			//NeuronAmplitude(0,0) += max_iter_dend_amp[i];

			if(!dend_status[i])
			 continue;

			// ������� ����������
			double dt = fabs(max_dend_amp_time[dend_index] - max_dend_amp_time[i]);

			// ���������� ����������� - ������ ������� ������
			if(dt < dissynchronization[i])
			{
				dissynchronization[i] = dt;
				dend_status[i] = 1;
			}
			// � ��������� ������ - ����������� �������
			else
			{
				dend_status[i] = -1;
			}

			is_new_iteration = true;
		}

		// ���� ������ �������� (��������� ��� ������ ������� ������ �������)
		is_first_iter = true;

		return true;
	}
	else
		return true;

	return true;
}


// ������� �������
bool NNeuronTrainer::CalculateProcess(void)
{
	bool res(true);
	//static bool first_iter = 1;//������ ���� � ������������
	static MDMatrix<double> auto_puttern;
	static ofstream fout("C:\\Test_out2.txt"); // ����� ������ ��� ������
	static ifstream fin("C:\\Test.txt");// ��������� ���� �� ��� � ������?
	static double local_trainigLTZtresh = 0;

	//������ ����� ��� ����� ��������
 if (IsNeedToTrain && thresh_first_iter)
	{
		local_trainigLTZtresh = TrainingLTZThreshold;
		LTZThreshold = local_trainigLTZtresh;
		thresh_first_iter = 0;
	}

	// ����� N-��� ��������
	// ��� ������� �� �����, ����� ���� ������� ��� ������� �/��� �����������
	NeuronAmplitude(0,0) = 0;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> dendrite = neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i+1)+"_1",true);
		NeuronAmplitude(i+1,0) = dendrite->SumPotential(0,0);
		NeuronAmplitude(0,0) = NeuronAmplitude(0,0) + /*8.0**/dendrite->SumPotential(0,0);
	}

	SomaNeuronAmplitude(0,0) = 0;
	for(int i = 0; i < NumInputDendrite; i++)
	{
		std::shared_ptr<NPulseMembrane> soma = neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);
		SomaNeuronAmplitude(i+1,0) = soma->SumPotential(0,0);
		SomaNeuronAmplitude(0,0) = SomaNeuronAmplitude(0,0) + /*8.0**/soma->SumPotential(0,0);
	}

	// ��������� ������������� ��������
	if(CalculateMode==5)
	{
		static double time_spike = 0;
		if(!IsNeedToTrain)
		{
			std::shared_ptr<NPulseLTZone> ltzone=neuron->GetComponentL<NPulseLTZone>("LTZone");
			if(!neuron)
				return true;
			if(!ltzone)
				return true;
			double time_now=Environment->GetTime().GetDoubleTime();
			const double step_thresh=0.0001;
			const double diff_time = 3;     //������ 3? 2 �� ��������
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
		//  ����� ����� �������� ���� ���������� �������
			return true;
		}
	}


	// �������� ������ �������
	switch(CalculateMode)
	{
	 case 0:
		return true;
		break;
	 case 1: case 2:
        // ����������� �������� ���� ����������� ����� ���������
		// ���� ������������ ��������� �������
        //res = MaximazeAmplitude();
        //return res;
        //break;
	 case 3:
		// ����������� �������� ���� ����������� ����� ���������
		// ���� ������������� �������� ��������
        //res = SynchronizePattern();
        //return res;
        //break;
	 case 4:
	 {
		// ����������� �������� ���� ����������� ����� ���������
		// ���� ������������� �������� ��������

        /*if(!is_synchronizated)
		 res = SynchronizePattern();
		// ��������� ������� ��� ����������� ������ ��������� ��� ����������� ���������
		if(is_synchronizated)
		 res = SynapseNormalization();
        return res;*/
        //break;
	 }
	 case 5:
		//����������� �������������� ������ ������ ������������ �������
		//��� ������� ������������ ���������������� ��������.
		//� header ���������� �����. ���������� <iostream><fstream><cstring>
     {/*
		//������� � �����
        static std::streamoff pos_file = 0;
		auto_puttern.Resize(NumInputDendrite,1);
		//�������� �� �������� ������ � ������ ��������, ����� ������ ������� �� �����
		if (fin.is_open()&&thresh_first_iter&&fout.is_open())
		{
			//������� � ���������� �������
			fin.seekg(pos_file);
			for(int i=0;i<NumInputDendrite;++i)
			{
				fin >> auto_puttern(i,0);
			}
			//���������� �������, ����� ����� ������� � ���������� ��������
			pos_file = fin.tellg();
			thresh_first_iter=0;
		}
		InputPattern = auto_puttern;
		//��������� ����������� ������
		LTZThreshold = 0.003; //���� ����� ����� ����� �������� �� �� ����� �������!!!!!!!!!!
		if(!is_synchronizated)
		 res = SynchronizePattern();
		// ��������� ������� ��� ����������� ������ ��������� ��� ����������� ���������
		if(is_synchronizated)
		 res = SynapseNormalization();
        return res;*/
        //break;
	 }
	 case 6:
	 {
		// ����������� �������� ���� ����������� ����� ���������
		// ���� ������������� �������� �������� �� ������ ����

		if(!is_synchronizated)
		 res = SomaSynchronizePattern();
		// ��������� ������� ��� ����������� ������ ��������� �� ������ ���� ��� ����������� ���������
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

// ��������� ������ ����� �������
bool NNeuronTrainer::ACalculate(void)
{
	CalculateProcess();

 	// ����� �������
	if(neuron)
	{
	 std::shared_ptr<NLTZone> ltzone=neuron->GetComponentL<NLTZone>("LTZone");//GetLTZone();
	 if(!ltzone)
	  return true;

	 Output = ltzone->Output;
	}

	return true;
}

// ������������� ���������� � ��������� ������� �������
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
