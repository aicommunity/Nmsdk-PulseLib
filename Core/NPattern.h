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

#ifndef NPATTERN_H
#define NPATTERN_H

#include "NSource.h"
#include "NPulseGenerator.h"

namespace NMSDK {

class RDK_LIB_TYPE NPattern: public NSource
{
public: // 
///  ,   
UProperty<std::string, NPattern, ptPubParameter> PulseGeneratorClassName;

///   
UProperty<int,NPattern, ptPubParameter> NumGenerators;

///   .      
///  ( ),  ,     .
UProperty<MDMatrix<double>, NPattern, ptPubParameter> InputPattern;

///   () ( )
UProperty<double,NPattern, ptPubParameter> Frequency;

///   ()
UProperty<double,NPattern, ptPubParameter> PulseLength;

///  
UProperty<double,NPattern, ptPubParameter> Amplitude;

///   
UProperty<double,NPattern, ptPubParameter> Delay;

///  
/// ( ,    !)
UProperty<double,NPattern, ptPubParameter> FrequencyDeviation;

///       
UProperty<bool,NPattern, ptPubParameter> UseTransitSignal;


public: //   


public: //  


protected:

double OldNumGenerators;

//  
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;

public: // 
// --------------------------
//   
// --------------------------
NPattern(void);
virtual ~NPattern(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
///  ,   
bool SetPulseGeneratorClassName(const std::string &value);

//    
bool SetNumGenerators(const int &value);

///    
bool SetInputPattern(const MDMatrix<double> &value);

//   
bool SetFrequency(const double &value);

//   
bool SetPulseLength(const double &value);

//   
bool SetAmplitude(const double &value);

//    
bool SetDelay(const double &value);

//    
// ( ,    !)
bool SetFrequencyDeviation(const double &value);

//       
bool SetUseTransitSignal(const bool &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPattern* New(void);
// --------------------------

// --------------------------
//    
// --------------------------

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

