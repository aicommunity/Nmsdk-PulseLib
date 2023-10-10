// ===========================================================
// Version:        3.0.2
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

#ifndef NNEURON_CPP
#define NNEURON_CPP

#include "NNeuron.h"
//#include "NConnector.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuron::NNeuron(void)
//: NNet(name)
{
}

NNeuron::~NNeuron(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNeuron* NNeuron::New(void)
{
 return new NNeuron;
}
// --------------------------


}
#endif


