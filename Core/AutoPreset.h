#pragma once
#include <math.h>
#include <vector>
#include <algorithm>
#include "RecommendedPar.h"

class Auto_Preset
{
  private:

    double tau;
    std::vector<double> T;
    std::vector<double> t_bias;
    std::vector<double> dendrite;

    bool deltaFn;
    double t_max;

    std::vector<double> recomDendrite;
    std::vector<double> recom_T;
    std::vector<double> peakErr;

    bool validation(const std::vector<double>& pattern);
    void findDendriteLength();
    void findRecommendedT(const std::vector<double>& dendrite);
    void findPeakErr(const std::vector<double>& pattern);

  public:
    void setFirstState(const PushParam& param);
    GetRecParam getResult() const;
};

