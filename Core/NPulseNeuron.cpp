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
: StructureBuildMode("StructureBuildMode",this,&NPulseNeuron::SetStructureBuildMode),
  MembraneClassName("MembraneClassName",this,&NPulseNeuron::SetMembraneClassName),
  LTMembraneClassName("LTMembraneClassName",this,&NPulseNeuron::SetLTMembraneClassName),
  LTZoneClassName("LTZoneClassName",this,&NPulseNeuron::SetLTZoneClassName),
  ExcGeneratorClassName("ExcGeneratorClassName",this,&NPulseNeuron::SetExcGeneratorClassName),
  InhGeneratorClassName("InhGeneratorClassName",this,&NPulseNeuron::SetInhGeneratorClassName),
  NumSomaMembraneParts("NumSomaMembraneParts",this,&NPulseNeuron::SetNumSomaMembraneParts),
  NumDendriteMembraneParts("NumDendriteMembraneParts",this,&NPulseNeuron::SetNumDendriteMembraneParts)
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
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool NPulseNeuron::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса участка мембраны
bool NPulseNeuron::SetMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса выделенного участка мембраны для генераторной зоны
bool NPulseNeuron::SetLTMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса генераторной зоны
bool NPulseNeuron::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
bool NPulseNeuron::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса источника задающего сигнала для тормозного ионного механизма
bool NPulseNeuron::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число участков мембраны тела нейрона
bool NPulseNeuron::SetNumSomaMembraneParts(const int &value)
{
 Ready=false;
 return true;
}

/// Число участков мембраны дендритов (исключая участок тела)
bool NPulseNeuron::SetNumDendriteMembraneParts(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// Методы управления структурой объекта
// --------------------------
// Удлинняет заданный участок мембраны, добавляя к нему новый участок мембраны,
// и переключая входы заданного участка на входы нового
// Возвращает указатель на созданный участок
NPulseMembraneCommon* NPulseNeuron::ElongateDendrite(const std::string &name, bool feedback)
{
 if(!Storage)
  return 0;

 UEPtr<UContainer> cont=GetComponent(name);
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
NPulseMembraneCommon* NPulseNeuron::BranchDendrite(const std::string &name, bool feedback)
{
 if(!Storage)
  return 0;

 UEPtr<NPulseMembrane> dendrite=dynamic_pointer_cast<NPulseMembrane>(GetComponent(name));
 if(!dendrite)
  return 0;

 UEPtr<NPulseMembrane> new_dendrite=dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(dendrite->GetClass()));
 if(!AddComponent(new_dendrite))
 {
  new_dendrite->Free();
  return 0;
 }

 RDK::ULinkSide item,conn;
 bool res=true;

 // Устанавливаем обратную связь
 if(feedback)
 {
  res&=CreateLink(GetLTZone()->GetLongName(this),"Output", new_dendrite->GetLongName(this),"InputFeedbackSignal");
 }

 // Подключаемся каналами к приемникам
 int size=dendrite->GetNumComponents();
 if(size>new_dendrite->GetNumComponents())
  size=new_dendrite->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(dendrite->GetComponentByIndex(k));
  UEPtr<NPulseChannelCommon> new_channel=dynamic_pointer_cast<NPulseChannelCommon>(new_dendrite->GetComponentL(channel->GetName()));
  if(!channel || !new_channel)
   continue;
 /* for(int i=0;i<channel->Output. ;i++)
  {
   NPulseSynapseCommon* synapse=dynamic_cast<NPulseSynapseCommon*>(channel->Inputs.GetItem(i));
   if(!synapse)
	res&=CreateLink(Inputs.GetItem(i)->GetLongName(this),Inputs.GetItemOutputName(i),);
  }                */

  size_t num_connectors=channel->Output.GetNumConnectors();
  for(size_t i=0;i<num_connectors;i++)
  {
   UConnector* conn=channel->Output.GetConnector(i);
   if(!conn)
	continue;
   std::string conn_input_prop_name=channel->Output.GetConnectorInputName(i);

   res &=CreateLink(new_channel->GetLongName(this),"Output",conn->GetLongName(this),conn_input_prop_name);
  }

   /*
  for(int i=0;i<channel->GetNumOutputs();i++)
   for(int j=0;j<channel->GetNumAConnectors(i);j++)
   {
//	item.Id=new_dendrite->GetComponentByIndex(k)->GetLongId(this);
	item.Id=new_dendrite->GetComponent(channel->GetName())->GetLongId(this);
	item.Index=0;
	conn.Id=channel->GetAConnectorByIndex(int(i),j)->GetLongId(this);
	conn.Index=-1;
	res&=CreateLink(item,conn);
   }    */
 }

 // Подключаем источники мембранных потенциалов
 //UEPtr<NPulseMembrane> membrane=static_pointer_cast<NPulseMembrane>(cont);
 for(size_t k=0;k<new_dendrite->GetNumNegChannels();k++)
 {
  res&=CreateLink(PosGenerator->GetLongName(this),"Output",new_dendrite->GetNegChannel(k)->GetLongName(this),"Inputs");
 /* item.Id=PosGenerator->GetLongName(this);
  item.Index=0;
  conn.Id=membrane->GetNegChannel(k)->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn); */
 }

 for(size_t k=0;k<new_dendrite->GetNumPosChannels();k++)
 {
  res&=CreateLink(NegGenerator->GetLongName(this),"Output",new_dendrite->GetPosChannel(k)->GetLongName(this),"Inputs");
 /*
  item.Id=NegGenerator->GetLongId(this);
  item.Index=0;
  conn.Id=membrane->GetPosChannel(k)->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn);        */
 }


 if(!res)
 {
  new_dendrite->Free();
//  Storage->ReturnObject(cont);
  return 0;
 }
 return new_dendrite;
}

// Удаляет заданный участок мембраны
// Если full == true, то удаляет и все другие участки, подключенные к нему
// Иначе перенаправляет связи со входов на свои выходы
bool NPulseNeuron::EraseDendrite(const std::string &name)
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

UComponent* NPulseNeuron::NewStatic(void)
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
 if(dynamic_pointer_cast<NPulseMembraneCommon>(comp) ||
	dynamic_pointer_cast<NPulseLTZoneCommon>(comp) ||
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
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NPulseNeuron::BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
								  const string &ltzone_class, const string &pos_gen_class,
								  const string &neg_gen_class, int num_soma_membranes,
								  int dendrite_length, int num_stimulates, int num_arresting)
{
 UEPtr<UContainer> membr=0,ltmembr=0;
 UEPtr<NPulseChannel> channel1, channel2, ltchannel1,ltchannel2, channel1temp,channel2temp;
 UEPtr<NNet> ltzone;
 bool res(true);
 RDK::ULinkSide item,conn;

 ltzone=dynamic_pointer_cast<NNet>(Storage->TakeObject(ltzone_class));
 res&=AddComponent(ltzone,&LTZone);
 ltzone->SetName("LTZone");

 UEPtr<UNet> gen_pos,gen_neg;
 gen_pos=dynamic_pointer_cast<UNet>(Storage->TakeObject(pos_gen_class));
 res&=AddComponent(gen_pos);
 gen_neg=dynamic_pointer_cast<UNet>(Storage->TakeObject(neg_gen_class));
 res&=AddComponent(gen_neg);

 // Случай, если задана выделенная часть мембраны генераторной зоны
 if(!ltzonemembraneclass.empty())
 {
  ltmembr=dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(ltzonemembraneclass));
  ltmembr->SetName("LTMembrane");
  res&=AddComponent(ltmembr);

  ltchannel1=dynamic_pointer_cast<NPulseChannel>(ltmembr->GetComponent("ExcChannel"));
  ltchannel2=dynamic_pointer_cast<NPulseChannel>(ltmembr->GetComponent("InhChannel"));

  // Устанавливаем обратную связь
  res&=CreateLink(ltzone->GetLongName(this),"Output",ltmembr->GetLongName(this),"InputFeedbackSignal");

  // Устанавливаем связь мембраны с низкопороговой зоной
  res&=CreateLink(ltchannel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
  res&=CreateLink(ltchannel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
 }

 for(int i=0;i<num_soma_membranes;i++)
 {
  membr=dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
  res&=AddComponent(membr);

  channel1=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("ExcChannel"));
  channel2=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("InhChannel"));

  // Случай, если задана выделенная часть мембраны генераторной зоны
  // тогда подключаем сому к ней
  if(!ltzonemembraneclass.empty())
  {
   res&=CreateLink(channel1->GetLongName(this),"Output",ltchannel1->GetLongName(this),"ChannelInputs");
   res&=CreateLink(channel2->GetLongName(this),"Output",ltchannel2->GetLongName(this),"ChannelInputs");
  }
  else // иначе подключаем сому напрямую к низкопороговой зоне
  {
   res&=CreateLink(channel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
   res&=CreateLink(channel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
  }

  for(int j=0;j<dendrite_length;j++)
  {
   membr=dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   res&=AddComponent(membr);

   channel1temp=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("ExcChannel"));
   channel2temp=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("InhChannel"));

   res&=CreateLink(channel1temp->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
   res&=CreateLink(channel2temp->GetLongName(this),"Output",channel2->GetLongName(this),"ChannelInputs");

   channel1 = channel1temp;
   channel2 = channel2temp;
  }

  // Связь между начальными значениями мощностей ионных каналов и каналами
  res&=CreateLink(gen_neg->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
  res&=CreateLink(gen_pos->GetLongName(this),"Output",channel2->GetLongName(this),"ChannelInputs");
 }

  /*
 UEPtr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(this);
 if(lifeneuron)
 {
  UEPtr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(Storage->TakeObject("NNeuronLife"));
  res&=lifeneuron->AddComponent(nlife);
  res&=n->CreateLink(ltzone->GetLongName(this),"Output",nlife->GetLongId(this),"Input1");
 }  */

 if(!res)
  return false;

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseNeuron::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;

 StructureBuildMode=0;
 MembraneClassName="NPMembrane";
 LTMembraneClassName="NPLTMembrane";
 LTZoneClassName="NPLTZone";
 ExcGeneratorClassName="NPNeuronPosCGenerator";
 InhGeneratorClassName="NPNeuronNegCGenerator";
 NumSomaMembraneParts=1;
 NumDendriteMembraneParts=1;

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

 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(MembraneClassName, LTMembraneClassName, LTZoneClassName,
							ExcGeneratorClassName, InhGeneratorClassName,
							NumSomaMembraneParts,
							NumDendriteMembraneParts,
							1, 1);
  if(!res)
   return false;
 }
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
