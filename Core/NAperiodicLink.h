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

#ifndef NAPERIODIC_LINK_H
#define NAPERIODIC_LINK_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

/// First-order aperiodic (inertial) link: y' = (x - y) / Tau; Output = y + Bias.
/// Required at the input of regenerative axon segments (feeds ExcChannel).
class RDK_LIB_TYPE NAperiodicLink: public UNet
{
public:
/// Time constant [s]
UProperty<double,NAperiodicLink, ptPubParameter> Tau;

/// Constant offset added to filtered state before Output (default -1)
UProperty<double,NAperiodicLink, ptPubParameter> Bias;

/// Input signal (1x1 by default)
UProperty<MDMatrix<double>,NAperiodicLink, ptInput | ptPubState> Input;

/// Output = filtered_y + Bias (1x1 by default)
UProperty<MDMatrix<double>,NAperiodicLink, ptOutput | ptPubState> Output;

public:
NAperiodicLink(void);
virtual ~NAperiodicLink(void);

protected:
bool SetTau(const double &value);
bool SetBias(const double &value);

public:
virtual NAperiodicLink* New(void);
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);

/// Internal filter state (without Bias); Output = state + Bias
MDMatrix<double> filter_state;
};

}
#endif
