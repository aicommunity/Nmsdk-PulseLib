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

#ifndef NEYEMUSCLE_H
#define NEYEMUSCLE_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NEyeMuscle: public UNet
{
public: //  
/// 
UProperty<vector<double>,NEyeMuscle, ptPubParameter> MulCoeffs;

UProperty<double,NEyeMuscle, ptPubParameter> K;

///  
UProperty<vector<double>,NEyeMuscle, ptPubParameter> TC;

public: //   
UProperty<std::vector<MDMatrix<double>>, NEyeMuscle, ptPubInput> Inputs;

UProperty<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputAcceleration;
UProperty<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputLength;
UProperty<MDMatrix<double>, NEyeMuscle, ptOutput | ptPubState> OutputSpeed;

public: //  
vector<double> P1,P2,P3,L;

// 
vector<double> Threshold;

// 
vector<double> Speed;

// 
vector<double> Acceleration;

public: // 
// --------------------------
//   
// --------------------------
NEyeMuscle(void);
virtual ~NEyeMuscle(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//   
bool SetAmplitude(double value);

//   
bool SetFrequency(double value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NEyeMuscle* New(void);
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

// --------------------------
//     
// --------------------------
protected:
//  
double MuscularReduction(size_t k,double in);

//  
void ThresholdCount(size_t k);
// --------------------------
};

}
#endif

