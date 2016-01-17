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

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "../../Nmsdk-BasicLib/Core/NNet.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapseCommon: public UNet
{
public: // ������������� ��������
// ��������� ������� ���������
RDK::ULProperty<double,NPulseSynapseCommon> PulseAmplitude;

// ��� (������������� �������) �������
RDK::ULProperty<double,NPulseSynapseCommon> Resistance;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ������������� �������� ������������� �������
RDK::ULProperty<double,NPulseSynapseCommon,ptPubState> PreOutput;

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
// --------------------------
};

}
#endif
