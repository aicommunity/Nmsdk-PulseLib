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

#ifndef NNEURONFREQGROUP_CPP
#define NNEURONFREQGROUP_CPP

#include "NNeuronFreqGroup.h"
#include "../../Nmsdk-PulseLib/Core/NReceptor.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"


// Класс, создающий группу афферентных нейронов, чувствительных к разным диапазонам частот
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NNeuronFreqGroup::NNeuronFreqGroup(void)
: StructureBuildMode("StructureBuildMode",this,&NNeuronFreqGroup::SetStructureBuildMode),
  AffNeuronClassName("AffNeuronClassName",this,&NNeuronFreqGroup::SetAffNeuronClassName),
  NumAffNeurons("NumAffNeurons",this,&NNeuronFreqGroup::SetNumAffNeurons),
  MinInputFreq("MinInputFreq",this,&NNeuronFreqGroup::SetMinInputFreq),
  MaxInputFreq("MaxInputFreq",this,&NNeuronFreqGroup::SetMaxInputFreq)
//  LTMembraneClassName("LTMembraneClassName",this,&NNeuronFreqGroup::SetLTMembraneClassName),
 // LTZoneClassName("LTZoneClassName",this,&NNeuronFreqGroup::SetLTZoneClassName),
 // ExcGeneratorClassName("ExcGeneratorClassName",this,&NNeuronFreqGroup::SetExcGeneratorClassName),
 // InhGeneratorClassName("InhGeneratorClassName",this,&NNeuronFreqGroup::SetInhGeneratorClassName),
 // NumSomaMembraneParts("NumSomaMembraneParts",this,&NNeuronFreqGroup::SetNumSomaMembraneParts),
 // NumDendriteMembraneParts("NumDendriteMembraneParts",this,&NNeuronFreqGroup::SetNumDendriteMembraneParts)
{
// MainOwner=this;
 OldNumAffNeurons=0;
}

NNeuronFreqGroup::~NNeuronFreqGroup(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool NNeuronFreqGroup::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса участка мембраны
bool NNeuronFreqGroup::SetAffNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число афферентных нейронов
bool NNeuronFreqGroup::SetNumAffNeurons(const int &value)
{
 Ready=false;
 OldNumAffNeurons = NumAffNeurons;
 return true;
}

/// Минимальная входная частота
bool NNeuronFreqGroup::SetMinInputFreq(const int &value)
{
 Ready=false;
 return true;
}

/// Максимальная входная частота
bool NNeuronFreqGroup::SetMaxInputFreq(const int &value)
{
 Ready=false;
 return true;
}
								/*
/// Имя класса выделенного участка мембраны для генераторной зоны
bool NNeuronFreqGroup::SetLTMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса генераторной зоны
bool NNeuronFreqGroup::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
bool NNeuronFreqGroup::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса источника задающего сигнала для тормозного ионного механизма
bool NNeuronFreqGroup::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число участков мембраны тела нейрона
bool NNeuronFreqGroup::SetNumSomaMembraneParts(const int &value)
{
 OldNumSoma=NumSomaMembraneParts;
 Ready=false;
 return true;
}

/// Число участков мембраны дендритов (исключая участок тела)
bool NNeuronFreqGroup::SetNumDendriteMembraneParts(const int &value)
{
 OldNumDendrited=NumDendriteMembraneParts;
 Ready=false;
 return true;
}                           */
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NNeuronFreqGroup* NNeuronFreqGroup::New(void)
{
 return new NNeuronFreqGroup;
}

UComponent* NNeuronFreqGroup::NewStatic(void)
{
 return new NNeuronFreqGroup;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NNeuronFreqGroup::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{            /*
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
				*/
 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NNeuronFreqGroup::ADelComponent(UEPtr<UContainer> comp)
{                     /*
 if(comp == PosGenerator)
  PosGenerator=0;
 else
 if(comp == NegGenerator)
  NegGenerator=0;

 return NNeuron::ADelComponent(comp); */
 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NNeuronFreqGroup::BuildStructure(int structure_build_mode, const string &aff_neuron_class_name,
										int old_num_aff_neurons, int num_aff_neurons, int min_inp_freq, int max_inp_freq)
{
 UEPtr <NAfferentNeuron> affneuron;
 bool res(true);
 int min_freq, max_freq;

 for(int i=num_aff_neurons;i<old_num_aff_neurons;i++)
 {
  DelComponent(std::string("AffNeuron")+sntoa(i+1));
 }

 for(int i=0;i<num_aff_neurons;i++)
 {
  affneuron=AddMissingComponent<NAfferentNeuron>(std::string("AffNeuron")+sntoa(i+1), aff_neuron_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
  affneuron->SetCoord(MVector<double,3>(8.7,1.67+i*2,0));

  if(StructureBuildMode == 1)
  {
   // Считаем диапазон частот для текущего афферентного нейрона
   double freq_length = double(max_inp_freq - min_inp_freq + 1);
   double freq_step = freq_length / double(num_aff_neurons);
   min_freq = min_inp_freq + int(double(i)*freq_step + 0.5);
   max_freq = min_inp_freq + int(double(i+1)*freq_step - 0.5);

   UEPtr<NReceptor> receptor=affneuron->GetComponentL<NReceptor>("Receptor");
   if(receptor)
   {
	receptor->MinInputRange=min_freq;
	receptor->MaxInputRange=max_freq;
   }
  }
 }


 /*UEPtr<UContainer> membr=0,ltmembr=0;
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

 for(int i=0;i<num_soma_membranes;i++)
 {
  membr=AddMissingComponent<NPulseMembrane>(std::string("Soma")+sntoa(i+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
  membr->SetCoord(MVector<double,3>(12.7+dendrite_length*8,4.67+i*2,0));

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


 UEPtr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(this);
 if(lifeneuron)
 {
  UEPtr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(Storage->TakeObject("NNeuronLife"));
  res&=lifeneuron->AddComponent(nlife);
  res&=n->CreateLink(ltzone->GetLongName(this),"Output",nlife->GetLongId(this),"Input1");
 }
 */
 if(!res)
  return false;

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NNeuronFreqGroup::ADefault(void)
{
 StructureBuildMode=1;
 AffNeuronClassName="NSimpleAfferentNeuron";
 NumAffNeurons = 1;
 MinInputFreq = 0;
 MaxInputFreq = 255;
 //LTMembraneClassName="NPLTZoneNeuronMembrane";
 //LTZoneClassName="NPLTZone";
 //ExcGeneratorClassName="NPNeuronPosCGenerator";
 //InhGeneratorClassName="NPNeuronNegCGenerator";
// NumSomaMembraneParts=1;
 //NumDendriteMembraneParts=0;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NNeuronFreqGroup::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, AffNeuronClassName,
						OldNumAffNeurons, NumAffNeurons, MinInputFreq, MaxInputFreq);
  if(!res)
   return false;
 }

 OldNumAffNeurons=0;
 return true;
}

// Сброс процесса счета.
bool NNeuronFreqGroup::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool NNeuronFreqGroup::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
