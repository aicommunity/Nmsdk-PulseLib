// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_COMMON_CPP
#define NPULSE_SYNAPSE_COMMON_CPP

#include "NPulseSynapseCommon.h"
#include "NPulseNeuron.h"
#include "NPulseChannel.h"

#include <fstream>
#include <sstream>
#include <limits>

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynapseCommon::NPulseSynapseCommon(void)
: PulseAmplitude("PulseAmplitude",this,&NPulseSynapseCommon::SetPulseAmplitude),
  Resistance("Resistance",this,&NPulseSynapseCommon::SetResistance),
  Weight("Weight",this),
  TrainerClassName("TrainerClassName",this, &NPulseSynapseCommon::SetTrainerClassName),
  Input("Input",this),
  WeightInput("WeightInput",this),
  Output("Output",this),
  OutInCopy("OutInCopy",this),
  PreOutput("PreOutput",this),
  InputPulseSignal("InputPulseSignal",this)
{
 Trainer=0;
}

NPulseSynapseCommon::~NPulseSynapseCommon(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NPulseSynapseCommon::SetPulseAmplitude(const double &value)
{
 return true;
}

// Вес (эффективность синапса) синапса
bool NPulseSynapseCommon::SetResistance(const double &value)
{
 return true;
}

/// Имя класса-учителя, настраивающего вес синапса
bool NPulseSynapseCommon::SetTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseSynapseCommon* NPulseSynapseCommon::New(void)
{
 return new NPulseSynapseCommon;
}

// Возвращает указатель на учителя
NSynapseTrainer* NPulseSynapseCommon::GetTrainer(void)
{
 return Trainer;
}

// Устанавливает связи тренера если он есть
void NPulseSynapseCommon::RebuildInternalLinks(void)
{
 if(Trainer)
  Trainer->RebuildInternalLinks();
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseSynapseCommon::ADefault(void)
{
 // Начальные значения всем параметрам
 // Амплитуда входных импульсов
 PulseAmplitude=1;

 // Вес (эффективность синапса) синапса
 Resistance=10.0;

 Output.Assign(1,1,0.0);
 Input->Assign(1,1,0.0);
 OutInCopy.Assign(1,1,0.0);
 WeightInput->Assign(1,1,0.0);
 InputPulseSignal=false;

 Weight=1;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseSynapseCommon::ABuild(void)
{
 return true;
}

bool NPulseSynapseCommon::AAfterBuild(void)
{
    if(!TrainerClassName->empty())
    {
     Trainer=AddMissingComponent<NSynapseTrainer>("Trainer", TrainerClassName);
     if(Trainer)
      Trainer->RebuildInternalLinks();
    }
    else
    {
     if(Trainer)
     {
      Storage->ReturnObject(Trainer);
      Trainer=0;
     }
    }
 return true;
}

// Сброс процесса счета.
bool NPulseSynapseCommon::AReset(void)
{
 // Сброс временных переменных
 PreOutput=0;
 InputPulseSignal=false;
 PulseSignalTemp=false;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseSynapseCommon::ACalculate(void)
{
 if((*Input)(0,0)>0 && !PulseSignalTemp)
 {
  InputPulseSignal=true;
  PulseSignalTemp=true;
 }

 if((*Input)(0,0)<=0 && PulseSignalTemp)
  PulseSignalTemp=false;

 *OutInCopy=*Input;
 if(WeightInput.IsConnected())
  Weight=(*WeightInput)(0,0);

 bool res=ACalculate2();
 InputPulseSignal=false;
 Output(0,0)=Output(0,0)*Weight;

 //DEBUG
 // Получение информации об обратной связи
 UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(Owner);
 if(membrane)
 {
     std::stringstream ss;
     ss<<"/home/ivan/rtv/log/"<<membrane->GetName().c_str()<<"_"<<GetName().c_str()<<".txt";
     std::ofstream ofs;
     ofs.open(ss.str(), std::ios_base::app);
     ofs<<GetTime().GetDoubleTime()<<" "<<std::scientific<<(*OutInCopy)(0,0)<<" "<<std::scientific<<Output(0,0)<<"\n";
     ofs.close();
 }
 //DEBUG

 return res;
}

bool NPulseSynapseCommon::ACalculate2(void)
{
 Output(0,0)=PreOutput;
 return true;
}
// --------------------------
}
#endif
