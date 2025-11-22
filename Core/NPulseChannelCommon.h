// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_CHANNEL_COMMON_H
#define NPULSE_CHANNEL_COMMON_H

#include "NPulseSynapse.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseChannelCommon: public UNet
{
public: //  
///   
/// <0 -      (  )
/// >0 -      (  )
UProperty<double,NPulseChannelCommon, ptPubParameter> Type;

///       
UProperty<bool, NPulseChannelCommon, ptPubParameter> UseAveragePotential;

///      
UProperty<bool, NPulseChannelCommon, ptPubParameter> UseAverageSynapsis;

public: //   
///       
UProperty<std::vector<MDMatrix<double>>, NPulseChannelCommon, ptInput | ptPubState> ChannelInputs;

///    
UProperty<std::vector<MDMatrix<double>>, NPulseChannelCommon, ptInput | ptPubState> SynapticInputs;

///     
UProperty<MDMatrix<double>,NPulseChannelCommon, ptOutput | ptPubState> Output;

///         
/// (, ..  )
UProperty<MDMatrix<double>,NPulseChannelCommon, ptOutput | ptPubState> SumChannelInput;

///   
UProperty<MDMatrix<double>, NPulseChannelCommon, ptOutput | ptPubState> SumSynapticInput;

public: //  
///   - (  
///     ).
UProperty<bool,NPulseChannelCommon, ptPubState> IsNeuronActivated;


public: // 
// --------------------------
//   
// --------------------------
NPulseChannelCommon(void);
virtual ~NPulseChannelCommon(void);
// --------------------------

// --------------------------
//    
// --------------------------
//   
virtual int GetNumSynapses(void) const;
// --------------------------

// --------------------------
//    
// --------------------------
//   
bool SetType(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseChannelCommon* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
//       
//     
//   'true'   
//  'false'    
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//    
//       
//      comp 
//     
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

//    
//       
//      comp
//    
virtual bool ADelComponent(UEPtr<UContainer> comp);
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

//   .
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
