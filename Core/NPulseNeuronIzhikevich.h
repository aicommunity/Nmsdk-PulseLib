#ifndef NMSDK_NPulseNeuronIzhikevichH
#define NMSDK_NPulseNeuronIzhikevichH

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuronCommon.h"

namespace NMSDK {

class NPulseNeuronIzhikevich: public NPulseNeuronCommon
{
public: // ��������

public: // ���������� ���������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuronIzhikevich(void);
virtual ~NPulseNeuronIzhikevich(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseNeuronIzhikevich* New(void);
// --------------------------        

// --------------------------
// ������ ���������� ������
// --------------------------
/// ������� ��������� �������� �������
virtual void CreateSimpleNeuron(void);
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
// --------------------------
};


}

#endif



