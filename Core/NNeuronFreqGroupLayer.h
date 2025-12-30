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

#ifndef NNEURONFREQGROUPLAYER_H
#define NNEURONFREQGROUPLAYER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,     ,     
class RDK_LIB_TYPE NNeuronFreqGroupLayer: public UNet
{
public: // 
///    
/// (  >1)         
/// 0 -    
/// 1 -   
UProperty<int,NNeuronFreqGroupLayer, ptPubParameter> StructureBuildMode;

///     
UProperty<std::string, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronGroupClassName;

///       
UProperty<int, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronsGroupHeight;
///       
UProperty<int, NNeuronFreqGroupLayer, ptPubParameter> AffNeuronsGroupWidth;
///     
UProperty<int, NNeuronFreqGroupLayer, ptPubParameter> NumAffNeuronsInGroup;


protected:

//       
int OldAffNeuronsGroupHeight, OldAffNeuronsGroupWidth;

public: // 
// --------------------------
//   
// --------------------------
NNeuronFreqGroupLayer(void);
virtual ~NNeuronFreqGroupLayer(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///    
bool SetAffNeuronGroupClassName(const std::string &value);

///       
bool SetAffNeuronsGroupHeight(const int &value);

///       x
bool SetAffNeuronsGroupWidth(const int &value);

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
virtual NNeuronFreqGroupLayer* New(void);
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
bool BuildStructure(int structure_build_mode, const string &aff_neuron_group_class_name,
					int old_num_aff_neurons_group_height, int old_num_aff_neurons_group_width,
					int num_aff_neurons_group_height, int num_aff_neurons_group_width,
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
