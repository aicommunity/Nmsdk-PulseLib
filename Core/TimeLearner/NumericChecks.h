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
}
