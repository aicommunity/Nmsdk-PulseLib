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

#ifndef NPULSE_CHANNEL_H
#define NPULSE_CHANNEL_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "NPulseChannelCommon.h"
#include "NPulseSynapse.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseChannel: public NPulseChannelCommon
{
public: // ������������� ��������
// ������� ��������
RDK::ULProperty<double,NPulseChannel> Capacity;

// ������������� ��������
RDK::ULProperty<double,NPulseChannel> Resistance;

// ������������� ���������� ��������
RDK::ULProperty<double,NPulseChannel> FBResistance;

// ��� �������� ����������� � ����� ���������
// � �����������
// �������������� �����
RDK::ULProperty<int,NPulseChannel> PotentialSummaryMode;
RDK::ULProperty<int,NPulseChannel, ptPubState> NumConnectedSynapsis;

public: // ������

double channel_input;
//double out;

protected: // �������� ��������

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannel(void);
virtual ~NPulseChannel(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
virtual size_t GetNumSynapses(void) const;

// ���������� ������ �� �������
UEPtr<NPulseSynapse> GetSynapse(size_t i);
// --------------------------
virtual double GetSynOutput(void);// �����-��������
virtual bool ResetOut(void);
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������� ��������
bool SetCapacity(const double &value);

// ������������� ��������
bool SetResistance(const double &value);

// ������������� ���������� ��������
bool SetFBResistance(const double &value);

bool SetPotentialSummaryMode(const int &value);
// --------------------------

// --------------------------
// ������ ���������� ��������
// --------------------------
// ���������� ������ ����� synapse � �������������� ���� �������-���������
// ���������� false ������ ���� ��������� ������ ��������� �����
// ���� synapse == 0, �� ���������� ��� ������� �����
bool InstallHebbSynapses(UEPtr<UContainer> synapse=0);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseChannel* New(void);
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
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
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
