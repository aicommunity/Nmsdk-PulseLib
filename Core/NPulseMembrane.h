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

#ifndef NPULSE_MEMBRANE_H
#define NPULSE_MEMBRANE_H

#include "NPulseMembraneCommon.h"
#include "NPulseChannel.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseMembrane: public NPulseMembraneCommon
{
public: //  
///   
UProperty<double,NPulseMembrane, ptPubParameter> FeedbackGain;

///   
UProperty<bool,NPulseMembrane, ptPubParameter> ResetAvailable;

///   
UProperty<std::string, NPulseMembrane, ptPubParameter> SynapseClassName;

///    
UProperty<std::string, NPulseMembrane, ptPubParameter> ExcChannelClassName;

///    
UProperty<std::string, NPulseMembrane, ptPubParameter> InhChannelClassName;

///   
UProperty<int, NPulseMembrane, ptPubParameter> NumExcitatorySynapses;

///   
UProperty<int, NPulseMembrane, ptPubParameter> NumInhibitorySynapses;

public: //   
///      
UProperty<MDMatrix<double>, NPulseMembrane, ptInput | ptPubState> InputFeedbackSignal;


protected: //  
//   
vector<NPulseChannelCommon*> ExcitatoryChannels;

//   
vector<NPulseChannelCommon*> InhibitoryChannels;

//  
vector<NPulseSynapseCommon*> ExcitatorySynapses;

//  
vector<NPulseSynapseCommon*> InhibitorySynapses;


public: // 
// --------------------------
//   
// --------------------------
NPulseMembrane(void);
virtual ~NPulseMembrane(void);
// --------------------------

// --------------------------
//    
// --------------------------
//   
size_t GetNumPosChannels(void) const;
NPulseChannelCommon* GetPosChannel(size_t i);

//   
size_t GetNumNegChannels(void) const;
NPulseChannelCommon* GetNegChannel(size_t i);

//  
size_t GetNumExcitatorySynapses(void) const;
NPulseSynapseCommon* GetExcitatorySynapses(size_t i);

//  
size_t GetNumInhibitorySynapses(void) const;
NPulseSynapseCommon* GetInhibitorySynapses(size_t i);

virtual bool UpdateChannelData(UEPtr<NPulseChannelCommon> channel, UEPtr<UIPointer> pointer=0);

virtual bool UpdateSynapseData(UEPtr<NPulseSynapseCommon> synapse, UEPtr<UIPointer> pointer=0);
// --------------------------

// --------------------------
//    
// --------------------------
///   
bool SetResetAvailable(const bool &value);

//   
bool SetFeedbackGain(const double &value);

bool SetSynapseClassName(const std::string &value);
bool SetExcChannelClassName(const std::string &value);
bool SetInhChannelClassName(const std::string &value);
bool SetNumExcitatorySynapses(const int &value);
bool SetNumInhibitorySynapses(const int &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseMembrane* New(void);

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
virtual bool ACalculate2(void);

//      
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif

