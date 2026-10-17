#pragma once
#include <vector>
#include <optional>

struct GetRecParam  // убрать глаголы
{
    std::vector<double> peakErr; // Оценка расогласования расчитывается от внесённых параметров

    std::vector<double> recomDendriteLength;
    std::vector<double> recom_T;
};

struct PushParam    // убрать глаголы
{
    std::optional<double> tau;

    std::optional<std::vector<double>> pattern;

    std::optional<double> R;
    std::optional<double> C;
    std::optional<double> T;
    std::optional<std::vector<double>> vectorT;

    std::optional<std::vector<double>> dendriteLength;
};
