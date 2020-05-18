// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2020.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_STDP_H
#define NPULSE_SYNAPSE_STDP_H

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapseStdp: public NPulseSynapse
{
public: // ������������� ��������
ULProperty<double,NPulseSynapseStdp, ptPubParameter> XModCoeff;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> YModCoeff;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> APlus;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> AMinus;

ULProperty<double,NPulseSynapseStdp, ptPubParameter> XTau;
ULProperty<double,NPulseSynapseStdp, ptPubParameter> YTau;

public: // ���������� ���������
/// ����������� �������� ��������
ULProperty<double,NPulseSynapseStdp, ptPubState> XAvg;

/// ����������� ��������� ��������
ULProperty<double,NPulseSynapseStdp, ptPubState> YAvg;

/// ������������� �������� ������� X � Y ���������
ULProperty<double,NPulseSynapseStdp, ptPubState> XYDiff;


public: // ����� � ������
/// ������� ������ ������� ����������������� ����������
/// (�������� ������������� �������)
UPropertyInputData<MDMatrix<double>, NPulseSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// �������� ������ ������� STDP
UPropertyOutputData<MDMatrix<double>, NPulseSynapseStdp, ptOutput | ptPubState> StdpInfluence;

protected: // �������� ��������

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapseStdp(void);
virtual ~NPulseSynapseStdp(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetXTau(const double &value);

bool SetYTau(const double &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynapseStdp* New(void);
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
