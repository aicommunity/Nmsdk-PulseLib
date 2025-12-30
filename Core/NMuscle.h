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

#ifndef NMUSCLE_H
#define NMUSCLE_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "NReceptor.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NMuscle: public UNet
{
public: //  
/// 
UProperty<vector<double>,NMuscle, ptPubParameter> MulCoeffs;

UProperty<vector<double>,NMuscle, ptPubParameter> Param;

///  
UProperty<vector<double>,NMuscle, ptPubParameter> TC;

/// 
UProperty<double,NMuscle, ptPubParameter> Mass;

/// 
UProperty<double,NMuscle, ptPubParameter> Threshold;

///    
UProperty<double,NMuscle, ptPubParameter> G;

public: //   
///   ( )
UProperty<MDMatrix<double>, NMuscle, ptPubInput> Input;

/// Length input
UProperty<MDMatrix<double>, NMuscle, ptPubInput> LengthInput;

UProperty<MDMatrix<double>, NMuscle, ptOutput | ptPubState> Output;

public: //  
double y[15], yOld[15], mr[7], mrOld[7],vereteno1, vereteno2, goldgi;
double MaxActionPower;
bool FirstStep;


/*
//      
NReceptor *ExternalForce;

//    
//  
NItem *ActivePower;

// 
NItem *Length;

// 
NItem *Spindle1, *Spindle2;

//   
NItem *Tendon;
             */
public: // 
// --------------------------
//   
// --------------------------
NMuscle(void);
virtual ~NMuscle(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//   
//bool SetAmplitude(double value);

//   
//bool SetFrequency(double value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NMuscle* New(void);
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
double AperiodicSection(double in, double inOld, double param1, double param2);

// 
double CompareThreshold(double);

//  
double MuscularReduction(double);

//    
double MuscleFeedback(double x_new, double x_old, double y_old,
					  double k, double T1, double T2);
// --------------------------
};

}
#endif

