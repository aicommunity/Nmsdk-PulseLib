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

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include "NPulseNeuron.h"
#include "NPulseMembrane.h"
#include "NPulseLTZone.h"
#include "NPulseSynapse.h"
#include "NPulseChannel.h"
#include "NConstGenerator.h"

namespace NMSDK {

namespace {
constexpr double kDendritePitchX = 11.0;  // was 8.0 — wider for Modern Diagram cards
constexpr double kSomaRowPitchY  = 2.0;
constexpr double kNeuronBaseX   = 12.7;
constexpr double kNeuronBaseY   = 4.67;
} // namespace

// ћетоды
// --------------------------
//  онструкторы и деструкторы
// --------------------------
NPulseNeuron::NPulseNeuron(void)
: StructureBuildMode("StructureBuildMode",this,&NPulseNeuron::SetStructureBuildMode),
  MembraneClassName("MembraneClassName",this,&NPulseNeuron::SetMembraneClassName),
  LTMembraneClassName("LTMembraneClassName",this,&NPulseNeuron::SetLTMembraneClassName),
  LTZoneClassName("LTZoneClassName",this,&NPulseNeuron::SetLTZoneClassName),
  ExcGeneratorClassName("ExcGeneratorClassName",this,&NPulseNeuron::SetExcGeneratorClassName),
  InhGeneratorClassName("InhGeneratorClassName",this,&NPulseNeuron::SetInhGeneratorClassName),
  NumSomaMembraneParts("NumSomaMembraneParts",this,&NPulseNeuron::SetNumSomaMembraneParts),
  NumDendriteMembraneParts("NumDendriteMembraneParts",this,&NPulseNeuron::SetNumDendriteMembraneParts),
  NumDendriteMembranePartsVec("NumDendriteMembranePartsVec",this,&NPulseNeuron::SetNumDendriteMembranePartsVec),
  TrainingPattern("TrainingPattern",this,&NPulseNeuron::SetTrainingPattern),
  TrainingDendIndexes("TrainingDendIndexes",this,&NPulseNeuron::SetTrainingDendIndexes),
  TrainingSynapsisNum("TrainingSynapsisNum",this,&NPulseNeuron::SetTrainingSynapsisNum),
  UseElementDefaults("UseElementDefaults",this,&NPulseNeuron::SetUseElementDefaults),
  MembraneCapacity("MembraneCapacity",this,&NPulseNeuron::SetMembraneCapacity),
  SynapseDissociationTC("SynapseDissociationTC",this,&NPulseNeuron::SetSynapseDissociationTC),
  ExcMembraneResistance("ExcMembraneResistance",this,&NPulseNeuron::SetExcMembraneResistance),
  InhMembraneResistance("InhMembraneResistance",this,&NPulseNeuron::SetInhMembraneResistance),
  ExcSynapseResistance("ExcSynapseResistance",this,&NPulseNeuron::SetExcSynapseResistance),
  InhSynapseResistance("InhSynapseResistance",this,&NPulseNeuron::SetInhSynapseResistance),
  SynapseSecretionTC("SynapseSecretionTC",this,&NPulseNeuron::SetSynapseSecretionTC)
{
 PosGenerator=0;
 NegGenerator=0;
 MainOwner=this;
 OldNumDendrites=OldNumSoma=0;
 OldStructureBuildMode=0;
}

NPulseNeuron::~NPulseNeuron(void)
{
 Soma.clear();
}
// --------------------------

// --------------------------
// ћетоды доступа к временным переменным
// --------------------------
// ¬озвращает указатель на модель источника возбуждаюшего потенциала
NConstGenerator* NPulseNeuron::GetPosGenerator(void)
{
 return PosGenerator;
}

// ¬озвращает указатель на модель источника тормозного потенциала
NConstGenerator* NPulseNeuron::GetNegGenerator(void)
{
 return NegGenerator;
}

/// ƒоступ к участкам мембраны
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
// ћетоды упралени€ параметрами
// --------------------------
/// –ежим сборки структуры нейрона
bool NPulseNeuron::SetStructureBuildMode(const int &value)
{
 if(value >0) // ѕересборка структуры нужна только если StructureBuildMode не 0
 {
  OldStructureBuildMode=StructureBuildMode;
  Ready=false;

  if(value == 2)
  {
   ChangeLookupPropertyType("NumDendriteMembraneParts",ptState);
   ChangeLookupPropertyType("NumDendriteMembranePartsVec",ptPubParameter);
   std::vector<int> vecs;
   vecs.assign(NumSomaMembraneParts.GetData(),NumDendriteMembraneParts.GetData());
   NumDendriteMembranePartsVec=vecs;
  }
  else
  {
   ChangeLookupPropertyType("NumDendriteMembraneParts",ptPubParameter);
   ChangeLookupPropertyType("NumDendriteMembranePartsVec",ptState);

   if(!NumDendriteMembranePartsVec.empty())
	NumDendriteMembraneParts=NumDendriteMembranePartsVec[0];
   else
    NumDendriteMembraneParts=0;
  }
 }
 return true;
}

/// »м€ класса участка мембраны
bool NPulseNeuron::SetMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// »м€ класса выделенного участка мембраны дл€ генераторной зоны
bool NPulseNeuron::SetLTMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// »м€ класса генераторной зоны
bool NPulseNeuron::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// »м€ класса источника задающего сигнала дл€ возбуждающего ионного механизма
bool NPulseNeuron::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// »м€ класса источника задающего сигнала дл€ тормозного ионного механизма
bool NPulseNeuron::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseNeuron::SetUseElementDefaults(const bool &value)
{
 (void)value;
 // Do not clear Ready: Cap/TC are applied in ApplyElementDefaults after ABuild.
 return true;
}

bool NPulseNeuron::SetMembraneCapacity(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetSynapseDissociationTC(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetExcMembraneResistance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetInhMembraneResistance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetExcSynapseResistance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetInhSynapseResistance(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NPulseNeuron::SetSynapseSecretionTC(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}


/// „исло участков мембраны тела нейрона
bool NPulseNeuron::SetNumSomaMembraneParts(const int &value)
{
 OldNumSoma=NumSomaMembraneParts;
 OldNumDendritesVec=NumDendriteMembranePartsVec;
 NumDendriteMembranePartsVec->resize(value,0);
 Ready=false;
 return true;
}

/// „исло участков мембраны дендритов (исключа€ участок тела)
bool NPulseNeuron::SetNumDendriteMembraneParts(const int &value)
{
 OldNumDendrites=NumDendriteMembraneParts;
 Ready=false;
 return true;
}


/// „исло участков мембраны дендритов (исключа€ участок тела)
bool NPulseNeuron::SetNumDendriteMembranePartsVec(const std::vector<int> &value)
{
 OldNumDendritesVec=NumDendriteMembranePartsVec;
 Ready=false;
 return true;
}

/// ѕаттерн, которому обучен нейрон
/// (если нейрон не обучен Size = 0)
/// Ќј«Ќј„ј≈“—я ѕ–ќ√–јћћЌќ! Ќ≈ ƒќЋ∆≈Ќ ћ≈Ќя“№—я ѕќЋ№«ќ¬ј“≈Ћ≈ћ!!!
bool NPulseNeuron::SetTrainingPattern(const MDMatrix<double> &value)
{
 return true;
}

/// »ндексы входных участков на дендритах
/// (если нейрон не обучен Size = 0)
/// Ќј«Ќј„ј≈“—я ѕ–ќ√–јћћЌќ! Ќ≈ ƒќЋ∆≈Ќ ћ≈Ќя“№—я ѕќЋ№«ќ¬ј“≈Ћ≈ћ!!!
bool NPulseNeuron::SetTrainingDendIndexes(const MDMatrix<int> &value)
{
 return true;
}

///  оличество синапсов на входных участков на дендритах
/// (если нейрон не обучен Size = 0)
/// Ќј«Ќј„ј≈“—я ѕ–ќ√–јћћЌќ! Ќ≈ ƒќЋ∆≈Ќ ћ≈Ќя“№—я ѕќЋ№«ќ¬ј“≈Ћ≈ћ!!!
bool NPulseNeuron::SetTrainingSynapsisNum(const MDMatrix<int> &value)
{
 return true;
}
// --------------------------

// --------------------------
// ћетоды управлени€ структурой объекта
// --------------------------
// ”длинн€ет заданный участок мембраны, добавл€€ к нему новый участок мембраны,
// и переключа€ входы заданного участка на входы нового
// ¬озвращает указатель на созданный участок
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

// –азветвл€ет заданный участок мембраны, добавл€€ к точке его подключени€
// дополнительно новый участок мембраны
// ¬озвращает указатель на созданный участок
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

 RDK::ULinkSide item, link_conn;
 bool res=true;

 // ”станавливаем обратную св€зь
 if(feedback)
 {
  res&=CreateLink(GetLTZone()->GetLongName(this),"Output", new_dendrite->GetLongName(this),"InputFeedbackSignal");
 }

 // ѕодключаемс€ каналами к приемникам
 int size=dendrite->GetNumComponents();
 if(size>new_dendrite->GetNumComponents())
  size=new_dendrite->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(dendrite->GetComponentByIndex(k));
  if(!channel)
	continue;
  UEPtr<NPulseChannelCommon> new_channel=dynamic_pointer_cast<NPulseChannelCommon>(new_dendrite->GetComponentL(channel->GetName(),true));
  if(!new_channel)
  	continue;
 /* for(int i=0;i<channel->Output. ;i++)
  {
   NPulseSynapseCommon* synapse=dynamic_cast<NPulseSynapseCommon*>(channel->Inputs.GetItem(i));
   if(!synapse)
	res&=CreateLink(Inputs.GetItem(i)->GetLongName(this),Inputs.GetItemOutputName(i),);
  }                */

  int num_connectors=int(channel->Output.GetNumConnectors());
  for(int i=0;i<num_connectors;i++)
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

 // ѕодключаем источники мембранных потенциалов
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

// ”дал€ет заданный участок мембраны
// ≈сли full == true, то удал€ет и все другие участки, подключенные к нему
// »наче перенаправл€ет св€зи со входов на свои выходы
bool NPulseNeuron::EraseDendrite(const std::string &name)
{
 return true;
}
// --------------------------

// --------------------------
// —истемные методы управлени€ объектом
// --------------------------
// ¬ыдел€ет пам€ть дл€ новой чистой копии объекта этого класса
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
// ћетоды доступа к компонентам
// --------------------------
// ћетод провер€ет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// ћетод возвращает 'true' в случае допустимости
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
// —крытые методы управлени€ компонентами
// --------------------------
// ¬ыполн€ет завершающие пользовательские действи€
// при добавлении дочернего компонента в этот объект
// ћетод будет вызван только если comp был
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

// ¬ыполн€ет предварительные пользовательские действи€
// при удалении дочернего компонента из этого объекта
// ћетод будет вызван только если comp
// существует в списке компонент
bool NPulseNeuron::ADelComponent(UEPtr<UContainer> comp)
{
 if(comp == PosGenerator)
  PosGenerator=0;
 else
 if(comp == NegGenerator)
  NegGenerator=0;

 return NPulseNeuronCommon::ADelComponent(comp);
}
// --------------------------

// --------------------------
// —крытые методы управлени€ счетом
// --------------------------
// ќсуществл€ет сборку структуры в соответствии с выбранными именами компонентов
bool NPulseNeuron::BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
								  const string &ltzone_class, const string &pos_gen_class,
								  const string &neg_gen_class, int num_soma_membranes, int dendrite_mode,
								  int dendrite_length, const vector<int> &dendrite_length_vec, int num_stimulates, int num_arresting)
{
 UEPtr<NPulseMembraneCommon> membr=0,ltmembr=0;
 UEPtr<NPulseChannelCommon> channel1, channel2, ltchannel1,ltchannel2, channel1temp,channel2temp;
 UEPtr<NLTZone> ltzone;
 bool res(true);
  RDK::ULinkSide item, link_conn;

 if(dendrite_mode == 2)
 {
  dendrite_length=0;
  for(size_t i=0;i<dendrite_length_vec.size();i++)
   if(dendrite_length<dendrite_length_vec[i])
	dendrite_length=dendrite_length_vec[i];

  if(int(OldNumDendritesVec.size()) != std::max(OldNumSoma, num_soma_membranes))
  {
   OldNumDendritesVec.resize(std::max(OldNumSoma, num_soma_membranes),0);
  }
 }

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);//dynamic_pointer_cast<NLTZone>(Storage->TakeObject(ltzone_class));
 if(!ltzone)
 {
  LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
    std::string("Failed to create LTZone with class: ")+ltzone_class);
  return false;
 }
 ltzone->SetCoord(MVector<double,3>(27.3+dendrite_length*kDendritePitchX,kNeuronBaseY,0));

 UEPtr<UNet> gen_pos,gen_neg;
 if(!ExcGeneratorClassName->empty())
 {
  gen_pos=AddMissingComponent<UNet>("PosGenerator", pos_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(pos_gen_class));
  if(!gen_pos)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create PosGenerator with class: ")+pos_gen_class);
   return false;
  }
  gen_pos->SetCoord(MVector<double,3>(4,2,0));
  gen_pos->DisconnectAll("Output");
 }
 else
  DelComponent("PosGenerator");

 if(!InhGeneratorClassName->empty())
 {
  gen_neg=AddMissingComponent<UNet>("NegGenerator", neg_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(neg_gen_class));
  if(!gen_neg)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create NegGenerator with class: ")+neg_gen_class);
   return false;
  }
  gen_neg->SetCoord(MVector<double,3>(4,7.3+(num_soma_membranes-1)*2,0));
  gen_neg->DisconnectAll("Output");
 }
 else
  DelComponent("NegGenerator");

 for(int i=NumSomaMembraneParts;i<OldNumSoma;i++)
 {
  DelComponent(std::string("Soma")+sntoa(i+1));

  int current_num_dendrite_parts(0), old_num_dendrite_parts(0);
  if(OldStructureBuildMode == 1)
  {
   if(int(NumDendriteMembranePartsVec.size())>i)
    current_num_dendrite_parts=NumDendriteMembranePartsVec[i];

   old_num_dendrite_parts=NumDendriteMembraneParts;
  }
  else
  if(OldStructureBuildMode == 2)
  {
   current_num_dendrite_parts=NumDendriteMembraneParts;
   if(int(NumDendriteMembranePartsVec.size())>i)
    old_num_dendrite_parts=NumDendriteMembranePartsVec[i];
  }
  else
  if(dendrite_mode == 1)
  {
   current_num_dendrite_parts=NumDendriteMembraneParts;
   old_num_dendrite_parts=OldNumDendrites;
  }
  else
  {
   if(int(NumDendriteMembranePartsVec.size())>i)
    current_num_dendrite_parts=NumDendriteMembranePartsVec[i];
   old_num_dendrite_parts=OldNumDendritesVec[i];
  }

  for(int j=0;j<((old_num_dendrite_parts == 0)?current_num_dendrite_parts:old_num_dendrite_parts);j++)
   DelComponent(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1));
 }

 int num_soma=(OldNumSoma>NumSomaMembraneParts)?OldNumSoma:NumSomaMembraneParts;
 for(int i=0;i<num_soma;i++)
 {
  int current_num_dendrite_parts(0), old_num_dendrite_parts(0);
  if(OldStructureBuildMode == 1)
  {
   if(int(NumDendriteMembranePartsVec.size())>i)
    current_num_dendrite_parts=NumDendriteMembranePartsVec[i];

   old_num_dendrite_parts=NumDendriteMembraneParts;
  }
  else
  if(OldStructureBuildMode == 2)
  {
   current_num_dendrite_parts=NumDendriteMembraneParts;
   if(int(NumDendriteMembranePartsVec.size())>i)
    old_num_dendrite_parts=NumDendriteMembranePartsVec[i];
  }
  else
  if(dendrite_mode == 1)
  {
   current_num_dendrite_parts=NumDendriteMembraneParts;
   old_num_dendrite_parts=OldNumDendrites;
  }
  else
  {
   if(int(NumDendriteMembranePartsVec.size())>i)
    current_num_dendrite_parts=NumDendriteMembranePartsVec[i];
   old_num_dendrite_parts=OldNumDendritesVec[i];
  }

  for(int j=current_num_dendrite_parts;j<old_num_dendrite_parts;j++)
   DelComponent(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1));
 }

 // —лучай, если задана выделенна€ часть мембраны генераторной зоны
 if(!ltzonemembraneclass.empty())
 {
  ltmembr=AddMissingComponent<NPulseMembrane>("LTMembrane", ltzonemembraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(ltzonemembraneclass));
  if(!ltmembr)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create LTMembrane with class: ")+ltzonemembraneclass);
   return false;
  }
  ltmembr->SetCoord(MVector<double,3>(20+dendrite_length*kDendritePitchX,kNeuronBaseY,0));
  // Ensure channels exist: TakeObject prototypes may be Ready with empty Channels[].
  if(!ltmembr->GetComponent("ExcChannel", true) || ltmembr->GetNumChannels() == 0)
  {
   if(UEPtr<NPulseMembrane> pm = dynamic_pointer_cast<NPulseMembrane>(ltmembr))
   {
    const int nsyn = pm->NumExcitatorySynapses;
    pm->NumExcitatorySynapses = nsyn + 1;
    pm->NumExcitatorySynapses = nsyn;
   }
   if(!ltmembr->Build())
   {
    LogMessageEx(RDK_EX_ERROR, __FUNCTION__, "Failed to Build LTMembrane");
    return false;
   }
  }

  ltchannel1=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("ExcChannel",true));
  ltchannel2=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("InhChannel",true));

  // ”станавливаем обратную св€зь
  res&=CreateLink(ltzone->GetLongName(this),"Output",ltmembr->GetLongName(this),"InputFeedbackSignal");

  // ”станавливаем св€зь мембраны с низкопороговой зоной
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
  if(!membr)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create Soma")+sntoa(i+1)+std::string(" with class: ")+membraneclass);
   return false;
  }
  membr->SetCoord(MVector<double,3>(kNeuronBaseX+dendrite_length*kDendritePitchX,kNeuronBaseY+i*kSomaRowPitchY,0));
  // Ensure channels exist before CreateLink (TakeObject may be Ready w/o Channels).
  if(!membr->GetComponent("ExcChannel", true) || membr->GetNumChannels() == 0)
  {
   if(UEPtr<NPulseMembrane> pm = dynamic_pointer_cast<NPulseMembrane>(membr))
   {
    const int nsyn = pm->NumExcitatorySynapses;
    pm->NumExcitatorySynapses = nsyn + 1;
    pm->NumExcitatorySynapses = nsyn;
   }
   if(!membr->Build())
   {
    LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
      std::string("Failed to Build Soma")+sntoa(i+1));
    return false;
   }
  }
  // TakeObject clones often keep Activity=false; Calculate() would skip them.
  if(!membr->GetActivity())
   membr->SetActivity(true);
  if(!membr->IsInit())
   membr->Init();
  Soma[i]=membr;

  channel1=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
  channel2=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

  // —лучай, если задана выделенна€ часть мембраны генераторной зоны
  // тогда подключаем сому к ней
  if(!ltzonemembraneclass.empty())
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(this),"Output",ltchannel1->GetLongName(this),"ChannelInputs");
   if(channel2)
	res&=CreateLink(channel2->GetLongName(this),"Output",ltchannel2->GetLongName(this),"ChannelInputs");
  }
  else // иначе подключаем сому напр€мую к низкопороговой зоне
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
   if(channel2)
	res&=CreateLink(channel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");

   // ”станавливаем обратную св€зь
   res&=CreateLink(ltzone->GetLongName(this),"Output",membr->GetLongName(this),"InputFeedbackSignal");
  }

  int current_dendrite_length(0);
  if(dendrite_mode == 1)
   current_dendrite_length=dendrite_length;
  else
   current_dendrite_length=dendrite_length_vec[i];


  for(int j=0;j<current_dendrite_length;j++)
  {
   membr=AddMissingComponent<NPulseMembrane>(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   if(!membr)
   {
    LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
      std::string("Failed to create Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1)+std::string(" with class: ")+membraneclass);
    return false;
   }
   membr->SetCoord(MVector<double,3>(kNeuronBaseX+(dendrite_length-j-1)*kDendritePitchX,kNeuronBaseY+i*kSomaRowPitchY,0));
   // New distal segments need real Channels[] before cable/PosNeg links.
   // TakeObject prototypes can be Ready with ExcChannel child but empty Channels.
   if(!membr->GetComponent("ExcChannel", true) || membr->GetNumChannels() == 0)
   {
    if(UEPtr<NPulseMembrane> pm = dynamic_pointer_cast<NPulseMembrane>(membr))
    {
     const int nsyn = pm->NumExcitatorySynapses;
     pm->NumExcitatorySynapses = nsyn + 1;
     pm->NumExcitatorySynapses = nsyn;
    }
    if(!membr->Build())
    {
     LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
       std::string("Failed to Build Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1));
     return false;
    }
   }
   // Newly added segments from TakeObject may be inactive — Calculate would no-op.
   if(!membr->GetActivity())
    membr->SetActivity(true);
   if(!membr->IsInit())
    membr->Init();

   channel1temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
   channel2temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

   if(channel1temp)
	res&=CreateLink(channel1temp->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
   if(channel2temp)
    res&=CreateLink(channel2temp->GetLongName(this),"Output",channel2->GetLongName(this),"ChannelInputs");

   channel1 = channel1temp;
   channel2 = channel2temp;
  }

  // —в€зь между начальными значени€ми мощностей ионных каналов и каналами
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

void NPulseNeuron::ApplyElementDefaults(void)
{
 if(!UseElementDefaults.GetData())
  return;

 const double cap = MembraneCapacity.GetData();
 const double dissoc = SynapseDissociationTC.GetData();
 const double secr = SynapseSecretionTC.GetData();
 const double exc_rm = ExcMembraneResistance.GetData();
 const double inh_rm = InhMembraneResistance.GetData();
 const double exc_rsyn = ExcSynapseResistance.GetData();
 const double inh_rsyn = InhSynapseResistance.GetData();
 if(cap <= 0.0 && dissoc <= 0.0 && secr <= 0.0
    && exc_rm <= 0.0 && inh_rm <= 0.0
    && exc_rsyn <= 0.0 && inh_rsyn <= 0.0)
  return;

 for(size_t mi = 0; mi < Membranes.size(); ++mi)
 {
  NPulseMembraneCommon *mcommon = Membranes[mi];
  if(!mcommon)
   continue;
  if(mcommon->GetName() == "LTMembrane")
   continue;

  NPulseMembrane *membr = dynamic_cast<NPulseMembrane*>(mcommon);
  if(!membr)
   continue;

  if(cap > 0.0 || exc_rm > 0.0 || inh_rm > 0.0)
  {
   UEPtr<UContainer> exc_c = membr->GetComponent("ExcChannel", true);
   if(NPulseChannel *exc = dynamic_cast<NPulseChannel*>((UContainer*)exc_c))
   {
    if(cap > 0.0)
     exc->Capacity = cap;
    if(exc_rm > 0.0)
    {
     exc->Resistance = exc_rm;
     exc->RestingResistance = exc_rm;
    }
   }
   UEPtr<UContainer> inh_c = membr->GetComponent("InhChannel", true);
   if(NPulseChannel *inh = dynamic_cast<NPulseChannel*>((UContainer*)inh_c))
   {
    if(cap > 0.0)
     inh->Capacity = cap;
    if(inh_rm > 0.0)
    {
     inh->Resistance = inh_rm;
     inh->RestingResistance = inh_rm;
    }
   }
  }

  if(dissoc > 0.0 || secr > 0.0 || exc_rsyn > 0.0 || inh_rsyn > 0.0)
  {
   const int nexc = int(membr->NumExcitatorySynapses);
   const int ninh = int(membr->NumInhibitorySynapses);
   for(int i = 0; i < nexc; ++i)
   {
    UEPtr<UContainer> sc = membr->GetComponent(std::string("ExcSynapse")+sntoa(i+1), true);
    if(NPulseSynapse *syn = dynamic_cast<NPulseSynapse*>((UContainer*)sc))
    {
     if(dissoc > 0.0)
      syn->DissociationTC = dissoc;
     if(secr > 0.0)
      syn->SecretionTC = secr;
     if(exc_rsyn > 0.0)
      syn->Resistance = exc_rsyn;
    }
   }
   for(int i = 0; i < ninh; ++i)
   {
    UEPtr<UContainer> sc = membr->GetComponent(std::string("InhSynapse")+sntoa(i+1), true);
    if(NPulseSynapse *syn = dynamic_cast<NPulseSynapse*>((UContainer*)sc))
    {
     if(dissoc > 0.0)
      syn->DissociationTC = dissoc;
     if(secr > 0.0)
      syn->SecretionTC = secr;
     if(inh_rsyn > 0.0)
      syn->Resistance = inh_rsyn;
    }
   }
  }
 }
}

// ¬осстановление настроек по умолчанию и сброс процесса счета
bool NPulseNeuron::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;

 StructureBuildMode=2;
 MembraneClassName="NPMembrane";
 LTMembraneClassName="NPLTZoneNeuronMembrane";
 LTZoneClassName="NPLTZone";
 ExcGeneratorClassName="NPNeuronPosCGenerator";
 InhGeneratorClassName="NPNeuronNegCGenerator";
 NumSomaMembraneParts=1;
 NumDendriteMembraneParts=0;
 TrainingPattern.Resize(0,0);
 TrainingDendIndexes.Resize(0,0);
 TrainingSynapsisNum.Resize(0,0);
 UseElementDefaults = false;
 MembraneCapacity = 0.0;
 SynapseDissociationTC = 0.0;
 ExcMembraneResistance = 0.0;
 InhMembraneResistance = 0.0;
 ExcSynapseResistance = 0.0;
 InhSynapseResistance = 0.0;
 SynapseSecretionTC = 0.0;

 return true;
}

// ќбеспечивает сборку внутренней структуры объекта
// после настройки параметров
// јвтоматически вызывает метод Reset() и выставл€ет Ready в true
// в случае успешной сборки
bool NPulseNeuron::ABuild(void)
{
 if(StructureBuildMode == 1)
 {
  bool res=BuildStructure(MembraneClassName, LTMembraneClassName, LTZoneClassName,
							ExcGeneratorClassName, InhGeneratorClassName,
							NumSomaMembraneParts, 1,
							NumDendriteMembraneParts, NumDendriteMembranePartsVec,
							1, 1);
  if(!res)
   return false;
 }
 else
 if(StructureBuildMode == 2)
 {
  bool res=BuildStructure(MembraneClassName, LTMembraneClassName, LTZoneClassName,
							ExcGeneratorClassName, InhGeneratorClassName,
							NumSomaMembraneParts, 2,
							NumDendriteMembraneParts, NumDendriteMembranePartsVec,
							1, 1);
  if(!res)
   return false;
 }

 OldNumDendrites=0;
 OldNumSoma=0;
 OldStructureBuildMode=0;

 if(!NPulseNeuronCommon::ABuild())
  return false;

 ApplyElementDefaults();
 return true;
}

// —брос процесса счета.
bool NPulseNeuron::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// ¬ыполн€ет расчет этого объекта
bool NPulseNeuron::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 // —читаем суммарный выходной потеницал всех дендритов и участков сомы
 DendriticSumPotential(0,0)=0;
 SomaSumPotential(0,0)=0;

 int channel_divisor=1;
 UEPtr<NPulseLTZoneCommon> zone = dynamic_pointer_cast<NPulseLTZoneCommon>(LTZone);
 if(zone && zone->NumChannelsInGroup.GetData() > 0)
  channel_divisor = zone->NumChannelsInGroup.GetData();

 int full_ch_counter(0);
 for(size_t i=0;i<Soma.size();i++)
 {
  if(Soma[i])
  {
   int soma_ch_counter(0);
   for(size_t j=0;j<Soma[i]->GetNumChannels();j++)
   {
    DendriticSumPotential(0,0)+=Soma[i]->GetChannel(j)->SumChannelInput(0,0);
    SomaSumPotential(0,0)+=Soma[i]->GetChannel(j)->Output(0,0);
    ++soma_ch_counter;
   }
   full_ch_counter += soma_ch_counter;
  }
 }

 int div_coeff = full_ch_counter/channel_divisor;
 if(div_coeff > 0)
 {
  SomaSumPotential(0,0) /= div_coeff;
  DendriticSumPotential(0,0) /= div_coeff;
 }

 return true;
}


// ”станавливает компоненты в требуемый пор€док расчета
void NPulseNeuron::UpdateComputationOrder(void)
{
 int position = 0;
 int num_soma=NumSomaMembraneParts;
 for(int i=0;i<num_soma;i++)
 {
  if(StructureBuildMode == 2)
  {
   for(int j=int(NumDendriteMembranePartsVec[i])-1;j>=0;j--)
   {
    std::string dendrite_name=std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1);
    SetComponentPosition(dendrite_name, position++);
   }
  }
  else
  if(StructureBuildMode == 1)
  {
   for(int j=NumDendriteMembraneParts-1;j>=0;j--)
   {
    std::string dendrite_name=std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1);
    SetComponentPosition(dendrite_name, position++);
   }
  }
 }

 for(int i=0;i<num_soma;i++)
 {
  std::string soma_name=std::string("Soma")+sntoa(i+1);
  SetComponentPosition(soma_name, position++);
 }

 if(!LTMembraneClassName->empty())
  SetComponentPosition("LTMembrane", position++);

 if(GetLTZone())
  SetComponentPosition(GetLTZone()->GetName(), position++);
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
