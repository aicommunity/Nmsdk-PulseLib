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

#ifndef NPULSEPERSEPTRON_CPP
#define NNEURONFREQGROUP_CPP

#include "NPulsePerseptron.h"
#include "NNeuronFreqGroupLayer.h"
#include "NNeuronsLayer.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"


// Класс, формирующий импульсный персептрон
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulsePerseptron::NPulsePerseptron(void)
: StructureBuildMode("StructureBuildMode",this,&NPulsePerseptron::SetStructureBuildMode),
  NeuronFreqGroupLayerClassName("NeuronFreqGroupLayerClassName",this,&NPulsePerseptron::SetNeuronFreqGroupLayerClassName),
  NumInputFeatures("NumInputFeatures", this, &NPulsePerseptron::SetNumInputFeatures),
  NeuronsLayerClassName("NeuronsLayerClassName",this,&NPulsePerseptron::SetNeuronsLayerClassName),
  NumNeuronsLayer("NumNeuronsLayer",this,&NPulsePerseptron::SetNumNeuronsLayer),
  AffNeuronsLayerSize("AffNeuronsLayerSize",this,&NPulsePerseptron::SetAffNeuronsLayerSize),
  NeuronsLayerSize("NeuronsLayerSize",this,&NPulsePerseptron::SetNeuronsLayerSize),
  NumAffNeuronsInGroup("NumAffNeuronsInGroup",this,&NPulsePerseptron::SetNumAffNeuronsInGroup),
  FlagFreqGroupLayer("FlagFreqGroupLayer",this)
{
 OldNumNeuronsLayer=0;
}

NPulsePerseptron::~NPulsePerseptron(void)
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
bool NPulsePerseptron::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего слой групп афферентных нейронов, чувствительных к разным диапазонам частот
bool NPulsePerseptron::SetNeuronFreqGroupLayerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число признаков на входе перспептрона (размер вектора признаков)
bool NPulsePerseptron::SetNumInputFeatures(const int &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего слой нейронов
bool NPulsePerseptron::SetNeuronsLayerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Число слоёв нейронов
bool NPulsePerseptron::SetNumNeuronsLayer(const int &value)
{
 Ready=false;
 OldNumNeuronsLayer = NumNeuronsLayer;
 NeuronsLayerSize.Resize(value,2,1);
 return true;
}

/// Размер слоя афферентных нейронов.
bool NPulsePerseptron::SetAffNeuronsLayerSize(const MDMatrix<int> &value)
{
 Ready=false;
 return true;
}

/// Размер слоя нейронов.
bool NPulsePerseptron::SetNeuronsLayerSize(const MDMatrix<int> &value)
{
 Ready=false;
 return true;
}

/// Число афферентных нейронов в группе
bool NPulsePerseptron::SetNumAffNeuronsInGroup(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulsePerseptron* NPulsePerseptron::New(void)
{
 return new NPulsePerseptron;
}

UComponent* NPulsePerseptron::NewStatic(void)
{
 return new NPulsePerseptron;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPulsePerseptron::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
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
bool NPulsePerseptron::ADelComponent(UEPtr<UContainer> comp)
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
bool NPulsePerseptron::BuildStructure(int structure_build_mode, const string &neuron_freq_group_layer_class_name,
										const string &neurons_layer_class_name,
										int old_num_neurons_layer, int num_neurons_layer,
										const MDMatrix<int> &aff_neurons_layer_size, const MDMatrix<int> &neurons_layer_size,
										int num_aff_neurons_in_group)
{
 UEPtr <NNeuronFreqGroupLayer> freq_group_layer;
 UEPtr <NNeuronsLayer> neurons_layer, neurons_layer2;
 bool res(true);

 // 1 - автоматическая сборка производится, 1 нейрон -> 1 дендрит, связь - каждый с каждым
 if(StructureBuildMode == 1)
 {
  for(int i=num_neurons_layer;i<old_num_neurons_layer;i++)
  {
   DelComponent(std::string("NeuronsLayer")+sntoa(i+1));
  }
  if(FlagFreqGroupLayer)
  {
    freq_group_layer=AddMissingComponent<NNeuronFreqGroupLayer>(std::string("NeuronFreqGroupLayer"), neuron_freq_group_layer_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
    freq_group_layer->SetCoord(MVector<double,3>(8.7,1.67,0));
    freq_group_layer->AffNeuronsGroupWidth = aff_neurons_layer_size(0,0);
    freq_group_layer->AffNeuronsGroupHeight = aff_neurons_layer_size(0,1);
    freq_group_layer->NumAffNeuronsInGroup = num_aff_neurons_in_group;
    freq_group_layer->Build();
  }

  for(int i=0;i<num_neurons_layer;i++)
  {
   neurons_layer=AddMissingComponent<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+1), neurons_layer_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   neurons_layer->SetCoord(MVector<double,3>(8.7,1.67+(i+1)*2,0));
   neurons_layer->NeuronsWidth = neurons_layer_size(i,0);
   neurons_layer->NeuronsHeight = neurons_layer_size(i,1);
   neurons_layer->Build();
  }

  // связь - каждый с каждым
  int affsize = aff_neurons_layer_size(0,0)*aff_neurons_layer_size(0,1);
  int neusize = neurons_layer_size(0,0)*neurons_layer_size(0,1);
  // афферентные нейроны - импульсные нейроны
  if(FlagFreqGroupLayer)
  {
    for(int i = 0; i < affsize; i++) // выключать по флагу
    {
     for(int j = 0; j < num_aff_neurons_in_group; j++)
     {
      UEPtr<NPulseNeuronCommon> n_out=freq_group_layer->GetComponentL<NPulseNeuronCommon>(std::string("AffNeuronGroup")+sntoa(i+1)+std::string(".AffNeuron")+sntoa(j+1),true);

      for(int n = 0; n < neusize; n++)
      {
       neurons_layer = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(1),true);
       UEPtr<NPulseNeuron> n_in=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(n+1),true);

       if(!n_out)
        return true;
       if(!n_in)
        return true;

       n_in->NumDendriteMembraneParts=1;
       n_in->Build();

       UEPtr<NLTZone> ltzone=n_out->GetComponentL<NLTZone>("LTZone");//GetLTZone();
       if(!ltzone)
        return true;

       UEPtr<NPulseSynapse> synapse=n_in->GetComponentL<NPulseSynapse>("Dendrite1_1.ExcSynapse1",true);
       if(!synapse)
        return true;

       res&=CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");

       if(!res)
        return false;
      }
     }
    }
  }

  // импульсные нейроны - импульсные нейроны
  for(int i = 0; i < NumNeuronsLayer-1; i++)
  {
   neurons_layer = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+1),true);
   neurons_layer2 = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+2),true);
   neusize = neurons_layer->NeuronsHeight*neurons_layer->NeuronsWidth;
   if(i == 0)
   {
    for(int j = 0; j < neusize; j++)
    {
     UEPtr<NPulseNeuron> n_in=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(j+1),true);

     if(!n_in)
      return true;

     UEPtr<NPulseMembrane> membr=n_in->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!membr)
      return true;

     membr->NumExcitatorySynapses=NumInputFeatures;
     membr->Build();
    }
   }

   for(int j = 0; j < neusize; j++)
   {
	UEPtr<NPulseNeuron> n_out=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(j+1),true);

    int neusize2 = neurons_layer2->NeuronsHeight*neurons_layer2->NeuronsWidth;
	for(int n = 0; n < neusize2; n++)
	{
	 UEPtr<NPulseNeuron> n_in=neurons_layer2->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(n+1),true);

	 if(!n_out)
	  return true;
	 if(!n_in)
	  return true;

//	 n_in->NumDendriteMembraneParts=1;
	 n_in->Build();

	 UEPtr<NLTZone> ltzone=n_out->GetComponentL<NLTZone>("LTZone");//GetLTZone();
	 if(!ltzone)
	  return true;

     UEPtr<NPulseMembrane> membr=n_in->GetComponentL<NPulseMembrane>("Soma1",true);
     if(!membr)
      return true;

     membr->NumExcitatorySynapses=neusize;
     membr->Build();
     membr->RebuildSynapticInternalLinks();

     UEPtr<NPulseSynapse> synapse=membr->GetComponentL<NPulseSynapse>("ExcSynapse"+sntoa(j+1),true);
	 if(!synapse)
	  return true;

	 res&=CreateLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");

     if(!res)
	  return false;
	}
   }
  }
 }

 // 2 - автоматическая сборка без связей
 if(StructureBuildMode == 2)
 {
  for(int i=num_neurons_layer;i<old_num_neurons_layer;i++)
  {
   DelComponent(std::string("NeuronsLayer")+sntoa(i+1));
  }
  if(FlagFreqGroupLayer)
  {
    freq_group_layer=AddMissingComponent<NNeuronFreqGroupLayer>(std::string("NeuronFreqGroupLayer"), neuron_freq_group_layer_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
    freq_group_layer->SetCoord(MVector<double,3>(8.7,1.67,0));
    freq_group_layer->AffNeuronsGroupWidth = aff_neurons_layer_size(0,0);
    freq_group_layer->AffNeuronsGroupHeight = aff_neurons_layer_size(0,1);
    freq_group_layer->NumAffNeuronsInGroup = num_aff_neurons_in_group;
    freq_group_layer->Build();
  }
  for(int i=0;i<num_neurons_layer;i++)
  {
   neurons_layer=AddMissingComponent<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+1), neurons_layer_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   neurons_layer->SetCoord(MVector<double,3>(8.7,1.67+(i+1)*2,0));
   neurons_layer->NeuronsWidth = neurons_layer_size(i,0);
   neurons_layer->NeuronsHeight = neurons_layer_size(i,1);
   neurons_layer->Build();
  }

  // связь - каждый с каждым
  int affsize = aff_neurons_layer_size(0,0)*aff_neurons_layer_size(0,1);
  int neusize = neurons_layer_size(0,0)*neurons_layer_size(0,1);
  // афферентные нейроны - импульсные нейроны
  if(FlagFreqGroupLayer)
  {
    for(int i = 0; i < affsize; i++)
    {
     for(int j = 0; j < num_aff_neurons_in_group; j++)
     {
      UEPtr<NPulseNeuronCommon> n_out=freq_group_layer->GetComponentL<NPulseNeuronCommon>(std::string("AffNeuronGroup")+sntoa(i+1)+std::string(".AffNeuron")+sntoa(j+1),true);

      for(int n = 0; n < neusize; n++)
      {
       neurons_layer = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(1),true);
       UEPtr<NPulseNeuron> n_in=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(n+1),true);

       if(!n_out)
        return true;
       if(!n_in)
        return true;

       n_in->NumDendriteMembraneParts=1;
       n_in->Build();

       UEPtr<NLTZone> ltzone=n_out->GetComponentL<NLTZone>("LTZone");//GetLTZone();
       if(!ltzone)
        return true;

       UEPtr<NPulseSynapse> synapse=n_in->GetComponentL<NPulseSynapse>("Dendrite1_1.ExcSynapse1",true);
       if(!synapse)
        return true;

       res&=BreakLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");

       if(!res)
        return false;
      }
     }
    }
  }

  // импульсные нейроны - импульсные нейроны
  for(int i = 0; i < NumNeuronsLayer-1; i++)
  {
   neurons_layer = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+1),true);
   neurons_layer2 = GetComponentL<NNeuronsLayer>(std::string("NeuronsLayer")+sntoa(i+2),true);

   neusize = neurons_layer->NeuronsHeight*neurons_layer->NeuronsWidth;
   if(i == 0)
   {
       for(int j = 0; j < neusize; j++)
       {
        UEPtr<NPulseNeuron> n_in=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(j+1),true);

        if(!n_in)
         return true;

        UEPtr<NPulseMembrane> membr=n_in->GetComponentL<NPulseMembrane>("Soma1",true);
        if(!membr)
         return true;

        membr->NumExcitatorySynapses=NumInputFeatures;
        membr->RebuildSynapticInternalLinks();
        membr->Build();
       }
   }

   for(int j = 0; j < neusize; j++)
   {
	UEPtr<NPulseNeuron> n_out=neurons_layer->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(j+1),true);

    int neusize2 = neurons_layer2->NeuronsHeight*neurons_layer2->NeuronsWidth;
	for(int n = 0; n < neusize2; n++)
	{
	 UEPtr<NPulseNeuron> n_in=neurons_layer2->GetComponentL<NPulseNeuron>(std::string("Neuron")+sntoa(n+1),true);

	 if(!n_out)
	  return true;
	 if(!n_in)
	  return true;

	 n_in->NumDendriteMembraneParts=1;
	 n_in->Build();

	 UEPtr<NLTZone> ltzone=n_out->GetComponentL<NLTZone>("LTZone");//GetLTZone();
	 if(!ltzone)
	  return true;

	 UEPtr<NPulseSynapse> synapse=n_in->GetComponentL<NPulseSynapse>("Dendrite1_1.ExcSynapse1",true);
	 if(!synapse)
	  return true;

	 res&=BreakLink(ltzone->GetLongName(this),"Output",synapse->GetLongName(this),"Input");

     if(!res)
	  return false;
	}
   }
  }
 }

 if(!res)
  return false;

 return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulsePerseptron::ADefault(void)
{
 StructureBuildMode=1;
 NeuronFreqGroupLayerClassName="NNeuronFreqGroupLayer";
 NeuronsLayerClassName="NNeuronsLayer";
 NumNeuronsLayer = 1;
 NumInputFeatures = 2;
 AffNeuronsLayerSize.Resize(1,2,1);
 NeuronsLayerSize.Resize(1,2,1);
 NumAffNeuronsInGroup = 1;
 FlagFreqGroupLayer = false;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulsePerseptron::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, NeuronFreqGroupLayerClassName,
						  NeuronsLayerClassName, OldNumNeuronsLayer, NumNeuronsLayer,
						  AffNeuronsLayerSize, NeuronsLayerSize, NumAffNeuronsInGroup);
  if(!res)
   return false;
 }

 OldNumNeuronsLayer=0;
 return true;
}

// Сброс процесса счета.
bool NPulsePerseptron::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool NPulsePerseptron::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
