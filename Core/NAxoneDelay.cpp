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

#ifndef NAXONE_DELAY_CPP
#define NAXONE_DELAY_CPP

#include <cmath>
#include "NAxoneDelay.h"

namespace NMSDK {

NAxoneDelay::NAxoneDelay(void)
 : DelayTime("DelayTime",this,&NAxoneDelay::SetDelayTime)
{
 desired_buffer_length=0;
}

NAxoneDelay::~NAxoneDelay(void)
{
}

bool NAxoneDelay::SetDelayTime(const double &value)
{
 if(value <= 0.0)
  return false;
 desired_buffer_length = int(value*TimeStep);
 buffer.clear();
 return true;
}

NAxoneDelay* NAxoneDelay::New(void)
{
 return new NAxoneDelay;
}

bool NAxoneDelay::CheckComponentType(UEPtr<UContainer> comp) const
{
 (void)comp;
 return false;
}

bool NAxoneDelay::ADefault(void)
{
 DelayTime = 0.001;
 desired_buffer_length = int(DelayTime*TimeStep);
 buffer.clear();
 return NAxoneCommon::ADefault();
}

bool NAxoneDelay::ABuild(void)
{
 desired_buffer_length = int(DelayTime*TimeStep);
 return true;
}

bool NAxoneDelay::AReset(void)
{
 buffer.clear();
 Output.ToZero();
 return NAxoneCommon::AReset();
}

bool NAxoneDelay::ACalculate(void)
{
 // Pass-through only when no delay steps (do not use 0.001 threshold —
 // default DelayTime is 0.001 and must actually delay).
 if(desired_buffer_length==0 || DelayTime <= 0.0)
 {
  *Output=*Input;
  return true;
 }

 int s = int(buffer.size());
 if((s-1)<desired_buffer_length)
 {
  buffer.push_front(Input);
  Output.ToZero();
 }
 else
 {
  std::list<MDMatrix<double> >::iterator lst = buffer.end();
  --lst;
  MDMatrix<double> m = *lst;
  Output = m;
  buffer.pop_back();
  buffer.push_front(Input);
 }
 return true;
}

}
#endif
