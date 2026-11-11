#include "Normalization.h"
#include <cmath>
#include <algorithm>
#include <sstream>

double Q_N(double x, int N)
{
    if (x <= 0.0) return 1.0;
    double term = std::exp(-x);   // k = 0
    double sum = term;
    for (int k = 1; k < N; k++) {
        term *= x / k;
        sum += term;
    }
    return sum;
}

double peakTime(double tau, double T, int N)
{
    if (N <= 1) return tau;       // один сегмент: пик в конце импульса
    // f(x1) = f(x2), x1 - x2 = tau/T  =>  t_peak = tau / (1 - exp(-tau / (T (N-1))))
    return tau / (1.0 - std::exp(-tau / (T * (N - 1.0))));
}

double shapeFactor(double tau, double T, int N)
{
    double tp = peakTime(tau, T, N);
    double x1 = tp / T;
    double x2 = (tp - tau) / T;
    return Q_N(x2, N) - Q_N(x1, N);
}

DendriteNorm normalizeDendrite(const NormParams& p, int N, double T)
{
    DendriteNorm r;
    r.N = N;
    r.T = T;

    const SegmentParams& b = p.base;
    if (p.tau <= 0.0) { r.message = "tau must be > 0"; return r; }
    if (p.E <= 0.0)   { r.message = "E must be > 0"; return r; }
    if (N < 1)        { r.message = "N must be >= 1"; return r; }
    if (T <= 0.0)     { r.message = "T (recom_T) must be > 0"; return r; }
    if (p.tolerance < 0.0)              { r.message = "tolerance must be >= 0"; return r; }
    if (!(p.kMax > 0.0 && p.kMax < 1.0)) { r.message = "kMax must be in (0, 1)"; return r; }
    if (b.g <= 0.0 || b.w <= 0.0 || b.n < 1 || b.C_m <= 0.0) {
        r.message = "invalid base segment parameters";
        return r;
    }

    const double gOne = b.g * b.w;                 // i_s / U0 для одного синапса

    // --- текущее состояние: n0, C0 при заданном T ---
    //   R' = T / C0,  k0 = g_sum0 * R'
    const double k0 = b.n * gOne * T / b.C_m;
    if (!(k0 < 1.0)) {
        r.message = "base segment inconsistent with T: g_sum*T/C_m >= 1 (R_m would be negative)";
        return r;
    }

    const double S1 = shapeFactor(p.tau, T, 1);
    const double SN = shapeFactor(p.tau, T, N);
    r.tPeak = peakTime(p.tau, T, N);
    r.ampMax = p.E * SN;

    // Эталоная амплитуда
    r.ampRef = (p.ref == RefMode::Input) ? p.E : p.E * k0 * S1;

    // Амплитуда при текущей длине и текущем T
    r.ampCurrent = p.E * k0 * SN;

    // Подгонка. Нужный коэффициент передачи: E * k * S_N = ampRef
    double kt = r.ampRef / (p.E * SN);
    if (kt > p.kMax) {              // эталон недостижим: берём лучшее достижимое приближение
        kt = p.kMax;
        r.clamped = true;
    }

    // n ближайшее к тому, при котором C_m остаётся базовой: k = n*g*w*T/C0
    int n = std::max(1, static_cast<int>(std::lround(kt * b.C_m / (gOne * T))));

    if (p.exactRC) {
        // R_m, C_m подгоняются: T не меняется
        //   C_m = g_sum*T/k,  R_m = k / (g_sum (1-k))
        const double gSum = gOne * n;
        r.n = n;
        r.k = kt;
        r.C_m = gSum * T / kt;
        r.R_m = kt / (gSum * (1.0 - kt));
    } else {
        // меняется только n; C_m базовая, R' = T/C0, R_m выводится из T
        double nMax = std::ceil(b.C_m / (gOne * T)) - 1.0;   // наибольшее n с k < 1
        if (nMax < 1.0) { r.message = "no n gives k < 1 with base C_m"; return r; }
        n = std::min(n, static_cast<int>(nMax));
        const double gSum = gOne * n;
        r.n = n;
        r.C_m = b.C_m;
        r.k = gSum * T / b.C_m;
        r.R_m = r.k / (gSum * (1.0 - r.k));
    }

    r.ampAchieved = p.E * r.k * SN;
    r.relErr = (r.ampAchieved - r.ampRef) / r.ampRef;

    // принимаем результат, если отклонение в допуске
    r.ok = std::fabs(r.relErr) <= p.tolerance + 1e-12;
    std::ostringstream os;
    if (r.ok) {
        os << (r.clamped ? "within tolerance: target unreachable, k limited to kMax"
                         : "ok");
    } else {
        os << "deviation " << r.relErr * 100.0 << "% exceeds tolerance "
           << p.tolerance * 100.0 << "%";
        if (r.clamped) os << " (target unreachable at T=" << T << ", N=" << N
                          << ": max " << r.ampMax << " < ref " << r.ampRef << ")";
    }
    r.message = os.str();
    return r;
}

NormOutput normalize(const NormParams& p,
                     const std::vector<double>& lengths,
                     const std::vector<double>& recomT)
{
    NormOutput out;
    const std::size_t m = lengths.size();
    out.R_m.assign(m, 0.0);
    out.C_m.assign(m, 0.0);
    out.n.assign(m, 0);
    out.details.reserve(m);
    out.allOk = true;

    for (std::size_t i = 0; i < m; i++) {
        DendriteNorm r;
        if (i >= recomT.size()) {
            r.message = "recom_T has no value for this dendrite";
        } else {
            // длина < 1 (в т.ч. 0 для t_bias = 0) трактуется как один сегмент
            int N = std::max(1, static_cast<int>(std::lround(lengths[i])));
            r = normalizeDendrite(p, N, recomT[i]);
        }
        if (r.ok) {
            out.R_m[i] = r.R_m;
            out.C_m[i] = r.C_m;
            out.n[i]   = r.n;
        } else {
            out.allOk = false;
        }
        out.details.push_back(r);
    }
    return out;
}

NormOutput normalize(const NormParams& p, const GetRecParam& rec)
{
    return normalize(p, rec.recomDendriteLength, rec.recom_T);
}
