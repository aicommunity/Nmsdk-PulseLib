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

#ifndef NPULSE_GENERATOR_MULTI_H
#define NPULSE_GENERATOR_MULTI_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGeneratorMulti: public NSource
{
public: // 
///   () ( )
UProperty<double,NPulseGeneratorMulti, ptPubParameter> Frequency;

///   ()
UProperty<double,NPulseGeneratorMulti, ptPubParameter> PulseLength;

///  
UProperty<double,NPulseGeneratorMulti, ptPubParameter> Amplitude;

///   
UProperty<int, NPulseGeneratorMulti, ptPubParameter> PulseCount;

///   
UProperty<MDMatrix<double>, NPulseGeneratorMulti, ptPubParameter> Delays;

///  
/// ( ,    !)
UProperty<double,NPulseGeneratorMulti, ptPubParameter> FrequencyDeviation;

///     
UProperty<double,NPulseGeneratorMulti, ptPubParameter> AvgInterval;

public: //   
///  (    PulseOutput) (1)
UProperty<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputPotential;

///  (2)
UProperty<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputFrequency;

///      (3)
UProperty<MDMatrix<double>,NPulseGeneratorMulti, ptOutput | ptPubState> OutputPulseTimes;

public: //  
///  
UProperty<vector<int>,NPulseGeneratorMulti,ptPubState> PulseCounters;

///    
UProperty<double,NPulseGeneratorMulti,ptPubState> RandomFrequency;

///      
UProperty<list<double>,NPulseGeneratorMulti,ptPubState> AvgFrequencyCounter;

protected:

//       (     Output)
std::vector<bool> pulses;

double OldFrequency;

//   ----->
//    Reset
double ResetTime;
// <-----

//   ----->
//        (UsePatternOutput)
double PatternStartTime;
//    (UsePatternOutput)
bool IsInPatternMode;
// <-----


public: // 
// --------------------------
//   
// --------------------------
NPulseGeneratorMulti(void);
virtual ~NPulseGeneratorMulti(void);
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
bool SetPulseCount(const int &value);

//    
bool SetDelays(const MDMatrix<double> &value);

//    
// ( ,    !)
bool SetFrequencyDeviation(const double &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseGeneratorMulti* New(void);
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

