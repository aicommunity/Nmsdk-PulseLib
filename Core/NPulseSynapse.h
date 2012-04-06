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

#include "../../Kernel/NBios.h"
#include "../BCL/NConnector.h"


namespace NMSDK {

class NPulseNeuron;

class NPulseSynapse: public NConnector
{
public: // ������������� ��������
// ��������� ������� ���������
RDK::ULProperty<real,NPulseSynapse> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::ULProperty<real,NPulseSynapse> SecretionTC;

// ���������� ������� ������� ���������
RDK::ULProperty<real,NPulseSynapse> DissociationTC;

// ����������� ����������������� ����������
RDK::ULProperty<real,NPulseSynapse> InhibitionCoeff;

// ��� (������������� �������) �������
RDK::ULProperty<real,NPulseSynapse> Resistance;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ������������� �������� ������������� �������
RDK::ULStateProperty<real,NPulseSynapse> PreOutput;

// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
real VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
real OutputConstData;

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
bool SetPulseAmplitude(real value);

// ���������� ������� ��������� ���������
bool SetSecretionTC(real value);

// ���������� ������� ������� ���������
bool SetDissociationTC(real value);

// ����������� ����������������� ����������
bool SetInhibitionCoeff(real value);

// ��� (������������� �������) �������
bool SetResistance(real value);
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
