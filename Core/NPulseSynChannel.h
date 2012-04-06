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

#include "../../Kernel/NBios.h"
#include "NPulseChannel.h"

namespace NMSDK {

class NPulseSynChannel: public NPulseChannel
{
public: // ������������� ��������
// ��������� ��������
// ��������� ������� ���������
RDK::ULProperty<real,NPulseSynChannel> PulseAmplitude;

// ���������� ������� ��������� ���������
RDK::ULProperty<real,NPulseSynChannel> SecretionTC;

// ���������� ������� ������� ���������
RDK::ULProperty<real,NPulseSynChannel> DissociationTC;

// ����������� ����������������� ����������
RDK::ULProperty<real,NPulseSynChannel> InhibitionCoeff;

// ��� (������������� �������) �������
RDK::ULProperty<real,NPulseSynChannel> SynapseResistance;

public: // ������
// ������������� �������� ������������� �������
RDK::ULStateProperty<vector<real>,NPulseSynChannel> PreOutput;

// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
real VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
real OutputConstData;


protected: // �������� ��������

protected: // ��������� ����������

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
bool SetSynapseResistance(real value);
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
virtual bool CheckComponentType(UEPtr<NAContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
//virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
//virtual bool ADelComponent(UEPtr<UAContainer> comp);
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
