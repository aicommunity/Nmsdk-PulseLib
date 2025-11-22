#ifndef NMSDK_NPulseChannelIaFH
#define NMSDK_NPulseChannelIaFH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIaF: public NPulseChannelClassic
{
public: // 
///  
UProperty<double, NPulseChannelIaF, ptPubParameter> Cm;

///  
UProperty<double, NPulseChannelIaF, ptPubParameter> EL;

///   
UProperty<double, NPulseChannelIaF, ptPubParameter> TauM;

///  
UProperty<double, NPulseChannelIaF, ptPubParameter> TRef;

///  
UProperty<double, NPulseChannelIaF, ptPubParameter> VReset;

///    
UProperty<double, NPulseChannelIaF, ptPubParameter> VMin;

///       
UProperty<bool, NPulseChannelIaF, ptPubParameter> VResetEnable;

public: //   

protected: //  
///  
///   
UProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Vm;

///  
UProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Rm;

public: // 
// --------------------------
//   
// --------------------------
NPulseChannelIaF(void);
virtual ~NPulseChannelIaF(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseChannelIaF* New(void);
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



