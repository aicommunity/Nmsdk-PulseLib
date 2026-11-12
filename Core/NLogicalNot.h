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

#ifndef NLOGICALNOT_H
#define NLOGICALNOT_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// ,       
class RDK_LIB_TYPE NLogicalNot: public UNet
{
public: // 
///    
/// 0 -    
/// 1 -       .
///     2   +
///  ,    .
///  :   ->  
///  :  
///       2- ,    .
///      ""  , 
///          .
///            ,
///       .   
///       2-   
/// "" 1- .
UProperty<int,NLogicalNot, ptPubParameter> StructureBuildMode;

///    
/// 0 -   
/// 1 -         
/// 2 ( !!!) -            
/// 3 -             
/// 4 -            
/// 5 -          

UProperty<int,NLogicalNot, ptPubParameter> CalculateMode;

///  ,   
UProperty<std::string, NLogicalNot, ptPubParameter> PulseGeneratorClassName;

///  ,    "" ( )
UProperty<std::string, NLogicalNot, ptPubParameter> LogicalNotClassName;

///  ,   
UProperty<std::string, NLogicalNot, ptPubParameter> NeuronTrainerClassName;

///   
UProperty<std::string, NLogicalNot, ptPubParameter> NeuronClassName;

///   
UProperty<std::string, NLogicalNot, ptPubParameter> SynapseClassName;

///   
UProperty<int, NLogicalNot, ptPubParameter> NumInputDendrite;

///   
UProperty<bool, NLogicalNot, ptPubParameter> IsNeedToTrain;

/////   ()
UProperty<double, NLogicalNot, ptPubParameter> SpikesFrequency;

///   .      
///  ( ),  ,     .
UProperty<MDMatrix<double>, NLogicalNot, ptPubParameter> InputPattern;

///         ( )
UProperty<int, NLogicalNot, ptPubParameter> DendriteLengthNGen1;

///         ( )
UProperty<int, NLogicalNot, ptPubParameter> DendriteLengthNGen2;

///         
UProperty<int, NLogicalNot, ptPubParameter> NumExcSynapsesNGen1;

///         
UProperty<int, NLogicalNot, ptPubParameter> NumExcSynapsesNGen2;

///       (UsePatternOutput) ()
UProperty<double, NLogicalNot, ptPubParameter> PatternFrequency;

// 
UEPtr<NNeuronTrainer> NeuronTrainer;

// 
UEPtr<NPulseNeuron> OutputNeuron;



protected:

//        (UsePatternOutput) ()
double PatternDuration;

//  
UEPtr<NPulseNeuron> GeneratorNeuron1;
UEPtr<NPulseNeuron> GeneratorNeuron2;

//      
 std::vector<UEPtr<NPulseSynapse>> synapses1;
 std::vector<UEPtr<NPulseSynapse>> synapses2;

//      
UEPtr<NPulseGeneratorTransit> Generator;

// 
//UEPtr<NNeuronTrainer> NeuronTrainer;

//,    NeuronTrainer  
UEPtr<NPulseGeneratorTransit> PatternGenerator;

// 
//UEPtr<NPulseNeuron> OutputNeuron;



public: // 
// --------------------------
//   
// --------------------------
NLogicalNot(void);
virtual ~NLogicalNot(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///   
bool SetCalculateMode(const int &value);

///  ,   
bool SetPulseGeneratorClassName(const std::string &value);

///  ,   
bool SetNeuronTrainerClassName(const std::string &value);

///  ,    "" ( )
bool SetLogicalNotClassName(const std::string &value);

///  ,  
bool SetNeuronClassName(const std::string &value);

///  ,  
bool SetSynapseClassName(const std::string &value);

///   
bool SetNeedToTrain(const bool &value);

///   
bool SetNumInputDendrite(const int &value);

///   ()
bool SetSpikesFrequency(const double &value);

///   
bool SetInputPattern(const MDMatrix<double> &value);

///         ( )
bool SetDendriteLengthNGen1(const int &value);

///         ( )
bool SetDendriteLengthNGen2(const int &value);

///         
bool SetNumExcSynapsesNGen1(const int &value);

///         
bool SetNumExcSynapsesNGen2(const int &value);

///       (UsePatternOutput) ()
bool SetPatternFrequency(const double &value);
// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NLogicalNot* New(void);
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
bool BuildStructure(int structure_build_mode, const string &pulse_generator_class_name,
                    const string &neuron_class_name, int num_input_dendrite,
                    double spikes_frequency,
                    int num_exc_synapses_ngen1,int num_exc_synapses_ngen2,
                    int dendrite_length_ngen1, int dendrite_length_ngen2,
                    const MDMatrix<double> &input_pattern);

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
