#include "iostream"
#include "AutoPreset.h"
#include "RecommendedPar.h"

bool Auto_Preset::validation(const std::vector<double>& pattern){

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
    recomDendrite.resize(dendrite.size());
    peakErr.resize(pattern.size());

    t_max = *std::max_element(pattern.begin(), pattern.end());


    for (std::size_t i = 0; i < pattern.size(); i++){
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

    recomDendrite[i] = round(N);

  }
}

void Auto_Preset::findRecommendedT(const std::vector<double>& dendrite){
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

void Auto_Preset::findPeakErr(const std::vector<double>& pattern){
  for (std::size_t i = 0; i < pattern.size(); i++){
    double N = dendrite[i];
      peakErr[i] = pattern[i] - (tau/(1 - (1.0 - std::exp(-tau / (T[i] * (N - 1.0))))));

  }
}

//--------------------- Публичные---------------------------


// Инициализация
void Auto_Preset::setFirstState(const PushParam& param)
{
  bool flagT = false;
  bool flagD = false;

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
    flagT = true;
  }
  // Проверяем передан ли T, если да используем его
  else if (param.T.has_value()){
    this->T.assign(param.pattern.value().size(), param.T.value());
    flagT = true;
  }
  // Если переданы R и C
  else if (param.R.has_value() && param.C.has_value()){
    double calculated_T = param.R.value() * param.C.value();
    this->T.assign(param.pattern.value().size(), calculated_T);
    flagT = true;
  }
  // Все поля определяющие T пусты, заполняем единицами
  else{
    this->T.assign(param.pattern.value().size(), 1.0);
  }

  // Проверяем заполнены ли поля с длинной дендрита
  if (param.dendriteLength.has_value() && !param.dendriteLength.value().empty()){
    this->dendrite = param.dendriteLength.value();
    flagD = true;
  }
  else{
    dendrite.assign(param.pattern.value().size(), 1);
  }


  if (!validation(param.pattern.value())) {
    throw std::invalid_argument("Invalid pattern data");
  }
  else if(flagD == false && flagT == false){
    throw std::invalid_argument("Invalid T and dendrite data");
  }
  else if(flagD == true and flagT == true){
    findPeakErr(param.pattern.value());
  }
  else if(flagT == true){
    findDendriteLength();
    findRecommendedT(recomDendrite);
  }
  else{
    findRecommendedT(dendrite); // с появлением расчёта синопсов DendriteLength может вернуться
  }


}

GetRecParam Auto_Preset::getResult() const
{
  GetRecParam result;
  result.recomDendriteLength = recomDendrite;
  result.recom_T = recom_T;
  result.peakErr = peakErr;
  return result;
}


