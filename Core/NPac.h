// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */
#ifndef NPAC_H
#define NPAC_H

#include "NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NPac: public UNet
{
public: //  
///   
UProperty<vector<vector<double> >,NPac, ptPubParameter> PulseAmplitude;

///    
UProperty<vector<vector<double> >,NPac, ptPubParameter> SecretionTC;

///    
UProperty<vector<vector<double> >,NPac, ptPubParameter> DissociationTC;

///  
UProperty<vector<vector<double> >,NPac, ptPubParameter> Gain;

///  
/// 0 -  
/// 1 -       
UProperty<int,NPac, ptPubParameter> Mode;

///  
/// 0 - 
/// 1 - 
UProperty<int,NPac, ptPubParameter> TCMode;

public: //   
UProperty<std::vector<MDMatrix<double>>, NPac, ptPubInput> Inputs;

UProperty<MDMatrix<double>, NPac, ptOutput | ptPubState> Output;

protected: //  
///    
UProperty<vector<vector<double> >,NPac, ptPubState> PreOutput;

public: // 
// --------------------------
//   
// --------------------------
NPac(void);
virtual ~NPac(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//   
bool SetPulseAmplitude(const vector<vector<double> > &value);

//    
bool SetSecretionTC(const vector<vector<double> > &value);

//    
bool SetDissociationTC(const vector<vector<double> > &value);

// 
bool SetGain(const vector<vector<double> > &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPac* New(void);
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

class RDK_LIB_TYPE NCPac: public NPac
{
public:
// --------------------------
//    
// --------------------------
//         
virtual NCPac* New(void);
// --------------------------

};

}
#endif
