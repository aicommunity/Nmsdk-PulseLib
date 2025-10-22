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

#ifndef NPULSE_SYNAPSE_COMMON_CPP
#define NPULSE_SYNAPSE_COMMON_CPP

#include "NPulseSynapseCommon.h"
#include "NPulseNeuron.h"
#include "NPulseChannel.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapseCommon::NPulseSynapseCommon(void)
: Type("Type", this, &NPulseSynapseCommon::SetType),
  PulseAmplitude("PulseAmplitude",this,&NPulseSynapseCommon::SetPulseAmplitude),
  Resistance("Resistance",this,&NPulseSynapseCommon::SetResistance),
  Weight("Weight",this),
  TrainerClassName("TrainerClassName",this, &NPulseSynapseCommon::SetTrainerClassName),
  Input("Input",this),
  WeightInput("WeightInput",this),
  Output("Output",this),
  OutInCopy("OutInCopy",this),
  PreOutput("PreOutput",this),
  InputPulseSignal("InputPulseSignal",this)
{
 Trainer=0;
}

NPulseSynapseCommon::~NPulseSynapseCommon(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NPulseSynapseCommon::SetType(const double &value)
{
 Type.v=value;

 std::shared_ptr<NPulseMembrane> membr=std::shared_ptr<NPulseMembrane>(dynamic_pointer_cast<NPulseMembrane>(Owner.lock()).get());
 if(membr)
 {
  membr->UpdateSynapseData(std::shared_ptr<NPulseSynapseCommon>(dynamic_cast<NPulseSynapseCommon*>(this), RDK::NonOwningDeleter()));
 }

 return true;
}

// ������������� ��������� ���������
bool NPulseSynapseCommon::SetPulseAmplitude(const double &value)
{
 return true;
}

// ��� (������������� �������) �������
bool NPulseSynapseCommon::SetResistance(const double &value)
{
 return true;
}

/// ��� ������-�������, �������������� ��� �������
bool NPulseSynapseCommon::SetTrainerClassName(const std::string &value)
{
 Ready=false;
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseSynapseCommon* NPulseSynapseCommon::New(void)
{
 return new NPulseSynapseCommon;
}

// ���������� ��������� �� �������
NSynapseTrainer* NPulseSynapseCommon::GetTrainer(void)
{
 return Trainer;
}

// ������������� ����� ������� ���� �� ����
void NPulseSynapseCommon::RebuildInternalLinks(void)
{
 if(Trainer)
  Trainer->RebuildInternalLinks();
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseSynapseCommon::ADefault(void)
{
 Type = -1;

 // ��������� �������� ���� ����������
 // ��������� ������� ���������
 PulseAmplitude=1;

 // ��� (������������� �������) �������
 Resistance=10.0;

 Output.Assign(1,1,0.0);
 Input->Assign(1,1,0.0);
 OutInCopy.Assign(1,1,0.0);
 WeightInput->Assign(1,1,0.0);
 InputPulseSignal=false;

 Weight=1;

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseSynapseCommon::ABuild(void)
{
 return true;
}

bool NPulseSynapseCommon::AAfterBuild(void)
{
    if(!TrainerClassName->empty())
    {
     Trainer=AddMissingComponent<NSynapseTrainer>("Trainer", TrainerClassName).get();
     if(Trainer)
      Trainer->RebuildInternalLinks();
    }
    else
    {
     if(Trainer)
     {
      DelComponent(std::shared_ptr<UContainer>(Trainer, RDK::NonOwningDeleter()), true);
      //Storage->ReturnObject(Trainer);
      Trainer=0;
     }
    }
 return true;
}

// ����� �������� �����.
bool NPulseSynapseCommon::AReset(void)
{
 // ����� ��������� ����������
 PreOutput=0;
 InputPulseSignal=false;
 PulseSignalTemp=false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseSynapseCommon::ACalculate(void)
{
 if((*Input)(0,0)>0 && !PulseSignalTemp)
 {
  InputPulseSignal=true;
  PulseSignalTemp=true;
 }

 if((*Input)(0,0)<=0 && PulseSignalTemp)
  PulseSignalTemp=false;

 *OutInCopy=*Input;
 if(WeightInput.IsConnected())
  Weight=(*WeightInput)(0,0);

 bool res=ACalculate2();
 InputPulseSignal=false;
 Output(0,0)=Output(0,0)*Weight;
 return res;
}

bool NPulseSynapseCommon::ACalculate2(void)
{
 Output(0,0)=PreOutput;
 return true;
}
// --------------------------
}
#endif
