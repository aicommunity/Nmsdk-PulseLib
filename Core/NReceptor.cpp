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

#ifndef NRECEPTOR_CPP
#define NRECEPTOR_CPP

#include "NReceptor.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NReceptor::NReceptor(void)
//: NSource(name),
 : Gain("Gain",this,&NReceptor::SetGain),
  MinInputRange("MinInputRange",this),
  MaxInputRange("MaxInputRange",this),
  MinOutputRange("MinOutputRange",this),
  MaxOutputRange("MaxOutputRange",this),
  InputAdaptationMode("InputAdaptationMode",this),
  ExpCoeff("ExpCoeff",this),
  OutputAdaptationMode("OutputAdaptationMode",this),
  SumCoeff("SumCoeff",this),
  InputAdaptationArrestingTC("InputAdaptationArrestingTC",this)
{
}

NReceptor::~NReceptor(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ����������� �������� �������� �������
bool NReceptor::SetGain(const double &value)
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
NReceptor* NReceptor::New(void)
{
 return new NReceptor;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NReceptor::ADefault(void)
{
 Gain=1;
 MinOutputRange=0.0;
 MaxOutputRange=0.01;//0.1;
 MinInputRange=0;
 MaxInputRange=10;
 InputAdaptationMode=1;
 OutputAdaptationMode=1;
 ExpCoeff=1;
 SumCoeff=2;
 InputAdaptationArrestingTC=1;
 return NSource::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NReceptor::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NReceptor::AReset(void)
{
 return NSource::AReset();
}

// ��������� ������ ����� �������
bool NReceptor::ACalculate(void)
{
 OutputRange=MaxOutputRange.v-MinOutputRange.v;

 size_t k=0;
 for(int i=0;i<NumInputs;i++)
  for(size_t j=0;j<GetOutputDataSize(i)[1];j++)
  {
   double input=0;
   if(GetInputDataSize(i)[1]>j)
    input=GetInputData(i)->Double[j];
//   if(k >= GetOutputDataSize(0))
//	break;

   switch(InputAdaptationMode.v)
   {
   case 1:
	if(input<MinInputRange.v)
	 MinInputRange=input;
	if(input>MaxInputRange.v)
	 MaxInputRange=input;
   break;

   case 2:
	if(input<MinInputRange.v)
	 MinInputRange=input;
	else // ���������
	{
	 MinInputRange.v-=MinInputRange.v/(InputAdaptationArrestingTC*TimeStep);
	}

	if(input>MaxInputRange.v)
	 MaxInputRange=input;
	else // ���������
	{
     MaxInputRange.v-=MaxInputRange.v/(InputAdaptationArrestingTC*TimeStep);
	}
   break;
   }

   InputRange=MaxInputRange.v-MinInputRange.v;
   if(!InputRange)
	return true;

   // ��������������� ������ � ��������� ���������
   input=(input-MinInputRange.v)*OutputRange/InputRange+MinOutputRange.v;

   double exp_coeff;
   switch(OutputAdaptationMode.v)
   {
   case 0:
	POutputData[i].Double[k]=Gain*input;
   break;

   case 1:
	// ��������������� ������ � ��������� ���������
	POutputData[i].Double[k]=Gain*(SumCoeff.v-exp(-ExpCoeff.v*input));
   break;

   case 2:
	// ��������������� ������ � ��������� ���������
	POutputData[i].Double[k]=Gain*exp(-ExpCoeff.v*input);
   break;

   case 3:
	// ��������������� ������ � �������������
	exp_coeff=-log(0.9)/InputRange;
	POutputData[i].Double[k]=Gain*(SumCoeff.v-exp(-exp_coeff*input));
   break;

   case 4:
	POutputData[i].Double[k]=Gain*(1.0+input);
   break;
   }

//   if(static_pointer_cast<UContainer>(Owner)->GetName() == "Afferent_II1")
//    POutputData[i].Double[k]=1.5;
   ++k;
  }
 return NSource::ACalculate();
}
// --------------------------
}
#endif
