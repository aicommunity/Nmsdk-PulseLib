// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NRECEIVER_H
#define NRECEIVER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NReceiver: public UNet//NAConnector
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NReceiver(void);
virtual ~NReceiver(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NReceiver* New(void);
// --------------------------
};

}

#endif



