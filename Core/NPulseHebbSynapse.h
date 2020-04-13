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

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbSynapse: public NPulseSynapse
{
public: // ������������� ��������
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
UCLProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
ULProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// �-� �������� ������������ �����
RDK::ULProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// �-� �������� ����������� �����
RDK::ULProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;

public: // ����� � ������
/// ������� ������ �� �������������� ����
UPropertyInputData<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputLTZoneFeedbackSignal;

/// ������ "���������"
UPropertyInputData<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputMotivation;

UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output4;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output6;

protected: // ��������� ����������
ULProperty<double,NPulseHebbSynapse,ptPubState> G;
ULProperty<double,NPulseHebbSynapse,ptPubState> Gd;
UCLProperty<vector<double>,NPulseHebbSynapse,ptPubState> Gs;
ULProperty<double,NPulseHebbSynapse,ptPubState> Win;
ULProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
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
