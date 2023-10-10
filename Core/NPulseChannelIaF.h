#ifndef NMSDK_NPulseChannelIaFH
#define NMSDK_NPulseChannelIaFH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIaF: public NPulseChannelClassic
{
public: // ��������
/// ������� ��������
ULProperty<double, NPulseChannelIaF, ptPubParameter> Cm;

/// ��������� �����
ULProperty<double, NPulseChannelIaF, ptPubParameter> EL;

/// ���������� ������� ��������
ULProperty<double, NPulseChannelIaF, ptPubParameter> TauM;

/// ������ ��������������
ULProperty<double, NPulseChannelIaF, ptPubParameter> TRef;

/// ��������� ����������
ULProperty<double, NPulseChannelIaF, ptPubParameter> VReset;

/// ������������ ���������� ���
ULProperty<double, NPulseChannelIaF, ptPubParameter> Ie;

/// ���������� ������� ���������� ��������
ULProperty<double, NPulseChannelIaF, ptPubParameter> VMin;

/// ���� ���������� ������ ���������� ��� ��������� ������
ULProperty<bool, NPulseChannelIaF, ptPubParameter> VResetEnable;

public: // ����� � ������

protected: // ���������� ���������
/// ���������� ���������
/// ��������� ��������� ��������
ULProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Vm;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelIaF(void);
virtual ~NPulseChannelIaF(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseChannelIaF* New(void);
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



