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

#ifndef NNEURONTIMELEARNERBRANCH_H
#define NNEURONTIMELEARNERBRANCH_H

#include "NDatasetMatrix.h"
#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

/// Single-dendrite temporal-pattern learner: N pulses attach at different distances
/// on one cable. DendriteLength[k] is pulse-k attach position (0 = Soma1 anchor,
/// >=1 = Dendrite1_seg). Reverse sequential sync + joint amp normalize. See ALGORITHM.md.
class RDK_LIB_TYPE NNeuronTimeLearnerBranch: public UNet
{
public:
 /// 0 - do not rebuild; 1 - BuildStructure in ABuild
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> StructureBuildMode;

 /// Class name for DatasetMatrix child generators
 UProperty<std::string, NNeuronTimeLearnerBranch, ptPubParameter> PulseGeneratorClassName;

 /// Neuron class name
 UProperty<std::string, NNeuronTimeLearnerBranch, ptPubParameter> NeuronClassName;

 /// Synapse class name (reserved)
 UProperty<std::string, NNeuronTimeLearnerBranch, ptPubParameter> SynapseClassName;

 /// 0 - auto clear IsNeedToTrain after learning; 1 - external control
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> CalculateMode;

 /// Training in progress
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubParameter> IsNeedToTrain;

 /// Forwarded to DatasetMatrix.Delay (pause between bursts, sec)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> Delay;

 /// Kept for compatibility; not used as iteration length
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> SpikesFrequency;

 /// Number of pulses in the pattern (legacy name; not the neuron dendrite count)
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> NumInputDendrite;

 /// Max dendrite segments
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> MaxDendriteLength;

 /// ISI sequence for DatasetMatrix.MatrixData (N x 1), also stored as TrainingPattern
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptPubParameter> InputPattern;

 /// Reserved (experiments)
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptPubParameter> AdditionalInputPattern;

 /// Previous InputPattern snapshot
 MDMatrix<double> PrevInputPattern;

 /// Dendrite amplitudes: [0]=sum, [i]=Dendrite i_1
 UProperty<MDMatrix<double>,NNeuronTimeLearnerBranch,ptOutput | ptPubState> DendriteNeuronAmplitude;

 /// Soma amplitudes: [0]=sum, [i]=Soma i
 UProperty<MDMatrix<double>,NNeuronTimeLearnerBranch,ptOutput | ptPubState> SomaNeuronAmplitude;

 /// Current LTZone threshold
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> LTZThreshold;

 /// Threshold after training
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> FixedLTZThreshold;

 /// Threshold while training (raised)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> TrainingLTZThreshold;

 /// Force FixedLTZThreshold
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubParameter> UseFixedLTZThreshold;

 /// After EndOfLearning: set FixedLTZThreshold from last synced training LTZ min/max
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubParameter> AutoCalibrateFixedLTZThreshold;

 /// 0 = gap_fraction (min + f*(max-min)); 1 = peak_fraction (max*f)
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> CalibrateLTZThresholdMode;

 /// Fraction for gap_fraction or peak_fraction calibration
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> CalibrateLTZThresholdFraction;

 /// Lower clamp for calibrated threshold
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> CalibrateLTZThresholdMin;

 /// Upper clamp for calibrated threshold
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> CalibrateLTZThresholdMax;

 /// Result of last AutoCalibrateFixedLTZThreshold (0 if not calibrated)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubState> CalibratedFixedLTZThreshold;

 /// Neuron output copy
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> Output;

 /// Norm/sync traces for StatisticDoubleMatrix (1×N or 1×1), updated each training iter
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> AmpDtTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> TipSynapseResistanceTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> ResistanceStatusTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> NoImproveResistanceTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> EffectiveGainTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> DendriteLengthTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> LastAbsDtTrace;
 UProperty<MDMatrix<double>, NNeuronTimeLearnerBranch, ptOutput | ptPubState> StimulusIterTrace;

 /// Resistance for extra synapses (k>=2)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> SynapseResistanceStep;

 /// 0 = structural (grow NumSynapse); 1 = parametric (NumSynapse=1, tune tip Resistance)
 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> NormalizationMode;

 /// Reference tip synapse resistance at L==1 (typically 8.6e7)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> SynapseResistanceBase;

 /// Lower / upper bounds for tip Resistance in parametric mode
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> ResistanceMin;
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> ResistanceMax;

 /// Cable attenuation gamma for feedforward R*=exp(-gamma*deltaL); <=0 = auto-estimate
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> AttenuationGamma;

 /// P-regulator gain for parametric R step (0..1], default 0.4
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> ResistanceAdjustGain;

 /// Current tip ExcSynapse1 resistance per dendrite (parametric mode)
 UProperty<std::vector<double>, NNeuronTimeLearnerBranch, ptPubParameter | ptPubState> TipSynapseResistance;

 /// Pulse attach positions: [k]=0 soma (anchor), [k]>=1 Dendrite1_seg (not per-dendrite lengths)
 UProperty<std::vector<int>, NNeuronTimeLearnerBranch, ptPubParameter> DendriteLength;

 std::vector<int> OldDendriteLength;

 /// Reference soma amplitude at length 1
 UProperty<std::vector<double>, NNeuronTimeLearnerBranch, ptPubParameter> InitialSomaPotential;

 /// Excitatory synapse counts on distal segments
 UProperty<std::vector<int>, NNeuronTimeLearnerBranch, ptPubParameter> NumSynapse;

 /// Min time after first impulse of previous iteration before next may start (sec) — parameter t
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> IterationGap;

 /// |dt| below this => dendrite considered synchronized (sec)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> SyncTolerance;

 /// Half-width of soma peak search window around Expected[i]+cable delay (sec)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> PeakMeasureMargin;

 /// Floor for DelayUseOf agree_margin: max(SyncTolerance, DelayAgreeMarginMin) (sec)
 UProperty<double, NNeuronTimeLearnerBranch, ptPubParameter> DelayAgreeMarginMin;

 /// 0 - joint train (length+synapses); 1 - legacy unused; 2 - Done
 UProperty<int, NNeuronTimeLearnerBranch, ptPubState> TrainingPhase;

 /// One-shot reset to initial untrained structure/state
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubState> ResetToUntrainedState;

 UProperty<int, NNeuronTimeLearnerBranch, ptPubParameter> ExperimentNum;
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubParameter> ExperimentMode;
 UProperty<bool, NNeuronTimeLearnerBranch, ptPubParameter> EnableDebug;

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
 /// Index 0..N-2 trained this burst (round-robin in TimeLearner; reverse sequential here)
 int ActiveDendrite;
 /// First unsynced pulse from the end (N-1 .. 0)
 int ActivePulseIndex;
 /// Locked PeakRel after pulse k finished length+amp (or anchor bootstrap)
 std::vector<double> StoredPeakRel;
 std::vector<bool> PulseSynced;
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
 /// Last values passed to setters during XML load (hash order may precede NumInputDendrite).
 std::vector<double> LoadedInitialSomaPotential;
 std::vector<double> LoadedTipSynapseResistance;
 bool HasLoadedInitialSomaPotential;
 bool HasLoadedTipSynapseResistance;
 /// 0 optimal; 1 grow; -1 shrink
 std::vector<int> DendStatus;
 /// 0 optimal; 1 add synapses; -1 remove
 std::vector<int> SynapseStatus;
 /// Parametric: 0 ok; nonzero = apply pending R change
 std::vector<int> ResistanceStatus;
 std::vector<double> ResistanceDifference;
 std::vector<double> PrevAmpError;
 std::vector<double> PrevResistanceRatio;
 std::vector<int> NoImproveResistanceCount;
 std::vector<double> EffectiveResistanceGain;

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
 static constexpr double kResistanceAdjustGainDefault = 0.4;
 static constexpr double kResistanceSettleRatio = 1e-3;
 /// Absolute |Initial-amp| tolerance for parametric amp Done (was 5e-6; cold runs
 /// often stall ~6e-6 on one dendrite and never clear IsNeedToTrain).
 static constexpr double kAmpNormEps = 1e-5;
 static constexpr double kAmpOscillationBand = 0.005;
 static constexpr double kGainOvershootFactor = 0.5;
 static constexpr double kGainUndershootFactor = 1.5;
 static constexpr double kUndershootBoostRatio = 2.0;
 static constexpr int kNoImproveResistanceLimit = 3;
 static constexpr double kAmpCollapseRatio = 0.35;
 static constexpr int kMaxLengthStep = 8;
 static constexpr int kMaxSynapsesPerDend = 128; // experimental headroom (was 16)

 int EpochCur;
 bool CanChangeDendLength;
 double SumMaxIterSomaAmp;
 double PrevSumMaxIterSomaAmp;

 /// LTZ Potential min/max during current training iteration
 double IterMinLTZPotential;
 double IterMaxLTZPotential;
 bool IterLTZTrackingActive;
 /// Snapshot from last iteration where AllDendritesSynced()
 double LastSyncedMinLTZ;
 double LastSyncedMaxLTZ;

 static constexpr int kCalibrateGapFraction = 0;
 static constexpr int kCalibratePeakFraction = 1;

public:
 NNeuronTimeLearnerBranch(void);
 virtual ~NNeuronTimeLearnerBranch(void);

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
 bool SetAutoCalibrateFixedLTZThreshold(const bool &value);
 bool SetCalibrateLTZThresholdMode(const int &value);
 bool SetCalibrateLTZThresholdFraction(const double &value);
 bool SetCalibrateLTZThresholdMin(const double &value);
 bool SetCalibrateLTZThresholdMax(const double &value);
 bool SetSynapseResistanceStep(const double &value);
 bool SetNormalizationMode(const int &value);
 bool SetSynapseResistanceBase(const double &value);
 bool SetResistanceMin(const double &value);
 bool SetResistanceMax(const double &value);
 bool SetAttenuationGamma(const double &value);
 bool SetResistanceAdjustGain(const double &value);
 bool SetTipSynapseResistance(const std::vector<double> &value);
 bool SetExperimentMode(const bool &value);
 bool SetDendriteLength(const std::vector<int> &value);
 bool SetInitialSomaPotential(const std::vector<double> &value);
 bool SetNumSynapse(const std::vector<int> &value);
 bool SetIterationGap(const double &value);
 bool SetSyncTolerance(const double &value);
 bool SetPeakMeasureMargin(const double &value);
 bool SetDelayAgreeMarginMin(const double &value);
 bool SetResetToUntrainedState(const bool &value);
 bool SetExperimentNum(const int &value);
 bool SetEnableDebug(const bool &value);

 virtual NNeuronTimeLearnerBranch* New(void);
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
 int PulseAttachPos(int pulse_k) const;
 int SynapseIndexOnMembrane(int pulse_k) const;
 int ChainLengthMax(void) const;
 NPulseMembrane* GetInputMembraneForPulse(int pulse_k) const;
 void ApplyPulseGeneratorMute(void);
 double MeanStoredPeakRel(int after_pulse_exclusive) const;
 bool PulseLengthAndAmpDone(int pulse_k) const;
 bool SetTipSynapseResistanceOnComponent(int dendrite_index0, double r);
 bool ChangeSynapseResistanceStatus(int num);
 bool ApplySynapseResistanceChange(int num);
 double ComputeModelTipResistance(int dendrite_index0) const;
 double ComputeDampedTipResistance(int dendrite_index0, double r_old,
  double amp, double initial, double dt, double &effective_gain_out) const;
 void ApplyComputedResistance(int num, double r_old, double r_new, double effective_gain);
 void FeedforwardResistanceOnLengthGrow(int dendrite_index0, int deltaL);
 void EnforceParametricSynapseCount(void);
 void ApplyLoadedAnchorProperties(void);
 /// True when every dendrite length is <= 1 (cold / untrained topology).
 bool StructureLooksUntrained(const std::vector<int> &lengths) const;
 /// Push TrainingLTZ or FixedLTZ onto LTZThreshold + neuron LTZone.
 void ApplyActiveLtzThreshold(void);
 double ReadLTZonePotential(void) const;
 void UpdateIterLTZPotential(void);
 void CalibrateFixedLTZThresholdFromTraining(void);
 void UpdateNormTraces(void);
 bool PatternRecognition(void);
 bool LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern);
 bool IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern);
 bool Experiment(void);
 bool EndOfLearning(void);
 bool Training(void);
 virtual bool ACalculate(void);

 /// Path to dataset generator feature_index (0-based) under this net
 static std::string DatasetGeneratorPath(int feature_index = 0);
 NPulseGeneratorTransit* GetDatasetGenerator(void);
 NPulseGeneratorTransit* GetDatasetGenerator(int feature_index);
 bool LinkSynapseToDataset(NPulseSynapseCommon *synapse, int feature_index);
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

#endif // NNEURONTIMELEARNERBRANCH_H
