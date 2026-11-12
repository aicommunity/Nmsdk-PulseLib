// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NNEURON_LIFE_H
#define NNEURON_LIFE_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "NSum.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNeuronLife: public UNet
{
public: //  
///   
UProperty<double,NNeuronLife,ptPubParameter> Threshold;

///   ,    - 
///  
UProperty<double,NNeuronLife,ptPubParameter> CriticalEnergy;

//   

///      
UProperty<double,NNeuronLife,ptPubParameter> MaxPotentialGradient;

UProperty<double,NNeuronLife,ptPubParameter> WearOutConstPositive;

UProperty<double,NNeuronLife,ptPubParameter> WearOutConstNegative;

UProperty<double,NNeuronLife,ptPubParameter> Kw;

///    
UProperty<double,NNeuronLife,ptPubParameter> Emax;

UProperty<double,NNeuronLife,ptPubParameter> En;

UProperty<double,NNeuronLife,ptPubParameter> Kq;

///     
UProperty<double,NNeuronLife,ptPubParameter> Ee0;
UProperty<double,NNeuronLife,ptPubParameter> Es;
UProperty<double,NNeuronLife,ptPubParameter> Eh0;
UProperty<double,NNeuronLife,ptPubParameter> Econst;
UProperty<double,NNeuronLife,ptPubParameter> Ecr;
UProperty<double,NNeuronLife, ptPubParameter> WearOutcr;
UProperty<double,NNeuronLife,ptPubParameter> EnergyWearOutCritical;
//RDK::UProperty<double,NNeuronLife, ptPubParameter> E;    //?
//RDK::UProperty<double,NNeuronLife, ptPubParameter> N;   //?
UProperty<double,NNeuronLife,ptPubParameter> EyConst;
UProperty<double,NNeuronLife,ptPubParameter> EyBonusPos;
UProperty<double,NNeuronLife,ptPubParameter> EyBonusNeg;

///    
UProperty<double,NNeuronLife,ptPubParameter> EnergyComprehensibility;

///    
UProperty<double,NNeuronLife,ptPubParameter> Kdp;
UProperty<double,NNeuronLife,ptPubParameter> Pdmax;
UProperty<double,NNeuronLife,ptPubParameter> Qd;
UProperty<double,NNeuronLife,ptPubParameter> Khp0;
UProperty<double,NNeuronLife,ptPubParameter> Khp1;

UProperty<double,NNeuronLife,ptPubParameter> Qsummax;

//     
UProperty<double,NNeuronLife,ptPubParameter> EnergyBonus;

public: //    
//  
UProperty<double,NNeuronLife,ptPubState> Energy;

public: //   
///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input1;

///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input2;

///
UProperty<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input3;

///
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output1;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output2;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output3;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> OutputThreshold;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output5;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output6;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output7;
UProperty<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output8;

protected: //  
///  
UProperty<double,NNeuronLife,ptPubState> WearOut;

///  
UProperty<double,NNeuronLife,ptPubState> ThresholdLife;

UProperty<double,NNeuronLife,ptPubState> ThresholdWearOut;

UProperty<double,NNeuronLife,ptPubState> ThresholdFeel;

/// 
UProperty<double,NNeuronLife,ptPubState> Feel;

UProperty<double,NNeuronLife,ptPubState> Qsum;

UProperty<double,NNeuronLife,ptPubState> Esum;

//RDK::UProperty<double,NNeuronLife,ptPubState> UsumOld;

//RDK::UProperty<double,NNeuronLife,ptPubState> UsumProizv;

UProperty<double,NNeuronLife,ptPubState> EsumOld;

UProperty<double,NNeuronLife,ptPubState> EsumProizv;

///    
UProperty<double,NNeuronLife,ptPubState> Ea;

///       
UProperty<double,NNeuronLife,ptPubState> Ey;

///        
UProperty<double,NNeuronLife,ptPubState> Ee;

///       
UProperty<double,NNeuronLife,ptPubState> Eh;

///  -    
UProperty<double,NNeuronLife,ptPubState> dEa;

///  -       
UProperty<double,NNeuronLife,ptPubState> dEy;

///  -        
UProperty<double,NNeuronLife,ptPubState> dEe;

///  -       
UProperty<double,NNeuronLife,ptPubState> dEh;

///    
UProperty<double,NNeuronLife,ptPubState> dE;

UProperty<double,NNeuronLife,ptPubState> Usum;

//  
//RDK::UProperty<double,NNeuronLife,ptPubState> NeuralPotential;

//   
//RDK::UProperty<double,NNeuronLife,ptPubState> PrePotential;

//   
//RDK::UProperty<int,NNeuronLife,ptPubState> PulseCounter;
public: //    
//     
UEPointer<NSum, NNeuronLife> ExternalEnergyBonus;

public: // 
// --------------------------
//   
// --------------------------
NNeuronLife(void);
virtual ~NNeuronLife(void);
// --------------------------

// --------------------------
//    
// --------------------------
bool SetThreshold(const double &value);

//       
bool SetMaxPotentialGradient(const double &value);

bool SetEmax(const double &value);

bool SetEn(const double &value);

bool SetPdmax(const double &value);

bool SetQsummax(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NNeuronLife* New(void);
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

virtual bool ACalcWearOut(void);

virtual bool ACalcThresholdLife(void);

virtual bool ACalcEnergy(void);

virtual bool ACalcFeel(void);

double FeelDiff(double kq, double ediff, double en);

//virtual bool ACalcEnergy(void);
};

}
#endif
