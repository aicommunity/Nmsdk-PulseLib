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

#include "../../Kernel/NSupport.h"
#include "NPulseChannel.h"


namespace NMSDK {

class NPulseNeuron;

class NPulseMembrane: public UNet
{
public: // �������� ��������
// ����������� �������� �����
RDK::ULProperty<real,NPulseMembrane> FeedbackGain;

public: // ������
// �������� �������� �����
RDK::ULProperty<real,NPulseMembrane,ptPubState> Feedback;

protected: // ��������� ����������
// ������ ��������� �������������
vector<NPulseChannel*> PosChannels;

// ������ ��������� ����������������
vector<NPulseChannel*> NegChannels;

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
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ����������� �������� �����
bool SetFeedbackGain(const real &value);
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

