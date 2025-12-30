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

#ifndef NPULSE_HEBB_SYNAPSE_H
#define NPULSE_HEBB_SYNAPSE_H

#include "NPulseSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbSynapse: public NPulseSynapse
{
public: //  
UProperty<double,NPulseHebbSynapse, ptPubParameter> Min;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Mout;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Md;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> ActiveMs;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> PassiveMs;
UProperty<vector<double>,NPulseHebbSynapse, ptPubParameter> Kmot;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Kin;
UProperty<double,NPulseHebbSynapse, ptPubParameter> Kout;

// -   
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GdGain;

// -   
RDK::UProperty<double,NPulseHebbSynapse, ptPubParameter> GsGain;

public: //   
///     
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputLTZoneFeedbackSignal;

///  ""
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptInput | ptPubState> InputMotivation;

UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output1;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output2;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output3;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output4;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output5;
UProperty<MDMatrix<double>, NPulseHebbSynapse, ptOutput | ptPubState> Output6;

protected: //  
UProperty<double,NPulseHebbSynapse,ptPubState> G;
UProperty<double,NPulseHebbSynapse,ptPubState> Gd;
UProperty<vector<double>,NPulseHebbSynapse,ptPubState> Gs;
UProperty<double,NPulseHebbSynapse,ptPubState> GsSum;
UProperty<double,NPulseHebbSynapse,ptPubState> Win;
UProperty<double,NPulseHebbSynapse,ptPubState> Wout;

public: // 
// --------------------------
//   
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseHebbSynapse* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//       -
//  false      
//  synapse == 0,     
bool InstallHebbianConnection(void);

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
