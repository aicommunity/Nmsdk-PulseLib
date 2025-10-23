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

#ifndef NNEURONLEARNER_CPP
#define NNEURONLEARNER_CPP

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>
#include "NNeuronLearner.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------

NNeuronLearner::NNeuronLearner(void):
 StructureBuildMode("StructureBuildMode",this,&NNeuronLearner::SetStructureBuildMode),
 PulseGeneratorClassName("PulseGeneratorClassName",this,&NNeuronLearner::SetPulseGeneratorClassName),
 NeuronClassName("NeuronClassName",this,&NNeuronLearner::SetNeuronClassName),
 SynapseClassName("SynapseClassName",this,&NNeuronLearner::SetSynapseClassName),
 CalculateMode("CalculateMode",this, &NNeuronLearner::SetCalculateMode),
 IsNeedToTrain("IsNeedToTrain",this,&NNeuronLearner::SetIsNeedToTrain),
 Delay("Delay",this,&NNeuronLearner::SetDelay),
 SpikesFrequency("SpikesFrequency",this,&NNeuronLearner::SetSpikesFrequency),
 NumInputDendrite("NumInputDendrite",this,&NNeuronLearner::SetNumInputDendrite),
 MaxDendriteLength("MaxDendriteLength",this,&NNeuronLearner::SetMaxDendriteLength),
 InputPattern("InputPattern",this,&NNeuronLearner::SetInputPattern),
 AdditionalInputPattern("AdditionalInputPattern",this,&NNeuronLearner::SetAdditionalInputPattern),
 DendriteNeuronAmplitude("DendriteNeuronAmplitude",this),
 SomaNeuronAmplitude("SomaNeuronAmplitude",this),
 LTZThreshold("LTZThreshold",this,&NNeuronLearner::SetLTZThreshold),
 FixedLTZThreshold("FixedLTZThreshold",this,&NNeuronLearner::SetFixedLTZThreshold),
 TrainingLTZThreshold("TrainingLTZThreshold",this,&NNeuronLearner::SetTrainingLTZThreshold),
 UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NNeuronLearner::SetUseFixedLTZThreshold),
 Output("Output",this),
 SynapseResistanceStep("SynapseResistanceStep", this, &NNeuronLearner::SetSynapseResistanceStep),
 DendriteLength("DendriteLength", this, &NNeuronLearner::SetDendriteLength),
 InitialSomaPotential("InitialSomaPotential", this, &NNeuronLearner::SetInitialSomaPotential),
 NumSynapse("NumSynapse", this, &NNeuronLearner::SetNumSynapse),
 ExperimentNum("ExperimentNum", this, &NNeuronLearner::SetExperimentNum),
 ExperimentMode("ExperimentMode", this, &NNeuronLearner::SetExperimentMode)
{
 OldNumInputDendrite = 0;
 Generators.clear();
 Neuron = NULL;

 // ���� ������������� ������� ���� � �������� �������
 IsFirstFileStep = true;
}


NNeuronLearner::~NNeuronLearner(void)
{
 OldNumInputDendrite = 0;
}

// --------------------------



// --------------------------
// ������ ��������� �����������
// --------------------------

/// ��������� ������ ������ ��������� �������
bool NNeuronLearner::SetStructureBuildMode(const int &value)
{
 if(value > 0)
  Ready = false;
 return true;
}

/// ��������� ����� ������ ����������� ���������
bool NNeuronLearner::SetPulseGeneratorClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// ��������� ����� ������ �������
bool NNeuronLearner::SetNeuronClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// ��������� ����� ������ ��������
bool NNeuronLearner::SetSynapseClassName(const std::string &value)
{
 Ready = false;
 return true;
}

/// ��������� ���������� � ��������, �������� ������ ������, ������ ��������� � ����� ��������
bool NNeuronLearner::ZeroingTrainingPattern(void)
{
 // �������, �������� ������ ������
 MDMatrix<double> zero;
 zero.Assign(NumInputDendrite, 1, 0.0);
 if (Neuron)
  Neuron->TrainingPattern = zero;

 // ���������� � ���������
 MDMatrix<int> temp;
 temp.Assign(NumInputDendrite, 1, 1);
 if (Neuron)
 {
  // ���������� ��������� � ���������
  Neuron->TrainingDendIndexes = temp;
  // ���������� �������� �� ������� ��������� ���������
  Neuron->TrainingSynapsisNum = temp;
 }


 return true;
}

/// ��������� ������������� ������ �������������� ���� �������
bool NNeuronLearner::SetLTZThreshold(const double &value)
{
 std::shared_ptr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 std::shared_ptr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");//GetLTZone();
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
 {
  UseFixedLTZThreshold = false;
 }

 return true;
}

/// ��������� (�������� ��� ���������) ����� ������������� �������� ���������� ��������
bool NNeuronLearner::SetIsNeedToTrain(const bool &value)
{
 if(CalculateMode == 1 && !value)
 {
  ZeroingTrainingPattern();
 }

 CountIteration = 0;  // �������� ������� ��������
 IsFirstBeat = true;

 // ��� �������� ����� ���� ���������� ������ LTZone ������� ��������
 if(value)
 {
  SetLTZThreshold(TrainingLTZThreshold.v);
  LTZThreshold.v = TrainingLTZThreshold.v;
 }
 // ��� ��������� ����� ���� ���������� ������������� ��������
 else
 {
  SetLTZThreshold(FixedLTZThreshold.v);
  LTZThreshold.v = FixedLTZThreshold.v;
 }
 return true;
}

/// ��������� ������ ������ (��������, �������������) �������:
/// ��� �������� ����� IsNeedToTrain ���������� ��������, ����� - �������������
/// 0 - ������ ������ NeuronTrainer: ����� ����, ��� ������ ������ ������,
/// ������������� ���������� ���� IsNeedToTrain, ����� ���������� �������������
/// 1 - ���� IsNeedToTrain ������������� �� ����������, ����� ������������ ������� ���������� ������
bool NNeuronLearner::SetCalculateMode(const int &value)
{
 // �������� ������������ ��������� ��������
 if((value != 0) && (value != 1))
  return false;

// // ��������� ���� ������������� ��������
// SetIsNeedToTrain(true);
// IsNeedToTrain = true;

 CountIteration = 0;  // �������� ������� ��������
 IsFirstBeat = true;

 return true;
}

/// ��������� ������� �������� ������ �������� ������������ ������ ������� (���)
bool NNeuronLearner::SetDelay(const double &value)
{
 for(int i = 0; i < int(Generators.size()); i++)
 {
  Generators[i]->Delay = value + InputPattern[i];
  Generators[i]->Reset();
 }
 return true;
}

/// ��������� ������� ����������� (��)
bool NNeuronLearner::SetSpikesFrequency(const double &value)
{
 for(size_t i = 0; i < Generators.size(); i++)
 {
  Generators[i]->Frequency = value;
  Generators[i]->Reset();
 }

 return true;
 }

/// ��������� ����� ������� ���������
bool NNeuronLearner::SetNumInputDendrite(const int &value)
{
 if(value < 1)
  return false;
 Ready = false;
 OldNumInputDendrite = NumInputDendrite;
 return true;
}

/// ��������� �������� ������������ ����� ��������� (= ����� ��������� � ���������)
bool NNeuronLearner::SetMaxDendriteLength(const int &value)
{
 if(value < 1)
  return false;

 // ��������� � ������� ������������ ������ �������
 for (int i = 0; i < NumInputDendrite; i++)
 {
  if (DendriteLength.empty())
   return true;

  // ���� ������� ������� ������, ��� ������ ���������,
  // �� �������� ������� �� ������� ������������ ��������� �����
  if (DendriteLength[i] > value)
  {
   DendriteLength[i] = value;

   // ������� ����� ����������
   Generators[i]->DisconnectAll("Output");

   if (!Neuron)
    return true;

   // ��������� ���������� � ������� � ������ ���������
   if(Neuron->StructureBuildMode != 2)
   {
    Neuron->NumDendriteMembraneParts = 1;
   }
   else
   {
    vector<int> temp;
    temp = Neuron->NumDendriteMembranePartsVec;
    temp[i] = value;
    Neuron->NumDendriteMembranePartsVec = temp;
   }
   // Neuron->GetStorage()->FreeObjectsStorage();
   Neuron->Reset();
   // Neuron->GetStorage()->FreeObjectsStorage();

   // ������� ��������� �������� ��������
   std::shared_ptr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                             + "_" + sntoa(DendriteLength[i]), true);
   if(!dendrite)
    return true;

   // ������ ����� �������� �� ��������. ��������������� �� ��, ��� � ����
   dendrite->NumExcitatorySynapses = NumSynapse[i];
//   dendrite->GetStorage()->FreeObjectsStorage();
   dendrite->Build();
//   dendrite->GetStorage()->FreeObjectsStorage();

   // ��� ������� �������, ����� ��������, ��� ��� �� ����� �������� � ������� �����������,
   // ��������� ����� � ����������� � ������ �������������
   for(int k = 0; k < NumSynapse[i]; k++)
   {
    NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(k);

    // �������� ����� ����� ������� ����������� � ��������
    bool res = CreateLink("Source" + sntoa(i + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input");

    if(!res || k == 0)
     continue;

    synapse->Resistance = SynapseResistanceStep;
   }
   Neuron->Reset();
  }
 }

 return true;
}

/// ��������� �������� ��� �����������, ��������������� �� TTFS (�������� ������������ ������� �������� ��� ������� ��������)
bool NNeuronLearner::SetInputPattern(const MDMatrix<double> &value)
{
 for(int i = 0; i < int(Generators.size()); i++)
 {
  Generators[i]->Delay = Delay + value[i];
  Generators[i]->Reset();
 }

 // ��������� ���� ������� ����� ��������
 IsFirstBeat = true;
 // ��� ����� �������� �������� ������� ��������
 CountIteration = 0;
 return true;
}

 /// ��������� ��������������� ��������, ������� ��������� � ������������� ��� ��������� ������������� ��������
bool NNeuronLearner::SetAdditionalInputPattern(const MDMatrix<double> &value)
{
 return true;
}

/// ��������� �������� ������ �������������� ���� �������, ���������������� �� ����� ��������
bool NNeuronLearner::SetTrainingLTZThreshold(const double &value)
{
 return true;
}

/// ��������� �������������� �������� (�������� �� ���������) ������ �������������� ���� �������
bool NNeuronLearner::SetFixedLTZThreshold(const double &value)
{
 return true;
}

/// ��������� �������� ������������� ������������� �������������� ������ �������������� ���� �������
bool NNeuronLearner::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  SetLTZThreshold(FixedLTZThreshold.v);
  LTZThreshold.v = FixedLTZThreshold.v;
 }

 return true;
}

/// ��������� �������� ������������� ����������� � �������� �������� ������������ ��������
bool NNeuronLearner::SetSynapseResistanceStep(const double &value)
{

 // ������ ������������� � ������������ ���������� ��������
 // �������������, ��� ����� ������� ���� ������ �� ������� ��������� ���������
 if (NumSynapse.empty() || !Neuron)
  return true;

 for(int numdend = 1; numdend <= NumInputDendrite; numdend++)
 {
  for(int numsyn = 2; numsyn <= NumSynapse[numdend - 1]; numsyn++)
  {
   std::shared_ptr<NPulseSynapse> synapse = Neuron->GetComponentL<NPulseSynapse>("Dendrite" +
                                                                       sntoa(numdend) + "_" + sntoa(DendriteLength[numdend - 1]) +
                                                                       ".ExcSynapse" + sntoa(numsyn), true);
   if(!synapse)
    continue;

   synapse->Resistance = value;
  }
 }
 return true;
}

/// ��������� ������ ���������� �������������
bool NNeuronLearner::SetExperimentMode(const bool &value)
{
 if(!value)
 {
  Fin.close();
  Fout.close();
  CanChangeDendLength = true;
 }
 else
 {
  ExperimentNum = 1;  // �� ��������� �������� 1-� �����������
  EpochCur = 1;
 }
 IsFirstFileStep = true;
 return true;
}

/// ��������� �������� ������� ���� ��������� (��������� ��������� � ���������)
bool NNeuronLearner::SetDendriteLength(const std::vector<int> &value)
{
 OldDendriteLength = DendriteLength;
 Ready = false;
 return true;
}

/// ������ �������� ���������� ����������� �� ������� ��������� ����,
/// ����� ������ ��������� � �������� ���������, �� ���� ��� ��������
/// ������� �� ������ �������� � ����� ������ �� ������ ���������� � ������������� �������
bool NNeuronLearner::SetInitialSomaPotential(const std::vector<double> &value)
{
 return true;
}

/// ��������� �������� ������� ��������� ������������ �������� �� ���������
bool NNeuronLearner::SetNumSynapse(const std::vector<int> &value)
{
 Ready = false;
 return true;
}

/// ��������� ������ ������������
/// 1 - PatternRecognition()
/// 2 - LearningSecondPattern()
bool NNeuronLearner::SetExperimentNum(const int &value)
{
 CalculateMode = 1;
 SetCalculateMode(1);
 IsFirstFileStep = true;
 if (value == 1)
 {
  IsNeedToTrain = false;
  SetIsNeedToTrain(false);
 }
 if (value == 2)
 {
  IsNeedToTrain = true;
  SetIsNeedToTrain(true);
 }
 return true;
}

// --------------------------



// --------------------------
// ��������� ������ ���������� ��������
// --------------------------

/// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNeuronLearner* NNeuronLearner::New(void)
{
 return new NNeuronLearner;
}

UComponent* NNeuronLearner::NewStatic(void)
{
 return new NNeuronLearner;
}

// --------------------------



// --------------------------
// ������� ������ ���������� ������������
// --------------------------

/// ��������� ����������� ���������������� ��������
/// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ��� ������� �������� � ������ ���������
bool NNeuronLearner::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 return true;
}

/// ��������� ��������������� ���������������� ��������
/// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp ���������� � ������ ���������
bool NNeuronLearner::ADelComponent(std::shared_ptr<UContainer> comp)
{
 return true;
}

// --------------------------



// --------------------------
// ������� ������ ���������� ������
// --------------------------

/// ������������ ������ ��������� � ������������ � ���������� ���������� ����������
bool NNeuronLearner::BuildStructure()
{
 bool res(true);

 // C����� ������
 Neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
 Neuron->SetCoord(MVector<double,3>(8.7 + 1 * 7, 1.67, 0));

 // ������������� ���������� ���
 Neuron->NumSomaMembraneParts = NumInputDendrite;

 // ������������� ���������� � ����� ���������
 if(Neuron->StructureBuildMode != 2)
  Neuron->NumDendriteMembraneParts = 1;
 else
 {
  // ��������� ���������� � NeuronLearner
  DendriteLength.resize(NumInputDendrite, 1);
  OldDendriteLength.resize(NumInputDendrite, 1);
  // ��������� ���������� � �������
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
 }
 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
// if(Neuron && Neuron->GetStorage())
  // Neuron->GetStorage()->FreeObjectsStorage();

 // ������� ������ ����������
 for(int i = NumInputDendrite; i < OldNumInputDendrite; i++)
 {
  DelComponent(std::string("Source") + sntoa(i + 1));
 }
 Storage.lock()->FreeObjectsStorage();

 // �������������� ���������� ���������
 Generators.resize(NumInputDendrite);
 for(int i = 0; i < NumInputDendrite; i++)
 {
  Generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source") + sntoa(i + 1), PulseGeneratorClassName);
  Generators[i]->SetCoord(MVector<double,3>(6.7, 1.67 + i * 2, 0));
  Generators[i]->Frequency = SpikesFrequency;
 }

 // ��������� �������� ����� �����������
 for(int i = 0; i < NumInputDendrite; i++)
 {
  Generators[i]->DisconnectAll("Output");
 }

 // ������ ���������� �������� �� ��������
 // ��� ������������� ������ ������
 if (NumInputDendrite != NumSynapse.size())
  NumSynapse.resize(NumInputDendrite, 1);

 // ������������� �� ��������� ���������� ���������� ��������
 for (int i = 0; i < NumInputDendrite; i++)
 {
  // ������� ���������� ������� �� �������� ��������, ������� ������ ��� �������
  if ((i < OldDendriteLength.size()) && (i < DendriteLength.size()) && (OldDendriteLength[i] < DendriteLength[i]))
  {
   std::shared_ptr<NPulseMembrane> previnputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                              "_" + sntoa(OldDendriteLength[i]), true);
   if(previnputsegmentofdendrite)
   {
    previnputsegmentofdendrite->NumExcitatorySynapses = 1;
    previnputsegmentofdendrite->Build();
   }
  }

  // ������� ������� ������� ��������
  std::shared_ptr<NPulseMembrane> inputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(i + 1) +
                                                                            + "_" + sntoa(DendriteLength[i]), true);
  if(!inputsegmentofdendrite)
   continue;

  // ������ ����� �������� �� ��������. ��������������� �� ��, ��� � ����
  inputsegmentofdendrite->NumExcitatorySynapses = NumSynapse[i];
//  dendrite->GetStorage()->FreeObjectsStorage();
  inputsegmentofdendrite->Build();
//  dendrite->GetStorage()->FreeObjectsStorage();
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 // �������� ����� ����� ������������ � ���������, ������ ������������� ��������
 for(int numdend = 0; numdend < NumInputDendrite; numdend++)
 {
  // ������� ������� ������� ��������
  std::shared_ptr<NPulseMembrane> segmentofdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(numdend + 1) +
                                                                            + "_" + sntoa(DendriteLength[numdend]), true);
  // ��� ������� �������
  for(int numsyn = 0; numsyn < NumSynapse[numdend]; numsyn++)
  {
   NPulseSynapseCommon *synapse = segmentofdendrite->GetExcitatorySynapses(numsyn);

   if(!synapse)
   {
    LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Can't create link because synapse not exists: "));
    continue;
   }

   // ������ �������������
   if(numsyn != 0)
    synapse->Resistance = SynapseResistanceStep;

   // ������������� �����
   if (!CheckLink("Source" + sntoa(numdend + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input"))
    res &= CreateLink("Source" + sntoa(numdend + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input");

   if(!res)
    continue;
  }
 }
 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 // ����� ������� � ��������� �������� ���������� ��������
 IsFirstBeat = true; // ���� ������� ����� � ������ �������� ������� �������
 // ������������ ��������� ��������� �������, ������ �� ��������
 DendriteNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 // ������������ ��������� ��������� �������, ������ �� ����
 SomaNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);

 // ��� ������ �������� �������������,
 // ��� ��� �������� �� ������ �����
 DendStatus.assign(NumInputDendrite, 0);
 // ��� � �������
 SynapseStatus.assign(NumInputDendrite, 0);

 // ������ �������� ����������� �� ������� ��������� ��� �� ��������� �����
 if (NumInputDendrite != InitialSomaPotential.size())
 {
  std::vector<double> newinitialsomapotential;
  newinitialsomapotential.assign(NumInputDendrite, 0.0);
  // ����������� ������ ����������
  for (int i = 0; i < std::min(NumInputDendrite.v, int(InitialSomaPotential.size())); i++)
  {
   newinitialsomapotential[i] = InitialSomaPotential[i];
  }
  InitialSomaPotential = newinitialsomapotential;
 }

 // ����� ��� ������ ����, ����� ��������� ��������� ������� �� ��� ����������� � ������� ��������
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 double period = 1.0 / SpikesFrequency + 0.001;
 // ������� ���������������� �������� �� �����
 Dissynchronization.assign(NumInputDendrite, period);
 Dissynchronization[NumInputDendrite - 1] = 0.0;
 // ������� ����� ������� ���������� �� ���� � ��������
 AmpDifference.assign(NumInputDendrite, 0.0);


 InputPattern.Resize(NumInputDendrite, 1, 0.0);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);  // ��������� �������� ����������� ��������
 AdditionalInputPattern.Resize(NumInputDendrite, 1, 0.0);
 return true;
}


/// �������������� �������� �� ��������� � ����� �������� �����
bool NNeuronLearner::ADefault(void)
{
 Generators.clear();
 StructureBuildMode = 1;
 CalculateMode = 0;
 PulseGeneratorClassName = "NPulseGeneratorTransit";
 NeuronClassName = "NSPNeuronGen";
 SynapseClassName = "NPSynapseBio";
 IsNeedToTrain = true;
 ExperimentMode = false;
 Delay = 0.1;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 OldNumInputDendrite = 0;
 MaxDendriteLength = 100;

 // ��������� ������
 LTZThreshold = 100;
 FixedLTZThreshold = 0.0115;
 TrainingLTZThreshold = 100;
 UseFixedLTZThreshold = false;

 // ��������
 InputPattern.Resize(NumInputDendrite, 1);
 AdditionalInputPattern.Resize(NumInputDendrite, 1);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);

 // ��� ��������
 DendriteNeuronAmplitude.Assign(NumInputDendrite, 1, 0.0);
 SomaNeuronAmplitude.Assign(NumInputDendrite, 1, 0.0);
 Output.Assign(NumInputDendrite, 1, 0.0);

 SynapseResistanceStep = 1.0e9;
 DendriteLength.assign(NumInputDendrite, 1);
 OldDendriteLength.assign(NumInputDendrite, 1);
 NumSynapse.assign(NumInputDendrite, 1);

 CountIteration = 0;
 ExperimentNum = 0;  // ����� �� ���������, ������������ ���
 EpochCur = 0;
 CanChangeDendLength = true;

 IsWritten = false; // ���� ����������� �������� �������� ������ � ����

 // ����� ������� � ��������� �������� ���������� ��������
 IsFirstBeat = true; // ���� ������� ����� � ������ �������� ������� �������

 // ������������ ��������� ��������� �������, ������ �� ��������
 DendriteNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 // ������������ ��������� ��������� �������, ������ �� ����
 SomaNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 InitialSomaPotential.assign(NumInputDendrite, 0.0);
 // ������������ ��������� ��������� ������� �� ����� ��� ������ ��������
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 // ����� ��� ������ ����, ����� ��������� ��������� ������� �� ��� ����������� � ������� ��������
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 // ������� ���������������� �������� �� �����
 Dissynchronization.assign(NumInputDendrite, 1.0 / SpikesFrequency + 0.001);
 // ������� ����� ������� ���������� �� ���� � ��������
 AmpDifference.assign(NumInputDendrite, 0.0);

 // ��� ������ �������� �������������, ��� ��� �������� �� ������ �����
 DendStatus.assign(NumInputDendrite, 0);
 // ��� � �������
 SynapseStatus.assign(NumInputDendrite, 0);

 return true;
}


/// ������������ ������ ���������� ��������� ������� ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NNeuronLearner::ABuild(void)
{
 bool res(true);
 if(StructureBuildMode == 1)
 {
  res = BuildStructure();
  if(!res)
   return false;
 }

 return true;
}


/// ����� �������� �����
bool NNeuronLearner::AReset(void)
{
 // ������������� �������� ������ �� ���������
 std::shared_ptr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 std::shared_ptr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = LTZThreshold;

 // ����� �������� ������� �� �����������
 for(size_t i = 0; i < Generators.size(); i++)
 {
  Generators[i]->Delay = Delay + InputPattern[i];
  Generators[i]->Frequency = SpikesFrequency;
  Generators[i]->Reset();
 }
 Output.ToZero();

 return true;
}


/// ���������� ����������� �������� ������� ���������
/// prev_input_pattern - ������� ������� �� ���������� �������� ��������
/// input_pattern - ������� ������� �� ������� �������� ��������
/// e - ���������� ����������� �������� ���������, ��� ������������ ������� �������� ��������� �����������
bool NNeuronLearner::CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e)
{
 bool res(true);
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // ���� ���� �� ��� ������ ����� ������� ��������� ���������� �����������,
  // �� �������, ��� �������� ������
  if(fabs(prev_input_pattern[i] - input_pattern[i]) > e)
  {
   res = false;
   break;
  }
 }
 return res;
}


/// ��������� ����� �������� (����� ��������� � ��������) num
bool NNeuronLearner::ChangeDendriteLength(int num)
{
 bool res(true); // ���������� ��� ����������� ���������� �������

 // ���� �� ����� ������ ����� ��������� - ������ �� ������
 if(!DendStatus[num])
  return true;


 // ���� ��������� ������� ������������ ���������� ���� ��������, ��
 // �������, ��� ���������� ����������� ����� ��������
 if((DendStatus[num] == -1) && (DendriteLength[num] < 2))
 {
  DendStatus[num] = 0;
  return true;
 }


 // ���������� ��������� ����� ��������
 if(DendStatus[num] == 1)
 {
  // ��������� ������� �� ������������ ����� ���������
  if(DendriteLength[num] >= MaxDendriteLength)
  {
   DendStatus[num] = 0;
   return true;
  }
 }


 // �������� ���������� � ����� ��������� � NeuronLearner
 OldDendriteLength[num] = DendriteLength[num];
 DendriteLength[num] += DendStatus[num];


 // ������� ����� ���������� � ���������
 Generators[num]->DisconnectAll("Output");


 // �������� ���������� � ����� �������� ��������� � ������ �������
 if(Neuron->StructureBuildMode != 2)
 {
  if(Neuron->NumDendriteMembraneParts != DendriteLength[num])
  {
   Neuron->NumDendriteMembraneParts = DendriteLength[num];
   // Neuron->GetStorage()->FreeObjectsStorage();
   Neuron->Reset();
   // Neuron->GetStorage()->FreeObjectsStorage();
  }
 }
 else
 {
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
  // Neuron->GetStorage()->FreeObjectsStorage();
  Neuron->Reset();
  // Neuron->GetStorage()->FreeObjectsStorage();

  // ����������, ��� ����� ���������� ���������
  int templength = Neuron->NumDendriteMembranePartsVec[num];
  if(DendriteLength[num] != templength)
  {
   LogMessageEx(RDK_EX_WARNING, __FUNCTION__, "Incorrect dendtic length after INCREASE. Need " +
                sntoa(DendriteLength[num]) + " current length " + sntoa(templength));
  }
 }


 // ���� ��������� �������, �� �� ���������� ����� �������� ������ 1 ������
 if((DendStatus[num] == 1) && (DendriteLength[num] > 1))
 {
  std::shared_ptr<NPulseMembrane> prevdendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                             "_" + sntoa(DendriteLength[num] - 1), true);
  if(!prevdendrite)
   return true;

  prevdendrite->NumExcitatorySynapses = 1;
//  prevdendrite->GetStorage()->FreeObjectsStorage();
  prevdendrite->Build();
//  prevdendrite->GetStorage()->FreeObjectsStorage();
 }


 // �������� ���������� �������� � ����� ��������
 std::shared_ptr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                         "_" + sntoa(DendriteLength[num]), true);
 if(!dendrite)
  return true;
 dendrite->NumExcitatorySynapses = NumSynapse[num];
// dendrite->GetStorage()->FreeObjectsStorage();
 dendrite->Build();
// dendrite->GetStorage()->FreeObjectsStorage();


 // �������� ����� ����� ������� ����������� � ��������� �� ����� ��������
 for (int i = 0; i < NumSynapse[num]; i++)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(i);

  res &= CreateLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input");
  if(!res)
   return true;

  if(i > 0)
   synapse->Resistance = SynapseResistanceStep;
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 return true;
}


/// ��������� ����� ������������ �������� �� ������� �������� �������� num
bool NNeuronLearner::ChangeSynapseNumber(int num)
{
 bool res(true); // ���������� ��� ����������� ���������� �������

 // ���� �� ����� ������ ����� �������� - ������ �� ������
 if(!SynapseStatus[num])
  return true;

 // ���� ��������� ������� ������������ ���������� ������, ��
 // �������, ��� ���������� ����������� ���������� ��������
 if((SynapseStatus[num] == -1) && (NumSynapse[num] < 2))
 {
  SynapseStatus[num] = 0;
  return true;
 }

 // �������� ���������� � ���������� �������� � NeuronLearner
 NumSynapse[num] += SynapseStatus[num];

 // ������� ��������� �������� ��������
 std::shared_ptr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite" + sntoa(num + 1) +
                                                                        "_" + sntoa(DendriteLength[num]), true);
 if(!dendrite)
  return true;

 // ���� ���� ������� ������, �� ������ ��������� ��� �����
 if (SynapseStatus[num] == -1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(NumSynapse[num]);
  synapse->DisconnectAll();
 }

 // �������� ���������� �������� � ��������
 dendrite->NumExcitatorySynapses = NumSynapse[num];
// dendrite->GetStorage()->FreeObjectsStorage();
 dendrite->Build();
// dendrite->GetStorage()->FreeObjectsStorage();

 // ���� ��������� ������
 if (SynapseStatus[num] == 1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetExcitatorySynapses(NumSynapse[num] - 1);
  if(!synapse)
   return true;

  // ������ � ���� �������������
  synapse->Resistance = SynapseResistanceStep;

  // ������ �����
  if (!CheckLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input"))
   res &= CreateLink("Source" + sntoa(num + 1), "Output", synapse->GetLongName(GetThisAsSharedContainer()), "Input");

  if(!res)
   return true;
 }

 // Neuron->GetStorage()->FreeObjectsStorage();
 Neuron->Reset();
 // Neuron->GetStorage()->FreeObjectsStorage();

 return true;
}


/// ��������� �������� ���������� �� ������ ��������� ����. ���������� �� ������ �����,
/// ����� ���������� ������������ �������� ���������� � ����� ��� ��������
/// � ������� ������� ��������
bool NNeuronLearner::MeasureMaxPotentialAndTime(void)
{
 // ������� �������� ��������� ����
 double currentsomaamp;

 // ��� ������ ���� ���� ������������ ���������
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // ������� ����
  std::shared_ptr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i+1),true);

  if(!soma)
   return true;

  // ������� ���������
  currentsomaamp = soma->SumPotential(0,0);

  // ��������� ������� �� ��������
  if(currentsomaamp >= MaxIterSomaAmp[i])
  {
   // ���������� ����� �������� ��������� � ��� �������
   MaxIterSomaAmp[i] = currentsomaamp;
   TimeOfMaxIterSomaAmp[i] = Environment.lock()->GetTime().GetDoubleTime();

   // ��� ���������� ������� �������� ���������� �������� ���������
   if(DendriteLength[i] == 1 && NumSynapse[i] && currentsomaamp > InitialSomaPotential[i])
    InitialSomaPotential[i] = currentsomaamp;
  }
 }
 return true;
}


/// ��������� �������� ��������� ��� ��������� ��������� �� ��������� ��������.
/// ���������� � ����� ������ ��������
bool NNeuronLearner::ChangeDendriteStatus(int num)
{
 // ������� ���������������� �������� �� ������������� �������� � �������
 double dt = TimeOfMaxIterSomaAmp[NumInputDendrite - 1] - TimeOfMaxIterSomaAmp[num];

 // �������, ��� ����� ����������, ����:
 // 1. ���������������� �������
 // 2. ��� ���������� ���������� � ������� ��������� ���������������� ������� ����
 // � ����������� �� ������
 // 3. ��� ���������� ���������� � ������� ��������� ��� ����� ������� ������
 if(dt == 0.0)
 {
  DendStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001)
          && ((((dt / fabs(dt)) * (Dissynchronization[num] / fabs(Dissynchronization[num])) < 0)
          && (fabs(dt) <= fabs(Dissynchronization[num]))) || (DendStatus[num] == 0)))
 {
  DendStatus[num] = 0;
 }
 else
 {
  // ������� ������� ������ ������ ��������������, ����� ������� �������
  if(dt > 0)
   DendStatus[num] = 1;
  // ������� ������� ������ ����� ��������������, ����� ����������� �������
  else if(dt < 0)
   DendStatus[num] = -1;
 }

 // ���������� ����������������
 Dissynchronization[num] = dt;

 return true;
}


/// ��������� �������� �������� ��� ��������� ��������� �� ��������� ��������.
/// ���������� � ����� ������ ��������
bool NNeuronLearner::ChangeSynapseStatus(int num)
{
 // ������� �������� ��������� �� ���� � �������
 double dt = InitialSomaPotential[num] - MaxIterSomaAmp[num];

 // ���������� ��� ������������ � ��������� ������� ������� - ����������, �����
 // ������������ ��������������� ����� �������� ��� ������� �������������� ����� ���������
 int dendstatus = DendStatus[num];
 if (ExperimentNum == 2 && !CanChangeDendLength)
  DendStatus[num] = 0;

 // �������, ��� ���������� �������� ����������, ����
 // ��� ���������� ���������� � ������� ��������� � ��� ������� ������� ��������:
 // 1. ��� ����� ������� ������ � �������
 // 2. ������� � ���������� �� ������ 0.000005
 // 3. ������� � ���������� ������� ���� � ����������� �� ������
 if((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
    && !SynapseStatus[num])
 {
  SynapseStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
          && (fabs(dt) <= 0.000005))
 {
  SynapseStatus[num] = 0;
 }
 else if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
          && ((dt / fabs(dt)) * (AmpDifference[num] / fabs(AmpDifference[num])) < 0)
          && (fabs(dt) <= fabs(AmpDifference[num])))
 {
  SynapseStatus[num] = 0;
 }
 else
 {
  // ������� ��������� ������ ��������, ������� ��������� �������
  if(dt > 0.0)
   SynapseStatus[num] = 1;
  // ������� ��������� ������ ��������, ������� ������� �������
  else if(dt < 0.0)
   SynapseStatus[num] = -1;
 }

 // ���������� ����������������
 AmpDifference[num] = dt;

 DendStatus[num] = dendstatus;

 return true;
}

/// �������, ����������� ������������� ������� ��������� �� �������� �����
bool NNeuronLearner::PatternRecognition(void)
{
 // ������ �������� ����� � ��������� ���������
 if(IsFirstFileStep)
 {
  // ���������� � ������ � �������
  Fin.open(Environment.lock()->GetCurrentDataDir()+"input_data.txt");
  Fout.open(Environment.lock()->GetCurrentDataDir()+"output_data"+sntoa(EpochCur)+".txt");
  Inputs.Assign(NumInputDendrite, 1, 0.0);  // ������� ������
  Outputs.assign(NumInputDendrite, 0);  // �������� ������

  // ���������� ���������� � ��������� �������, ������� �������������� �������������
  Fout << "��������� �������, ��� ������� ���������� �������������:" << std::endl;
  for(int i = 0; i < NumInputDendrite; i++)  // ����� ���������
  {
   Fout << DendriteLength[i] << "\t";
  }
  for(int i = 0; i < NumInputDendrite; i++)  // ����� ��������
  {
   Fout << NumSynapse[i] << "\t";
  }
  Fout << std::endl;
  Fout << "���������� �������������:" << std::endl;
  IsFirstFileStep = false;
 }

 // ��������� ������ �������� ������� ��� �������������
 // ����������� ������������� ��� �������� ����� ������� �����. ��� �������
 // ����� ������� ����� ������� ��������� ���������� �������� � ����� ������� �����
 if(IsFirstBeat)
 {
  StartIterTime = Environment.lock()->GetTime().GetDoubleTime();
  IterLength = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep));

  // ���� �������� ����� ����� - ��������� ����� �������������
  if(Fin.eof())
  {
   Fin.close();
   Fout.close();
   IsFirstFileStep = true;
  }

  // (����� ����� �� ��������� - ����������� �������������)
  // ��������� ������� ������ �� �����
  for(int i = 0; i < NumInputDendrite; i++)
  {
   double in;
   Fin >> in;
   Inputs[i] = in;
  }

  // ������������� ������ ��� �������������
  InputPattern = Inputs;
  SetInputPattern(Inputs);
  Outputs[0] = 0;
  IsWritten = false;
  IsFirstBeat = false;
 }

 // ������� ��������� ������������� �������� �������
 std::shared_ptr<NPulseNeuron> neuron = this->GetComponent<NPulseNeuron>(std::string("Neuron"), true);
 if(!neuron)
  return true;
 std::shared_ptr<NLTZone> ltzone = neuron->GetComponent<NLTZone>("LTZone");
 if(!ltzone)
  return true;
 if(ltzone->Output(0,0) > 0.001)
  Outputs[0]= 1;

 // ���������� ��������� ������������� � �������� ����
 double currentitertime = Environment.lock()->GetTime().GetDoubleTime() - StartIterTime;
 if((currentitertime - IterLength >= 0) && !IsWritten)
 {
  Fout << Outputs[0] << std::endl;
  IsWritten = true;
  IsFirstBeat = true;
 }

 return true;
}

/// ���, ����������� �������� ��������������� �������
/// �� ������������� ������� ������ ��������� ��������� ������� �������. ����� � �-�� ��������������� ������� ��������������.
/// ������ ����� �������� �������� ��������� �������: �� ������ �������� ����� ���������� ��������� ����� ��������� � ��������� �
/// ����� ��������, �� ������ 4-� ��������� ���������� ��������� ������ ����� ��������, ����� � ����� ������ �����
/// ����� �������� ��������������� ����� ��������� � ���������, ����� ������������� �������� �� ����� input_data.txt
bool NNeuronLearner::LearningAdditionalPattern_1_4(MDMatrix<double> additional_pattern)
{
 // �������� ��������������� �������
 if (IsNeedToTrain)
 {
  if(IsFirstBeat)
  {
   // ��������� �������� �������� ��� ������ ��������� � �������
   if (CountIteration == 0 && EpochCur == 1)
   {
    InputPattern = additional_pattern;
    SetInputPattern(InputPattern);
   }
   // ����� 5 �������� ��������� � �������������
   if (CountIteration >= 5)
   {
    IsFirstFileStep = true;
    IsNeedToTrain = false;
   }
   // ����� 1 ������� �������� �� ������ ����� ���������
   else if(CountIteration >= 1)
   {
    CanChangeDendLength = false;
   }
  }
 }
 // ������������� �������� �������� �� ����� input_data.txt � ����� �����
 else
 {
  // ������������� ��������� �� �����
  PatternRecognition();

  // ����� ��������� ������������� ��������� �� ����� (�������� IsFirstFileStep
  // - ��������� ������� �����), ���������� ���� ������� � ��������
  // ��������������� �������, ���� ��������� �����������
  if (IsFirstFileStep)
  {
   // ���� ��������� ������� �������������� ������ - ��������� �����������
   if (EndOfLearning())
   {
    ExperimentMode = false;
    SetExperimentMode(false);
   }
   // ����� ���������� �������� ��������������� �������
   else
   {
    IsNeedToTrain = true;
    CountIteration = 0;
    CanChangeDendLength = true;
    EpochCur++;
    PrevInputPattern = additional_pattern;
    InputPattern = additional_pattern;
    SetInputPattern(additional_pattern);
    for (int i = 0; i < NumInputDendrite - 1; i++)
    {
     SynapseStatus[i] = 1;
    }
    return true;
   }
  }
 }

 return true;
}

/// ����������� ����������� ��������:
/// ������ ������� �������������� �������, ��� ����������� ��������� �� ��������� ��������
bool NNeuronLearner::IncrementalLearning(MDMatrix<double> initial_pattern, MDMatrix<double> additional_pattern)
{
 if(!IsFirstBeat)
  return true;

 // ��������� �������� ��� �������� ��� ������ ��������� � �������
 if (CountIteration == 0 && EpochCur == 1)
 {
  InputPattern = additional_pattern;
  SetInputPattern(additional_pattern);
  Fout.open(Environment.lock()->GetCurrentDataDir()+"output_data"+sntoa(EpochCur)+".txt");
 }

 // ����� 5 �������� ������ ������� ������� �� ��������,
 // ����� ��������� ��� ���� ����� �����������
 if (CountIteration == 5)
 {
  // (���� ��� �� ���� �������� MaxIterSomaAmp)
  // ��������� ����� ���������� ����������� ��� ���������� ��������,
  // �� ��� �������� additional_pattern
  for (int k = 0; k < NumInputDendrite; k++)
  {
   SumMaxIterSomaAmp += MaxIterSomaAmp[k];
  }

  InputPattern = initial_pattern;
  SetInputPattern(initial_pattern);
  CountIteration = 5;  // ������������� ��������, �� SetInputPattern() ��� ��������
 }

 // �������� ������� �������������
 else if (CountIteration == 6)
 {
  // ��������� ����� ���������� ����������� ��� ���������� ��������,
  // �� ��� �������� initial_pattern
  double suminitialpattern = 0;
  for (int k = 0; k < NumInputDendrite; k++)
  {
   suminitialpattern += MaxIterSomaAmp[k];
  }
  SumMaxIterSomaAmp += suminitialpattern;

  // ���������� ��������� ��� additional_pattern, initial_pattern � �����
  Fout << "�������� ��������" + sntoa(EpochCur) + ":" << "\t" << SumMaxIterSomaAmp;
  Fout << "\t" << suminitialpattern << "\t" << SumMaxIterSomaAmp << std::endl;

  // ��������� ����� ��������� ��� �������, ���������� � ��� �� �����������
  if (SumMaxIterSomaAmp > PrevSumMaxIterSomaAmp)
  {
   PrevInputPattern = additional_pattern;
  InputPattern = additional_pattern;
   SetInputPattern(InputPattern);
   CanChangeDendLength = true;
   EpochCur++;
   for (int i = 0; i < NumInputDendrite - 1; i++)
   {
    SynapseStatus[i] = 1;
   }
  }
  // ����� ���� - ��������� ���������� ��������� � ����������� ��������
  else
  {
   Fout.close();
  }

 PrevSumMaxIterSomaAmp = SumMaxIterSomaAmp;
  SumMaxIterSomaAmp = 0;
 }

 // ����� 1 �������� �������� �� ������ ����� ���������
 else if(CountIteration >= 1)
 {
  CanChangeDendLength = false;
 }

 return true;
}

/// ������� ��� ���������� ������������� � �������, � �.�. ���������� ������ � �������
/// 1 ����������� - PatternRecognition() - ������������� ������� �� �����
/// 2 ����������� - LearningAdditionalPattern_1_4() - �������� ��������������� �������
/// 3 ����������� - IncrementalLearning() - ������������ ��������
bool NNeuronLearner::Experiment(void)
{
 switch (ExperimentNum) {
 case 1:  // ������������� ������ �� �����
 {
  PatternRecognition();
  // ����� ��������� ������������� ��������� �� ����� (�������� IsFirstFileStep
  // - ��������� ������� �����), ��������� �����������
  if (IsFirstFileStep)
  {
   ExperimentMode = false;
   SetExperimentMode(false);
  }
  break;
 }
 case 2:  // �������� ��������������� �������
 {
  // ������ ������ ���� ������ ���������� �������,
  // ����� ���� ����� �������������� ������ �������
  LearningAdditionalPattern_1_4(AdditionalInputPattern);
  break;
 }
 case 3:  // ������������ �������� ���� ���������� ������������ ��������� ����� ����� ����������
 {
  // ������ ������ ���� ������ ���������� �������,
  // ����� ���� ����� �������������� ������ �������
  IncrementalLearning(InputPattern, AdditionalInputPattern);
  break;
 }
 default:
  break;
 }
 return true;
}


/// ����������� �������� ��������
bool NNeuronLearner::EndOfLearning(void)
{
 // �������� ����������� ������� ����������� ��������
 bool istrained = true;
 for(int i = 0; i < NumInputDendrite; i++)
 {
  // ���� ���� ���� �� ���� ������� ��� ����, ����� ���-� ���� ��������,
  // �� ������ ��� �� ������ �������
  if(DendStatus[i] || SynapseStatus[i])
  {
   istrained = false;
   break;
  }
 }

 // ���� ������ ��� ������ �������, �� ��������� ����������
 if(istrained)
 {
  // �������, �������� ������ ������
  Neuron->TrainingPattern = InputPattern;

  // ����� ���������
  MDMatrix<int> temp;
  temp.Resize(NumInputDendrite, 1);
  for(int i = 0; i < NumInputDendrite; i++)
  {
   temp(i, 0) = DendriteLength[i];
  }
  Neuron->TrainingDendIndexes.Resize(NumInputDendrite, 1);
  Neuron->TrainingDendIndexes = temp;

  // ���������� �������� �� ������� �������� ���������
  for(int i = 0; i < NumInputDendrite; i++)
  {
   temp(i, 0) = NumSynapse[i];
  }
  Neuron->TrainingSynapsisNum.Resize(NumInputDendrite, 1);
  Neuron->TrainingSynapsisNum = temp;

  SetIsNeedToTrain(false);
  IsNeedToTrain = false;

  return true;
 }

 return false;
}


/// ����� �������� ������� NeuronLearner - �������� ������
// ��� ��������� ��������������� ���������� ������� ����� ��������� ���������
// �� ����� �������� ����� ������� ���������� �����������
// ������ ����� ����� ������� �������� ������
bool NNeuronLearner::Training(void)
{
 // ���� 0 ����� �������� � �� 0 ��������, �� ����� ��������� ��������
 if(!CalculateMode && (CountIteration > 0))
 {
  if (EndOfLearning())
   return true;
 }


 // �������� ������ ��� ������� ����� ��������
 if(IsFirstBeat)
 {
  // �������� ����� ������ ������� �������� � ��������� � ������������
  StartIterTime = Environment.lock()->GetTime().GetDoubleTime();
  IterLength = (1.0 / SpikesFrequency) - (1.0 / double(TimeStep));

  // �������� ������������ ��������� ��������� ������� �� ����� ��� ������ ��������
  MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
  TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);

  // �������� ��������� �������
  for(int i = 0; i < NumInputDendrite - 1; i++)
  {
   // �������� ����� �������� �� �������� ��� �������
   if(CanChangeDendLength) // ������ �� ��������� ����� ������������ � LearningSecondPattern
    ChangeDendriteLength(i);
   // �������� ����� �������� �� �������� �������
   ChangeSynapseNumber(i);
  }

  IsFirstBeat = false;

  // �������� ��������� ����
  return true;
 }


 // ��������� ��������� ���������� �� ����, ����� ����� �������� � ��� �����
 MeasureMaxPotentialAndTime();


 // �������� ��� ���������� ��������
 // ������� ������������ �������� - ������� ������ ������� �� ������ ��������
 double currentitertime = Environment.lock()->GetTime().GetDoubleTime() - StartIterTime;

 if(currentitertime - IterLength >= 0)
 {
  // ������ ������� ���� ��������� � ��������, ����� �������������
  for(int i = 0; i < NumInputDendrite; i++)
  {
   ChangeDendriteStatus(i);
   ChangeSynapseStatus(i);
   // ���������� ������� ������� ����� ������� ��������� ��������
   PrevInputPattern[i] = InputPattern[i];
  }

  // ��������� ��� ��������� �������� ���� ������� �����
  IsFirstBeat = true;
  // ����������� ������� ��������.
  CountIteration++;
 }

 return true;
}


/// ��������� ������ ����� �������
// ��� ��������� ��������������� ���������� ������� ����� ��������� ���������
// �� ����� �������� ����� ������� ���������� �����������
// ������ ����� ����� ������� �������� ������
bool NNeuronLearner::ACalculate(void)
{
 // ���� ��� �������, �� ������ ������ �� ����
 if(!Neuron)
  return true;


 // ��������� �������� ����������� �� ��������� � �����
 DendriteNeuronAmplitude(0, 0) = 0;
 for(int i = 0; i < NumInputDendrite; i++)
 {
     std::shared_ptr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>("Dendrite"+sntoa(i + 1)+"_1",true);
     if(!dendrite)
      return true;
     DendriteNeuronAmplitude(i + 1, 0) = dendrite->SumPotential(0, 0);
     DendriteNeuronAmplitude(0, 0) += dendrite->SumPotential(0, 0);
 }

 SomaNeuronAmplitude(0, 0) = 0;
 for(int i = 0; i < NumInputDendrite; i++)
 {
     std::shared_ptr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>("Soma"+sntoa(i + 1),true);
     if(!soma)
      return true;
     SomaNeuronAmplitude(i + 1, 0) = soma->SumPotential(0, 0);
     SomaNeuronAmplitude(0, 0) += soma->SumPotential(0, 0);
 }


 // ���� 0 ����� �������� � �� 0 ��������, �� ����� ��������� ��������
 if(!CalculateMode && (CountIteration > 0))
 {
  // ��������� ����������� ������� ����������� ��������
  bool istrained = true;
  for(int i = 0; i < NumInputDendrite; i++)
  {
   // ���� ���� ���� �� ���� ������� ��� ����, ����� ���-� ���� ��������,
   // �� ������ ��� �� ������ �������
   if(DendStatus[i] || SynapseStatus[i])
   {
    istrained = false;
    break;
   }
  }

  // ���� ������ ��� ������ �������, �� ��������� ����������
  if(istrained)
  {
   // �������, �������� ������ ������
   Neuron->TrainingPattern = InputPattern;

   // ����� ���������
   MDMatrix<int> temp;
   temp.Resize(NumInputDendrite, 1);
   for(int i = 0; i < NumInputDendrite; i++)
   {
    temp(i, 0) = DendriteLength[i];
   }
   Neuron->TrainingDendIndexes.Resize(NumInputDendrite, 1);
   Neuron->TrainingDendIndexes = temp;

   // ���������� �������� �� ������� �������� ���������
   for(int i = 0; i < NumInputDendrite; i++)
   {
    temp(i, 0) = NumSynapse[i];
   }
   Neuron->TrainingSynapsisNum.Resize(NumInputDendrite, 1);
   Neuron->TrainingSynapsisNum = temp;

   SetIsNeedToTrain(false);
   IsNeedToTrain = false;

   // �������� ��������� ���� ����
   return true;
  }
 }


 // ������� ��� ������ � �������.
 // ������������ ������ ������� ������ �� �����,
 // ��������� ����������� � ������ ����������� � ����
 if(ExperimentMode)
     Experiment();

 // � ������� ���������� �������� �������
 if (IsNeedToTrain)
  Training();


 // ����� ���������� � ������ ������� �� ����� NNeuronLearner
 if(Neuron)
  Output = Neuron->Output;

 return true;
}

// --------------------------

}

#endif
