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

#ifndef NSUM_CPP
#define NSUM_CPP

#include "NSum.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSum::NSum(void)
: Mode("Mode",this,&NSum::SetMode),
  Gain("Gain",this,&NSum::SetGain),
  Inputs("Inputs",this),
  Output("Output",this)
{
}

NSum::~NSum(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ��������
bool NSum::SetGain(const MDMatrix<double> &value)
{
 return true;
}

// ����� ��������
bool NSum::SetMode(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------



// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSum* NSum::New(void)
{
 return new NSum;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSum::ADefault(void)
{
 Mode=0;
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NSum::ABuild(void)
{
/* if(Mode == 0)
 {
  Gain->resize(NumInputs);
  for(int i=0;i<NumInputs;i++)
   Gain[i].resize(GetInputDataSize(i));
 }
 else
 if(Mode == 1)
 {
  Gain->resize(NumInputs);
  for(int i=0;i<NumInputs;i++)
   Gain[i].resize(GetInputDataSize(i));
 }*/
 return true;
}

// ����� �������� �����.
bool NSum::AReset(void)
{
 return true;
}

// ��������� ������ ����� �������
bool NSum::ACalculate(void)
{
 int min_input_data_size(0);
 if(Inputs->size()>0)
 {
  min_input_data_size=Inputs[0].GetCols();
  for(int i=1;i<int(Inputs->size());i++)
   if(min_input_data_size>Inputs[i].GetCols())
    min_input_data_size=Inputs[i].GetCols();
 }

 if(Mode == 0)
 {

  Output.Assign(1,min_input_data_size,0.0);
  Build();

  for(int j=0;j<int(Inputs->size());j++)
   for(int i=0;i<min_input_data_size;i++)
   {
    double gain=1;
    if(Gain.GetRows()>int(j) && Gain.GetCols()>i)
     gain=Gain(j,i);
    Output(0,i)+=gain*Inputs[j](0,i);//GetInputData(j)->Double[i];
   }
 }
 else
 if(Mode == 1)
 {
  Output.Assign(1,1,0.0);
  Build();

  for(int j=0;j<int(Inputs->size());j++)
   for(int i=0;i<Inputs[j].GetCols();i++)
   {
    double gain=1;
    if(Gain.GetRows()>int(j) && Gain.GetCols()>i)
     gain=Gain(j,i);
    Output(0,0)+=gain*Inputs[j](0,i);//GetInputData(j)->Double[i];
   }
 }
 return true;
}
// --------------------------
}
#endif
