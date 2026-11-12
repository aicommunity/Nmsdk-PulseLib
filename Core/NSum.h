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

#ifndef NSUM_H
#define NSUM_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NSum: public UNet
{
public: //  
///  
/// 0 -      ,
///    .
/// 1 -        
UProperty<int,NSum, ptPubParameter> Mode;

///  
UProperty<MDMatrix<double>, NSum, ptPubParameter> Gain;

public: //   
///   
UProperty<std::vector<MDMatrix<double>>, NSum, ptInput | ptPubState> Inputs;

///   
UProperty<MDMatrix<double>, NSum, ptOutput | ptPubState> Output;

public: // 
// --------------------------
//   
// --------------------------
NSum(void);
virtual ~NSum(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
// 
bool SetGain(const MDMatrix<double> &value);

//  
bool SetMode(const int &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NSum* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
//   
//    Reset()   Ready  true
//    
virtual bool ABuild(void);

//   .
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
