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
#include "NConstGenerator.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuronCommon::NPulseNeuronCommon(void)
//: NNeuron(name),
 : UseAverageDendritesPotential("UseAverageDendritesPotential",this,&NPulseNeuronCommon::SetUseAverageDendritesPotential),
   UseAverageLTZonePotential("UseAverageLTZonePotential",this,&NPulseNeuronCommon::SetUseAverageLTZonePotential),
   Output("Output", this),
   ActiveOutputs("ActiveOutputs",this),
   ActivePosInputs("ActivePosInputs",this),
   ActiveNegInputs("ActiveNegInputs",this),
   DendriticSumPotential("DendriticSumPotential",this),
   SomaSumPotential("SomaSumPotential",this),
   NumActiveOutputs("NumActiveOutputs",this),
   NumActivePosInputs("NumActivePosInputs",this),
   NumActiveNegInputs("NumActiveNegInputs",this)
 // LTZone("LTZone",this)
{
 MainOwner=std::shared_ptr<UComponent>(this);
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
 return dynamic_pointer_cast<NLTZone>(LTZone);// &(*LTZone);
}

// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
  /*
// ��������� �������� ������� ��������, �������� � ���� ����� ������� ��������,
// � ���������� ����� ��������� ������� �� ����� ������
// ���������� ��������� �� ��������� �������
NPulseMembraneCommon* NPulseNeuronCommon::ElongateDendrite(const std::string &name, bool feedback)
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

// ����������� �������� ������� ��������, �������� � ����� ��� �����������
// ������������� ����� ������� ��������
// ���������� ��������� �� ��������� �������
NPulseMembraneCommon* NPulseNeuronCommon::BranchDendrite(const std::string &name, bool feedback)
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

 // ������������� �������� �����
 if(feedback)
 {
  res&=CreateLink(GetLTZone()->GetLongName(this),"Output", new_dendrite->GetLongName(this),"InputFeedbackSignal");
 }

 // ������������ �������� � ����������
 int size=dendrite->GetNumComponents();
 if(size>new_dendrite->GetNumComponents())
  size=new_dendrite->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(dendrite->GetComponentByIndex(k));
  UEPtr<NPulseChannelCommon> new_channel=dynamic_pointer_cast<NPulseChannelCommon>(new_dendrite->GetComponentL(channel->GetName()));
  if(!channel || !new_channel)
   continue;

  size_t num_connectors=channel->Output.GetNumConnectors();
  for(size_t i=0;i<num_connectors;i++)
  {
   UConnector* conn=channel->Output.GetConnector(i);
   if(!conn)
	continue;
   std::string conn_input_prop_name=channel->Output.GetConnectorInputName(i);

   res &=CreateLink(new_channel->GetLongName(this),"Output",conn->GetLongName(this),conn_input_prop_name);
  }

 }

 // ���������� ��������� ���������� �����������
 //UEPtr<NPulseMembrane> membrane=static_pointer_cast<NPulseMembrane>(cont);
 for(size_t k=0;k<new_dendrite->GetNumNegChannels();k++)
 {
  res&=CreateLink(PosGenerator->GetLongName(this),"Output",membrane->GetNegChannel(k)->GetLongName(this),"Inputs");
 }

 for(size_t k=0;k<new_dendrite->GetNumPosChannels();k++)
 {
  res&=CreateLink(NegGenerator->GetLongName(this),"Output",membrane->GetPosChannel(k)->GetLongName(this),"Inputs");

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
bool NPulseNeuronCommon::EraseDendrite(const std::string &name)
{
 return true;
}
*/
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

 comp->SetMainOwner(std::shared_ptr<UComponent>(this),-1);

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
   membrane->RebuildSynapticInternalLinks();
   // ���������� ������� ����� ���� ��� ����
 /*  for(int i=0;i<membrane->GetNumComponents();i++)
   {
	UEPtr<NPulseChannel> channel(dynamic_pointer_cast<NPulseChannel>(membrane->GetComponentByIndex(i)));
	if(channel)
	 channel->InstallHebbSynapses();
   } */
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

 DendriticSumPotential.Assign(1,1,0.0);
 SomaSumPotential.Assign(1,1,0.0);

 Output.Assign(1,1,0.0);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseNeuronCommon::ABuild(void)
{
 vector<NameT> buffer;
 GetComponentsNameByClassType<NLTZone>(buffer, this);
 if(buffer.empty())
  LTZone=0;
 else
 {
  LTZone=GetComponentL<NLTZone>(buffer[0],true);
 }

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

 DendriticSumPotential.ToZero();
 SomaSumPotential.ToZero();
 Output.ToZero();

 return true;
}

// ��������� ������ ����� �������
bool NPulseNeuronCommon::ACalculate(void)
{
 // ������� ������ � LTZone �� ����� ���������� �������
 if (LTZone)
  Output = LTZone->Output;

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
