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

#ifndef NPULSE_CHANNEL_COMMON_H
#define NPULSE_CHANNEL_COMMON_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "NPulseSynapse.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseChannelCommon: public UNet
{
public: // ������������� ��������
// ��� ������� ���������
// <0 - ����������� ������������� ����� � ��������� (��� ��������������� ��������)
// >0 - ����������� ������������� ����� � ��������� (��� ������������ ��������)
RDK::UProperty<double,NPulseChannelCommon, ptPubParameter> Type;

/// ������� ������� ���������� � �������� ������ �������
UProperty<bool, NPulseChannelCommon, ptPubParameter> UseAveragePotential;

/*/// ���������� ����������� ���������, ���� ���������� �� �������� ������ ��� �����
ULProperty<bool, NPulseChannelCommon> RestingFlag;

/// ����� ���������� �������� ��� ������ ������������ ����������
ULProperty<double, NPulseChannelCommon> RestingThreshold;
  */

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelCommon(void);
virtual ~NPulseChannelCommon(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ������������
// --------------------------
// ���������� ����� ��������
virtual size_t GetNumSynapses(void) const;
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ��� ������� ���������
bool SetType(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseChannelCommon* New(void);
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
