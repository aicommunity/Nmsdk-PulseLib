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

#ifndef NAPERIODIC_LINK_CPP
#define NAPERIODIC_LINK_CPP

#include <cmath>
#include "NAperiodicLink.h"

namespace NMSDK {

NAperiodicLink::NAperiodicLink(void)
 : Tau("Tau",this,&NAperiodicLink::SetTau),
   Bias("Bias",this,&NAperiodicLink::SetBias),
   Input("Input",this),
   Output("Output",this)
{
}

NAperiodicLink::~NAperiodicLink(void)
{
}

bool NAperiodicLink::SetTau(const double &value)
{
 if(value < 0.0)
  return false;
 return true;
}

bool NAperiodicLink::SetBias(const double &value)
{
 (void)value;
 return true;
}

NAperiodicLink* NAperiodicLink::New(void)
{
 return new NAperiodicLink;
}

bool NAperiodicLink::CheckComponentType(UEPtr<UContainer> comp) const
{
 (void)comp;
 return false;
}

bool NAperiodicLink::ADefault(void)
{
 Tau=0.001;
 Bias=-1.0;
 Input.Assign(1,1,0.0);
 Output.Assign(1,1,0.0);
 filter_state.Assign(1,1,0.0);
 return true;
}

bool NAperiodicLink::ABuild(void)
{
 return true;
}

bool NAperiodicLink::AReset(void)
{
 filter_state.ToZero();
 Output.ToZero();
 // Keep Output consistent with Bias after reset of filter state
 {
  MDMatrix<double> out = filter_state;
  for(int r=0;r<out.GetRows();++r)
   for(int c=0;c<out.GetCols();++c)
    out(r,c) = out(r,c) + Bias;
  Output = out;
 }
 return true;
}

bool NAperiodicLink::ACalculate(void)
{
 MDMatrix<double> x = *Input;
 if(filter_state.GetRows()!=x.GetRows() || filter_state.GetCols()!=x.GetCols())
  filter_state.Assign(x.GetRows(), x.GetCols(), 0.0);

 if(Tau <= 0.0 || TimeStep <= 0.0 || fabs(Tau * TimeStep) < 1e-30)
 {
  filter_state = x;
 }
 else
 {
  const double denom = Tau * TimeStep;
  for(int r=0;r<x.GetRows();++r)
   for(int c=0;c<x.GetCols();++c)
    filter_state(r,c) = filter_state(r,c) + (x(r,c) - filter_state(r,c)) / denom;
 }

 MDMatrix<double> out = filter_state;
 for(int r=0;r<out.GetRows();++r)
  for(int c=0;c<out.GetCols();++c)
   out(r,c) = out(r,c) + Bias;
 Output = out;
 return true;
}

}
#endif
