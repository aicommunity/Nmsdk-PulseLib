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

#ifndef NMSDK_NCableModelH
#define NMSDK_NCableModelH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelCable: public NPulseChannelClassic
{
public: // 
///  , 
UProperty<double, NPulseChannelCable, ptPubParameter> EL;

///   , *
UProperty<double, NPulseChannelCable, ptPubParameter> Ri;

///  , 
UProperty<double, NPulseChannelCable, ptPubParameter> CableMembraneResistance;

///  , 
UProperty<double, NPulseChannelCable, ptPubParameter> D;

///   , *^2
UProperty<double, NPulseChannelCable, ptPubParameter> Rm;

///  , 
UProperty<double, NPulseChannelCable, ptPubParameter> Cm;

///     , 
UProperty<double, NPulseChannelCable, ptPubParameter> ModelMaxLength;

///    , 
UProperty<double, NPulseChannelCable, ptPubState> ModelMaxTime;

///     , 
UProperty<double, NPulseChannelCable, ptPubParameter> dx;

///    , 
UProperty<double, NPulseChannelCable, ptPubState> dt;

///  :
/// 0/false -  D   CableMembraneResistance,
/// 1/true -  CableMembraneResistance   D
UProperty<bool, NPulseChannelCable, ptPubParameter> CalcMode;



protected: //  
///   
UProperty<double, NPulseChannelCable, ptPubState> TauM;

/////   
UProperty<MDMatrix<double>, NPulseChannelCable, ptPubState> Vm;

/////  
UProperty<MDMatrix<double>, NPulseChannelCable, ptPubState> InpV;

/////
MDMatrix<double> x;

/////
MDMatrix<double> t;

/////      
int x_points_number;

/////     
int t_points_number;

/////  ,    
int inpT;

/////  ,    
int inpX;

public: // 
// --------------------------
//   
// --------------------------
NPulseChannelCable(void);
virtual ~NPulseChannelCable(void);
// --------------------------

// --------------------------
//   
// --------------------------
/// 
bool SetEL(const double &value);

///
bool SetRi(const double &value);

///
bool SetCableMembraneResistance(const double &value);

///
bool SetD(const double &value);

//
bool SetRm(const double &value);

//
bool SetTauM(const double &value);

//
bool SetCm(const double &value);

//
bool SetModelMaxLength(const double &value);

//
bool SetModelMaxTime(const double &value);

//
bool Setdx(const double &value);

//
bool Setdt(const double &value);

//
bool SetCalcMode(const bool &value);
// --------------------------


// --------------------------
//    
// --------------------------
//         
virtual NPulseChannelCable* New(void);
// --------------------------

// --------------------------
//     
// --------------------------
protected:

void FormingInput();


//        
virtual bool ADefault(void);

//     
//   
//    Reset()   Ready  true
//    
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate2(void);
// --------------------------
};


}
#endif



