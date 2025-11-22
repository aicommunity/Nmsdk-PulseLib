#ifndef NMSDK_NSYNAPSECLASSIC_H
#define NMSDK_NSYNAPSECLASSIC_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseClassic: public NPulseSynapseCommon
{
public: // 
///   
UProperty<double, NSynapseClassic, ptPubParameter> TauS;

///   
UProperty<double, NSynapseClassic, ptPubParameter> Current;

public: //   

public: // 
// --------------------------
//   
// --------------------------
NSynapseClassic(void);
virtual ~NSynapseClassic(void);
// --------------------------

// ---------------------
//   
// ---------------------
// ---------------------

// ---------------------
//    
// ---------------------
// ---------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseClassic* New(void);
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

//      
virtual bool AReset(void);

//    
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



