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

#ifndef NNEURONTRAINER_H
#define NNEURONTRAINER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// ,    ,      
class RDK_LIB_TYPE NNeuronTrainer: public UNet
{
public: // 
///    
/// 0 -    
/// 1 -  ,             
///  ,    ,  __   ,      
UProperty<int,NNeuronTrainer, ptPubParameter> StructureBuildMode;

///    
/// 0 -   
/// 1 -         
/// 2 ( !!!) -            
/// 3 -             
/// 4 -            
/// 5 -          

UProperty<int,NNeuronTrainer, ptPubParameter> CalculateMode;

///  ,   
UProperty<std::string, NNeuronTrainer, ptPubParameter> PulseGeneratorClassName;

///  ,  
UProperty<std::string, NNeuronTrainer, ptPubParameter> NeuronClassName;

///   
UProperty<std::string, NNeuronTrainer, ptPubParameter> SynapseClassName;

///   
UProperty<bool, NNeuronTrainer, ptPubParameter> IsNeedToTrain;

///        ()
UProperty<double, NNeuronTrainer, ptPubParameter> Delay;

///   ()
UProperty<double, NNeuronTrainer, ptPubParameter> SpikesFrequency;

///   
UProperty<int, NNeuronTrainer, ptPubParameter> NumInputDendrite;

///   
UProperty<int, NNeuronTrainer, ptPubParameter> MaxDendriteLength;

///   .      
///  ( ),  ,     .
UProperty<MDMatrix<double>, NNeuronTrainer, ptPubParameter> InputPattern;

///    
// 0 - DendriticSumPotential -    
// N -  N- 
UProperty<MDMatrix<double>,NNeuronTrainer,ptOutput | ptPubState> NeuronAmplitude;

UProperty<MDMatrix<double>,NNeuronTrainer,ptOutput | ptPubState> SomaNeuronAmplitude;

///    
UProperty<double, NNeuronTrainer, ptPubParameter> LTZThreshold;

///     
UProperty<double, NNeuronTrainer, ptPubParameter> FixedLTZThreshold;

///      
UProperty<double, NNeuronTrainer, ptPubParameter> TrainingLTZThreshold;

///        
UProperty<bool, NNeuronTrainer, ptPubParameter> UseFixedLTZThreshold;

///  
UProperty<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> Output;

///   
UProperty<double, NNeuronTrainer, ptPubParameter> SynapseResistanceStep;

/// Флаг включения DEBUG-логирования работы тренера:
/// при активации выводит подробную информацию о процессе обучения.
UProperty<bool, NNeuronTrainer, ptPubParameter> EnableDebug;


protected:

 //     
 int OldNumInputDendrite;

 //  
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;
 //  
 UEPtr <NPulseNeuron> neuron;


 //    (     )
 bool is_first_iter;
 //     (  ) ()
 double start_iter_time;

 //       
 double max_iter_neuron_amp;
 //         
 double max_neuron_amp;
 double max_neuron_amp_time;
 //   
 std::vector<int> DendriteLength;
 //       
 std::vector<double> InitialDendritePotential;
 //     
 std::vector<int> SynapseNum;

 //    
 int dend_counter;
 //   
 int dend_index;
 //    
 bool is_new_dend;
 //    (  )
 bool is_new_iteration;
 //     = c+1 ,    
 int syn_counter;

 bool thresh_first_iter;
 ////////////////////////////////////////////
 //     
 /////////////////////////////////////////////
 //    
 bool is_need_to_build;
 //        
 std::vector<double> max_iter_dend_amp;
 //          
 std::vector<double> max_dend_amp_time;
 //   
 std::vector<double> dissynchronization;
 //  :
 // 0 -   
 // 1 -  
 // -1 -  
 // 2 -  
 // -2 -  
 std::vector<int> dend_status;
 //   
 bool is_synchronizated;


public: // 
// --------------------------
//   
// --------------------------
NNeuronTrainer(void);
virtual ~NNeuronTrainer(void);
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
bool SetNeuronClassName(const std::string &value);

///   
bool SetSynapseClassName(const std::string &value);

///        ()
bool SetDelay(const double &value);

///   ()
bool SetSpikesFrequency(const double &value);

///   
bool SetNumInputDendrite(const int &value);

///   
bool SetMaxDendriteLength(const int &value);

///   
bool SetInputPattern(const MDMatrix<double> &value);

///   
bool SetNeedToTrain(const bool &value);

///    
bool SetLTZThreshold(const double &value);

///      
bool SetTrainingLTZThreshold(const double &value);

///     
bool SetFixedLTZThreshold(const double &value);

///        
bool SetUseFixedLTZThreshold(const bool &value);

///   
bool SetSynapseResistanceStep(const double &value);

/// Установка флага включения DEBUG-логирования обучения
bool SetEnableDebug(const bool &value);


// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NNeuronTrainer* New(void);
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
					const string &neuron_class_name,
					int old_num_input_dendrite, int num_input_dendrite,
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
//    
//bool MaximazeAmplitude(void);
//      
//       
//bool SynchronizePattern(void);
//            
//bool SynapseNormalization(void);

//      
//     
bool SomaSynchronizePattern(void);
//            
bool SomaSynapseNormalization(void);

//  
bool CalculateProcess(void);

//    
virtual bool ACalculate(void);

//      
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif
