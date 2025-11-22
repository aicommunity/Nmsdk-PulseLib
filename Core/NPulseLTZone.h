// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_LTZONE_H
#define NPULSE_LTZONE_H

#include "NPulseLTZoneCommon.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseLTZoneThreshold: public NPulseLTZoneCommon
{
public: // 
// --------------------------
//   
// --------------------------
NPulseLTZoneThreshold(void);
virtual ~NPulseLTZoneThreshold(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseLTZoneThreshold* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
//       
//     
//   'true'   
//  'false'    
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;

///  true      
virtual bool CheckPulseOn(void);

///  true       
virtual bool CheckPulseOff(void);
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


class RDK_LIB_TYPE NPulseLTZone: public NPulseLTZoneThreshold
{
public: //  
///  
UProperty<double,NPulseLTZone, ptPubParameter> TimeConstant;

///     LTZ
UProperty<bool, NPulseLTZone, ptPubParameter> UseLTZIntegtation;

///     
///  ,       
///      
UProperty<bool, NPulseLTZone, ptPubParameter> UseSpikeStabilizer;

public: // 
// --------------------------
//   
// --------------------------
NPulseLTZone(void);
virtual ~NPulseLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
//    
bool SetTimeConstant(const double &value);
//      LTZ
bool SetLTZIntegtation(const bool &value);
//     
bool SetUseSpikeStabilizer(const bool &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseLTZone* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

class RDK_LIB_TYPE NContinuesLTZone: public NPulseLTZoneCommon
{
public: // 
// --------------------------
//   
// --------------------------
NContinuesLTZone(void);
virtual ~NContinuesLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NContinuesLTZone* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

class RDK_LIB_TYPE NPulseSimpleLTZone: public NPulseLTZone
{
public: // 
///   ,  (    [0;500]
UProperty<double, NPulseSimpleLTZone, ptPubParameter> MaxFrequency;

protected: //  
NPulseGenerator generator;

public: // 
// --------------------------
//   
// --------------------------
NPulseSimpleLTZone(void);
virtual ~NPulseSimpleLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseSimpleLTZone* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};

class RDK_LIB_TYPE NContinuesSimpleLTZone: public NContinuesLTZone
{
public: // 
// --------------------------
//   
// --------------------------
NContinuesSimpleLTZone(void);
virtual ~NContinuesSimpleLTZone(void);
// --------------------------

// --------------------------
//    
// --------------------------
//    
bool SetTimeConstant(const double &value);

//   
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NContinuesSimpleLTZone* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};


}
#endif
