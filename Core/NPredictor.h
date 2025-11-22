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

#ifndef NPREDICTOR_H
#define NPREDICTOR_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// ,       
class RDK_LIB_TYPE NPredictor: public UNet
{
public: // 
///    
/// 0 -    
/// 1 -       .
///     2   +
///  ,    .
///  :   ->  
///  :  
///       2- ,    .
///      ""  , 
///          .
///            ,
///       .   
///       2-   
/// "" 1- .
UProperty<int,NPredictor, ptPubParameter> StructureBuildMode;

///    
/// 0 -      
/// 1 -    
UProperty<int, NPredictor, ptPubParameter> InputMode;

///   (   )
///  N        t, t-tau, ... t-(N-1)tau,
/// tau -     , N -  
///  M ,   . M -  
UProperty<MDMatrix<double>, NPredictor, ptPubParameter> InputMatrix;

////     =
/// =     
UProperty<int, NPredictor, ptPubParameter> InputCols;

////   =
/// =     
UProperty<int, NPredictor, ptPubParameter> InputRows;

/// tau     (      )
UProperty<double, NPredictor, ptPubParameter> MeasurementPeriod;

///   (   ,        )
UProperty<MDMatrix<double>,NPredictor, ptInput | ptPubState> Input;

///  ,  
UProperty<MDMatrix<double>, NPredictor, ptOutput | ptPubState> Output;



protected:
///  N  i-       t, t-tau, ... t-(N-1)tau,
/// tau -     , N -  
MDMatrix<double> input_pattern;

///  tau     (      )
double measurement_period_start;

///  input_pattern (      )
int k;

///     input_pattern (      )
bool between_measurements;


public: // 
// --------------------------
//   
// --------------------------
NPredictor(void);
virtual ~NPredictor(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///   
bool SetInputMode(const int &value);

///   
bool SetInputMatrix(const MDMatrix<double> &value);

////     =
/// =     
bool SetInputCols(const int &value);

///   =
/// =     
bool SetInputRows(const int &value);

/// tau    
/// (     )
bool SetMeasurementPeriod(const double &value);

// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPredictor* New(void);
static UComponent* NewStatic(void);
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
///         
bool BuildStructure();

///          5 
double Predict(MDMatrix<double> &input_pattern);

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
