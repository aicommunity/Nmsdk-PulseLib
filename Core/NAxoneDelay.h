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

#ifndef NAXONE_DELAY_H
#define NAXONE_DELAY_H

#include "NAxoneCommon.h"
#include <list>

namespace NMSDK {

/// Phenomenological axon: delays Input by DelayTime seconds (default 1 ms).
/// Distinct from NPDelay / NPulseDelay; part of the axon hierarchy.
class RDK_LIB_TYPE NAxoneDelay: public NAxoneCommon
{
public:
/// Delay duration [s], default 0.001
UProperty<double,NAxoneDelay, ptPubParameter> DelayTime;

protected:
std::list<MDMatrix<double> > buffer;
int desired_buffer_length;

public:
NAxoneDelay(void);
virtual ~NAxoneDelay(void);

protected:
bool SetDelayTime(const double &value);

public:
virtual NAxoneDelay* New(void);
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);
};

}
#endif
