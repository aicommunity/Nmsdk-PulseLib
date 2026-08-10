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

#ifndef NNEURONTIMELEARNER_CPP
#define NNEURONTIMELEARNER_CPP

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>
#include <cmath>
#include <sstream>
#include "NNeuronTimeLearner.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Rdk/Deploy/Include/rdk_cpp_init.h"


namespace NMSDK {

namespace {
std::string MakeLearnerSomaName(const int index1)
{
 return std::string("Soma") + sntoa(index1);
}

std::string MakeLearnerDendriteName(const int dendrite_index1, const int segment_index1)
{
 return std::string("Dendrite") + sntoa(dendrite_index1) + std::string("_") + sntoa(segment_index1);
}

std::string MakeLearnerExcSynapsePath(const int dendrite_index1, const int segment_index1, const int synapse_index1)
{
 return MakeLearnerDendriteName(dendrite_index1, segment_index1) + std::string(".ExcSynapse") + sntoa(synapse_index1);
}

const int kPhaseSync = 0;
const int kPhaseNormalize = 1;
const int kPhaseDone = 2;
}

std::string NNeuronTimeLearner::DatasetGeneratorPath(void)
{
 return std::string("DatasetMatrix.Generator1");
}

NPulseGeneratorTransit* NNeuronTimeLearner::GetDatasetGenerator(void)
{
 if(!Dataset)
  return 0;
 if(!Dataset->Generators.empty() && Dataset->Generators[0])
  return Dataset->Generators[0];
 return Dataset->GetComponentL<NPulseGeneratorTransit>("Generator1", true).Get();
}

bool NNeuronTimeLearner::LinkSynapseToDataset(NPulseSynapseCommon *synapse)
{
 if(!synapse)
  return false;
 const std::string gen_path = DatasetGeneratorPath();
 if(!CheckLink(gen_path, "Output", synapse->GetLongName(this), "Input"))
  return CreateLink(gen_path, "Output", synapse->GetLongName(this), "Input");
 return true;
}

bool NNeuronTimeLearner::RelinkDendriteSynapsesToDataset(int dendrite_index0)
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return true;
 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
  MakeLearnerDendriteName(dendrite_index0 + 1, DendriteLength[dendrite_index0]), true);
 if(!dendrite)
  return true;
 for(int k = 0; k < NumSynapse[dendrite_index0]; k++)
 {
  NPulseSynapseCommon *synapse = dendrite->GetComponentL<NPulseSynapseCommon>(
   std::string("ExcSynapse") + sntoa(k + 1), true);
  if(!synapse)
   continue;
  if(k != 0)
   synapse->Resistance = SynapseResistanceStep;
  if(!LinkSynapseToDataset(synapse))
   return false;
 }
 return true;
}

void NNeuronTimeLearner::SyncDatasetDimsFromDendrites(void)
{
 if(!Dataset)
  return;
 Dataset->PulseGeneratorClassName = PulseGeneratorClassName;
 if(Dataset->NumFeatures != 1)
  Dataset->SetNumFeatures(1);
 if(Dataset->NumSamples != 1)
  Dataset->SetNumSamples(1);
 if(Dataset->MaxSpikesPerFeature != NumInputDendrite)
  Dataset->SetMaxSpikesPerFeature(NumInputDendrite);
 Dataset->Delay = Delay;
 Dataset->AdvanceSampleAfterBurst = false;
 Dataset->StateGeneration = 2;
}


NNeuronTimeLearner::NNeuronTimeLearner(void):
 StructureBuildMode("StructureBuildMode",this,&NNeuronTimeLearner::SetStructureBuildMode),
 PulseGeneratorClassName("PulseGeneratorClassName",this,&NNeuronTimeLearner::SetPulseGeneratorClassName),
 NeuronClassName("NeuronClassName",this,&NNeuronTimeLearner::SetNeuronClassName),
 SynapseClassName("SynapseClassName",this,&NNeuronTimeLearner::SetSynapseClassName),
 CalculateMode("CalculateMode",this, &NNeuronTimeLearner::SetCalculateMode),
 IsNeedToTrain("IsNeedToTrain",this,&NNeuronTimeLearner::SetIsNeedToTrain),
 Delay("Delay",this,&NNeuronTimeLearner::SetDelay),
 SpikesFrequency("SpikesFrequency",this,&NNeuronTimeLearner::SetSpikesFrequency),
 NumInputDendrite("NumInputDendrite",this,&NNeuronTimeLearner::SetNumInputDendrite),
 MaxDendriteLength("MaxDendriteLength",this,&NNeuronTimeLearner::SetMaxDendriteLength),
 InputPattern("InputPattern",this,&NNeuronTimeLearner::SetInputPattern),
 AdditionalInputPattern("AdditionalInputPattern",this,&NNeuronTimeLearner::SetAdditionalInputPattern),
 DendriteNeuronAmplitude("DendriteNeuronAmplitude",this),
 SomaNeuronAmplitude("SomaNeuronAmplitude",this),
 LTZThreshold("LTZThreshold",this,&NNeuronTimeLearner::SetLTZThreshold),
 FixedLTZThreshold("FixedLTZThreshold",this,&NNeuronTimeLearner::SetFixedLTZThreshold),
 TrainingLTZThreshold("TrainingLTZThreshold",this,&NNeuronTimeLearner::SetTrainingLTZThreshold),
 UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NNeuronTimeLearner::SetUseFixedLTZThreshold),
 Output("Output",this),
 SynapseResistanceStep("SynapseResistanceStep", this, &NNeuronTimeLearner::SetSynapseResistanceStep),
 DendriteLength("DendriteLength", this, &NNeuronTimeLearner::SetDendriteLength),
 InitialSomaPotential("InitialSomaPotential", this, &NNeuronTimeLearner::SetInitialSomaPotential),
 NumSynapse("NumSynapse", this, &NNeuronTimeLearner::SetNumSynapse),
 IterationGap("IterationGap", this, &NNeuronTimeLearner::SetIterationGap),
 SyncTolerance("SyncTolerance", this, &NNeuronTimeLearner::SetSyncTolerance),
 TrainingPhase("TrainingPhase", this),
 ExperimentNum("ExperimentNum", this, &NNeuronTimeLearner::SetExperimentNum),
 ExperimentMode("ExperimentMode", this, &NNeuronTimeLearner::SetExperimentMode),
 EnableDebug("EnableDebug", this, &NNeuronTimeLearner::SetEnableDebug)
{
 OldNumInputDendrite = 0;
 Dataset = NULL;
 Neuron = NULL;
 IsFirstFileStep = true;
 FirstImpulseTime = 0.0;
 PrevFirstImpulseTime = -1.0;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 IterationActive = false;
 WaitingPeakAfterLastPulse = false;
 LastPulseTime = 0.0;
}


NNeuronTimeLearner::~NNeuronTimeLearner(void)
{
 OldNumInputDendrite = 0;
}

bool NNeuronTimeLearner::SetStructureBuildMode(const int &value)
{
 if(value > 0)
  Ready = false;
 return true;
}

bool NNeuronTimeLearner::SetPulseGeneratorClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearner::SetNeuronClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearner::SetSynapseClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearner::ZeroingTrainingPattern(void)
{
 MDMatrix<double> zero;
 zero.Assign(NumInputDendrite, 1, 0.0);
 if (Neuron)
  Neuron->TrainingPattern = zero;

 MDMatrix<int> temp;
 temp.Assign(NumInputDendrite, 1, 1);
 if (Neuron)
 {
  Neuron->TrainingDendIndexes = temp;
  Neuron->TrainingSynapsisNum = temp;
 }
 return true;
}

bool NNeuronTimeLearner::SetLTZThreshold(const double &value)
{
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
 {
  UseFixedLTZThreshold = false;
 }
 return true;
}

bool NNeuronTimeLearner::SetIsNeedToTrain(const bool &value)
{
 if(CalculateMode == 1 && !value)
 {
  ZeroingTrainingPattern();
 }

 CountIteration = 0;
 IsFirstBeat = true;
 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 if(value)
 {
  TrainingPhase = kPhaseSync;
  SetLTZThreshold(TrainingLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(TrainingLTZThreshold.GetData());
 }
 else
 {
  TrainingPhase = kPhaseDone;
  SetLTZThreshold(FixedLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(FixedLTZThreshold.GetData());
 }
 return true;
}

bool NNeuronTimeLearner::SetCalculateMode(const int &value)
{
 if((value != 0) && (value != 1))
  return false;
 CountIteration = 0;
 IsFirstBeat = true;
 return true;
}

bool NNeuronTimeLearner::SetDelay(const double &value)
{
 if(Dataset)
  Dataset->Delay = value;
 return true;
}

bool NNeuronTimeLearner::SetSpikesFrequency(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearner::SetNumInputDendrite(const int &value)
{
 if(value < 1)
  return false;
 Ready = false;
 OldNumInputDendrite = NumInputDendrite;
 return true;
}

bool NNeuronTimeLearner::SetMaxDendriteLength(const int &value)
{
 if(value < 1)
  return false;

 for (int i = 0; i < NumInputDendrite; i++)
 {
  if (DendriteLength.empty())
   return true;

  if (DendriteLength[i] > value)
  {
   DendriteLength[i] = value;

   if (!Neuron)
    return true;

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
   Neuron->Reset();

   UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
    MakeLearnerDendriteName(i + 1, DendriteLength[i]), true);
   if(!dendrite)
    return true;

   dendrite->NumExcitatorySynapses = NumSynapse[i];
   dendrite->Build();
   RelinkDendriteSynapsesToDataset(i);
   Neuron->Reset();
  }
 }
 return true;
}

bool NNeuronTimeLearner::SetInputPattern(const MDMatrix<double> &value)
{
 if(Dataset)
 {
  MDMatrix<double> matrix = value;
  const int n = NumInputDendrite;
  if(matrix.GetRows() != n || matrix.GetCols() != 1)
   matrix.Resize(n, 1, 0.0);
  Dataset->SetMatrixData(matrix);
 }
 IsFirstBeat = true;
 CountIteration = 0;
 IterationActive = false;
 HasPrevIteration = false;
 return true;
}

bool NNeuronTimeLearner::SetAdditionalInputPattern(const MDMatrix<double> &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearner::SetPrevInputPattern(const MDMatrix<double> &value)
{
 PrevInputPattern = value;
 return true;
}

bool NNeuronTimeLearner::SetTrainingLTZThreshold(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearner::SetFixedLTZThreshold(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearner::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  SetLTZThreshold(FixedLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(FixedLTZThreshold.GetData());
 }
 return true;
}

bool NNeuronTimeLearner::SetSynapseResistanceStep(const double &value)
{
 if (NumSynapse.empty() || !Neuron)
  return true;

 for(int numdend = 1; numdend <= NumInputDendrite; numdend++)
 {
  for(int numsyn = 2; numsyn <= NumSynapse[numdend - 1]; numsyn++)
  {
   UEPtr<NPulseSynapse> synapse = Neuron->GetComponentL<NPulseSynapse>(
    MakeLearnerExcSynapsePath(numdend, DendriteLength[numdend - 1], numsyn), true);
   if(!synapse)
    continue;
   synapse->Resistance = value;
  }
 }
 return true;
}

bool NNeuronTimeLearner::SetExperimentMode(const bool &value)
{
 if(!value)
 {
  Fin.close();
  Fout.close();
  CanChangeDendLength = true;
 }
 else
 {
  ExperimentNum.SetDataDirect(1);
  EpochCur = 1;
 }
 IsFirstFileStep = true;
 return true;
}

bool NNeuronTimeLearner::SetDendriteLength(const std::vector<int> &value)
{
 (void)value;
 OldDendriteLength = DendriteLength;
 if (DendriteLength.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (DendriteLength.size() < static_cast<size_t>(NumInputDendrite))
   DendriteLength.resize(NumInputDendrite, 1);
  else
   DendriteLength.resize(NumInputDendrite);
 }
 Ready = false;
 return true;
}

bool NNeuronTimeLearner::SetInitialSomaPotential(const std::vector<double> &value)
{
 if (InitialSomaPotential.size() != static_cast<size_t>(NumInputDendrite))
 {
  std::vector<double> normalized(NumInputDendrite, 0.0);
  const size_t copyCount = std::min(normalized.size(), value.size());
  for (size_t i = 0; i < copyCount; ++i)
   normalized[i] = value[i];
  InitialSomaPotential.SetDataDirect(normalized);
 }
 return true;
}

bool NNeuronTimeLearner::SetNumSynapse(const std::vector<int> &value)
{
 (void)value;
 if (NumSynapse.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (NumSynapse.size() < static_cast<size_t>(NumInputDendrite))
   NumSynapse.resize(NumInputDendrite, 1);
  else
   NumSynapse.resize(NumInputDendrite);
 }
 Ready = false;
 return true;
}

bool NNeuronTimeLearner::SetIterationGap(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetSyncTolerance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetExperimentNum(const int &value)
{
 (void)value;
 CalculateMode.SetDataDirect(1);
 SetCalculateMode(1);
 IsFirstFileStep = true;
 return true;
}

bool NNeuronTimeLearner::SetEnableDebug(const bool & /*value*/)
{
 return true;
}

NNeuronTimeLearner* NNeuronTimeLearner::New(void)
{
 return new NNeuronTimeLearner;
}

UComponent* NNeuronTimeLearner::NewStatic(void)
{
 return new NNeuronTimeLearner;
}

bool NNeuronTimeLearner::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 (void)comp;
 (void)pointer;
 return true;
}

bool NNeuronTimeLearner::ADelComponent(UEPtr<UContainer> comp)
{
 (void)comp;
 return true;
}

bool NNeuronTimeLearner::BuildStructure()
{
 bool res(true);
 try
 {
 Neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
 Neuron->SetCoord(MVector<double,3>(8.7 + 1 * 7, 1.67, 0));
 Neuron->NumSomaMembraneParts = NumInputDendrite;

 if (DendriteLength.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (DendriteLength.size() < static_cast<size_t>(NumInputDendrite))
   DendriteLength.resize(NumInputDendrite, 1);
  else
   DendriteLength.resize(NumInputDendrite);
 }

 if (OldDendriteLength.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (OldDendriteLength.size() < static_cast<size_t>(NumInputDendrite))
   OldDendriteLength.resize(NumInputDendrite, 1);
  else
   OldDendriteLength.resize(NumInputDendrite);
 }

 if(Neuron->StructureBuildMode != 2)
 {
  Neuron->NumDendriteMembraneParts = 1;
 }
 else
 {
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
 }
 Neuron->Reset();

 // Remove legacy per-dendrite Sources from copied Learner layouts
 for(int i = 0; i < std::max(OldNumInputDendrite, NumInputDendrite.GetData()) + 4; i++)
 {
  DelComponent(std::string("Source") + sntoa(i + 1));
 }

 Dataset = AddMissingComponent<NDatasetMatrix>(std::string("DatasetMatrix"), "NDatasetMatrix");
 Dataset->SetCoord(MVector<double,3>(6.7, 1.67, 0));
 SyncDatasetDimsFromDendrites();
 if(InputPattern.GetRows() != NumInputDendrite || InputPattern.GetCols() != 1)
  InputPattern.Resize(NumInputDendrite, 1, 0.0);
 Dataset->SetMatrixData(InputPattern);
 Dataset->Build();
 Dataset->Reset();

 if (NumInputDendrite != int(NumSynapse.size()))
  NumSynapse.resize(NumInputDendrite, 1);

 for (int i = 0; i < NumInputDendrite; i++)
 {
  if ((i < int(OldDendriteLength.size())) && (i < int(DendriteLength.size())) && (OldDendriteLength[i] < DendriteLength[i]))
  {
   UEPtr<NPulseMembrane> previnputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>(
    MakeLearnerDendriteName(i + 1, OldDendriteLength[i]), true);
   if(previnputsegmentofdendrite)
   {
    previnputsegmentofdendrite->NumExcitatorySynapses = 1;
    previnputsegmentofdendrite->Build();
   }
  }

  UEPtr<NPulseMembrane> inputsegmentofdendrite = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(i + 1, DendriteLength[i]), true);
  if(!inputsegmentofdendrite)
   continue;

  inputsegmentofdendrite->NumExcitatorySynapses = NumSynapse[i];
  inputsegmentofdendrite->Build();
 }

 Neuron->Reset();

 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(gen)
  gen->DisconnectAll("Output");

 for(int numdend = 0; numdend < NumInputDendrite; numdend++)
 {
  if(!RelinkDendriteSynapsesToDataset(numdend))
  {
   res = false;
   continue;
  }
 }

 Neuron->Reset();

 IsFirstBeat = true;
 DendriteNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 SomaNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);

 {
  std::vector<double> cur = InitialSomaPotential;
  if (cur.size() != static_cast<size_t>(NumInputDendrite))
  {
   cur.resize(NumInputDendrite, 0.0);
   InitialSomaPotential.SetDataDirect(cur);
  }
 }

 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 double period = IterationGap > 0.0 ? IterationGap.GetData() : 0.5;
 Dissynchronization.assign(NumInputDendrite, period);
 Dissynchronization[NumInputDendrite - 1] = 0.0;
 AmpDifference.assign(NumInputDendrite, 0.0);

 InputPattern.Resize(NumInputDendrite, 1, 0.0);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);
 AdditionalInputPattern.Resize(NumInputDendrite, 1, 0.0);

 TrainingPhase = IsNeedToTrain ? kPhaseSync : kPhaseDone;
 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 if(gen)
  PrevPulseCounter = gen->PulseCounter;

 OldNumInputDendrite = NumInputDendrite;
 return res;
 }
 catch (const UException &ex)
 {
  try
  {
   if (RDK::GetLogger())
   {
    std::ostringstream oss;
    oss << "NNeuronTimeLearner::BuildStructure: UException number=" << ex.GetNumber()
        << ", type=" << ex.GetType()
        << ", file=" << ex.GetExFileName()
        << ", line=" << ex.GetExLineNumber()
        << ", what=" << ex.what();
    RDK::GetLogger()->LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner", oss.str(), ex.GetNumber());
   }
  }
  catch (...) {}
  throw;
 }
 catch (const std::exception &ex)
 {
  try
  {
   if (RDK::GetLogger())
   {
    std::ostringstream oss;
    oss << "NNeuronTimeLearner::BuildStructure: std::exception what=" << ex.what();
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, oss.str());
   }
  }
  catch (...) {}
  throw;
 }
 catch (...)
 {
  try
  {
   if (RDK::GetLogger())
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, "NNeuronTimeLearner::BuildStructure: unknown exception");
  }
  catch (...) {}
  throw;
 }
}


bool NNeuronTimeLearner::ADefault(void)
{
 Dataset = NULL;
 StructureBuildMode = 1;
 CalculateMode = 0;
 PulseGeneratorClassName = "NPulseGeneratorTransit";
 NeuronClassName = "NSPNeuronGen";
 SynapseClassName = "NPSynapseBio";
 IsNeedToTrain = true;
 ExperimentMode = false;
 Delay = 0.5;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 OldNumInputDendrite = 0;
 MaxDendriteLength = 100;
 IterationGap = 0.5;
 SyncTolerance = 1.0e-6;
 TrainingPhase = kPhaseSync;

 LTZThreshold = 100;
 FixedLTZThreshold = 0.0115;
 TrainingLTZThreshold = 100;
 UseFixedLTZThreshold = false;

 InputPattern.Resize(NumInputDendrite, 1);
 AdditionalInputPattern.Resize(NumInputDendrite, 1);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);

 DendriteNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 SomaNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 Output.Assign(NumInputDendrite, 1, 0.0);

 SynapseResistanceStep = 1.0e9;
 DendriteLength.assign(NumInputDendrite, 1);
 OldDendriteLength.assign(NumInputDendrite, 1);
 NumSynapse.assign(NumInputDendrite, 1);

 CountIteration = 0;
 ExperimentNum = 0;
 EpochCur = 0;
 CanChangeDendLength = true;
 IsWritten = false;
 IsFirstBeat = true;
 InitialSomaPotential.assign(NumInputDendrite, 0.0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 Dissynchronization.assign(NumInputDendrite, IterationGap + 0.001);
 AmpDifference.assign(NumInputDendrite, 0.0);
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);
 EnableDebug = false;

 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 FirstImpulseTime = 0.0;
 PrevFirstImpulseTime = -1.0;
 LastPulseTime = 0.0;
 return true;
}


bool NNeuronTimeLearner::ABuild(void)
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

void NNeuronTimeLearner::UpdateComputationOrder(void)
{
 int position = 0;
 if(Dataset)
  SetComponentPosition(Dataset->GetName(), position++);
 if(Neuron)
  SetComponentPosition(Neuron->GetName(), position++);
}

bool NNeuronTimeLearner::AReset(void)
{
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(n_in)
 {
  UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
  if(ltzone)
   ltzone->Threshold = LTZThreshold;
 }

 if(Dataset)
 {
  SyncDatasetDimsFromDendrites();
  Dataset->Reset();
 }

 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(gen)
  PrevPulseCounter = gen->PulseCounter;
 PrevGenOutput = 0.0;
 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 IsFirstBeat = true;
 return true;
}

bool NNeuronTimeLearner::CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e)
{
 bool res(true);
 for(int i = 0; i < NumInputDendrite; i++)
 {
  if(fabs(prev_input_pattern[i] - input_pattern[i]) > e)
  {
   res = false;
   break;
  }
 }
 return res;
}


bool NNeuronTimeLearner::ChangeDendriteLength(int num)
{
 bool res(true);

 if(!DendStatus[num])
  return true;

 if((DendStatus[num] == -1) && (DendriteLength[num] < 2))
 {
  DendStatus[num] = 0;
  return true;
 }

 if(DendStatus[num] == 1)
 {
  if(DendriteLength[num] >= MaxDendriteLength)
  {
   DendStatus[num] = 0;
   return true;
  }
 }

 OldDendriteLength[num] = DendriteLength[num];
 DendriteLength[num] += DendStatus[num];

 if(Neuron->StructureBuildMode != 2)
 {
  if(Neuron->NumDendriteMembraneParts != DendriteLength[num])
  {
   Neuron->NumDendriteMembraneParts = DendriteLength[num];
   Neuron->Reset();
  }
 }
 else
 {
  Neuron->NumDendriteMembranePartsVec = DendriteLength;
  Neuron->Reset();
 }

 if((DendStatus[num] == 1) && (DendriteLength[num] > 1))
 {
  UEPtr<NPulseMembrane> prevdendrite = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(num + 1, DendriteLength[num] - 1), true);
  if(!prevdendrite)
   return true;
  prevdendrite->NumExcitatorySynapses = 1;
  prevdendrite->Build();
 }

 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
  MakeLearnerDendriteName(num + 1, DendriteLength[num]), true);
 if(!dendrite)
  return true;
 dendrite->NumExcitatorySynapses = NumSynapse[num];
 dendrite->Build();

 res &= RelinkDendriteSynapsesToDataset(num);
 Neuron->Reset();
 return res;
}


bool NNeuronTimeLearner::ChangeSynapseNumber(int num)
{
 bool res(true);

 if(!SynapseStatus[num])
  return true;

 if((SynapseStatus[num] == -1) && (NumSynapse[num] < 2))
 {
  SynapseStatus[num] = 0;
  return true;
 }

 NumSynapse[num] += SynapseStatus[num];

 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
  MakeLearnerDendriteName(num + 1, DendriteLength[num]), true);
 if(!dendrite)
  return true;

 if (SynapseStatus[num] == -1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetComponentL<NPulseSynapseCommon>(
   std::string("ExcSynapse") + sntoa(NumSynapse[num] + 1), true);
  if(synapse)
   synapse->DisconnectAll();
 }

 dendrite->NumExcitatorySynapses = NumSynapse[num];
 dendrite->Build();

 if (SynapseStatus[num] == 1)
 {
  NPulseSynapseCommon *synapse = dendrite->GetComponentL<NPulseSynapseCommon>(
   std::string("ExcSynapse") + sntoa(NumSynapse[num]), true);
  if(!synapse)
   return true;
  synapse->Resistance = SynapseResistanceStep;
  res &= LinkSynapseToDataset(synapse);
  if(!res)
   return true;
 }

 Neuron->Reset();
 return true;
}


bool NNeuronTimeLearner::MeasureMaxPotentialAndTime(void)
{
 if(ActiveMeasureSoma < 0 || ActiveMeasureSoma >= NumInputDendrite)
  return true;

 const int i = ActiveMeasureSoma;
 UEPtr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>(MakeLearnerSomaName(i + 1), true);
 if(!soma)
  return true;

 const double currentsomaamp = soma->SumPotential(0,0);
 if(currentsomaamp >= MaxIterSomaAmp[i])
 {
  MaxIterSomaAmp[i] = currentsomaamp;
  TimeOfMaxIterSomaAmp[i] = Environment->GetTime().GetDoubleTime();

  if(DendriteLength[i] == 1 && NumSynapse[i] && currentsomaamp > InitialSomaPotential[i])
   InitialSomaPotential[i] = currentsomaamp;
 }
 return true;
}


bool NNeuronTimeLearner::ChangeDendriteStatus(int num)
{
 if(num == NumInputDendrite - 1)
 {
  DendStatus[num] = 0;
  Dissynchronization[num] = 0.0;
  return true;
 }

 double dt = TimeOfMaxIterSomaAmp[NumInputDendrite - 1] - TimeOfMaxIterSomaAmp[num];

 if(fabs(dt) <= SyncTolerance)
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
  if(dt > 0)
   DendStatus[num] = 1;
  else if(dt < 0)
   DendStatus[num] = -1;
 }

 Dissynchronization[num] = dt;
 return true;
}


bool NNeuronTimeLearner::ChangeSynapseStatus(int num)
{
 double dt = InitialSomaPotential[num] - MaxIterSomaAmp[num];
 int dendstatus = DendStatus[num];
 if (TrainingPhase == kPhaseNormalize)
  DendStatus[num] = 0;

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
  if(dt > 0.0)
   SynapseStatus[num] = 1;
  else if(dt < 0.0)
   SynapseStatus[num] = -1;
 }

 AmpDifference[num] = dt;
 DendStatus[num] = dendstatus;
 return true;
}

bool NNeuronTimeLearner::PatternRecognition(void)
{
 return true;
}

bool NNeuronTimeLearner::LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern)
{
 (void)second_pattern;
 return true;
}

bool NNeuronTimeLearner::IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern)
{
 (void)InitialPattern;
 (void)second_pattern;
 return true;
}

bool NNeuronTimeLearner::Experiment(void)
{
 return true;
}

bool NNeuronTimeLearner::AllDendritesSynced(void) const
{
 for(int i = 0; i < NumInputDendrite - 1; i++)
 {
  if(DendStatus[i])
   return false;
 }
 return true;
}

bool NNeuronTimeLearner::AllSynapsesNormalized(void) const
{
 for(int i = 0; i < NumInputDendrite; i++)
 {
  if(SynapseStatus[i])
   return false;
 }
 return true;
}

bool NNeuronTimeLearner::EndOfLearning(void)
{
 if(TrainingPhase == kPhaseSync)
 {
  if(!AllDendritesSynced())
   return false;
  TrainingPhase = kPhaseNormalize;
  // Force at least one normalize iteration to measure amplitudes
  SynapseStatus.assign(NumInputDendrite, 1);
  CanChangeDendLength = false;
  if (EnableDebug.GetData() && RDK::GetLogger())
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", "phase -> Normalize");
  return false;
 }

 if(TrainingPhase != kPhaseNormalize)
  return false;

 if(!AllSynapsesNormalized())
  return false;

 Neuron->TrainingPattern = InputPattern;

 MDMatrix<int> temp;
 temp.Resize(NumInputDendrite, 1);
 for(int i = 0; i < NumInputDendrite; i++)
  temp(i, 0) = DendriteLength[i];
 Neuron->TrainingDendIndexes.Resize(NumInputDendrite, 1);
 Neuron->TrainingDendIndexes = temp;

 for(int i = 0; i < NumInputDendrite; i++)
  temp(i, 0) = NumSynapse[i];
 Neuron->TrainingSynapsisNum.Resize(NumInputDendrite, 1);
 Neuron->TrainingSynapsisNum = temp;

 TrainingPhase = kPhaseDone;
 SetIsNeedToTrain(false);
 IsNeedToTrain = false;
 return true;
}


bool NNeuronTimeLearner::DetectNewImpulse(void)
{
 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(!gen)
  return false;

 const int counter = gen->PulseCounter;
 double out = 0.0;
 if(gen->Output.GetRows() > 0 && gen->Output.GetCols() > 0)
  out = gen->Output(0, 0);

 bool rising = false;
 if(counter > PrevPulseCounter)
 {
  rising = true;
  PrevPulseCounter = counter;
 }
 else if(out > 0.0 && PrevGenOutput <= 0.0)
 {
  rising = true;
 }
 PrevGenOutput = out;
 return rising;
}

void NNeuronTimeLearner::BeginTrainingIteration(double now)
{
 StartIterTime = now;
 FirstImpulseTime = now;
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 PulseIndexInIter = 0;
 ActiveMeasureSoma = 0;
 WaitingPeakAfterLastPulse = false;
 IterationActive = true;
 IsFirstBeat = false;

 for(int i = 0; i < NumInputDendrite - 1; i++)
 {
  if(TrainingPhase == kPhaseSync && CanChangeDendLength)
   ChangeDendriteLength(i);
  if(TrainingPhase == kPhaseNormalize)
   ChangeSynapseNumber(i);
 }
 // Last dendrite also updates synapses in normalize phase
 if(TrainingPhase == kPhaseNormalize)
  ChangeSynapseNumber(NumInputDendrite - 1);

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BeginTrainingIteration: iter=" << CountIteration
      << " phase=" << TrainingPhase.GetData()
      << " t=" << now;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
}

void NNeuronTimeLearner::FinishTrainingIteration(void)
{
 if(TrainingPhase == kPhaseSync)
 {
  for(int i = 0; i < NumInputDendrite; i++)
   ChangeDendriteStatus(i);
 }
 else if(TrainingPhase == kPhaseNormalize)
 {
  for(int i = 0; i < NumInputDendrite; i++)
   ChangeSynapseStatus(i);
 }

 for(int i = 0; i < NumInputDendrite; i++)
  PrevInputPattern[i] = InputPattern[i];

 PrevFirstImpulseTime = FirstImpulseTime;
 HasPrevIteration = true;
 IterationActive = false;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 IsFirstBeat = true;
 CountIteration++;

 if(!CalculateMode)
  EndOfLearning();
}


bool NNeuronTimeLearner::Training(void)
{
 if(!CalculateMode && (CountIteration > 0) && TrainingPhase == kPhaseDone)
  return true;

 if(!Neuron || !Dataset)
  return true;

 const double now = Environment->GetTime().GetDoubleTime();
 const bool impulse = DetectNewImpulse();

 if(!IterationActive)
 {
  bool can_start = !HasPrevIteration;
  if(HasPrevIteration)
   can_start = (now - PrevFirstImpulseTime) >= IterationGap;

  if(impulse && can_start)
  {
   BeginTrainingIteration(now);
   // First impulse already assigned to soma 0
   MeasureMaxPotentialAndTime();
  }
  return true;
 }

 // Active iteration
 if(impulse && PulseIndexInIter < NumInputDendrite - 1)
 {
  PulseIndexInIter++;
  ActiveMeasureSoma = PulseIndexInIter;
  LastPulseTime = now;
  if(PulseIndexInIter == NumInputDendrite - 1)
   WaitingPeakAfterLastPulse = true;
 }
 else if(impulse && PulseIndexInIter >= NumInputDendrite - 1)
 {
  // Extra impulse within iteration: ignore for mapping; may end after gap logic
  LastPulseTime = now;
 }

 MeasureMaxPotentialAndTime();

 // End after last pulse had enough time for peak, or when IterationGap from first impulse elapsed
 const double after_last_window = std::max(0.01, IterationGap.GetData() * 0.25);
 bool finished = false;
 if(WaitingPeakAfterLastPulse && (now - LastPulseTime) >= after_last_window)
  finished = true;
 if(PulseIndexInIter >= NumInputDendrite - 1 && (now - FirstImpulseTime) >= IterationGap)
  finished = true;

 if(finished)
  FinishTrainingIteration();

 return true;
}


bool NNeuronTimeLearner::ACalculate(void)
{
 try
 {
  if(!Neuron)
   return true;

  DendriteNeuronAmplitude(0, 0) = 0;
  for(int i = 0; i < NumInputDendrite; i++)
  {
   UEPtr<NPulseMembrane> dendrite =
    Neuron->GetComponentL<NPulseMembrane>(MakeLearnerDendriteName(i + 1, 1), true);
   if(!dendrite)
    return true;

   DendriteNeuronAmplitude(i + 1, 0) = dendrite->SumPotential(0, 0);
   DendriteNeuronAmplitude(0, 0) += dendrite->SumPotential(0, 0);
  }

  SomaNeuronAmplitude(0, 0) = 0;
  for(int i = 0; i < NumInputDendrite; i++)
  {
   UEPtr<NPulseMembrane> soma =
    Neuron->GetComponentL<NPulseMembrane>(MakeLearnerSomaName(i + 1), true);
   if(!soma)
    return true;

   SomaNeuronAmplitude(i + 1, 0) = soma->SumPotential(0, 0);
   SomaNeuronAmplitude(0, 0) += soma->SumPotential(0, 0);
  }

  if(!CalculateMode && (CountIteration > 0) && TrainingPhase != kPhaseDone)
  {
   if(EndOfLearning() && TrainingPhase == kPhaseDone)
    return true;
  }

  if(ExperimentMode)
   Experiment();

  if (IsNeedToTrain)
   Training();

  if(Neuron)
   Output = Neuron->Output;

  return true;
 }
 catch (const UException &ex)
 {
  try
  {
   if (RDK::GetLogger())
   {
    std::ostringstream oss;
    oss << "NNeuronTimeLearner::ACalculate: UException number=" << ex.GetNumber()
        << ", type=" << ex.GetType()
        << ", file=" << ex.GetExFileName()
        << ", line=" << ex.GetExLineNumber()
        << ", what=" << ex.what();
    RDK::GetLogger()->LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner", oss.str(), ex.GetNumber());
   }
  }
  catch (...) {}
  throw;
 }
 catch (const std::exception &ex)
 {
  try
  {
   if (RDK::GetLogger())
   {
    std::ostringstream oss;
    oss << "NNeuronTimeLearner::ACalculate: std::exception what=" << ex.what();
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, oss.str());
   }
  }
  catch (...) {}
  throw;
 }
 catch (...)
 {
  try
  {
   if (RDK::GetLogger())
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, "NNeuronTimeLearner::ACalculate: unknown exception");
  }
  catch (...) {}
  throw;
 }
}

}

#endif
