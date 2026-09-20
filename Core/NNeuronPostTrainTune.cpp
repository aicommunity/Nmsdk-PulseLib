#include "NNeuronPostTrainTune.h"

#include <set>
#include <sstream>

namespace NMSDK
{
namespace PostTrainTune
{

double PatternSum(const std::vector<double> &isi)
{
 double s = 0.0;
 for(size_t i = 0; i < isi.size(); ++i)
  s += (isi[i] < 0.0) ? 0.0 : isi[i];
 return s;
}

bool PatternsEqual(const std::vector<double> &a, const std::vector<double> &b, double eps)
{
 if(a.size() != b.size())
  return false;
 for(size_t i = 0; i < a.size(); ++i)
 {
  if(std::fabs(a[i] - b[i]) > eps)
   return false;
 }
 return true;
}

static std::string PatternKey(const std::vector<double> &isi)
{
 std::ostringstream oss;
 oss.precision(12);
 for(size_t i = 0; i < isi.size(); ++i)
 {
  if(i) oss << ',';
  oss << ((isi[i] < 0.0) ? 0.0 : isi[i]);
 }
 return oss.str();
}

static void PushUniquePattern(
 std::vector<std::vector<double> > &out,
 std::set<std::string> &seen,
 const std::vector<double> &p,
 const std::vector<double> &target,
 int max_foils)
{
 if(int(out.size()) - 1 >= max_foils)
  return;
 if(PatternSum(p) <= 1e-15)
  return;
 if(PatternsEqual(p, target))
  return;
 const std::string key = PatternKey(p);
 if(seen.count(key))
  return;
 seen.insert(key);
 out.push_back(p);
}

std::vector<std::vector<double> > BuildSyntheticPatterns(
 const std::vector<double> &target_isi,
 int foil_cap)
{
 std::vector<std::vector<double> > out;
 const int n = int(target_isi.size());
 if(n < 1)
  return out;

 std::vector<double> target = target_isi;
 for(int i = 0; i < n; ++i)
 {
  if(target[static_cast<size_t>(i)] < 0.0)
   target[static_cast<size_t>(i)] = 0.0;
 }
 out.push_back(target);

 std::vector<std::vector<double> > candidates;

 // Circular shifts
 for(int k = 1; k < n; ++k)
 {
  std::vector<double> p(static_cast<size_t>(n), 0.0);
  for(int i = 0; i < n; ++i)
   p[static_cast<size_t>(i)] = target[static_cast<size_t>((i + k) % n)];
  candidates.push_back(p);
 }

 // Adjacent swaps
 for(int i = 0; i + 1 < n; ++i)
 {
  std::vector<double> p = target;
  std::swap(p[static_cast<size_t>(i)], p[static_cast<size_t>(i + 1)]);
  candidates.push_back(p);
 }

 // Uniform scale 0.5 / 2.0
 {
  std::vector<double> half = target;
  std::vector<double> dbl = target;
  for(int i = 0; i < n; ++i)
  {
   half[static_cast<size_t>(i)] *= 0.5;
   dbl[static_cast<size_t>(i)] *= 2.0;
  }
  candidates.push_back(half);
  candidates.push_back(dbl);
 }

 // Zero one ISI
 for(int i = 0; i < n; ++i)
 {
  std::vector<double> p = target;
  p[static_cast<size_t>(i)] = 0.0;
  candidates.push_back(p);
 }

 std::set<std::string> seen;
 seen.insert(PatternKey(target));

 const int max_foils = (foil_cap > 0) ? foil_cap : 8;
 for(size_t c = 0; c < candidates.size(); ++c)
  PushUniquePattern(out, seen, candidates[c], target, max_foils);
 return out;
}

std::vector<std::vector<double> > BuildRecognitionProbePatterns(
 const std::vector<double> &target_isi,
 int foil_cap)
{
 std::vector<std::vector<double> > out;
 const int n = int(target_isi.size());
 if(n < 1)
  return out;

 std::vector<double> target = target_isi;
 for(int i = 0; i < n; ++i)
 {
  if(target[static_cast<size_t>(i)] < 0.0)
   target[static_cast<size_t>(i)] = 0.0;
 }
 out.push_back(target);

 const int max_foils = (foil_cap > 0) ? foil_cap : 8;
 std::set<std::string> seen;
 seen.insert(PatternKey(target));

 if(n == 1)
  return out;

 const double onset = target[0];
 double body = 0.0;
 for(int i = 1; i < n; ++i)
  body += target[static_cast<size_t>(i)];
 if(body <= 1e-15)
 {
  // Degenerate target: fall back to legacy synthetic (no scale preferred later).
  return BuildSyntheticPatterns(target_isi, foil_cap);
 }

 // AsymRm pack A body-mass fractions for N=4 (phase8 silent mid set).
 // onset fixed; remaining mass redistributed. Matches EXP_span*ms_packA_* MatrixData.
 if(n == 4)
 {
  static const double kPackABodyFrac[7][3] = {
   {0.5, 1.0 / 3.0, 1.0 / 6.0},           // reverse body
   {1.0 / 3.0, 1.0 / 6.0, 0.5},           // mid swap
   {1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0},     // equal
   {1.0 / 24.0, 11.0 / 24.0, 0.5},        // squeeze early
   {0.5, 11.0 / 24.0, 1.0 / 24.0},        // squeeze late
   {1.0 / 12.0, 1.0 / 12.0, 5.0 / 6.0},   // mass on last
   {5.0 / 6.0, 1.0 / 12.0, 1.0 / 12.0},   // mass on first body
  };
  for(int f = 0; f < 7; ++f)
  {
   std::vector<double> p(4, 0.0);
   p[0] = onset;
   p[1] = body * kPackABodyFrac[f][0];
   p[2] = body * kPackABodyFrac[f][1];
   p[3] = body * kPackABodyFrac[f][2];
   PushUniquePattern(out, seen, p, target, max_foils);
  }
  return out;
 }

 // General N: fixed-onset permutations of body + equal + extremes.
 {
  std::vector<double> body_vals(target.begin() + 1, target.end());
  std::sort(body_vals.begin(), body_vals.end());
  do
  {
   std::vector<double> p(static_cast<size_t>(n), 0.0);
   p[0] = onset;
   for(int i = 1; i < n; ++i)
    p[static_cast<size_t>(i)] = body_vals[static_cast<size_t>(i - 1)];
   PushUniquePattern(out, seen, p, target, max_foils);
   if(int(out.size()) - 1 >= max_foils)
    return out;
  } while(std::next_permutation(body_vals.begin(), body_vals.end()));
 }

 {
  std::vector<double> equal(static_cast<size_t>(n), 0.0);
  equal[0] = onset;
  const double slot = body / double(n - 1);
  for(int i = 1; i < n; ++i)
   equal[static_cast<size_t>(i)] = slot;
  PushUniquePattern(out, seen, equal, target, max_foils);
 }

 {
  std::vector<double> last_heavy(static_cast<size_t>(n), 0.0);
  last_heavy[0] = onset;
  const double tiny = body / double(12 * (n - 1));
  double used = 0.0;
  for(int i = 1; i + 1 < n; ++i)
  {
   last_heavy[static_cast<size_t>(i)] = tiny;
   used += tiny;
  }
  last_heavy[static_cast<size_t>(n - 1)] = body - used;
  PushUniquePattern(out, seen, last_heavy, target, max_foils);
 }

 {
  std::vector<double> first_heavy(static_cast<size_t>(n), 0.0);
  first_heavy[0] = onset;
  const double tiny = body / double(12 * (n - 1));
  double used = 0.0;
  for(int i = 2; i < n; ++i)
  {
   first_heavy[static_cast<size_t>(i)] = tiny;
   used += tiny;
  }
  first_heavy[1] = body - used;
  PushUniquePattern(out, seen, first_heavy, target, max_foils);
 }

 return out;
}

void ComputeMidThreshold(
 double tgt,
 const std::vector<double> &foil_metrics,
 double &mid_out,
 double &gap_out)
{
 std::vector<double> below;
 double max_any = 0.0;
 bool have_any = false;
 for(size_t i = 0; i < foil_metrics.size(); ++i)
 {
  const double s = foil_metrics[i];
  if(!have_any || s > max_any)
  {
   max_any = s;
   have_any = true;
  }
  if(s < tgt)
   below.push_back(s);
 }
 double foil = 0.0;
 if(!below.empty())
 {
  foil = below[0];
  for(size_t i = 1; i < below.size(); ++i)
   foil = std::max(foil, below[i]);
  mid_out = 0.5 * (tgt + foil);
 }
 else
 {
  foil = have_any ? max_any : 0.0;
  mid_out = tgt * 0.99;
 }
 gap_out = tgt - foil;
}

std::vector<double> MakeCanonRminVector(int n, double floor_r, double last_r)
{
 if(n < 1)
  n = 1;
 std::vector<double> v(static_cast<size_t>(n), floor_r);
 v[static_cast<size_t>(n - 1)] = last_r;
 return v;
}

std::vector<double> MakeFlatLastVector(int n, double last_r)
{
 if(n < 1)
  n = 1;
 return std::vector<double>(static_cast<size_t>(n), last_r);
}

} // namespace PostTrainTune
} // namespace NMSDK
