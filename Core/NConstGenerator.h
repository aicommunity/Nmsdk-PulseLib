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

#ifndef NCONST_GENERATOR_H
#define NCONST_GENERATOR_H

#include "NSource.h"


namespace NMSDK {

class RDK_LIB_TYPE NConstGenerator: public NSource
{
public: //  
///   
UProperty<double,NConstGenerator, ptPubParameter> Amplitude;

protected: //  
///      
bool UpdateOutputFlag;

public: // 
// --------------------------
//   
// --------------------------
NConstGenerator(void);
virtual ~NConstGenerator(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
///   
bool SetAmplitude(const double &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
///         
virtual NConstGenerator* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
///        
virtual bool ADefault(void);

///     
///   
///    Reset()   Ready  true
///    
virtual bool ABuild(void);

///   .
virtual bool AReset(void);

///    
virtual bool ACalculate(void);
// --------------------------
};

}
#endif

