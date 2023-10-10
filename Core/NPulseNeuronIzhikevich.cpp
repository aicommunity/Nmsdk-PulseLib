#ifndef NMSDK_NPulseNeuronIzhikevichCPP
#define NMSDK_NPulseNeuronIzhikevichCPP

#include "NPulseNeuronIzhikevich.h"
#include "NPulseChannelIzhikevich.h"
#include "NPulseMembraneIzhikevich.h"
#include "NPulseLTZoneIzhikevich.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuronIzhikevich::NPulseNeuronIzhikevich(void)
{
}

NPulseNeuronIzhikevich::~NPulseNeuronIzhikevich(void)
{
}
// --------------------------    

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseNeuronIzhikevich* NPulseNeuronIzhikevich::New(void)
{
 return new NPulseNeuronIzhikevich;
}
// --------------------------

// --------------------------
// ������ ���������� ������
// --------------------------
/// ������� ��������� �������� �������
void NPulseNeuronIzhikevich::CreateSimpleNeuron(void)
{
 AddMissingComponent<NPulseMembraneIzhikevich>("PulseMembrane", "NPulseMembraneIzhikevich");
 AddMissingComponent<NPulseLTZoneIzhikevich>("LTZone", "NPulseLTZoneIzhikevich");

 bool res=CreateLink("LTZone","Output","PulseMembrane","InputFeedbackSignal");
 res&=CreateLink("PulseMembrane.PosChannel","Output","LTZone","InputChannels");
 res&=CreateLink("PulseMembrane.PosChannel","Output","LTZone","InputChannels");
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseNeuronIzhikevich::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseNeuronIzhikevich::ABuild(void)
{
 if(!NPulseNeuronCommon::ABuild())
  return false;

 CreateSimpleNeuron();
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NPulseNeuronIzhikevich::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool NPulseNeuronIzhikevich::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif



