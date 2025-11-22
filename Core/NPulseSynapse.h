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

#ifndef NPULSE_SYNAPSE_H
#define NPULSE_SYNAPSE_H

#include "NPulseSynapseCommon.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapse: public NPulseSynapseCommon
{
public: //  
///    
UProperty<double,NPulseSynapse, ptPubParameter> SecretionTC;

///    
UProperty<double,NPulseSynapse, ptPubParameter> DissociationTC;

///   
UProperty<double,NPulseSynapse, ptPubParameter> InhibitionCoeff;

///   , 
UProperty<double,NPulseSynapse, ptPubParameter> TypicalPulseDuration;

///         
UProperty<bool,NPulseSynapse, ptPubParameter> UsePulseSignal;

///    
UProperty<bool,NPulseSynapse, ptPubParameter> UsePresynapticInhibition;

public: //   

protected: //  

protected: //  
//          
double VSecretionTC,VDissociationTC;

//    
double OutputConstData;

///          
int PulseCounter;

public: // 
// --------------------------
//   
// --------------------------
NPulseSynapse(void);
virtual ~NPulseSynapse(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//   
bool SetPulseAmplitude(const double &value);

//    
bool SetSecretionTC(const double &value);

//    
bool SetDissociationTC(const double &value);

///   , 
bool SetTypicalPulseDuration(const double &value);

//   
bool SetInhibitionCoeff(const double &value);

///         
bool SetUsePulseSignal(const bool &value);

//     
bool SetUsePresynapticInhibition(const bool &value);

//  ( ) 
bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseSynapse* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

}
#endif
