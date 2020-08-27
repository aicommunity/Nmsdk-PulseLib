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
 OldNumDendrited=OldNumSoma=0;
}

NPulseNeuron::~NPulseNeuron(void)
{
 Soma.clear();
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
 OldNumSoma=NumSomaMembraneParts;
 Ready=false;
 return true;
}

/// Число участков мембраны дендритов (исключая участок тела)
bool NPulseNeuron::SetNumDendriteMembraneParts(const int &value)
{
 OldNumDendrited=NumDendriteMembraneParts;
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
  UEPtr<NPulseChannelCommon> new_channel=dynamic_pointer_cast<NPulseChannelCommon>(new_dendrite->GetComponentL(channel->GetName(),true));
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
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NPulseNeuron::BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
								  const string &ltzone_class, const string &pos_gen_class,
								  const string &neg_gen_class, int num_soma_membranes,
								  int dendrite_length, int num_stimulates, int num_arresting)
{
 UEPtr<NPulseMembraneCommon> membr=0,ltmembr=0;
 UEPtr<NPulseChannelCommon> channel1, channel2, ltchannel1,ltchannel2, channel1temp,channel2temp;
 UEPtr<NLTZone> ltzone;
 bool res(true);
 RDK::ULinkSide item,conn;

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);//dynamic_pointer_cast<NLTZone>(Storage->TakeObject(ltzone_class));
 ltzone->SetCoord(MVector<double,3>(27.3+dendrite_length*8,4.67,0));

 UEPtr<UNet> gen_pos,gen_neg;
 if(!ExcGeneratorClassName->empty())
 {
  gen_pos=AddMissingComponent<UNet>("PosGenerator", pos_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(pos_gen_class));
  gen_pos->SetCoord(MVector<double,3>(4,2,0));
  gen_pos->DisconnectAll("Output");
 }
 else
  DelComponent("PosGenerator");

 if(!InhGeneratorClassName->empty())
 {
  gen_neg=AddMissingComponent<UNet>("NegGenerator", neg_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(neg_gen_class));
  gen_neg->SetCoord(MVector<double,3>(4,7.3+(num_soma_membranes-1)*2,0));
  gen_neg->DisconnectAll("Output");
 }
 else
  DelComponent("NegGenerator");

 for(int i=NumSomaMembraneParts;i<OldNumSoma;i++)
 {
  DelComponent(std::string("Soma")+sntoa(i+1));
  for(int j=0;j<((OldNumDendrited == 0)?NumDendriteMembraneParts:OldNumDendrited);j++)
  {
   DelComponent(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1));
  }
 }

 int num_soma=(OldNumSoma>NumSomaMembraneParts)?OldNumSoma:NumSomaMembraneParts;
 for(int i=0;i<num_soma;i++)
 {
  for(int j=NumDendriteMembraneParts;j<OldNumDendrited;j++)
  {
   DelComponent(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1));
  }
 }

 // Случай, если задана выделенная часть мембраны генераторной зоны
 if(!ltzonemembraneclass.empty())
 {
  ltmembr=AddMissingComponent<NPulseMembrane>("LTMembrane", ltzonemembraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(ltzonemembraneclass));
  ltmembr->SetCoord(MVector<double,3>(20+dendrite_length*8,4.67,0));

  ltchannel1=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("ExcChannel",true));
  ltchannel2=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("InhChannel",true));

  // Устанавливаем обратную связь
  res&=CreateLink(ltzone->GetLongName(this),"Output",ltmembr->GetLongName(this),"InputFeedbackSignal");

  // Устанавливаем связь мембраны с низкопороговой зоной
  if(ltchannel1)
   res&=CreateLink(ltchannel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
  if(ltchannel2)
   res&=CreateLink(ltchannel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
 }
 else
  DelComponent("LTMembrane");

 Soma.resize(num_soma_membranes);
 for(int i=0;i<num_soma_membranes;i++)
 {
  membr=AddMissingComponent<NPulseMembrane>(std::string("Soma")+sntoa(i+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
  membr->SetCoord(MVector<double,3>(12.7+dendrite_length*8,4.67+i*2,0));
  Soma[i]=membr;

  channel1=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
  channel2=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

  // Случай, если задана выделенная часть мембраны генераторной зоны
  // тогда подключаем сому к ней
  if(!ltzonemembraneclass.empty())
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(this),"Output",ltchannel1->GetLongName(this),"ChannelInputs");
   if(channel2)
	res&=CreateLink(channel2->GetLongName(this),"Output",ltchannel2->GetLongName(this),"ChannelInputs");
  }
  else // иначе подключаем сому напрямую к низкопороговой зоне
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
   if(channel2)
	res&=CreateLink(channel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");

   // Устанавливаем обратную связь
   res&=CreateLink(ltzone->GetLongName(this),"Output",membr->GetLongName(this),"InputFeedbackSignal");
  }

  for(int j=0;j<dendrite_length;j++)
  {
   membr=AddMissingComponent<NPulseMembrane>(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   membr->SetCoord(MVector<double,3>(12.7+(dendrite_length-j-1)*8,4.67+i*2,0));

   channel1temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
   channel2temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

   if(channel1temp)
	res&=CreateLink(channel1temp->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
   if(channel2temp)
    res&=CreateLink(channel2temp->GetLongName(this),"Output",channel2->GetLongName(this),"ChannelInputs");

   channel1 = channel1temp;
   channel2 = channel2temp;
  }

  // Связь между начальными значениями мощностей ионных каналов и каналами
  if(channel1 && gen_neg)
   res&=CreateLink(gen_neg->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
  if(channel2 && gen_pos)
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

 StructureBuildMode=1;
 MembraneClassName="NPMembrane";
 LTMembraneClassName="NPLTZoneNeuronMembrane";
 LTZoneClassName="NPLTZone";
 ExcGeneratorClassName="NPNeuronPosCGenerator";
 InhGeneratorClassName="NPNeuronNegCGenerator";
 NumSomaMembraneParts=1;
 NumDendriteMembraneParts=0;

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

 OldNumDendrited=0;
 OldNumSoma=0;
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

 // Считаем суммарный выходной потеницал всех дендритов и участков сомы
 DendriticSumPotential(0,0)=0;
 SomaSumPotential(0,0)=0;
 for(size_t i=0;i<Soma.size();i++)
 {
  if(Soma[i])
  {
   for(size_t j=0;j<Soma[i]->GetNumChannels();j++)
   {
	DendriticSumPotential(0,0)+=Soma[i]->GetChannel(j)->SumChannelInputs(0,0);
    SomaSumPotential(0,0)+=Soma[i]->GetChannel(j)->Output(0,0);
   }
  }
 }



 return true;
}
// --------------------------
int NPulseNeuron::GetNumOfConnectedSynToPosCh(NPulseMembrane* membr)
{
  int temp=0;
  for(size_t i=0;i<membr->GetNumPosChannels();i++)
  {
   NPulseChannel *ch=dynamic_cast<NPulseChannel*>(membr->GetPosChannel(i));
   temp+=ch->NumConnectedSynapsis;
  }
  return temp;
}

int NPulseNeuron::GetNumOfConnectedSynToNegCh(NPulseMembrane* membr)
{
  int temp=0;
  for(size_t i=0;i<membr->GetNumNegChannels();i++)
  {
   NPulseChannel *ch=dynamic_cast<NPulseChannel*>(membr->GetNegChannel(i));
   temp+=ch->NumConnectedSynapsis;
  }
  return temp;
}

}
#endif
