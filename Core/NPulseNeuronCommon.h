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

#ifndef NPULSE_NEURON_COMMON_H
#define NPULSE_NEURON_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NNeuron.h"
#include "NPulseLTZone.h"
#include "../../Nmsdk-SourceLib/Core/NConstGenerator.h"
#include "NPulseMembraneCommon.h"
//#include "NAfferentNeuron.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseNeuronCommon: public NNeuron
{
public: // ���������
/// ������� ������� ���������� � ��������� ���������
UProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageDendritesPotential;

/// ������� ������� ���������� � ��������� ���������
UProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageLTZonePotential;

public: // ����������
// ����� ������ �������������� ���� �������� �� ������ (� ����)
RDK::UProperty<double,NPulseNeuronCommon,ptPubState> NumActiveOutputs;

// ����� ������������ ������ �������������� ������� ��������� �� ����
RDK::UProperty<double,NPulseNeuronCommon,ptPubState> NumActivePosInputs;

// ����� ��������� ������ �������������� ������� ��������� �� ����
RDK::UProperty<double,NPulseNeuronCommon,ptPubState> NumActiveNegInputs;

public:
RDK::UEPointer<NPulseLTZone,NPulseNeuronCommon> LTZone;

vector<NPulseMembraneCommon*> Membranes;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuronCommon(void);
virtual ~NPulseNeuronCommon(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
/// ������� ������� ���������� � �������� ������ �������
bool SetUseAverageDendritesPotential(const bool &value);

/// ������� ������� ���������� � �������� ������ �������
bool SetUseAverageLTZonePotential(const bool &value);
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ �������������� ����
NLTZone* GetLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
//// ��������� �������� ������� ��������, �������� � ���� ����� ������� ��������,
//// � ���������� ����� ��������� ������� �� ����� ������
//// ���������� ��������� �� ��������� �������
//NPulseMembrane* ElongateDendrite(const UId &id, bool feedback=false);
//
//// ����������� �������� ������� ��������, �������� � ����� ��� �����������
//// ������������� ����� ������� ��������
//// ���������� ��������� �� ��������� �������
//NPulseMembrane* BranchDendrite(const UId &id, bool feedback=false);
//
//// ������� �������� ������� ��������
//// ���� full == true, �� ������� � ��� ������ �������, ������������ � ����
//// ����� �������������� ����� �� ������ �� ���� ������
//bool EraseDendrite(const UId &id);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseNeuronCommon* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
