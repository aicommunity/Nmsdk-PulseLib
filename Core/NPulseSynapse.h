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

#ifndef NPULSE_SYNAPSE_H
#define NPULSE_SYNAPSE_H

#include "../../Kernel/NSupport.h"
#include "../BCL/NNet.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseSynapse: public UNet
{
public: // ������������� ��������
// ��������� ������� ���������
RDK::ULProperty<double,NPulseSynapse> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::ULProperty<double,NPulseSynapse> SecretionTC;

// ���������� ������� ������� ���������
RDK::ULProperty<double,NPulseSynapse> DissociationTC;

// ����������� ����������������� ����������
RDK::ULProperty<double,NPulseSynapse> InhibitionCoeff;

// ��� (������������� �������) �������
RDK::ULProperty<double,NPulseSynapse> Resistance;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ������������� �������� ������������� �������
RDK::ULProperty<double,NPulseSynapse,ptPubState> PreOutput;

// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
double VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
double OutputConstData;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapse(void);
virtual ~NPulseSynapse(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);

// ���������� ������� ��������� ���������
bool SetSecretionTC(const double &value);

// ���������� ������� ������� ���������
bool SetDissociationTC(const double &value);

// ����������� ����������������� ����������
bool SetInhibitionCoeff(const double &value);

// ��� (������������� �������) �������
bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynapse* New(void);
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
