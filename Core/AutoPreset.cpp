#include "iostream"
#include "AutoPreset.h"
#include "RecommendedPar.h"

bool Auto_Preset::validation(const std::vector<double>& pattern)
{
  bool err = false;

  if (tau < 0.0) {
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
        // добавить вывод ошибки
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
    if(dendrite[i] >= 2){
      recom_T[i] = -tau /( (dendrite[i] - 1.0) * std::log( 1.0 - (tau / t_bias[i]) ) );
    }
    else{
      recom_T[i] = T[i];
    }
  }
}

//--------------------- Публичные---------------------------


// Инициализация
void Auto_Preset::setFirstState(const PushParam& param)
{
  this->T.clear();

  //Проверяем наличие tau
  if (param.tau.has_value()) {
    this->tau = param.tau.value();
    }
  else {
    throw std::invalid_argument("Параметр tau не задан");
  }

  //  Проверка наличия и запоненности Pattern
  if (!param.pattern.has_value() || param.pattern.value().empty()) {
    throw std::invalid_argument("Патерн не передан или пуст");
  }

  // Если вектор T заполнен используем его
  if (param.vectorT.has_value() && !param.vectorT.value().empty()) {
    this->T = param.vectorT.value();
  }
  // Проверяем передан ли T, если да используем его
  else if (param.T.has_value()){
    this->T.assign(param.pattern.value().size(), param.T.value());
  }
  // Если переданы R и C
  else if (param.R.has_value() && param.C.has_value()){
    double calculated_T = param.R.value() * param.C.value();
    this->T.assign(param.pattern.value().size(), calculated_T);
  }
  // Все поля определяющие T пусты
  else{
  }

  if (!validation(param.pattern.value())) {
    //throw std::invalid_argument("Invalid pattern data");
  }

  findDendriteLength();
  findRecommendedT();

}

GetRecParam Auto_Preset::getResult() const
{
  GetRecParam result;
  result.recomDendriteLength = dendrite;
  result.recom_T = recom_T;
  return result;
}


