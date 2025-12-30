#ifndef NMSDK_NPulseChannelIzhikevichH
#define NMSDK_NPulseChannelIzhikevichH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIzhikevich: public NPulseChannelClassic
{
public: // 
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> A;

UProperty<double, NPulseChannelIzhikevich, ptPubParameter> B;

/// The after-spike reset value of the membrane potential v
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> C;

/// The after-spike reset of the recovery variable u caused by slowhigh-threshold Na+ and K+ conductances.
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> D;

public: //   

///
UProperty<MDMatrix<double>,NPulseChannelIzhikevich, ptOutput | ptPubState> OutputU;

protected: //  
UProperty<double, NPulseChannelIzhikevich, ptPubState> V;
UProperty<double, NPulseChannelIzhikevich, ptPubState> U;


public: // 
// --------------------------
//   
// --------------------------
NPulseChannelIzhikevich(void);
virtual ~NPulseChannelIzhikevich(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseChannelIzhikevich* New(void);
// --------------------------        

// --------------------------
//     
// --------------------------
public:
///       
/// (   )
virtual void NeuronActivated(void);

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



