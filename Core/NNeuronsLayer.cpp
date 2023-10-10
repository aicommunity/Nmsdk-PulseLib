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

#ifndef NNEURONSLAYER_CPP
#define NNEURONSLAYER_CPP

#include "NNeuronFreqGroupLayer.h"
#include "NNeuronsLayer.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"


// �����, ��������� ���� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuronsLayer::NNeuronsLayer(void)
: StructureBuildMode("StructureBuildMode",this,&NNeuronsLayer::SetStructureBuildMode),
  NeuronsClassName("NeuronsClassName",this,&NNeuronsLayer::SetNeuronsClassName),
  NeuronsHeight("NeuronsHeight",this,&NNeuronsLayer::SetNeuronsHeight),
  NeuronsWidth("NeuronsWidth",this,&NNeuronsLayer::SetNeuronsWidth)
{
 OldNeuronsHeight = OldNeuronsWidth = 0;
}

NNeuronsLayer::~NNeuronsLayer(void)
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
bool NNeuronsLayer::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������ ������� ��������
bool NNeuronsLayer::SetNeuronsClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ����� ����� ����������� �������� �� ��� �
bool NNeuronsLayer::SetNeuronsHeight(const int &value)
{
 Ready=false;
 OldNeuronsHeight = NeuronsHeight;
 return true;
}

/// ����� ����� ����������� �������� �� ��� x
bool NNeuronsLayer::SetNeuronsWidth(const int &value)
{
 Ready=false;
 OldNeuronsWidth = NeuronsWidth;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNeuronsLayer* NNeuronsLayer::New(void)
{
 return new NNeuronsLayer;
}

UComponent* NNeuronsLayer::NewStatic(void)
{
 return new NNeuronsLayer;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NNeuronsLayer::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
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
bool NNeuronsLayer::ADelComponent(UEPtr<UContainer> comp)
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
bool NNeuronsLayer::BuildStructure(int structure_build_mode, const string &neurons_class_name,
									int old_neurons_height, int old_neurons_width,
									int neurons_height, int neurons_width)
{
 UEPtr <NPulseNeuron> neuron;
 bool res(true);
 int ind;

 for(int i = neurons_height*neurons_width; i < old_neurons_height*old_neurons_width; i++)
 {
  DelComponent(std::string("Neuron")+sntoa(i+1));
 }

 for(int j = 0; j < neurons_height; j++)
 {
  for(int i = neurons_width; i < old_neurons_width; i++)
  {
   ind = j*old_neurons_height+i;
   DelComponent(std::string("Neuron")+sntoa(ind+1));
  }
 }

 for(int i=0;i<neurons_height;i++)
 {
	for(int j = 0; j < neurons_width; j++)
	 {
	  ind = i*neurons_width + j;
	  neuron = AddMissingComponent<NPulseNeuron>(std::string("Neuron")+sntoa(ind+1), neurons_class_name);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
	  neuron->SetCoord(MVector<double,3>(8.7+j*7,1.67+i*2,0));
	 }
 }

 if(!res)
  return false;

 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NNeuronsLayer::ADefault(void)
{
 StructureBuildMode=1;
 NeuronsClassName="NSPNeuron";
 NeuronsHeight = 1;
 NeuronsWidth = 1;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NNeuronsLayer::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, NeuronsClassName,
						OldNeuronsHeight, OldNeuronsWidth,
						NeuronsHeight, NeuronsWidth);
  if(!res)
   return false;
 }

 OldNeuronsHeight = OldNeuronsWidth = 0;
 return true;
}

// ����� �������� �����.
bool NNeuronsLayer::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool NNeuronsLayer::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
