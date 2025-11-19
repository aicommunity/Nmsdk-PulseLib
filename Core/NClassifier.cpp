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
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
//#include <QString>
//#include <QDateTime>
//#include <QFile>

// �����, ��������� ������ ��������� �������� ��� ������������� ��������� �������� ���������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
  DataFromFile("DataFromFile",this,&NClassifier::SetDataFromFile)
{
 OldNumClasses=0;
 OldNumInputDendrite=0;
 OldSizeTrainingSet=0;
 generators.clear();
 groups_trainers.clear();

 // ���������� ��� ������ � ������ ������ � ����
 // ------->
 // ������� ������ ��������
 IsFirstFileStep = true;
 // <-------
}

NClassifier::~NClassifier(void)
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
bool NClassifier::SetStructureBuildMode(const int &value)
{
	if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
		Ready=false;
	return true;
}

/// ����� ������� �������
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

/// ��� ������, ���������� ��������� ���������
bool NClassifier::SetPulseGeneratorClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// ��� ������, ���������� ������� �������
bool NClassifier::SetNeuronTrainerClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// ��� ������, ���������� ������
bool NClassifier::SetNeuronClassName(const std::string &value)
{
	Ready=false;
	return true;
}

/// ��� ������, ���������� ������
bool NClassifier::SetSynapseClassName(const std::string &value)
{
	Ready=false;
	return true;
}


/// ������� ������������� ��������
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
	 std::shared_ptr<NNeuronTrainer> trainer;
	 std::shared_ptr<NPulseGeneratorTransit> generator;

	 for(int i = 0; i < NumClasses; i++)
	 {
		for(int j = 0; j < SizeTrainingSet; j++)
		{
			// CRITICAL: GetComponentL now returns weak_ptr, need to lock
			std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
			if(!trainer_weak.expired())
			 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
			else
			 trainer = nullptr;
			if(!trainer)
				return true;
			trainer->IsNeedToTrain = value;
			for(int k = 0; k < NumInputDendrite; k++)
			{
				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> generator_weak = trainer->GetComponentL("Source"+sntoa(k+1),true);
				if(!generator_weak.expired())
				 generator = std::dynamic_pointer_cast<NPulseGeneratorTransit>(generator_weak.lock());
				else
				 generator = nullptr;
				if(!generator)
					return true;

				generator->UseTransitSignal = (value)? false : true;
			}
		}
	 }
	}
	return true;
}

/// ����� �������� ������ �������� ������������ ������ ������� (���)
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

/// ������� ����������� (��)
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

/// ����� ������� ���������
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

/// ������������ ����� ��������
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

/// ������� ��� �����������
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

/// ������� ��� �������������
bool NClassifier::SetInputPattern(const MDMatrix<double> &value)
{
    for(int i = 0; i < int(generators.size()); i++)
	{
		generators[i]->Delay = Delay + value[i];
		generators[i]->Reset();
	}

	return true;
}

/// ����� �������������� ���� �������
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

/// ����� �������������� ���� ������� ��� ����� ��������
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

/// ������������� ����� �������������� ���� �������
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

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
bool NClassifier::SetUseFixedLTZThreshold(const bool &value)
{
	if(value)
	{
		double threshold = FixedLTZThreshold;
		LTZThreshold = threshold;
	}

	return true;
}

// ������������� ������������� �������� ������� �� �������� ���������
bool NClassifier::SetUseTransitSignal(const bool &value)
{
	for(size_t i = 0; i < generators.size(); i++)
	{
		generators[i]->UseTransitSignal = value;
	}

	return true;
}

/// ����� �������
bool NClassifier::SetNumClasses(const int &value)
{
	Ready=false;
	OldNumClasses = NumClasses;
	TrainingPatterns.Resize(SizeTrainingSet*value,NumInputDendrite);
	return true;
}

/// ����� ������� ������ ��������� �������
bool NClassifier::SetSizeTrainingSet(const int &value)
{
	Ready=false;
	OldSizeTrainingSet = SizeTrainingSet;
	TrainingPatterns.Resize(NumClasses*value,NumInputDendrite);
	return true;
}

// ������������� ������������� �������� ������� �� �������� ���������
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
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NClassifier::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
	return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NClassifier::ADelComponent(std::shared_ptr<UContainer> comp)
{
	return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ����� �������� �����.
bool NClassifier::AReset(void)
{
	for(size_t i = 0; i < groups_trainers.size(); i++)
	{
		for(size_t j = 0; j < groups_trainers[i].size(); j++)
		{
 //			groups_trainers[i][j]->IsNeedToTrain = IsNeedToTrain;

 /*			std::shared_ptr<NPulseGeneratorTransit> generator;
			for(int k = 0; k < NumInputDendrite; k++)
			{
				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> generator_weak = groups_trainers[i][j]->GetComponentL("Source"+sntoa(k+1),true);
				if(!generator_weak.expired())
				 generator = std::dynamic_pointer_cast<NPulseGeneratorTransit>(generator_weak.lock());
				else
				 generator = nullptr;
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

	return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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

	return true;
}

// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NClassifier::BuildStructure(void)
{
	if(StructureBuildMode == 2)
	{
		// ������� ������ �������
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

		// �������������� ������� � ������������ ��������
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
				 // CRITICAL: GetComponentL now returns weak_ptr, need to lock
				 std::weak_ptr<RDK::UContainer> generator_weak = groups_trainers[i][j]->GetComponentL("Source"+sntoa(k+1),true);
				 std::shared_ptr<NPulseGeneratorTransit> generator;
				 if(!generator_weak.expired())
				  generator = std::dynamic_pointer_cast<NPulseGeneratorTransit>(generator_weak.lock());
				 else
				  generator = nullptr;
				 if(!generator)
				  continue;

				 generator->UseTransitSignal = (IsNeedToTrain.v)? false : true;
				}
    groups_trainers[i][j]->Reset();
			}
		}

		// ������� ������ ����������
		for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
		{
			DelComponent(std::string("Source")+sntoa(i+1));
		}

		// �������������� ���������� ���������
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

		// ��������� �������� ����� �����������
		for(int i = 0; i < NumInputDendrite; i++)
		{
			generators[i]->DisconnectAll("Output");
		}

		// �������� ����� ����� ������������ � ��������� � ������������ ��������
		for(int k = 0; k < NumInputDendrite; k++)
		{
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
					std::shared_ptr<NPulseGeneratorTransit> gen_in; // ��������������� ���� ������� � ������������ ��������

					// CRITICAL: GetComponentL now returns weak_ptr, need to lock
					std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
					if(!trainer_weak.expired())
					 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
					else
					 trainer = nullptr;
					if(!trainer)
						return true;
					// CRITICAL: GetComponentL now returns weak_ptr, need to lock
					std::weak_ptr<RDK::UContainer> gen_in_weak = trainer->GetComponentL("Source"+sntoa(k+1),true);
					if(!gen_in_weak.expired())
					 gen_in = std::dynamic_pointer_cast<NPulseGeneratorTransit>(gen_in_weak.lock());
					else
					 gen_in = nullptr;
					if(!gen_in)
						return true;

					bool res(true);
					res&=CreateLink("Source"+sntoa(k+1),"Output",gen_in->GetLongName(GetThisAsSharedContainer()),"Input");
					if(!res)
						return true;
				}
			}
		}


		// ������ ������� ���
		for(int i = 0; i < NumClasses; i++)
		{
			LogicalOrNeuron = AddMissingComponent<NPulseNeuron>(std::string("OrNeuron"+sntoa(i+1)), NeuronClassName);
			LogicalOrNeuron->SetCoord(MVector<double,3>(8.7+1*7+i*20+10,(1.67+SizeTrainingSet)/2,0));

			// CRITICAL: GetComponentL now returns weak_ptr, need to lock


			std::weak_ptr<RDK::UContainer> soma_weak = LogicalOrNeuron->GetComponentL("Soma1",true);


			std::shared_ptr<NPulseMembrane> soma;


			if(!soma_weak.expired())


			 soma = std::dynamic_pointer_cast<NPulseMembrane>(soma_weak.lock());


			else


			 soma = nullptr;
			if(!soma)
				return true;

			// ��������� ������������ ������� ��� ���������� ������� "���"
			soma->NumExcitatorySynapses=*SizeTrainingSet;//soma->NumExcitatorySynapses+1;
			soma->Build();

			//��������� ������� � ��������������� ��������� ������� ���
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
				std::shared_ptr<NNeuronTrainer> trainer;
				if(!trainer_weak.expired())
				 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
				else
				 trainer = nullptr;
				if(!trainer)
					return true;

				// CRITICAL: GetComponentL now returns weak_ptr, need to lock


				std::weak_ptr<RDK::UContainer> ltzone_weak = trainer->GetComponentL("Neuron.LTZone", true);


				std::shared_ptr<NLTZone> ltzone;


				if(!ltzone_weak.expired())


				 ltzone = std::dynamic_pointer_cast<NLTZone>(ltzone_weak.lock());


				else


				 ltzone = nullptr;
				if(!ltzone)
					return true;

				NPulseSynapseCommon *synapse=soma->GetExcitatorySynapses(j);

				bool res(true);
				res &= CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");  //"ExcSynapse"+sntoa(j+1)
				if(!res)
					return true;
			}

			// ��������� ��������� ������� ��� ���������� ������� "���"
			soma->NumInhibitorySynapses = NumClasses - 1;
			soma->Build();

			LogicalOrNeuron->Reset();
		}

		// �������� ����� ����� ��������� ���
		for(int i = 0; i < NumClasses; i++)
		{
			// CRITICAL: GetComponentL now returns weak_ptr, need to lock
			std::weak_ptr<RDK::UContainer> LogicalOrNeuron_weak = GetComponentL("OrNeuron"+sntoa(i+1),true);
			std::shared_ptr<NPulseNeuron> LogicalOrNeuron;
			if(!LogicalOrNeuron_weak.expired())
			 LogicalOrNeuron = std::dynamic_pointer_cast<NPulseNeuron>(LogicalOrNeuron_weak.lock());
			else
			 LogicalOrNeuron = nullptr;
			if(!LogicalOrNeuron)
				return true;

			// CRITICAL: GetComponentL now returns weak_ptr, need to lock
			std::weak_ptr<RDK::UContainer> soma_weak = LogicalOrNeuron->GetComponentL("Soma1",true);
			std::shared_ptr<NPulseMembrane> soma;
			if(!soma_weak.expired())
			 soma = std::dynamic_pointer_cast<NPulseMembrane>(soma_weak.lock());
			else
			 soma = nullptr;
			if(!soma)
				return true;

			int indexSynapse = 0;

			for(int j = 0; j < NumClasses; j++)
			{
				if (i == j)
					continue;

				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> LogicalOrNeuron_weak_2 = GetComponentL("OrNeuron" + sntoa(j + 1), true);
				if(!LogicalOrNeuron_weak_2.expired())
				 LogicalOrNeuron = std::dynamic_pointer_cast<NPulseNeuron>(LogicalOrNeuron_weak_2.lock());
				else
				 LogicalOrNeuron = nullptr;
				if(!LogicalOrNeuron)
					return true;

				// CRITICAL: GetComponentL now returns weak_ptr, need to lock


				std::weak_ptr<RDK::UContainer> ltzone_weak = LogicalOrNeuron->GetComponentL("LTZone");


				std::shared_ptr<NLTZone> ltzone;


				if(!ltzone_weak.expired())


				 ltzone = std::dynamic_pointer_cast<NLTZone>(ltzone_weak.lock());


				else


				 ltzone = nullptr;
				 if(!ltzone)
					return true;

				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> synapse_weak = soma->GetComponentL("InhSynapse" + sntoa(indexSynapse + 1), true);
				std::shared_ptr<NPulseSynapse> synapse;
				if(!synapse_weak.expired())
				 synapse = std::dynamic_pointer_cast<NPulseSynapse>(synapse_weak.lock());
				else
				 synapse = nullptr;
				if(!synapse)
					return true;

				bool res(true);
				res &= CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
				if(!res)
					return true;

				indexSynapse++;
			}
		}

		OldNumClasses = int(groups_trainers.size());
		OldSizeTrainingSet = int(SizeTrainingSet);
	}

	return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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


// ������� ��� ������ � �������.
// ������������ ������ ������� ������ �� �����,
// ��������� ����������� � ������ ����������� � ����
bool NClassifier::TreatDataFromFile(void)
{
    // ������� ������� ����� � �������
    if(IsFirstFileStep)
    {
        // ��������� ���� ��� ������ ������
        fin.open(Environment->GetCurrentDataDir()+"input_data.txt");
        // ��������� ���� ��� ������ ������
        fout.open(Environment->GetCurrentDataDir()+"output_data.txt");
        // ���� ������ ��������
        is_first_iter = true;
        // ������� ������
        inputs.Assign(NumInputDendrite, 1, 0.0);

        IsFirstFileStep = false;
    }

    // ������� ������ ����� ��������
    if(is_first_iter)
    {
        // ����� ������ ������� �������� (���)
        start_iter_time = Environment->GetTime().GetDoubleTime();
        // ������ ��������
        outputs.assign(NumClasses,0);
        // ��������� ������� ������
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
            double in;
            fin >> in;
            /*if(fin.eof())
            {
                fin.close();
                fout.close();
                DataFromFile = false;
                return true;
            } */
            inputs(i,0) = in;
        }
        InputPattern = inputs;

        is_first_iter = false;
    }

    // ��������� ������ ��������
    //std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
    std::shared_ptr<NPulseNeuron> neuron;
    std::shared_ptr<NLTZone> ltzone;
    for(int i= 0; i < NumClasses; i++)
    {
        //trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
        //if(!trainer)
        //	return true;

        // CRITICAL: GetComponentL now returns weak_ptr, need to lock
        std::weak_ptr<RDK::UContainer> neuron_weak = GetComponentL("OrNeuron"+sntoa(i+1),true);
        if(!neuron_weak.expired())
         neuron = std::dynamic_pointer_cast<NPulseNeuron>(neuron_weak.lock());
        else
         neuron = nullptr;
        if(!neuron)
            return true;

        // CRITICAL: GetComponentL now returns weak_ptr, need to lock


        std::weak_ptr<RDK::UContainer> ltzone_weak_assign = neuron->GetComponentL("LTZone");


        if(!ltzone_weak_assign.expired())


         ltzone = std::dynamic_pointer_cast<NLTZone>(ltzone_weak_assign.lock());


        else


         ltzone = nullptr;
        if(!ltzone)
            return true;

        if(ltzone->Output(0,0) > 0.00001)
            outputs[i] = 1;
    }

    // ��������� ��������� ��������
    double iter_time = Environment->GetTime().GetDoubleTime() - start_iter_time; // ������� ����� ��������
    double iter_length = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep)); // ����� ����� ��������
    if(iter_time >= iter_length)
    {
        // ���������� ���������� � ����
        for(int i = 0; i < NumClasses; i++)
        {
            fout << outputs[i] << "\t";
        }
        fout << std::endl;

        is_first_iter = true;
    }

    return true;
}


// ��������� ������ ����� �������
bool NClassifier::ACalculate(void)
{
	std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
	std::shared_ptr<NPulseGeneratorTransit> generator;  // ���������

	if(IsNeedToTrain)
	{
		bool is_trained(true);

		// ��������� ����������� ���� ��������
		for(int i = 0; i < NumClasses; i++)
		{
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
				if(!trainer_weak.expired())
				 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
				else
				 trainer = nullptr;
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

			// ��������� ��� ���������� � ����� ��������
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					// CRITICAL: GetComponentL now returns weak_ptr, need to lock
					std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
					if(!trainer_weak.expired())
					 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
					else
					 trainer = nullptr;
					if(!trainer)
						return true;

		  			trainer->LTZThreshold=FixedLTZThreshold;
					for(int k = 0; k < NumInputDendrite; k++)
					{
						// CRITICAL: GetComponentL now returns weak_ptr, need to lock
						std::weak_ptr<RDK::UContainer> generator_weak = trainer->GetComponentL("Source"+sntoa(k+1),true);
						if(!generator_weak.expired())
						 generator = std::dynamic_pointer_cast<NPulseGeneratorTransit>(generator_weak.lock());
						else
						 generator = nullptr;
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

		// ��������� ����������� ���� ��������
		for(int i = 0; i < NumClasses; i++)
		{
			for(int j = 0; j < SizeTrainingSet; j++)
			{
				// CRITICAL: GetComponentL now returns weak_ptr, need to lock
				std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
				if(!trainer_weak.expired())
				 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
				else
				 trainer = nullptr;
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

			// ��������� ��� ���������� � ����� ��������
			// ��������� ����������� ���� ��������
			for(int i = 0; i < NumClasses; i++)
			{
				for(int j = 0; j < SizeTrainingSet; j++)
				{
					// CRITICAL: GetComponentL now returns weak_ptr, need to lock
					std::weak_ptr<RDK::UContainer> trainer_weak = GetComponentL("NeuronTrainer"+sntoa(i+1)+"_"+sntoa(j+1),true);
					if(!trainer_weak.expired())
					 trainer = std::dynamic_pointer_cast<NNeuronTrainer>(trainer_weak.lock());
					else
					 trainer = nullptr;
					if(!trainer)
						return true;

				for(int k = 0; k < NumInputDendrite; k++)
				{
					// CRITICAL: GetComponentL now returns weak_ptr, need to lock
					std::weak_ptr<RDK::UContainer> generator_weak = trainer->GetComponentL("Source"+sntoa(k+1),true);
					if(!generator_weak.expired())
					 generator = std::dynamic_pointer_cast<NPulseGeneratorTransit>(generator_weak.lock());
					else
					 generator = nullptr;
					if(!generator)
						return true;

					generator->UseTransitSignal = false;
				}
				}
			}
		}
		// ������� ��� ������ � �������.
		// ������������ ������ ������� ������ �� �����,
		// ��������� ����������� � ������ ����������� � ����
		if(DataFromFile)
			TreatDataFromFile();
		return true;
	}

	return true;
}

// --------------------------


}
#endif
