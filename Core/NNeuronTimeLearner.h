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
 
 /// After EndOfLearning: set FixedLTZThreshold from last synced training LTZ min/max
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> AutoCalibrateFixedLTZThreshold;
 
 /// 0 = gap_fraction (min + f*(max-min)); 1 = peak_fraction (max*f)
 UProperty<int, NNeuronTimeLearner, ptPubParameter> CalibrateLTZThresholdMode;
 
 /// Fraction for gap_fraction or peak_fraction calibration
 UProperty<double, NNeuronTimeLearner, ptPubParameter> CalibrateLTZThresholdFraction;
 
 /// Lower clamp for calibrated threshold
 UProperty<double, NNeuronTimeLearner, ptPubParameter> CalibrateLTZThresholdMin;
 
 /// Upper clamp for calibrated threshold
 UProperty<double, NNeuronTimeLearner, ptPubParameter> CalibrateLTZThresholdMax;
 
 /// Result of last AutoCalibrateFixedLTZThreshold (0 if not calibrated)
 UProperty<double, NNeuronTimeLearner, ptPubState> CalibratedFixedLTZThreshold;
 
 /// Neuron output copy
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> Output;
 
 /// Trace: amp error / dt diagnostic (1×N), for StatisticDoubleMatrix
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> AmpDtTrace;
 
 /// Trace: tip synapse resistance per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> TipSynapseResistanceTrace;
 
 /// Trace: ResistanceStatus per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> ResistanceStatusTrace;
 
 /// Trace: NoImproveResistanceCount per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> NoImproveResistanceTrace;
 
 /// Trace: EffectiveResistanceGain per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> EffectiveGainTrace;
 
 /// Trace: DendriteLength per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> DendriteLengthTrace;
 
 /// Trace: |dt| from last sync decision per dendrite
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> LastAbsDtTrace;
 
 /// Trace: stimulus / iteration counter (1×1 or 1×N)
 UProperty<MDMatrix<double>, NNeuronTimeLearner, ptOutput | ptPubState> StimulusIterTrace;
 
 /// Resistance for extra synapses (k>=2)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SynapseResistanceStep;
 
 /// 0 = structural (grow NumSynapse); 1 = parametric (NumSynapse=1, tune tip Resistance)
 UProperty<int, NNeuronTimeLearner, ptPubParameter> NormalizationMode;
 
 /// Reference tip synapse resistance at L==1 (typically 8.6e7)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SynapseResistanceBase;
 
 /// Lower bound for tip Resistance in parametric mode
 UProperty<double, NNeuronTimeLearner, ptPubParameter> ResistanceMin;
 
 /// Upper bound for tip Resistance in parametric mode
 UProperty<double, NNeuronTimeLearner, ptPubParameter> ResistanceMax;
 
 /// Cable attenuation gamma for feedforward R*=exp(-gamma*deltaL); <=0 = auto-estimate
 UProperty<double, NNeuronTimeLearner, ptPubParameter> AttenuationGamma;
 
 /// P-regulator gain for parametric R step (0..1], default 0.4
 UProperty<double, NNeuronTimeLearner, ptPubParameter> ResistanceAdjustGain;
 
 /// Current tip ExcSynapse1 resistance per dendrite (parametric mode)
 UProperty<std::vector<double>, NNeuronTimeLearner, ptPubParameter | ptPubState> TipSynapseResistance;
 
 /// Dendrite segment counts
 UProperty<std::vector<int>, NNeuronTimeLearner, ptPubParameter> DendriteLength;
 
 /// Dendrite lengths before the last structural change (for pruning old tip synapses)
 std::vector<int> OldDendriteLength;
 
 /// Reference soma amplitude at length 1
 UProperty<std::vector<double>, NNeuronTimeLearner, ptPubParameter> InitialSomaPotential;
 
 /// Excitatory synapse counts on distal segments
 UProperty<std::vector<int>, NNeuronTimeLearner, ptPubParameter> NumSynapse;
 
 /// Min time after first impulse of previous iteration before next may start (sec) — parameter t
 UProperty<double, NNeuronTimeLearner, ptPubParameter> IterationGap;
 
 /// |dt| below this => dendrite considered synchronized (sec)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> SyncTolerance;
 
 /// Half-width of soma peak search window around Expected[i]+cable delay (sec)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> PeakMeasureMargin;
 
 /// Floor for DelayUseOf agree_margin: max(SyncTolerance, DelayAgreeMarginMin) (sec)
 UProperty<double, NNeuronTimeLearner, ptPubParameter> DelayAgreeMarginMin;
 
 /// 0 - joint train (length+synapses); 1 - legacy unused; 2 - Done
 UProperty<int, NNeuronTimeLearner, ptPubState> TrainingPhase;
 
 /// One-shot reset to initial untrained structure/state
 UProperty<bool, NNeuronTimeLearner, ptPubState> ResetToUntrainedState;
 
 /// Experiment id when ExperimentMode is on (1=PatternRecognition, 2=LearningSecondPattern)
 UProperty<int, NNeuronTimeLearner, ptPubParameter> ExperimentNum;
 
 /// When true, ACalculate runs Experiment() instead of Training()
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> ExperimentMode;
 
 /// Enable detailed DEBUG logging of sync/norm steps
 UProperty<bool, NNeuronTimeLearner, ptPubParameter> EnableDebug;

protected:
 /// Previous NumInputDendrite (resize / rebuild)
 int OldNumInputDendrite;
 
 /// Shared NDatasetMatrix burst source (fans out to tip synapses)
 UEPtr<NDatasetMatrix> Dataset;
 
 /// Trainable NPulseNeuron child
 UEPtr<NPulseNeuron> Neuron;
 
 /// Wall time when the current training iteration started (sec)
 double StartIterTime;
 
 /// Time of first pulse in the current burst (sec)
 double FirstImpulseTime;
 
 /// FirstImpulseTime of the previous burst
 double PrevFirstImpulseTime;
 
 /// True after at least one training iteration completed
 bool HasPrevIteration;
 
 /// Index of the latest pulse within the current burst
 int PulseIndexInIter;
 
 /// Soma channel currently used for peak measurement
 int ActiveMeasureSoma;
 
 /// Previous Dataset pulse counter (edge detect)
 int PrevPulseCounter;
 
 /// Previous generator output sample
 double PrevGenOutput;
 
 /// True while a burst/iteration is in progress
 bool IterationActive;
 
 /// Waiting for soma peak after the last pulse of the burst
 bool WaitingPeakAfterLastPulse;
 
 /// Time of the most recent pulse in the burst (sec)
 double LastPulseTime;
 
 /// Cumulative expected pulse times relative to burst start (sec)
 std::vector<double> ExpectedPulseRelTimes;
 
 /// First local max after pulse i is locked (descending flank seen)
 std::vector<bool> PeakLocked;
 
 /// Peak candidate observed for pulse i (before lock)
 std::vector<bool> PeakSeen;
 
 /// Optional input file stream (file experiments)
 ifstream Fin;
 
 /// Optional output file stream (file experiments)
 ofstream Fout;
 
 /// Buffered input matrix for file recognition
 MDMatrix<double> Inputs;
 
 /// Buffered recognition outputs
 std::vector<int> Outputs;
 
 /// File output header already written
 bool IsWritten;
 
 /// First step of file recognition pass
 bool IsFirstFileStep;
 
 /// Training iteration counter
 int CountIteration;
 
 /// Untrained structure snapshot captured
 bool HasUntrainedSnapshot;
 
 /// First tick of the current iteration
 bool IsFirstBeat;
 
 /// Nominal iteration length (sec)
 double IterLength;
 
 /// Max soma amplitude per channel this iteration
 std::vector<double> MaxIterSomaAmp;
 
 /// Time of MaxIterSomaAmp per channel
 std::vector<double> TimeOfMaxIterSomaAmp;
 
 /// Per-dendrite sync error metric this iteration
 std::vector<double> Dissynchronization;
 
 /// Snapshot of Dissynchronization at the start of FinishTrainingIteration
 std::vector<double> PrevDissynchronization;
 
 /// Amp vs InitialSomaPotential difference per dendrite
 std::vector<double> AmpDifference;
 
 /// True if MaxIterSomaAmp[i] exceeded kMinMeasurableSomaAmp this iteration
 std::vector<bool> SomaPeakValid;
 
 /// TimeOfMax[i] - FirstImpulseTime (current burst)
 std::vector<double> PeakRel;
 
 /// TimeOfMax[i] - (FirstImpulseTime + ExpectedPulseRelTimes[i])
 std::vector<double> DelayFromPulse;
 
 /// Previous-burst PeakRel / Delay / validity (Sync reference snapshot)
 std::vector<double> PrevPeakRel;
 
 /// Previous-burst DelayFromPulse snapshot
 std::vector<double> PrevDelayFromPulse;
 
 /// Previous-burst SomaPeakValid snapshot
 std::vector<bool> PrevPeakValid;
 
 /// PrevPeak* snapshot is valid
 bool HasPrevPeakSnapshot;
 
 /// Index 0..N-2 trained this burst (round-robin)
 int ActiveDendrite;
 
 /// |dt| from last decision per dendrite (AllDendritesSynced / no-improve)
 std::vector<double> DendLastAbsDt;
 
 /// Consecutive no-improve length decisions per dendrite
 std::vector<int> NoImproveCount;
 
 /// Accepted without |dt|≤tol (amp-collapse / no-improve stop)
 std::vector<bool> DendBestEffortSynced;
 
 /// Estimated cable delay per segment (sec); refined from observed |Δdt|/ΔL
 double EstDelayPerSeg;
 
 /// Last length delta applied for EstDelayPerSeg update
 int LastLengthDelta;
 
 int LastLengthDeltaDendrite;
 
 /// Snapshot of DendriteLength for ResetToUntrained
 std::vector<int> UntrainedDendriteLength;
 
 /// Snapshot of NumSynapse for ResetToUntrained
 std::vector<int> UntrainedNumSynapse;
 
 /// Snapshot of InitialSomaPotential for ResetToUntrained
 std::vector<double> UntrainedInitialSomaPotential;
 
 /// Snapshot of TipSynapseResistance for ResetToUntrained
 std::vector<double> UntrainedTipSynapseResistance;
 
 /// Last values passed to setters during XML load (hash order may precede NumInputDendrite).
 std::vector<double> LoadedInitialSomaPotential;
 
 /// TipSynapseResistance captured/reloaded from XML parameters
 std::vector<double> LoadedTipSynapseResistance;
 
 /// LoadedInitialSomaPotential is available
 bool HasLoadedInitialSomaPotential;
 
 /// LoadedTipSynapseResistance is available
 bool HasLoadedTipSynapseResistance;
 
 /// 0 optimal; 1 grow; -1 shrink
 std::vector<int> DendStatus;
 
 /// 0 optimal; 1 add synapses; -1 remove
 std::vector<int> SynapseStatus;
 
 /// Parametric: 0 ok; nonzero = apply pending R change
 std::vector<int> ResistanceStatus;
 
 /// Pending resistance delta per dendrite
 std::vector<double> ResistanceDifference;
 
 /// Previous amp error (parametric regulator)
 std::vector<double> PrevAmpError;
 
 /// Previous R ratio (parametric regulator)
 std::vector<double> PrevResistanceRatio;
 
 /// Consecutive no-improve resistance steps
 std::vector<int> NoImproveResistanceCount;
 
 /// Effective gain used in last R update
 std::vector<double> EffectiveResistanceGain;
 
 static constexpr int kNormStructural = 0;
 
 static constexpr int kNormParametric = 1;
 
 static constexpr double kSynapseResistanceBioDefault = 8.6e7;
 
 static constexpr double kAttenuationGammaAuto = -1.0;
 
 static constexpr double kAttenuationGammaFallback = 0.05;
 
 static constexpr double kMinMeasurableSomaAmp = 1e-6;
 
 static constexpr double kMinSettle = 0.08;
 
 static constexpr double kDelayPerSegDefault = 0.005;
 
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
 
 /// Current experiment epoch counter
 int EpochCur;
 
 /// Gate for applying length changes this iteration
 bool CanChangeDendLength;
 
 /// Sum of max soma amps this iteration
 double SumMaxIterSomaAmp;
 
 /// Sum of max soma amps previous iteration
 double PrevSumMaxIterSomaAmp;
 
 /// LTZ Potential min/max during current training iteration
 double IterMinLTZPotential;
 
 /// Max LTZ potential observed this training iteration
 double IterMaxLTZPotential;
 
 /// LTZ min/max tracking enabled for this iteration
 bool IterLTZTrackingActive;
 
 /// Snapshot from last iteration where AllDendritesSynced()
 double LastSyncedMinLTZ;
 
 /// LTZ max from last fully synced iteration
 double LastSyncedMaxLTZ;
 
 static constexpr int kCalibrateGapFraction = 0;
 
 static constexpr int kCalibratePeakFraction = 1;

public: // Methods
 // --------------------------
 // Constructors and destructors
 // --------------------------
 
 NNeuronTimeLearner(void);
 
 virtual ~NNeuronTimeLearner(void);
 
 // --------------------------
 // Parameter setters
 // --------------------------
 
 /// Set StructureBuildMode (0=no rebuild, 1=BuildStructure in ABuild)
 bool SetStructureBuildMode(const int &value);
 
 /// Set DatasetMatrix / generator class name
 bool SetPulseGeneratorClassName(const std::string &value);
 
 /// Set neuron class name
 bool SetNeuronClassName(const std::string &value);
 
 /// Set synapse class name (reserved)
 bool SetSynapseClassName(const std::string &value);
 
 /// Set calculate mode: 0=auto clear IsNeedToTrain after learning; 1=external control
 bool SetCalculateMode(const int &value);
 
 /// Raise/clear training flag; applies Training/Fixed LTZ threshold
 bool SetIsNeedToTrain(const bool &value);
 
 /// Set DatasetMatrix.Delay (pause between bursts, sec)
 bool SetDelay(const double &value);
 
 /// Compatibility setter (not used as iteration length)
 bool SetSpikesFrequency(const double &value);
 
 /// Set dendrite/pulse count and resize dependent vectors
 bool SetNumInputDendrite(const int &value);
 
 /// Set max cable length (segments)
 bool SetMaxDendriteLength(const int &value);
 
 /// Set ISI / training pattern and sync DatasetMatrix.MatrixData
 bool SetInputPattern(const MDMatrix<double> &value);
 
 /// Set reserved additional pattern for experiments
 bool SetAdditionalInputPattern(const MDMatrix<double> &value);
 
 /// Set previous InputPattern snapshot
 bool SetPrevInputPattern(const MDMatrix<double> &value);
 
 /// Set active LTZone threshold on the neuron
 bool SetLTZThreshold(const double &value);
 
 /// Set LTZ threshold used while training
 bool SetTrainingLTZThreshold(const double &value);
 
 /// Set post-training / fixed LTZ threshold
 bool SetFixedLTZThreshold(const double &value);
 
 /// Prefer FixedLTZThreshold over TrainingLTZThreshold
 bool SetUseFixedLTZThreshold(const bool &value);
 
 /// Auto-calibrate FixedLTZ after EndOfLearning
 bool SetAutoCalibrateFixedLTZThreshold(const bool &value);
 
 /// Calibration mode: 0=gap_fraction, 1=peak_fraction
 bool SetCalibrateLTZThresholdMode(const int &value);
 
 /// Fraction for LTZ calibration
 bool SetCalibrateLTZThresholdFraction(const double &value);
 
 /// Lower clamp for calibrated FixedLTZ
 bool SetCalibrateLTZThresholdMin(const double &value);
 
 /// Upper clamp for calibrated FixedLTZ
 bool SetCalibrateLTZThresholdMax(const double &value);
 
 /// Resistance for extra excitatory synapses (structural mode, k>=2)
 bool SetSynapseResistanceStep(const double &value);
 
 /// 0=structural (grow NumSynapse); 1=parametric (tune tip R)
 bool SetNormalizationMode(const int &value);
 
 /// Reference tip resistance at L==1
 bool SetSynapseResistanceBase(const double &value);
 
 /// Lower bound for tip Resistance (parametric)
 bool SetResistanceMin(const double &value);
 
 /// Upper bound for tip Resistance (parametric)
 bool SetResistanceMax(const double &value);
 
 /// Cable attenuation gamma; <=0 = auto-estimate
 bool SetAttenuationGamma(const double &value);
 
 /// P-regulator gain for parametric R step
 bool SetResistanceAdjustGain(const double &value);
 
 /// Set tip ExcSynapse1 resistance vector (parametric)
 bool SetTipSynapseResistance(const std::vector<double> &value);
 
 /// Enable/disable experiment path in ACalculate
 bool SetExperimentMode(const bool &value);
 
 /// Set dendrite lengths / pulse attach segments; may rebuild cable
 bool SetDendriteLength(const std::vector<int> &value);
 
 /// Set reference soma amplitude at L==1 per channel
 bool SetInitialSomaPotential(const std::vector<double> &value);
 
 /// Set excitatory synapse counts on distal segments
 bool SetNumSynapse(const std::vector<int> &value);
 
 /// Min gap after previous first impulse before next burst (sec)
 bool SetIterationGap(const double &value);
 
 /// |dt| below this => dendrite synchronized (sec)
 bool SetSyncTolerance(const double &value);
 
 /// Half-width of soma peak search window (sec)
 bool SetPeakMeasureMargin(const double &value);
 
 /// Floor for DelayUseOf agree margin (sec)
 bool SetDelayAgreeMarginMin(const double &value);
 
 /// One-shot request to reset to untrained structure/state
 bool SetResetToUntrainedState(const bool &value);
 
 /// Set experiment number (1=PatternRecognition, 2=LearningSecondPattern)
 bool SetExperimentNum(const int &value);
 
 /// Enable DEBUG logging; value stored in property
 bool SetEnableDebug(const bool &value);
 
 // --------------------------
 // System methods
 // --------------------------
 
 /// Allocate a clean copy of this class
 virtual NNeuronTimeLearner* New(void);
 
 /// Factory for UComponent registration
 static UComponent* NewStatic(void);

protected:
 // --------------------------
 // Component management
 // --------------------------
 
 /// Post-actions when a child component was added
 virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);
 
 /// Pre-actions when a child component is removed
 virtual bool ADelComponent(UEPtr<UContainer> comp);
 
 // --------------------------
 // Calculation / training
 // --------------------------
 
 /// Build Dataset + Neuron topology and tip links
 bool BuildStructure();
 
 /// Hook before Build
 virtual bool ABeforeBuild(void);
 
 /// Restore defaults and clear training state
 virtual bool ADefault(void);
 
 /// Build after parameters; may call BuildStructure
 virtual bool ABuild(void);
 
 /// Sources (generators) before Neuron — Dataset calc contract
 void UpdateComputationOrder(void) override;
 
 /// Reset runtime counters / iteration state
 virtual bool AReset(void);
 
 /// Compare two patterns within tolerance e
 bool CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e);
 
 /// Clear learned pattern / length / synapse info
 bool ZeroingTrainingPattern(void);
 
 /// Restore Untrained* snapshots onto live structure
 bool ResetToUntrained(void);
 
 /// Apply DendStatus length change for dendrite/pulse num
 bool ChangeDendriteLength(int num);
 
 /// Grow/shrink pending length (typically one active); synapses applied separately in Finish
 bool ApplyPendingDendriteLengthChanges(void);
 
 /// Apply SynapseStatus count change for dendrite/pulse num
 bool ChangeSynapseNumber(int num);
 
 /// Sample soma max amp/time for the current tick
 bool MeasureMaxPotentialAndTime(void);
 
 /// Decide grow/shrink for dendrite/pulse num
 bool ChangeDendriteStatus(int num);
 
 /// Decide add/remove synapses for dendrite/pulse num
 bool ChangeSynapseStatus(int num);
 
 /// True when NormalizationMode == parametric
 bool IsParametricNormalization(void) const;
 
 /// Clamp R into [ResistanceMin, ResistanceMax]
 double ClampResistance(double r) const;
 
 /// Tip ExcSynapse1 for dendrite/pulse index (0-based)
 NPulseSynapseCommon* GetTipSynapse(int dendrite_index0) const;
 
 /// Write tip Resistance onto the membrane component
 bool SetTipSynapseResistanceOnComponent(int dendrite_index0, double r);
 
 /// Decide pending parametric R change
 bool ChangeSynapseResistanceStatus(int num);
 
 /// Apply pending ResistanceStatus change
 bool ApplySynapseResistanceChange(int num);
 
 /// Model tip R from cable length / attenuation
 double ComputeModelTipResistance(int dendrite_index0) const;
 
 /// Damped P-step tip R from amp error
 double ComputeDampedTipResistance(int dendrite_index0, double r_old,
  double amp, double initial, double dt, double &effective_gain_out) const;
  
 /// Commit computed tip R and update gain traces
 void ApplyComputedResistance(int num, double r_old, double r_new, double effective_gain);
 
 /// Scale tip R on length grow by exp(-gamma*deltaL)
 void FeedforwardResistanceOnLengthGrow(int dendrite_index0, int deltaL);
 
 /// Force NumSynapse=1 in parametric mode
 void EnforceParametricSynapseCount(void);
 
 /// Reload InitialSomaPotential / TipR from XML when degenerate
 void ApplyLoadedAnchorProperties(void);
 
 /// True when every dendrite length is <= 1 (cold / untrained topology).
 bool StructureLooksUntrained(const std::vector<int> &lengths) const;
 
 /// Push TrainingLTZ or FixedLTZ onto LTZThreshold + neuron LTZone.
 void ApplyActiveLtzThreshold(void);
 
 /// Read current LTZone membrane potential
 double ReadLTZonePotential(void) const;
 
 /// Track IterMin/Max LTZ for this iteration
 void UpdateIterLTZPotential(void);
 
 /// Set FixedLTZ from last synced LTZ min/max
 void CalibrateFixedLTZThresholdFromTraining(void);
 
 /// Publish AmpDt / TipR / length traces for watchers
 void UpdateNormTraces(void);
 
 /// Recognition pass over patterns from input file
 bool PatternRecognition(void);
 
 /// Experiment: learn additional pattern
 bool LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern);
 
 /// Experiment: incremental learning path
 bool IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern);
 
 /// Dispatch experiment by ExperimentNum
 bool Experiment(void);
 
 /// Finalize training (clear flag, calibrate LTZ)
 bool EndOfLearning(void);
 
 /// One training step: detect pulses, measure, sync/norm
 bool Training(void);
 
 /// Main calculate tick for the learner
 virtual bool ACalculate(void);
 
 /// Path to shared dataset generator under this net
 static std::string DatasetGeneratorPath(void);
 
 /// Resolve DatasetMatrix child generator
 NPulseGeneratorTransit* GetDatasetGenerator(void);
 
 /// Link one tip synapse Input to Dataset generator Output
 bool LinkSynapseToDataset(NPulseSynapseCommon *synapse);
 
 /// Relink all tip synapses on dendrite/pulse to Dataset
 bool RelinkDendriteSynapsesToDataset(int dendrite_index0);
 
 /// Align Dataset MaxSpikes / dims with NumInputDendrite
 void SyncDatasetDimsFromDendrites(void);
 
 /// Push InputPattern (or override) into DatasetMatrix
 bool SyncInputPatternToDataset(const MDMatrix<double> *pattern_override = 0);
 
 /// Edge-detect a new Dataset pulse
 bool DetectNewImpulse(void);
 
 /// How many new pulses since last check
 int DetectNewImpulseCount(void);
 
 /// Start a training burst/iteration at time now
 void BeginTrainingIteration(double now);
 
 /// Close iteration: decide length/synapse/R updates
 void FinishTrainingIteration(void);
 
 /// True when every dendrite/pulse meets sync criteria
 bool AllDendritesSynced(void) const;
 
 /// True when amp normalization finished for all tips
 bool AllSynapsesNormalized(void) const;
 
 /// Duration of the ISI pattern (sec)
 double PatternSpanSec() const;
 
 /// Extra settle time after last pulse (sec)
 double SettleMarginSec() const;
 
 /// Effective IterationGap including slack (sec)
 double EffectiveIterationGapSec() const;
 
 /// Effective Dataset delay used between bursts (sec)
 double EffectiveDatasetDelaySec() const;
 
 /// Resize PeakRel / Delay / status vectors to n
 void ResizeSyncVectors(int n);
 
 /// Compute PeakRel and DelayFromPulse from measurements
 void ComputePeakRelAndDelay(void);
 
 /// Choose ActiveDendrite / ActivePulseIndex for this burst
 int SelectActiveDendrite() const;
 
 /// Save current PeakRel/Delay as Prev* snapshot
 void CommitPrevPeakSnapshot(void);
 
 /// Refresh DendLastAbsDt from prev-burst anchor
 void RefreshDendLastAbsDtFromPrevAnchor(void);
 
 /// Cable-model delay for dendrite/pulse num (sec)
 double DelayLenOf(int num) const;
 
 /// Prefer DelayFromPulse when it agrees with cable model; else delay_len.
 double DelayUseOf(int num, double expected_k, double delay_meas) const;
};

}

#endif // NNEURONTIMELEARNER_H
