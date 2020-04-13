// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_CHANNEL_CPP
#define NPULSE_CHANNEL_CPP

#include "NPulseChannel.h"
#include "NPulseSynapse.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "NPulseHebbSynapse.h"
#include "../../Nmsdk-NeuronLifeLib/Core/NPulseLifeNeuron.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannel::NPulseChannel(void)
: Capacity("Capacity",this,&NPulseChannel::SetCapacity),
Resistance("Resistance",this,&NPulseChannel::SetResistance),
FBResistance("FBResistance",this,&NPulseChannel::SetFBResistance),
RestingResistance("RestingResistance",this),
NumConnectedSynapsis("NumConnectedSynapsis",this),
Inputs("Inputs",this)
{
 channel_input=0;
}

NPulseChannel::~NPulseChannel(void)
{
}
// --------------------------


// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
int NPulseChannel::GetNumSynapses(void) const
{
 return GetNumComponents();
}

// Возвращает синапс по индексу
UEPtr<NPulseSynapse> NPulseChannel::GetSynapse(int i)
{
 return static_pointer_cast<NPulseSynapse>(GetComponentByIndex(i));
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Емкость мембраны
bool NPulseChannel::SetCapacity(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Сопротивление мембраны
bool NPulseChannel::SetResistance(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// Сопротивление перезаряда мембраны
bool NPulseChannel::SetFBResistance(const double &value)
{
 if(value <= 0)
  return false;

 return true;

}
// --------------------------

// --------------------------
// Методы управления объектом
// --------------------------
// Подключает синапс хебба synapse к низкопороговой зоне нейрона-владельца
// Возвращает false только если произошла ошибка установки связи
// Если synapse == 0, то подключает все синапсы хебба
bool NPulseChannel::InstallHebbSynapses(UEPtr<UContainer> synapse)
{
 bool res=true;
 UEPtr<NPulseNeuron> mowner=dynamic_pointer_cast<NPulseNeuron>(MainOwner);

 if(mowner && mowner->GetLTZone())
 {
  if(synapse)
  {
   UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(synapse);
   if(hsynapse)
   {
	RDK::UStringLinkSide item,conn;
	item.Id=mowner->GetLTZone()->GetLongName(mowner);
	item.Name="Output";
	conn.Id=hsynapse->GetLongName(mowner);
	conn.Name="InputLTZoneFeedbackSignal";
	res&=mowner->CreateLink(item,conn);
   }
  }
  else
  {
   RDK::UStringLinkSide item,conn;
   for(int i=0;i<GetNumSynapses();i++)
   {
	UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(GetSynapse(i));
	if(hsynapse)
	{
	 item.Id=mowner->GetLTZone()->GetLongName(mowner);
     item.Name="Output";
	 conn.Id=hsynapse->GetLongName(mowner);
	 conn.Name="InputLTZoneFeedbackSignal";
	 res&=mowner->CreateLink(item,conn);
	}
   }
  }
 }

 UEPtr<NPulseLifeNeuron> mlowner=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);

 if(mlowner && mlowner->GetNeuronLife())
 {
  if(synapse)
  {
   UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(synapse);
   if(hsynapse)
   {
	RDK::UStringLinkSide item,conn;
	item.Id=mlowner->GetNeuronLife()->GetLongName(mlowner);
#pragma warning
	if(Type.v>0)
	 item.Index=6;
	else
	 item.Index=5;
	conn.Id=hsynapse->GetLongName(mlowner);
	conn.Name="InputNeuronLifeSignal";
	res&=mlowner->CreateLink(item,conn);
   }
  }
  else
  {
   RDK::UStringLinkSide item,conn;
   for(int i=0;i<GetNumSynapses();i++)
   {
	UEPtr<NPulseHebbSynapse> hsynapse=dynamic_pointer_cast<NPulseHebbSynapse>(GetSynapse(i));
	if(hsynapse)
	{
	 item.Id=mlowner->GetNeuronLife()->GetLongName(mlowner);
#pragma warning
	 if(Type.v>0)
	  item.Index=6;
	 else
	  item.Index=5;
	 conn.Id=hsynapse->GetLongName(mlowner);
	 conn.Name="InputNeuronLifeSignal";
	 res&=mlowner->CreateLink(item,conn);
	}
   }
  }
 }
 return res;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseChannel* NPulseChannel::New(void)
{
 return new NPulseChannel;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseChannel::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseSynapse>(comp))
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
bool NPulseChannel::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NPulseChannelCommon::AAddComponent(comp, pointer))
  return false;
 InstallHebbSynapses(comp);
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseChannel::ADelComponent(UEPtr<UContainer> comp)
{
 if(!NPulseChannelCommon::ADelComponent(comp))
  return false;

 return true;
}
// --------------------------
double NPulseChannel::GetSynOutput(void)
{
	 return 0;
}

bool NPulseChannel::ResetOut(void)
{
	 return false;
}
// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseChannel::ADefault(void)
{
 if(!NPulseChannelCommon::AReset())
  return false;

 // Емкость мембраны
 Capacity=1.0e-9;

 // Сопротивление мембраны
 Resistance=1.0e7;

 // Сопротивление перезаряда мембраны
 FBResistance=1.0e8;

 RestingResistance=1.0e7;

 Type=0;

 NumConnectedSynapsis = 0;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseChannel::ABuild(void)
{
 if(!NPulseChannelCommon::ABuild())
  return false;

 return true;
}

// Сброс процесса счета.
bool NPulseChannel::AReset(void)
{
 if(!NPulseChannelCommon::AReset())
  return false;

 if(Type>0)
  Output(0,0)=1;
 else
 if(Type<0)
  Output(0,0)=-1;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseChannel::ACalculate(void)
{
 if(!NPulseChannelCommon::ACalculate())
  return false;

 channel_input=0;
 double G=0;

 // Получение доступа к данным синапса
 for(int i=0;i<NumComponents;i++)
  G+=static_pointer_cast<NPulseSynapseCommon>(PComponents[i])->Output(0,0);

 // Получение данных канала
 size_t inp_size;
 size_t full_inp_data_size(0);
 for(size_t i=0;i<Inputs->size();i++)
 {
  if((inp_size=Inputs[i]->GetCols()) >0)
  {
   full_inp_data_size+=inp_size;
   double *data=Inputs[i]->Data;
   for(size_t j=0;j<inp_size;j++,++data)
	channel_input+=*data;
  }

  if(UseAveragePotential)
   channel_input/=full_inp_data_size;//FullInputDataSize;
 }

 // Получение информации об обратной связи
 double feedback=static_pointer_cast<NPulseMembrane>(Owner)->Feedback;
 if(Owner)
  channel_input-=feedback;

 // Расчет
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);

 // Проверяем необходимость сброса
// if(RestingFlag)
// {
//  if(G<RestingThreshold)
//  {
//   *out=channel_input;
//  }
// }

 if(!feedback)
 {
  double resistance(0.0);
  if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
   resistance=RestingResistance.v;
  else
   resistance=Resistance.v;

  Ti=Capacity.v/(G+1.0/resistance);
  sum_u=(1.0+G*resistance);
 }
 else
 {
  Ti=Capacity.v/(G+1.0/FBResistance.v);
  sum_u=(1.0+G*FBResistance.v);
 }

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

