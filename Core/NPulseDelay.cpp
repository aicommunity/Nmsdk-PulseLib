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

#ifndef NPULSE_DELAY_CPP
#define NPULSE_DELAY_CPP

#include <cstdlib>
#include <cstdio>
#include "NPulseDelay.h"
//#include "NPulseChannel.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseDelay::NPulseDelay(void)
//: NSource(name),
 : Input("Input",this),
  DelayTime("DelayTime",this,&NPulseDelay::SetDelayTime)
{

}


NPulseDelay::~NPulseDelay(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------

// ������������� ������������ ��������
bool NPulseDelay::SetDelayTime(const double &value)
{
 if(value <= 0)
  return false;
 desired_buffer_length = value*TimeStep;
 buffer.clear();
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseDelay* NPulseDelay::New(void)
{
 return new NPulseDelay;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseDelay::CheckComponentType(UEPtr<UContainer> comp) const
{
 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseDelay::ADefault(void)
{
 DelayTime = 0;

 Output.Assign(1,1,0.0);

 return NSource::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseDelay::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseDelay::AReset(void)
{
 buffer.clear();
 Output.ToZero();

 return NSource::AReset();
}

// ��������� ������ ����� �������
bool NPulseDelay::ACalculate(void)
{
 if(desired_buffer_length==0 || fabs(DelayTime)<0.001)
 {
     *Output=*Input;
     return true;
 }
 int s = buffer.size();
 if((s-1)<desired_buffer_length)
 {
     buffer.push_front(Input);
     Output.ToZero();
 }
 else
 {
    std::list<MDMatrix<double> >::iterator lst;
    lst = buffer.end();
    lst--;
    MDMatrix<double> m = *(lst);
    Output = m;
    buffer.pop_back();
    buffer.push_front(Input);
 }
 return true;
}
// --------------------------

}
#endif
