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

#ifndef NPULSE_NEURON_COMMON_H
#define NPULSE_NEURON_COMMON_H

#include "NNeuron.h"
#include "NPulseLTZoneCommon.h"
#include "NConstGenerator.h"
#include "NPulseMembraneCommon.h"
//#include "NAfferentNeuron.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseNeuronCommon: public NNeuron
{
public: // 
///      
UProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageDendritesPotential;

///      
UProperty<bool, NPulseNeuronCommon, ptPubParameter> UseAverageLTZonePotential;

public: //   
///  
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> Output;

///        ( )
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActiveOutputs;

///        
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActivePosInputs;

///        
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> ActiveNegInputs;

///     
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> DendriticSumPotential;

///     
UProperty<MDMatrix<double>,NPulseNeuronCommon,ptOutput | ptPubState> SomaSumPotential;

public: // 
///        ( )
UProperty<double,NPulseNeuronCommon,ptPubState> NumActiveOutputs;

///        
UProperty<double,NPulseNeuronCommon,ptPubState> NumActivePosInputs;

///        
UProperty<double,NPulseNeuronCommon,ptPubState> NumActiveNegInputs;

public: //  
UEPtr<NLTZone> LTZone;

vector<NPulseMembraneCommon*> Membranes;

public: // 
// --------------------------
//   
// --------------------------
NPulseNeuronCommon(void);
virtual ~NPulseNeuronCommon(void);
// --------------------------

// --------------------------
//   
// --------------------------
///       
bool SetUseAverageDendritesPotential(const bool &value);

///       
bool SetUseAverageLTZonePotential(const bool &value);
// --------------------------

// --------------------------
//     
// --------------------------
//      
NLTZone* GetLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
////    ,      ,
////        
////     
//virtual NPulseMembraneCommon* ElongateDendrite(const std::string &name, bool feedback=false);

////    ,     
////    
////     
//virtual NPulseMembraneCommon* BranchDendrite(const std::string &name, bool feedback=false);

////    
////  full == true,      ,   
////        
//virtual bool EraseDendrite(const std::string &name);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseNeuronCommon* New(void);
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
