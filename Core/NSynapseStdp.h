#ifndef NMSDK_NSYNAPSESTDP_H
#define NMSDK_NSYNAPSESTDP_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseStdp: public NPulseSynapseCommon
{
public:
ULProperty<double,NSynapseStdp, ptPubParameter> XModCoeff;

ULProperty<double,NSynapseStdp, ptPubParameter> YModCoeff;

ULProperty<double,NSynapseStdp, ptPubParameter> APlus;

ULProperty<double,NSynapseStdp, ptPubParameter> AMinus;

ULProperty<double,NSynapseStdp, ptPubParameter> XTau;
ULProperty<double,NSynapseStdp, ptPubParameter> YTau;

public: // ���������� ���������
/// ����������� �������� ��������
ULProperty<double,NSynapseStdp, ptPubState> XAvg;

/// ����������� ��������� ��������
ULProperty<double,NSynapseStdp, ptPubState> YAvg;

/// ������������� �������� ������� X � Y ���������
ULProperty<double,NSynapseStdp, ptPubState> XYDiff;


public: // ����� � ������
/// ������� ������ ����������������� ���������� (���� ����� �� �������, ���� ��������
/// �������� �������������)
UPropertyInputData<MDMatrix<double>, NSynapseStdp, ptInput | ptPubState> PsActivityInput;

/// �������� ������ ������� STDP
UPropertyOutputData<MDMatrix<double>, NSynapseStdp, ptOutput | ptPubState> StdpInfluence;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseStdp(void);
virtual ~NSynapseStdp(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseStdp* New(void);
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

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



