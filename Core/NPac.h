// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */
#ifndef NPAC_H
#define NPAC_H

#include "../BCL/NNet.h"


namespace NMSDK {

class NPac: public UANet
{
public: // ������������� ��������
// ��������� ������� ���������
RDK::UCLProperty<vector<Real>,NPac> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::UCLProperty<vector<Real>,NPac> SecretionTC;

// ���������� ������� ������� ���������
RDK::UCLProperty<vector<Real>,NPac> DissociationTC;

// ����������� ��������
RDK::UCLProperty<vector<Real>,NPac> Gain;

protected: // �������� ��������

protected: // ��������� ����������
// ������������� �������� ������������� �������
RDK::UCLProperty<vector<Real>,NPac, ptPubState> PreOutput;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPac(void);
virtual ~NPac(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool SetPulseAmplitude(vector<Real> value);

// ���������� ������� ��������� ���������
bool SetSecretionTC(vector<Real> value);

// ���������� ������� ������� ���������
bool SetDissociationTC(vector<Real> value);

// ��������
bool SetGain(vector<Real> value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPac* New(void);
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
