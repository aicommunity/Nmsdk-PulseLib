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

#ifndef NPULSE_GENERATOR_DELAY_CPP
#define NPULSE_GENERATOR_DELAY_CPP

#include <cstdlib>
#include <cstdio>
#include "NPulseGeneratorDelay.h"
//#include "NPulseChannel.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseGeneratorDelay::NPulseGeneratorDelay(void)
//: NSource(name),
 : PulseLength("PulseLength",this,&NPulseGeneratorDelay::SetPulseLength),
  Amplitude("Amplitude",this,&NPulseGeneratorDelay::SetAmplitude),
  ParameterData("ParameterData",this,&NPulseGeneratorDelay::SetParameterData),
  MinSample("MinSample",this,&NPulseGeneratorDelay::SetMinSample),
  MaxSample("MaxSample",this,&NPulseGeneratorDelay::SetMaxSample),

  Number("Number",this),
  FlagStart("FlagStart",this)
{
    Delay=0;
}


NPulseGeneratorDelay::~NPulseGeneratorDelay(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------

// ������������� ������������ ��������
bool NPulseGeneratorDelay::SetPulseLength(const double &value)
{
 if(value <= 0)
  return false;

 return true;
}

// ������������� ��������� ���������
bool NPulseGeneratorDelay::SetAmplitude(const double &value)
{
 return true;
}

// ������������� ��������� ���������
bool NPulseGeneratorDelay::SetParameterData(const int &value)
{
 if((value <=0)|(value >= 5))
  return false;
 return true;
}

// ������������� ����������� �������� � ������
bool NPulseGeneratorDelay::SetMinSample(const double &value)
{
 return true;
}

// ������������� ������������ �������� � ������
bool NPulseGeneratorDelay::SetMaxSample(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseGeneratorDelay* NPulseGeneratorDelay::New(void)
{
 return new NPulseGeneratorDelay;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseGeneratorDelay::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseGeneratorDelay::ADefault(void)
{
 PulseLength=0.001;
 Amplitude=1.0;
 ParameterData=1;
 MinSample=4.3;
 MaxSample=7.9;

 Output.Assign(1,1,0.0);

 return NSource::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseGeneratorDelay::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NPulseGeneratorDelay::AReset(void)
{
 time_t tm;
 time(&tm);
 srand(static_cast<unsigned>(tm));
 switch(ParameterData)
 {
  case 1:
   MinSample=4.3;
   MaxSample=7.9;
     break;
  case 2:
   MinSample=2;
   MaxSample=4.4;
     break;
  case 3:
   MinSample=1;
   MaxSample=6.9;
     break;
  case 4:
   MinSample=0.1;
   MaxSample=2.5;
     break;
 }
 Output.ToZero();

 return NSource::AReset();
}

// ��������� ������ ����� �������
bool NPulseGeneratorDelay::ACalculate(void)
{
 double norm;
 //if ((*FlagStart)(0,0) > 0)
 {
  auto TimeStart = Environment.lock()->GetTime().GetDoubleTime();
  norm=(Number - MinSample)/(MaxSample - MinSample);
  Delay=norm*0.01;
  while(Environment.lock()->GetTime().GetDoubleTime() - TimeStart < Delay){;}
  Output.Assign(1,1,Amplitude);
  auto TimeStartPulse = Environment.lock()->GetTime().GetDoubleTime();
  while(Environment.lock()->GetTime().GetDoubleTime() - TimeStartPulse < PulseLength){;}
  Output.ToZero();
 }
 return true;
}
// --------------------------

}
#endif
