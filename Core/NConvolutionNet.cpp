#ifndef NMSDK_NCONVOLUTIONNET_CPP
#define NMSDK_NCONVOLUTIONNET_CPP

#include "NConvolutionNet.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NConvolutionNet::NConvolutionNet(void)
{
}

NConvolutionNet::~NConvolutionNet(void)
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
NConvolutionNet* NConvolutionNet::New(void)
{
 return new NConvolutionNet;
}
// --------------------------


// --------------------------
// ������� ������ ���������� ������ 
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NConvolutionNet::ADefault(void)
{           
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NConvolutionNet::ABuild(void)
{
 return true;
}

// ����� �������� ����� ��� ������ ��������
bool NConvolutionNet::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool NConvolutionNet::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif



