#ifndef NMSDK_NSYNAPSESTDP_H
#define NMSDK_NSYNAPSESTDP_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseStdp: public NPulseSynapseCommon
{
public:
UProperty<double,NSynapseStdp, ptPubParameter> XModCoeff;

UProperty<double,NSynapseStdp, ptPubParameter> YModCoeff;

UProperty<double,NSynapseStdp, ptPubParameter> APlus;

UProperty<double,NSynapseStdp, ptPubParameter> AMinus;

UProperty<double,NSynapseStdp, ptPubParameter> XTau;
UProperty<double,NSynapseStdp, ptPubParameter> YTau;

public: //  
///   
UProperty<double,NSynapseStdp, ptPubState> XAvg;

///   
UProperty<double,NSynapseStdp, ptPubState> YAvg;

///    X  Y 
UProperty<double,NSynapseStdp, ptPubState> XYDiff;


public: //   
///     (   ,  
///  )
UProperty<MDMatrix<double>, NSynapseStdp, ptInput | ptPubState> PsActivityInput;

///    STDP
UProperty<MDMatrix<double>, NSynapseStdp, ptOutput | ptPubState> StdpInfluence;


public: // 
// --------------------------
//   
// --------------------------
NSynapseStdp(void);
virtual ~NSynapseStdp(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseStdp* New(void);
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

//      
virtual bool AReset(void);

//    
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



