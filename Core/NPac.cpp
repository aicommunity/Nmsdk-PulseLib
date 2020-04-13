// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */
#ifndef NPAC_CPP
#define NPAC_CPP

#include "NPac.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPac::NPac(void)
: PulseAmplitude("PulseAmplitude",this,&NPac::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NPac::SetSecretionTC),
DissociationTC("DissociationTC",this,&NPac::SetDissociationTC),
Gain("Gain",this,&NPac::SetGain),
Mode("Mode",this),
TCMode("TCMode",this),
Inputs("Inputs",this),
Output("Output",this),
PreOutput("PreOutput",this)
{
}

NPac::~NPac(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NPac::SetPulseAmplitude(const vector<vector<double> > &value)
{
 return true;
}

// Постоянная времени выделения медиатора
bool NPac::SetSecretionTC(const vector<vector<double> > &value)
{
 return true;
}

// Постоянная времени распада медиатора
bool NPac::SetDissociationTC(const vector<vector<double> > &value)
{
 return true;
}

// Усиление

bool NPac::SetGain(const vector<vector<double> > &value)
{
 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPac* NPac::New(void)
{
 return new NPac;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPac::ADefault(void)
{
 // Начальные значения всем параметрам
 // Амплитуда входных импульсов
 vector<vector<double> > values;

 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,1);
 PulseAmplitude=values;

 // Постоянная времени выделения медиатора
 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,0.05);
 SecretionTC=values;

 // Постоянная времени распада медиатора
 values.resize(4);
 for(size_t i=0;i<values.size();i++)
  values[i].assign(2,0.5);
 DissociationTC=values;

 // Усиление
 values.resize(4);
 values[0].resize(2);
 values[0][0]=30;
 values[0][1]=30;
 values[1].resize(2);
 values[1][0]=30;
 values[1][1]=30;
 values[2].resize(2);
 values[2][0]=-30;
 values[2][1]=-30;
 values[3].resize(2);
 values[3][0]=-30;
 values[3][1]=-30;
 Gain=values;

 Mode=0;
 TCMode=1;

 Output->Assign(1,1,0.0);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPac::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NPac::AReset(void)
{
 // Сброс временных переменных
 PreOutput->resize(0);

 return true;
}

// Выполняет расчет этого объекта
bool NPac::ACalculate(void)
{
 int min_input_data_size(0);
 if(Inputs->size()>0)
 {
  min_input_data_size=Inputs[0]->GetCols();
  for(int i=1;i<NumInputs;i++)
   if(min_input_data_size>Inputs[i]->GetCols())
	min_input_data_size=Inputs[i]->GetCols();
 }

 int size=(int(min_input_data_size)<Output.GetCols())?int(min_input_data_size):Output.GetCols();
 double Ts;
 double input;

 Output.Resize(1,size);

 PreOutput->resize(Inputs->size());
 for(size_t i=0;i<Inputs->size();i++)
  PreOutput[i].resize(size);

 if(TCMode == 0)
 {
  for(size_t i=0;i<Inputs->size();i++)
  {
   for(int j=0;j<size;j++)
   {
	if(!Inputs[i])
	 continue;
	input=(*Inputs[i])(0,j);

	PreOutput[i][j]=input/PulseAmplitude[i][j];
   }
  }
 }
 else
 if(TCMode == 1)
 {
  for(size_t i=0;i<Inputs->size();i++)
  {
   for(int j=0;j<size;j++)
   {
	if(!Inputs[i])
	 continue;
	input=(*Inputs[i])(0,j);

	Ts=(fabs(input)>0)?SecretionTC[i][j]:DissociationTC[i][j];
	PreOutput[i][j]+=(input/PulseAmplitude[i][j]-PreOutput[i][j])/(Ts*TimeStep);
   }
  }
 }
  for(int j=0;j<size;j++)
   Output(0,j)=0;

 if(Mode == 0)
 {
  for(int i=0;i<Inputs->size();i++)
  {
   for(int j=0;j<size;j++)
   {
	Output(0,j)+=PreOutput[i][j]*Gain[i][j];
   }
  }
 }
 else
 if(Mode == 1)
 {
  for(int j=0;j<size;j++)
  {
   double pos_values=0;
   double neg_values=0;
   for(size_t i=0;i<Inputs->size();i++)
   {
	if(PreOutput[i][j]*Gain[i][j]>0)
	 pos_values+=PreOutput[i][j]*Gain[i][j];
	if(PreOutput[i][j]*Gain[i][j]<0)
	 neg_values+=PreOutput[i][j]*Gain[i][j];
   }
   if(pos_values>1e-5 && neg_values<-1e-5)
   {
	for(size_t i=0;i<Inputs->size();i++)
	 PreOutput[i][j]=0;

	Output(0,j)=0.0;
   }
   else
    Output(0,j)=pos_values+neg_values;
  }
 }

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NCPac* NCPac::New(void)
{
 return new NCPac;
}
// --------------------------
}
#endif
