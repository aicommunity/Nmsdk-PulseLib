#pragma once
#include <string>
#include <vector>
#include "RecommendedPar.h"

/* Модель выходной амплитуды дендрита из N сегментов:

    A_out = E * k * S_N(T),  k = g_sum * R',  S_N = Q_N(x2) - Q_N(x1)
    g_sum = n * g * w  (i_s/U0 = g*w, U0 сокращается),  T = R' * C_m,
    1/R' = g_sum + 1/R_m.

    Базовый (текущий) сегмент. T приходит из AutoPreset (recom_T),
    R' = T / C_m, а R_m выводится.

*/

struct SegmentParams
{
    double g;     // проводимость одного синапса при w = 1
    double w;     // вес синапса
    int    n;     // исходное число синапсов
    double C_m;   // исходная ёмкость
};

enum class RefMode
{
    Input,       // эталон = входная амплитуда E (достижим только если E <= E*S_N, т.е. почти никогда)
    OneSegment   // эталон = амплитуда дендрита из одного сегмента при том же T
};

struct NormParams
{
    double tau;                       // длительность входного импульса
    double E;                         // входная амплитуда
    SegmentParams base;
    RefMode ref = RefMode::OneSegment;
    bool exactRC = true;              // true: n подбирается, R_m и C_m подгоняются под точное равенство
                                      // false: меняется только n (C_m = базовая), остаётся остаточная ошибка

    /*  Допустимое относительное отклонение |A_achieved - A_ref| / A_ref.

        Если эталон недостижим (нужное k > kMax), берётся лучшее достижимое k = kMax,
        результат принимается (ok = true), когда отклонение укладывается в tolerance.

    */

    double tolerance = 0.05;
    double kMax = 0.999;              // верхний предел k = g_sum*R' (k -> 1 означает R_m -> inf)
};


// Результат для одного дендрита
struct DendriteNorm
{
    bool        ok = false;
    std::string message;

    int    N = 0;            // длина дендрита (сегментов)
    double T = 0.0;          // recom_T (не меняется)
    int    n = 0;            // подобранное число синапсов
    double R_m = 0.0;
    double C_m = 0.0;
    double k = 0.0;          // k = g_sum * R' после подбора

    double tPeak = 0.0;
    double ampRef = 0.0;      // эталонная амплитуда
    double ampCurrent = 0.0;  // амплитуда при исходных n, C_m (до подбора)
    double ampAchieved = 0.0; // амплитуда после подбора
    double ampMax = 0.0;      // предел при данном T и N (k -> 1): E * S_N
    double relErr = 0.0;      // (ampAchieved - ampRef) / ampRef
    bool   clamped = false;   // эталон недостижим: k ограничено kMax (параметры - лучшее приближение)
};

// Итог по всем дендритам: те самые векторы, которые ожидаются на выходе
struct NormOutput
{
    std::vector<double> R_m;
    std::vector<double> C_m;
    std::vector<int>    n;

    std::vector<DendriteNorm> details;   // подробности и диагностика по каждому дендриту
    bool allOk = false;
};

double Q_N(double x, int N);
double peakTime(double tau, double T, int N);
double shapeFactor(double tau, double T, int N);   // S_N

// Один дендрит: длина N (сегментов) и T (recom_T)
DendriteNorm normalizeDendrite(const NormParams& p, int N, double T);

// Все дендриты: lengths = recomDendriteLength, recomT = recom_T
NormOutput normalize(const NormParams& p,
                     const std::vector<double>& lengths,
                     const std::vector<double>& recomT);

// Удобная обёртка над результатом Auto_Preset::getResult()
NormOutput normalize(const NormParams& p, const GetRecParam& rec);
