// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_LTZONE_COMMON_H
#define NPULSE_LTZONE_COMMON_H

#include "NPulseGenerator.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NLTZone: public UNet
{
public: //  
///   
UProperty<double,NLTZone, ptPubParameter> Threshold;

///     
UProperty<double,NLTZone, ptPubParameter> ThresholdOff;

///       
UProperty<bool,NLTZone, ptPubParameter> UseAveragePotential;

public: //   
///      
UProperty<std::vector<MDMatrix<double>>,NLTZone, ptInput | ptPubState> Inputs;

///   
UProperty<MDMatrix<double>, NLTZone, ptOutput | ptPubState> Output;

public: // 
///  ,   
UProperty<double,NLTZone, ptPubState> Potential;


///    
//NPulseNeuron* Neuron;

protected: //  

protected: //  
///       
int CachedNumAConnectors;

public: // 
// --------------------------
//   
// --------------------------
NLTZone(void);
virtual ~NLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
//   
bool SetThreshold(const double &value);

//      
bool SetThresholdOff(const double &value);
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


class RDK_LIB_TYPE NPulseLTZoneCommon: public NLTZone
{
public: //  
///     
/// (     )
///        2  
///    ,    2.  1.
UProperty<int,NPulseLTZoneCommon, ptPubParameter> NumChannelsInGroup;

///  
UProperty<double,NPulseLTZoneCommon, ptPubParameter> PulseAmplitude;

///  
UProperty<double,NPulseLTZoneCommon, ptPubParameter> PulseLength;

///     
UProperty<double,NPulseLTZoneCommon, ptPubParameter> AvgInterval;

public: //   
///  (    PulseOutput) (1)
UProperty<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputPotential;

///  (2)
UProperty<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputFrequency;

///      (3)
UProperty<MDMatrix<double>,NPulseLTZoneCommon, ptOutput | ptPubState> OutputPulseTimes;

public: // 
///   
///         
UProperty<double,NPulseLTZoneCommon,ptPubState> PrePotential;

protected: //  

protected: //  
//  
RDK::UProperty<double,NPulseLTZoneCommon,ptPubState> NeuralPotential;

///   
UProperty<int,NPulseLTZoneCommon,ptPubState> PulseCounter;

///      
UProperty<list<double>,NPulseLTZoneCommon,ptPubState> AvgFrequencyCounter;

///    
UProperty<bool,NPulseLTZoneCommon,ptPubState> PulseFlag;

public: // 
// --------------------------
//   
// --------------------------
NPulseLTZoneCommon(void);
virtual ~NPulseLTZoneCommon(void);
// --------------------------

// --------------------------
//    
// --------------------------
//   
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseLTZoneCommon* New(void);
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
virtual bool ACalculate2(void);

///  true      
virtual bool CheckPulseOn(void);

///  true       
virtual bool CheckPulseOff(void);
// --------------------------
};


}
#endif
