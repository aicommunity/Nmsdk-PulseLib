// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_HEBB_LIFE_SYNAPSE_CPP
#define NPULSE_HEBB_LIFE_SYNAPSE_CPP

#include "NPulseHebbLifeSynapse.h"
#include "NPulseLifeNeuron.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseHebbLifeSynapse::NPulseHebbLifeSynapse(void)
{
}

NPulseHebbLifeSynapse::~NPulseHebbLifeSynapse(void)
{
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseHebbLifeSynapse* NPulseHebbLifeSynapse::New(void)
{
 return new NPulseHebbLifeSynapse;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Подключает синапс хебба к модели жизнеобеспечения нейрона
bool NPulseHebbLifeSynapse::InstallLifeConnection(void)
{
 bool res=true;
 UEPtr<NPulseLifeNeuron> mlowner=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);

 if(mlowner && mlowner->GetNeuronLife())
 {
  RDK::UStringLinkSide item,conn;
  UEPtr<UConnector> connected_component=GetAConnectorByIndex("Output", 0);
  UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(connected_component);
  if(!channel)
   return false;

  Type=channel->Type;
  if(channel->Type.GetData()>0)
   res&=mlowner->CreateLink(mlowner->GetNeuronLife()->GetLongName(mlowner),"Output7",GetLongName(mlowner), "InputNeuronLifeSignal");
  else
   res&=mlowner->CreateLink(mlowner->GetNeuronLife()->GetLongName(mlowner),"Output6",GetLongName(mlowner), "InputNeuronLifeSignal");
}
 return res;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseHebbLifeSynapse::ADefault(void)
{
 if(!NPulseHebbSynapse::ADefault())
  return false;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseHebbLifeSynapse::ABuild(void)
{
 Type=0;
 return NPulseHebbSynapse::ABuild();
}

// Сброс процесса счета.
bool NPulseHebbLifeSynapse::AReset(void)
{
 if(!NPulseHebbSynapse::AReset())
  return false;
 InstallLifeConnection();

 return true;
}

// Выполняет расчет этого объекта
bool NPulseHebbLifeSynapse::ACalculate2(void)
{
 if(!NPulseHebbSynapse::ACalculate2())
  return false;

 if(MainOwner && Owner)
 {
  UEPtr<NPulseLifeNeuron> neuron=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);
  if(neuron)
  {
   // Кэшируем значения для оптимизации
   const double gsSum = GsSum.GetData();
   const double gsGain = GsGain.GetData();
   const double gd = Gd.GetData();
   const double gdGain = GdGain.GetData();
   const double g = G.GetData();
   
   if(Type>0)
   {
	neuron->SummaryNegGs = neuron->SummaryNegGs.GetData() + gsSum*gsGain;
	neuron->SummaryNegGd = neuron->SummaryNegGd.GetData() + gd*gdGain;
	neuron->SummaryNegG = neuron->SummaryNegG.GetData() + g;
   }
   else
   {
	neuron->SummaryPosGs = neuron->SummaryPosGs.GetData() + gsSum*gsGain;
	neuron->SummaryPosGd = neuron->SummaryPosGd.GetData() + gd*gdGain;
	neuron->SummaryPosG = neuron->SummaryPosG.GetData() + g;
   }
  }
 }

 return true;
}
// --------------------------
}

#endif
