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

#ifndef NPULSE_LIFE_NEURON_H
#define NPULSE_LIFE_NEURON_H

#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "NNeuronLife.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseLifeNeuron: public NPulseNeuron
{
public: // 
///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGd;

///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGs;

///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosG;

///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGd;

///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGs;

///     
UProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegG;

public: //   
/// :     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGd;

/// :           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGdNorm;

/// :     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGs;

/// :           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGsNorm;

///     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosG;

///           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGNorm;

/// :     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGd;

/// :           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGdNorm;

/// :     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGs;

/// :           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGsNorm;

///     
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegG;

///           
UProperty<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGNorm;

protected: //  

UEPointer<NNeuronLife,NPulseLifeNeuron> NeuronLife;

public: // 
// --------------------------
//   
// --------------------------
NPulseLifeNeuron(void);
virtual ~NPulseLifeNeuron(void);
// --------------------------

// --------------------------
//     
// --------------------------
//     
NNeuronLife* GetNeuronLife(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseLifeNeuron* New(void);
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

};

}
#endif
