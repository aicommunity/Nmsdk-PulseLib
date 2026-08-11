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
 // Always recreate: after dendrite growth CheckLink may stay true while connectors are gone.
 synapse->DisconnectAll("Input");
 return CreateLink(gen_path, "Output", synapse->GetLongName(this), "Input");
}

bool NNeuronTimeLearner::RelinkDendriteSynapsesToDataset(int dendrite_index0)
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return true;
 // Drive the distal tip. Also keep a link on segment 1: for some membrane builds the
 // newly added distal segment accepts CreateLink but never shows synaptic current.
 const int tip_seg = DendriteLength[dendrite_index0];
 const int segs[2] = { tip_seg, 1 };
 for(int s = 0; s < 2; ++s)
 {
  if(s == 1 && segs[1] == segs[0])
   break;
  UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(dendrite_index0 + 1, segs[s]), true);
  if(!dendrite)
   continue;
  const int nsyn = (segs[s] == tip_seg) ? NumSynapse[dendrite_index0] : 1;
  for(int k = 0; k < nsyn; k++)
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

bool NNeuronTimeLearner::SyncInputPatternToDataset(const MDMatrix<double> *pattern_override)
{
 if(!Dataset)
  return true;

 SyncDatasetDimsFromDendrites();

 MDMatrix<double> matrix = pattern_override ? *pattern_override : InputPattern.GetData();
 const int n = NumInputDendrite.GetData();
 if(matrix.GetRows() != n || matrix.GetCols() != 1)
  matrix.Resize(n, 1, 0.0);

 // SetMatrixData stores payload and validates dims against MaxSpikesPerFeature.
 if(!Dataset->SetMatrixData(matrix))
  return false;

 if(Dataset->MatrixClasses.GetRows() != 1 || Dataset->MatrixClasses.GetCols() != 1)
  Dataset->MatrixClasses.Resize(1, 1, 0);

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "SyncInputPatternToDataset: trainer InputPattern -> DatasetMatrix.MatrixData rows="
      << Dataset->MatrixData.GetRows() << " cols=" << Dataset->MatrixData.GetCols()
      << " values=[";
  for(int i = 0; i < n && i < Dataset->MatrixData.GetRows(); ++i)
  {
   if(i) oss << ',';
   oss << Dataset->MatrixData(i, 0);
  }
  oss << "]";
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
 return true;
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
 ResetToUntrainedState("ResetToUntrainedState", this, &NNeuronTimeLearner::SetResetToUntrainedState),
 ExperimentNum("ExperimentNum", this, &NNeuronTimeLearner::SetExperimentNum),
 ExperimentMode("ExperimentMode", this, &NNeuronTimeLearner::SetExperimentMode),
 EnableDebug("EnableDebug", this, &NNeuronTimeLearner::SetEnableDebug)
{
 OldNumInputDendrite = 0;
 Dataset = NULL;
 Neuron = NULL;
 IsFirstFileStep = true;
 HasUntrainedSnapshot = false;
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

bool NNeuronTimeLearner::ResetToUntrained(void)
{
 if(HasUntrainedSnapshot)
 {
  DendriteLength = UntrainedDendriteLength;
  NumSynapse = UntrainedNumSynapse;
  InitialSomaPotential = UntrainedInitialSomaPotential;
 }
 else
 {
  DendriteLength.assign(NumInputDendrite, 1);
  NumSynapse.assign(NumInputDendrite, 1);
  InitialSomaPotential.assign(NumInputDendrite, 0.0);
 }
 OldDendriteLength = DendriteLength.GetData();
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 const double period = IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5;
 Dissynchronization.assign(NumInputDendrite, period);
 if(!Dissynchronization.empty())
  Dissynchronization[NumInputDendrite - 1] = 0.0;
 AmpDifference.assign(NumInputDendrite, 0.0);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);
 CountIteration = 0;
 IsFirstBeat = true;
 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 PeakLocked.assign(NumInputDendrite, false);
 PeakSeen.assign(NumInputDendrite, false);

 if (Neuron)
 {
  MDMatrix<double> empty_pattern;
  MDMatrix<int> empty_int;
  empty_pattern.Resize(0, 0);
  empty_int.Resize(0, 0);
  Neuron->TrainingPattern = empty_pattern;
  Neuron->TrainingDendIndexes = empty_int;
  Neuron->TrainingSynapsisNum = empty_int;
 }

 Ready = false;
 return BuildStructure();
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
 if(Dataset && !SyncInputPatternToDataset(&value))
  return false;
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

bool NNeuronTimeLearner::SetResetToUntrainedState(const bool &value)
{
 (void)value;
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
 (void)pointer;
 UEPtr<NDatasetMatrix> dataset = dynamic_pointer_cast<NDatasetMatrix>(comp);
 if(dataset)
 {
  Dataset = dataset;
  SyncInputPatternToDataset();
 }
 return true;
}

bool NNeuronTimeLearner::ADelComponent(UEPtr<UContainer> comp)
{
 (void)comp;
 return true;
}

bool NNeuronTimeLearner::ABeforeBuild(void)
{
 if(!Dataset)
  Dataset = GetComponentL<NDatasetMatrix>(std::string("DatasetMatrix"), true);
 if(Dataset)
  SyncInputPatternToDataset();
 return true;
}

bool NNeuronTimeLearner::BuildStructure()
{
 bool res(true);
 try
 {
 Neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
 Neuron->SetCoord(MVector<double,3>(8.7 + 1 * 7, 1.67, 0));
 // Independent dendrite lengths are required for temporal sync.
 Neuron->StructureBuildMode = 2;
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
  Neuron->StructureBuildMode = 2;
 }
 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 Neuron->Reset();

 // Remove legacy per-dendrite Sources from copied Learner layouts
 for(int i = 0; i < std::max(OldNumInputDendrite, NumInputDendrite.GetData()) + 4; i++)
 {
  DelComponent(std::string("Source") + sntoa(i + 1));
 }

 Dataset = AddMissingComponent<NDatasetMatrix>(std::string("DatasetMatrix"), "NDatasetMatrix");
 if(!Dataset)
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "BuildStructure: failed to create DatasetMatrix (is NDatasetMatrix registered?)");
  return false;
 }
 Dataset->SetCoord(MVector<double,3>(6.7, 1.67, 0));
 if(InputPattern.GetRows() != NumInputDendrite || InputPattern.GetCols() != 1)
  InputPattern.Resize(NumInputDendrite, 1, 0.0);
 if(!SyncInputPatternToDataset())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "BuildStructure: failed to sync InputPattern into DatasetMatrix");
  return false;
 }
 Dataset->Build();
 Dataset->Reset();
 if(!SyncInputPatternToDataset())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "BuildStructure: failed to sync InputPattern into DatasetMatrix");
  return false;
 }

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
 NeuronClassName = "NSPNeuronBio2";
 SynapseClassName = "NPSynapseBio";
 IsNeedToTrain = true;
 ExperimentMode = false;
 Delay = 0.5;
 SpikesFrequency = 1.5;
 NumInputDendrite = 1;
 OldNumInputDendrite = 0;
 MaxDendriteLength = 100;
 IterationGap = 0.5;
 // ~1 model step (DefaultTimeStep often 2e-3 s); 1e-6 never matched discrete peaks
 SyncTolerance = 0.02;
 TrainingPhase = kPhaseSync;
 ResetToUntrainedState = false;
 HasUntrainedSnapshot = false;

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
 if(!HasUntrainedSnapshot)
 {
  UntrainedDendriteLength = DendriteLength.GetData();
  UntrainedNumSynapse = NumSynapse.GetData();
  UntrainedInitialSomaPotential = InitialSomaPotential.GetData();
  HasUntrainedSnapshot = true;
 }

 if(ResetToUntrainedState.GetData())
 {
  ResetToUntrainedState.SetDataDirect(false);
  if(!ResetToUntrained())
   return false;
 }

 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>(std::string("Neuron"),true);
 if(n_in)
 {
  UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
  if(ltzone)
   ltzone->Threshold = LTZThreshold;
 }

 if(Dataset)
 {
  Dataset->Reset();
  if(!SyncInputPatternToDataset())
   return false;
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

 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(gen)
  gen->DisconnectAll("Output");

 OldDendriteLength[num] = DendriteLength[num];
 DendriteLength[num] += DendStatus[num];

 if(Neuron->StructureBuildMode != 2)
  Neuron->StructureBuildMode = 2;

 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 if((DendStatus[num] == 1) && (DendriteLength[num] > 1))
 {
  UEPtr<NPulseMembrane> prevdendrite = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(num + 1, DendriteLength[num] - 1), true);
  if(prevdendrite)
  {
   prevdendrite->NumExcitatorySynapses = 1;
   prevdendrite->Build();
  }
 }

 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
  MakeLearnerDendriteName(num + 1, DendriteLength[num]), true);
 if(!dendrite)
  return false;
 dendrite->NumExcitatorySynapses = NumSynapse[num];
 dendrite->Build();

 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 // Relink keeps a drive on segment 1 as well as the tip (see RelinkDendriteSynapsesToDataset):
 // distal-only injection on BuildStructure-created cables currently yields amp=0.
 for(int d = 0; d < NumInputDendrite; ++d)
  res &= RelinkDendriteSynapsesToDataset(d);

 if(gen)
  gen->Reset();
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
 if(!IterationActive || !Neuron)
  return true;

 const double now = Environment->GetTime().GetDoubleTime();
 const size_t n = static_cast<size_t>(NumInputDendrite.GetData());
 if(PeakLocked.size() != n)
  PeakLocked.assign(n, false);
 if(PeakSeen.size() != n)
  PeakSeen.assign(n, false);

 const double pattern_end = ExpectedPulseRelTimes.empty()
  ? 0.0 : ExpectedPulseRelTimes.back();
 // Allow dendritic delay up to the full pattern span plus a small settle margin.
 const double settle = 0.08;

 for(int i = 0; i < NumInputDendrite; ++i)
 {
  if(i < int(PeakLocked.size()) && PeakLocked[i])
   continue;

  const double t_pulse = FirstImpulseTime
   + ((i < int(ExpectedPulseRelTimes.size())) ? ExpectedPulseRelTimes[i] : 0.0);
  if(now + 1e-12 < t_pulse)
   continue;

  const double t_end = FirstImpulseTime + pattern_end + settle;
  if(now > t_end)
  {
   PeakLocked[i] = true;
   continue;
  }

  UEPtr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>(MakeLearnerSomaName(i + 1), true);
  if(!soma)
   continue;

  const double currentsomaamp = soma->SumPotential(0, 0);
  // Ignore empty traces (broken tip links previously left amp==0 and
  // `>= 0` kept pushing TimeOfMax to the window end).
  if(currentsomaamp <= 1.0e-12)
   continue;

  if(currentsomaamp >= MaxIterSomaAmp[i])
  {
   MaxIterSomaAmp[i] = currentsomaamp;
   TimeOfMaxIterSomaAmp[i] = now;
   if(i < int(PeakSeen.size()))
    PeakSeen[i] = true;

   if(DendriteLength[i] == 1 && NumSynapse[i] && currentsomaamp > InitialSomaPotential[i])
    InitialSomaPotential[i] = currentsomaamp;
  }
  else if(i < int(PeakSeen.size()) && PeakSeen[i]
          && MaxIterSomaAmp[i] > 0.0
          && currentsomaamp < MaxIterSomaAmp[i] * 0.85)
  {
   PeakLocked[i] = true;
  }
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

 // Only SyncTolerance stops growth. The Learner sign-flip heuristic freezes too early
 // when a single-segment step overshoots noisy timing.
 if(fabs(dt) <= SyncTolerance)
  DendStatus[num] = 0;
 else if(dt > 0)
  DendStatus[num] = 1;
 else
  DendStatus[num] = -1;

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
  if(i < int(Dissynchronization.size()) && fabs(Dissynchronization[i]) > SyncTolerance.GetData())
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
 if (EnableDebug.GetData() && RDK::GetLogger())
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", "phase -> Done");
 return true;
}


bool NNeuronTimeLearner::DetectNewImpulse(void)
{
 return DetectNewImpulseCount() > 0;
}

int NNeuronTimeLearner::DetectNewImpulseCount(void)
{
 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(!gen)
  return 0;

 const int counter = gen->PulseCounter;
 double out = 0.0;
 if(gen->Output.GetRows() > 0 && gen->Output.GetCols() > 0)
  out = gen->Output(0, 0);

 int count = 0;
 if(counter > PrevPulseCounter)
 {
  count = counter - PrevPulseCounter;
  PrevPulseCounter = counter;
 }
 else if(out > 0.0 && PrevGenOutput <= 0.0)
 {
  count = 1;
 }
 PrevGenOutput = out;
 return count > 0 ? count : 0;
}

void NNeuronTimeLearner::BeginTrainingIteration(double now)
{
 StartIterTime = now;
 FirstImpulseTime = now;
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 PeakLocked.assign(NumInputDendrite, false);
 PeakSeen.assign(NumInputDendrite, false);
 ExpectedPulseRelTimes.assign(NumInputDendrite, 0.0);
 {
  double acc = 0.0;
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   // InputPattern holds ISI slots (same layout as DatasetMatrix.MatrixData)
   double isi = 0.0;
   if(InputPattern.GetRows() > i && InputPattern.GetCols() > 0)
    isi = InputPattern(i, 0);
   if(isi < 0.0)
    isi = 0.0;
   acc += isi;
   ExpectedPulseRelTimes[i] = acc;
  }
 }
 WaitingPeakAfterLastPulse = (NumInputDendrite <= 1);
 LastPulseTime = now;
 IterationActive = true;
 IsFirstBeat = false;

 // Structure changes are applied in FinishTrainingIteration (during the inter-burst
 // gap). Changing length here would Reset() the neuron after pulse 0 and break timing.

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BeginTrainingIteration: iter=" << CountIteration
      << " phase=" << TrainingPhase.GetData()
      << " t=" << now
      << " N=" << NumInputDendrite.GetData()
      << " gap=" << IterationGap.GetData()
      << " delay=" << Delay.GetData()
      << " expected=[";
  for(size_t i = 0; i < ExpectedPulseRelTimes.size(); ++i)
  {
   if(i) oss << ',';
   oss << ExpectedPulseRelTimes[i];
  }
  oss << "]";
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

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "FinishTrainingIteration: iter=" << CountIteration
      << " phase=" << TrainingPhase.GetData()
      << " pulseIndex=" << PulseIndexInIter
      << " DendStatus=[";
  for(size_t i = 0; i < DendStatus.size(); ++i)
  {
   if(i) oss << ',';
   oss << DendStatus[i];
  }
  oss << "] SynapseStatus=[";
  for(size_t i = 0; i < SynapseStatus.size(); ++i)
  {
   if(i) oss << ',';
   oss << SynapseStatus[i];
  }
  oss << "] times=[";
  for(size_t i = 0; i < TimeOfMaxIterSomaAmp.size(); ++i)
  {
   if(i) oss << ',';
   oss << TimeOfMaxIterSomaAmp[i];
  }
  oss << "] amp=[";
  for(size_t i = 0; i < MaxIterSomaAmp.size(); ++i)
  {
   if(i) oss << ',';
   oss << MaxIterSomaAmp[i];
  }
  oss << "] len=[";
  for(size_t i = 0; i < DendriteLength.size(); ++i)
  {
   if(i) oss << ',';
   oss << DendriteLength[i];
  }
  oss << "] dsyn=[";
  for(size_t i = 0; i < Dissynchronization.size(); ++i)
  {
   if(i) oss << ',';
   oss << Dissynchronization[i];
  }
  oss << "]";
  if(Neuron && !DendriteLength.empty())
  {
   oss << " d0amp=[";
   for(int seg = 1; seg <= DendriteLength[0]; ++seg)
   {
    UEPtr<NPulseMembrane> dmem = Neuron->GetComponentL<NPulseMembrane>(
     MakeLearnerDendriteName(1, seg), true);
    if(seg > 1) oss << ',';
    oss << (dmem ? dmem->SumPotential(0, 0) : -1.0);
   }
   oss << "]";
  }
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }

 // Apply structure updates in the inter-burst gap so the next burst sees a stable neuron.
 if(TrainingPhase == kPhaseSync && CanChangeDendLength)
 {
  int worst = -1;
  double worst_abs = -1.0;
  for(int j = 0; j < NumInputDendrite - 1; ++j)
  {
   if(!DendStatus[j])
    continue;
   const double a = fabs(Dissynchronization[j]);
   if(a > worst_abs)
   {
    worst_abs = a;
    worst = j;
   }
  }
  if(worst >= 0)
   ChangeDendriteLength(worst);
 }
 else if(TrainingPhase == kPhaseNormalize)
 {
  for(int i = 0; i < NumInputDendrite; ++i)
   ChangeSynapseNumber(i);
 }

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
 const int new_pulses = DetectNewImpulseCount();

 if(!IterationActive)
 {
  bool can_start = !HasPrevIteration;
  if(HasPrevIteration)
   can_start = (now - PrevFirstImpulseTime) >= IterationGap;

  if(new_pulses > 0 && can_start)
  {
   BeginTrainingIteration(now);
   MeasureMaxPotentialAndTime();
  }
  return true;
 }

 // Track how far the burst schedule has progressed (for debug / LastPulseTime).
 {
  int active = NumInputDendrite - 1;
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   const double t0 = FirstImpulseTime + ExpectedPulseRelTimes[i];
   if(now + 1e-12 < t0)
   {
    active = (i > 0) ? (i - 1) : 0;
    break;
   }
   active = i;
  }
  PulseIndexInIter = active;
  ActiveMeasureSoma = active;
  if(active >= NumInputDendrite - 1)
  {
   WaitingPeakAfterLastPulse = true;
   LastPulseTime = FirstImpulseTime + ExpectedPulseRelTimes[NumInputDendrite - 1];
  }
 }

 MeasureMaxPotentialAndTime();

 const double last_pulse = FirstImpulseTime
  + ((NumInputDendrite > 0 && !ExpectedPulseRelTimes.empty())
     ? ExpectedPulseRelTimes[NumInputDendrite - 1] : 0.0);
 bool all_locked = !PeakLocked.empty();
 for(size_t i = 0; i < PeakLocked.size(); ++i)
 {
  if(!PeakLocked[i])
  {
   all_locked = false;
   break;
  }
 }

 // Wait until every soma locked its first post-pulse max, or IterationGap.
 // Do not cut shortly after the last ISI — long dendrites need more settle time.
 bool finished = false;
 if(all_locked && now >= last_pulse + 0.01)
  finished = true;
 if((now - FirstImpulseTime) >= IterationGap)
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
