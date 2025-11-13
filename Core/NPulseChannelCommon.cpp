// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_CHANNEL_COMMON_CPP
#define NPULSE_CHANNEL_COMMON_CPP

#include "NPulseChannelCommon.h"
#include "NPulseMembrane.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelCommon::NPulseChannelCommon(void)
 : Type("Type", this, &NPulseChannelCommon::SetType),
   UseAveragePotential("UseAveragePotential",this),
   UseAverageSynapsis("UseAverageSynapsis",this),
   ChannelInputs("ChannelInputs",this),
   SynapticInputs("SynapticInputs",this),
   Output("Output",this),
   SumChannelInput("SumChannelInput",this),
   SumSynapticInput("SumSynapticInput",this),
   IsNeuronActivated("IsNeuronActivated",this)
{
}

NPulseChannelCommon::~NPulseChannelCommon(void)
{
}
// --------------------------


// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
int NPulseChannelCommon::GetNumSynapses(void) const
{
 return int(SynapticInputs->size());
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
//
bool NPulseChannelCommon::SetType(const double &value)
{
 Type.v=value;

 // NPulseChannelCommon inherits from UNet, which inherits from UContainer (UComponent)
 // Use shared_from_this() instead of safe_shared_cast
 std::shared_ptr<NPulseMembrane> membr=dynamic_pointer_cast<NPulseMembrane>(Owner.lock());
 if(membr)
 {
  membr->UpdateChannelData(std::static_pointer_cast<NPulseChannelCommon>(shared_from_this()));
 }

 for(int i=0;i<int(SynapticInputs->size());i++)
 {
  NPulseSynapseCommon* synapse = dynamic_cast<NPulseSynapseCommon*>(SynapticInputs.GetItem(i));
  if(synapse)
   synapse->Type = value;
 }

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseChannelCommon* NPulseChannelCommon::New(void)
{
 return new NPulseChannelCommon;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseChannelCommon::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
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
bool NPulseChannelCommon::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseChannelCommon::ADelComponent(std::shared_ptr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
/// ��������� ������ � ��������� ������� �������������� �����
/// (���������� �� ������ ��������)
void NPulseChannelCommon::NeuronActivated(void)
{
 IsNeuronActivated=true;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseChannelCommon::ADefault(void)
{
 Type=0;
 UseAveragePotential=true;
 UseAverageSynapsis=false;
 Output.Assign(1,1,0.0);
 SumChannelInput.Assign(1,1,0.0);
 SumSynapticInput.Assign(1,1,0.0);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseChannelCommon::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseChannelCommon::AReset(void)
{
 Output.ToZero();
 SumChannelInput.ToZero();
 SumSynapticInput.ToZero();
 IsNeuronActivated=false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseChannelCommon::ACalculate(void)
{
 ACalculate2();
 IsNeuronActivated=false;
 return true;
}

bool NPulseChannelCommon::ACalculate2(void)
{
 return true;
}

// --------------------------
}

#endif

