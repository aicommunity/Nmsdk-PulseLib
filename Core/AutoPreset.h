#pragma once
#include <math.h>
#include <vector>
#include <algorithm>
#include "RecommendedPar.h"

class Auto_Preset
{
  private:
    bool deltaFn;
    double tau;
    double t_max;
    std::vector<double> T;
    std::vector<double> t_bias;
    std::vector<double> dendrite;
    std::vector<double> recom_T;

    bool validation(const std::vector<double>& pattern);
    void findDendriteLength();
    void findRecommendedT();

  public:
    void setFirstState(const std::vector<double>&  pattern, double T, double tau);
    void setFirstState(const std::vector<double>&  pattern, double R, double C, double tau);
    void setFirstState(const std::vector<double>&  pattern, const std::vector<double>&  VectorT, double tau);
    gParam getResult() const;
};

