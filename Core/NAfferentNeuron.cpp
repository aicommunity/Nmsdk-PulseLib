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
#include "NConstGenerator.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NAfferentNeuron::NAfferentNeuron(void)
: StructureBuildMode("StructureBuildMode",this,&NAfferentNeuron::SetStructureBuildMode),
  MembraneClassName("MembraneClassName",this,&NAfferentNeuron::SetMembraneClassName),
  LTZoneClassName("LTZoneClassName",this,&NAfferentNeuron::SetLTZoneClassName),
  ReceptorClassName("ReceptorClassName",this,&NAfferentNeuron::SetReceptorClassName),
  ExcGeneratorClassName("ExcGeneratorClassName",this,&NAfferentNeuron::SetExcGeneratorClassName),
//  InhGeneratorClassName("InhGeneratorClassName",this,&NAfferentNeuron::SetInhGeneratorClassName),
  NumSomaMembraneParts("NumSomaMembraneParts",this,&NAfferentNeuron::SetNumSomaMembraneParts),
  MaxReceptorOutput("MaxReceptorOutput",this,&NAfferentNeuron::SetMaxReceptorOutput)
{
}

NAfferentNeuron::~NAfferentNeuron(void)
{
}
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NAfferentNeuron::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������ ������� ��������
bool NAfferentNeuron::SetMembraneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ������������ ����
bool NAfferentNeuron::SetLTZoneClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ���������
bool NAfferentNeuron::SetReceptorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������ ��������� ��������� ������� ��� ������������� ������� ���������
bool NAfferentNeuron::SetExcGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}
		/*
/// ��� ������ ��������� ��������� ������� ��� ���������� ������� ���������
bool NAfferentNeuron::SetInhGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}        */

/// ����� �������� �������� ���� �������
bool NAfferentNeuron::SetNumSomaMembraneParts(const int &value)
{
 Ready=false;
 return true;
}

/// ������������ �������� �������� �������� ���������
bool NAfferentNeuron::SetMaxReceptorOutput(const double &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NAfferentNeuron* NAfferentNeuron::New(void)
{
 return new NAfferentNeuron;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NAfferentNeuron::CheckComponentType(std::shared_ptr<UContainer> comp) const
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
// ������� ������ ���������� ������
// --------------------------
/// ������������ ������ ������������ ���������
bool NAfferentNeuron::BuildClassicalStructure(const string &membraneclass, const string &ltzone_class,
					const string &receptorclass, const string &neg_gen_class,
					/*const string &neg_gen_class,*/ int num_membranes)
{
 std::shared_ptr<UContainer> membr;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NLTZone> ltzone;
 std::shared_ptr<NReceptor> receptor;
 bool res=true;

 auto storage = Storage;
if(!storage)
  return false;

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);
 if(ltzone)
 {
  ltzone->Threshold=0;
  ltzone->SetCoord(MVector<double,3>(20,3,0));
 }

 std::shared_ptr<NConstGenerator> gen_neg;
 gen_neg=AddMissingComponent<NConstGenerator>("NegGenerator", neg_gen_class);
 if(gen_neg)
 {
  gen_neg->SetCoord(MVector<double,3>(12,6.33,0));
 }
 //gen_neg=dynamic_pointer_cast<NConstGenerator>(Storage->TakeObject(neg_gen_class));
 //res&=AddComponent(gen_neg);

 for(int i=0;i<num_membranes;i++)
 {
  membr=AddMissingComponent<NPulseMembrane>("PMembrane", membraneclass);
  membr->SetCoord(MVector<double,3>(12,3,0));

  receptor=AddMissingComponent<NReceptor>("Receptor", receptorclass);
  receptor->ExpCoeff=0.1;
  receptor->Gain=1;
  receptor->MinOutputRange=0;
  receptor->MaxOutputRange=1;
  receptor->MaxInputRange=1;
  receptor->OutputAdaptationMode=5;
  receptor->InputAdaptationMode=0;
  receptor->SumCoeff=1;
  AddComponent(receptor);

  receptor->SetCoord(MVector<double,3>(4,3,0));
  receptor->DisconnectAll("Output");

  channel1=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("ExcChannel"));
  channel2=dynamic_pointer_cast<NPulseChannel>(membr->GetComponent("InhChannel"));

  // ������������� �������� �����
  res&=CreateLink(ltzone->GetLongName(GetThisAsSharedContainer()),"Output",membr->GetLongName(GetThisAsSharedContainer()),"InputFeedbackSignal");

  // ������������� ����� �������� � �������������� �����
  res&=CreateLink(channel1->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");
  res&=CreateLink(channel2->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  res&=CreateLink(receptor->GetLongName(GetThisAsSharedContainer()),"Output",channel2->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
  res&=CreateLink(gen_neg->GetLongName(GetThisAsSharedContainer()),"Output",channel1->GetLongName(GetThisAsSharedContainer()),"ChannelInputs");
 }

 return res;
}

/// ������������ ������ ���������� ���������
bool NAfferentNeuron::BuildSimpleStructure(const string &ltzone_class,
					const string &receptorclass, double max_output)
{
 std::shared_ptr<UContainer> membr;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NLTZone> ltzone;
 std::shared_ptr<NReceptor> receptor;
 bool res(true);

 auto storage = Storage;
if(!storage)
  return false;

 DelComponent("PMembrane");
 DelComponent("PosGenerator");

 receptor=AddMissingComponent<NReceptor>("Receptor", receptorclass);
 receptor->Gain=1;
 receptor->OutputAdaptationMode=0;
 receptor->InputAdaptationMode=0;
 receptor->MaxInputRange=1;
 receptor->MinOutputRange=0;
 receptor->MaxOutputRange=max_output;
 receptor->SetCoord(MVector<double,3>(5,3,0));

 ltzone=AddMissingComponent<NLTZone>("LTZone", ltzone_class);
 ltzone->Threshold=0;
 ltzone->SetCoord(MVector<double,3>(13,3,0));

 res&=CreateLink(receptor->GetLongName(GetThisAsSharedContainer()),"Output",ltzone->GetLongName(GetThisAsSharedContainer()),"Inputs");

 return res;
}


// �������������� �������� �� ��������� � ����� �������� �����
bool NAfferentNeuron::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;

 StructureBuildMode=0;
 MembraneClassName="NPMembrane";
 ReceptorClassName="NReceptor";
 LTZoneClassName="NPLTZone";
 ExcGeneratorClassName="NPNeuronNegCGenerator";
 //InhGeneratorClassName="NPNeuronPosCGenerator";
 NumSomaMembraneParts=1;
 MaxReceptorOutput=100;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NAfferentNeuron::ABuild(void)
{
 if(!NPulseNeuronCommon::ABuild())
  return false;

 if(StructureBuildMode == 1)
 {
  bool res=BuildClassicalStructure(MembraneClassName, LTZoneClassName,
							ReceptorClassName, ExcGeneratorClassName,
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

// ����� �������� �����.
bool NAfferentNeuron::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool NAfferentNeuron::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif
