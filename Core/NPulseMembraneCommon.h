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

#ifndef NPULSE_MEMBRANE_COMMON_H
#define NPULSE_MEMBRANE_COMMON_H

#include "NPulseChannelCommon.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseMembraneCommon: public UNet
{
public: // 
///       
UProperty<bool, NPulseMembraneCommon, ptPubParameter> UseAveragePotential;

public: // 
///   
UProperty<double,NPulseMembraneCommon,ptPubState> Feedback;

///      
UProperty<MDMatrix<double>, NPulseMembraneCommon, ptPubState> SumPotential;


protected: //  
//  
vector<NPulseChannelCommon*> Channels;

// 
vector<NPulseSynapseCommon*> Synapses;

/// ,       
bool IsNeuronActive;

public: // 
// --------------------------
//   
// --------------------------
NPulseMembraneCommon(void);
virtual ~NPulseMembraneCommon(void);
// --------------------------

// --------------------------
//   
// --------------------------
///       
bool SetUseAveragePotential(const bool &value);
// --------------------------

// --------------------------
//    
// --------------------------
//  
size_t GetNumChannels(void) const;
NPulseChannelCommon* GetChannel(size_t i);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseMembraneCommon* New(void);
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
public:
virtual void RebuildSynapticInternalLinks(void);
};

}
#endif

