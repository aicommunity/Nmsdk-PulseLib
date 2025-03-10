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

#ifndef NPULSE_LTZONE_COMMON_H
#define NPULSE_LTZONE_COMMON_H

#include "NPulseGenerator.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NLTZone: public UNet
{
public: // ������������� ��������
/// ����� ����������� �������
ULProperty<double,NLTZone, ptPubParameter> Threshold;

/// ����� ���������� ��������� �������� ��������
ULProperty<double,NLTZone, ptPubParameter> ThresholdOff;

/// ������� ������� ���������� � �������� ������ �������
ULProperty<bool,NLTZone, ptPubParameter> UseAveragePotential;

public: // ����� � ������
/// ������ � ������ ���������� �������� ��������
UPropertyInputCData<MDMatrix<double>,NLTZone, ptInput | ptPubState> Inputs;

/// ����� ��������� �������
UPropertyOutputData<MDMatrix<double>, NLTZone, ptOutput | ptPubState> Output;

public: // ������
/// ��������� �������, ������������ � �������
ULProperty<double,NLTZone, ptPubState> Potential;


/// ������ �������� �������� ������
//NPulseNeuron* Neuron;

protected: // �������� ��������

protected: // ��������� ����������
/// ����� ������������ �������� � ���� �������������� ����
int CachedNumAConnectors;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NLTZone(void);
virtual ~NLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ����� �������
bool SetThreshold(const double &value);

// ������������� ����� ���������� ��������� �������� ��������
bool SetThresholdOff(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ����� �������� �����.
virtual bool AReset(void);
// --------------------------
};


class RDK_LIB_TYPE NPulseLTZoneCommon: public NLTZone
{
public: // ������������� ��������
/// ����� ������ ���������� � ������
/// (������������ ��� ���������� ���������� �������� ����������)
/// �������� ���� ������� �������� ������ ������� �� 2 ����� ����������
/// ������ ����� ������ ������, �� �������� ����� 2. ����� 1.
ULProperty<int,NPulseLTZoneCommon, ptPubParameter> NumChannelsInGroup;

/// ��������� ���������
ULProperty<double,NPulseLTZoneCommon, ptPubParameter> PulseAmplitude;

/// ������������ ��������
ULProperty<double,NPulseLTZoneCommon, ptPubParameter> PulseLength;

/// �������� ������� ������ ������� ���������
ULProperty<double,NPulseLTZoneCommon, ptPubParameter> AvgInterval;

public: // ����� � ������
/// ��������� (��� ����� ���������� �������� PulseOutput) (1)
UPropertyOutputData<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputPotential;

/// ������� (2)
UPropertyOutputData<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputFrequency;

/// ������ �������� ������� ������ ��������� (3)
UPropertyOutputData<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputPulseTimes;

public: // ������
/// ������������� �������� ����������
/// ������� ������������ ��� ���������� ��� ���������� ��������� ����������� ����������
ULProperty<double,NPulseLTZoneCommon,ptPubState> PrePotential;

protected: // �������� ��������

protected: // ��������� ����������
// ��������� ���������
RDK::ULProperty<double,NPulseLTZoneCommon,ptPubState> NeuralPotential;

/// ���� ������� ���������
ULProperty<int,NPulseLTZoneCommon,ptPubState> PulseCounter;

/// ������� ������� �� �������� �������� �������
UCLProperty<list<double>,NPulseLTZoneCommon,ptPubState> AvgFrequencyCounter;

/// ������� ������� ��������� ��������
ULProperty<bool,NPulseLTZoneCommon,ptPubState> PulseFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZoneCommon(void);
virtual ~NPulseLTZoneCommon(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseLTZoneCommon* New(void);
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
virtual bool ACalculate2(void);

/// ���������� true ���� ������� ��� ��������� �������� ���������
virtual bool CheckPulseOn(void);

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
virtual bool CheckPulseOff(void);
// --------------------------
};


}
#endif
