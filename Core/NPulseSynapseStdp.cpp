// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2020.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_STDP_CPP
#define NPULSE_SYNAPSE_STDP_CPP

#include "NPulseSynapseStdp.h"
#include "NPulseNeuronCommon.h"
#include "NPulseLTZoneCommon.h"
#include "NPulseChannel.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapseStdp::NPulseSynapseStdp(void)
 //: NConnector(name),
: XModCoeff("XModCoeff",this, &NPulseSynapseStdp::SetXModCoeff),
  YModCoeff("YModCoeff",this, &NPulseSynapseStdp::SetYModCoeff),
  APlus("APlus",this,&NPulseSynapseStdp::SetAPlus),
  AMinus("AMinus",this,&NPulseSynapseStdp::SetAMinus),
  XTau("XTau",this,&NPulseSynapseStdp::SetXTau),
  YTau("YTau",this,&NPulseSynapseStdp::SetYTau),
  XAvg("XAvg",this),
  YAvg("YAvg",this),
  XYDiff("XYDiff",this),
  PsActivityInput("PsActivityInput",this),
  StdpInfluence("StdpInfluence",this)
 // Input("Input",this)
{
}

NPulseSynapseStdp::~NPulseSynapseStdp(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool NPulseSynapseStdp::SetXModCoeff(const double &value)
{
 return true;
}

bool NPulseSynapseStdp::SetYModCoeff(const double &value)
{
 return true;
}

bool NPulseSynapseStdp::SetAPlus(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NPulseSynapseStdp::SetAMinus(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NPulseSynapseStdp::SetXTau(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

bool NPulseSynapseStdp::SetYTau(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseSynapseStdp* NPulseSynapseStdp::New(void)
{
 return new NPulseSynapseStdp;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseSynapseStdp::ADefault(void)
{
 if(!NPulseSynapse::ADefault())
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
bool NPulseSynapseStdp::ABuild(void)
{
 if(!NPulseSynapse::ABuild())
  return false;

 return true;
}

// Сброс процесса счета.
bool NPulseSynapseStdp::AReset(void)
{
 if(!NPulseSynapse::AReset())
  return false;

 XAvg=0;
 YAvg=0;
 XYDiff=0;
 StdpInfluence.ToZero();
 return true;
}

// Выполняет расчет этого объекта
bool NPulseSynapseStdp::ACalculate2(void)
{
 if(!NPulseSynapse::ACalculate2())
  return false;

 bool is_input_pulse_active(false);
 bool is_output_pulse_active(false);

 if(!Input.IsConnected() || Input->GetCols()<=0)
  return true;

 if((*Input)(0,0)>0)
  is_input_pulse_active=true;

 if(PsActivityInput.IsConnected() && PsActivityInput->GetCols()>0)
 {
  if((*PsActivityInput)(0,0)>0)
   is_output_pulse_active=true;
 }
 else
 {
  if(!MainOwner)
   return true;

  UEPtr<NPulseNeuronCommon> neuron=dynamic_pointer_cast<NPulseNeuronCommon>(MainOwner);
  NLTZone* zone=neuron->LTZone;
  if(!zone)
   return true;

  if(zone->Output->GetCols()<=0)
   return true;

  if(zone->Output(0,0)>0)
   is_output_pulse_active=true;
 }

 if(is_output_pulse_active)
  XAvg.v+=(XModCoeff-XAvg.v)/(XTau.v*TimeStep);
 else
  XAvg.v-=XAvg.v/(XTau.v*TimeStep);

 if(is_input_pulse_active)
  YAvg.v+=(YModCoeff-YAvg.v)/(YTau.v*TimeStep);
 else
  YAvg.v-=YAvg.v/(YTau.v*TimeStep);

 double x_avg_res=(is_output_pulse_active)?XAvg.v*APlus.v:0;
 double y_avg_res=(is_input_pulse_active)?YAvg.v*AMinus.v:0;;

 XYDiff.v+=(x_avg_res-y_avg_res)/TimeStep;

 StdpInfluence(0,0)=(1-XYDiff.v);

 if(StdpInfluence(0,0)<0)
  Output(0,0)=0;
 else
  Output(0,0)*=StdpInfluence(0,0);

 return true;
}
// --------------------------
}
#endif
