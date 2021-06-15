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

#ifndef NEYEMUSCLE_CPP
#define NEYEMUSCLE_CPP

//#pragma hdrstop

#include "NEyeMuscle.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NEyeMuscle::NEyeMuscle(void)
 : MulCoeffs("MulCoeffs",this),
  K("K",this),
  TC("TC",this),
  Inputs("Inputs",this),
  OutputAcceleration("OutputAcceleration",this),
  OutputLength("OutputLength",this),
  OutputSpeed("OutputSpeed",this)
{
}

NEyeMuscle::~NEyeMuscle(void)
{
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду синуса
bool NEyeMuscle::SetAmplitude(double value)
{
 return true;
}

// Устанавливает частоту
bool NEyeMuscle::SetFrequency(double value)
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
NEyeMuscle* NEyeMuscle::New(void)
{
 return new NEyeMuscle;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NEyeMuscle::ADefault(void)
{
 vector<double> value;
 value.resize(3);
 value[0]=0.74;//0.4;
 value[1]=0.75;//0.12;
 value[2]=0.5;//0.0015;
 MulCoeffs=value;

 K=0.2;

 value.resize(3);
 value[0]=0.003;//1;
 value[1]=0.0092;//1;
 value[2]=0.0327;//1;
 TC=value;
 SetTimeStep(1000);

 OutputAcceleration.Assign(1,1,0.0);
 OutputLength.Assign(1,1,0.0);
 OutputSpeed.Assign(1,1,0.0);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NEyeMuscle::ABuild(void)
{
// int size=(GetNumOutputs()>0)?GetOutputDataSize(0)[1]:0;
 int size=OutputAcceleration.GetCols();
 P1.resize(size);
 P2.resize(size);
 P3.resize(size);
 L.resize(size);

 Threshold.assign(size,0.5);

 return true;
}

// Сброс процесса счета.
bool NEyeMuscle::AReset(void)
{
// int size=(GetNumOutputs()>0)?GetOutputDataSize(0)[1]:0;
 int size=OutputAcceleration.GetCols();
 P1.assign(size,0);
 P2.assign(size,0);
 P3.assign(size,0);
 L.assign(size,0);
 Speed.assign(size,0);
 Acceleration.assign(size,0);

 // Порог
 Threshold.assign(size,0.5);

 return true;
}

// Выполняет расчет этого объекта
bool NEyeMuscle::ACalculate(void)
{
 int k=0;
// SetOutputDataSize(0,MMatrixSize(1,Inputs->size()));
// SetOutputDataSize(1,MMatrixSize(1,Inputs->size()));
// SetOutputDataSize(2,MMatrixSize(1,Inputs->size()));
 OutputAcceleration.Resize(1,int(Inputs->size()));
 OutputLength.Resize(1,int(Inputs->size()));
 OutputSpeed.Resize(1,int(Inputs->size()));

 int size=OutputAcceleration.GetCols();
 for(int i=0;i<int(Inputs->size());i++)
 {
  P1.resize(size,0);
  P2.resize(size,0);
  P3.resize(size,0);
  L.resize(size,0);
  Speed.resize(size,0);
  Acceleration.resize(size,0);
  Threshold.resize(size,0.5);

  for(int j=0;j<Inputs[i]->GetCols();j++)
  {
   if(k >= size)
	break;

   double in=(*Inputs[i])(0,j);
   ThresholdCount(k);
   in*=Threshold[k];

   double leng=MuscularReduction(k,in)*K;
   double speed=(leng-L[k])*TimeStep;
   Acceleration[k]=(speed-Speed[k])*TimeStep;
   Speed[k]=speed;
   L[k]=leng;
   OutputAcceleration(0,k)=Acceleration[k];
   OutputLength(0,k)=L[k];
   OutputSpeed(0,k)=Speed[k];
   ++k;
  }
 }

 return true;
}
// --------------------------


// --------------------------
// Дополнительные скрытые методы управления счетом
// --------------------------
// мускульное сокращение
double NEyeMuscle::MuscularReduction(size_t k,double in)
{
 double p[3]={0,0,0};
 p[0]=P1[k]+(in-P3[k]*MulCoeffs[2]-P1[k]*MulCoeffs[0])/(TC[0]*TimeStep);
 p[1]=P2[k]+(p[0]-P2[k]*MulCoeffs[1])/(TC[1]*TimeStep);
 p[2]=P3[k]+(p[1])/(TC[2]*TimeStep);
 P1[k]=p[0];
 P2[k]=p[1];
 P3[k]=p[2];

 return P3[k];
}


// Пороговая функция
void NEyeMuscle::ThresholdCount(size_t k)
{
 Threshold[k]+=0.4*L[k]-0.005*L[k]*L[k];
 if(Threshold[k]>1.) Threshold[k]=1.;
 if(Threshold[k]<0.) Threshold[k]=0.;
}
// --------------------------
}
//---------------------------------------------------------------------------
#endif
