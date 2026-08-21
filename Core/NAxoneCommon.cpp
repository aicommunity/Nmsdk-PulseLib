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

#ifndef NAXONE_COMMON_CPP
#define NAXONE_COMMON_CPP

#include "NAxoneCommon.h"

namespace NMSDK {

NAxoneCommon::NAxoneCommon(void)
 : Input("Input",this)
{
}

NAxoneCommon::~NAxoneCommon(void)
{
}

NAxoneCommon* NAxoneCommon::New(void)
{
 return new NAxoneCommon;
}

bool NAxoneCommon::CheckComponentType(UEPtr<UContainer> comp) const
{
 (void)comp;
 return false;
}

bool NAxoneCommon::ADefault(void)
{
 Input.Assign(1,1,0.0);
 Output.Assign(1,1,0.0);
 return NSource::ADefault();
}

bool NAxoneCommon::ABuild(void)
{
 return true;
}

bool NAxoneCommon::AReset(void)
{
 Output.ToZero();
 return NSource::AReset();
}

bool NAxoneCommon::ACalculate(void)
{
 *Output=*Input;
 return true;
}

}
#endif
