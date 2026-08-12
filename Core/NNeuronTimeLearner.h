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

#ifndef NNEURONTIMELEARNER_H
#define NNEURONTIMELEARNER_H

#include "NDatasetMatrix.h"
#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

/// Temporal-pattern neuron learner: one NDatasetMatrix burst fans out to all dendrites;
/// joint train = one-dendrite-per-burst length sync (PrevPeakRel / DelayFromPulse) plus
/// synapse amp normalize each burst (NNeuronLearner-style). See ALGORITHM.md.
class RDK_LIB_TYPE NNeuronTimeLearner: public UNet
{
public:
 /// 0 - do not rebuild; 1 - BuildStructure in ABuild
 UProperty<int, NNeuronTimeLearner, ptPubParameter> StructureBuildMode;

 /// Class name for DatasetMatrix child generators
 UProperty<std::string, NNeuronTimeLearner, ptPubParameter> PulseGeneratorClassName;

 /// Neuron class name
 UProperty<std::string, NNeuronTimeLearner, ptPubParameter> NeuronClassName;

 /// Synapse class name (reserved)
 UProperty<std::string, NNeuronTimeLearner, ptPubParameter> SynapseClassName;

 /// 0 - auto clear IsNeedToTrain after learning; 1 - external control
 UProperty<int, NNeuronTimeLearner, ptPubParameter> CalculateMode;

 /// Training in progress
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> IsNeedToTrain;

 /// Forwarded to DatasetMatrix.Delay (pause between bursts, sec)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> Delay;

 /// Kept for compatibility; not used as iteration length
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SpikesFrequency;

 /// Number of dendrites (= MaxSpikesPerFeature on DatasetMatrix)
 UProperty<int, NNeuronTimeLearner, ptPubParameter> NumInputDendrite;

 /// Max dendrite segments
 UProperty<int, NNeuronTimeLearner, ptPubParameter> MaxDendriteLength;

 /// ISI sequence for DatasetMatrix.MatrixData (N x 1), also stored as TrainingPattern
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptPubParameter> InputPattern;

 /// Reserved (experiments)
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptPubParameter> AdditionalInputPattern;

 /// Previous InputPattern snapshot
 MDMatrix<double> PrevInputPattern;

 /// Dendrite amplitudes: [0]=sum, [i]=Dendrite i_1
 UProperty<MDMatrix<double>,NNeuronTimeLearner,ptOutput | ptPubState> DendriteNeuronAmplitude;

 /// Soma amplitudes: [0]=sum, [i]=Soma i
 UProperty<MDMatrix<double>,NNeuronTimeLearner,ptOutput | ptPubState> SomaNeuronAmplitude;

 /// Current LTZone threshold
 UProperty<double, NNeuronTimeLearner, ptPubParameter> LTZThreshold;

 /// Threshold after training
 UProperty<double, NNeuronTimeLearner, ptPubParameter> FixedLTZThreshold;

 /// Threshold while training (raised)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> TrainingLTZThreshold;

 /// Force FixedLTZThreshold
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> UseFixedLTZThreshold;

 /// Neuron output copy
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> Output;

 /// Resistance for extra synapses (k>=2)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SynapseResistanceStep;

 /// 0 = structural (grow NumSynapse); 1 = parametric (NumSynapse=1, tune tip Resistance)
 UProperty<int, NNeuronTimeLearner, ptPubParameter> NormalizationMode;

 /// Reference tip synapse resistance at L==1 (typically 8.6e7)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SynapseResistanceBase;

 /// Lower / upper bounds for tip Resistance in parametric mode
 UProperty<double, NNeuronTimeLearner, ptPubParameter> ResistanceMin;
 UProperty<double, NNeuronTimeLearner, ptPubParameter> ResistanceMax;

 /// Cable attenuation gamma for feedforward R*=exp(-gamma*deltaL); <=0 = auto-estimate
 UProperty<double, NNeuronTimeLearner, ptPubParameter> AttenuationGamma;

 /// Current tip ExcSynapse1 resistance per dendrite (parametric mode)
 UProperty<std::vector<double>, NNeuronTimeLearner, ptPubParameter | ptPubState> TipSynapseResistance;

 /// Dendrite segment counts
 UProperty<std::vector<int>, NNeuronTimeLearner, ptPubParameter> DendriteLength;

 std::vector<int> OldDendriteLength;

 /// Reference soma amplitude at length 1
 UProperty<std::vector<double>, NNeuronTimeLearner, ptPubParameter> InitialSomaPotential;

 /// Excitatory synapse counts on distal segments
 UProperty<std::vector<int>, NNeuronTimeLearner, ptPubParameter> NumSynapse;

 /// Min time after first impulse of previous iteration before next may start (sec) — parameter t
 UProperty<double, NNeuronTimeLearner, ptPubParameter> IterationGap;

 /// |dt| below this => dendrite considered synchronized (sec)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SyncTolerance;

 /// 0 - joint train (length+synapses); 1 - legacy unused; 2 - Done
 UProperty<int, NNeuronTimeLearner, ptPubState> TrainingPhase;

 /// One-shot reset to initial untrained structure/state
 UProperty<bool, NNeuronTimeLearner, ptPubState> ResetToUntrainedState;

 UProperty<int, NNeuronTimeLearner, ptPubParameter> ExperimentNum;
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> ExperimentMode;
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> EnableDebug;

protected:
 int OldNumInputDendrite;

 UEPtr<NDatasetMatrix> Dataset;
 UEPtr<NPulseNeuron> Neuron;

 double StartIterTime;
 double FirstImpulseTime;
 double PrevFirstImpulseTime;
 bool HasPrevIteration;
 int PulseIndexInIter;
 int ActiveMeasureSoma;
 int PrevPulseCounter;
 double PrevGenOutput;
 bool IterationActive;
 bool WaitingPeakAfterLastPulse;
 double LastPulseTime;
 std::vector<double> ExpectedPulseRelTimes;
 /// First local max after pulse i is locked (descending flank seen)
 std::vector<bool> PeakLocked;
 std::vector<bool> PeakSeen;

 ifstream Fin;
 ofstream Fout;
 MDMatrix<double> Inputs;
 std::vector<int> Outputs;
 bool IsWritten;
 bool IsFirstFileStep;
 int CountIteration;
 bool HasUntrainedSnapshot;

 bool IsFirstBeat;
 double IterLength;
 std::vector<double> MaxIterSomaAmp;
 std::vector<double> TimeOfMaxIterSomaAmp;
 std::vector<double> Dissynchronization;
 /// Snapshot of Dissynchronization at the start of FinishTrainingIteration
 std::vector<double> PrevDissynchronization;
 std::vector<double> AmpDifference;
 /// True if MaxIterSomaAmp[i] exceeded kMinMeasurableSomaAmp this iteration
 std::vector<bool> SomaPeakValid;
 /// TimeOfMax[i] - FirstImpulseTime (current burst)
 std::vector<double> PeakRel;
 /// TimeOfMax[i] - (FirstImpulseTime + ExpectedPulseRelTimes[i])
 std::vector<double> DelayFromPulse;
 /// Previous-burst PeakRel / Delay / validity (Sync reference snapshot)
 std::vector<double> PrevPeakRel;
 std::vector<double> PrevDelayFromPulse;
 std::vector<bool> PrevPeakValid;
 bool HasPrevPeakSnapshot;
 /// Index 0..N-2 trained this burst (round-robin)
 int ActiveDendrite;
 /// |dt| from last decision per dendrite (AllDendritesSynced / no-improve)
 std::vector<double> DendLastAbsDt;
 std::vector<int> NoImproveCount;
 /// Accepted without |dt|≤tol (amp-collapse / no-improve stop)
 std::vector<bool> DendBestEffortSynced;
 /// Estimated cable delay per segment (sec); refined from observed |Δdt|/ΔL
 double EstDelayPerSeg;
 /// Last length delta applied for EstDelayPerSeg update
 int LastLengthDelta;
 int LastLengthDeltaDendrite;
 std::vector<int> UntrainedDendriteLength;
 std::vector<int> UntrainedNumSynapse;
 std::vector<double> UntrainedInitialSomaPotential;
 std::vector<double> UntrainedTipSynapseResistance;
 /// 0 optimal; 1 grow; -1 shrink
 std::vector<int> DendStatus;
 /// 0 optimal; 1 add synapses; -1 remove
 std::vector<int> SynapseStatus;
 /// Parametric: 0 ok; nonzero = apply pending R change
 std::vector<int> ResistanceStatus;
 std::vector<double> ResistanceDifference;

 static constexpr int kNormStructural = 0;
 static constexpr int kNormParametric = 1;
 static constexpr double kSynapseResistanceBioDefault = 8.6e7;
 static constexpr double kAttenuationGammaAuto = -1.0;
 static constexpr double kAttenuationGammaFallback = 0.05;

 static constexpr double kMinMeasurableSomaAmp = 1e-6;
 static constexpr double kMinSettle = 0.08;
 static constexpr double kDelayPerSegDefault = 0.01;
 static constexpr double kGapSlack = 0.05;
 static constexpr int kNoImproveLimit = 2;
 static constexpr double kAmpCollapseRatio = 0.35;
 static constexpr int kMaxLengthStep = 8;
 static constexpr int kMaxSynapsesPerDend = 128; // experimental headroom (was 16)

 int EpochCur;
 bool CanChangeDendLength;
 double SumMaxIterSomaAmp;
 double PrevSumMaxIterSomaAmp;

public:
 NNeuronTimeLearner(void);
 virtual ~NNeuronTimeLearner(void);

 bool SetStructureBuildMode(const int &value);
 bool SetPulseGeneratorClassName(const std::string &value);
 bool SetNeuronClassName(const std::string &value);
 bool SetSynapseClassName(const std::string &value);
 bool SetCalculateMode(const int &value);
 bool SetIsNeedToTrain(const bool &value);
 bool SetDelay(const double &value);
 bool SetSpikesFrequency(const double &value);
 bool SetNumInputDendrite(const int &value);
 bool SetMaxDendriteLength(const int &value);
 bool SetInputPattern(const MDMatrix<double> &value);
 bool SetAdditionalInputPattern(const MDMatrix<double> &value);
 bool SetPrevInputPattern(const MDMatrix<double> &value);
 bool SetLTZThreshold(const double &value);
 bool SetTrainingLTZThreshold(const double &value);
 bool SetFixedLTZThreshold(const double &value);
 bool SetUseFixedLTZThreshold(const bool &value);
 bool SetSynapseResistanceStep(const double &value);
 bool SetNormalizationMode(const int &value);
 bool SetSynapseResistanceBase(const double &value);
 bool SetResistanceMin(const double &value);
 bool SetResistanceMax(const double &value);
 bool SetAttenuationGamma(const double &value);
 bool SetTipSynapseResistance(const std::vector<double> &value);
 bool SetExperimentMode(const bool &value);
 bool SetDendriteLength(const std::vector<int> &value);
 bool SetInitialSomaPotential(const std::vector<double> &value);
 bool SetNumSynapse(const std::vector<int> &value);
 bool SetIterationGap(const double &value);
 bool SetSyncTolerance(const double &value);
 bool SetResetToUntrainedState(const bool &value);
 bool SetExperimentNum(const int &value);
 bool SetEnableDebug(const bool &value);

 virtual NNeuronTimeLearner* New(void);
 static UComponent* NewStatic(void);

protected:
 virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);
 virtual bool ADelComponent(UEPtr<UContainer> comp);

 bool BuildStructure();
 virtual bool ABeforeBuild(void);
 virtual bool ADefault(void);
 virtual bool ABuild(void);
 void UpdateComputationOrder(void) override;
 virtual bool AReset(void);

 bool CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e);
 bool ZeroingTrainingPattern(void);
 bool ResetToUntrained(void);
 bool ChangeDendriteLength(int num);
 /// Grow/shrink pending length (typically one active); synapses applied separately in Finish
 bool ApplyPendingDendriteLengthChanges(void);
 bool ChangeSynapseNumber(int num);
 bool MeasureMaxPotentialAndTime(void);
 bool ChangeDendriteStatus(int num);
 bool ChangeSynapseStatus(int num);
 bool IsParametricNormalization(void) const;
 double ClampResistance(double r) const;
 NPulseSynapseCommon* GetTipSynapse(int dendrite_index0) const;
 bool SetTipSynapseResistanceOnComponent(int dendrite_index0, double r);
 bool ChangeSynapseResistanceStatus(int num);
 bool ApplySynapseResistanceChange(int num);
 void FeedforwardResistanceOnLengthGrow(int dendrite_index0, int deltaL);
 void EnforceParametricSynapseCount(void);
 bool PatternRecognition(void);
 bool LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern);
 bool IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern);
 bool Experiment(void);
 bool EndOfLearning(void);
 bool Training(void);
 virtual bool ACalculate(void);

 /// Path to shared dataset generator under this net
 static std::string DatasetGeneratorPath(void);
 NPulseGeneratorTransit* GetDatasetGenerator(void);
 bool LinkSynapseToDataset(NPulseSynapseCommon *synapse);
 bool RelinkDendriteSynapsesToDataset(int dendrite_index0);
 void SyncDatasetDimsFromDendrites(void);
 bool SyncInputPatternToDataset(const MDMatrix<double> *pattern_override = 0);
 bool DetectNewImpulse(void);
 int DetectNewImpulseCount(void);
 void BeginTrainingIteration(double now);
 void FinishTrainingIteration(void);
 bool AllDendritesSynced(void) const;
 bool AllSynapsesNormalized(void) const;

 double PatternSpanSec() const;
 double SettleMarginSec() const;
 double EffectiveIterationGapSec() const;
 double EffectiveDatasetDelaySec() const;
 void ResizeSyncVectors(int n);
 void ComputePeakRelAndDelay(void);
 int SelectActiveDendrite() const;
 void CommitPrevPeakSnapshot(void);
 void RefreshDendLastAbsDtFromPrevAnchor(void);
 double DelayLenOf(int num) const;
 /// Prefer DelayFromPulse when it agrees with cable model; else delay_len.
 double DelayUseOf(int num, double expected_k, double delay_meas) const;
};

}

#endif // NNEURONTIMELEARNER_H
