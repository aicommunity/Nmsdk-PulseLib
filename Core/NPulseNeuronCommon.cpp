// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_NEURON_COMMON_CPP
#define NPULSE_NEURON_COMMON_CPP

#include "NPulseNeuronCommon.h"
#include "NPulseMembrane.h"
#include "NPulseLTZone.h"
#include "NPulseSynapse.h"
#include "NPulseChannel.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuronCommon::NPulseNeuronCommon(void)
//: NNeuron(name),
 : UseAverageDendritesPotential("UseAverageDendritesPotential",this,&NPulseNeuronCommon::SetUseAverageDendritesPotential),
   UseAverageLTZonePotential("UseAverageLTZonePotential",this,&NPulseNeuronCommon::SetUseAverageLTZonePotential),
   ActiveOutputs("ActiveOutputs",this),
   ActivePosInputs("ActivePosInputs",this),
   ActiveNegInputs("ActiveNegInputs",this),
  NumActiveOutputs("NumActiveOutputs",this),
  NumActivePosInputs("NumActivePosInputs",this),
  NumActiveNegInputs("NumActiveNegInputs",this),

  LTZone("LTZone",this)
{
 MainOwner=this;
}

NPulseNeuronCommon::~NPulseNeuronCommon(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ������� ������� ���������� � �������� ������ �������
bool NPulseNeuronCommon::SetUseAverageDendritesPotential(const bool &value)
{
 for(size_t i=0;i<Membranes.size();i++)
  if(Membranes[i])
   Membranes[i]->UseAveragePotential=value;
 return true;
}

/// ������� ������� ���������� � �������� ������ �������
bool NPulseNeuronCommon::SetUseAverageLTZonePotential(const bool &value)
{
 if(LTZone)
  LTZone->UseAveragePotential=value;
 return true;
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ �������������� ����
NLTZone* NPulseNeuronCommon::GetLTZone(void)
{
 return dynamic_pointer_cast<NLTZone>(LTZone.Get());// &(*LTZone);
}

// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
/*
// ��������� �������� ������� ��������, �������� � ���� ����� ������� ��������,
// � ���������� ����� ��������� ������� �� ����� ������
// ���������� ��������� �� ��������� �������
NPulseMembrane* NPulseNeuronCommon::ElongateDendrite(const UId &id, bool feedback)
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

// ����������� �������� ������� ��������, �������� � ����� ��� �����������
// ������������� ����� ������� ��������
// ���������� ��������� �� ��������� �������
NPulseMembrane* NPulseNeuronCommon::BranchDendrite(const UId &id, bool feedback)
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

 // ������������� �������� �����
 if(feedback)
 {
  item.Id=GetLTZone()->GetLongId(this);
  item.Index=0;
  conn.Id=cont->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn);
 }

 // ������������ �������� � �����������
 int size=dendrite->GetNumComponents();
 if(size>cont->GetNumComponents())
  size=cont->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  UEPtr<UADItem> channel=static_pointer_cast<UADItem>(dendrite->GetComponentByIndex(k));
  for(int i=0;i<channel->GetNumOutputs();i++)
   for(int j=0;j<channel->GetNumAConnectors(i);j++)
   {
//	item.Id=cont->GetComponentByIndex(k)->GetLongId(this);
	item.Id=cont->GetComponent(channel->GetName())->GetLongId(this);
	item.Index=0;
	conn.Id=channel->GetAConnectorByIndex(int(i),j)->GetLongId(this);
	conn.Index=-1;
	res&=CreateLink(item,conn);
   }
 }

 // ���������� ��������� ���������� �����������
 UEPtr<NPulseMembrane> membrane=static_pointer_cast<NPulseMembrane>(cont);
 for(size_t k=0;k<membrane->GetNumNegChannels();k++)
 {
  item.Id=PosGenerator->GetLongId(this);
  item.Index=0;
  conn.Id=membrane->GetNegChannel(k)->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn);
 }

 for(size_t k=0;k<membrane->GetNumPosChannels();k++)
 {
  item.Id=NegGenerator->GetLongId(this);
  item.Index=0;
  conn.Id=membrane->GetPosChannel(k)->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn);
 }


 if(!res)
 {
  cont->Free();
//  Storage->ReturnObject(cont);
  return 0;
 }
 return membrane;
}

// ������� �������� ������� ��������
// ���� full == true, �� ������� � ��� ������ �������, ������������ � ����
// ����� �������������� ����� �� ������ �� ���� ������
bool NPulseNeuronCommon::EraseDendrite(const UId &id)
{
 return true;
}                                     */
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseNeuronCommon* NPulseNeuronCommon::New(void)
{
 return new NPulseNeuronCommon;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseNeuronCommon::CheckComponentType(UEPtr<UContainer> comp) const
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPulseNeuronCommon::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NNeuron::AAddComponent(comp,pointer))
  return false;

 comp->SetMainOwner(this,-1);

 {
  UEPtr<NPulseMembraneCommon> membrane=dynamic_pointer_cast<NPulseMembraneCommon>(comp);
  if(membrane)
  {
   membrane->UseAveragePotential=UseAverageDendritesPotential;
   vector<NPulseMembraneCommon*>::iterator I;
   bool exists = false;
   for(I=Membranes.begin();I!=Membranes.end();I++)
   {
	if(comp==*I)
	{
	 exists=true;
	 break;
	}
   }
   if(!exists)
	Membranes.push_back(membrane);
   // ���������� ������� ����� ���� ��� ����
   for(int i=0;i<membrane->GetNumComponents();i++)
   {
	UEPtr<NPulseChannel> channel(dynamic_pointer_cast<NPulseChannel>(membrane->GetComponentByIndex(i)));
	if(channel)
	 channel->InstallHebbSynapses();
   }
  }
 }

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseNeuronCommon::ADelComponent(UEPtr<UContainer> comp)
{
  vector<NPulseMembraneCommon*>::iterator I;
  for(I=Membranes.begin();I!=Membranes.end();I++)
  {
   if(comp==*I)
   {
	Membranes.erase(I);
	break;
   }
  }
 return NNeuron::ADelComponent(comp);
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseNeuronCommon::ADefault(void)
{
 UseAverageLTZonePotential=true;
 UseAverageDendritesPotential=true;

 ActiveOutputs.Assign(1,1,0.0);
 ActivePosInputs.Assign(1,1,0.0);
 ActiveNegInputs.Assign(1,1,0.0);
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseNeuronCommon::ABuild(void)
{
 if(LTZone)
  LTZone->UseAveragePotential=UseAverageLTZonePotential;

 for(size_t i=0;i<Membranes.size();i++)
  if(Membranes[i])
   Membranes[i]->UseAveragePotential=UseAverageDendritesPotential;
 return true;
}

// ����� �������� �����.
bool NPulseNeuronCommon::AReset(void)
{
 // ����� ������ �������������� ���� �������� �� ������ (� ����)
 NumActiveOutputs=0;

 // ����� ������ �������������� ������� ��������� �� ����
 NumActivePosInputs=0;
 NumActiveNegInputs=0;

 return true;
}

// ��������� ������ ����� �������
bool NPulseNeuronCommon::ACalculate(void)
{
 // ����� ������ �������������� ���� �������� �� ������ (� ����)
 ActiveOutputs(0,0)=NumActiveOutputs.v;
 NumActiveOutputs.v=0;

 // ����� ������ �������������� ������� ��������� �� ����
 ActivePosInputs(0,0)=NumActivePosInputs.v;
 NumActivePosInputs.v=0;
 ActiveNegInputs(0,0)=NumActiveNegInputs.v;
 NumActiveNegInputs.v=0;

 return true;
}
// --------------------------

}
#endif
