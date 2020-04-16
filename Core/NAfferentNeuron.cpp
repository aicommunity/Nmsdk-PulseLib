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

#ifndef NAFFERENT_NEURON_CPP
#define NAFFERENT_NEURON_CPP

#include "NAfferentNeuron.h"
#include "NPulseMembrane.h"
#include "NPulseLTZone.h"
#include "NReceptor.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NAfferentNeuron::NAfferentNeuron(void)
: StructureBuildMode("StructureBuildMode",this,&NAfferentNeuron::SetStructureBuildMode),
  MembraneClassName("MembraneClassName",this,&NAfferentNeuron::SetMembraneClassName),
  LTZoneClassName("LTZoneClassName",this,&NAfferentNeuron::SetLTZoneClassName),
  ReceptorClassName("ReceptorClassName",this,&NAfferentNeuron::SetReceptorClassName),
//  ExcGeneratorClassName("ExcGeneratorClassName",this,&NAfferentNeuron::SetExcGeneratorClassName),
  InhGeneratorClassName("InhGeneratorClassName",this,&NAfferentNeuron::SetInhGeneratorClassName),
  NumSomaMembraneParts("NumSomaMembraneParts",this,&NAfferentNeuron::SetNumSomaMembraneParts),
  MaxReceptorOutput("MaxReceptorOutput",this,&NAfferentNeuron::SetMaxReceptorOutput)
{
}

NAfferentNeuron::~NAfferentNeuron(void)
{
}
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool NAfferentNeuron::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса участка мембраны
bool NAfferentNeuron::SetMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса генераторной зоны
bool NAfferentNeuron::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса рецептора
bool NAfferentNeuron::SetReceptorClassName(const std::string &value)
{
 Ready=false;
 return true;
}
         /*
/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
bool NAfferentNeuron::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}       */

/// Имя класса источника задающего сигнала для тормозного ионного механизма
bool NAfferentNeuron::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число участков мембраны тела нейрона
bool NAfferentNeuron::SetNumSomaMembraneParts(const int &value)
{
 Ready=false;
 return true;
}

/// Максимальная величина входного значения рецептора
bool NAfferentNeuron::SetMaxReceptorOutput(const double &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NAfferentNeuron* NAfferentNeuron::New(void)
{
 return new NAfferentNeuron;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NAfferentNeuron::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseMembrane>(comp) ||
	dynamic_pointer_cast<NLTZone>(comp) ||
	dynamic_pointer_cast<NReceptor>(comp) ||
	dynamic_pointer_cast<NConstGenerator>(comp) ||
	dynamic_pointer_cast<NPulseGenerator>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
/// Осуществляет сборку классической структуры
bool NAfferentNeuron::BuildClassicalStructure(const string &membraneclass, const string &ltzone_class,
					const string &receptorclass, const string &pos_gen_class,
					/*const string &neg_gen_class,*/ int num_membranes)
{
 UEPtr<UContainer> membr;
 UEPtr<NPulseChannel> channel1, channel2;
 UEPtr<NLTZone> ltzone;
 UEPtr<NReceptor> receptor;
 bool res=true;

 if(!Storage)
  return false;

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);
 ltzone->Threshold=0;
 ltzone->SetCoord(MVector<double,3>(20,3,0));

 UEPtr<NConstGenerator> gen_pos/*,gen_neg*/;
 gen_pos=AddMissingComponent<NConstGenerator>("PosGenerator", pos_gen_class);
 gen_pos->SetCoord(MVector<double,3>(12,6.33,1));
 //gen_neg=dynamic_pointer_cast<NConstGenerator>(Storage->TakeObject(neg_gen_class));
 //res&=AddComponent(gen_neg);

 for(int i=0;i<num_membranes;i++)
 {
  membr=AddMissingComponent<NPulseMembrane>("PMembrane", membraneclass);
  membr->SetCoord(MVector<double,3>(12,3,2));

  receptor=AddMissingComponent<NReceptor>("Receptor", receptorclass);
  receptor->ExpCoeff=0.01;
  receptor->Gain=1;
  receptor->MinOutputRange=-1;
  receptor->MaxOutputRange=0;
  res&=AddComponent(receptor);
  membr->SetCoord(MVector<double,3>(4,3,3));

  receptor->DisconnectAll("Output");

  channel1=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("ExcChannel"));
  channel2=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("InhChannel"));

  // Устанавливаем обратную связь
  res&=CreateLink(ltzone->GetLongName(this),"Output",membr->GetLongName(this),"InputFeedbackSignal");

  // Устанавливаем связь мембраны с низкопороговой зоной
  res&=CreateLink(channel1->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");
  res&=CreateLink(channel2->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");

  // Связь между начальными значениями мощностей ионных каналов и каналами
  res&=CreateLink(receptor->GetLongName(this),"Output",channel1->GetLongName(this),"ChannelInputs");
  res&=CreateLink(gen_pos->GetLongName(this),"Output",channel2->GetLongName(this),"ChannelInputs");
 }

 return res;
}

/// Осуществляет сборку упрощенной структуры
bool NAfferentNeuron::BuildSimpleStructure(const string &ltzone_class,
					const string &receptorclass, double max_output)
{
 UEPtr<UContainer> membr;
 UEPtr<NPulseChannel> channel1, channel2;
 UEPtr<NLTZone> ltzone;
 UEPtr<NReceptor> receptor;
 bool res(true);

 if(!Storage)
  return false;

 DelComponent("PMembrane");
 DelComponent("PosGenerator");

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);
 ltzone->Threshold=0;
 ltzone->SetCoord(MVector<double,3>(13,3,0));

 receptor=AddMissingComponent<NReceptor>("Receptor", receptorclass);
 receptor->Gain=1;
 receptor->OutputAdaptationMode=0;
 receptor->InputAdaptationMode=0;
 receptor->MinOutputRange=0;
 receptor->MaxOutputRange=max_output;
 receptor->SetCoord(MVector<double,3>(5,3,0));

 res&=CreateLink(receptor->GetLongName(this),"Output",ltzone->GetLongName(this),"Inputs");

 return res;
}


// Восстановление настроек по умолчанию и сброс процесса счета
bool NAfferentNeuron::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;

 StructureBuildMode=0;
 MembraneClassName="NPMembrane";
 ReceptorClassName="NReceptor";
 LTZoneClassName="NPLTZone";
 //ExcGeneratorClassName="NPNeuronNegCGenerator";
 InhGeneratorClassName="NPNeuronPosCGenerator";
 NumSomaMembraneParts=1;
 MaxReceptorOutput=100;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NAfferentNeuron::ABuild(void)
{
 if(!NPulseNeuronCommon::ABuild())
  return false;

 if(StructureBuildMode == 1)
 {
  bool res=BuildClassicalStructure(MembraneClassName, LTZoneClassName,
							ReceptorClassName, /*ExcGeneratorClassName,*/ InhGeneratorClassName,
							NumSomaMembraneParts);
  if(!res)
   return false;
 }
 else
 if(StructureBuildMode == 2)
 {
  bool res=BuildSimpleStructure(LTZoneClassName,
							ReceptorClassName, MaxReceptorOutput);

  if(!res)
   return false;
 }
 return true;
}

// Сброс процесса счета.
bool NAfferentNeuron::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool NAfferentNeuron::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif
