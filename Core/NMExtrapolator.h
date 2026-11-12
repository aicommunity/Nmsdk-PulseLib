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

#ifndef NMExtrapolatorH
#define NMExtrapolatorH

#include "../../../Rdk/Deploy/Include/rdk.h"
//---------------------------------------------------------------------------

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NMExtrapolator: public UNet
{
public: //  
//  , 
UProperty<RDK::UTime,NMExtrapolator, ptPubParameter> PredictionTime;

//   
UProperty<size_t,NMExtrapolator, ptPubParameter> NumLevels;

public: //   
///   
UProperty<std::vector<MDMatrix<double>>, NMExtrapolator, ptInput | ptPubState> Inputs;

///   
UProperty<MDMatrix<double>, NMExtrapolator, ptOutput | ptPubState> Output;

protected: //  
//   
vector<size_t> PascalCoeffs;

protected: //  
//  
RDK::UQueue<std::vector<double> > History;
//vector<Real> History2;

//   
int HistoryCounter;

public:
// --------------------------
//   
// --------------------------
NMExtrapolator(void);
virtual ~NMExtrapolator(void);
// --------------------------

// --------------------------
//    
// --------------------------
//  , 
bool SetPredictionTime(const RDK::UTime &ptime);

//   
bool SetNumLevels(const size_t &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual UContainer* New(void);
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
void CalcPascalCoeff(size_t level, vector<size_t> &coeffs);
// --------------------------
};

}

#endif
