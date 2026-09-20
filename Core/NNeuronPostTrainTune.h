#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
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

std::vector<double> MakeCanonRminVector(int n, double floor_r, double last_r);
std::vector<double> MakeFlatLastVector(int n, double last_r);

bool PatternsEqual(const std::vector<double> &a, const std::vector<double> &b, double eps = 1e-12);
double PatternSum(const std::vector<double> &isi);

} // namespace PostTrainTune
} // namespace NMSDK
