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

#ifndef NPULSE_SYNAPSE_COMMON_H
#define NPULSE_SYNAPSE_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NNet.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapseCommon: public UNet
{
public: // ������������� ��������
/// ��������� ������� ���������
ULProperty<double,NPulseSynapseCommon, ptPubParameter> PulseAmplitude;

/// ��� (������������� �������) �������
ULProperty<double,NPulseSynapseCommon, ptPubParameter> Resistance;

public: // ����� � ������
/// ������� ������ � �������
UPropertyInputData<MDMatrix<double>, NPulseSynapseCommon, ptInput | ptPubState> Input;

/// �������� ������� ������� �� ��������
UPropertyOutputData<MDMatrix<double>,NPulseSynapseCommon, ptOutput | ptPubState> Output;

protected: // ��������� ����������
/// ������������� �������� ������������� �������
ULProperty<double,NPulseSynapseCommon,ptPubState> PreOutput;

/// ������ ������� �������� ��������
/// (������������ ������������� � ����� ��������)
ULProperty<bool, NPulseSynapseCommon, ptPubState> InputPulseSignal;

bool PulseSignalTemp;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapseCommon(void);
virtual ~NPulseSynapseCommon(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
virtual bool SetPulseAmplitude(const double &value);

// ��� (������������� �������) �������
virtual bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynapseCommon* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
