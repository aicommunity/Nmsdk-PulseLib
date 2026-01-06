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

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NReceptor::NReceptor(void)
//: NSource(name),
 : Gain("Gain",this,&NReceptor::SetGain),
  MinInputRange("MinInputRange",this),
  MaxInputRange("MaxInputRange",this),
  MinOutputRange("MinOutputRange",this),
  MaxOutputRange("MaxOutputRange",this),
  ExpCoeff("ExpCoeff",this),
  SumCoeff("SumCoeff",this),
  InputAdaptationArrestingTC("InputAdaptationArrestingTC",this),
  InputAdaptationMode("InputAdaptationMode",this),
  OutputAdaptationMode("OutputAdaptationMode",this),
  Input("Input",this)
{
 InputRange=0;
 OutputRange=0;
}

NReceptor::~NReceptor(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Коэффициент усиления входного сигнала
bool NReceptor::SetGain(const double &value)
{
 if(value <=0)
  return false;

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NReceptor* NReceptor::New(void)
{
 return new NReceptor;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NReceptor::ADefault(void)
{
 Gain=1;
 MinOutputRange=0.0;
 MaxOutputRange=1;//0.01;//0.1;
 MinInputRange=0;
 MaxInputRange=1;
 InputAdaptationMode=0;
 OutputAdaptationMode=5;
 ExpCoeff=0.1;
 SumCoeff=1;
 InputAdaptationArrestingTC=1;
 return NSource::ADefault();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NReceptor::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NReceptor::AReset(void)
{
 return NSource::AReset();
}

// Выполняет расчет этого объекта
bool NReceptor::ACalculate(void)
{
 const double maxOutputRange = MaxOutputRange.GetData();
 const double minOutputRange = MinOutputRange.GetData();
 OutputRange = maxOutputRange - minOutputRange;

 Output.Resize(Input->GetRows(),Input->GetCols());

 for(int i=0;i<Input->GetRows();i++)
  for(int j=0;j<Input->GetCols();j++)
  {
   double input=0;
   input=(*Input)(i,j);// GetInputData(i)->Double[j];

   switch(InputAdaptationMode.GetData())
   {
   case 1:
	if(input<MinInputRange.GetData())
	 MinInputRange=input;
	if(input>MaxInputRange.GetData())
	 MaxInputRange=input;
   break;

   case 2:
	{
	 const double minInputRange = MinInputRange.GetData();
	 if(input<minInputRange)
	  MinInputRange=input;
	 else // забывание
	 {
	  MinInputRange = minInputRange - minInputRange/(InputAdaptationArrestingTC*TimeStep);
	 }

	 const double maxInputRange = MaxInputRange.GetData();
	 if(input>maxInputRange)
	  MaxInputRange=input;
	 else // забывание
	 {
      const double maxInputRangeVal = MaxInputRange.GetData();
      MaxInputRange = maxInputRangeVal - maxInputRangeVal/(InputAdaptationArrestingTC*TimeStep);
	 }
	}
   break;
   }

   const double maxInputRangeFinal = MaxInputRange.GetData();
   const double minInputRangeFinal = MinInputRange.GetData();
   InputRange = maxInputRangeFinal - minInputRangeFinal;
   if(!InputRange)
	return true;

   // Преобразовываем сигнал к заданному диапазону
   input=(input-MinInputRange.GetData())*OutputRange/InputRange+MinOutputRange.GetData();

   double exp_coeff;
   double sumCoeffVal = SumCoeff.GetData();
   double expCoeffVal = ExpCoeff.GetData();
   switch(OutputAdaptationMode.GetData())
   {
   case 0:
	Output(i,j)=Gain*input;
   break;

   case 1:
	// Преобразовываем сигнал к заданному диапазону
	Output(i,j)=Gain*(sumCoeffVal-exp(-expCoeffVal*input));
   break;

   case 2:
	// Преобразовываем сигнал к заданному диапазону
	Output(i,j)=Gain*exp(-expCoeffVal*input);
   break;

   case 3:
	// Преобразовываем сигнал к автодиапазону
	exp_coeff=-log(0.9)/InputRange;
	Output(i,j)=Gain*(sumCoeffVal-exp(-exp_coeff*input));
   break;

   case 4:
	Output(i,j)=Gain*(sumCoeffVal+input);
   break;

   case 5:
	Output(i,j)=Gain*(sumCoeffVal+(1.0-exp(-expCoeffVal*input)));
   break;
   }
  }
 return NSource::ACalculate();
}
// --------------------------
}
#endif
