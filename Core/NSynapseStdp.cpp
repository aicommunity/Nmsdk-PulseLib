#ifndef NMSDK_NSYNAPSESTDP_CPP
#define NMSDK_NSYNAPSESTDP_CPP

#include "NSynapseStdp.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseStdp::NSynapseStdp(void)
: XModCoeff("XModCoeff",this, &NSynapseStdp::SetXModCoeff),
  YModCoeff("YModCoeff",this, &NSynapseStdp::SetYModCoeff),
  APlus("APlus",this,&NSynapseStdp::SetAPlus),
  AMinus("AMinus",this,&NSynapseStdp::SetAMinus),
  XTau("XTau",this,&NSynapseStdp::SetXTau),
  YTau("YTau",this,&NSynapseStdp::SetYTau),
  XAvg("XAvg",this),
  YAvg("YAvg",this),
  XYDiff("XYDiff",this),
  PsActivityInput("PsActivityInput",this),
  StdpInfluence("StdpInfluence",this)
{
}

NSynapseStdp::~NSynapseStdp(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NSynapseStdp* NSynapseStdp::New(void)
{
 return new NSynapseStdp;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool NSynapseStdp::SetXModCoeff(const double &value)
{
 return true;
}

bool NSynapseStdp::SetYModCoeff(const double &value)
{
 return true;
}

bool NSynapseStdp::SetAPlus(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NSynapseStdp::SetAMinus(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NSynapseStdp::SetXTau(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NSynapseStdp::SetYTau(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NSynapseStdp::ADefault(void)
{
 if(!NPulseSynapseCommon::ADefault())
  return false;

 XModCoeff=1;
 YModCoeff=1;
 APlus=1;
 AMinus=1;
 XTau=1e-2;
 YTau=1e-3;
 StdpInfluence.Assign(1,1,0.0);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NSynapseStdp::ABuild(void)
{
 if(!NPulseSynapseCommon::ABuild())
  return false;

 return true;
}

// Сброс процесса счета.
bool NSynapseStdp::AReset(void)
{
 if(!NPulseSynapseCommon::AReset())
  return false;

 XAvg=0;
 YAvg=0;
 XYDiff=0;
 StdpInfluence.ToZero();
 return true;
}

// Выполняет расчет этого объекта
bool NSynapseStdp::ACalculate2(void)
{
 bool is_input_pulse_active(false);
 bool is_output_pulse_active(false);

 if(!Input.IsConnected() || Input->GetCols()<=0)
  return true;

 if(!PsActivityInput.IsConnected() || PsActivityInput->GetCols()<=0)
  return true;

 if((*Input)(0,0)>0)
  is_input_pulse_active=true;

 if((*PsActivityInput)(0,0)>0)
  is_output_pulse_active=true;

 if(is_output_pulse_active)
  XAvg.v+=(XModCoeff.v-XAvg.v)/(XTau.v*TimeStep);
 else
  XAvg.v-=XAvg.v/(XTau.v*TimeStep);

 if(is_input_pulse_active)
  YAvg.v+=(YModCoeff.v-YAvg.v)/(YTau.v*TimeStep);
 else
  YAvg.v-=YAvg.v/(YTau.v*TimeStep);

 double x_avg_res=(is_output_pulse_active)?XAvg.v*APlus.v:0;
 double y_avg_res=(is_input_pulse_active)?YAvg.v*AMinus.v:0;;

 XYDiff.v+=(x_avg_res-y_avg_res)/TimeStep;

 StdpInfluence(0,0)=(1-XYDiff.v);
 Output(0,0)=StdpInfluence(0,0)*Resistance.v;

 return true;
}
// --------------------------

}
#endif



