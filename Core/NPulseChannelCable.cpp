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

#ifndef NMSDK_NCableModelCPP
#define NMSDK_NCableModelCPP

#include "NPulseChannelCable.h"
#include "../../Nmsdk-PulseLib/Core/NPulseMembrane.h"
#include <math.h>
#define _USE_MATH_DEFINES

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelCable::NPulseChannelCable(void)
 :
   //model parameters
   EL("EL", this, &NPulseChannelCable::SetEL),
   Ri("Ri", this, &NPulseChannelCable::SetRi),
   CableMembraneResistance("CableMembraneResistance", this, &NPulseChannelCable::SetCableMembraneResistance),
   D("D", this, &NPulseChannelCable::SetD),
   Rm("Rm", this, &NPulseChannelCable::SetRm),
   Cm("Cm", this, &NPulseChannelCable::SetCm),

   //boundaries and number of grid points
   ModelMaxLength("ModelMaxLength", this, &NPulseChannelCable::SetModelMaxLength),
   ModelMaxTime("ModelMaxTime", this, &NPulseChannelCable::SetModelMaxTime),
   dx("dx", this, &NPulseChannelCable::Setdx),
   dt("dt", this, &NPulseChannelCable::Setdt),
   CalcMode("CalcMode", this, &NPulseChannelCable::SetCalcMode),

   TauM("TauM", this),
   Vm("Vm", this),
   InpV("InpV", this)
{
}

NPulseChannelCable::~NPulseChannelCable(void)
{
}
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Потенциал покоя
bool NPulseChannelCable::SetEL(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::SetRi(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::SetCableMembraneResistance(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::SetD(const double &value)
{
    Ready = false;
    return true;
}

/// Сопротивление мембраны
bool NPulseChannelCable::SetRm(const double &value)
{
    Ready = false;
    return true;
}

/// Ёмкость мембраны
bool NPulseChannelCable::SetCm(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::SetModelMaxLength(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::SetModelMaxTime(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::Setdx(const double &value)
{
    Ready = false;
    return true;
}

///
bool NPulseChannelCable::Setdt(const double &value)
{
    Ready = false;
    return true;
}

bool NPulseChannelCable::SetCalcMode(const bool &value)
{
    if (value == false)
    {
        ChangeLookupPropertyType("CableMembraneResistance", ptPubParameter);
        ChangeLookupPropertyType("D", ptPubState);
        CableMembraneResistance =  (Rm)/(M_PI*D);
        TauM = CableMembraneResistance*Cm;
    }
    else
    {
        ChangeLookupPropertyType("CableMembraneResistance", ptPubState);
        ChangeLookupPropertyType("D", ptPubParameter);
        D = Rm/(M_PI*CableMembraneResistance);
        TauM = CableMembraneResistance*Cm;
    }

    Ready = false;
    return true;
}


// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseChannelCable* NPulseChannelCable::New(void)
{
 return new NPulseChannelCable;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseChannelCable::ADefault(void)
{
 if(!NPulseChannelClassic::ADefault())
     return false;
 //Model parameters
 CalcMode = false;
 EL = -0.07; //mV - The Rest potential
 Ri = 100000; // Ohm*m2
 Rm = 1000; // 1 kOm
 Cm = 1.0e-9; // F
 //D = 0.00002;
 CableMembraneResistance = 1e7;
 D = Rm/(M_PI*CableMembraneResistance);

// CableMembraneResistance =  Rm/(M_PI*D);
 TauM = CableMembraneResistance*Cm;

 //Boundaries and number of grid points
 ModelMaxLength = 0.00027; // meters
 ModelMaxTime = 1.0/TimeStep; // seconds
 dx = 1e-5; //m
 dt = 1.0e-7; //sec

 t_points_number = int(ModelMaxTime/dt); //t
 x_points_number = int(ModelMaxLength/dx); //x

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseChannelCable::ABuild(void)
{
 if(!NPulseChannelClassic::ABuild())
  return false;

 if(CalcMode)
 {
    ChangeLookupPropertyType("CableMembraneResistance", ptPubState);
    ChangeLookupPropertyType("D", ptPubParameter);
    CableMembraneResistance =  Rm/(M_PI*D);
    TauM = CableMembraneResistance*Cm;
 }
 else
 {
     ChangeLookupPropertyType("CableMembraneResistance", ptPubParameter);
     ChangeLookupPropertyType("D", ptPubState);
     D = Rm/(M_PI*CableMembraneResistance);
     TauM = CableMembraneResistance*Cm;
 }

  ModelMaxTime = 1.0/TimeStep; // seconds

  double tau_m = TauM.v;
  double ri = (4*Ri)/(M_PI*D*D);
  double rm = CableMembraneResistance.v;
  double lambdaSq = rm/ri;
  dt = (dx*dx)/(4*lambdaSq/tau_m);

 //Создание сетки
   t_points_number = int(ModelMaxTime/dt); //t
   x_points_number = int(ModelMaxLength/dx); //x

   x.Resize(1, x_points_number);
   t.Resize(1, t_points_number);

   for (int k = 0; k < x_points_number; k++)
   {
       x(0, k) = double(dx*k);
   }

   for (int k =0; k < t_points_number; k++)
   {
       t(0, k) = double(dt*k);
   }

 return true;
}

// Сброс процесса счета без потери настроек
bool NPulseChannelCable::AReset(void)
{
 if(!NPulseChannelClassic::ABuild())
  return false;

 SumChannelInput->Assign(1,1,EL); // TODO: костыль

 //Задание начальных значений в матрице
 Vm.Assign(t_points_number+1, x_points_number+1, SumChannelInput(0,0));

 return true;
}


//Формирует входной потенциал в виде функции синуса
void NPulseChannelCable::FormingInput()
{
    InpV.Resize(1, t_points_number+1, 0);

    // Получение информации об обратной связи
    UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(Owner);
    if(membrane)
     SumChannelInput(0,0)-=membrane->Feedback;

    for (int j = 0; j < (t_points_number+1); j++) //t
    {
     InpV(0,j) = SumChannelInput(0,0)+SumSynapticInput(0,0)/Cm.v;
    }
 return;
}

// Выполняет расчет этого объекта
bool NPulseChannelCable::ACalculate2(void)
{
    FormingInput();

    //setting the input
    inpX = 0;

    for (int j = 0; j < (t_points_number+1); j++) //t
    {
        Vm(j, inpX) = InpV(0, j);
    }

    double tau_m = TauM.v;

    double ri = (4*Ri)/(M_PI*D*D);
    double rm = CableMembraneResistance.v;
    double lambdaSq = rm/ri;
    double dt_tau_m = dt/tau_m;
    double s = lambdaSq*dt_tau_m/(dx*dx);
    double s1 = (1.0 - (dt/tau_m) - 2*s);
    double el = SumChannelInput(0,0)*dt/tau_m;

    for (int j = 0; j < (t_points_number); j++) //t
    {
        double* vm_curr_line = &Vm.v(j,0);
        double* vm_new_line = &Vm.v(j+1,0);
        for (int i = 1; i < (x_points_number); i++) //x
        {
//             Vm.v(j+1,i) = el + s*Vm.v(j,i+1) + s1*Vm.v(j,i) + s*Vm.v(j,i-1);
             *(vm_new_line+i) = el + s*(*(vm_curr_line+i+1)) + s1*(*(vm_curr_line+i)) + s*(*(vm_curr_line+i-1));
        }
    }

    //Проверка на inf
    if(isnan(Vm(t_points_number, x_points_number-1)) || isinf(Vm(t_points_number, x_points_number-1)) || isinf(-Vm(t_points_number, x_points_number-1)))
    {
        Output(0,0) = -1;
    }
    else
    {
        Output(0,0) = Vm(t_points_number, x_points_number-1);
    }

 return true;
}
// --------------------------

}
#endif



