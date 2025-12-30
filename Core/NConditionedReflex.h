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

#ifndef NCONDITIONEDREFLEX_H
#define NCONDITIONEDREFLEX_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,       
class RDK_LIB_TYPE NConditionedReflex: public UNet
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
UProperty<int,NConditionedReflex, ptPubParameter> StructureBuildMode;

///  ,   
UProperty<std::string, NConditionedReflex, ptPubParameter> PulseGeneratorClassName;

///  ,   
UProperty<std::string, NConditionedReflex, ptPubParameter> NeuronTrainerClassName;

///   
UProperty<std::string, NConditionedReflex, ptPubParameter> NeuronClassName;

///   
UProperty<std::string, NConditionedReflex, ptPubParameter> SynapseClassName;

///  "" 
UProperty<int, NConditionedReflex, ptPubParameter> BigNeuronSize;

///   
UProperty<bool, NConditionedReflex, ptPubParameter> IsNeedToTrain;

///        ()
UProperty<double, NConditionedReflex, ptPubParameter> Delay;

///   ()
UProperty<double, NConditionedReflex, ptPubParameter> SpikesFrequency;

///       
UProperty<int, NConditionedReflex, ptPubParameter> NumUnconditionalStimulDendrite;

///       
UProperty<int, NConditionedReflex, ptPubParameter> NumConditionalStimulDendrite;

///      
UProperty<int, NConditionedReflex, ptPubParameter> MaxDendriteLength;

///      .
///      
///  ( ),  ,     .
///        !!!
UProperty<MDMatrix<double>, NConditionedReflex, ptPubParameter> UnconditionalStimulPattern;

///      .
///      
///  ( ),  ,     .
///        !!!
UProperty<MDMatrix<double>, NConditionedReflex, ptPubParameter> ConditionalStimulPattern;

///  
/// false -  
/// true -  
UProperty<bool, NConditionedReflex, ptPubParameter> IsNegInfluence;

///    
UProperty<double, NConditionedReflex, ptPubParameter> LTZThreshold;

///     
UProperty<double, NConditionedReflex, ptPubParameter> FixedLTZThreshold;

///        
UProperty<bool, NConditionedReflex, ptPubParameter> UseFixedLTZThreshold;


protected:

 //     
 //int OldNumNeurons;
 //     
 //int OldNumInputDendrite;

 //  
 UEPtr<NNeuronTrainer> UnconditionalStimul;
 //  
 UEPtr<NNeuronTrainer> ConditionalStimul;
 // ,   ""
 UEPtr<NNeuronTrainer> LogicalAndNeuron;
 // "" 
 UEPtr<NPulseNeuron> BigNeuron;
 // ,   ""
 UEPtr<NPulseNeuron> LogicalOrNeuron;

 // ,    
 bool is_stimuls_trained;   	//       
 bool is_interval_found;		//           
 bool is_and_neuron_trained;	//     ""
 bool is_big_neuron_trained;	//     

 //   ,   ,   ""
 double first_spike_time;	//   
 bool is_first_spike;		//  ?
 double second_spike_time;  //   
 bool is_second_spike;		//  ?
 double difference;			// 
 //    Delay  ,   ""
 double ready_time;

 //        "" 
 bool one_spike;
 //     "" 
 int synapses_num;

public: // 
// --------------------------
//   
// --------------------------
NConditionedReflex(void);
virtual ~NConditionedReflex(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///  ,   
bool SetPulseGeneratorClassName(const std::string &value);

///  ,   
bool SetNeuronTrainerClassName(const std::string &value);

///  ,  
bool SetNeuronClassName(const std::string &value);

///  ,  
bool SetSynapseClassName(const std::string &value);

//  "" 
bool SetBigNeuronSize(const int &value);

///   
bool SetNeedToTrain(const bool &value);

///        ()
bool SetDelay(const double &value);

///   ()
bool SetSpikesFrequency(const double &value);

///       
bool SetNumUnconditionalStimulDendrite(const int &value);

///       
bool SetNumConditionalStimulDendrite(const int &value);

///   
bool SetMaxDendriteLength(const int &value);

///       
bool SetUnconditionalStimulPattern(const MDMatrix<double> &value);

///       
bool SetConditionalStimulPattern(const MDMatrix<double> &value);

///  
bool SetNegInfluence(const bool &value);

///    
bool SetLTZThreshold(const double &value);

///     
bool SetFixedLTZThreshold(const double &value);

///        
bool SetUseFixedLTZThreshold(const bool &value);
// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NConditionedReflex* New(void);
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
bool BuildStructure(void);

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
