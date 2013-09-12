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

#include "../../Kernel/NSupport.h"
#include "NPulseSynapse.h"


namespace NMSDK {

class NPulseHebbSynapse: public NPulseSynapse
{
public: // ������������� ��������
RDK::ULProperty<double,NPulseHebbSynapse> Min;
RDK::ULProperty<double,NPulseHebbSynapse> Mout;
RDK::ULProperty<double,NPulseHebbSynapse> Md;
RDK::UCLProperty<vector<double>,NPulseHebbSynapse> ActiveMs;
RDK::UCLProperty<vector<double>,NPulseHebbSynapse> PassiveMs;
RDK::UCLProperty<vector<double>,NPulseHebbSynapse> Kmot;
RDK::ULProperty<double,NPulseHebbSynapse> Kin;
RDK::ULProperty<double,NPulseHebbSynapse> Kout;

// �-� �������� ������������ �����
RDK::ULProperty<double,NPulseHebbSynapse> GdGain;

// �-� �������� ����������� �����
RDK::ULProperty<double,NPulseHebbSynapse> GsGain;


protected: // �������� ��������

protected: // ��������� ����������
RDK::ULProperty<double,NPulseHebbSynapse,ptPubState> G;
RDK::ULProperty<double,NPulseHebbSynapse,ptPubState> Gd;
RDK::UCLProperty<vector<double>,NPulseHebbSynapse,ptPubState> Gs;
RDK::ULProperty<double,NPulseHebbSynapse,ptPubState> Win;
RDK::ULProperty<double,NPulseHebbSynapse,ptPubState> Wout;

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
