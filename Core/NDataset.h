#ifndef NDATASET_H
#define NDATASET_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,   
class RDK_LIB_TYPE NDataset: public UNet
{
///  ,   
UProperty<std::string, NDataset, ptPubParameter> PulseGeneratorClassName;

/// 
UProperty<int,NDataset, ptPubParameter> NumGenerators;

///  ( )
UProperty<int,NDataset, ptPubParameter> NumFeatures;

///  ( )
UProperty<int,NDataset, ptPubParameter> NumSamples;

///  
UProperty<MDMatrix<double>,NDataset, ptPubParameter> MatrixData;

///  
UProperty<MDMatrix<int>,NDataset, ptPubParameter> MatrixClasses;

///    
UProperty<bool,NDataset, ptPubParameter> ReloadDataset;

///    
UProperty<MDMatrix<double>,NDataset, ptPubState> MatrixDelay;

///   
UProperty<int,NDataset, ptPubParameter> Iteration;

///   
UProperty<float,NDataset, ptPubParameter> Tay;

///        ()
UProperty<double, NDataset, ptPubParameter> Delay;

/// ,   :  =0,   
/// =1,     
/// =2,    
UProperty<int, NDataset, ptPubState> StateGeneration;

/// ,  
UProperty<double, NDataset, ptPubState> TimeGeneration;

/// 
UProperty<double, NDataset, ptPubState> OperatingTime;

///   
UProperty<bool, NDataset, ptPubState> ResetDelay;

///   ()
UProperty<double, NDataset, ptPubParameter> SpikesFrequency;

///   
UProperty<int,NDataset, ptPubState> NumClasses;

/// 
vector<NPulseGeneratorTransit*> Generators;

///  
UProperty<string,NDataset, ptPubState> FileName;

protected:

 // <-------

public: // 
// --------------------------
//   
// --------------------------
NDataset(void);
virtual ~NDataset(void);
// --------------------------

// --------------------------
//   
// --------------------------

///  ,   
bool SetPulseGeneratorClassName(const std::string &value);

/// 
bool SetNumGenerators(const int &value);

///  ( )
bool SetNumFeatures(const int &value);

///  
bool SetMatrixData(const MDMatrix<double> &value);

///  
bool SetMatrixClasses(const MDMatrix<int> &value);

///   ()
bool SetSpikesFrequency(const double &value);

///        ()
bool SetDelay(const double &value);

///    
///       
//      comp      
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

///    
///       
//      comp    
virtual bool ADelComponent(UEPtr<UContainer> comp);

// --------------------------
//    
// --------------------------
//         
virtual NDataset* New(void);
static UComponent* NewStatic(void);
// --------------------------


// --------------------------
//    
// --------------------------
protected:
//     .
//      ,
//  
bool TreatDataFromFile(void);

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
