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

#ifndef NSINUS_GENERATOR_CPP
#define NSINUS_GENERATOR_CPP

#include "NSinusGenerator.h"
//#include "rdk.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSinusGenerator::NSinusGenerator(void)
//: NSource(name),
 : Amplitude("Amplitude",this,&NSinusGenerator::SetAmplitude),
  Frequency("Frequency",this,&NSinusGenerator::SetFrequency),

  TimeCounter("TimeCounter",this)
{
 // ������������� ����� ���������
// AddLookupParameter("Amplitude",Amplitude);
// AddLookupParameter("Frequency",Frequency);
}

NSinusGenerator::~NSinusGenerator(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ������
bool NSinusGenerator::SetAmplitude(const double &value)
{
 return true;
}

// ������������� �������
bool NSinusGenerator::SetFrequency(const double &value)
{
 if(value <=0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSinusGenerator* NSinusGenerator::New(void)
{
 return new NSinusGenerator;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSinusGenerator::ADefault(void)
{
 Amplitude=1.0;
 Frequency=1;
 return NSource::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NSinusGenerator::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSinusGenerator::AReset(void)
{
 TimeCounter=0;
 return NSource::AReset();
}

// ��������� ������ ����� �������
bool NSinusGenerator::ACalculate(void)
{
 TimeCounter.v+=1.0/TimeStep;
 double tmp=Amplitude*sin(2.0*M_PI*TimeCounter*Frequency);
 Output(0,0)=tmp;
 return true;
}
// --------------------------
}
#endif
