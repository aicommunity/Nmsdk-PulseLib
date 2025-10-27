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

#ifndef NSPIKECLASSIFIER_CPP
#define NSPIKECLASSIFIER_CPP

#include "NSpikeClassifier.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"


// �����, ��������� ������ ��������� �������� ��� ������������� ��������� �������� ���������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSpikeClassifier::NSpikeClassifier(void)
: StructureBuildMode("StructureBuildMode",this,&NSpikeClassifier::SetStructureBuildMode),
  PulseGeneratorClassName("PulseGeneratorClassName",this,&NSpikeClassifier::SetPulseGeneratorClassName),
  NeuronTrainerClassName("NeuronTrainerClassName",this,&NSpikeClassifier::SetNeuronTrainerClassName),
  NeuronClassName("NeuronClassName",this,&NSpikeClassifier::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NSpikeClassifier::SetSynapseClassName),
  NumNeurons("NumNeurons",this,&NSpikeClassifier::SetNumNeurons),
  IsNeedToTrain("IsNeedToTrain",this,&NSpikeClassifier::SetNeedToTrain),
  Delay("Delay",this,&NSpikeClassifier::SetDelay),
  SpikesFrequency("SpikesFrequency",this,&NSpikeClassifier::SetSpikesFrequency),
  NumInputDendrite("NumInputDendrite",this,&NSpikeClassifier::SetNumInputDendrite),
  MaxDendriteLength("MaxDendriteLength",this,&NSpikeClassifier::SetMaxDendriteLength),
  TrainingPatterns("TrainingPatterns",this,&NSpikeClassifier::SetTrainingPatterns),
  InputPattern("InputPattern",this,&NSpikeClassifier::SetInputPattern),
  LTZThreshold("LTZThreshold",this,&NSpikeClassifier::SetLTZThreshold),
  FixedLTZThreshold("FixedLTZThreshold",this,&NSpikeClassifier::SetFixedLTZThreshold),
  TrainingLTZThreshold("TrainingLTZThreshold",this,&NSpikeClassifier::SetTrainingLTZThreshold),
  UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NSpikeClassifier::SetUseFixedLTZThreshold),
  UseTransitSignal("UseTransitSignal",this,&NSpikeClassifier::SetUseTransitSignal),
  DataFromFile("DataFromFile",this,&NSpikeClassifier::SetDataFromFile)
{
 OldNumNeurons=0;
 OldNumInputDendrite=0;
 generators.clear();
 trainers.clear();

 // ���������� ��� ������ � ������ ������ � ����
 // ------->
 // ������� ������ ��������
 IsFirstFileStep = true;
 // <-------
}

NSpikeClassifier::~NSpikeClassifier(void)
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
bool NSpikeClassifier::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������, ���������� ��������� ���������
bool NSpikeClassifier::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������� �������
bool NSpikeClassifier::SetNeuronTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NSpikeClassifier::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NSpikeClassifier::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ����� ��������
bool NSpikeClassifier::SetNumNeurons(const int &value)
{
 Ready=false;

 OldNumNeurons = NumNeurons;
 TrainingPatterns.Resize(value,NumInputDendrite);

 return true;
}

/// ������� ������������� ��������
bool NSpikeClassifier::SetNeedToTrain(const bool &value)
{
 if(value)
 {
  OldNumNeurons=0;
  OldNumInputDendrite=0;
  generators.clear();
  trainers.clear();
  DelAllComponents();
  Ready=false;
 }
 else
 {
  std::shared_ptr<NNeuronTrainer> trainer;
  std::shared_ptr<NPulseGeneratorTransit> generator;
  for(int i = 0; i < NumNeurons; i++)
  {
   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
   if(!trainer)
	return true;
   for(int j = 0; j < NumInputDendrite; j++)
   {
	generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
	if(!generator)
	 return true;

	generator->UseTransitSignal = (value)? false : true;
   }
  }
 }
 return true;
}

/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool NSpikeClassifier::SetDelay(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->Delay = value;

 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = value + InputPattern[i];
  generators[i]->Reset();
 }
 return true;
}

/// ������� ����������� (��)
bool NSpikeClassifier::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->SpikesFrequency = value;

 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Frequency = value;
  generators[i]->Reset();
 }

 return true;
}

/// ����� ������� ���������
bool NSpikeClassifier::SetNumInputDendrite(const int &value)
{
 Ready=false;

 OldNumInputDendrite = NumInputDendrite;
 InputPattern.Resize(value,1);
 /*for(size_t i = 0; i < trainers.size(); i++)
 {
	trainers[i]->NumInputDendrite = value;
	trainers[i]->Reset();
 } */
 TrainingPatterns.Resize(NumNeurons,value);

 return true;
}

/// ������������ ����� ��������
bool NSpikeClassifier::SetMaxDendriteLength(const int &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->MaxDendriteLength = value;

 return true;
}

/// ������� ��� �����������
bool NSpikeClassifier::SetTrainingPatterns(const MDMatrix<double> &value)
{
 if(IsNeedToTrain)
 {
  Ready=false;
 }
 else
 {
  MDMatrix<double> pattern;
  pattern.Resize(NumInputDendrite,1);
  for(int i = 0; i < int(trainers.size()); i++)
  {
   for(int j = 0; j < NumInputDendrite; j++)
	pattern[j] = value(i,j);
   trainers[i]->InputPattern = pattern;
   trainers[i]->IsNeedToTrain=IsNeedToTrain;
  }
 }
 return true;
}

/// ������� ��� �������������
bool NSpikeClassifier::SetInputPattern(const MDMatrix<double> &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = Delay + value[int(i)];
  generators[i]->Reset();
 }

 return true;
}

/// ����� �������������� ���� �������
bool NSpikeClassifier::SetLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->LTZThreshold = value;

 return true;
}

/// ����� �������������� ���� ������� ��� ����� ��������
bool NSpikeClassifier::SetTrainingLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->TrainingLTZThreshold = value;

 return true;
}

/// ������������� ����� �������������� ���� �������
bool NSpikeClassifier::SetFixedLTZThreshold(const double &value)
{
 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->FixedLTZThreshold = value;

 return true;
}

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
bool NSpikeClassifier::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  double threshold = FixedLTZThreshold;
  LTZThreshold = threshold;
 }

 for(size_t i = 0; i < trainers.size(); i++)
  trainers[i]->UseFixedLTZThreshold = value;

 return true;
}

// ������������� ������������� �������� ������� �� �������� ���������
bool NSpikeClassifier::SetUseTransitSignal(const bool &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->UseTransitSignal = value;
 }

 return true;
}

// ������������� ������������� �������� ������� �� �������� ���������
bool NSpikeClassifier::SetDataFromFile(const bool &value)
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
NSpikeClassifier* NSpikeClassifier::New(void)
{
 return new NSpikeClassifier;
}

UComponent* NSpikeClassifier::NewStatic(void)
{
 return new NSpikeClassifier;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NSpikeClassifier::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ������� ������������� ������ � ������ ������ �� �����
bool NSpikeClassifier::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ����� �������� �����.
bool NSpikeClassifier::AReset(void)
{
 for(size_t i = 0; i < trainers.size(); i++)
 {
//  trainers[i]->IsNeedToTrain = IsNeedToTrain;

/*  std::shared_ptr<NPulseGeneratorTransit> generator;
  for(int j = 0; j < NumInputDendrite; j++)
  {
   generator = trainers[i]->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
   if(!generator)
	return true;

   generator->UseTransitSignal = (IsNeedToTrain)? false : true;
  }       */

 // trainers[i]->Delay = Delay;
 // trainers[i]->SpikesFrequency = SpikesFrequency;
 // trainers[i]->NumInputDendrite = NumInputDendrite;
 // trainers[i]->MaxDendriteLength = MaxDendriteLength;
  //trainers[i]->LTZThreshold = LTZThreshold;
  //trainers[i]->FixedLTZThreshold = FixedLTZThreshold;
  //trainers[i]->UseFixedLTZThreshold = UseFixedLTZThreshold;
	 /*
  MDMatrix<double> pattern;
  pattern.Resize(NumInputDendrite,1);
  for(int j = 0; j < NumInputDendrite; j++)
   pattern[j] = TrainingPatterns(i,j);
  trainers[i]->InputPattern = pattern;
       */
 // trainers[i]->Reset();
 }
	 /*
 for(size_t i = 0; i < generators.size(); i++)
 {
  generators[i]->Delay = Delay + InputPattern[i];
  generators[i]->Frequency = SpikesFrequency;
//  generators[i]->UseTransitSignal = UseTransitSignal;
  generators[i]->Reset();
 }   */

 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NSpikeClassifier::ADefault(void)
{
 StructureBuildMode=1;
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NeuronTrainerClassName="NNeuronTrainer";
 NeuronClassName="NSPNeuronGen";
 SynapseClassName="NPSynapseBio";    // NPSynapse
 NumNeurons = 1;
 IsNeedToTrain = true;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 MaxDendriteLength = 100;
 LTZThreshold = 0.0117;
 TrainingLTZThreshold=100;
 FixedLTZThreshold = 0.0117;
 UseFixedLTZThreshold = true;
 UseTransitSignal = false;
 TrainingPatterns.Assign(1,1,0.0);
 InputPattern.Assign(1,1,0.0);
 DataFromFile = false;

 return true;
}

// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NSpikeClassifier::BuildStructure(void)
{
 // 1 - ������ ������ ��������, ���������� �������������� ��� ������������� ��������� ��������
 if(StructureBuildMode == 1)
 {
	// ������� ������ ������� � ������������ ��������
	for(int i = NumNeurons; i < OldNumNeurons; i++)
	{
		DelComponent(std::string("NeuronTrainer")+sntoa(i+1));
	}
	// �������������� ������� � ������������ ��������
	OldNumNeurons = int(trainers.size());
	trainers.resize(NumNeurons);
	for(int i = 0; i < NumNeurons; i++)
	{
		trainers[i] = AddMissingComponent<NNeuronTrainer>(std::string("NeuronTrainer")+sntoa(i+1), NeuronTrainerClassName);
		trainers[i]->SetCoord(MVector<double,3>(8.7+1*7,1.67+i*2,0));
		trainers[i]->NumInputDendrite = NumInputDendrite;
        trainers[i]->Reset();
		trainers[i]->LTZThreshold = LTZThreshold;
		trainers[i]->TrainingLTZThreshold = TrainingLTZThreshold;
        trainers[i]->FixedLTZThreshold = FixedLTZThreshold;

		MDMatrix<double> pattern;
		pattern.Resize(NumInputDendrite,1);
		for(int j = 0; j < NumInputDendrite; j++)
		 pattern[j] = TrainingPatterns(i,j);
		trainers[i]->InputPattern = pattern;
		trainers[i]->IsNeedToTrain = IsNeedToTrain;
		trainers[i]->Delay = Delay;
		trainers[i]->SpikesFrequency = SpikesFrequency;
 		trainers[i]->MaxDendriteLength = MaxDendriteLength;
		trainers[i]->Reset();
	}

	// ������� ������ ����������
	for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));
	}
	// �������������� ���������� ���������
    //OldNumInputDendrite = int(generators.size());
	generators.resize(NumInputDendrite);
	for(int i = 0; i < NumInputDendrite; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), PulseGeneratorClassName);
		generators[i]->SetCoord(MVector<double,3>(6.7,1.67+i*2,0));
		generators[i]->UseTransitSignal = UseTransitSignal;
		generators[i]->Delay = Delay + InputPattern[i];
		generators[i]->Frequency = SpikesFrequency;
  		generators[i]->Reset();
	}
	// ��������� �������� ����� �����������
	for(int i = 0; i < NumInputDendrite; i++)
	{
		generators[i]->DisconnectAll("Output");
	}

	// �������� ����� ����� ������������ � ��������� � ������������ ��������
	std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
	std::shared_ptr<NPulseGeneratorTransit> gen_in; // ��������������� ���� ������� � ������������ ��������
	for(int i = 0; i < NumInputDendrite; i++)
	{
	 for(int j = 0; j < NumNeurons; j++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(j+1)),true);
	  if(!trainer)
	   return true;
	  gen_in = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(i+1)),true);
	  if(!gen_in)
	   return true;

	  gen_in->UseTransitSignal=(IsNeedToTrain.v)? false : true;


	  bool res(true);
	  res&=CreateLink("Source"+sntoa(i+1),"Output",gen_in->GetLongName(GetThisAsSharedContainer()),"Input");
	  if(!res)
	   return true;
	 }
	}
 }

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NSpikeClassifier::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure();
  if(!res)
   return false;
 }

 OldNumNeurons=0;
 OldNumInputDendrite=0;
 return true;
}

// ������� ��� ������ � �������.
// ������������ ������ ������� ������ �� �����,
// ��������� ����������� � ������ ����������� � ����
bool NSpikeClassifier::TreatDataFromFile(void)
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
		outputs.assign(NumNeurons,0);
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
	std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
	std::shared_ptr<NPulseNeuron> neuron;
	std::shared_ptr<NLTZone> ltzone;
	for(int i= 0; i < NumNeurons; i++)
	{
		trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
		if(!trainer)
			return true;

		neuron  = trainer->GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
		if(!neuron)
			return true;

		ltzone = neuron->GetComponentL<NLTZone>("LTZone");
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
		for(int i = 0; i < NumNeurons; i++)
		{
			fout << outputs[i] << "\t";
		}
		fout << std::endl;

		is_first_iter = true;
	}

	return true;
}

// ��������� ������ ����� �������
bool NSpikeClassifier::ACalculate(void)
{
	std::shared_ptr<NNeuronTrainer> trainer; // ������ � ������������ ��������
	std::shared_ptr<NPulseGeneratorTransit> generator;  // ���������

	// ������� ��������
	if(IsNeedToTrain)
	{
	 bool is_trained(true);

	 // ��������� ����������� ���� ��������
	 for(int i = 0; i < NumNeurons; i++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	  if(!trainer)
	   return true;

	  if(trainer->IsNeedToTrain)
	  {
	   is_trained = false;
	   break;
	  }
	 }

	 if(is_trained)
	 {
	  IsNeedToTrain = false;

	  // ��������� ��� ���������� � ����� ��������
      for(int i = 0; i < NumNeurons; i++)
	  {
	   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	   if(!trainer)
		return true;

       trainer->LTZThreshold=FixedLTZThreshold;

	   for(int j = 0; j < NumInputDendrite; j++)
	   {
		generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
		if(!generator)
		 return true;

		generator->UseTransitSignal = true;
	   }

       // �������� ����� ����� ��������� � ������������ ��������
       std::shared_ptr<NPulseNeuron> neuron;
       std::shared_ptr<NPulseMembrane> soma;
       std::shared_ptr<NPulseSynapse> synapse;
       std::shared_ptr<NLTZone> ltzone;
       std::vector<int> inh_synapse_counter;
       inh_synapse_counter.assign(NumNeurons,1);
       for(int i = 0; i < NumNeurons; i++)
       {
        trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
        if(!trainer)
         return true;

        neuron  = trainer->GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
        if(!neuron)
         return true;

        ltzone = neuron->GetComponentL<NLTZone>("LTZone");
        if(!ltzone)
         return true;

        for(int j = 0; j < NumNeurons; j++)
        {
         if(i==j)
          continue;

         neuron  = GetComponentL<NPulseNeuron>(std::string("NeuronTrainer"+sntoa(j+1)+".Neuron"),true);
         if(!neuron)
          return true;

         for(int n = 0; n < NumInputDendrite; n++)
         {
          soma = neuron->GetComponentL<NPulseMembrane>(std::string("Soma"+sntoa(n+1)),true);
          if(!soma)
           return true;

          soma->NumInhibitorySynapses=NumNeurons-1;
          soma->Reset();
          synapse = soma->GetComponentL<NPulseSynapse>(std::string("InhSynapse")+RDK::sntoa(inh_synapse_counter[j]),true);
          if(!synapse)
           return true;

          bool res(true);
          res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",synapse->GetLongName(GetThisAsSharedContainer()),"Input");
          if(!res)
           return true;
         }
         ++inh_synapse_counter[j];
        }
       }
	  }
	 }

	 return true;
	}
    // ������� �������������
	else
	{
	 bool is_trained(true);

	 // ��������� ����������� ���� ��������
	 for(int i = 0; i < NumNeurons; i++)
	 {
	  trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	  if(!trainer)
	   return true;

	  if(trainer->IsNeedToTrain)
	  {
	   is_trained = false;
	   break;
	  }
	 }

	 if(!is_trained)
	 {
	  IsNeedToTrain = true;

	  // ��������� ��� ���������� � ����� ���������
      for(int i = 0; i < NumNeurons; i++)
	  {
	   trainer = GetComponentL<NNeuronTrainer>(std::string("NeuronTrainer"+sntoa(i+1)),true);
	   if(!trainer)
		return true;

	   for(int j = 0; j < NumInputDendrite; j++)
	   {
		generator = trainer->GetComponentL<NPulseGeneratorTransit>(std::string("Source"+sntoa(j+1)),true);
		if(!generator)
		 return true;

		generator->UseTransitSignal = false;
	   }
	  }

	  return true;
	 }

		// ������� ��� ������ � �������.
		// ������������ ������ ������� ������ �� �����,
		// ��������� ����������� � ������ ����������� � ����
		if(DataFromFile)
			TreatDataFromFile();
	}

	return true;
}

// ������������� ���������� � ��������� ������� �������
void NSpikeClassifier::UpdateComputationOrder(void)
{
 int position=0;
 for(size_t i=0;i<generators.size();i++)
  SetComponentPosition(generators[i]->GetName(),position++);
 for(size_t i=0;i<trainers.size();i++)
  SetComponentPosition(trainers[i]->GetName(),position++);
}
// --------------------------


}
#endif
