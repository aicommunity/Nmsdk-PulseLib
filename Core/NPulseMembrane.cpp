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
#include "NPulseChannel.h"



namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembrane::NPulseMembrane(void)
 : Feedback("Feedback",this),
  FeedbackGain("FeedbackGain",this,&NPulseMembrane::SetFeedbackGain),
  ResetAvailable("ResetAvailable",this)
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
 return PosChannels.size();
}

NPulseChannel* NPulseMembrane::GetPosChannel(size_t i)
{
 return PosChannels[i];
}

// Ионные механизмы гиперполяризации
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return NegChannels.size();
}

NPulseChannel* NPulseMembrane::GetNegChannel(size_t i)
{
 return NegChannels[i];
}

bool NPulseMembrane::UpdateChannelData(UEPtr<NPulseChannel> channel, UEPtr<UIPointer> pointer)
{
  vector<NPulseChannel* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(PosChannels.begin(),PosChannels.end(),channel) == PosChannels.end())
	PosChannels.push_back(channel);
   if((I=find(NegChannels.begin(),NegChannels.end(),channel)) != NegChannels.end())
	NegChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(NegChannels.begin(),NegChannels.end(),channel) == NegChannels.end())
	NegChannels.push_back(channel);
   if((I=find(PosChannels.begin(),PosChannels.end(),channel)) != PosChannels.end())
	PosChannels.erase(I);
  }

 else
  return false;

 return true;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Коэффициент обратной связи
bool NPulseMembrane::SetFeedbackGain(const double &value)
{
 if(value < 0)
  return false;

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
 if(dynamic_pointer_cast<NPulseChannel>(comp))
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
   if(find(PosChannels.begin(),PosChannels.end(),channel) == PosChannels.end())
	PosChannels.push_back(channel);
   if((I=find(NegChannels.begin(),NegChannels.end(),channel)) != NegChannels.end())
	NegChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(NegChannels.begin(),NegChannels.end(),channel) == NegChannels.end())
	NegChannels.push_back(channel);
   if((I=find(PosChannels.begin(),PosChannels.end(),channel)) != PosChannels.end())
	PosChannels.erase(I);
  }
  else
   return false;
 }
 else
  return false;

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
  I=find(PosChannels.begin(),PosChannels.end(),channel);
  if(I != PosChannels.end())
   PosChannels.erase(I);
  I=find(NegChannels.begin(),NegChannels.end(),channel);
  if(I != NegChannels.end())
   NegChannels.erase(I);
 }
 return true;
}
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

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembrane::ABuild(void)
{
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
 if(!PosChannels.empty() && !NegChannels.empty() && ResetAvailable)
 {
	if(PosChannels[0]->GetSynOutput() > 1.0e-10)
	{
		NegChannels[0]->ResetOut();
	}
	else
	if(NegChannels[0]->GetSynOutput() > 1.0e-10)
	{
		PosChannels[0]->ResetOut();
	}
 }
 // Получение данных канала
// Feedback=GetFullSumInput();
 Feedback=0;

 for(int i=0;i<NumInputs;i++)
  for(int j=0;j<GetInputDataSize(i)[1];j++)
   Feedback.v+=GetInputData(i)->Double[j];

 Feedback.v*=FeedbackGain.v;

 return true;
}
// --------------------------
}
#endif
