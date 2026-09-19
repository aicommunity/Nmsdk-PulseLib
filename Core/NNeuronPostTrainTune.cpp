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
 {
  if(int(out.size()) - 1 >= max_foils)
   break;
  std::vector<double> &p = candidates[c];
  if(PatternSum(p) <= 1e-15)
   continue;
  if(PatternsEqual(p, target))
   continue;
  const std::string key = PatternKey(p);
  if(seen.count(key))
   continue;
  seen.insert(key);
  out.push_back(p);
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
