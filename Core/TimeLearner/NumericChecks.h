#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

namespace NMSDK::TimeLearner::Detail {
inline bool Finite(double value) { return std::isfinite(value); }
// Floating-point comparison slack only; never a physical sync tolerance.
inline bool After(double a,double b)
{
    const double slack=32.*std::numeric_limits<double>::epsilon()*std::max({1.,std::fabs(a),std::fabs(b)});
    return a>b+slack;
}
// Preserve the scale of absolute endpoints. Comparing (end-begin) with a
// duration loses subtraction error when the model clock is far from zero.
inline bool IntervalAfter(double end,double begin,double duration)
{ return After(end,begin+duration); }
inline bool ResolvableTimeInterval(double begin,double end,double interval)
{
    const double roundoff=32.*std::numeric_limits<double>::epsilon()*
        std::max({1.,std::fabs(begin),std::fabs(end)});
    // If a declared sampling period cannot be distinguished from its own
    // timestamp uncertainty, reject it instead of accepting missed frames.
    return Finite(begin) && Finite(end) && Finite(interval) && interval>2.*roundoff;
}
}
