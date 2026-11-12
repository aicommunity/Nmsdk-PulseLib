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

#ifndef NPULSEPERSEPTRON_H
#define NPULSEPERSEPTRON_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,   
class RDK_LIB_TYPE NPulsePerseptron: public UNet
{
public: // 
///    
/// (  >1)         
/// 0 -    
/// 1 -   , 1  -> 1 ,  -   
/// 2 -    
UProperty<int,NPulsePerseptron, ptPubParameter> StructureBuildMode;

///  ,     ,     
UProperty<std::string, NPulsePerseptron, ptPubParameter> NeuronFreqGroupLayerClassName;

///      (  )
UProperty<int, NPulsePerseptron, ptPubParameter> NumInputFeatures;

///  ,   
UProperty<std::string, NPulsePerseptron, ptPubParameter> NeuronsLayerClassName;

///   
UProperty<int, NPulsePerseptron, ptPubParameter> NumNeuronsLayer;

///    .
UProperty<MDMatrix<int>, NPulsePerseptron, ptPubParameter> AffNeuronsLayerSize;

///   .
UProperty<MDMatrix<int>, NPulsePerseptron, ptPubParameter> NeuronsLayerSize;

///     
UProperty<int, NPulsePerseptron, ptPubParameter> NumAffNeuronsInGroup;

///  FreqGroupLayer
UProperty<bool, NPulsePerseptron, ptPubState> FlagFreqGroupLayer;

protected:

//     
int OldNumNeuronsLayer;

public: // 
// --------------------------
//   
// --------------------------
NPulsePerseptron(void);
virtual ~NPulsePerseptron(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///  ,     ,     
bool SetNeuronFreqGroupLayerClassName(const std::string &value);

///      (  )
bool SetNumInputFeatures(const int &value);

///  ,   
bool SetNeuronsLayerClassName(const std::string &value);

///   
bool SetNumNeuronsLayer(const int &value);

///    .
bool SetAffNeuronsLayerSize(const MDMatrix<int> &value);

///   .
bool SetNeuronsLayerSize(const MDMatrix<int> &value);

///     
bool SetNumAffNeuronsInGroup(const int &value);
// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulsePerseptron* New(void);
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
bool BuildStructure(int structure_build_mode, const string &neuron_freq_group_layer_class_name,
					const string &neurons_layer_class_name,
					int old_num_neurons_layer, int num_neurons_layer,
					const MDMatrix<int> &aff_neurons_layer_size, const MDMatrix<int> &neurons_layer_size,
					int num_aff_neurons_in_group);

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
