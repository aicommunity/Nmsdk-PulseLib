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

#ifndef NPULSE_NEURON_CPP
#define NPULSE_NEURON_CPP

#include "NPulseNeuron.h"
#include "NPulseMembrane.h"
#include "NPulseLTZone.h"
#include "NPulseSynapse.h"
#include "NPulseChannel.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseNeuron::NPulseNeuron(void)
{
 PosGenerator=0;
 NegGenerator=0;
 MainOwner=this;
}

NPulseNeuron::~NPulseNeuron(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// Возвращает указатель на модель источника возбуждаюшего потенциала
NConstGenerator* NPulseNeuron::GetPosGenerator(void)
{
 return PosGenerator;
}

// Возвращает указатель на модель источника тормозного потенциала
NConstGenerator* NPulseNeuron::GetNegGenerator(void)
{
 return NegGenerator;
}

/// Доступ к участкам мембраны
size_t NPulseNeuron::GetNumMembranes(void) const
{
 return Membranes.size();
}

NPulseMembrane* NPulseNeuron::GetMembrane(size_t i)
{
 return dynamic_cast<NPulseMembrane*>(Membranes[i]);
}
// --------------------------

// --------------------------
// Методы управления структурой объекта
// --------------------------
// Удлинняет заданный участок мембраны, добавляя к нему новый участок мембраны,
// и переключая входы заданного участка на входы нового
// Возвращает указатель на созданный участок
NPulseMembrane* NPulseNeuron::ElongateDendrite(const UId &id, bool feedback)
{
 if(!Storage)
  return 0;

 UEPtr<UContainer> cont=GetComponent(id);
 if(!cont)
  return 0;


 UEPtr<UContainer> newcont=static_pointer_cast<UContainer>(Storage->TakeObject(cont->GetClass()));
 if(!newcont)
  return 0;


 return 0;
}

// Разветвляет заданный участок мембраны, добавляя к точке его подключения
// дополнительно новый участок мембраны
// Возвращает указатель на созданный участок
NPulseMembrane* NPulseNeuron::BranchDendrite(const UId &id, bool feedback)
{
 if(!Storage)
  return 0;

 UEPtr<NPulseMembrane> dendrite=dynamic_pointer_cast<NPulseMembrane>(GetComponent(id));
 if(!dendrite)
  return 0;


 UEPtr<UContainer> cont=static_pointer_cast<UContainer>(Storage->TakeObject(dendrite->GetClass()));
 if(!AddComponent(cont))
 {
//  Storage->ReturnObject(cont);
  cont->Free();
  return 0;
 }

 RDK::ULinkSide item,conn;
 bool res=true;

 // Устанавливаем обратную связь
 if(feedback)
 {
  item.Id=GetLTZone()->GetLongId(this);
  item.Name="DataOutput0";
  conn.Id=cont->GetLongId(this);
  conn.Name="";
  res&=CreateLink(item,conn);
 }

 // Подключаемся каналами к приемниками
 int size=dendrite->GetNumComponents();
 if(size>cont->GetNumComponents())
  size=cont->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  UEPtr<UADItem> channel=static_pointer_cast<UADItem>(dendrite->GetComponentByIndex(k));
  for(int i=0;i<channel->GetNumOutputs();i++)
  {
   std::string output_name("DataOutput");
   output_name+=RDK::sntoa(i);
//   for(int j=0;j<channel->GetNumAConnectors(i);j++)
   for(int j=0;j<channel->GetNumAConnectors(output_name);j++)
   {
	item.Id=cont->GetComponent(channel->GetName())->GetLongId(this);
	item.Name="DataOutput0";
	conn.Id=channel->GetAConnectorByIndex(output_name,j)->GetLongId(this);
//	conn.Id=channel->GetAConnectorByIndex(int(i),j)->GetLongId(this);
	conn.Name="";
	res&=CreateLink(item,conn);
   }
  }
 }

 // Подключаем источники мембранных потенциалов
 UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(cont);
 if(membrane)
 {
  for(size_t k=0;k<membrane->GetNumNegChannels();k++)
  {
   item.Id=PosGenerator->GetLongId(this);
   item.Name="DataOutput0";
   conn.Id=membrane->GetNegChannel(k)->GetLongId(this);
   conn.Name="";
   res&=CreateLink(item,conn);
  }

  for(size_t k=0;k<membrane->GetNumPosChannels();k++)
  {
   item.Id=NegGenerator->GetLongId(this);
   item.Name="DataOutput0";
   conn.Id=membrane->GetPosChannel(k)->GetLongId(this);
   conn.Name="";
   res&=CreateLink(item,conn);
  }
 }

 if(!res)
 {
  cont->Free();
  return 0;
 }
 return membrane;
}

// Удаляет заданный участок мембраны
// Если full == true, то удаляет и все другие участки, подключенные к нему
// Иначе перенаправляет связи со входов на свои выходы
bool NPulseNeuron::EraseDendrite(const UId &id)
{
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseNeuron* NPulseNeuron::New(void)
{
 return new NPulseNeuron;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseNeuron::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseMembrane>(comp) ||
	dynamic_pointer_cast<NLTZone>(comp) ||
//	dynamic_cast<const NPulseSynapse*>(comp) ||
	dynamic_pointer_cast<NConstGenerator>(comp))
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
bool NPulseNeuron::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NPulseNeuronCommon::AAddComponent(comp,pointer))
  return false;

 comp->SetMainOwner(this,-1);

  UEPtr<NConstGenerator> temp=dynamic_pointer_cast<NConstGenerator>(comp);

  if(temp && temp->Amplitude()>0)
  {
   if(PosGenerator)
	return false;
   PosGenerator=temp;
   return true;
  }
  else
  if(temp && temp->Amplitude()<0)
  {
   if(NegGenerator)
	return false;
   NegGenerator=temp;
   return true;
  }

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseNeuron::ADelComponent(UEPtr<UContainer> comp)
{
 if(comp == PosGenerator)
  PosGenerator=0;
 else
 if(comp == NegGenerator)
  NegGenerator=0;

 return NNeuron::ADelComponent(comp);
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseNeuron::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseNeuron::ABuild(void)
{
 if(!NPulseNeuronCommon::ABuild())
  return false;

 return true;
}

// Сброс процесса счета.
bool NPulseNeuron::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseNeuron::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 return true;
}
// --------------------------
int NPulseNeuron::GetNumOfConnectedSynToPosCh(NPulseMembrane* membr)
{
  int temp=0;
  for(size_t i=0;i<membr->GetNumPosChannels();i++)
  {
   temp+=membr->GetPosChannel(i)->NumConnectedSynapsis;
  }
  return temp;
}

int NPulseNeuron::GetNumOfConnectedSynToNegCh(NPulseMembrane* membr)
{
  int temp=0;
  for(size_t i=0;i<membr->GetNumNegChannels();i++)
  {
   temp+=membr->GetNegChannel(i)->NumConnectedSynapsis;
  }
  return temp;
}

}
#endif
