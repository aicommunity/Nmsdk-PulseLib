#ifndef NMSDK_NPulseChannelClassicH
#define NMSDK_NPulseChannelClassicH

//#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseChannelCommon.h"

namespace NMSDK {

class NPulseChannelClassic: public NPulseChannelCommon
{
public: // ���������
/// ������������� �������� (��� ���������� �������� ���� �� �������� ����������)
ULProperty<double, NPulseChannelClassic, ptPubParameter> CompartmentR;

/// ������������� ������� (��� ���������� �������������� ���� �� ������������� �������)
ULProperty<double, NPulseChannelClassic, ptPubParameter> SynapticR;

protected: // ���������� ���������
ULProperty<double, NPulseChannelClassic, ptPubState> SynapticI;
ULProperty<double, NPulseChannelClassic, ptPubState> CompartmentI;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseChannelClassic(void);
virtual ~NPulseChannelClassic(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseChannelClassic* New(void);
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
virtual bool ACalculate(void);
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



