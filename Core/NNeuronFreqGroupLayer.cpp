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

#ifndef NNEURONFREQGROUPLAYER_CPP
#define NNEURONFREQGROUPLAYER_CPP

#include "NNeuronFreqGroupLayer.h"
#include "NNeuronFreqGroup.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"


// �����, ��������� ���� ����� ����������� ��������, �������������� � ������ ���������� ������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuronFreqGroupLayer::NNeuronFreqGroupLayer(void)
: StructureBuildMode("StructureBuildMode",this,&NNeuronFreqGroupLayer::SetStructureBuildMode),
  AffNeuronGroupClassName("AffNeuronGroupClassName",this,&NNeuronFreqGroupLayer::SetAffNeuronGroupClassName),
  AffNeuronsGroupHeight("AffNeuronsGroupHeight",this,&NNeuronFreqGroupLayer::SetAffNeuronsGroupHeight),
  AffNeuronsGroupWidth("AffNeuronsGroupWidth",this,&NNeuronFreqGroupLayer::SetAffNeuronsGroupWidth),
  NumAffNeuronsInGroup("NumAffNeuronsInGroup",this,&NNeuronFreqGroupLayer::SetNumAffNeuronsInGroup)
{
 OldAffNeuronsGroupHeight = OldAffNeuronsGroupWidth = 0;
}

NNeuronFreqGroupLayer::~NNeuronFreqGroupLayer(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NNeuronFreqGroupLayer::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������ ������� ��������
bool NNeuronFreqGroupLayer::SetAffNeuronGroupClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ����� ����� ����������� �������� �� ��� �
bool NNeuronFreqGroupLayer::SetAffNeuronsGroupHeight(const int &value)
{
 Ready=false;
 OldAffNeuronsGroupHeight = AffNeuronsGroupHeight;
 return true;
}

/// ����� ����� ����������� �������� �� ��� x
bool NNeuronFreqGroupLayer::SetAffNeuronsGroupWidth(const int &value)
{
 Ready=false;
 OldAffNeuronsGroupWidth = AffNeuronsGroupWidth;
 return true;
}

/// ����� ����������� �������� � ������
bool NNeuronFreqGroupLayer::SetNumAffNeuronsInGroup(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNeuronFreqGroupLayer* NNeuronFreqGroupLayer::New(void)
{
 return new NNeuronFreqGroupLayer;
}

UComponent* NNeuronFreqGroupLayer::NewStatic(void)
{
 return new NNeuronFreqGroupLayer;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NNeuronFreqGroupLayer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
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

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NNeuronFreqGroupLayer::ADelComponent(UEPtr<UContainer> comp)
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
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NNeuronFreqGroupLayer::BuildStructure(int structure_build_mode, const string &aff_neuron_group_class_name,
											int old_num_aff_neurons_group_height, int old_num_aff_neurons_group_width,
											int num_aff_neurons_group_height, int num_aff_neurons_group_width,
											int num_aff_neurons_in_group)
{
 UEPtr <NNeuronFreqGroup> affneurongroup;
 bool res(true);
 int ind;

 for(int i = num_aff_neurons_group_height*num_aff_neurons_group_width; i < old_num_aff_neurons_group_height*old_num_aff_neurons_group_width; i++)
 {
  DelComponent(std::string("AffNeuronGroup")+sntoa(i+1));
 }

 for(int j = 0; j < num_aff_neurons_group_height; j++)
 {
  for(int i = num_aff_neurons_group_width; i < old_num_aff_neurons_group_width; i++)
  {
   ind = j*old_num_aff_neurons_group_height+i;
   DelComponent(std::string("AffNeuronGroup")+sntoa(ind+1));
  }
 }

 for(int i=0;i<num_aff_neurons_group_height;i++)
 {
	for(int j = 0; j < num_aff_neurons_group_width; j++)
	 {
	  ind = i*num_aff_neurons_group_width + j;
	  affneurongroup=AddMissingComponent<NNeuronFreqGroup>(std::string("AffNeuronGroup")+sntoa(ind+1), aff_neuron_group_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
	  affneurongroup->SetCoord(MVector<double,3>(8.7+j*7,1.67+i*2,0));
	  affneurongroup->NumAffNeurons = num_aff_neurons_in_group;
      affneurongroup->Build();
	 }
 }

 if(!res)
  return false;

 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NNeuronFreqGroupLayer::ADefault(void)
{
 StructureBuildMode=1;
 AffNeuronGroupClassName="NNeuronFreqGroup";
 AffNeuronsGroupHeight = 1;
 AffNeuronsGroupWidth = 1;
 NumAffNeuronsInGroup = 1;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NNeuronFreqGroupLayer::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, AffNeuronGroupClassName,
						OldAffNeuronsGroupHeight, OldAffNeuronsGroupWidth,
						AffNeuronsGroupHeight, AffNeuronsGroupWidth,NumAffNeuronsInGroup);
  if(!res)
   return false;
 }

 OldAffNeuronsGroupHeight = OldAffNeuronsGroupWidth = 0;
 return true;
}

// ����� �������� �����.
bool NNeuronFreqGroupLayer::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool NNeuronFreqGroupLayer::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
