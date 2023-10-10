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

#include "NNet.h"
#include "NSynapseTrainer.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapseCommon: public UNet
{
public: // ������������� ��������
/// ��� ������� ���������
/// <0 - �������� ���������� �������������� ������ � ��������� (��� ������������ ��������) - ������������ ������
/// >0 - �������� ���������� �������������� ������ � ��������� (��� ��������������� ��������) - ��������� ������
ULProperty<double,NPulseSynapseCommon, ptPubParameter> Type;

/// ��������� ������� ���������
ULProperty<double,NPulseSynapseCommon, ptPubParameter> PulseAmplitude;

/// ������������� �������
ULProperty<double,NPulseSynapseCommon, ptPubParameter> Resistance;

/// ��� �������
ULProperty<double, NPulseSynapseCommon, ptPubParameter> Weight;

/// ��� ������-�������, �������������� ��� �������
ULProperty<std::string, NPulseSynapseCommon, ptPubParameter> TrainerClassName;

public: // ����� � ������
/// ������� ������ � �������
UPropertyInputData<MDMatrix<double>, NPulseSynapseCommon, ptInput | ptPubState> Input;

/// ������� �������� ���������� � ����. �������������� �������� Weight ���� ���� �����������
UPropertyInputData<MDMatrix<double>, NPulseSynapseCommon, ptInput | ptPubState> WeightInput;

/// �������� ������� ������� �� ��������
UPropertyOutputData<MDMatrix<double>,NPulseSynapseCommon, ptOutput | ptPubState> Output;

/// ��������������� �����, ����������� ������� ������ �� ������
UPropertyOutputData<MDMatrix<double>,NPulseSynapseCommon, ptOutput | ptPubState> OutInCopy;

protected: // ��������� ����������
/// ������������� �������� ������������� �������
ULProperty<double,NPulseSynapseCommon,ptPubState> PreOutput;

/// ������ ������� �������� ��������
/// (������������ ������������� � ����� ��������)
ULProperty<bool, NPulseSynapseCommon, ptPubState> InputPulseSignal;

bool PulseSignalTemp;

NSynapseTrainer* Trainer;

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
virtual bool SetType(const double &value);

// ������������� ��������� ���������
virtual bool SetPulseAmplitude(const double &value);

// ��� (������������� �������) �������
virtual bool SetResistance(const double &value);

/// ��� ������-�������, �������������� ��� �������
virtual bool SetTrainerClassName(const std::string &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynapseCommon* New(void);

// ���������� ��������� �� �������
NSynapseTrainer* GetTrainer(void);

// ������������� ����� ������� ���� �� ����
virtual void RebuildInternalLinks(void);
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
virtual bool AAfterBuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
