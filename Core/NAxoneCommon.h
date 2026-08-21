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

#ifndef NAXONE_COMMON_H
#define NAXONE_COMMON_H

#include "NSource.h"

namespace NMSDK {

/// Base class for axon transmission components.
/// Translates a neuron LTZone-like pulse signal further along the network.
class RDK_LIB_TYPE NAxoneCommon: public NSource
{
public:
/// Axon input (typically linked from neuron LTZone Output), default 1x1
UProperty<MDMatrix<double>,NAxoneCommon, ptInput | ptPubState> Input;

public:
// --------------------------
// Constructors / destructor
// --------------------------
NAxoneCommon(void);
virtual ~NAxoneCommon(void);
// --------------------------

public:
virtual NAxoneCommon* New(void);
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);
};

}
#endif
