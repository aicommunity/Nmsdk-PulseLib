#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

namespace NMSDK
{
namespace PostTrainTune
{

inline constexpr int kPostTipOff = 0;
inline constexpr int kPostTipCanonRmin = 1;
inline constexpr int kPostTipFlatLastR = 2;
inline constexpr int kPostTipKeepDone = 3;
inline constexpr int kPostTipSearchSynthetic = 4;

inline constexpr int kMidMetricAuto = 0;
inline constexpr int kMidMetricLtz = 1;
inline constexpr int kMidMetricSoma = 2;

/// PostTuneResult pub-state codes (Complete ≠ quality PASS).
inline constexpr int kResultNone = 0;
inline constexpr int kResultSuccess = 1;
inline constexpr int kResultNonSeparable = 2;
inline constexpr int kResultSetupFailure = 3;
inline constexpr int kResultTimeout = 4;
inline constexpr int kResultInvalidMetrics = 5;
/// Ideal LandscapeOk failed, but applied candidate mid so target can still react.
inline constexpr int kResultBestEffort = 6;
/// TipR reverted to PostTuneTipSnapshot (KeepDone); mid left silent for Test inference.
inline constexpr int kResultRevertedKeepDone = 7;

/// Per-sample free-run observation (0 amp ≠ unobserved).
struct SampleMetricState
{
 double peak;
 bool observed;
 bool finished;
 bool finite_ok;
 SampleMetricState()
  : peak(0.0), observed(false), finished(false), finite_ok(true) {}
};

inline bool AllSamplesValid(const std::vector<SampleMetricState> &states)
{
 if(states.empty())
  return false;
 for(size_t i = 0; i < states.size(); ++i)
 {
  if(!states[i].finished || !states[i].observed || !states[i].finite_ok)
   return false;
 }
 return true;
}

/// Legacy synthetic foils (shifts / scale / zero-one). Kept for SearchSynthetic.
std::vector<std::vector<double> > BuildSyntheticPatterns(
 const std::vector<double> &target_isi,
 int foil_cap);

/// Recognition foils matching AsymRm pack A / phase8 silent mid:
/// fixed onset, body mass redistributed (N=4 uses pack-A fraction table).
/// Prefer this for PostTune mid (not scale ×0.5/×2).
std::vector<std::vector<double> > BuildRecognitionProbePatterns(
 const std::vector<double> &target_isi,
 int foil_cap);

/// mid = 0.5*(tgt+max_foil_below) else tgt*0.99; gap = tgt - foil_ref
void ComputeMidThreshold(
 double tgt,
 const std::vector<double> &foil_metrics,
 double &mid_out,
 double &gap_out);

inline bool MetricsFinite(double tgt, const std::vector<double> &foil_metrics)
{
 if(!std::isfinite(tgt))
  return false;
 for(size_t i = 0; i < foil_metrics.size(); ++i)
 {
  if(!std::isfinite(foil_metrics[i]))
   return false;
 }
 return true;
}

/// Stage0 Train emit: sep_gap = tgt - max_foil (same gap as ComputeMidThreshold);
/// sep_margin = sep_gap/tgt when metrics_usable && finite && tgt>0; else NaN.
/// Does not change mid policy — emit-only for rollup / Controller v2.
inline void FillSepMarginFields(
 bool metrics_usable,
 double tgt,
 const std::vector<double> &foil_metrics,
 double &sep_tgt_out,
 double &sep_max_foil_out,
 double &sep_gap_out,
 double &sep_margin_out)
{
 const double nan = std::numeric_limits<double>::quiet_NaN();
 sep_tgt_out = nan;
 sep_max_foil_out = nan;
 sep_gap_out = nan;
 sep_margin_out = nan;
 if(!metrics_usable || !MetricsFinite(tgt, foil_metrics) || !(tgt > 0.0))
  return;
 double mid = 0.0;
 double gap = 0.0;
 ComputeMidThreshold(tgt, foil_metrics, mid, gap);
 sep_tgt_out = tgt;
 sep_gap_out = gap;
 sep_max_foil_out = tgt - gap;
 sep_margin_out = gap / tgt;
}

/// True iff every foil is finite and strictly below tgt (usable landscape).
inline bool LandscapeOk(
 double tgt,
 const std::vector<double> &foil_metrics,
 double eps = 1e-12)
{
 if(!MetricsFinite(tgt, foil_metrics))
  return false;
 for(size_t i = 0; i < foil_metrics.size(); ++i)
 {
  if(foil_metrics[i] + eps >= tgt)
   return false;
 }
 return true;
}

std::vector<double> MakeCanonRminVector(int n, double floor_r, double last_r);
std::vector<double> MakeFlatLastVector(int n, double last_r);

bool PatternsEqual(const std::vector<double> &a, const std::vector<double> &b, double eps = 1e-12);
double PatternSum(const std::vector<double> &isi);

/// Test inference only (UseCandidateMidOnNonSeparableTest=1): replace silent
/// PostTrainSilentThreshold with finite probe candidate_mid in (0,1). Keeps
/// PostTuneResult (NonSeparable remains quality_fail); does not affect Train.
inline bool ApplyCandidateMidOnNonSeparableTest(
 bool inference,
 bool use_candidate_mid_on_nonseparable_test,
 double silent_threshold,
 int post_tune_result,
 double candidate_mid,
 double &mid_in_out)
{
 if(!inference || !use_candidate_mid_on_nonseparable_test)
  return false;
 if(!std::isfinite(candidate_mid) || candidate_mid <= 0.0 || candidate_mid >= 1.0)
  return false;
 if(post_tune_result == kResultSuccess || post_tune_result == kResultBestEffort)
  return false;
 const bool nonsep = post_tune_result == kResultNonSeparable;
 const bool silent_mid = std::isfinite(silent_threshold)
  && mid_in_out + 1e-12 >= silent_threshold;
 if(!nonsep && !silent_mid)
  return false;
 mid_in_out = candidate_mid;
 return true;
}

} // namespace PostTrainTune
} // namespace NMSDK
