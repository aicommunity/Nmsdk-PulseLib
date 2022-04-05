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

#ifndef NPULSE_MEMBRANE_COMMON_CPP
#define NPULSE_MEMBRANE_COMMON_CPP

#include <algorithm>
#include "NPulseMembraneCommon.h"

#include <fstream>
#include <sstream>
#include <limits>

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembraneCommon::NPulseMembraneCommon(void)
 : UseAveragePotential("UseAveragePotential",this,&NPulseMembraneCommon::SetUseAveragePotential),
  Feedback("Feedback",this),
  SumPotential("SumPotential",this)
{
}

NPulseMembraneCommon::~NPulseMembraneCommon(void)
{
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
/// Признак наличия усреднения в выходных данных нейрона
bool NPulseMembraneCommon::SetUseAveragePotential(const bool &value)
{
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   Channels[i]->UseAveragePotential=value;
 return true;
}
// --------------------------

// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы
size_t NPulseMembraneCommon::GetNumChannels(void) const
{
 return Channels.size();
}

NPulseChannelCommon* NPulseMembraneCommon::GetChannel(size_t i)
{
 return Channels[i];
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseMembraneCommon* NPulseMembraneCommon::New(void)
{
 return new NPulseMembraneCommon;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseMembraneCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp) ||
	dynamic_pointer_cast<NPulseSynapseCommon>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPulseMembraneCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 vector<NPulseChannelCommon* >::iterator I;

 if(channel)
 {
   if(find(Channels.begin(),Channels.end(),channel) == Channels.end())
	Channels.push_back(channel);
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
   if(find(Synapses.begin(),Synapses.end(),synapse) == Synapses.end())
	Synapses.push_back(synapse);
 }

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseMembraneCommon::ADelComponent(UEPtr<UContainer> comp)
{
 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  channel->UseAveragePotential=UseAveragePotential;
  vector<NPulseChannelCommon*>::iterator I;
  I=find(Channels.begin(),Channels.end(),channel);
  if(I != Channels.end())
   Channels.erase(I);
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  vector<NPulseSynapseCommon*>::iterator I;
  I=find(Synapses.begin(),Synapses.end(),synapse);
  if(I != Synapses.end())
   Synapses.erase(I);
 }

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseMembraneCommon::ADefault(void)
{
 UseAveragePotential=true;
 SumPotential->Assign(1,1,0.0);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembraneCommon::ABuild(void)
{
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   Channels[i]->UseAveragePotential=UseAveragePotential;
 return true;
}

// Сброс процесса счета.
bool NPulseMembraneCommon::AReset(void)
{
 Feedback=0;
 IsNeuronActive=false;
 SumPotential->ToZero();
 return true;
}

// Выполняет расчет этого объекта
bool NPulseMembraneCommon::ACalculate(void)
{
 if(!ACalculate2())
  return false;

 if(Feedback>0 && !IsNeuronActive)
 {
  IsNeuronActive=true;
  for(size_t i=0;i<Channels.size();i++)
   if(Channels[i])
	Channels[i]->NeuronActivated();
 }

 if(IsNeuronActive && Feedback<=0)
  IsNeuronActive=false;
 SumPotential(0,0)=0;
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   SumPotential(0,0)+=Channels[i]->Output(0,0);

 //
 std::stringstream ss;
 ss<<"/home/ivan/rtv/log/"<<GetName().c_str()<<".txt";

 std::ofstream ofs;
 ofs.open(ss.str(), std::ios_base::app);
 typedef std::numeric_limits< double > dbl;
 std::cout.precision(dbl::max_digits10 - 1);
 ofs<<GetTime().GetDoubleTime()<<" "<<std::scientific<<SumPotential(0,0)<<" "<<Feedback.v<<" "<<(int)(IsNeuronActive)<<"\n";
 ofs.close();
 return true;
}

bool NPulseMembraneCommon::ACalculate2(void)
{
 return true;
}
// --------------------------
}
#endif
