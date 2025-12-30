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

#ifndef NPULSE_GENERATOR_H
#define NPULSE_GENERATOR_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGenerator: public NSource
{
public: // 
///   () ( )
UProperty<double,NPulseGenerator, ptPubParameter> Frequency;

///   ()
UProperty<double,NPulseGenerator, ptPubParameter> PulseLength;

///  
UProperty<double,NPulseGenerator, ptPubParameter> Amplitude;

///   
UProperty<double,NPulseGenerator, ptPubParameter> Delay;

///  
/// ( ,    !)
UProperty<double,NPulseGenerator, ptPubParameter> FrequencyDeviation;

///     
UProperty<double,NPulseGenerator, ptPubParameter> AvgInterval;

public: //   
///  (    PulseOutput) (1)
UProperty<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPotential;

///  (2)
UProperty<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputFrequency;

///      (3)
UProperty<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPulseTimes;

public: //  
///  
UProperty<int,NPulseGenerator,ptPubState> PulseCounter;

///    
UProperty<double,NPulseGenerator,ptPubState> RandomFrequency;

///      
UProperty<list<double>,NPulseGenerator,ptPubState> AvgFrequencyCounter;

//      (UsePatternOutput)
bool IsInPatternMode;

protected:

double OldFrequency;

//   ----->
//    Reset
double ResetTime;
// <-----

//   ----->
//        (UsePatternOutput)
double PatternStartTime;

// <-----


public: // 
// --------------------------
//   
// --------------------------
NPulseGenerator(void);
virtual ~NPulseGenerator(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
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
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseGenerator* New(void);
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

class RDK_LIB_TYPE NPulseGeneratorTransit: public NPulseGenerator
{
public: // 
///       
UProperty<bool,NPulseGeneratorTransit, ptPubParameter> UseTransitSignal;

///  
UProperty<MDMatrix<double>,NPulseGeneratorTransit, ptInput | ptPubState> Input;

///        PatternFrequency   
///  :
/// 1.         PatternDuration
/// 2.         ,  
UProperty<bool,NPulseGeneratorTransit, ptPubParameter> UsePatternOutput;

///        (UsePatternOutput) (c)
UProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternDuration;

///       (UsePatternOutput) ()
UProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternFrequency;


protected:

/// ,       ,     
///      ,     > 0
///         ,  TheSamePulse == true
bool TheSamePulse = false;


public: // 
// --------------------------
//   
// --------------------------
NPulseGeneratorTransit(void);
virtual ~NPulseGeneratorTransit(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//       
bool SetUseTransitSignal(const bool &value);
// --------------------------
//      (   )   
bool SetUsePatternOutput(const bool &value);
// --------------------------
//         (UsePatternOutput)
bool SetPatternDuration(const double &value);
// --------------------------
//        (UsePatternOutput)
bool SetPatternFrequency(const double &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseGeneratorTransit* New(void);
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

