// ===========================================================
// Version:        3.0.3
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

#ifndef NSOURCE_H
#define NSOURCE_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NSource: public UNet
{
public: //  
///      ()
/// 0 -  
UProperty<UTime,NSource, ptPubParameter> ActionPeriod;

public: //   
///   
UProperty<MDMatrix<double>, NSource, ptOutput | ptPubState> Output;

protected: //  
UProperty<UTime,NSource, ptPubState> ActionCounter;

public: // 
// --------------------------
//   
// --------------------------
NSource(void);
virtual ~NSource(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//     
bool SetActionPeriod(const RDK::UTime &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSource* New(void);
// --------------------------


// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//   .
virtual bool AReset(void);
// --------------------------
};

}
#endif



