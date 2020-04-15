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

#ifndef NPULSE_MEMBRANE_H
#define NPULSE_MEMBRANE_H

#include "NPulseMembraneCommon.h"
#include "NPulseChannel.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseMembrane: public NPulseMembraneCommon
{
public: // �������� ��������
/// ����������� �������� �����
ULProperty<double,NPulseMembrane, ptPubParameter> FeedbackGain;

/// ������� ��������� ������
ULProperty<bool,NPulseMembrane, ptPubParameter> ResetAvailable;

/// ��� ������ �������
ULProperty<std::string, NPulseMembraneCommon, ptPubParameter> SynapseClassName;

/// ��� ������ ������� ������
ULProperty<std::string, NPulseMembraneCommon, ptPubParameter> ChannelClassName;

/// ����� ������������ ��������
ULProperty<int, NPulseMembraneCommon, ptPubParameter> NumExcitatorySynapses;

/// ����� ��������� ��������
ULProperty<int, NPulseMembraneCommon, ptPubParameter> NumInhibitorySynapses;

public: // ����� � ������
/// ������ �������� ����� �� �������������� ����
UPropertyInputData<MDMatrix<double>, NPulseMembrane, ptInput | ptPubState> InputFeedbackSignal;

public: // ������
/// �������� �������� �����
ULProperty<double,NPulseMembrane,ptPubState> Feedback;

protected: // ��������� ����������
// ������ ��������� �������������
vector<NPulseChannel*> ExcitatoryChannels;

// ������ ��������� ����������������
vector<NPulseChannel*> InhibitoryChannels;

// ������������ �������
vector<NPulseSynapseCommon*> ExcitatorySynapses;

// ��������� �������
vector<NPulseSynapseCommon*> InhibitorySynapses;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseMembrane(void);
virtual ~NPulseMembrane(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� ����������
// --------------------------
// ������ ��������� �������������
size_t GetNumPosChannels(void) const;
NPulseChannel* GetPosChannel(size_t i);

// ������ ��������� ����������������
size_t GetNumNegChannels(void) const;
NPulseChannel* GetNegChannel(size_t i);

virtual bool UpdateChannelData(UEPtr<NPulseChannel> comp, UEPtr<UIPointer> pointer=0);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ����������� �������� �����
bool SetFeedbackGain(const double &value);

bool SetSynapseClassName(const std::string &value);
bool SetChannelClassName(const std::string &value);
bool SetNumExcitatorySynapses(const int &value);
bool SetNumInhibitorySynapses(const int &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseMembrane* New(void);
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

