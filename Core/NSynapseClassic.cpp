#ifndef NMSDK_NSYNAPSECLASSIC_CPP
#define NMSDK_NSYNAPSECLASSIC_CPP

#include "NSynapseClassic.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseClassic::NSynapseClassic(void)
 : TauS("TauS",this),
   Current("Current",this)
{
}

NSynapseClassic::~NSynapseClassic(void)
{
}
// --------------------------    


// ---------------------
// ������ ���������� �����������
// ---------------------
// ---------------------

// ---------------------
// ������ ���������� ����������� ���������
// ---------------------
// ---------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseClassic* NSynapseClassic::New(void)
{
 return new NSynapseClassic;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������ 
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseClassic::ADefault(void)
{
 if(!NPulseSynapseCommon::ADefault())
  return false;

 TauS=1e-3;
 Current=376e-12;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NSynapseClassic::ABuild(void)
{
 if(!NPulseSynapseCommon::ABuild())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NSynapseClassic::AReset(void)
{
 if(!NPulseSynapseCommon::AReset())
  return false;

 return true;
}

// ��������� ������ ����� �������
bool NSynapseClassic::ACalculate2(void)
{
 return true;
}
// --------------------------

}
#endif



