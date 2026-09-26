#include "iostream"
#include "AutoPreset.h"
#include "RecommendedPar.h"

bool Auto_Preset::validation(const std::vector<double>& pattern)
{
  bool err = false;

  if (pattern.size() == 0) {
    // добавить вывод ошибки
    std::cout << "pattern == 0!" <<'\n';
    err = true;
  }
  else if (tau < 0.0) {
    // добавить вывод ошибки
    std::cout << "tau < 0!" <<'\n';
    err = true;
  }
  else {

    deltaFn = false;

    t_bias.resize(pattern.size());
    recom_T.assign(pattern.size(), 0.0);
    t_max = *std::max_element(pattern.begin(), pattern.end());

    dendrite.assign(pattern.size(), 1);

    for (std::size_t i = 0; i < pattern.size(); i++)
    {
      t_bias[i] = t_max - pattern[i];

    }


    for (std::size_t i = 0; i < dendrite.size(); i++)

    {
      if (T[i] <= 0.0) {

        std::cout << "error: T_" << i << " <= 0!" <<'\n';
        err = true;
      }
      if (t_bias[i] < tau && t_bias[i] != 0.0) {
        // добавить вывод ошибки
        std::cout << "t_bias[" << i << "] < tau!" <<'\n';
        err = true;
      }
      if (t_bias[i] < 0.0) {
        // добавить вывод ошибки
        std::cout << "error: t_bias_" << i << " < 0!" <<'\n';
        err = true;
      }
      if (tau == 0.0){
        // считаем как дельта функцию
        deltaFn = true;
      }
    }
  }

  if (err == false) {
    return true;
  }
  else {
    return false;
  }

}

void Auto_Preset::findDendriteLength() {

  for (std::size_t i = 0; i < dendrite.size(); i++)
  {
    double N;

    if (t_bias[i] == 0.0) {
      dendrite[i] = 1.0;
      continue;
    }

    if (deltaFn == true){
      N = t_bias[i] / T[i] + 1.0;
    }
    else{
      N = (-tau / ( T[i] * std::log(1.0 - (tau / t_bias[i]))) ) + 1.0;
    }

    dendrite[i] = round(N);

  }
}

void Auto_Preset::findRecommendedT()
{
  for(std::size_t i = 0; i < dendrite.size(); i++)
  {
    if(dendrite[i] >= 2)
    {
      recom_T[i] = -tau /( (dendrite[i] - 1.0) * std::log( 1.0 - (tau / t_bias[i]) ) );
    }
    else
    {
      recom_T[i] = T[i];
    }
  }
}

//--------------------- Публичные---------------------------
void Auto_Preset::setFirstState(const std::vector<double>& pattern, double T, double tau)
{
  this->T.assign(pattern.size(), T);
  this->tau = tau;

  if (validation(pattern) == true){
    findDendriteLength();
    findRecommendedT();
  }
}


void Auto_Preset::setFirstState(const std::vector<double>& pattern, double R, double C, double tau)
{
  this->T.assign(pattern.size(), R*C);
  this->tau = tau;

  if (validation(pattern) == true){
    findDendriteLength();
    findRecommendedT();
  }
}


void Auto_Preset::setFirstState(const std::vector<double>& pattern, const std::vector<double>& VectorT, double tau)
{
  this->T = VectorT;
  this->tau = tau;

  if (validation(pattern) == true){
    findDendriteLength();
    findRecommendedT();
  }
}

gParam Auto_Preset::getResult() const
{
  gParam result;
  result.recomDendriteLength = dendrite;
  result.recom_T = recom_T;
  return result;
}

