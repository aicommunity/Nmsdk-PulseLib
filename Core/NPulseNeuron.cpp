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

// ������
// --------------------------
// ������������ � �����������
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
  TrainingSynapsisNum("TrainingSynapsisNum",this,&NPulseNeuron::SetTrainingSynapsisNum)
{
 PosGenerator=0;
 NegGenerator=0;
 // MainOwner should be set after object is fully constructed
 // shared_from_this() may throw bad_weak_ptr if called during construction
 try {
  MainOwner=UComponent::shared_from_this();
 } catch (const std::bad_weak_ptr&) {
  // Object not fully initialized - MainOwner will be set later in Default() or Build()
  // This is OK during construction
 }
 OldNumDendrites=OldNumSoma=0;
 OldStructureBuildMode=0;
}

NPulseNeuron::~NPulseNeuron(void)
{
 Soma.clear();
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ ��������� ������������� ����������
NConstGenerator* NPulseNeuron::GetPosGenerator(void)
{
 return PosGenerator;
}

// ���������� ��������� �� ������ ��������� ���������� ����������
NConstGenerator* NPulseNeuron::GetNegGenerator(void)
{
 return NegGenerator;
}

/// ������ � �������� ��������
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
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NPulseNeuron::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
 {
  OldStructureBuildMode=StructureBuildMode;
  Ready=false;

  if(value == 2)
  {
   ChangeLookupPropertyType("NumDendriteMembraneParts",ptState);
   ChangeLookupPropertyType("NumDendriteMembranePartsVec",ptPubParameter);
   std::vector<int> vecs;
   vecs.assign(NumSomaMembraneParts.v,NumDendriteMembraneParts.v);
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

/// ��� ������ ������� ��������
bool NPulseNeuron::SetMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ����������� ������� �������� ��� ������������ ����
bool NPulseNeuron::SetLTMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ������������ ����
bool NPulseNeuron::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// ��� ������ ��������� ��������� ������� ��� ������������� ������� ���������
bool NPulseNeuron::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ��������� ��������� ������� ��� ���������� ������� ���������
bool NPulseNeuron::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ����� �������� �������� ���� �������
bool NPulseNeuron::SetNumSomaMembraneParts(const int &value)
{
 OldNumSoma=NumSomaMembraneParts;
 OldNumDendritesVec=NumDendriteMembranePartsVec;
 NumDendriteMembranePartsVec->resize(value,0);
 Ready=false;
 return true;
}

/// ����� �������� �������� ��������� (�������� ������� ����)
bool NPulseNeuron::SetNumDendriteMembraneParts(const int &value)
{
 OldNumDendrites=NumDendriteMembraneParts;
 Ready=false;
 return true;
}


/// ����� �������� �������� ��������� (�������� ������� ����)
bool NPulseNeuron::SetNumDendriteMembranePartsVec(const std::vector<int> &value)
{
 OldNumDendritesVec=NumDendriteMembranePartsVec;
 Ready=false;
 return true;
}

/// �������, �������� ������ ������
/// (���� ������ �� ������ Size = 0)
/// ����������� ����������! �� ������ �������� �������������!!!
bool NPulseNeuron::SetTrainingPattern(const MDMatrix<double> &value)
{
 return true;
}

/// ������� ������� �������� �� ���������
/// (���� ������ �� ������ Size = 0)
/// ����������� ����������! �� ������ �������� �������������!!!
bool NPulseNeuron::SetTrainingDendIndexes(const MDMatrix<int> &value)
{
 return true;
}

/// ���������� �������� �� ������� �������� �� ���������
/// (���� ������ �� ������ Size = 0)
/// ����������� ����������! �� ������ �������� �������������!!!
bool NPulseNeuron::SetTrainingSynapsisNum(const MDMatrix<int> &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� �������� ������� ��������, �������� � ���� ����� ������� ��������,
// � ���������� ����� ��������� ������� �� ����� ������
// ���������� ��������� �� ��������� �������
NPulseMembraneCommon* NPulseNeuron::ElongateDendrite(const std::string &name, bool feedback)
{
 if(!Storage)
  return 0;

 std::shared_ptr<UContainer> cont=GetComponent(name);
 if(!cont)
  return 0;


 std::shared_ptr<UContainer> newcont=static_pointer_cast<UContainer>(Storage->TakeObject(cont->GetClass()));
 if(!newcont)
  return 0;


 return 0;
}

// ����������� �������� ������� ��������, �������� � ����� ��� �����������
// ������������� ����� ������� ��������
// ���������� ��������� �� ��������� �������
NPulseMembraneCommon* NPulseNeuron::BranchDendrite(const std::string &name, bool feedback)
{
 if(!Storage)
  return 0;

 std::shared_ptr<NPulseMembrane> dendrite=dynamic_pointer_cast<NPulseMembrane>(GetComponent(name));
 if(!dendrite)
  return 0;

 std::shared_ptr<NPulseMembrane> new_dendrite=dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(dendrite->GetClass()));
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
  res&=CreateLink(GetLTZone()->GetLongName(GetThisAsSharedContainer()),"Output", new_dendrite->GetLongName(GetThisAsSharedContainer()),"InputFeedbackSignal");
 }

 // ������������ �������� � ����������
 int size=dendrite->GetNumComponents();
 if(size>new_dendrite->GetNumComponents())
  size=new_dendrite->GetNumComponents();
 for(int k=0;k<size;k++)
 {
  std::shared_ptr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(dendrite->GetComponentByIndex(k));
  if(!channel)
	continue;
  std::shared_ptr<NPulseChannelCommon> new_channel=dynamic_pointer_cast<NPulseChannelCommon>(new_dendrite->GetComponentL(channel->GetName(),true));
  if(!new_channel)
  	continue;
 /* for(int i=0;i<channel->Output. ;i++)
  {
   NPulseSynapseCommon* synapse=dynamic_cast<NPulseSynapseCommon*>(channel->Inputs.GetItem(i));
   if(!synapse)
	res&=CreateLink(Inputs.GetItem(i)->GetLongName(GetThisAsSharedContainer()),Inputs.GetItemOutputName(i),);
  }                */

  int num_connectors=int(channel->Output.GetNumConnectors());
  for(int i=0;i<num_connectors;i++)
  {
   UConnector* conn=channel->Output.GetConnector(i);
   if(!conn)
	continue;
   std::string conn_input_prop_name=channel->Output.GetConnectorInputName(i);

   res &=CreateLink(new_channel->GetLongName(GetThisAsSharedContainer()),"Output",conn->GetLongName(GetThisAsSharedContainer()),conn_input_prop_name);
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

 // ���������� ��������� ���������� �����������
 //std::shared_ptr<NPulseMembrane> membrane=static_pointer_cast<NPulseMembrane>(cont);
 for(size_t k=0;k<new_dendrite->GetNumNegChannels();k++)
 {
  res&=CreateLink(PosGenerator->GetLongName(GetThisAsSharedContainer()),"Output",new_dendrite->GetNegChannel(k)->GetLongName(GetThisAsSharedContainer()),"Inputs");
 /* item.Id=PosGenerator->GetLongName(GetThisAsSharedContainer());
  item.Index=0;
  conn.Id=membrane->GetNegChannel(k)->GetLongId(this);
  conn.Index=-1;
  res&=CreateLink(item,conn); */
 }

 for(size_t k=0;k<new_dendrite->GetNumPosChannels();k++)
 {
  res&=CreateLink(NegGenerator->GetLongName(GetThisAsSharedContainer()),"Output",new_dendrite->GetPosChannel(k)->GetLongName(GetThisAsSharedContainer()),"Inputs");
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
 return new_dendrite.get();
}

// ������� �������� ������� ��������
// ���� full == true, �� ������� � ��� ������ �������, ������������ � ����
// ����� �������������� ����� �� ������ �� ���� ������
bool NPulseNeuron::EraseDendrite(const std::string &name)
{
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseNeuron::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
bool NPulseNeuron::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 if(!NPulseNeuronCommon::AAddComponent(comp,pointer))
  return false;

 comp->SetMainOwner(UComponent::shared_from_this(),-1);

  std::shared_ptr<NConstGenerator> temp=dynamic_pointer_cast<NConstGenerator>(comp);

  if(temp && temp->Amplitude()>0)
  {
   if(PosGenerator)
	return false;
   PosGenerator=temp.get();
   return true;
  }
  else
  if(temp && temp->Amplitude()<0)
  {
   if(NegGenerator)
	return false;
   NegGenerator=temp.get();
   return true;
  }

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseNeuron::ADelComponent(std::shared_ptr<UContainer> comp)
{
 if(comp.get() == PosGenerator)
  PosGenerator=0;
 else
 if(comp.get() == NegGenerator)
  NegGenerator=0;

 return NPulseNeuronCommon::ADelComponent(comp);
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NPulseNeuron::BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
								  const string &ltzone_class, const string &pos_gen_class,
								  const string &neg_gen_class, int num_soma_membranes, int dendrite_mode,
								  int dendrite_length, const vector<int> &dendrite_length_vec, int num_stimulates, int num_arresting)
{
 std::shared_ptr<NPulseMembraneCommon> membr=0,ltmembr=0;
 std::shared_ptr<NPulseChannelCommon> channel1, channel2, ltchannel1,ltchannel2, channel1temp,channel2temp;
 std::shared_ptr<NLTZone> ltzone;
 bool res(true);
 RDK::ULinkSide item,conn;

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
 if(ltzone)
 {
  ltzone->SetCoord(MVector<double,3>(27.3+dendrite_length*8,4.67,0));
 }

 std::shared_ptr<UNet> gen_pos,gen_neg;
 if(!ExcGeneratorClassName->empty())
 {
  gen_pos=AddMissingComponent<UNet>("PosGenerator", pos_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(pos_gen_class));
  if(gen_pos)
  {
   gen_pos->SetCoord(MVector<double,3>(4,2,0));
   gen_pos->DisconnectAll("Output");
  }
 }
 else
  DelComponent("PosGenerator");

 if(!InhGeneratorClassName->empty())
 {
  gen_neg=AddMissingComponent<UNet>("NegGenerator", neg_gen_class);//dynamic_pointer_cast<UNet>(Storage->TakeObject(neg_gen_class));
  if(gen_neg)
  {
   gen_neg->SetCoord(MVector<double,3>(4,7.3+(num_soma_membranes-1)*2,0));
   gen_neg->DisconnectAll("Output");
  }
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

 // ������, ���� ������ ���������� ����� �������� ������������ ����
 if(!ltzonemembraneclass.empty())
 {
  ltmembr=AddMissingComponent<NPulseMembrane>("LTMembrane", ltzonemembraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(ltzonemembraneclass));
  if(ltmembr)
  {
   ltmembr->SetCoord(MVector<double,3>(20+dendrite_length*8,4.67,0));

   ltchannel1=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("ExcChannel",true));
   ltchannel2=dynamic_pointer_cast<NPulseChannelCommon>(ltmembr->GetComponent("InhChannel",true));
  }
  else
  {
   ltchannel1=nullptr;
   ltchannel2=nullptr;
  }

  // ������������� �������� �����
  if(ltzone && ltmembr)
  {
   res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",ltmembr->GetLongName(GetThisAsSharedContainer()),"InputFeedbackSignal");

   // ������������� ����� �������� � �������������� �����
   if(ltchannel1 && ltzone)
    res&=CreateLink(ltchannel1->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");
   if(ltchannel2 && ltzone)
    res&=CreateLink(ltchannel2->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");
  }
 }
 else
  DelComponent("LTMembrane");

 Soma.resize(num_soma_membranes);
 for(int i=0;i<num_soma_membranes;i++)
 {
  membr=AddMissingComponent<NPulseMembrane>(std::string("Soma")+sntoa(i+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
  if(!membr)
  {
   Soma[i]=nullptr;
   channel1=nullptr;
   channel2=nullptr;
   continue; // Skip this iteration if membrane creation failed
  }
  membr->SetCoord(MVector<double,3>(12.7+dendrite_length*8,4.67+i*2,0));
  Soma[i]=membr;

  channel1=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
  channel2=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

  // ������, ���� ������ ���������� ����� �������� ������������ ����
  // ����� ���������� ���� � ���
  if(!ltzonemembraneclass.empty())
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(GetThisAsSharedContainer()),"Output",ltchannel1->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
   if(channel2)
	res&=CreateLink(channel2->GetLongName(GetThisAsSharedContainer()),"Output",ltchannel2->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
  }
  else // ����� ���������� ���� �������� � �������������� ����
  {
   if(channel1)
	res&=CreateLink(channel1->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");
   if(channel2)
        res&=CreateLink(channel2->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");

   // ������������� �������� �����
   res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",membr->GetLongName(GetThisAsSharedContainer()),"InputFeedbackSignal");
  }

  int current_dendrite_length(0);
  if(dendrite_mode == 1)
   current_dendrite_length=dendrite_length;
  else
   current_dendrite_length=dendrite_length_vec[i];


  for(int j=0;j<current_dendrite_length;j++)
  {
   membr=AddMissingComponent<NPulseMembrane>(std::string("Dendrite")+sntoa(i+1)+std::string("_")+sntoa(j+1), membraneclass);//dynamic_pointer_cast<NPulseMembrane>(Storage->TakeObject(membraneclass));
   membr->SetCoord(MVector<double,3>(12.7+(dendrite_length-j-1)*8,4.67+i*2,0));

   channel1temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("ExcChannel",true));
   channel2temp=dynamic_pointer_cast<NPulseChannelCommon>(membr->GetComponent("InhChannel",true));

   if(channel1temp)
        res&=CreateLink(channel1temp->GetLongName(GetThisAsSharedContainer()),"Output",channel1->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
   if(channel2temp)
    res&=CreateLink(channel2temp->GetLongName(GetThisAsSharedContainer()),"Output",channel2->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");

   channel1 = channel1temp;
   channel2 = channel2temp;
  }

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  if(channel1 && gen_neg)
   res&=CreateLink(gen_neg->GetLongName(GetThisAsSharedContainer()),"Output",channel1->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
  if(channel2 && gen_pos)
  res&=CreateLink(gen_pos->GetLongName(GetThisAsSharedContainer()),"Output",channel2->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
 }

  /*
 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(this);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(Storage->TakeObject("NNeuronLife"));
  res&=lifeneuron->AddComponent(nlife);
  res&=n->CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",nlife->GetLongId(this),"Input1");
 }  */

 if(!res)
  return false;

 return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

 return true;
}

// ����� �������� �����.
bool NPulseNeuron::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseNeuron::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 // ������� ��������� �������� ��������� ���� ��������� � �������� ����
 DendriticSumPotential(0,0)=0;
 SomaSumPotential(0,0)=0;

 int channel_divisor=1;
 std::shared_ptr<NPulseLTZoneCommon> zone = dynamic_pointer_cast<NPulseLTZoneCommon>(LTZone);
 if(zone && zone->NumChannelsInGroup.v > 0)
  channel_divisor = zone->NumChannelsInGroup.v;

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


// ������������� ���������� � ��������� ������� �������
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
