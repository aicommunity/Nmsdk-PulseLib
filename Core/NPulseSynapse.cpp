// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_CPP
#define NPULSE_SYNAPSE_CPP

#include "NPulseSynapse.h"
#include "NPulseNeuron.h"
#include "NPulseChannel.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapse::NPulseSynapse(void)
 //: NConnector(name),
: SecretionTC("SecretionTC",this,&NPulseSynapse::SetSecretionTC),
  DissociationTC("DissociationTC",this,&NPulseSynapse::SetDissociationTC),
  InhibitionCoeff("InhibitionCoeff",this,&NPulseSynapse::SetInhibitionCoeff),
  TypicalPulseDuration("TypicalPulseDuration",this,&NPulseSynapse::SetTypicalPulseDuration),
  UsePulseSignal("UsePulseSignal",this,&NPulseSynapse::SetUsePulseSignal),
  UsePresynapticInhibition("UsePresynapticInhibition",this,&NPulseSynapse::SetUsePresynapticInhibition)
{
 VSecretionTC=1;
 VDissociationTC=1;
 OutputConstData=0;
}

NPulseSynapse::~NPulseSynapse(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NPulseSynapse::SetPulseAmplitude(const double &value)
{
 return true;
}

// Постоянная времени выделения медиатора
bool NPulseSynapse::SetSecretionTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Постоянная времени распада медиатора
bool NPulseSynapse::SetDissociationTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

/// Типовая длительность импульса, с
bool NPulseSynapse::SetTypicalPulseDuration(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Коэффициент пресинаптического торможения
bool NPulseSynapse::SetInhibitionCoeff(const double &value)
{
 if(Resistance.v > 0)
 {
  if(value>0)
   OutputConstData=4.0*(value+1)/Resistance.v;
  else
   OutputConstData=1.0/Resistance.v;
 }
 else
  OutputConstData=0;

 return true;
}

// Вес (эффективность синапса) синапса
bool NPulseSynapse::SetResistance(const double &value)
{
 if(value<=0)
  return false;

 if(InhibitionCoeff.v>0 && UsePresynapticInhibition)
  OutputConstData=4.0*InhibitionCoeff.v/value;
 else
  OutputConstData=1.0/value;

 return true;
}

bool NPulseSynapse::SetUsePulseSignal(const bool &value)
{
 Ready=false;
 return true;
}
// Задание флага включения пресинаптического торомжения
bool NPulseSynapse::SetUsePresynapticInhibition(const bool &value)
{
 if(InhibitionCoeff.v>0 && value)
  OutputConstData=4.0*InhibitionCoeff.v/Resistance;
 else
  OutputConstData=1.0/Resistance;

 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseSynapse* NPulseSynapse::New(void)
{
 return new NPulseSynapse;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseSynapse::ADefault(void)
{
 if(!NPulseSynapseCommon::ADefault())
  return false;
 // Начальные значения всем параметрам
 // Амплитуда входных импульсов
 PulseAmplitude=1;

 // Постоянная времени выделения медиатора
 SecretionTC=0.001;

 // Постоянная времени распада медиатора
 DissociationTC=0.01;

 /// Типовая длительность импульса, с
 TypicalPulseDuration=0.001;

 // Коэффициент пресинаптического торможения
 InhibitionCoeff=0;

 // Вес (эффективность синапса) синапса
 Resistance=1.0e9;

 UsePresynapticInhibition=false;
 UsePulseSignal=true;

 PulseCounter=0;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseSynapse::ABuild(void)
{
 if(!NPulseSynapseCommon::ABuild())
  return false;

 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;

 return true;
}

// Сброс процесса счета.
bool NPulseSynapse::AReset(void)
{
 if(!NPulseSynapseCommon::AReset())
  return false;

 PulseCounter=0;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseSynapse::ACalculate2(void)
{
 double input=0;
 bool is_spike(false);

 if(UsePulseSignal)
 {
  if(InputPulseSignal)
  {
   PulseCounter=int(TypicalPulseDuration*TimeStep);
  }

  if(PulseCounter>0)
  {
   input=PulseAmplitude;
   --PulseCounter;
   is_spike=true;
  }
 }
 else
 if(Input.IsConnected() && Input->GetCols()>0)
 {
  input=(*Input)(0,0);
  is_spike=true;
 }

 if(is_spike && MainOwner && Owner)
 {
  if(static_pointer_cast<NPulseChannel>(Owner)->Type() < 0)
   ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
  else
   ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
 }

 if(input>0)
  PreOutput.v+=(input/PulseAmplitude.v-PreOutput.v)/VSecretionTC;
 else
  PreOutput.v-=PreOutput.v/VDissociationTC;

 if(UsePresynapticInhibition)
  Output(0,0)=OutputConstData*(1.0-InhibitionCoeff.v*PreOutput.v)*PreOutput.v;
 else
  Output(0,0)=OutputConstData*PreOutput.v;

 if(Output(0,0)<0)
  Output(0,0)=0;

 return true;
}
// --------------------------
}
#endif
