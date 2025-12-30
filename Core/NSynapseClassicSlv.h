#ifndef NMSDK_NSYNAPSECLASSICSLV_H
#define NMSDK_NSYNAPSECLASSICSLV_H

#include "NSynapseClassic.h"

namespace NMSDK {

///       
struct NSynapseSpikeData
{
///         ()
double SpikeTimeStamp;

///     
double Weight;

NSynapseSpikeData(void);
NSynapseSpikeData(const NSynapseSpikeData& value);
};

class NSynapseClassicSlv: public NSynapseClassic
{
public: // 
///      
UProperty<int, NSynapseClassicSlv, ptPubParameter> SpikeBufferSize;

protected: //  
///        
std::vector<NSynapseSpikeData> SpikeBuffer;

///        
size_t FirstSpikeBufferIndex, LastSpikeBufferIndex;

///    
MDMatrix<double> PreviousInput;

public: // 
// --------------------------
//   
// --------------------------
NSynapseClassicSlv(void);
virtual ~NSynapseClassicSlv(void);
// --------------------------

// --------------------------
//   
// --------------------------
bool SetSpikeBufferSize(const int &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseClassicSlv* New(void);
// --------------------------        

// --------------------------
//     
// --------------------------
protected:
///      
/// time_value     
double CalcCurrent(double tau_s, double current, double time_value, double weight);

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



