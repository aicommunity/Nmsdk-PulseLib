#ifndef NMSDK_NPulseLTZoneIzhikevichH
#define NMSDK_NPulseLTZoneIzhikevichH

#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"

namespace NMSDK {

class NPulseLTZoneIzhikevich: public NPulseLTZoneCommon
{
protected: // ��������

protected: // ���������� ���������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZoneIzhikevich(void);
virtual ~NPulseLTZoneIzhikevich(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseLTZoneIzhikevich* New(void);
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

/// ���������� true ���� ������� ��� ��������� �������� ���������
virtual bool CheckPulseOn(void);

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
virtual bool CheckPulseOff(void);
// --------------------------
};


}

#endif



