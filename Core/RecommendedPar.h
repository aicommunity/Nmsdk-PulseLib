#pragma once
#include <vector>
#include <optional>

struct GetRecParam
{
    std::vector<double> recomDendriteLength;
    std::vector<double> recom_T;
};

struct PushParam
{
    std::optional<double> tau;

    std::optional<std::vector<double>> pattern;

    std::optional<double> R;
    std::optional<double> C;

    std::optional<double> T;

    std::optional<std::vector<double>> vectorT;
};
