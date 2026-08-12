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

double NNeuronTimeLearner::PatternSpanSec() const
{
 if(ExpectedPulseRelTimes.empty())
  return 0.0;
 return ExpectedPulseRelTimes.back();
}

double NNeuronTimeLearner::SettleMarginSec() const
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

double NNeuronTimeLearner::EffectiveIterationGapSec() const
{
 const double configured = IterationGap.GetData() > 0.0 ? IterationGap.GetData() : 0.5;
 return std::max(configured, PatternSpanSec() + SettleMarginSec() + kGapSlack);
}

double NNeuronTimeLearner::EffectiveDatasetDelaySec() const
{
 const double configured = Delay.GetData() > 0.0 ? Delay.GetData() : 0.5;
 return std::max(configured, SettleMarginSec() + kGapSlack);
}

void NNeuronTimeLearner::ResizeSyncVectors(int n)
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
 ActiveDendrite = 0;
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

bool NNeuronTimeLearner::IsParametricNormalization(void) const
{
 return NormalizationMode.GetData() == kNormParametric;
}

double NNeuronTimeLearner::ClampResistance(double r) const
{
 const double rmin = ResistanceMin.GetData();
 const double rmax = ResistanceMax.GetData();
 if(r < rmin)
  return rmin;
 if(r > rmax)
  return rmax;
 return r;
}

NPulseSynapseCommon* NNeuronTimeLearner::GetTipSynapse(int dendrite_index0) const
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return NULL;
 if(dendrite_index0 >= int(DendriteLength.size()))
  return NULL;
 const int seg = DendriteLength[static_cast<size_t>(dendrite_index0)];
 return Neuron->GetComponentL<NPulseSynapseCommon>(
  MakeLearnerExcSynapsePath(dendrite_index0 + 1, seg, 1), true);
}

bool NNeuronTimeLearner::SetTipSynapseResistanceOnComponent(int dendrite_index0, double r)
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

void NNeuronTimeLearner::EnforceParametricSynapseCount(void)
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

double NNeuronTimeLearner::ComputeModelTipResistance(int dendrite_index0) const
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

double NNeuronTimeLearner::ComputeDampedTipResistance(int dendrite_index0, double r_old,
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

 if(amp < initial)
 {
  const double r_model = ComputeModelTipResistance(dendrite_index0);
  if(r_new < r_model)
   r_new = r_model;
 }

 return ClampResistance(r_new);
}

void NNeuronTimeLearner::ApplyComputedResistance(int num, double r_old, double r_new,
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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
}

void NNeuronTimeLearner::FeedforwardResistanceOnLengthGrow(int dendrite_index0, int deltaL)
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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
}

bool NNeuronTimeLearner::ChangeSynapseResistanceStatus(int num)
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
 const double eps = 0.000005;

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
 else if(same_pattern
         && (dt > eps) && (fabs(ResistanceDifference[num]) > eps)
         && (ResistanceDifference[num] > 0.0)
         && (fabs(dt) <= fabs(ResistanceDifference[num])))
 {
  ResistanceStatus[num] = 0;
 }
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

  const double prev_res_dt = ResistanceDifference[num];
  ApplyComputedResistance(num, r_old, r_new, eff_gain);

  if(num < int(NoImproveResistanceCount.size()))
  {
   if(ResistanceStatus[num] && fabs(dt) >= fabs(prev_res_dt) - 1e-12)
    NoImproveResistanceCount[static_cast<size_t>(num)]++;
   else if(fabs(dt) < fabs(prev_res_dt) - 1e-12 || fabs(dt) <= eps)
    NoImproveResistanceCount[static_cast<size_t>(num)] = 0;
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

bool NNeuronTimeLearner::ApplySynapseResistanceChange(int num)
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

void NNeuronTimeLearner::ComputePeakRelAndDelay(void)
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

int NNeuronTimeLearner::SelectActiveDendrite() const
{
 const int n = NumInputDendrite.GetData();
 if(n < 2)
  return 0;
 // CountIteration 0 is the bootstrap snapshot pack; training rounds start at 1.
 const int round = (CountIteration > 0) ? (CountIteration - 1) : 0;
 return round % (n - 1);
}

void NNeuronTimeLearner::CommitPrevPeakSnapshot(void)
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
  && (ref < int(PrevPeakValid.size()))
  && PrevPeakValid[static_cast<size_t>(ref)];
}

void NNeuronTimeLearner::RefreshDendLastAbsDtFromPrevAnchor(void)
{
 const int n = NumInputDendrite.GetData();
 const int ref = n - 1;
 if(n < 2 || !HasPrevPeakSnapshot)
  return;
 if(ref < 0 || ref >= int(PrevPeakValid.size()) || !PrevPeakValid[static_cast<size_t>(ref)])
  return;
 if(int(DendLastAbsDt.size()) != n)
  DendLastAbsDt.assign(static_cast<size_t>(n), IterationGap.GetData() + 0.001);

 const double tol = SyncTolerance.GetData();
 for(int i = 0; i < n - 1; ++i)
 {
  const double expected_k = (i < int(ExpectedPulseRelTimes.size()))
   ? ExpectedPulseRelTimes[static_cast<size_t>(i)] : 0.0;
  const double needed = PrevPeakRel[static_cast<size_t>(ref)] - expected_k;
  const double delay_meas = (i < int(DelayFromPulse.size()))
   ? DelayFromPulse[static_cast<size_t>(i)] : 0.0;
  const bool peak_valid = (i < int(SomaPeakValid.size())) && SomaPeakValid[static_cast<size_t>(i)];
  const double delay_use = peak_valid
   ? DelayUseOf(i, expected_k, delay_meas)
   : DelayLenOf(i);
  const double abs_dt = fabs(needed - delay_use);
  DendLastAbsDt[static_cast<size_t>(i)] = abs_dt;
  // Invalid/dead tip: length judged by cable model — mark best-effort so Done
  // does not wait for ActiveDendrite to rotate onto this dendrite.
  if(!peak_valid && abs_dt <= tol && i < int(DendBestEffortSynced.size()))
   DendBestEffortSynced[static_cast<size_t>(i)] = true;
 }
}

double NNeuronTimeLearner::DelayLenOf(int num) const
{
 if(num < 0 || num >= int(DendriteLength.size()) || DendriteLength[num] <= 1)
  return 0.0;
 return (DendriteLength[num] - 1) * EstDelayPerSeg;
}

double NNeuronTimeLearner::DelayUseOf(int num, double expected_k, double delay_meas) const
{
 const double delay_len = DelayLenOf(num);
 // Tight margin: loose 0.06 allowed PeakRel-aligned delay_meas to "agree" while
 // delay_len still disagreed by ~0.05 (L1 stopped near 47 instead of ~41).
 const double agree_margin = std::max(SyncTolerance.GetData(), 0.03);
 const bool peak_valid = (num < int(SomaPeakValid.size())) && SomaPeakValid[static_cast<size_t>(num)];
 const double peak_meas = (num < int(PeakRel.size())) ? PeakRel[static_cast<size_t>(num)] : 0.0;
 const double peak_model = expected_k + delay_len;
 const bool meas_agrees = peak_valid
  && (fabs(delay_meas - delay_len) <= agree_margin)
  && (fabs(peak_meas - peak_model) <= agree_margin);
 return meas_agrees ? delay_meas : delay_len;
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
 // Clear incoming Input attachment (DisconnectAll("Input") only clears outgoing
 // RelatedConnectors and does not DetachFrom the Input property).
 synapse->Input.DetachFrom();
 synapse->DisconnectAllItems();
 const bool ok = CreateLink(gen_path, "Output", synapse->GetLongName(this), "Input", -1, true);
 if(EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "LinkSynapseToDataset: " << gen_path << ".Output -> "
      << synapse->GetLongName(this) << ".Input ok=" << (ok ? 1 : 0)
      << " connected=" << (synapse->Input.IsConnected() ? 1 : 0);
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
 return ok;
}

bool NNeuronTimeLearner::RelinkDendriteSynapsesToDataset(int dendrite_index0)
{
 if(!Neuron || dendrite_index0 < 0 || dendrite_index0 >= NumInputDendrite)
  return true;
 // Only the distal tip receives the dataset generator. A second drive on segment 1
 // (previous workaround) left long dendrites with two Generator→ExcSynapse links.
 const int tip_seg = DendriteLength[dendrite_index0];
 if(tip_seg > 1)
 {
  UEPtr<NPulseMembrane> proximal = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(dendrite_index0 + 1, 1), true);
  if(proximal)
  {
   NPulseSynapseCommon *prox_syn = proximal->GetComponentL<NPulseSynapseCommon>(
    std::string("ExcSynapse1"), true);
   if(prox_syn)
  {
   prox_syn->Input.DetachFrom();
   prox_syn->DisconnectAllItems();
  }
  }
 }

 UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
  MakeLearnerDendriteName(dendrite_index0 + 1, tip_seg), true);
 if(!dendrite)
  return false;
 const int nsyn = IsParametricNormalization() ? 1 : NumSynapse[dendrite_index0];
 for(int k = 0; k < nsyn; k++)
 {
  NPulseSynapseCommon *synapse = dendrite->GetComponentL<NPulseSynapseCommon>(
   std::string("ExcSynapse") + sntoa(k + 1), true);
  if(!synapse)
   return false;
  if(k == 0 && IsParametricNormalization())
  {
   double r = SynapseResistanceBase.GetData();
   if(dendrite_index0 < int(TipSynapseResistance.size()))
    r = TipSynapseResistance[static_cast<size_t>(dendrite_index0)];
   if(r <= 0.0)
    r = kSynapseResistanceBioDefault;
   synapse->Resistance = ClampResistance(r);
  }
  else if(k != 0)
   synapse->Resistance = SynapseResistanceStep;
  if(!LinkSynapseToDataset(synapse))
   return false;
  if(!synapse->Input.IsConnected())
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
 Dataset->Delay = EffectiveDatasetDelaySec();
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
 NormalizationMode("NormalizationMode", this, &NNeuronTimeLearner::SetNormalizationMode),
 SynapseResistanceBase("SynapseResistanceBase", this, &NNeuronTimeLearner::SetSynapseResistanceBase),
 ResistanceMin("ResistanceMin", this, &NNeuronTimeLearner::SetResistanceMin),
 ResistanceMax("ResistanceMax", this, &NNeuronTimeLearner::SetResistanceMax),
 AttenuationGamma("AttenuationGamma", this, &NNeuronTimeLearner::SetAttenuationGamma),
 ResistanceAdjustGain("ResistanceAdjustGain", this, &NNeuronTimeLearner::SetResistanceAdjustGain),
 TipSynapseResistance("TipSynapseResistance", this, &NNeuronTimeLearner::SetTipSynapseResistance),
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
 HasPrevPeakSnapshot = false;
 ActiveDendrite = 0;
 EstDelayPerSeg = kDelayPerSegDefault;
 LastLengthDelta = 0;
 LastLengthDeltaDendrite = -1;
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
  if(UntrainedTipSynapseResistance.size() == static_cast<size_t>(NumInputDendrite))
   TipSynapseResistance.SetDataDirect(UntrainedTipSynapseResistance);
 }
 else
 {
  DendriteLength.assign(NumInputDendrite, 1);
  NumSynapse.assign(NumInputDendrite, 1);
  InitialSomaPotential.assign(NumInputDendrite, 0.0);
  TipSynapseResistance.assign(NumInputDendrite, SynapseResistanceBase.GetData());
 }
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
  CanChangeDendLength = true;
  HasPrevPeakSnapshot = false;
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
  Dataset->Delay = std::max(value > 0.0 ? value : 0.5, SettleMarginSec() + kGapSlack);
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

 // During XML load NumInputDendrite may already be 4 while DendriteLength is still
 // the default size-1 vector — never index past DendriteLength.size().
 const int n = std::min(NumInputDendrite.GetData(), int(DendriteLength.size()));
 for (int i = 0; i < n; i++)
 {
  if (DendriteLength[i] > value)
  {
   DendriteLength[i] = value;

   if (!Neuron)
    continue;

   if(Neuron->StructureBuildMode != 2)
   {
    Neuron->NumDendriteMembraneParts = 1;
   }
   else
   {
    vector<int> temp;
    temp = Neuron->NumDendriteMembranePartsVec;
    if(i < int(temp.size()))
    {
     temp[i] = value;
     Neuron->NumDendriteMembranePartsVec = temp;
    }
   }
   Neuron->Reset();

   UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
    MakeLearnerDendriteName(i + 1, DendriteLength[i]), true);
   if(!dendrite)
    continue;

   dendrite->NumExcitatorySynapses = (i < int(NumSynapse.size())) ? NumSynapse[i] : 1;
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
 if(IsParametricNormalization())
  return true;
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

bool NNeuronTimeLearner::SetNormalizationMode(const int &value)
{
 if(value != kNormStructural && value != kNormParametric)
  return false;
 if(value == kNormParametric)
 {
  EnforceParametricSynapseCount();
  if(Neuron)
  {
   std::vector<double> tips = TipSynapseResistance.GetData();
   if(tips.size() != static_cast<size_t>(NumInputDendrite))
    tips.resize(static_cast<size_t>(NumInputDendrite), SynapseResistanceBase.GetData());
   for(int i = 0; i < NumInputDendrite; ++i)
   {
    if(NPulseSynapseCommon *syn = GetTipSynapse(i))
     tips[static_cast<size_t>(i)] = syn->Resistance;
   }
   TipSynapseResistance.SetDataDirect(tips);
  }
 }
 return true;
}

bool NNeuronTimeLearner::SetSynapseResistanceBase(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetResistanceMin(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetResistanceMax(const double &value)
{
 if(value <= 0.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetAttenuationGamma(const double &value)
{
 (void)value;
 return true;
}

bool NNeuronTimeLearner::SetResistanceAdjustGain(const double &value)
{
 if(value <= 0.0 || value > 1.0)
  return false;
 return true;
}

bool NNeuronTimeLearner::SetTipSynapseResistance(const std::vector<double> &value)
{
 if(value.size() != static_cast<size_t>(NumInputDendrite))
 {
  std::vector<double> normalized(NumInputDendrite, SynapseResistanceBase.GetData());
  const size_t copyCount = std::min(normalized.size(), value.size());
  for(size_t i = 0; i < copyCount; ++i)
   normalized[i] = ClampResistance(value[i]);
  TipSynapseResistance.SetDataDirect(normalized);
 }
 else
 {
  std::vector<double> clamped = value;
  for(size_t i = 0; i < clamped.size(); ++i)
   clamped[i] = ClampResistance(clamped[i]);
  TipSynapseResistance.SetDataDirect(clamped);
 }
 if(Neuron && IsParametricNormalization())
 {
  for(int i = 0; i < NumInputDendrite; ++i)
   SetTipSynapseResistanceOnComponent(i, TipSynapseResistance[i]);
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

 if(Neuron->StructureBuildMode != 2)
 {
  Neuron->StructureBuildMode = 2;
 }
 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 // Force neuron ABuild so soma/dendrite cables and Pos/Neg generator links match tips.
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "BuildStructure: Neuron->Build failed after length sync");
  return false;
 }
 Neuron->Reset();

 if (EnableDebug.GetData() && RDK::GetLogger())
 {
  std::ostringstream oss;
  oss << "BuildStructure: DendriteLength=[";
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(i) oss << ',';
   oss << DendriteLength[i];
  }
  oss << "] NeuronClass=" << NeuronClassName.GetData();
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }

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
 if(IsParametricNormalization())
  EnforceParametricSynapseCount();

 // Neuron->Build already creates tips with default NumExcitatorySynapses=1.
 // Avoid a blanket tip->Build() here: it historically left ExcSynapse Input dead
 // until a later ApplyPending Invalidate+Relink cycle (amp=0 on L=1 cold start).
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

  if(int(inputsegmentofdendrite->NumExcitatorySynapses) != NumSynapse[i])
  {
   inputsegmentofdendrite->NumExcitatorySynapses = NumSynapse[i];
   inputsegmentofdendrite->Build();
  }
  if(!inputsegmentofdendrite->GetActivity())
   inputsegmentofdendrite->SetActivity(true);
  if(!inputsegmentofdendrite->IsInit())
   inputsegmentofdendrite->Init();
 }

 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(gen)
  gen->DisconnectAll("Output");

 for(int numdend = 0; numdend < NumInputDendrite; numdend++)
 {
  bool linked = RelinkDendriteSynapsesToDataset(numdend);
  if(!linked)
  {
   UEPtr<NPulseMembrane> tip = Neuron->GetComponentL<NPulseMembrane>(
    MakeLearnerDendriteName(numdend + 1, DendriteLength[numdend]), true);
   if(tip)
   {
    tip->NumExcitatorySynapses = NumSynapse[numdend];
    tip->Build();
    if(!tip->GetActivity())
     tip->SetActivity(true);
    if(!tip->IsInit())
     tip->Init();
    linked = RelinkDendriteSynapsesToDataset(numdend);
   }
  }
  if(!linked)
   res = false;
 }

 if(gen)
  gen->Reset();

 // Cold-start: first Relink after Dataset Build often leaves L=1 tips with amp=0
 // (links report connected). A second cable Build+Relink matches the first
 // ApplyPending cycle that historically woke the tips.
 if(gen)
  gen->DisconnectAll("Output");
 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "BuildStructure: Neuron->Build wake-pass failed");
  return false;
 }
 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();
 for(int numdend = 0; numdend < NumInputDendrite; numdend++)
 {
  if(!RelinkDendriteSynapsesToDataset(numdend))
   res = false;
  if(IsParametricNormalization())
   SetTipSynapseResistanceOnComponent(numdend, TipSynapseResistance[numdend]);
 }
 if(gen)
  gen->Reset();

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
 NormalizationMode = kNormParametric;
 SynapseResistanceBase = kSynapseResistanceBioDefault;
 ResistanceMin = 1.0e6;
 ResistanceMax = 1.0e11;
 AttenuationGamma = kAttenuationGammaAuto;
 ResistanceAdjustGain = kResistanceAdjustGainDefault;
 TipSynapseResistance.assign(NumInputDendrite, SynapseResistanceBase.GetData());
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
  UntrainedTipSynapseResistance = TipSynapseResistance.GetData();
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


bool NNeuronTimeLearner::ApplyPendingDendriteLengthChanges(void)
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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }

 if(!Neuron)
  return false;

 NPulseGeneratorTransit *gen = GetDatasetGenerator();
 if(gen)
  gen->DisconnectAll("Output");

 Neuron->NumDendriteMembranePartsVec = DendriteLength;
 Neuron->StructureBuildMode = 2;
 if(!Neuron->Build())
 {
  LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
               "ApplyPendingDendriteLengthChanges: Neuron->Build failed");
  return false;
 }

 // Neuron->Build already creates tip membranes with default NumExcitatorySynapses=1
 // and cable/PosNeg links. Only rebuild a tip when synapse count must change;
 // a blanket tip->Build() after cable linking has left ExcSynapse Input dead
 // for Dataset fan-out on newly added distal segments.
 for(int i : changed)
 {
  UEPtr<NPulseMembrane> tip = Neuron->GetComponentL<NPulseMembrane>(
   MakeLearnerDendriteName(i + 1, DendriteLength[i]), true);
  if(!tip)
  {
   LogMessageEx(RDK_EX_ERROR, "NNeuronTimeLearner",
                std::string("ApplyPendingDendriteLengthChanges: missing tip ")
                + MakeLearnerDendriteName(i + 1, DendriteLength[i]));
   return false;
  }
  if(int(tip->NumExcitatorySynapses) != NumSynapse[i])
  {
   tip->NumExcitatorySynapses = NumSynapse[i];
   tip->Build();
  }
  if(!tip->GetActivity())
   tip->SetActivity(true);
  if(!tip->IsInit())
   tip->Init();

  if(DendriteLength[i] > OldDendriteLength[i] && DendriteLength[i] > 1)
  {
   UEPtr<NPulseMembrane> prevdendrite = Neuron->GetComponentL<NPulseMembrane>(
    MakeLearnerDendriteName(i + 1, OldDendriteLength[i]), true);
   // Previous tip keeps one ExcSynapse for structure, but must not stay
   // driven by Dataset — RelinkDendriteSynapsesToDataset disconnects it.
   if(prevdendrite && int(prevdendrite->NumExcitatorySynapses) != 1)
   {
    prevdendrite->NumExcitatorySynapses = 1;
    prevdendrite->Build();
   }
  }
 }

 Neuron->Reset();
 Neuron->InvalidateActiveComponentsCache();

 for(int d = 0; d < NumInputDendrite; ++d)
 {
  bool linked = RelinkDendriteSynapsesToDataset(d);
  // After length change, a failed tip link often leaves amp=0; one Build+Relink retry.
  if(!linked)
  {
   const bool was_changed = std::find(changed.begin(), changed.end(), d) != changed.end();
   if(was_changed)
   {
    UEPtr<NPulseMembrane> tip = Neuron->GetComponentL<NPulseMembrane>(
     MakeLearnerDendriteName(d + 1, DendriteLength[d]), true);
    if(tip)
    {
     tip->NumExcitatorySynapses = NumSynapse[d];
     tip->Build();
     if(!tip->GetActivity())
      tip->SetActivity(true);
     if(!tip->IsInit())
      tip->Init();
     if (EnableDebug.GetData() && RDK::GetLogger())
     {
      std::ostringstream oss;
      oss << "ApplyPendingDendriteLengthChanges: tip Build+Relink retry dend="
          << d << " L=" << DendriteLength[d] << " Ns=" << NumSynapse[d];
      RDK::GetLogger()->LogMessageEx(RDK_EX_WARNING, "NNeuronTimeLearner", oss.str());
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

 if(gen)
  gen->Reset();
 return res;
}


bool NNeuronTimeLearner::ChangeSynapseNumber(int num)
{
 if(IsParametricNormalization())
 {
  SynapseStatus[num] = 0;
  if(NumSynapse[num] != 1)
  {
   NumSynapse[num] = 1;
   if(Neuron)
   {
    UEPtr<NPulseMembrane> dendrite = Neuron->GetComponentL<NPulseMembrane>(
     MakeLearnerDendriteName(num + 1, DendriteLength[num]), true);
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
 // Allow dendritic delay up to settle margin that scales with max length.
 const double settle = SettleMarginSec();

 for(int i = 0; i < NumInputDendrite; ++i)
 {
  if(i < int(PeakLocked.size()) && PeakLocked[i])
   continue;

  const double t_pulse = FirstImpulseTime
   + ((i < int(ExpectedPulseRelTimes.size())) ? ExpectedPulseRelTimes[i] : 0.0);
  if(now + 1e-12 < t_pulse)
   continue;

  // Track own-pulse soma peak near Expected[i]+cable(L), not a later pulse's wave.
  const double delay_est = std::max(0.0,
   (DendriteLength[i] > 1) ? (DendriteLength[i] - 1) * EstDelayPerSeg : 0.0);
  const double expected_i = (i < int(ExpectedPulseRelTimes.size()))
   ? ExpectedPulseRelTimes[static_cast<size_t>(i)] : 0.0;
  const double margin = std::max(0.06, 0.5 * double(kMaxLengthStep) * EstDelayPerSeg);
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

   // Latch Initial only on L==1 (full short-cable peak). Updating at L>1 while
   // Initial<=0 froze the anchor on the rising-edge sample after early growth.
   if(DendriteLength[i] == 1 && NumSynapse[i]
      && currentsomaamp > InitialSomaPotential[i])
    InitialSomaPotential[i] = currentsomaamp;

   if(DendriteLength[i] == 1 && NumSynapse[i] == 1 && IsParametricNormalization())
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


bool NNeuronTimeLearner::ChangeDendriteStatus(int num)
{
 if(num == NumInputDendrite - 1)
 {
  DendStatus[num] = 0;
  Dissynchronization[num] = 0.0;
  return true;
 }

 if(num != ActiveDendrite)
 {
  DendStatus[num] = 0;
  return true;
 }

 const int ref = NumInputDendrite - 1;
 const bool num_valid = (num < int(SomaPeakValid.size())) && SomaPeakValid[num];
 const bool prev_ref_valid = HasPrevPeakSnapshot
  && (ref < int(PrevPeakValid.size())) && PrevPeakValid[static_cast<size_t>(ref)];

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
   const double needed = PrevPeakRel[static_cast<size_t>(ref)] - expected_k;
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
     RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
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
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
  }
  return true;
 }

 const double expected_k = (num < int(ExpectedPulseRelTimes.size()))
  ? ExpectedPulseRelTimes[static_cast<size_t>(num)] : 0.0;
 const double needed = PrevPeakRel[static_cast<size_t>(ref)] - expected_k;
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
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
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
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
 return true;
}


bool NNeuronTimeLearner::ChangeSynapseStatus(int num)
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
 if(!HasPrevPeakSnapshot)
  return false;

 const int n = NumInputDendrite.GetData();
 const int ref = n - 1;
 if(n < 2)
  return true;

 if(ref < int(SomaPeakValid.size()) && !SomaPeakValid[static_cast<size_t>(ref)])
  return false;
 if(ref >= int(PrevPeakValid.size()) || !PrevPeakValid[static_cast<size_t>(ref)])
  return false;

 const double tol = SyncTolerance.GetData();
 for(int i = 0; i < n - 1; i++)
 {
  const bool best_effort = (i < int(DendBestEffortSynced.size()))
   && DendBestEffortSynced[static_cast<size_t>(i)];
  if(best_effort)
   continue;
  const bool length_ok = (i < int(DendLastAbsDt.size()))
   && (DendLastAbsDt[static_cast<size_t>(i)] <= tol);
  const bool dead_tip = (i < int(MaxIterSomaAmp.size()))
   && (MaxIterSomaAmp[i] < kMinMeasurableSomaAmp);
  // Dead tip: PeakRel invalid, but cable |needed-delay_len| already within tol.
  if(dead_tip && length_ok)
   continue;
  if(i < int(SomaPeakValid.size()) && !SomaPeakValid[static_cast<size_t>(i)])
   return false;
  if(!length_ok)
   return false;
 }
 return true;
}

bool NNeuronTimeLearner::AllSynapsesNormalized(void) const
{
 const double eps = 0.000005;
 if(IsParametricNormalization())
 {
  const double rmin = ResistanceMin.GetData();
  for(int i = 0; i < NumInputDendrite; i++)
  {
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
   if(dead_tip && length_ok)
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

 for(int i = 0; i < NumInputDendrite; i++)
 {
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
  const bool length_ok = (i < int(DendLastAbsDt.size())
   && DendLastAbsDt[static_cast<size_t>(i)] <= SyncTolerance.GetData())
   || ((i < int(DendBestEffortSynced.size()))
       && DendBestEffortSynced[static_cast<size_t>(i)]);
  if(dead_tip && length_ok)
   continue;
  if(at_cap)
   continue; // best-effort amp after synapse cap
  return false;
 }
 return true;
}

bool NNeuronTimeLearner::EndOfLearning(void)
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

 TrainingPhase = kPhaseDone;
 CanChangeDendLength = false;
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
  // FirstImpulseTime is the detected first pulse. Rel times are gaps after that pulse,
  // so Expected[0]=0 (not cumsum including the pre-first ISI — that shifted measure
  // windows +0.01 and missed L=1 peaks at ~FI+0.008).
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
  RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
 }
}

void NNeuronTimeLearner::FinishTrainingIteration(void)
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
   RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner", oss.str());
  }
 }

 ComputePeakRelAndDelay();

 if(TrainingPhase != kPhaseDone && IsNeedToTrain)
 {
  ActiveDendrite = SelectActiveDendrite();
  DendStatus.assign(static_cast<size_t>(NumInputDendrite.GetData()), 0);
  if(!HasPrevPeakSnapshot)
  {
   if (EnableDebug.GetData() && RDK::GetLogger())
    RDK::GetLogger()->LogMessageEx(RDK_EX_DEBUG, "NNeuronTimeLearner",
     "FinishTrainingIteration: bootstrap waiting valid ref peak (no growth)");
  }
  else
  {
   ChangeDendriteStatus(ActiveDendrite); // length: one active dendrite
   // Refresh abs-dt / best-effort before synapse status so dead-tip + length_ok
   // can clear SynapseStatus on the same burst (not one rotation later).
   RefreshDendLastAbsDtFromPrevAnchor();
   for(int i = 0; i < NumInputDendrite; i++)
    ChangeSynapseStatus(i); // amp: all somas
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

 // Apply length then synapses in one inter-burst gap (tip after Build/relink).
 if(TrainingPhase != kPhaseDone && IsNeedToTrain && CanChangeDendLength)
  ApplyPendingDendriteLengthChanges();
 if(TrainingPhase != kPhaseDone && IsNeedToTrain)
 {
  for(int i = 0; i < NumInputDendrite; ++i)
  {
   if(IsParametricNormalization())
    ApplySynapseResistanceChange(i);
   else
    ChangeSynapseNumber(i);
  }
 }

 // Snapshot this burst for next iteration's Prev* comparison (before CountIteration++).
 CommitPrevPeakSnapshot();

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
   can_start = (now - PrevFirstImpulseTime) >= EffectiveIterationGapSec();

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
