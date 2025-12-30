// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2020.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYNAPSE_STDP_H
#define NPULSE_SYNAPSE_STDP_H

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapseStdp: public NPulseSynapse
{
public: //  
UProperty<double,NPulseSynapseStdp, ptPubParameter> XModCoeff;

UProperty<double,NPulseSynapseStdp, ptPubParameter> YModCoeff;

UProperty<double,NPulseSynapseStdp, ptPubParameter> APlus;

UProperty<double,NPulseSynapseStdp, ptPubParameter> AMinus;

UProperty<double,NPulseSynapseStdp, ptPubParameter> XTau;
UProperty<double,NPulseSynapseStdp, ptPubParameter> YTau;

public: //  
///   
UProperty<double,NPulseSynapseStdp, ptPubState> XAvg;

///   
UProperty<double,NPulseSynapseStdp, ptPubState> YAvg;

///    X  Y 
UProperty<double,NPulseSynapseStdp, ptPubState> XYDiff;


public: //   
///     
/// (  )
UProperty<MDMatrix<double>, NPulseSynapseStdp, ptInput | ptPubState> PsActivityInput;

///    STDP
UProperty<MDMatrix<double>, NPulseSynapseStdp, ptOutput | ptPubState> StdpInfluence;

protected: //  

protected: //  

public: // 
// --------------------------
//   
// --------------------------
NPulseSynapseStdp(void);
virtual ~NPulseSynapseStdp(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetXTau(const double &value);

bool SetYTau(const double &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseSynapseStdp* New(void);
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
