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

#ifndef NNEURONTIMELEARNERBRANCH_CPP
#define NNEURONTIMELEARNERBRANCH_CPP

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include "NNeuronTimeLearnerBranch.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Rdk/Deploy/Include/rdk_cpp_init.h"


namespace NMSDK {

namespace {
std::string MakeBranchSomaName(void)
{
 return std::string("Soma1");
}

std::string MakeBranchDendriteName(const int segment_index1)
{
 return std::string("Dendrite1_") + sntoa(segment_index1);
}

int BranchAttachSegment(const int attach_pos)
{
 return (attach_pos <= 0) ? 1 : attach_pos;
}

void EnsureSingleInputSynapseOnBranchSegments(NPulseNeuron *neuron, const int lmax)
{
 if(!neuron || lmax < 1)
  return;
 for(int seg = 1; seg <= lmax; ++seg)
 {
  UEPtr<NPulseMembrane> membr = neuron->GetComponentL<NPulseMembrane>(
   MakeBranchDendriteName(seg), true);
  if(!membr)
   continue;
  if(int(membr->NumExcitatorySynapses) != 1)
  {
   membr->NumExcitatorySynapses = 1;
   membr->Build();
  }
  if(!membr->GetActivity())
   membr->SetActivity(true);
  if(!membr->IsInit())
   membr->Init();
 }
}

std::string MakeBranchExcSynapsePath(const int attach_pos, const int synapse_index1)
{
 return MakeBranchDendriteName(BranchAttachSegment(attach_pos))
  + std::string(".ExcSynapse") + sntoa(synapse_index1);
}

const int kPhaseSync = 0;
const int kPhaseNormalize = 1;
const int kPhaseDone = 2;
}

std::string NNeuronTimeLearnerBranch::DatasetGeneratorPath(int feature_index)
{
 if(feature_index < 0)
  feature_index = 0;
 return std::string("DatasetMatrix.Generator") + sntoa(feature_index + 1);
}

double NNeuronTimeLearnerBranch::PatternSpanSec() const
{
 if(ExpectedPulseRelTimes.empty())
  return 0.0;
 return ExpectedPulseRelTimes.back();
}

double NNeuronTimeLearnerBranch::SettleMarginSec() const
{
 int max_len = 1;
 for(size_t i = 0; i < DendriteLength.size(); ++i)
 {
  if(DendriteLength[i] > max_len)
   max_len = DendriteLength[i];
 }
 const double from_len = kDelayPerSegDefault * double(max_len);
 return std::max(kMinSettle, from_len);
}

double NNeuronTimeLearnerBranch::EffectiveIterationGapSec() const
{
 const double configured = IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5;
 return std::max(configured, PatternSpanSec() + SettleMarginSec() + kGapSlack);
}

double NNeuronTimeLearnerBranch::EffectiveDatasetDelaySec() const
{
 const double configured = Delay.GetData() > 0.0 ? Delay.GetData() : 0.5;
 return std::max(configured, SettleMarginSec() + kGapSlack);
}

void NNeuronTimeLearnerBranch::ResizeSyncVectors(int n)
{
 if(n < 1)
  n = 1;
 const double large_dt = (IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5) + 0.001;
 PeakRel.assign(static_cast<size_t>(n), 0.0);
 DelayFromPulse.assign(static_cast<size_t>(n), 0.0);
 PrevPeakRel.assign(static_cast<size_t>(n), 0.0);
 PrevDelayFromPulse.assign(static_cast<size_t>(n), 0.0);
 PrevPeakValid.assign(static_cast<size_t>(n), false);
 DendLastAbsDt.assign(static_cast<size_t>(n), large_dt);
 NoImproveCount.assign(static_cast<size_t>(n), 0);
 DendBestEffortSynced.assign(static_cast<size_t>(n), false);
 ActiveDendrite = n - 1;
 ActivePulseIndex = n - 1;
 StoredPeakRel.assign(static_cast<size_t>(n), 0.0);
 PulseSynced.assign(static_cast<size_t>(n), false);
 HasPrevPeakSnapshot = false;
 EstDelayPerSeg = kDelayPerSegDefault;
 LastLengthDelta = 0;
 LastLengthDeltaDendrite = -1;
 if(int(ResistanceStatus.size()) != n)
  ResistanceStatus.assign(static_cast<size_t>(n), 0);
 if(int(ResistanceDifference.size()) != n)
  ResistanceDifference.assign(static_cast<size_t>(n), 0.0);
 if(int(PrevAmpError.size()) != n)
  PrevAmpError.assign(static_cast<size_t>(n), 0.0);
 if(int(PrevResistanceRatio.size()) != n)
  PrevResistanceRatio.assign(static_cast<size_t>(n), 1.0);
 if(int(NoImproveResistanceCount.size()) != n)
  NoImproveResistanceCount.assign(static_cast<size_t>(n), 0);
 if(int(EffectiveResistanceGain.size()) != n)
 {
  const double g = (ResistanceAdjustGain.GetData() > 0.0)
   ? ResistanceAdjustGain.GetData() : kResistanceAdjustGainDefault;
  EffectiveResistanceGain.assign(static_cast<size_t>(n), g);
 }
 if(IsParametricNormalization() && int(TipSynapseResistance.size()) != n)
 {
  TipSynapseResistance.resize(static_cast<size_t>(n), SynapseResistanceBase.GetData());
  TipSynapseResistance.SetDataDirect(TipSynapseResistance.GetData());
 }
}

bool NNeuronTimeLearnerBranch::IsParametricNormalization(void) const
{
 return NormalizationMode.GetData() == kNormParametric;
}

double NNeuronTimeLearnerBranch::ClampResistance(double r) const
{
 const double rmin = ResistanceMin.GetData();
 const double rmax = ResistanceMax.GetData();
 if(r < rmin)
  return rmin;
 if(r > rmax)
  return rmax;
 return r;
}

int NNeuronTimeLearnerBranch::PulseAttachPos(int pulse_k) const
{
 if(pulse_k < 0 || pulse_k >= int(DendriteLength.size()))
  return 1;
 return BranchAttachSegment(DendriteLength[static_cast<size_t>(pulse_k)]);
}

int NNeuronTimeLearnerBranch::SynapseIndexOnMembrane(int pulse_k) const
{
 (void)pulse_k;
 // One excitatory synapse per dendrite segment; active pulse selected by mute.
 return 1;
}

int NNeuronTimeLearnerBranch::ChainLengthMax(void) const
{
 int lmax = 1;
 for(int i = 0; i < NumInputDendrite - 1 && i < int(DendriteLength.size()); ++i)
 {
  if(DendriteLength[static_cast<size_t>(i)] > lmax)
   lmax = DendriteLength[static_cast<size_t>(i)];
 }
 return lmax;
}

NPulseMembrane* NNeuronTimeLearnerBranch::GetInputMembraneForPulse(int pulse_k) const
{
 if(!Neuron)
  return NULL;
 return Neuron->GetComponentL<NPulseMembrane>(
  MakeBranchDendriteName(PulseAttachPos(pulse_k)), true);
}

NPulseSynapseCommon* NNeuronTimeLearnerBranch::GetTipSynapse(int dendrite_index0) const
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return NULL;
 const int pos = PulseAttachPos(dendrite_index0);
 const int syn = SynapseIndexOnMembrane(dendrite_index0);
 return Neuron->GetComponentL<NPulseSynapseCommon>(
  MakeBranchExcSynapsePath(pos, syn), true);
}

double NNeuronTimeLearnerBranch::MeanStoredPeakRel(int after_pulse_exclusive) const
{
 double sum = 0.0;
 int count = 0;
 for(int j = after_pulse_exclusive + 1; j < NumInputDendrite; ++j)
 {
  if(j < int(PulseSynced.size()) && PulseSynced[static_cast<size_t>(j)])
  {
   sum += (j < int(StoredPeakRel.size())) ? StoredPeakRel[static_cast<size_t>(j)] : 0.0;
   ++count;
  }
 }
 if(count <= 0)
  return 0.0;
 return sum / double(count);
}

bool NNeuronTimeLearnerBranch::PulseLengthAndAmpDone(int pulse_k) const
{
 if(pulse_k < 0 || pulse_k >= NumInputDendrite)
  return true;
 if(pulse_k == NumInputDendrite - 1)
  return (pulse_k < int(PulseSynced.size())) && PulseSynced[static_cast<size_t>(pulse_k)];
 const bool synced = (pulse_k < int(PulseSynced.size())) && PulseSynced[static_cast<size_t>(pulse_k)];
 return synced;
}

bool NNeuronTimeLearnerBranch::SetTipSynapseResistanceOnComponent(int dendrite_index0, double r)
{
 if(dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return true;
 r = ClampResistance(r);
 std::vector<double> tips = TipSynapseResistance.GetData();
 if(tips.size() != static_cast<size_t>(NumInputDendrite))
  tips.resize(static_cast<size_t>(NumInputDendrite), SynapseResistanceBase.GetData());
 tips[static_cast<size_t>(dendrite_index0)] = r;
 TipSynapseResistance.SetDataDirect(tips);
 if(NPulseSynapseCommon *synapse = GetTipSynapse(dendrite_index0))
  synapse->Resistance = r;
 return true;
}

void NNeuronTimeLearnerBranch::EnforceParametricSynapseCount(void)
{
 if(!IsParametricNormalization())
  return;
 std::vector<int> ns = NumSynapse.GetData();
 if(ns.size() != static_cast<size_t>(NumInputDendrite))
  ns.resize(static_cast<size_t>(NumInputDendrite), 1);
 for(int i = 0; i < NumInputDendrite; ++i)
  ns[static_cast<size_t>(i)] = 1;
 NumSynapse.SetDataDirect(ns);
 Ready = false;
}

void NNeuronTimeLearnerBranch::UpdateNormTraces(void)
{
 const int n = std::max(1, NumInputDendrite.GetData());

 MDMatrix<double> amp_dt;
 amp_dt.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
 {
  const double initial = (i < int(InitialSomaPotential.size())) ? InitialSomaPotential[i] : 0.0;
  const double amp = (i < int(MaxIterSomaAmp.size())) ? MaxIterSomaAmp[i] : 0.0;
  amp_dt(0, i) = initial - amp;
 }
 AmpDtTrace = amp_dt;

 MDMatrix<double> tips;
 tips.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  tips(0, i) = (i < int(TipSynapseResistance.size())) ? TipSynapseResistance[i] : 0.0;
 TipSynapseResistanceTrace = tips;

 MDMatrix<double> res_st;
 res_st.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  res_st(0, i) = (i < int(ResistanceStatus.size())) ? double(ResistanceStatus[i]) : 0.0;
 ResistanceStatusTrace = res_st;

 MDMatrix<double> no_imp;
 no_imp.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  no_imp(0, i) = (i < int(NoImproveResistanceCount.size()))
   ? double(NoImproveResistanceCount[i]) : 0.0;
 NoImproveResistanceTrace = no_imp;

 MDMatrix<double> gain;
 gain.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  gain(0, i) = (i < int(EffectiveResistanceGain.size()))
   ? EffectiveResistanceGain[i] : 0.0;
 EffectiveGainTrace = gain;

 MDMatrix<double> lens;
 lens.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  lens(0, i) = (i < int(DendriteLength.size())) ? double(DendriteLength[i]) : 0.0;
 DendriteLengthTrace = lens;

 MDMatrix<double> last_dt;
 last_dt.Assign(1, n, 0.0);
 for(int i = 0; i < n; ++i)
  last_dt(0, i) = (i < int(DendLastAbsDt.size())) ? DendLastAbsDt[i] : 0.0;
 LastAbsDtTrace = last_dt;

 MDMatrix<double> iter_m;
 iter_m.Assign(1, 1, double(CountIteration));
 StimulusIterTrace = iter_m;
}

bool NNeuronTimeLearnerBranch::StructureLooksUntrained(const std::vector<int> &lengths) const
{
 // Cold: all pulses on the first dendrite segment (pos<=1); no soma inputs.
 if(lengths.empty())
  return true;
 for(size_t i = 0; i < lengths.size(); ++i)
 {
  if(lengths[i] > 1)
   return false;
 }
 return true;
}

void NNeuronTimeLearnerBranch::ApplyActiveLtzThreshold(void)
{
 // Parameters often ship LTZThreshold=Fixed (recognition) together with
 // IsNeedToTrain=1. During training soma peaks (~0.012–0.03) sit above Fixed
 // (~0.0115) and the neuron spikes continuously unless TrainingLTZ is applied.
 const double thr = IsNeedToTrain.GetData()
  ? TrainingLTZThreshold.GetData()
  : FixedLTZThreshold.GetData();
 LTZThreshold.SetDataDirect(thr);
 SetLTZThreshold(thr);
}

double NNeuronTimeLearnerBranch::ReadLTZonePotential(void) const
{
 if(!Neuron)
  return 0.0;
 UEPtr<NLTZone> ltzone = Neuron->GetComponentL<NLTZone>(std::string("LTZone"), true);
 if(!ltzone)
  return 0.0;
 return ltzone->Potential.GetData();
}

void NNeuronTimeLearnerBranch::UpdateIterLTZPotential(void)
{
 if(!IsNeedToTrain.GetData() || !IterationActive)
  return;

 const double ltz = ReadLTZonePotential();
 if(ltz <= 0.0)
  return;

 if(!IterLTZTrackingActive)
 {
  IterMinLTZPotential = ltz;
  IterMaxLTZPotential = ltz;
  IterLTZTrackingActive = true;
 }
 else
 {
  IterMinLTZPotential = std::min(IterMinLTZPotential, ltz);
  IterMaxLTZPotential = std::max(IterMaxLTZPotential, ltz);
 }
}

void NNeuronTimeLearnerBranch::CalibrateFixedLTZThresholdFromTraining(void)
{
 if(!AutoCalibrateFixedLTZThreshold.GetData())
  return;

 const double min_ltz = LastSyncedMinLTZ;
 const double max_ltz = LastSyncedMaxLTZ;
 if(max_ltz <= min_ltz + 1e-9)
 {
  if(EnableDebug.GetData() && RDK::GetLogger())
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch",
    "CalibrateFixedLTZ: skip (no valid min/max LTZ snapshot)");
  return;
 }

 double thr;
 if(CalibrateLTZThresholdMode.GetData() == kCalibratePeakFraction)
  thr = max_ltz * CalibrateLTZThresholdFraction.GetData();
 else
  thr = min_ltz + CalibrateLTZThresholdFraction.GetData() * (max_ltz - min_ltz);

 const double tmin = CalibrateLTZThresholdMin.GetData();
 const double tmax = CalibrateLTZThresholdMax.GetData();
 thr = std::max(tmin, std::min(tmax, thr));

 FixedLTZThreshold.SetDataDirect(thr);
 CalibratedFixedLTZThreshold.SetDataDirect(thr);
 UseFixedLTZThreshold = true;
 SetLTZThreshold(thr);

 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "CalibrateFixedLTZ: mode=" << CalibrateLTZThresholdMode.GetData()
      << " min=" << min_ltz << " max=" << max_ltz
      << " fraction=" << CalibrateLTZThresholdFraction.GetData()
      << " thr=" << thr;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
}

void NNeuronTimeLearnerBranch::ApplyLoadedAnchorProperties(void)
{
 // Restore InitialSomaPotential / TipSynapseResistance only when they are empty or
 // degenerate. Do NOT re-apply Loaded* on every call — that undoes ResetToUntrained
 // (cold TipR=base, cleared Initial) and pulls trained tips from Parameters_00.xml.
 const int n = NumInputDendrite.GetData();
 if(n < 1)
  return;

 auto vector_all_zero = [](const std::vector<double> &v, int count) {
  for(int i = 0; i < count; ++i)
  {
   if(i < int(v.size()) && v[static_cast<size_t>(i)] > 0.0)
    return false;
  }
  return true;
 };

 auto parse_simplevector_text = [](const std::string &text, int size_hint) {
  std::vector<double> out;
  if(size_hint > 0)
   out.resize(static_cast<size_t>(size_hint), 0.0);
  const char *start = text.c_str();
  const char *end = start + text.size();
  for(int i = 0; i < size_hint && start < end; ++i)
  {
   while(start < end && (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r'))
    ++start;
   if(start >= end)
    break;
   const char *next = start;
   while(next < end && *next != ' ' && *next != '\t' && *next != '\n' && *next != '\r')
    ++next;
   out[static_cast<size_t>(i)] = RDK::atof(std::string(start, next));
   start = next;
  }
  return out;
 };

 auto reload_vector_property = [&](const char *tag, int size_hint, std::vector<double> &dest,
                                 bool &has_dest) {
  std::vector<std::string> paths;
  if(GetEnvironment())
   paths.push_back(GetEnvironment()->GetCurrentDataDir() + "Parameters_00.xml");
  paths.push_back("Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearner/Parameters_00.xml");
  paths.push_back("Parameters_00.xml");
  if(GetEnvironment())
  {
   paths.push_back(GetEnvironment()->GetCurrentDataDir() + "Model_00.xml");
   paths.push_back(GetEnvironment()->GetCurrentDataDir() + "Parameters.xml");
  }
  for(size_t pi = 0; pi < paths.size(); ++pi)
  {
   std::ifstream f(paths[pi].c_str());
   if(!f)
    continue;
   std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
   const std::string stag(tag);
   const size_t pos = content.find("<" + stag);
   if(pos == std::string::npos)
    continue;
   const size_t gt = content.find('>', pos);
   const size_t lt = content.find('<', gt + 1);
   if(gt == std::string::npos || lt == std::string::npos || lt <= gt)
    continue;
   int hint = size_hint;
   const size_t size_attr = content.find("Size=\"", pos);
   if(size_attr != std::string::npos && size_attr < gt)
    hint = std::max(hint, std::atoi(content.c_str() + size_attr + 6));
   dest = parse_simplevector_text(content.substr(gt + 1, lt - gt - 1), hint);
   has_dest = !dest.empty();
   return has_dest;
  }
  return false;
 };

 const std::vector<double> cur_initial = InitialSomaPotential.GetData();
 if(vector_all_zero(cur_initial, n))
 {
  // Reload from Parameters when the in-memory vector is still default zeros
  // (ABuild often runs before InitialSomaPotential XML is applied).
  (void)reload_vector_property("InitialSomaPotential", n, LoadedInitialSomaPotential,
                               HasLoadedInitialSomaPotential);
  // Cold ResetToUntrained must keep zeros so L=1 can recapture amp. Trained
  // cables (any L>1) need the file anchors or continue-train never reaches
  // AllSynapsesNormalized (EXP01 GUI continue: initial=[0,0,0,ref]).
  const bool trained_cables = !StructureLooksUntrained(DendriteLength.GetData());
  if(trained_cables && HasLoadedInitialSomaPotential && !LoadedInitialSomaPotential.empty()
     && !ResetToUntrainedState.GetData())
  {
   const int target_n = std::max(n, int(LoadedInitialSomaPotential.size()));
   std::vector<double> normalized(static_cast<size_t>(target_n), 0.0);
   const size_t copyCount = std::min(normalized.size(), LoadedInitialSomaPotential.size());
   for(size_t i = 0; i < copyCount; ++i)
    normalized[i] = LoadedInitialSomaPotential[i];
   InitialSomaPotential.SetDataDirect(normalized);
  }
 }

 const std::vector<double> cur_tips = TipSynapseResistance.GetData();
 const double rmin = ResistanceMin.GetData();
 bool tips_need_reload = false;
 for(int i = 0; i < n; ++i)
 {
  if(i >= int(cur_tips.size()) || cur_tips[static_cast<size_t>(i)] <= 0.0
     || cur_tips[static_cast<size_t>(i)] <= rmin * (1.0 + 1e-6))
  {
   tips_need_reload = true;
   break;
  }
 }
 if(tips_need_reload)
 {
  if(reload_vector_property("TipSynapseResistance", n, LoadedTipSynapseResistance,
                            HasLoadedTipSynapseResistance)
     && HasLoadedTipSynapseResistance && !LoadedTipSynapseResistance.empty())
  {
   const int target_n = std::max(n, int(LoadedTipSynapseResistance.size()));
   std::vector<double> normalized(static_cast<size_t>(target_n), SynapseResistanceBase.GetData());
   const size_t copyCount = std::min(normalized.size(), LoadedTipSynapseResistance.size());
   for(size_t i = 0; i < copyCount; ++i)
    normalized[i] = ClampResistance(LoadedTipSynapseResistance[i]);
   TipSynapseResistance.SetDataDirect(normalized);
   if(Neuron && IsParametricNormalization())
   {
    const int apply_n = std::min(n, int(normalized.size()));
    for(int i = 0; i < apply_n; ++i)
     SetTipSynapseResistanceOnComponent(i, normalized[static_cast<size_t>(i)]);
   }
  }
 }
}

double NNeuronTimeLearnerBranch::ComputeModelTipResistance(int dendrite_index0) const
{
 double r = SynapseResistanceBase.GetData();
 if(r <= 0.0)
  r = kSynapseResistanceBioDefault;
 if(dendrite_index0 >= 0 && dendrite_index0 < int(DendriteLength.size())
    && DendriteLength[static_cast<size_t>(dendrite_index0)] > 1)
 {
  double gamma = AttenuationGamma.GetData();
  if(gamma <= 0.0)
   gamma = kAttenuationGammaFallback;
  r *= std::exp(-gamma * double(DendriteLength[static_cast<size_t>(dendrite_index0)] - 1));
 }
 return ClampResistance(r);
}

double NNeuronTimeLearnerBranch::ComputeDampedTipResistance(int dendrite_index0, double r_old,
 double amp, double initial, double dt, double &effective_gain_out) const
{
 effective_gain_out = ResistanceAdjustGain.GetData();
 if(effective_gain_out <= 0.0)
  effective_gain_out = kResistanceAdjustGainDefault;

 if(r_old <= 0.0 || initial <= 0.0 || amp <= kMinMeasurableSomaAmp)
  return ClampResistance(r_old);

 const double target_ratio = amp / initial;

 if(dendrite_index0 >= 0 && dendrite_index0 < int(PrevAmpError.size())
    && fabs(PrevAmpError[static_cast<size_t>(dendrite_index0)]) > 1e-12)
 {
  const double prev_e = PrevAmpError[static_cast<size_t>(dendrite_index0)];
  if(prev_e * dt < 0.0)
   effective_gain_out *= kGainOvershootFactor;
  else if(fabs(prev_e) > kUndershootBoostRatio * fabs(dt) && prev_e * dt > 0.0)
   effective_gain_out *= kGainUndershootFactor;
 }
 effective_gain_out = std::max(0.05, std::min(1.0, effective_gain_out));

 double step_ratio = 1.0 + effective_gain_out * (target_ratio - 1.0);
 if(step_ratio <= 0.0)
  step_ratio = 0.05;

 double r_new = r_old * step_ratio;

 // r_model is a feedforward orientation, not a hard floor for feedback: when amp
 // is still below Initial, allow R below the cable model down to ResistanceMin
 // so long dendrites are not permanently stuck on the model floor.
 if(amp < initial)
 {
  const double r_model = ComputeModelTipResistance(dendrite_index0);
  if(r_new < r_model && EnableDebug.GetData() && RDK::GetLogger())
  {
   std::ostringstream oss;
   oss << "ResistanceFloor: dend=" << dendrite_index0
       << " r_old=" << r_old << " r_model=" << r_model
       << " r_step=" << r_new << " ampDt=" << (initial - amp)
       << " (allowing below model toward Rmin)";
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }
 }

 return ClampResistance(r_new);
}

void NNeuronTimeLearnerBranch::ApplyComputedResistance(int num, double r_old, double r_new,
 double effective_gain)
{
 if(num < int(TipSynapseResistance.size()))
 {
  std::vector<double> tips = TipSynapseResistance.GetData();
  tips[static_cast<size_t>(num)] = r_new;
  TipSynapseResistance.SetDataDirect(tips);
 }
 const double rel_change = (r_old > 0.0) ? fabs(r_new - r_old) / r_old : 0.0;
 ResistanceStatus[static_cast<size_t>(num)] =
  (rel_change >= kResistanceSettleRatio) ? 1 : 0;
 if(num < int(EffectiveResistanceGain.size()))
  EffectiveResistanceGain[static_cast<size_t>(num)] = effective_gain;
 if(num < int(PrevResistanceRatio.size()) && r_old > 0.0)
  PrevResistanceRatio[static_cast<size_t>(num)] = r_new / r_old;
 if(EnableDebug.GetData() && RDK::GetLogger() && ResistanceStatus[static_cast<size_t>(num)])
 {
  std::ostringstream oss;
  oss << "DampedResistance: dend=" << num << " gain=" << effective_gain
      << " R " << r_old << " -> " << r_new;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
}

void NNeuronTimeLearnerBranch::FeedforwardResistanceOnLengthGrow(int dendrite_index0, int deltaL)
{
 if(!IsParametricNormalization() || deltaL <= 0)
  return;
 if(dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return;

 double gamma = AttenuationGamma.GetData();
 if(gamma <= 0.0)
  gamma = kAttenuationGammaFallback;

 double r = SynapseResistanceBase.GetData();
 if(dendrite_index0 < int(TipSynapseResistance.size()))
  r = TipSynapseResistance[static_cast<size_t>(dendrite_index0)];
 if(r <= 0.0)
  r = SynapseResistanceBase.GetData();

 r *= std::exp(-gamma * double(deltaL));
 SetTipSynapseResistanceOnComponent(dendrite_index0, r);
 if(dendrite_index0 < int(ResistanceStatus.size()))
  ResistanceStatus[static_cast<size_t>(dendrite_index0)] = 0;
 if(dendrite_index0 < int(ResistanceDifference.size())
    && dendrite_index0 < int(InitialSomaPotential.size())
    && dendrite_index0 < int(MaxIterSomaAmp.size()))
 {
  ResistanceDifference[static_cast<size_t>(dendrite_index0)] =
   InitialSomaPotential[static_cast<size_t>(dendrite_index0)]
   - MaxIterSomaAmp[static_cast<size_t>(dendrite_index0)];
 }

 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "FeedforwardResistance: dend=" << dendrite_index0
      << " deltaL=" << deltaL << " gamma=" << gamma << " R=" << r;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
}

bool NNeuronTimeLearnerBranch::ChangeSynapseResistanceStatus(int num)
{
 const int dendstatus = DendStatus[num];

 if(num == NumInputDendrite - 1)
 {
  ResistanceStatus[num] = 0;
  SynapseStatus[num] = 0;
  DendStatus[num] = dendstatus;
  return true;
 }

 if(num >= int(InitialSomaPotential.size()) || InitialSomaPotential[num] <= 0.0)
 {
  ResistanceStatus[num] = 0;
  ResistanceDifference[num] = 0.0;
  SynapseStatus[num] = 0;
  DendStatus[num] = dendstatus;
  return true;
 }

 const bool length_settled = (num < int(DendLastAbsDt.size())
  && DendLastAbsDt[static_cast<size_t>(num)] <= SyncTolerance.GetData())
  || ((num < int(DendBestEffortSynced.size()))
      && DendBestEffortSynced[static_cast<size_t>(num)]);
 if(!DendStatus[num] && length_settled
    && (num < int(MaxIterSomaAmp.size()))
    && (MaxIterSomaAmp[num] < kMinMeasurableSomaAmp))
 {
  ResistanceStatus[num] = 0;
  SynapseStatus[num] = 0;
  ResistanceDifference[num] = InitialSomaPotential[num] - MaxIterSomaAmp[num];
  DendStatus[num] = dendstatus;
  return true;
 }

 double dt = InitialSomaPotential[num] - MaxIterSomaAmp[num];
 const double eps = kAmpNormEps;

 // Auto-estimate cable attenuation gamma from observed amp/Initial at L>1.
 if(MaxIterSomaAmp[num] > kMinMeasurableSomaAmp && InitialSomaPotential[num] > 0.0
    && num < int(DendriteLength.size()) && DendriteLength[num] > 1)
 {
  const double ratio = MaxIterSomaAmp[num] / InitialSomaPotential[num];
  if(ratio > 1e-6 && ratio < 1.0)
  {
   const double gamma_obs = -std::log(ratio) / double(DendriteLength[num] - 1);
   if(gamma_obs > 1e-4 && gamma_obs < 1.0)
   {
    const double cur_gamma = AttenuationGamma.GetData();
    if(cur_gamma <= 0.0)
     AttenuationGamma = gamma_obs;
    else
     AttenuationGamma = 0.7 * cur_gamma + 0.3 * gamma_obs;
   }
  }
 }

 double r_old = SynapseResistanceBase.GetData();
 if(num < int(TipSynapseResistance.size()))
  r_old = TipSynapseResistance[static_cast<size_t>(num)];
 if(r_old <= 0.0)
  r_old = SynapseResistanceBase.GetData();

 const bool same_pattern = (fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001);
 const double amp = MaxIterSomaAmp[num];
 const double initial = InitialSomaPotential[num];
 const bool ready_for_r_tune = length_settled && !DendStatus[num];

 if(!ready_for_r_tune)
 {
  ResistanceStatus[num] = (fabs(dt) > eps) ? 1 : 0;
 }
 else if(same_pattern && (fabs(dt) <= eps))
 {
  ResistanceStatus[num] = 0;
 }
 // Do not stop on partial ampDt improvement: keep damped-P until |dt|<=eps
 // (or no-improve / Rmin escapes). The old "dt got smaller" settle left dend0
 // stuck with ampDt~1e-3 after a single R step.
 else if(fabs(dt) > eps)
 {
  double eff_gain = kResistanceAdjustGainDefault;
  double r_new = r_old;
  if(amp > kMinMeasurableSomaAmp && initial > 0.0)
   r_new = ComputeDampedTipResistance(num, r_old, amp, initial, dt, eff_gain);
  else
  {
   eff_gain = (ResistanceAdjustGain.GetData() > 0.0)
    ? ResistanceAdjustGain.GetData() : kResistanceAdjustGainDefault;
   if(dt > 0.0)
    r_new = ClampResistance(r_old * (1.0 - 0.15 * eff_gain));
   else
    r_new = ClampResistance(r_old * (1.0 + 0.15 * eff_gain));
  }

  // Near the target, damped-P steps shrink below settle ratio and ampDt can
  // freeze just above eps (seen: dend2 stuck at ~6e-6). Force a minimal R step.
  if(r_old > 0.0 && fabs(r_new - r_old) < kResistanceSettleRatio * r_old)
  {
   if(dt > 0.0)
    r_new = ClampResistance(r_old * (1.0 - kResistanceSettleRatio));
   else
    r_new = ClampResistance(r_old * (1.0 + kResistanceSettleRatio));
  }

  const double prev_res_dt = ResistanceDifference[num];
  ApplyComputedResistance(num, r_old, r_new, eff_gain);
  // Tiny ΔR near the target still needs to reach the tip synapse; settle-ratio
  // alone left ResistanceStatus=0 and froze ampDt just above eps.
  if(fabs(dt) > eps)
   ResistanceStatus[num] = 1;

  if(num < int(NoImproveResistanceCount.size()))
  {
   // Near eps, |dt| 1.16e-5↔1.30e-5 used to reset the counter every other
   // iter (PSI k=0.5 GUI). Far from target, a slow crawl (0.07→0.065) must
   // NOT freeze R — that blocked k=2 after coincident pulses raised amp.
   const bool reached_eps = (fabs(dt) <= eps);
   const bool near_target = (fabs(dt) <= kAmpOscillationBand)
    || (fabs(prev_res_dt) <= kAmpOscillationBand);
   const bool meaningful_drop = (fabs(prev_res_dt) > eps)
    && (fabs(dt) < 0.5 * fabs(prev_res_dt));
   const bool any_drop = (fabs(dt) < fabs(prev_res_dt));
   if(reached_eps || meaningful_drop || (!near_target && any_drop))
    NoImproveResistanceCount[static_cast<size_t>(num)] = 0;
   else if(ResistanceStatus[num])
    NoImproveResistanceCount[static_cast<size_t>(num)]++;
   if(NoImproveResistanceCount[static_cast<size_t>(num)] >= kNoImproveResistanceLimit)
    ResistanceStatus[num] = 0;
  }

  if(num < int(PrevAmpError.size()))
   PrevAmpError[static_cast<size_t>(num)] = dt;
 }
 else
  ResistanceStatus[num] = 0;

 ResistanceDifference[num] = dt;
 SynapseStatus[num] = 0;
 DendStatus[num] = dendstatus;
 return true;
}

bool NNeuronTimeLearnerBranch::ApplySynapseResistanceChange(int num)
{
 if(num < 0 || num >= int(ResistanceStatus.size()) || !ResistanceStatus[num])
  return true;
 if(!Neuron)
  return true;
 double r = SynapseResistanceBase.GetData();
 if(num < int(TipSynapseResistance.size()))
  r = TipSynapseResistance[static_cast<size_t>(num)];
 SetTipSynapseResistanceOnComponent(num, r);
 ResistanceStatus[num] = 0;
 Neuron->Reset();
 return true;
}

void NNeuronTimeLearnerBranch::ComputePeakRelAndDelay(void)
{
 const int n = NumInputDendrite.GetData();
 if(int(PeakRel.size()) != n)
  PeakRel.assign(static_cast<size_t>(n), 0.0);
 if(int(DelayFromPulse.size()) != n)
  DelayFromPulse.assign(static_cast<size_t>(n), 0.0);

 for(int i = 0; i < n; ++i)
 {
  if(i >= int(SomaPeakValid.size()) || !SomaPeakValid[i]
     || i >= int(TimeOfMaxIterSomaAmp.size()))
  {
   PeakRel[static_cast<size_t>(i)] = 0.0;
   DelayFromPulse[static_cast<size_t>(i)] = 0.0;
   continue;
  }
  const double t_pulse_rel = (i < int(ExpectedPulseRelTimes.size()))
   ? ExpectedPulseRelTimes[static_cast<size_t>(i)] : 0.0;
  PeakRel[static_cast<size_t>(i)] = TimeOfMaxIterSomaAmp[static_cast<size_t>(i)] - FirstImpulseTime;
  DelayFromPulse[static_cast<size_t>(i)] =
   TimeOfMaxIterSomaAmp[static_cast<size_t>(i)] - (FirstImpulseTime + t_pulse_rel);
 }
}

int NNeuronTimeLearnerBranch::SelectActiveDendrite() const
{
 const int n = NumInputDendrite.GetData();
 if(n < 1)
  return 0;
 if(n == 1)
  return 0;
 // Reverse sequential: first unsynced pulse from the end (anchor N-1 down to 0).
 for(int k = n - 1; k >= 0; --k)
 {
  if(k >= int(PulseSynced.size()) || !PulseSynced[static_cast<size_t>(k)])
   return k;
 }
 return 0;
}

void NNeuronTimeLearnerBranch::CommitPrevPeakSnapshot(void)
{
 const int n = NumInputDendrite.GetData();
 if(int(PrevPeakRel.size()) != n)
  PrevPeakRel.assign(static_cast<size_t>(n), 0.0);
 if(int(PrevDelayFromPulse.size()) != n)
  PrevDelayFromPulse.assign(static_cast<size_t>(n), 0.0);
 if(int(PrevPeakValid.size()) != n)
  PrevPeakValid.assign(static_cast<size_t>(n), false);

 for(int i = 0; i < n; ++i)
 {
  const bool valid = (i < int(SomaPeakValid.size())) && SomaPeakValid[static_cast<size_t>(i)];
  PrevPeakValid[static_cast<size_t>(i)] = valid;
  PrevPeakRel[static_cast<size_t>(i)] =
   (i < int(PeakRel.size())) ? PeakRel[static_cast<size_t>(i)] : 0.0;
  PrevDelayFromPulse[static_cast<size_t>(i)] =
   (i < int(DelayFromPulse.size())) ? DelayFromPulse[static_cast<size_t>(i)] : 0.0;
 }
 // Empty bursts must not unlock length training (synthetic growth on amp=0).
 const int ref = n - 1;
 HasPrevPeakSnapshot = (ref >= 0)
  && ((ref < int(PulseSynced.size()) && PulseSynced[static_cast<size_t>(ref)])
      || ((ref < int(PrevPeakValid.size()) && PrevPeakValid[static_cast<size_t>(ref)])));
}

void NNeuronTimeLearnerBranch::RefreshDendLastAbsDtFromPrevAnchor(void)
{
 const int n = NumInputDendrite.GetData();
 const int ref = n - 1;
 if(n < 2)
  return;
 const bool anchor_ok = (ref < int(PulseSynced.size()) && PulseSynced[static_cast<size_t>(ref)])
  || HasPrevPeakSnapshot;
 if(!anchor_ok)
  return;
 if(int(DendLastAbsDt.size()) != n)
  DendLastAbsDt.assign(static_cast<size_t>(n), IterationGap.GetData() + 0.001);

 const double tol = SyncTolerance.GetData();
 for(int i = 0; i < n - 1; ++i)
 {
  const double expected_k = (i < int(ExpectedPulseRelTimes.size()))
   ? ExpectedPulseRelTimes[static_cast<size_t>(i)] : 0.0;
  const double t_star = MeanStoredPeakRel(i);
  const double needed = t_star - expected_k;
  const double delay_meas = (i < int(DelayFromPulse.size()))
   ? DelayFromPulse[static_cast<size_t>(i)] : 0.0;
  const bool peak_valid = (i < int(SomaPeakValid.size())) && SomaPeakValid[static_cast<size_t>(i)];
  const double delay_use = peak_valid
   ? DelayUseOf(i, expected_k, delay_meas)
   : DelayLenOf(i);
  const double abs_dt = fabs(needed - delay_use);
  DendLastAbsDt[static_cast<size_t>(i)] = abs_dt;
  if(!peak_valid && abs_dt <= tol && i < int(DendBestEffortSynced.size()))
   DendBestEffortSynced[static_cast<size_t>(i)] = true;
 }
}

double NNeuronTimeLearnerBranch::DelayLenOf(int num) const
{
 if(num < 0 || num >= int(DendriteLength.size()) || DendriteLength[num] <= 1)
  return 0.0;
 return (DendriteLength[num] - 1) * EstDelayPerSeg;
}

double NNeuronTimeLearnerBranch::DelayUseOf(int num, double expected_k, double delay_meas) const
{
 const double delay_len = DelayLenOf(num);
 // Tight margin: loose PeakMeasureMargin-scale agree allowed PeakRel-aligned
 // delay_meas to "agree" while delay_len still disagreed (L1 stopped early).
 const double agree_margin = std::max(SyncTolerance.GetData(), DelayAgreeMarginMin.GetData());
 const bool peak_valid = (num < int(SomaPeakValid.size())) && SomaPeakValid[static_cast<size_t>(num)];
 const double peak_meas = (num < int(PeakRel.size())) ? PeakRel[static_cast<size_t>(num)] : 0.0;
 const double peak_model = expected_k + delay_len;
 const bool meas_agrees = peak_valid
  && (fabs(delay_meas - delay_len) <= agree_margin)
  && (fabs(peak_meas - peak_model) <= agree_margin);
 return meas_agrees ? delay_meas : delay_len;
}

NPulseGeneratorTransit* NNeuronTimeLearnerBranch::GetDatasetGenerator(void)
{
 if(!Dataset)
  return 0;
 if(!Dataset->Generators.empty() && Dataset->Generators[0])
  return Dataset->Generators[0];
 return Dataset->GetComponentL<NPulseGeneratorTransit>("Generator1", true).Get();
}

bool NNeuronTimeLearnerBranch::LinkSynapseToDataset(NPulseSynapseCommon *synapse)
{
 if(!synapse)
  return false;
 const std::string gen_path = DatasetGeneratorPath();
 synapse->Input.DetachFrom();
 synapse->DisconnectAllItems();
 const bool ok = CreateLink(gen_path, "Output", synapse->GetLongName(this), "Input", -1, true);
 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "LinkSynapseToDataset: " << gen_path << ".Output -> "
      << synapse->GetLongName(this) << ".Input ok=" << (ok ? 1 : 0)
      << " connected=" << (synapse->Input.IsConnected() ? 1 : 0);
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 return ok;
}

void NNeuronTimeLearnerBranch::DetachBranchExcSynapseAtSegment(int segment_index1)
{
 if(!Neuron || segment_index1 < 1)
  return;
 const std::string syn_rel = MakeBranchExcSynapsePath(segment_index1, 1);
 const std::string syn_path = std::string("Neuron.") + syn_rel;
 BreakLink(DatasetGeneratorPath(), "Output", syn_path, "Input", -1);
 if(NPulseSynapseCommon *syn = Neuron->GetComponentL<NPulseSynapseCommon>(syn_rel, true))
 {
  syn->Input.DetachFrom();
  syn->DisconnectAllItems();
 }
}

void NNeuronTimeLearnerBranch::DisconnectGeneratorFromBranchExcSynapses(void)
{
 if(!Neuron)
  return;
 int lmax = ChainLengthMax();
 const int max_cfg = MaxDendriteLength.GetData();
 if(max_cfg > lmax)
  lmax = max_cfg;
 for(int i = 0; i < NumInputDendrite - 1 && i < int(DendriteLength.size()); ++i)
 {
  if(DendriteLength[static_cast<size_t>(i)] > lmax)
   lmax = DendriteLength[static_cast<size_t>(i)];
 }
 for(int i = 0; i < NumInputDendrite - 1 && i < int(OldDendriteLength.size()); ++i)
 {
  if(OldDendriteLength[static_cast<size_t>(i)] > lmax)
   lmax = OldDendriteLength[static_cast<size_t>(i)];
 }
 for(int seg = 1; seg <= lmax; ++seg)
  DetachBranchExcSynapseAtSegment(seg);
}

int NNeuronTimeLearnerBranch::CountGeneratorToBranchExcSynapseLinks(void) const
{
 if(!Neuron)
  return 0;
 int lmax = ChainLengthMax();
 const int max_cfg = MaxDendriteLength.GetData();
 if(max_cfg > lmax)
  lmax = max_cfg;
 for(int i = 0; i < NumInputDendrite && i < int(DendriteLength.size()); ++i)
 {
  if(DendriteLength[static_cast<size_t>(i)] > lmax)
   lmax = DendriteLength[static_cast<size_t>(i)];
 }
 int count = 0;
 for(int seg = 1; seg <= lmax; ++seg)
 {
  if(NPulseSynapseCommon *syn = Neuron->GetComponentL<NPulseSynapseCommon>(
       MakeBranchExcSynapsePath(seg, 1), true))
  {
   if(syn->Input.IsConnected())
    ++count;
  }
 }
 return count;
}

void NNeuronTimeLearnerBranch::RebuildGeneratorSynapseLinks(bool all_on, int active_pulse)
{
 if(!Dataset)
  return;
 DisconnectGeneratorFromBranchExcSynapses();
 const int n = NumInputDendrite.GetData();
 if(all_on)
 {
  for(int f = 0; f < n; ++f)
  {
   if(NPulseSynapseCommon *syn = GetTipSynapse(f))
    LinkSynapseToDataset(syn);
  }
 }
 else if(active_pulse >= 0 && active_pulse < n)
 {
  if(NPulseSynapseCommon *syn = GetTipSynapse(active_pulse))
   LinkSynapseToDataset(syn);
 }
 if(Neuron)
  Neuron->InvalidateActiveComponentsCache();
}

void NNeuronTimeLearnerBranch::EnforceSegmentMonotonicity(int changed_pulse)
{
 const int n = NumInputDendrite.GetData();
 if(n < 2)
  return;
 const int ref = n - 1;
 for(int k = 0; k < n - 1; ++k)
 {
  if(changed_pulse >= 0 && k != changed_pulse)
   continue;
  if(k >= int(DendStatus.size()) || k >= int(DendriteLength.size()))
   continue;
  if(ref < int(DendriteLength.size()) && DendriteLength[static_cast<size_t>(k)]
     <= DendriteLength[static_cast<size_t>(ref)])
   DendStatus[static_cast<size_t>(k)] = 1;
  if(k + 1 < n && k + 1 < int(DendriteLength.size())
     && DendriteLength[static_cast<size_t>(k)] <= DendriteLength[static_cast<size_t>(k + 1)])
   DendStatus[static_cast<size_t>(k)] = 1;
  for(int j = 0; j < n; ++j)
  {
   if(j == k || j >= int(DendriteLength.size()))
    continue;
   if(DendriteLength[static_cast<size_t>(k)] == DendriteLength[static_cast<size_t>(j)]
      && k < j)
    DendStatus[static_cast<size_t>(k)] = 1;
  }
 }
}

void NNeuronTimeLearnerBranch::ApplyPulseGeneratorMute(void)
{
 if(!Dataset)
  return;
 const int n = NumInputDendrite.GetData();
 const bool all_on = (!IsNeedToTrain.GetData()) || (TrainingPhase.GetData() == kPhaseDone);
 const int active = ActivePulseIndex;
 RebuildGeneratorSynapseLinks(all_on, active);
 const int link_count = CountGeneratorToBranchExcSynapseLinks();
 const int expect = all_on ? n : ((active >= 0 && active < n) ? 1 : 0);
 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "ApplyPulseGeneratorMute: active=" << active << " all_on=" << (all_on ? 1 : 0)
      << " enable=[";
  for(int f = 0; f < n; ++f)
  {
   if(f) oss << ',';
   oss << ((all_on || (active >= 0 && f == active)) ? 1 : 0);
  }
  NPulseSynapseCommon *active_syn = (active >= 0 && active < n) ? GetTipSynapse(active) : NULL;
  oss << "] connected=" << ((active_syn && active_syn->Input.IsConnected()) ? 1 : 0)
      << " gen_links=" << link_count << " expect=" << expect;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 if(link_count != expect && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "ApplyPulseGeneratorMute: link count mismatch got=" << link_count
      << " expect=" << expect << " all_on=" << (all_on ? 1 : 0);
  RDK::GetLogger()->LogMessageEx(RDK_EX_WARNING, "NNeuronTimeLearnerBranch", oss.str());
 }
}

bool NNeuronTimeLearnerBranch::RelinkDendriteSynapsesToDataset(int dendrite_index0)
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return true;

 const int tip_seg = PulseAttachPos(dendrite_index0);

 UEPtr<NPulseMembrane> membr = Neuron->GetComponentL<NPulseMembrane>(
  MakeBranchDendriteName(tip_seg), true);
 if(!membr)
  return false;

 if(int(membr->NumExcitatorySynapses) != 1)
 {
  membr->NumExcitatorySynapses = 1;
  membr->Build();
 }
 if(!membr->GetActivity())
  membr->SetActivity(true);
 if(!membr->IsInit())
  membr->Init();

 NPulseSynapseCommon *synapse = membr->GetComponentL<NPulseSynapseCommon>(
  std::string("ExcSynapse1"), true);
 if(!synapse)
  return false;
 if(IsParametricNormalization())
 {
  double r = SynapseResistanceBase.GetData();
  if(dendrite_index0 < int(TipSynapseResistance.size()))
   r = TipSynapseResistance[static_cast<size_t>(dendrite_index0)];
  if(r <= 0.0)
   r = kSynapseResistanceBioDefault;
  synapse->Resistance = ClampResistance(r);
 }
 else if(NumSynapse[dendrite_index0] > 1)
  synapse->Resistance = SynapseResistanceStep;

 const bool wire_now = (!IsNeedToTrain.GetData()) || (TrainingPhase.GetData() == kPhaseDone);
 if(wire_now)
 {
  if(!LinkSynapseToDataset(synapse))
   return false;
  if(!synapse->Input.IsConnected())
   return false;
 }
 return true;
}

void NNeuronTimeLearnerBranch::SyncDatasetDimsFromDendrites(void)
{
 if(!Dataset)
  return;
 if(!IsNeedToTrain.GetData())
  return;
 Dataset->PulseGeneratorClassName = PulseGeneratorClassName;
 if(Dataset->NumFeatures != 1)
  Dataset->SetNumFeatures(1);
 if(Dataset->NumSamples != 1)
  Dataset->SetNumSamples(1);
 if(Dataset->MaxSpikesPerFeature != NumInputDendrite)
  Dataset->SetMaxSpikesPerFeature(NumInputDendrite);
 Dataset->Delay = EffectiveDatasetDelaySec();
 Dataset->AdvanceSampleAfterBurst = false;
 Dataset->StateGeneration = 2;
}

bool NNeuronTimeLearnerBranch::SyncInputPatternToDataset(const MDMatrix<double> *pattern_override)
{
 if(!Dataset)
  return true;
 if(!IsNeedToTrain.GetData())
  return true;

 SyncDatasetDimsFromDendrites();

 MDMatrix<double> matrix = pattern_override ? *pattern_override : InputPattern.GetData();
 const int n = NumInputDendrite.GetData();
 if(matrix.GetRows() != n || matrix.GetCols() != 1)
  matrix.Resize(n, 1, 0.0);

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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 return true;
}


NNeuronTimeLearnerBranch::NNeuronTimeLearnerBranch(void):
 StructureBuildMode("StructureBuildMode",this,&NNeuronTimeLearnerBranch::SetStructureBuildMode),
 PulseGeneratorClassName("PulseGeneratorClassName",this,&NNeuronTimeLearnerBranch::SetPulseGeneratorClassName),
 NeuronClassName("NeuronClassName",this,&NNeuronTimeLearnerBranch::SetNeuronClassName),
 SynapseClassName("SynapseClassName",this,&NNeuronTimeLearnerBranch::SetSynapseClassName),
 CalculateMode("CalculateMode",this, &NNeuronTimeLearnerBranch::SetCalculateMode),
 IsNeedToTrain("IsNeedToTrain",this,&NNeuronTimeLearnerBranch::SetIsNeedToTrain),
 Delay("Delay",this,&NNeuronTimeLearnerBranch::SetDelay),
 SpikesFrequency("SpikesFrequency",this,&NNeuronTimeLearnerBranch::SetSpikesFrequency),
 NumInputDendrite("NumInputDendrite",this,&NNeuronTimeLearnerBranch::SetNumInputDendrite),
 MaxDendriteLength("MaxDendriteLength",this,&NNeuronTimeLearnerBranch::SetMaxDendriteLength),
 InputPattern("InputPattern",this,&NNeuronTimeLearnerBranch::SetInputPattern),
 AdditionalInputPattern("AdditionalInputPattern",this,&NNeuronTimeLearnerBranch::SetAdditionalInputPattern),
 DendriteNeuronAmplitude("DendriteNeuronAmplitude",this),
 SomaNeuronAmplitude("SomaNeuronAmplitude",this),
 LTZThreshold("LTZThreshold",this,&NNeuronTimeLearnerBranch::SetLTZThreshold),
 FixedLTZThreshold("FixedLTZThreshold",this,&NNeuronTimeLearnerBranch::SetFixedLTZThreshold),
 TrainingLTZThreshold("TrainingLTZThreshold",this,&NNeuronTimeLearnerBranch::SetTrainingLTZThreshold),
 UseFixedLTZThreshold("UseFixedLTZThreshold",this,&NNeuronTimeLearnerBranch::SetUseFixedLTZThreshold),
 AutoCalibrateFixedLTZThreshold("AutoCalibrateFixedLTZThreshold",this,&NNeuronTimeLearnerBranch::SetAutoCalibrateFixedLTZThreshold),
 CalibrateLTZThresholdMode("CalibrateLTZThresholdMode",this,&NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMode),
 CalibrateLTZThresholdFraction("CalibrateLTZThresholdFraction",this,&NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdFraction),
 CalibrateLTZThresholdMin("CalibrateLTZThresholdMin",this,&NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMin),
 CalibrateLTZThresholdMax("CalibrateLTZThresholdMax",this,&NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMax),
 CalibratedFixedLTZThreshold("CalibratedFixedLTZThreshold",this),
 Output("Output",this),
 AmpDtTrace("AmpDtTrace",this),
 TipSynapseResistanceTrace("TipSynapseResistanceTrace",this),
 ResistanceStatusTrace("ResistanceStatusTrace",this),
 NoImproveResistanceTrace("NoImproveResistanceTrace",this),
 EffectiveGainTrace("EffectiveGainTrace",this),
 DendriteLengthTrace("DendriteLengthTrace",this),
 LastAbsDtTrace("LastAbsDtTrace",this),
 StimulusIterTrace("StimulusIterTrace",this),
 SynapseResistanceStep("SynapseResistanceStep", this, &NNeuronTimeLearnerBranch::SetSynapseResistanceStep),
 NormalizationMode("NormalizationMode", this, &NNeuronTimeLearnerBranch::SetNormalizationMode),
 SynapseResistanceBase("SynapseResistanceBase", this, &NNeuronTimeLearnerBranch::SetSynapseResistanceBase),
 ResistanceMin("ResistanceMin", this, &NNeuronTimeLearnerBranch::SetResistanceMin),
 ResistanceMax("ResistanceMax", this, &NNeuronTimeLearnerBranch::SetResistanceMax),
 AttenuationGamma("AttenuationGamma", this, &NNeuronTimeLearnerBranch::SetAttenuationGamma),
 ResistanceAdjustGain("ResistanceAdjustGain", this, &NNeuronTimeLearnerBranch::SetResistanceAdjustGain),
 TipSynapseResistance("TipSynapseResistance", this, &NNeuronTimeLearnerBranch::SetTipSynapseResistance),
 DendriteLength("DendriteLength", this, &NNeuronTimeLearnerBranch::SetDendriteLength),
 InitialSomaPotential("InitialSomaPotential", this, &NNeuronTimeLearnerBranch::SetInitialSomaPotential),
 NumSynapse("NumSynapse", this, &NNeuronTimeLearnerBranch::SetNumSynapse),
 IterationGap("IterationGap", this, &NNeuronTimeLearnerBranch::SetIterationGap),
 SyncTolerance("SyncTolerance", this, &NNeuronTimeLearnerBranch::SetSyncTolerance),
 PeakMeasureMargin("PeakMeasureMargin", this, &NNeuronTimeLearnerBranch::SetPeakMeasureMargin),
 DelayAgreeMarginMin("DelayAgreeMarginMin", this, &NNeuronTimeLearnerBranch::SetDelayAgreeMarginMin),
 TrainingPhase("TrainingPhase", this),
 ResetToUntrainedState("ResetToUntrainedState", this, &NNeuronTimeLearnerBranch::SetResetToUntrainedState),
 ExperimentNum("ExperimentNum", this, &NNeuronTimeLearnerBranch::SetExperimentNum),
 ExperimentMode("ExperimentMode", this, &NNeuronTimeLearnerBranch::SetExperimentMode),
 EnableDebug("EnableDebug", this, &NNeuronTimeLearnerBranch::SetEnableDebug)
{
 OldNumInputDendrite = 0;
 InitialSomaPotential.SetCheckEquals(false);
 TipSynapseResistance.SetCheckEquals(false);
 Dataset = NULL;
 Neuron = NULL;
 IsFirstFileStep = true;
 HasUntrainedSnapshot = false;
 HasLoadedInitialSomaPotential = false;
 HasLoadedTipSynapseResistance = false;
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
 HasPrevPeakSnapshot = false;
 ActiveDendrite = 0;
 ActivePulseIndex = 0;
 EstDelayPerSeg = kDelayPerSegDefault;
 LastLengthDelta = 0;
 LastLengthDeltaDendrite = -1;
 IterMinLTZPotential = std::numeric_limits<double>::max();
 IterMaxLTZPotential = 0.0;
 IterLTZTrackingActive = false;
 LastSyncedMinLTZ = 0.0;
 LastSyncedMaxLTZ = 0.0;
}


NNeuronTimeLearnerBranch::~NNeuronTimeLearnerBranch(void)
{
 OldNumInputDendrite = 0;
}

bool NNeuronTimeLearnerBranch::SetStructureBuildMode(const int &value)
{
 if(value > 0)
  Ready = false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetPulseGeneratorClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetNeuronClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetSynapseClassName(const std::string &value)
{
 Ready = false;
 return true;
}

bool NNeuronTimeLearnerBranch::ZeroingTrainingPattern(void)
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

bool NNeuronTimeLearnerBranch::ResetToUntrained(void)
{
 // Prefer a cold (L<=1) snapshot. StructTrain Parameters ship trained lengths
 // (49 41 25 1) + Initial — never restore that as "untrained".
 const bool snapshot_ok =
  HasUntrainedSnapshot && StructureLooksUntrained(UntrainedDendriteLength);

 if(snapshot_ok)
 {
  // SetDataDirect: avoid SetDendriteLength clearing HasUntrainedSnapshot.
  DendriteLength.SetDataDirect(UntrainedDendriteLength);
  NumSynapse.SetDataDirect(UntrainedNumSynapse);
  InitialSomaPotential.SetDataDirect(UntrainedInitialSomaPotential);
  if(UntrainedTipSynapseResistance.size() == static_cast<size_t>(NumInputDendrite))
   TipSynapseResistance.SetDataDirect(UntrainedTipSynapseResistance);
 }
 else
 {
  HasUntrainedSnapshot = false;
  DendriteLength.assign(NumInputDendrite, 1);
  if(NumInputDendrite.GetData() >= 1)
   DendriteLength[NumInputDendrite.GetData() - 1] = 0;
  NumSynapse.assign(NumInputDendrite, 1);
  InitialSomaPotential.assign(NumInputDendrite, 0.0);
  TipSynapseResistance.assign(NumInputDendrite, SynapseResistanceBase.GetData());
 }
 // Prefer setter path for LTZ; SetDataDirect(true) alone left FixedLTZ on the
 // membrane and the neuron fired through the whole training run.
 TrainingPhase.SetDataDirect(kPhaseSync);
 IsNeedToTrain.SetDataDirect(true);
 ApplyActiveLtzThreshold();
 OldDendriteLength = DendriteLength.GetData();
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);
 ResistanceStatus.assign(NumInputDendrite, 0);
 ResistanceDifference.assign(NumInputDendrite, 0.0);
 PrevAmpError.assign(NumInputDendrite, 0.0);
 PrevResistanceRatio.assign(NumInputDendrite, 1.0);
 NoImproveResistanceCount.assign(NumInputDendrite, 0);
 EffectiveResistanceGain.assign(NumInputDendrite, kResistanceAdjustGainDefault);
 AttenuationGamma = kAttenuationGammaAuto;
 if(IsParametricNormalization())
  EnforceParametricSynapseCount();
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 const double period = IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5;
 Dissynchronization.assign(NumInputDendrite, period);
 if(!Dissynchronization.empty())
  Dissynchronization[NumInputDendrite - 1] = 0.0;
 PrevDissynchronization = Dissynchronization;
 AmpDifference.assign(NumInputDendrite, 0.0);
 SomaPeakValid.assign(NumInputDendrite, false);
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
 ResizeSyncVectors(NumInputDendrite.GetData());
 CanChangeDendLength = true;

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

bool NNeuronTimeLearnerBranch::SetLTZThreshold(const double &value)
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

bool NNeuronTimeLearnerBranch::SetIsNeedToTrain(const bool &value)
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
  CanChangeDendLength = true;
  HasPrevPeakSnapshot = false;
  ResizeSyncVectors(NumInputDendrite.GetData());
  SetLTZThreshold(TrainingLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(TrainingLTZThreshold.GetData());
  ApplyPulseGeneratorMute();
 }
 else
 {
  TrainingPhase = kPhaseDone;
  SetLTZThreshold(FixedLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(FixedLTZThreshold.GetData());
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetCalculateMode(const int &value)
{
 if((value != 0) && (value != 1))
  return false;
 CountIteration = 0;
 IsFirstBeat = true;
 return true;
}

bool NNeuronTimeLearnerBranch::SetDelay(const double &value)
{
 if(Dataset)
  Dataset->Delay = std::max(value > 0.0 ? value : 0.5, SettleMarginSec() + kGapSlack);
 return true;
}

bool NNeuronTimeLearnerBranch::SetSpikesFrequency(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetNumInputDendrite(const int &value)
{
 if(value < 1)
  return false;
 Ready = false;
 OldNumInputDendrite = NumInputDendrite;
 ApplyLoadedAnchorProperties();
 return true;
}

bool NNeuronTimeLearnerBranch::SetMaxDendriteLength(const int &value)
{
 if(value < 1)
  return false;

 const int n = std::min(NumInputDendrite.GetData(), int(DendriteLength.size()));
 for (int i = 0; i < n; i++)
 {
  if(i == n - 1)
   continue;
  if (DendriteLength[i] > value)
   DendriteLength[i] = value;
 }
 if(Neuron)
 {
  std::vector<int> chain(1, ChainLengthMax());
  Neuron->NumDendriteMembranePartsVec = chain;
  Neuron->StructureBuildMode = 2;
  Neuron->Reset();
  for(int i = 0; i < NumInputDendrite; ++i)
   RelinkDendriteSynapsesToDataset(i);
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetInputPattern(const MDMatrix<double> &value)
{
 if(Dataset && !SyncInputPatternToDataset(&value))
  return false;
 IsFirstBeat = true;
 CountIteration = 0;
 IterationActive = false;
 HasPrevIteration = false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetAdditionalInputPattern(const MDMatrix<double> &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetPrevInputPattern(const MDMatrix<double> &value)
{
 PrevInputPattern = value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetTrainingLTZThreshold(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetFixedLTZThreshold(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetUseFixedLTZThreshold(const bool &value)
{
 if(value)
 {
  SetLTZThreshold(FixedLTZThreshold.GetData());
  LTZThreshold.SetDataDirect(FixedLTZThreshold.GetData());
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetAutoCalibrateFixedLTZThreshold(const bool &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMode(const int &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdFraction(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMin(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetCalibrateLTZThresholdMax(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetSynapseResistanceStep(const double &value)
{
 if(IsParametricNormalization())
  return true;
 if (NumSynapse.empty() || !Neuron)
  return true;

 for(int numdend = 1; numdend <= NumInputDendrite; numdend++)
 {
  for(int numsyn = 2; numsyn <= NumSynapse[numdend - 1]; numsyn++)
  {
   UEPtr<NPulseSynapse> synapse = Neuron->GetComponentL<NPulseSynapse>(
    MakeBranchExcSynapsePath(PulseAttachPos(numdend - 1), numsyn), true);
   if(!synapse)
    continue;
   synapse->Resistance = value;
  }
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetNormalizationMode(const int &value)
{
 if(value != kNormStructural && value != kNormParametric)
  return false;
 if(value == kNormParametric && Neuron)
 {
  std::vector<double> tips = TipSynapseResistance.GetData();
  if(tips.size() != static_cast<size_t>(NumInputDendrite))
   tips.resize(static_cast<size_t>(NumInputDendrite), SynapseResistanceBase.GetData());
  int synced = 0;
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(NPulseSynapseCommon *syn = GetTipSynapse(i))
   {
    tips[static_cast<size_t>(i)] = syn->Resistance;
    synced++;
   }
  }
  // Only adopt neuron tips when every dendrite resolves; partial sync leaves zeros
  // and breaks mid-train Console reload before BuildStructure rebuilds the neuron.
  if(synced == NumInputDendrite)
  {
   bool all_positive = true;
   for(int i = 0; i < NumInputDendrite; ++i)
   {
    if(tips[static_cast<size_t>(i)] <= 0.0)
    {
     all_positive = false;
     break;
    }
   }
   if(all_positive)
    TipSynapseResistance.SetDataDirect(tips);
  }
  EnforceParametricSynapseCount();
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetSynapseResistanceBase(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetResistanceMin(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetResistanceMax(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetAttenuationGamma(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetResistanceAdjustGain(const double &value)
{
 if(value <= 0.0 || value > 1.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetTipSynapseResistance(const std::vector<double> &value)
{
 LoadedTipSynapseResistance = value;
 HasLoadedTipSynapseResistance = true;
 const int target_n = std::max(NumInputDendrite.GetData(), int(value.size()));
 std::vector<double> normalized(static_cast<size_t>(target_n), SynapseResistanceBase.GetData());
 const size_t copyCount = std::min(normalized.size(), value.size());
 for(size_t i = 0; i < copyCount; ++i)
  normalized[i] = ClampResistance(value[i]);
 TipSynapseResistance.SetDataDirect(normalized);
 if(Neuron && IsParametricNormalization())
 {
  const int apply_n = std::min(NumInputDendrite.GetData(), int(normalized.size()));
  for(int i = 0; i < apply_n; ++i)
   SetTipSynapseResistanceOnComponent(i, normalized[static_cast<size_t>(i)]);
 }
 return true;
}

bool NNeuronTimeLearnerBranch::SetExperimentMode(const bool &value)
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

bool NNeuronTimeLearnerBranch::SetDendriteLength(const std::vector<int> &value)
{
 (void)value;
 // Parameters may override Model after an early snapshot; allow re-snapshot.
 HasUntrainedSnapshot = false;
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

bool NNeuronTimeLearnerBranch::SetInitialSomaPotential(const std::vector<double> &value)
{
 LoadedInitialSomaPotential = value;
 HasLoadedInitialSomaPotential = true;
 const int target_n = std::max(NumInputDendrite.GetData(), int(value.size()));
 std::vector<double> normalized(static_cast<size_t>(target_n), 0.0);
 const size_t copyCount = std::min(normalized.size(), value.size());
 for(size_t i = 0; i < copyCount; ++i)
  normalized[i] = value[i];
 InitialSomaPotential.SetDataDirect(normalized);
 return true;
}

bool NNeuronTimeLearnerBranch::SetNumSynapse(const std::vector<int> &value)
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

bool NNeuronTimeLearnerBranch::SetIterationGap(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetSyncTolerance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetPeakMeasureMargin(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetDelayAgreeMarginMin(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearnerBranch::SetResetToUntrainedState(const bool &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearnerBranch::SetExperimentNum(const int &value)
{
 (void)value;
 CalculateMode.SetDataDirect(1);
 SetCalculateMode(1);
 IsFirstFileStep = true;
 return true;
}

bool NNeuronTimeLearnerBranch::SetEnableDebug(const bool & /*value*/)
{
 return true;
}

NNeuronTimeLearnerBranch* NNeuronTimeLearnerBranch::New(void)
{
 return new NNeuronTimeLearnerBranch;
}

UComponent* NNeuronTimeLearnerBranch::NewStatic(void)
{
 return new NNeuronTimeLearnerBranch;
}

bool NNeuronTimeLearnerBranch::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
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

bool NNeuronTimeLearnerBranch::ADelComponent(UEPtr<UContainer> comp)
{
 (void)comp;
 return true;
}

bool NNeuronTimeLearnerBranch::ABeforeBuild(void)
{
 if(!Dataset)
  Dataset = GetComponentL<NDatasetMatrix>(std::string("DatasetMatrix"), true);
 if(Dataset)
  SyncInputPatternToDataset();
 return true;
}

bool NNeuronTimeLearnerBranch::BuildStructure()
{
 bool res(true);
 try
 {
 ApplyLoadedAnchorProperties();

  if (EnableDebug.GetData() && RDK::GetLogger() && Dataset)
  {
   std::ostringstream oss;
   oss << "BuildStructure begin: DatasetMatrix dims samples=" << int(Dataset->NumSamples)
       << " max_spikes=" << int(Dataset->MaxSpikesPerFeature)
       << " rows=" << Dataset->MatrixData.GetRows()
       << " cols=" << Dataset->MatrixData.GetCols();
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }

 Neuron = GetComponentL<NPulseNeuron>(std::string("Neuron"), true);
 if(Neuron)
 {
  // Drop stale neuron copied from class prototype / previous save so cable and
  // generator links are rebuilt for current DendriteLength.
  DelComponent(std::string("Neuron"));
  Neuron = NULL;
 }
 Neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron"), NeuronClassName);
 Neuron->SetCoord(MVector<double,3>(8.7 + 1 * 7, 1.67, 0));
 Neuron->StructureBuildMode = 2;
 Neuron->NumSomaMembraneParts = 1;

 if (DendriteLength.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (DendriteLength.size() < static_cast<size_t>(NumInputDendrite))
   DendriteLength.resize(NumInputDendrite, 1);
  else
   DendriteLength.resize(NumInputDendrite);
 }
 for(int i = 0; i < NumInputDendrite; ++i)
 {
  if(DendriteLength[i] < 1)
   DendriteLength[i] = 1;
 }

 if (OldDendriteLength.size() != static_cast<size_t>(NumInputDendrite))
 {
  if (OldDendriteLength.size() < static_cast<size_t>(NumInputDendrite))
   OldDendriteLength.resize(NumInputDendrite, 1);
  else
   OldDendriteLength.resize(NumInputDendrite);
 }

 {
  std::vector<int> chain(1, ChainLengthMax());
  Neuron->NumDendriteMembranePartsVec = chain;
 }
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "BuildStructure: Neuron->Build failed after length sync");
  return false;
 }
 Neuron->Reset();

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BuildStructure: PulseAttach=[";
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(i) oss << ',';
   oss << DendriteLength[i];
  }
  oss << "] Lmax=" << ChainLengthMax()
      << " NeuronClass=" << NeuronClassName.GetData();
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }

 // Remove legacy per-dendrite Sources from copied Learner layouts
 for(int i = 0; i < std::max(OldNumInputDendrite, NumInputDendrite.GetData()) + 4; i++)
 {
  DelComponent(std::string("Source") + sntoa(i + 1));
 }

 Dataset = AddMissingComponent<NDatasetMatrix>(std::string("DatasetMatrix"), "NDatasetMatrix");
 if(!Dataset)
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "BuildStructure: failed to create DatasetMatrix (is NDatasetMatrix registered?)");
  return false;
 }
 Dataset->SetCoord(MVector<double,3>(6.7, 1.67, 0));
 if(InputPattern.GetRows() != NumInputDendrite || InputPattern.GetCols() != 1)
  InputPattern.Resize(NumInputDendrite, 1, 0.0);
 if(!SyncInputPatternToDataset())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "BuildStructure: failed to sync InputPattern into DatasetMatrix");
  return false;
 }
 Dataset->Build();
 Dataset->Reset();
 if(!SyncInputPatternToDataset())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "BuildStructure: failed to sync InputPattern into DatasetMatrix");
  return false;
 }

 if (NumInputDendrite != int(NumSynapse.size()))
  NumSynapse.resize(NumInputDendrite, 1);
 if(IsParametricNormalization())
  EnforceParametricSynapseCount();

 // One ExcSynapse1 per dendrite segment (active pulse wired by mute).
 EnsureSingleInputSynapseOnBranchSegments(Neuron, ChainLengthMax());

 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 // Keep non-synapse fan-out (e.g. PatternResponseAnalyzer stimulus tap).
 // RelinkDendriteSynapsesToDataset() already detaches the target synapse input
 // before creating the fresh Generator1 -> ExcSynapse link.

 for(int pulse_k = 0; pulse_k < NumInputDendrite; pulse_k++)
 {
  bool linked = RelinkDendriteSynapsesToDataset(pulse_k);
  if(!linked)
  {
   if(UEPtr<NPulseMembrane> tip = GetInputMembraneForPulse(pulse_k))
   {
    if(!tip->GetActivity())
     tip->SetActivity(true);
    if(!tip->IsInit())
     tip->Init();
    linked = RelinkDendriteSynapsesToDataset(pulse_k);
   }
  }
  if(!linked)
   res = false;
 }

 for(int f = 0; f < NumInputDendrite; ++f)
 {
  if(NPulseGeneratorTransit *g = GetDatasetGenerator())
   g->Reset();
 }

 // Cold-start wake-pass: rebuild single cable then relink all pulse synapses.
 // Preserve external listeners on Generator1.Output across the wake-pass.
 {
  std::vector<int> chain(1, ChainLengthMax());
  Neuron->NumDendriteMembranePartsVec = chain;
 }
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "BuildStructure: Neuron->Build wake-pass failed");
  return false;
 }
 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();
 for(int pulse_k = 0; pulse_k < NumInputDendrite; pulse_k++)
 {
  if(!RelinkDendriteSynapsesToDataset(pulse_k))
   res = false;
  if(IsParametricNormalization())
   SetTipSynapseResistanceOnComponent(pulse_k, TipSynapseResistance[pulse_k]);
 }
 for(int f = 0; f < NumInputDendrite; ++f)
 {
  if(NPulseGeneratorTransit *g = GetDatasetGenerator())
   g->Reset();
 }
 ApplyPulseGeneratorMute();

 Neuron->Reset();

 IsFirstBeat = true;
 DendriteNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 SomaNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);

 {
  std::vector<double> cur = InitialSomaPotential.GetData();
  if(cur.size() != static_cast<size_t>(NumInputDendrite))
   cur.resize(static_cast<size_t>(NumInputDendrite), 0.0);
  if(HasUntrainedSnapshot
     && UntrainedInitialSomaPotential.size() == static_cast<size_t>(NumInputDendrite))
  {
   for(int i = 0; i < NumInputDendrite; ++i)
   {
    if(cur[static_cast<size_t>(i)] <= 0.0
       && UntrainedInitialSomaPotential[static_cast<size_t>(i)] > 0.0)
     cur[static_cast<size_t>(i)] = UntrainedInitialSomaPotential[static_cast<size_t>(i)];
   }
  }
  InitialSomaPotential.SetDataDirect(cur);
 }

 if(IsParametricNormalization())
 {
  std::vector<double> tips = TipSynapseResistance.GetData();
  if(tips.size() != static_cast<size_t>(NumInputDendrite))
   tips.resize(static_cast<size_t>(NumInputDendrite), SynapseResistanceBase.GetData());
  if(HasUntrainedSnapshot
     && UntrainedTipSynapseResistance.size() == static_cast<size_t>(NumInputDendrite))
  {
   for(int i = 0; i < NumInputDendrite; ++i)
   {
    if(tips[static_cast<size_t>(i)] <= 0.0
       && UntrainedTipSynapseResistance[static_cast<size_t>(i)] > 0.0)
     tips[static_cast<size_t>(i)] = UntrainedTipSynapseResistance[static_cast<size_t>(i)];
   }
   TipSynapseResistance.SetDataDirect(tips);
  }
 }

 {
  // First successful cold anchors only (all L<=1). Never snapshot trained lengths.
  if(!HasUntrainedSnapshot && StructureLooksUntrained(DendriteLength.GetData()))
  {
   bool any_initial = false;
   for(int i = 0; i < NumInputDendrite; ++i)
   {
    if(i < int(InitialSomaPotential.size()) && InitialSomaPotential[i] > 0.0)
    {
     any_initial = true;
     break;
    }
   }
   if(any_initial)
   {
    UntrainedDendriteLength = DendriteLength.GetData();
    UntrainedNumSynapse = NumSynapse.GetData();
    UntrainedInitialSomaPotential = InitialSomaPotential.GetData();
    UntrainedTipSynapseResistance = TipSynapseResistance.GetData();
    HasUntrainedSnapshot = true;
   }
  }
 }

 TimeOfMaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 MaxIterSomaAmp.assign(NumInputDendrite, 0.0);
 double period = IterationGap > 0.0 ? IterationGap.GetData() : 0.5;
 Dissynchronization.assign(NumInputDendrite, period);
 Dissynchronization[NumInputDendrite - 1] = 0.0;
 PrevDissynchronization = Dissynchronization;
 AmpDifference.assign(NumInputDendrite, 0.0);
 SomaPeakValid.assign(NumInputDendrite, false);
 ResizeSyncVectors(NumInputDendrite.GetData());

 InputPattern.Resize(NumInputDendrite, 1, 0.0);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);
 AdditionalInputPattern.Resize(NumInputDendrite, 1, 0.0);

 TrainingPhase = IsNeedToTrain ? kPhaseSync : kPhaseDone;
 if(IsNeedToTrain.GetData())
  CanChangeDendLength = true;
 ApplyActiveLtzThreshold();
 IterationActive = false;
 HasPrevIteration = false;
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 if(NPulseGeneratorTransit *gen = GetDatasetGenerator())
  PrevPulseCounter = gen->PulseCounter;

 OldNumInputDendrite = NumInputDendrite;
 ApplyLoadedAnchorProperties();
 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BuildStructure anchors: Initial=[";
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(i) oss << ',';
   oss << ((i < int(InitialSomaPotential.size())) ? InitialSomaPotential[i] : -1.0);
  }
  oss << "] tips=[";
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(i) oss << ',';
   oss << ((i < int(TipSynapseResistance.size())) ? TipSynapseResistance[i] : -1.0);
  }
  oss << "]";
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 InvalidateActiveComponentsCache();
 if(Dataset && !Dataset->IsInit())
  Dataset->Init();
 if(Neuron && !Neuron->IsInit())
  Neuron->Init();
 if (EnableDebug.GetData() && RDK::GetLogger() && Dataset)
 {
  std::ostringstream oss;
  oss << "BuildStructure end: DatasetMatrix dims samples=" << int(Dataset->NumSamples)
      << " max_spikes=" << int(Dataset->MaxSpikesPerFeature)
      << " rows=" << Dataset->MatrixData.GetRows()
      << " cols=" << Dataset->MatrixData.GetCols();
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 return res;
 }
 catch (const UException &ex)
 {
  try
  {
   if (RDK::GetLogger())
   {
    std::ostringstream oss;
    oss << "NNeuronTimeLearnerBranch::BuildStructure: UException number=" << ex.GetNumber()
        << ", type=" << ex.GetType()
        << ", file=" << ex.GetExFileName()
        << ", line=" << ex.GetExLineNumber()
        << ", what=" << ex.what();
    RDK::GetLogger()->LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch", oss.str(), ex.GetNumber());
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
    oss << "NNeuronTimeLearnerBranch::BuildStructure: std::exception what=" << ex.what();
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
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, "NNeuronTimeLearnerBranch::BuildStructure: unknown exception");
  }
  catch (...) {}
  throw;
 }
}


bool NNeuronTimeLearnerBranch::ADefault(void)
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
 // ~1 model step (DefaultTimeStep often 2e-3 s); 1e-6 never matched discrete peaks
 SyncTolerance = 0.02;
 PeakMeasureMargin = 0.06;
 DelayAgreeMarginMin = 0.03;
 TrainingPhase = kPhaseSync;
 ResetToUntrainedState = false;
 HasUntrainedSnapshot = false;

 LTZThreshold = 100;
 FixedLTZThreshold = 0.0115;
 TrainingLTZThreshold = 100;
 UseFixedLTZThreshold = false;
 AutoCalibrateFixedLTZThreshold = false;
 CalibrateLTZThresholdMode = kCalibrateGapFraction;
 CalibrateLTZThresholdFraction = 0.85;
 CalibrateLTZThresholdMin = 0.0115;
 CalibrateLTZThresholdMax = 0.05;
 CalibratedFixedLTZThreshold = 0.0;
 IterMinLTZPotential = std::numeric_limits<double>::max();
 IterMaxLTZPotential = 0.0;
 IterLTZTrackingActive = false;
 LastSyncedMinLTZ = 0.0;
 LastSyncedMaxLTZ = 0.0;

 InputPattern.Resize(NumInputDendrite, 1);
 AdditionalInputPattern.Resize(NumInputDendrite, 1);
 PrevInputPattern.Assign(NumInputDendrite, 1, -1.0);

 DendriteNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 SomaNeuronAmplitude.Assign(NumInputDendrite + 1, 1, 0.0);
 Output.Assign(NumInputDendrite, 1, 0.0);

 SynapseResistanceStep = 1.0e9;
 NormalizationMode = kNormParametric;
 SynapseResistanceBase = kSynapseResistanceBioDefault;
 ResistanceMin = 1.0e6;
 ResistanceMax = 1.0e11;
 AttenuationGamma = kAttenuationGammaAuto;
 ResistanceAdjustGain = kResistanceAdjustGainDefault;
 TipSynapseResistance.assign(NumInputDendrite, SynapseResistanceBase.GetData());
 DendriteLength.assign(NumInputDendrite, 1);
 if(NumInputDendrite.GetData() >= 1)
  DendriteLength[NumInputDendrite.GetData() - 1] = 0;
 OldDendriteLength = DendriteLength.GetData();
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
 PrevDissynchronization = Dissynchronization;
 AmpDifference.assign(NumInputDendrite, 0.0);
 SomaPeakValid.assign(NumInputDendrite, false);
 DendStatus.assign(NumInputDendrite, 0);
 SynapseStatus.assign(NumInputDendrite, 0);
 ResistanceStatus.assign(NumInputDendrite, 0);
 ResistanceDifference.assign(NumInputDendrite, 0.0);
 PrevAmpError.assign(NumInputDendrite, 0.0);
 PrevResistanceRatio.assign(NumInputDendrite, 1.0);
 NoImproveResistanceCount.assign(NumInputDendrite, 0);
 EffectiveResistanceGain.assign(NumInputDendrite, kResistanceAdjustGainDefault);
 EnableDebug = false;
 ResizeSyncVectors(NumInputDendrite.GetData());

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


bool NNeuronTimeLearnerBranch::ABuild(void)
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

void NNeuronTimeLearnerBranch::UpdateComputationOrder(void)
{
 int position = 0;
 if(Dataset)
  SetComponentPosition(Dataset->GetName(), position++);
 if(Neuron)
  SetComponentPosition(Neuron->GetName(), position++);
}

bool NNeuronTimeLearnerBranch::AReset(void)
{
 ApplyLoadedAnchorProperties();

 // Snapshot only a cold topology (all L<=1). Trained samples (L=49 41 25 1 in
 // Parameters_00.xml) must not become the "untrained" baseline.
 const bool do_reset_untrained = ResetToUntrainedState.GetData();
 if(!HasUntrainedSnapshot && !do_reset_untrained
    && StructureLooksUntrained(DendriteLength.GetData()))
 {
  bool any_initial = false;
  for(size_t i = 0; i < InitialSomaPotential.size(); ++i)
  {
   if(InitialSomaPotential[i] > 0.0)
   {
    any_initial = true;
    break;
   }
  }
  if(any_initial)
  {
   UntrainedDendriteLength = DendriteLength.GetData();
   UntrainedNumSynapse = NumSynapse.GetData();
   UntrainedInitialSomaPotential = InitialSomaPotential.GetData();
   UntrainedTipSynapseResistance = TipSynapseResistance.GetData();
   HasUntrainedSnapshot = true;
  }
 }

 // Drop a contaminated snapshot taken before this guard existed (trained L).
 if(HasUntrainedSnapshot && !StructureLooksUntrained(UntrainedDendriteLength))
  HasUntrainedSnapshot = false;

 if(do_reset_untrained)
 {
  ResetToUntrainedState.SetDataDirect(false);
  if(!ResetToUntrained())
   return false;
 }
 else
 {
  // Load / ordinary Reset: Parameters may keep IsNeedToTrain=1 with Fixed LTZ.
  ApplyActiveLtzThreshold();
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
 HasPrevPeakSnapshot = false;
 if(int(DendLastAbsDt.size()) != NumInputDendrite.GetData())
  ResizeSyncVectors(NumInputDendrite.GetData());
 else
 {
  const double large_dt = (IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5) + 0.001;
  std::fill(DendLastAbsDt.begin(), DendLastAbsDt.end(), large_dt);
  std::fill(NoImproveCount.begin(), NoImproveCount.end(), 0);
  if(int(NoImproveResistanceCount.size()) != NumInputDendrite.GetData())
   NoImproveResistanceCount.assign(NumInputDendrite.GetData(), 0);
  else
   std::fill(NoImproveResistanceCount.begin(), NoImproveResistanceCount.end(), 0);
  if(DendBestEffortSynced.size() != DendLastAbsDt.size())
   DendBestEffortSynced.assign(DendLastAbsDt.size(), false);
  else
   std::fill(DendBestEffortSynced.begin(), DendBestEffortSynced.end(), false);
 }
 PulseIndexInIter = 0;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 IsFirstBeat = true;
 ApplyPulseGeneratorMute();
 return true;
}

bool NNeuronTimeLearnerBranch::CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e)
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


bool NNeuronTimeLearnerBranch::ChangeDendriteLength(int num)
{
 if(num < 0 || num >= NumInputDendrite - 1)
  return true;
 if(!DendStatus[num])
  return true;

 // Preserve other statuses, apply only this dendrite via the batch path.
 std::vector<int> saved = DendStatus;
 for(int i = 0; i < NumInputDendrite - 1; ++i)
 {
  if(i != num)
   DendStatus[i] = 0;
 }
 const bool ok = ApplyPendingDendriteLengthChanges();
 DendStatus = saved;
 return ok;
}


bool NNeuronTimeLearnerBranch::ApplyPendingDendriteLengthChanges(void)
{
 bool res(true);
 std::vector<int> changed;
 changed.reserve(static_cast<size_t>(std::max(0, NumInputDendrite.GetData() - 1)));

 for(int i = 0; i < NumInputDendrite - 1; ++i)
 {
  if(!DendStatus[i])
   continue;
  if((DendStatus[i] == -1) && (DendriteLength[i] < 2))
  {
   DendStatus[i] = 0;
   continue;
  }
  if((DendStatus[i] == 1) && (DendriteLength[i] >= MaxDendriteLength))
  {
   DendStatus[i] = 0;
   continue;
  }

  if(i >= int(OldDendriteLength.size()))
   OldDendriteLength.resize(static_cast<size_t>(i + 1), DendriteLength[i]);
  OldDendriteLength[i] = DendriteLength[i];

  const int direction = (DendStatus[i] > 0) ? 1 : -1;
  int delta = 1;
  if(i < int(Dissynchronization.size()) && EstDelayPerSeg > 1e-9)
  {
   delta = int(std::round(fabs(Dissynchronization[static_cast<size_t>(i)]) / EstDelayPerSeg));
   if(delta < 1)
    delta = 1;
   if(delta > kMaxLengthStep)
    delta = kMaxLengthStep;
  }

  // Anti-overshoot: do not jump past needed±SyncTolerance on the cable model.
  if(EstDelayPerSeg > 1e-9 && i < int(Dissynchronization.size()))
  {
   const double delay_now = DelayLenOf(i);
   const double dsyn = Dissynchronization[static_cast<size_t>(i)];
   const double tol = SyncTolerance.GetData();
   if(direction > 0 && dsyn > 0.0)
   {
    const double needed_est = delay_now + dsyn;
    int max_delta = int(std::floor((needed_est + tol - delay_now) / EstDelayPerSeg + 1e-9));
    if(max_delta < 1)
     max_delta = 1;
    if(delta > max_delta)
     delta = max_delta;
   }
   else if(direction < 0 && dsyn < 0.0)
   {
    const double needed_est = delay_now + dsyn; // dsyn negative
    int max_delta = int(std::floor((delay_now - (needed_est - tol)) / EstDelayPerSeg + 1e-9));
    if(max_delta < 1)
     max_delta = 1;
    if(delta > max_delta)
     delta = max_delta;
   }
  }

  if(direction > 0)
  {
   const int room = MaxDendriteLength.GetData() - DendriteLength[i];
   if(room < 1)
   {
    DendStatus[i] = 0;
    continue;
   }
   if(delta > room)
    delta = room;
  }
  else
  {
   const int room = DendriteLength[i] - 1;
   if(room < 1)
   {
    DendStatus[i] = 0;
    continue;
   }
   if(delta > room)
    delta = room;
  }

  DendriteLength[i] += direction * delta;
  LastLengthDelta = delta;
  LastLengthDeltaDendrite = i;
  if(i < int(OldDendriteLength.size()) && OldDendriteLength[i] != DendriteLength[i])
   DetachBranchExcSynapseAtSegment(OldDendriteLength[i]);
  EnforceSegmentMonotonicity(i);
  changed.push_back(i);
 }

 if(changed.empty())
  return true;

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "ApplyPendingDendriteLengthChanges: changed=[";
  for(size_t k = 0; k < changed.size(); ++k)
  {
   if(k) oss << ',';
   oss << changed[k] << ":" << OldDendriteLength[changed[k]]
       << "->" << DendriteLength[changed[k]];
  }
  oss << "] len=[";
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(i) oss << ',';
   oss << DendriteLength[i];
  }
  oss << "]";
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }

 if(!Neuron)
  return false;

 // Preserve external listeners on Generator1.Output while retargeting synapses.

 {
  std::vector<int> chain(1, ChainLengthMax());
  Neuron->NumDendriteMembranePartsVec = chain;
 }
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch",
               "ApplyPendingDendriteLengthChanges: Neuron->Build failed");
  return false;
 }

 EnsureSingleInputSynapseOnBranchSegments(Neuron, ChainLengthMax());

 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 for(int d = 0; d < NumInputDendrite; ++d)
 {
  bool linked = RelinkDendriteSynapsesToDataset(d);
  if(!linked)
  {
   const bool was_changed = std::find(changed.begin(), changed.end(), d) != changed.end();
   if(was_changed)
   {
    if(UEPtr<NPulseMembrane> tip = GetInputMembraneForPulse(d))
    {
     if(!tip->GetActivity())
      tip->SetActivity(true);
     if(!tip->IsInit())
      tip->Init();
     if (EnableDebug.GetData() && RDK::GetLogger())
     {
      std::ostringstream oss;
      oss << "ApplyPendingDendriteLengthChanges: tip Build+Relink retry pulse="
          << d << " pos=" << DendriteLength[d];
      RDK::GetLogger()->LogMessageEx(RDK_EX_WARNING, "NNeuronTimeLearnerBranch", oss.str());
     }
     linked = RelinkDendriteSynapsesToDataset(d);
    }
   }
  }
  res &= linked;
 }

 for(int d : changed)
 {
  if(d < 0 || d >= int(DendriteLength.size()) || d >= int(OldDendriteLength.size()))
   continue;
  const int deltaL = DendriteLength[static_cast<size_t>(d)] - OldDendriteLength[static_cast<size_t>(d)];
  FeedforwardResistanceOnLengthGrow(d, deltaL);
 }

 ApplyPulseGeneratorMute();
 if(NPulseGeneratorTransit *g = GetDatasetGenerator())
  g->Reset();
 PrevPulseCounter = 0;
 PrevGenOutput = 0.0;
 return res;
}


bool NNeuronTimeLearnerBranch::ChangeSynapseNumber(int num)
{
 if(IsParametricNormalization())
 {
  SynapseStatus[num] = 0;
  if(NumSynapse[num] != 1)
  {
   NumSynapse[num] = 1;
   if(Neuron)
   {
    UEPtr<NPulseMembrane> dendrite = GetInputMembraneForPulse(num);
    if(dendrite && int(dendrite->NumExcitatorySynapses) != 1)
    {
     dendrite->NumExcitatorySynapses = 1;
     dendrite->Build();
    }
   }
  }
  return ApplySynapseResistanceChange(num);
 }

 bool res(true);

 if(!SynapseStatus[num])
  return true;

 if((SynapseStatus[num] == -1) && (NumSynapse[num] < 2))
 {
  SynapseStatus[num] = 0;
  return true;
 }

 if((SynapseStatus[num] == 1) && (NumSynapse[num] >= kMaxSynapsesPerDend))
 {
  SynapseStatus[num] = 0;
  return true;
 }

 NumSynapse[num] += SynapseStatus[num];

 UEPtr<NPulseMembrane> dendrite = GetInputMembraneForPulse(num);
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
  res &= (synapse != 0);
  if(num == ActivePulseIndex)
   ApplyPulseGeneratorMute();
  if(!res)
   return true;
 }

 Neuron->Reset();
 return true;
}


bool NNeuronTimeLearnerBranch::MeasureMaxPotentialAndTime(void)
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
 // Allow dendritic delay up to settle margin that scales with max length.
 const double settle = SettleMarginSec();

 for(int i = 0; i < NumInputDendrite; ++i)
 {
  if(i < int(PeakLocked.size()) && PeakLocked[i])
   continue;

  if(i < ActivePulseIndex && IsNeedToTrain.GetData() && TrainingPhase.GetData() != kPhaseDone)
  {
   if(i < int(PeakLocked.size()))
    PeakLocked[i] = true;
   continue;
  }

  const double t_pulse = FirstImpulseTime
   + ((i < int(ExpectedPulseRelTimes.size())) ? ExpectedPulseRelTimes[i] : 0.0);
  if(now + 1e-12 < t_pulse)
   continue;

  // Track own-pulse soma peak near Expected[i]+cable(L), not a later pulse's wave.
  const double delay_est = std::max(0.0,
   (DendriteLength[i] > 1) ? (DendriteLength[i] - 1) * EstDelayPerSeg : 0.0);
  const double expected_i = (i < int(ExpectedPulseRelTimes.size()))
   ? ExpectedPulseRelTimes[static_cast<size_t>(i)] : 0.0;
  const double margin = std::max(PeakMeasureMargin.GetData(),
   0.5 * double(kMaxLengthStep) * EstDelayPerSeg);
  double t_lo_rel = expected_i + delay_est - margin;
  double t_hi_rel = expected_i + delay_est + margin;
  if(t_lo_rel < 0.0)
   t_lo_rel = 0.0;
  // Allow a little early membrane peak; do not clamp lo up to expected_i.
  if(i + 1 < int(ExpectedPulseRelTimes.size()))
  {
   // Do not enter the next pulse's expected peak band.
   const double next_peak = ExpectedPulseRelTimes[static_cast<size_t>(i + 1)] + delay_est;
   if(t_hi_rel > next_peak - 0.002)
    t_hi_rel = next_peak - 0.002;
  }
  else
  {
   const double cap = pattern_end + settle;
   if(t_hi_rel > cap)
    t_hi_rel = cap;
  }
  if(t_hi_rel <= t_lo_rel + 1e-9)
   t_hi_rel = t_lo_rel + 0.01;

  if(now + 1e-12 < FirstImpulseTime + t_lo_rel)
   continue;

  const double t_end = FirstImpulseTime + t_hi_rel;
  if(now > t_end)
  {
   // Lock only after a real peak; otherwise leave unlocked so IterationGap
   // ends the wait while SomaPeakValid stays false (amp≈0 is not sync).
   if(i < int(PeakSeen.size()) && PeakSeen[i] && i < int(PeakLocked.size()))
    PeakLocked[i] = true;
   continue;
  }

  UEPtr<NPulseMembrane> soma = Neuron->GetComponentL<NPulseMembrane>(MakeBranchSomaName(), true);
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

   // Latch Initial only on L==1 (full short-cable peak). Updating at L>1 while
   // Initial<=0 froze the anchor on the rising-edge sample after early growth.
   if(DendriteLength[i] <= 1 && NumSynapse[i]
      && currentsomaamp > InitialSomaPotential[i])
    InitialSomaPotential[i] = currentsomaamp;

   if(DendriteLength[i] <= 1 && NumSynapse[i] == 1 && IsParametricNormalization())
   {
    if(NPulseSynapseCommon *synapse = GetTipSynapse(i))
    {
     std::vector<double> tips = TipSynapseResistance.GetData();
     if(tips.size() != static_cast<size_t>(NumInputDendrite))
      tips.resize(static_cast<size_t>(NumInputDendrite), SynapseResistanceBase.GetData());
     tips[static_cast<size_t>(i)] = synapse->Resistance;
     TipSynapseResistance.SetDataDirect(tips);
     if(i == 0 || SynapseResistanceBase.GetData() <= 0.0)
      SynapseResistanceBase = synapse->Resistance;
    }
   }
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


bool NNeuronTimeLearnerBranch::ChangeDendriteStatus(int num)
{
 if(num == NumInputDendrite - 1)
 {
  DendStatus[num] = 0;
  Dissynchronization[num] = 0.0;
  return true;
 }

 if(num != ActiveDendrite && num != ActivePulseIndex)
 {
  DendStatus[num] = 0;
  return true;
 }

 const int ref = NumInputDendrite - 1;
 const bool num_valid = (num < int(SomaPeakValid.size())) && SomaPeakValid[num];
 const bool prev_ref_valid = (ref < int(PulseSynced.size()) && PulseSynced[static_cast<size_t>(ref)])
  || HasPrevPeakSnapshot;

 if(!num_valid || !prev_ref_valid || num >= int(PeakRel.size())
    || ref >= int(PrevPeakRel.size()))
 {
  // Never grow/shrink length without a peak on the active dendrite and a valid
  // Prev ref. Dead tip after cable length is already near needed: accept only.
  DendStatus[num] = 0;

  if(!num_valid && prev_ref_valid
     && num < int(ExpectedPulseRelTimes.size())
     && ref < int(PrevPeakRel.size()))
  {
   const double expected_k = ExpectedPulseRelTimes[static_cast<size_t>(num)];
   const double needed = MeanStoredPeakRel(num) - expected_k;
   const double delay_len = DelayLenOf(num);
   const double dt = needed - delay_len;
   Dissynchronization[num] = dt;
   if(num < int(DendLastAbsDt.size()))
    DendLastAbsDt[static_cast<size_t>(num)] = fabs(dt);

   const bool amp_low = (num < int(InitialSomaPotential.size()))
    && (InitialSomaPotential[num] > 0.0)
    && (num < int(MaxIterSomaAmp.size()))
    && (MaxIterSomaAmp[num] < kAmpCollapseRatio * InitialSomaPotential[num]);
   const bool synapses_exhausted = (num < int(NumSynapse.size()))
    && (NumSynapse[num] >= kMaxSynapsesPerDend);
   const bool model_ok = fabs(dt) <= SyncTolerance.GetData();

   if((amp_low && synapses_exhausted) || model_ok)
   {
    if(num < int(DendLastAbsDt.size()))
     DendLastAbsDt[static_cast<size_t>(num)] =
      std::min(fabs(dt), SyncTolerance.GetData());
    if(num < int(DendBestEffortSynced.size()))
     DendBestEffortSynced[static_cast<size_t>(num)] = true;
    if (EnableDebug.GetData() && RDK::GetLogger())
    {
     std::ostringstream oss;
     oss << "ChangeDendriteStatus: num=" << num
         << " invalid peak accept-only needed=" << needed
         << " delay_len=" << delay_len << " dt=" << dt
         << " amp_collapse=" << ((amp_low && synapses_exhausted) ? 1 : 0)
         << " syn=" << NumSynapse[num];
     RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
    }
    return true;
   }
  }

  if (EnableDebug.GetData() && RDK::GetLogger())
  {
   std::ostringstream oss;
   oss << "ChangeDendriteStatus: num=" << num
       << " skip length: no peak (num_valid=" << (num_valid ? 1 : 0)
       << " prev_ref_valid=" << (prev_ref_valid ? 1 : 0) << ")";
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }
  return true;
 }

 const double expected_k = (num < int(ExpectedPulseRelTimes.size()))
  ? ExpectedPulseRelTimes[static_cast<size_t>(num)] : 0.0;
 const double needed = MeanStoredPeakRel(num) - expected_k;
 const double delay_k = (num < int(DelayFromPulse.size()))
  ? DelayFromPulse[static_cast<size_t>(num)] : 0.0;
 if(delay_k < 0.0 || delay_k >= EffectiveIterationGapSec())
 {
  if(DendriteLength[num] <= DendriteLength[ref])
   DendStatus[num] = 1;
  if (EnableDebug.GetData() && RDK::GetLogger())
  {
   std::ostringstream oss;
   oss << "ChangeDendriteStatus: num=" << num
       << " bad delay_k=" << delay_k << " DendStatus=" << DendStatus[num];
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }
  return true;
 }
 const double delay_len = DelayLenOf(num);
 const double delay_use = DelayUseOf(num, expected_k, delay_k);
 const bool meas_agrees = (fabs(delay_use - delay_k) < 1e-15);
 const double dt = needed - delay_use;
 const double prev_dt = (num < int(PrevDissynchronization.size()))
  ? PrevDissynchronization[num] : 0.0;

 // Did we apply a length change to this dendrite after the previous decision?
 const bool had_length_apply = (LastLengthDeltaDendrite == num && LastLengthDelta > 0);
 const int applied_delta = LastLengthDelta;

 // Refine EstDelayPerSeg from previous apply on this dendrite.
 if(had_length_apply && fabs(prev_dt) > 1e-12 && fabs(dt) < fabs(prev_dt))
 {
  const double observed = (fabs(prev_dt) - fabs(dt)) / double(applied_delta);
  if(observed > 1e-4 && observed < 1.0)
   EstDelayPerSeg = 0.7 * EstDelayPerSeg + 0.3 * observed;
 }
 if(had_length_apply && IsParametricNormalization())
 {
  if(num < int(NoImproveResistanceCount.size()))
   NoImproveResistanceCount[static_cast<size_t>(num)] = 0;
  if(num < int(PrevAmpError.size()))
   PrevAmpError[static_cast<size_t>(num)] = 0.0;
  if(num < int(PrevResistanceRatio.size()))
   PrevResistanceRatio[static_cast<size_t>(num)] = 1.0;
  if(num < int(EffectiveResistanceGain.size()))
  {
   const double g = (ResistanceAdjustGain.GetData() > 0.0)
    ? ResistanceAdjustGain.GetData() : kResistanceAdjustGainDefault;
   EffectiveResistanceGain[static_cast<size_t>(num)] = g;
  }
 }
 LastLengthDelta = 0;
 LastLengthDeltaDendrite = -1;

 // Amp collapse: accept best-effort only if time already ok (and cable model agrees)
 // or synapses exhausted. Do not stop early while delay_len still short of needed.
 const bool amp_low = (num < int(InitialSomaPotential.size()))
  && (InitialSomaPotential[num] > 0.0)
  && (num < int(MaxIterSomaAmp.size()))
  && (MaxIterSomaAmp[num] < kAmpCollapseRatio * InitialSomaPotential[num]);
 const bool time_ok = fabs(dt) <= SyncTolerance.GetData();
 const bool model_still_short = (delay_len + SyncTolerance.GetData()) < needed;
 const bool synapses_exhausted = (num < int(NumSynapse.size()))
  && (NumSynapse[num] >= kMaxSynapsesPerDend);

 if(dt > 0.0 && amp_low && (synapses_exhausted || (time_ok && !model_still_short)))
 {
  DendStatus[num] = 0;
  if(num < int(DendLastAbsDt.size()))
   DendLastAbsDt[static_cast<size_t>(num)] = std::min(fabs(dt), SyncTolerance.GetData());
  if(num < int(DendBestEffortSynced.size()))
   DendBestEffortSynced[static_cast<size_t>(num)] = true;
  Dissynchronization[num] = dt;
  if(num < int(NoImproveCount.size()))
   NoImproveCount[static_cast<size_t>(num)] = 0;
  if (EnableDebug.GetData() && RDK::GetLogger())
  {
   std::ostringstream oss;
   oss << "ChangeDendriteStatus: num=" << num
       << " amp-collapse accept dt=" << dt
       << " needed=" << needed << " delay_use=" << delay_use
       << " amp=" << MaxIterSomaAmp[num]
       << " init=" << InitialSomaPotential[num]
       << " syn=" << NumSynapse[num];
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }
  return true;
 }

 if(dt == 0.0)
  DendStatus[num] = 0;
 else if(fabs(dt) <= SyncTolerance)
  DendStatus[num] = 0;
 else if((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001)
         && (prev_dt * dt < 0.0)
         && (fabs(dt) <= fabs(prev_dt)))
  DendStatus[num] = 0; // sign flip with shrinking |dt| — stop oscillation
 else if(dt > 0)
  DendStatus[num] = 1;
 else
  DendStatus[num] = -1;

 if(DendStatus[num] != 0 && num < int(DendBestEffortSynced.size()))
  DendBestEffortSynced[static_cast<size_t>(num)] = false;

 // No-improve only after a real length apply on this dendrite failed to reduce |dt|.
 if(num < int(NoImproveCount.size()))
 {
  if(had_length_apply && DendStatus[num] != 0
     && fabs(dt) >= fabs(prev_dt) - 1e-12)
   NoImproveCount[static_cast<size_t>(num)]++;
  else if(fabs(dt) < fabs(prev_dt) - 1e-12 || fabs(dt) <= SyncTolerance.GetData())
   NoImproveCount[static_cast<size_t>(num)] = 0;
  if(NoImproveCount[static_cast<size_t>(num)] >= kNoImproveLimit)
  {
   DendStatus[num] = 0;
   if(num < int(DendBestEffortSynced.size()))
    DendBestEffortSynced[static_cast<size_t>(num)] = true;
  }
 }

 if(fabs(dt) <= SyncTolerance.GetData() && num < int(DendBestEffortSynced.size()))
  DendBestEffortSynced[static_cast<size_t>(num)] = false; // real sync supersedes best-effort

 if(num < int(DendLastAbsDt.size()))
  DendLastAbsDt[static_cast<size_t>(num)] = fabs(dt);
 Dissynchronization[num] = dt;

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "ChangeDendriteStatus: num=" << num
      << " dt=" << dt
      << " needed=" << needed
      << " delay_meas=" << delay_k
      << " delay_len=" << delay_len
      << " delay_use=" << delay_use
      << " agrees=" << (meas_agrees ? 1 : 0)
      << " expected_k=" << expected_k
      << " DendStatus=" << DendStatus[num]
      << " prev_dt=" << prev_dt
      << " estDelayPerSeg=" << EstDelayPerSeg;
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 return true;
}


bool NNeuronTimeLearnerBranch::ChangeSynapseStatus(int num)
{
 if(IsParametricNormalization())
  return ChangeSynapseResistanceStatus(num);

 int dendstatus = DendStatus[num];

 // No amp target yet (need a successful L==1 peak) — do not ±1 synapses.
 if(num >= int(InitialSomaPotential.size()) || InitialSomaPotential[num] <= 0.0)
 {
  SynapseStatus[num] = 0;
  AmpDifference[num] = 0.0;
  DendStatus[num] = dendstatus;
  return true;
 }

 double dt = InitialSomaPotential[num] - MaxIterSomaAmp[num];

 // Dead tip after length sync: cannot restore amp — stop adding synapses.
 const bool length_settled = (num < int(DendLastAbsDt.size())
  && DendLastAbsDt[static_cast<size_t>(num)] <= SyncTolerance.GetData())
  || ((num < int(DendBestEffortSynced.size()))
      && DendBestEffortSynced[static_cast<size_t>(num)]);
 if(!DendStatus[num] && length_settled
    && (num < int(MaxIterSomaAmp.size()))
    && (MaxIterSomaAmp[num] < kMinMeasurableSomaAmp))
 {
  SynapseStatus[num] = 0;
  AmpDifference[num] = dt;
  DendStatus[num] = dendstatus;
  return true;
 }

 // Optimum only when dendrite length is settled (!DendStatus); otherwise amp
 // still drives SynapseStatus ±1 in parallel with length (NNeuronLearner-style).
 // Do not lock SynapseStatus=0 solely because it was already 0 (max-cap / prior).
 if ((fabs(PrevInputPattern[num] - InputPattern[num]) < 0.0001) && !DendStatus[num]
          && (fabs(dt) <= kAmpNormEps))
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

bool NNeuronTimeLearnerBranch::PatternRecognition(void)
{
 return true;
}

bool NNeuronTimeLearnerBranch::LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern)
{
 (void)second_pattern;
 return true;
}

bool NNeuronTimeLearnerBranch::IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern)
{
 (void)InitialPattern;
 (void)second_pattern;
 return true;
}

bool NNeuronTimeLearnerBranch::Experiment(void)
{
 return true;
}

bool NNeuronTimeLearnerBranch::AllDendritesSynced(void) const
{
 const int n = NumInputDendrite.GetData();
 if(n < 2)
  return (n < 1) || (int(PulseSynced.size()) > 0 && PulseSynced[0]);
 if(n - 1 >= int(PulseSynced.size()) || !PulseSynced[static_cast<size_t>(n - 1)])
  return false;
 for(int i = 0; i < n - 1; i++)
 {
  if(i >= int(PulseSynced.size()) || !PulseSynced[static_cast<size_t>(i)])
   return false;
 }
 return true;
}

bool NNeuronTimeLearnerBranch::AllSynapsesNormalized(void) const
{
 const double eps = kAmpNormEps;
 // Ref dendrite (N-1) is the length/timing anchor: ChangeSynapseResistanceStatus
 // skips R-tune there, and DendLastAbsDt[ref] stays a large sentinel. Match
 // AllDendritesSynced — only non-ref dendrites gate amp Done.
 const int n_check = std::max(0, NumInputDendrite.GetData() - 1);
 if(IsParametricNormalization())
 {
  const double rmin = ResistanceMin.GetData();
  for(int i = 0; i < n_check; i++)
  {
   // Reverse train: only the active pulse is tuned; lower indices are not started
   // yet and higher indices are already committed — muted bursts must not re-gate Done.
   if(i != ActivePulseIndex)
    continue;

   const bool length_ok = (i < int(DendLastAbsDt.size())
    && DendLastAbsDt[static_cast<size_t>(i)] <= SyncTolerance.GetData())
    || ((i < int(DendBestEffortSynced.size()))
        && DendBestEffortSynced[static_cast<size_t>(i)]);

   if(i < int(ResistanceStatus.size()) && ResistanceStatus[i])
   {
    if(length_ok && i < int(NoImproveResistanceCount.size())
       && NoImproveResistanceCount[static_cast<size_t>(i)] >= kNoImproveResistanceLimit)
     ; // best-effort: stop blocking on pending resStatus
    else
     return false;
   }

   const bool amp_ok = length_ok
    && (i < int(InitialSomaPotential.size()))
    && (i < int(MaxIterSomaAmp.size()))
    && (fabs(InitialSomaPotential[i] - MaxIterSomaAmp[i]) <= eps);
   if(amp_ok)
    continue;

   const bool at_r_min = (i < int(TipSynapseResistance.size()))
    && (TipSynapseResistance[static_cast<size_t>(i)] <= rmin * (1.0 + 1e-6));
   const bool dt_positive = (i < int(InitialSomaPotential.size()))
    && (i < int(MaxIterSomaAmp.size()))
    && (InitialSomaPotential[i] > MaxIterSomaAmp[i] + eps);
   const bool dead_tip = (i < int(MaxIterSomaAmp.size()))
    && (MaxIterSomaAmp[i] < kMinMeasurableSomaAmp);
   // Require a real peak attempt this burst (PeakSeen): freshly zeroed MaxAmp at
   // BeginTrainingIteration must not count as dead-tip Done.
   const bool peak_attempted = (i < int(PeakSeen.size())) && PeakSeen[static_cast<size_t>(i)];
   if(dead_tip && length_ok && peak_attempted)
    continue;
   if(at_r_min && dt_positive && length_ok)
    continue;

   const bool oscillation_ok =
    length_ok
    && (i < int(NoImproveResistanceCount.size()))
    && (NoImproveResistanceCount[static_cast<size_t>(i)] >= kNoImproveResistanceLimit)
    && (i < int(InitialSomaPotential.size()))
    && (i < int(MaxIterSomaAmp.size()))
    && (fabs(InitialSomaPotential[i] - MaxIterSomaAmp[i]) < kAmpOscillationBand);
   if(oscillation_ok)
    continue;

   const bool no_improve_done =
    length_ok
    && (i < int(NoImproveResistanceCount.size()))
    && (NoImproveResistanceCount[static_cast<size_t>(i)] >= kNoImproveResistanceLimit)
    && at_r_min && dt_positive;
   if(no_improve_done)
    continue;

   return false;
  }
  return true;
 }

 for(int i = 0; i < n_check; i++)
 {
  if(i < ActivePulseIndex)
   continue;
  if(SynapseStatus[i])
   return false;
  const bool at_cap = (i < int(NumSynapse.size()))
   && (NumSynapse[i] >= kMaxSynapsesPerDend);
  const bool amp_ok = (i < int(InitialSomaPotential.size()))
   && (i < int(MaxIterSomaAmp.size()))
   && (fabs(InitialSomaPotential[i] - MaxIterSomaAmp[i]) <= eps);
  if(amp_ok)
   continue;
  const bool dead_tip = (i < int(MaxIterSomaAmp.size()))
   && (MaxIterSomaAmp[i] < kMinMeasurableSomaAmp);
  const bool peak_attempted = (i < int(PeakSeen.size())) && PeakSeen[static_cast<size_t>(i)];
  const bool length_ok = (i < int(DendLastAbsDt.size())
   && DendLastAbsDt[static_cast<size_t>(i)] <= SyncTolerance.GetData())
   || ((i < int(DendBestEffortSynced.size()))
       && DendBestEffortSynced[static_cast<size_t>(i)]);
  if(dead_tip && length_ok && peak_attempted)
   continue;
  if(at_cap)
   continue; // best-effort amp after synapse cap
  return false;
 }
 return true;
}

bool NNeuronTimeLearnerBranch::EndOfLearning(void)
{
 if(TrainingPhase == kPhaseDone)
  return true;

 // Joint train: Done only when lengths synced and synapse amps normalized.
 if(!AllDendritesSynced() || !AllSynapsesNormalized())
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

 CalibrateFixedLTZThresholdFromTraining();

 TrainingPhase = kPhaseDone;
 CanChangeDendLength = false;
 SetIsNeedToTrain(false);
 IsNeedToTrain = false;
 ApplyPulseGeneratorMute(); // recognition: reconnect all tip synapses after Done
 const int link_count = CountGeneratorToBranchExcSynapseLinks();
 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "phase -> Done len=[";
  for(int i = 0; i < NumInputDendrite && i < int(DendriteLength.size()); ++i)
  {
   if(i) oss << ',';
   oss << DendriteLength[static_cast<size_t>(i)];
  }
  oss << "] gen_links=" << link_count << " expect=" << NumInputDendrite.GetData();
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
 if(link_count != NumInputDendrite.GetData())
  LogMessageEx(RDK_EX_WARNING, "NNeuronTimeLearnerBranch",
   "EndOfLearning: Generator link count != NumInputDendrite");
 return true;
}


bool NNeuronTimeLearnerBranch::DetectNewImpulse(void)
{
 return DetectNewImpulseCount() > 0;
}

int NNeuronTimeLearnerBranch::DetectNewImpulseCount(void)
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

void NNeuronTimeLearnerBranch::BeginTrainingIteration(double now)
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
  // Rel times are gaps after pulse 0, so Expected[0]=0 (not cumsum including the
  // pre-first ISI — that shifted measure windows +0.01 and missed L=1 peaks).
  ExpectedPulseRelTimes[0] = 0.0;
  double acc = 0.0;
  for(int i = 1; i < NumInputDendrite; ++i)
  {
   double isi = 0.0;
   if(InputPattern.GetRows() > i && InputPattern.GetCols() > 0)
    isi = InputPattern(i, 0);
   if(isi < 0.0)
    isi = 0.0;
   acc += isi;
   ExpectedPulseRelTimes[static_cast<size_t>(i)] = acc;
  }
 }
 WaitingPeakAfterLastPulse = (NumInputDendrite <= 1);
 LastPulseTime = now;
 IterationActive = true;
 IsFirstBeat = false;
 PeakRel.assign(static_cast<size_t>(NumInputDendrite.GetData()), 0.0);
 DelayFromPulse.assign(static_cast<size_t>(NumInputDendrite.GetData()), 0.0);
 IterMinLTZPotential = std::numeric_limits<double>::max();
 IterMaxLTZPotential = 0.0;
 IterLTZTrackingActive = false;

 // Structure changes are applied in FinishTrainingIteration (during the inter-burst
 // gap). Changing length here would Reset() the neuron after pulse 0 and break timing.

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BeginTrainingIteration: iter=" << CountIteration
      << " phase=" << TrainingPhase.GetData()
      << " t=" << now
      << " t0=" << FirstImpulseTime
      << " active=" << ActivePulseIndex
      << " N=" << NumInputDendrite.GetData()
      << " gap=" << IterationGap.GetData()
      << " gapEff=" << EffectiveIterationGapSec()
      << " delay=" << Delay.GetData()
      << " span=" << PatternSpanSec()
      << " settle=" << SettleMarginSec()
      << " expected=[";
  for(size_t i = 0; i < ExpectedPulseRelTimes.size(); ++i)
  {
   if(i) oss << ',';
   oss << ExpectedPulseRelTimes[i];
  }
  oss << "]";
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }
}

void NNeuronTimeLearnerBranch::FinishTrainingIteration(void)
{
 PrevDissynchronization = Dissynchronization;
 SomaPeakValid.assign(static_cast<size_t>(NumInputDendrite.GetData()), false);
 for(int i = 0; i < NumInputDendrite; ++i)
 {
  const bool valid = (i < int(MaxIterSomaAmp.size()))
   && (MaxIterSomaAmp[i] > kMinMeasurableSomaAmp)
   && (i < int(PeakSeen.size()) ? PeakSeen[i] : false);
  if(i < int(SomaPeakValid.size()))
   SomaPeakValid[i] = valid;
  if(!valid && EnableDebug.GetData() && RDK::GetLogger())
  {
   std::ostringstream oss;
   oss << "FinishTrainingIteration: soma " << (i + 1)
       << " no peak detected (amp="
       << ((i < int(MaxIterSomaAmp.size())) ? MaxIterSomaAmp[i] : 0.0) << ")";
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
  }
 }

 ComputePeakRelAndDelay();

 if(TrainingPhase != kPhaseDone && IsNeedToTrain)
 {
  ActivePulseIndex = SelectActiveDendrite();
  ActiveDendrite = ActivePulseIndex;
  DendStatus.assign(static_cast<size_t>(NumInputDendrite.GetData()), 0);
  const int n = NumInputDendrite.GetData();
  const int ref = n - 1;
  const bool anchor_synced = (ref >= 0 && ref < int(PulseSynced.size())
   && PulseSynced[static_cast<size_t>(ref)]);
  if(!anchor_synced)
  {
   const bool ref_valid = (ref >= 0 && ref < int(SomaPeakValid.size())
    && SomaPeakValid[static_cast<size_t>(ref)]);
   if(ref_valid)
   {
    if(int(StoredPeakRel.size()) != n)
     StoredPeakRel.assign(static_cast<size_t>(n), 0.0);
    if(int(PulseSynced.size()) != n)
     PulseSynced.assign(static_cast<size_t>(n), false);
    StoredPeakRel[static_cast<size_t>(ref)] =
     (ref < int(PeakRel.size())) ? PeakRel[static_cast<size_t>(ref)] : 0.0;
    PulseSynced[static_cast<size_t>(ref)] = true;
    HasPrevPeakSnapshot = true;
    ActivePulseIndex = (n >= 2) ? (n - 2) : 0;
    ActiveDendrite = ActivePulseIndex;
    if (EnableDebug.GetData() && RDK::GetLogger())
     RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch",
      "FinishTrainingIteration: anchor pulse committed on Dendrite1_1");
   }
   else if (EnableDebug.GetData() && RDK::GetLogger())
    RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch",
     "FinishTrainingIteration: bootstrap waiting valid anchor peak (no growth)");
  }
  else
  {
   ChangeDendriteStatus(ActivePulseIndex);
   RefreshDendLastAbsDtFromPrevAnchor();
   ChangeSynapseStatus(ActivePulseIndex);
  }
 }

 for(int i = 0; i < NumInputDendrite; i++)
  PrevInputPattern[i] = InputPattern[i];

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "FinishTrainingIteration: iter=" << CountIteration
      << " phase=" << TrainingPhase.GetData()
      << " active=" << ActiveDendrite
      << " hasPrev=" << (HasPrevPeakSnapshot ? 1 : 0)
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
  oss << "] NumSynapse=[";
  for(size_t i = 0; i < NumSynapse.size(); ++i)
  {
   if(i) oss << ',';
   oss << NumSynapse[i];
  }
  oss << "] times=[";
  for(size_t i = 0; i < TimeOfMaxIterSomaAmp.size(); ++i)
  {
   if(i) oss << ',';
   oss << TimeOfMaxIterSomaAmp[i];
  }
  oss << "] peakRel=[";
  for(size_t i = 0; i < PeakRel.size(); ++i)
  {
   if(i) oss << ',';
   oss << PeakRel[i];
  }
  oss << "] delay=[";
  for(size_t i = 0; i < DelayFromPulse.size(); ++i)
  {
   if(i) oss << ',';
   oss << DelayFromPulse[i];
  }
  oss << "] prevPeakRel=[";
  for(size_t i = 0; i < PrevPeakRel.size(); ++i)
  {
   if(i) oss << ',';
   oss << PrevPeakRel[i];
  }
  oss << "] amp=[";
  for(size_t i = 0; i < MaxIterSomaAmp.size(); ++i)
  {
   if(i) oss << ',';
   oss << MaxIterSomaAmp[i];
  }
  oss << "] initial=[";
  for(size_t i = 0; i < InitialSomaPotential.size(); ++i)
  {
   if(i) oss << ',';
   oss << InitialSomaPotential[i];
  }
  oss << "] ampDt=[";
  for(size_t i = 0; i < MaxIterSomaAmp.size(); ++i)
  {
   if(i) oss << ',';
   const double initial = (i < InitialSomaPotential.size())
    ? InitialSomaPotential[i] : 0.0;
   oss << (initial - MaxIterSomaAmp[i]);
  }
  oss << "] atCap=[";
  for(size_t i = 0; i < NumSynapse.size(); ++i)
  {
   if(i) oss << ',';
   oss << ((NumSynapse[i] >= kMaxSynapsesPerDend) ? 1 : 0);
  }
  oss << "] resistance=[";
  for(size_t i = 0; i < TipSynapseResistance.size(); ++i)
  {
   if(i) oss << ',';
   oss << TipSynapseResistance[i];
  }
  oss << "] resStatus=[";
  for(size_t i = 0; i < ResistanceStatus.size(); ++i)
  {
   if(i) oss << ',';
   oss << ResistanceStatus[i];
  }
  oss << "] atRMin=[";
  {
   const double rmin = ResistanceMin.GetData();
   for(size_t i = 0; i < TipSynapseResistance.size(); ++i)
   {
    if(i) oss << ',';
    oss << ((TipSynapseResistance[i] <= rmin * (1.0 + 1e-6)) ? 1 : 0);
   }
  }
  oss << "] adjGain=[";
  for(size_t i = 0; i < EffectiveResistanceGain.size(); ++i)
  {
   if(i) oss << ',';
   oss << EffectiveResistanceGain[i];
  }
  oss << "] noImpR=[";
  for(size_t i = 0; i < NoImproveResistanceCount.size(); ++i)
  {
   if(i) oss << ',';
   oss << NoImproveResistanceCount[i];
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
  oss << "] lastAbsDt=[";
  for(size_t i = 0; i < DendLastAbsDt.size(); ++i)
  {
   if(i) oss << ',';
   oss << DendLastAbsDt[i];
  }
  oss << "] peakValid=[";
  for(size_t i = 0; i < SomaPeakValid.size(); ++i)
  {
   if(i) oss << ',';
   oss << (SomaPeakValid[i] ? 1 : 0);
  }
  oss << "]";
  if(Neuron && !DendriteLength.empty())
  {
   oss << " d0amp=[";
   for(int seg = 1; seg <= ChainLengthMax(); ++seg)
   {
    UEPtr<NPulseMembrane> dmem = Neuron->GetComponentL<NPulseMembrane>(
     MakeBranchDendriteName(seg), true);
    if(seg > 1) oss << ',';
    oss << (dmem ? dmem->SumPotential(0, 0) : -1.0);
   }
   oss << "]";
  }
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
 }

 if(AllDendritesSynced() && IterLTZTrackingActive
  && IterMaxLTZPotential > IterMinLTZPotential + 1e-9)
 {
  LastSyncedMinLTZ = IterMinLTZPotential;
  LastSyncedMaxLTZ = IterMaxLTZPotential;
 }

 // Apply length then synapses in one inter-burst gap (tip after Build/relink).
 if(TrainingPhase != kPhaseDone && IsNeedToTrain && CanChangeDendLength)
  ApplyPendingDendriteLengthChanges();
 if(TrainingPhase != kPhaseDone && IsNeedToTrain)
 {
  const int k = ActivePulseIndex;
  if(k >= 0 && k < NumInputDendrite - 1)
  {
   if(IsParametricNormalization())
    ApplySynapseResistanceChange(k);
   else
    ChangeSynapseNumber(k);
  }
 }

 {
  const int k = ActivePulseIndex;
  const int n = NumInputDendrite.GetData();
  const double tol = SyncTolerance.GetData();
  if(TrainingPhase != kPhaseDone && IsNeedToTrain
     && k >= 0 && k < n - 1
     && k < int(PulseSynced.size()) && !PulseSynced[static_cast<size_t>(k)])
  {
   const bool length_ok = (k < int(DendLastAbsDt.size())
    && DendLastAbsDt[static_cast<size_t>(k)] <= tol)
    || ((k < int(DendBestEffortSynced.size()))
        && DendBestEffortSynced[static_cast<size_t>(k)])
    || ((k < int(DendStatus.size())) && DendStatus[static_cast<size_t>(k)] == 0
        && k < int(Dissynchronization.size())
        && fabs(Dissynchronization[static_cast<size_t>(k)]) <= tol);
   if(length_ok && AllSynapsesNormalized())
   {
    const int ref = n - 1;
    bool seg_ok = true;
    if(k < ref && k < int(DendriteLength.size()) && ref < int(DendriteLength.size())
       && DendriteLength[static_cast<size_t>(k)] <= DendriteLength[static_cast<size_t>(ref)])
     seg_ok = false;
    if(k + 1 < n && k < int(DendriteLength.size())
       && k + 1 < int(DendriteLength.size())
       && DendriteLength[static_cast<size_t>(k)] <= DendriteLength[static_cast<size_t>(k + 1)])
     seg_ok = false;
    if(seg_ok)
    {
    if(int(StoredPeakRel.size()) != n)
     StoredPeakRel.assign(static_cast<size_t>(n), 0.0);
    StoredPeakRel[static_cast<size_t>(k)] =
     (k < int(PeakRel.size())) ? PeakRel[static_cast<size_t>(k)] : 0.0;
    PulseSynced[static_cast<size_t>(k)] = true;
    ActivePulseIndex = k - 1;
    ActiveDendrite = ActivePulseIndex;
    if (EnableDebug.GetData() && RDK::GetLogger())
    {
     std::ostringstream oss;
     oss << "FinishTrainingIteration: pulse " << k << " synced+norm StoredPeakRel="
         << StoredPeakRel[static_cast<size_t>(k)] << " nextActive=" << ActivePulseIndex;
     RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
    }
    }
    else if (EnableDebug.GetData() && RDK::GetLogger())
    {
     std::ostringstream oss;
     oss << "FinishTrainingIteration: pulse " << k << " sync blocked seg=[";
     for(int pi = 0; pi < n && pi < int(DendriteLength.size()); ++pi)
     {
      if(pi) oss << ',';
      oss << DendriteLength[static_cast<size_t>(pi)];
     }
     oss << "]";
     RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearnerBranch", oss.str());
    }
   }
  }
  ApplyPulseGeneratorMute();
 }

 // Snapshot this burst for next iteration's Prev* comparison (before CountIteration++).
 CommitPrevPeakSnapshot();

 PrevFirstImpulseTime = FirstImpulseTime;
 HasPrevIteration = true;
 IterationActive = false;
 ActiveMeasureSoma = -1;
 WaitingPeakAfterLastPulse = false;
 IsFirstBeat = true;

 UpdateNormTraces();
 CountIteration++;

 if(!CalculateMode)
  EndOfLearning();
}


bool NNeuronTimeLearnerBranch::Training(void)
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
   can_start = (now - PrevFirstImpulseTime) >= EffectiveIterationGapSec();

  if(new_pulses > 0 && can_start)
  {
   BeginTrainingIteration(now);
   MeasureMaxPotentialAndTime();
   UpdateIterLTZPotential();
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
 UpdateIterLTZPotential();

 const double last_pulse = FirstImpulseTime
  + ((NumInputDendrite > 0 && !ExpectedPulseRelTimes.empty())
     ? ExpectedPulseRelTimes[NumInputDendrite - 1] : 0.0);
 bool all_locked = true;
 const int lock_from = (IsNeedToTrain.GetData() && TrainingPhase.GetData() != kPhaseDone)
  ? std::max(0, ActivePulseIndex) : 0;
 if(PeakLocked.empty())
  all_locked = false;
 for(int i = lock_from; i < int(PeakLocked.size()); ++i)
 {
  if(!PeakLocked[static_cast<size_t>(i)])
  {
   all_locked = false;
   break;
  }
 }

 // Wait until every soma locked its first post-pulse max, or effective gap.
 // Do not cut shortly after the last ISI — long dendrites need more settle time.
 bool finished = false;
 const double settle = SettleMarginSec();
 if(all_locked && now >= last_pulse + std::max(0.01, settle * 0.25))
  finished = true;
 if((now - FirstImpulseTime) >= EffectiveIterationGapSec())
  finished = true;

 if(finished)
  FinishTrainingIteration();

 return true;
}


bool NNeuronTimeLearnerBranch::ACalculate(void)
{
 try
 {
  if(!Neuron)
   return true;

  if(DendriteNeuronAmplitude.GetRows() < 1 + NumInputDendrite)
   DendriteNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
  if(SomaNeuronAmplitude.GetRows() < 1 + NumInputDendrite)
   SomaNeuronAmplitude.Assign(1 + NumInputDendrite, 1, 0.0);
  DendriteNeuronAmplitude(0, 0) = 0;
  for(int i = 0; i < NumInputDendrite; i++)
  {
   UEPtr<NPulseMembrane> membr = GetInputMembraneForPulse(i);
   if(!membr)
    continue;

   DendriteNeuronAmplitude(i + 1, 0) = membr->SumPotential(0, 0);
   DendriteNeuronAmplitude(0, 0) += membr->SumPotential(0, 0);
  }

  SomaNeuronAmplitude(0, 0) = 0;
  {
   UEPtr<NPulseMembrane> soma =
    Neuron->GetComponentL<NPulseMembrane>(MakeBranchSomaName(), true);
   const double soma_amp = soma ? soma->SumPotential(0, 0) : 0.0;
   SomaNeuronAmplitude(0, 0) = soma_amp;
   for(int i = 0; i < NumInputDendrite; i++)
   {
    const double amp = (i < int(MaxIterSomaAmp.size())) ? MaxIterSomaAmp[i] : 0.0;
    SomaNeuronAmplitude(i + 1, 0) = amp;
   }
  }

  // Only evaluate Done between bursts: BeginTrainingIteration zeros MaxIterSomaAmp,
  // which would falsely trip the dead-tip escape in AllSynapsesNormalized mid-burst.
  if(!CalculateMode && (CountIteration > 0) && !IterationActive
     && TrainingPhase != kPhaseDone)
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
    oss << "NNeuronTimeLearnerBranch::ACalculate: UException number=" << ex.GetNumber()
        << ", type=" << ex.GetType()
        << ", file=" << ex.GetExFileName()
        << ", line=" << ex.GetExLineNumber()
        << ", what=" << ex.what();
    RDK::GetLogger()->LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearnerBranch", oss.str(), ex.GetNumber());
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
    oss << "NNeuronTimeLearnerBranch::ACalculate: std::exception what=" << ex.what();
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
    RDK::GetLogger()->LogMessage(RDK_EX_ERROR, "NNeuronTimeLearnerBranch::ACalculate: unknown exception");
  }
  catch (...) {}
  throw;
 }
}

}

#endif
