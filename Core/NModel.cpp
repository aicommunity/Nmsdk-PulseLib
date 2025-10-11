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

#ifndef NMODEL_CPP
#define NMODEL_CPP

#include "NModel.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NModel::NModel(void)
//: NANet(name)
{
}

NModel::~NModel(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NModel* NModel::New(void)
{
 return new NModel();
}
// --------------------------



}
#endif


