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

#ifndef NPULSE_SYN_CHANNEL_H
#define NPULSE_SYN_CHANNEL_H

#include "NPulseChannel.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseSynChannel: public NPulseChannel
{
public: // ������������� ��������
// ��������� ��������
// ��������� ������� ���������
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> SecretionTC;

// ���������� ������� ������� ���������
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> DissociationTC;

// ����������� ����������������� ����������
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> InhibitionCoeff;

// ��� (������������� �������) �������
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> SynapseResistance;

public: // ������
// ������������� �������� ������������� �������
RDK::UProperty<vector<double>,NPulseSynChannel,ptPubState> PreOutput;

// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
double VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
double OutputConstData;

//�������� ������ �������
double syn_output;

protected: // �������� ��������

protected: // ��������� ����������
/// ������ ��������� �������� �� ���� �������� ��� ���
std::vector<bool> SynapseInputFlagsList;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynChannel(void);
virtual ~NPulseSynChannel(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
size_t GetNumSynapses(void) const;

// ���������� ������ �� �������
NPulseSynapse* GetSynapse(size_t i);
// --------------------------
double GetSynOutput(void);
bool ResetOut(void);
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
bool SetSynapseResistance(const double &value);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynChannel* New(void);
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
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
//virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
//virtual bool ADelComponent(UEPtr<UContainer> comp);
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

class RDK_LIB_TYPE NContinuesSynChannel: public NPulseChannel
{
public: // ������������� ��������
// ��������� ��������
// ��������� ������� ���������
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> SecretionTC;

// ���������� ������� ������� ���������
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> DissociationTC;

// ����������� ����������������� ����������
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> InhibitionCoeff;

// ��� (������������� �������) �������
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> SynapseResistance;

public: // ������
// ������������� �������� ������������� �������
RDK::UProperty<vector<double>,NContinuesSynChannel,ptPubState> PreOutput;

// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
double VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
double OutputConstData;


protected: // �������� ��������

protected: // ��������� ����������
/// ������ ��������� �������� �� ���� �������� ��� ���
std::vector<bool> SynapseInputFlagsList;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesSynChannel(void);
virtual ~NContinuesSynChannel(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
size_t GetNumSynapses(void) const;

// ���������� ������ �� �������
NPulseSynapse* GetSynapse(size_t i);
// --------------------------

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
bool SetSynapseResistance(const double &value);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NContinuesSynChannel* New(void);
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
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
//virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
//virtual bool ADelComponent(UEPtr<UContainer> comp);
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
