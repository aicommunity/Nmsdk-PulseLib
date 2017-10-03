// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_HEBB_SYNAPSE_H
#define NPULSE_HEBB_SYNAPSE_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbSynapse: public NPulseSynapse
{
public: // ������������� ��������
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// �-� �������� ������������ �����
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// �-� �������� ����������� �����
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;


protected: // �������� ��������

protected: // ��������� ����������
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> G;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Gd;
RDK::UPropertyRange<double, vector<double>,NPulseHebbSynapse,ptPubState> Gs;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Win;
RDK::UProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
//bool SetPulseAmplitude(double value);

// ���������� ������� ��������� ���������
//bool SetSecretionTC(double value);

// ���������� ������� ������� ���������
//bool SetDissociationTC(double value);

// ����������� ����������������� ����������
//bool SetInhibitionCoeff(double value);

// ��� (������������� �������) �������
//bool SetResistance(double value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseHebbSynapse* New(void);
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
