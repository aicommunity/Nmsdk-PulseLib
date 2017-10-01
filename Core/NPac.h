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

#include "../../Nmsdk-BasicLib/Core/NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NPac: public UDynamicMatNet
{
public: // ������������� ��������
// ��������� ������� ���������
RDK::UPropertyRange<Real, vector<Real>,NPac, ptPubParameter> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::UPropertyRange<Real, vector<Real>,NPac, ptPubParameter> SecretionTC;

// ���������� ������� ������� ���������
RDK::UPropertyRange<Real, vector<Real>,NPac, ptPubParameter> DissociationTC;

// ����������� ��������
RDK::UPropertyRange<Real, vector<Real>,NPac, ptPubParameter> Gain;

/// ����� ������
/// 0 - ������� �����
/// 1 - �� ������� ��� ������������� ��������� ����� �������
RDK::UProperty<int,NPac, ptPubParameter> Mode;

/// ����� ����������
/// 0 - ��������
/// 1 - �������
RDK::UProperty<int,NPac, ptPubParameter> TCMode;

protected: // �������� ��������

protected: // ��������� ����������
// ������������� �������� ������������� �������
RDK::UPropertyRange<Real, vector<Real>,NPac, ptPubState> PreOutput;

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
bool SetPulseAmplitude(const vector<Real> &value);

// ���������� ������� ��������� ���������
bool SetSecretionTC(const vector<Real> &value);

// ���������� ������� ������� ���������
bool SetDissociationTC(const vector<Real> &value);

// ��������
bool SetGain(const vector<Real> &value);
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

class RDK_LIB_TYPE NCPac: public NPac
{
public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NCPac* New(void);
// --------------------------

};

}
#endif
