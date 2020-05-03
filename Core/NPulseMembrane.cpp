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

#ifndef NPULSE_MEMBRANE_CPP
#define NPULSE_MEMBRANE_CPP

#include <algorithm>
#include "NPulseMembrane.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembrane::NPulseMembrane(void)
 : FeedbackGain("FeedbackGain",this,&NPulseMembrane::SetFeedbackGain),
  ResetAvailable("ResetAvailable",this,&NPulseMembrane::SetResetAvailable),
  SynapseClassName("SynapseClassName",this,&NPulseMembrane::SetSynapseClassName),
  ExcChannelClassName("ExcChannelClassName",this,&NPulseMembrane::SetExcChannelClassName),
  InhChannelClassName("InhChannelClassName",this,&NPulseMembrane::SetInhChannelClassName),
  NumExcitatorySynapses("NumExcitatorySynapses",this,&NPulseMembrane::SetNumExcitatorySynapses),
  NumInhibitorySynapses("NumInhibitorySynapses",this,&NPulseMembrane::SetNumInhibitorySynapses),
  InputFeedbackSignal("InputFeedbackSignal",this),
  Feedback("Feedback",this)

{
}

NPulseMembrane::~NPulseMembrane(void)
{
}
// --------------------------

// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы деполяризации
size_t NPulseMembrane::GetNumPosChannels(void) const
{
 return ExcitatoryChannels.size();
}

NPulseChannel* NPulseMembrane::GetPosChannel(size_t i)
{
 return ExcitatoryChannels[i];
}

// Ионные механизмы гиперполяризации
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return InhibitoryChannels.size();
}

NPulseChannel* NPulseMembrane::GetNegChannel(size_t i)
{
 return InhibitoryChannels[i];
}

bool NPulseMembrane::UpdateChannelData(UEPtr<NPulseChannel> channel, UEPtr<UIPointer> pointer)
{
  vector<NPulseChannel* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel);
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel)) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel);
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel)) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }

 else
  return false;

 return true;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Наличие механизма сброса
bool NPulseMembrane::SetResetAvailable(const bool &value)
{
 return true;
}

// Коэффициент обратной связи
bool NPulseMembrane::SetFeedbackGain(const double &value)
{
 if(value < 0)
  return false;

 return true;
}

bool NPulseMembrane::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetExcChannelClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetInhChannelClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumExcitatorySynapses(const int &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumInhibitorySynapses(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseMembrane* NPulseMembrane::New(void)
{
 return new NPulseMembrane;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseMembrane::CheckComponentType(UEPtr<UContainer> comp) const
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
/*
bool NPulseMembrane::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NPulseMembraneCommon::AAddComponent(comp, pointer))
  return false;

 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 vector<NPulseChannel* >::iterator I;

 if(channel)
 {
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel);
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel)) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel);
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel)) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }
  else
   LogMessageEx(RDK_EX_WARNING,__FUNCTION__,"Channel->Type is undefined");
 }



 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseMembrane::ADelComponent(UEPtr<UContainer> comp)
{
 if(!NPulseMembraneCommon::ADelComponent(comp))
  return false;

 UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(comp);
 if(channel)
 {
  vector<NPulseChannel*>::iterator I;
  I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel);
  if(I != ExcitatoryChannels.end())
   ExcitatoryChannels.erase(I);
  I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel);
  if(I != InhibitoryChannels.end())
   InhibitoryChannels.erase(I);
 }
 return true;
}               */
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseMembrane::ADefault(void)
{
 if(!NPulseMembraneCommon::ADefault())
  return false;
 FeedbackGain=2;
 ResetAvailable=true;
 SynapseClassName="NPSynapse";
 ExcChannelClassName="NPExcChannel";
 InhChannelClassName="NPInhChannel";
 NumExcitatorySynapses=1;
 NumInhibitorySynapses=1;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembrane::ABuild(void)
{
 if(!Storage)
  return true;
 UEPtr<NPulseChannel> exc_channel=AddMissingComponent<NPulseChannel>("ExcChannel", ExcChannelClassName);
 UEPtr<NPulseChannel> inh_channel=AddMissingComponent<NPulseChannel>("InhChannel", InhChannelClassName);

 ExcitatoryChannels.resize(1);
 ExcitatoryChannels[0]=exc_channel;
 exc_channel->SetCoord(MVector<double,3>(5,4,0));
 InhibitoryChannels.resize(1);
 InhibitoryChannels[0]=inh_channel;
 inh_channel->SetCoord(MVector<double,3>(5,8,0));

 bool res=true;

 int old_ex_synapses=int(ExcitatorySynapses.size());

 for(int i=NumExcitatorySynapses;i<old_ex_synapses;i++)
  ExcitatorySynapses[i]->Free();
 ExcitatorySynapses.clear();

 for(int i=0;i<NumExcitatorySynapses;i++)
 {
  UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("ExcSynapse")+sntoa(i+1), SynapseClassName);

  ExcitatorySynapses.push_back(synapse);
  res&=CreateLink(synapse->GetName(),"Output","ExcChannel","SynapticInputs");
  synapse->SetCoord(MVector<double,3>(5+i*6,1.7,0));
 }

 int old_in_synapses=int(InhibitorySynapses.size());
 for(int i=NumInhibitorySynapses;i<old_in_synapses;i++)
  InhibitorySynapses[i]->Free();
 InhibitorySynapses.clear();

 for(int i=0;i<NumInhibitorySynapses;i++)
 {
  UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("InhSynapse")+sntoa(i+1), SynapseClassName);

  InhibitorySynapses.push_back(synapse);
  res&=CreateLink(synapse->GetName(),"Output","InhChannel","SynapticInputs");
  synapse->SetCoord(MVector<double,3>(5+i*6,10.6,0));
 }

 if(!NPulseMembraneCommon::ABuild())
  return false;
 return true;
}

// Сброс процесса счета.
bool NPulseMembrane::AReset(void)
{
 if(!NPulseMembraneCommon::AReset())
  return false;
 Feedback=0;
 return true;
}

// Выполняет расчет этого объекта
bool NPulseMembrane::ACalculate(void)
{
 if(!NPulseMembraneCommon::ACalculate())
  return false;
 if(!ExcitatoryChannels.empty() && !InhibitoryChannels.empty() && ResetAvailable)
 {
	if(ExcitatoryChannels[0]->GetSynOutput() > 1.0e-10)
	{
		InhibitoryChannels[0]->ResetOut();
	}
	else
	if(InhibitoryChannels[0]->GetSynOutput() > 1.0e-10)
	{
		ExcitatoryChannels[0]->ResetOut();
	}
 }

 Feedback=0;

  for(int j=0;j<InputFeedbackSignal->GetRows();j++)
   for(int i=0;i<InputFeedbackSignal->GetCols();i++)
    Feedback.v+=(*InputFeedbackSignal)(j,i);

 Feedback.v*=FeedbackGain.v;

 return true;
}
// --------------------------
}
#endif
