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

#ifndef NPULSE_HEBB_LIFE_SYNAPSE_H
#define NPULSE_HEBB_LIFE_SYNAPSE_H

#include "../../Nmsdk-PulseLib/Core/NPulseHebbSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbLifeSynapse: public NPulseHebbSynapse
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbLifeSynapse(void);
virtual ~NPulseHebbLifeSynapse(void);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseHebbLifeSynapse* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ���������� ������ ����� � ������ ���������������� �������
bool InstallLifeConnection(void);

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
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
