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

#ifndef NNEURONSLAYER_H
#define NNEURONSLAYER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,   
class RDK_LIB_TYPE NNeuronsLayer: public UNet
{
public: // 
///    
/// (  >1)         
/// 0 -    
/// 1 -   
UProperty<int,NNeuronsLayer, ptPubParameter> StructureBuildMode;

///     
UProperty<std::string, NNeuronsLayer, ptPubParameter> NeuronsClassName;

///       
UProperty<int, NNeuronsLayer, ptPubParameter> NeuronsHeight;
///       
UProperty<int, NNeuronsLayer, ptPubParameter> NeuronsWidth;


protected:

//       
int OldNeuronsHeight, OldNeuronsWidth;

public: // 
// --------------------------
//   
// --------------------------
NNeuronsLayer(void);
virtual ~NNeuronsLayer(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///    
bool SetNeuronsClassName(const std::string &value);

///       
bool SetNeuronsHeight(const int &value);

///       x
bool SetNeuronsWidth(const int &value);
// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NNeuronsLayer* New(void);
static UComponent* NewStatic(void);
// --------------------------

// --------------------------
//    
// --------------------------
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
protected:
///         
bool BuildStructure(int structure_build_mode, const string &neurons_class_name,
					int old_neurons_height, int old_neurons_width,
					int neurons_height, int neurons_width);

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
