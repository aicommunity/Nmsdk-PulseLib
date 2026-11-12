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

#ifndef NPainReflexSimple_H
#define NPainReflexSimple_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// ,       
class RDK_LIB_TYPE NPainReflexSimple: public UNet
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
UProperty<int,NPainReflexSimple, ptPubParameter> StructureBuildMode;

///  ,   
UProperty<std::string, NPainReflexSimple, ptPubParameter> PulseGeneratorClassName;

///  ,   
UProperty<std::string, NPainReflexSimple, ptPubParameter> NeuronTrainerClassName;

///   
UProperty<std::string, NPainReflexSimple, ptPubParameter> NeuronClassName;

///   
UProperty<std::string, NPainReflexSimple, ptPubParameter> SynapseClassName;

///  "" 
//UProperty<int, NPainReflexSimple, ptPubParameter> BigNeuronSize;

///   
UProperty<bool, NPainReflexSimple, ptPubParameter> IsNeedToTrain;

///        ()
UProperty<double, NPainReflexSimple, ptPubParameter> Delay;

///   ()
UProperty<double, NPainReflexSimple, ptPubParameter> SpikesFrequency;

///       
//UProperty<int, NPainReflexSimple, ptPubParameter> NumUnconditionalStimulDendrite;

///       
UProperty<int, NPainReflexSimple, ptPubParameter> NumConditionalStimulDendrite;

///      
UProperty<int, NPainReflexSimple, ptPubParameter> MaxDendriteLength;

///      .
///      
///  ( ),  ,     .
///        !!!
//UProperty<MDMatrix<double>, NPainReflexSimple, ptPubParameter> UnconditionalStimulPattern;

///      .
///      
///  ( ),  ,     .
///        !!!
//UProperty<MDMatrix<double>, NPainReflexSimple, ptPubParameter> ConditionalStimulPattern;

///  
/// false -  
/// true -  
UProperty<bool, NPainReflexSimple, ptPubParameter> IsNegInfluence;

///    
UProperty<double, NPainReflexSimple, ptPubParameter> LTZThreshold;

///     
UProperty<double, NPainReflexSimple, ptPubParameter> FixedLTZThreshold;

///        
UProperty<bool, NPainReflexSimple, ptPubParameter> UseFixedLTZThreshold;

/// "" ,    -     
UProperty<MDMatrix<double>, NPainReflexSimple, ptInput | ptPubState> PainInput;


/// "" - (,  )
//UProperty<MDMatrix<double>, NNeuronTrainer, ptInput | ptPubState> NormalInput;

/// ""  ( ,   ,  , 
///  ,   )
//UProperty<MDMatrix<double>, NNeuronTrainer, ptInput | ptPubState> AlternativeInput;

///  ( )  - 
//UProperty<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> NormalOutput;

///  ( )  - ,   
//UProperty<MDMatrix<double>, NNeuronTrainer, ptOutput | ptPubState> PainOutput;

protected:

 //     
 //int OldNumNeurons;
 //     
 //int OldNumInputDendrite;

 //  
 //UEPtr<NNeuronTrainer> UnconditionalStimul;
 //  
 UEPtr<NNeuronTrainer> ConditionalStimul;
 // ,   ""
 //UEPtr<NNeuronTrainer> LogicalAndNeuron;
 // "" 
 //UEPtr<NPulseNeuron> BigNeuron;

 // , / "" 
 UEPtr<NPulseNeuron> NormalOutputNeuron;

 // , / "" 
 UEPtr<NPulseNeuron> AltOutputNeuron;

 //,  NormalOutputNeuron   ,      
 UEPtr<NPulseGeneratorTransit> NormalBlocker;

 //,  PainOutputNeuron   ,     "" 
 UEPtr<NPulseGeneratorTransit> AltBlocker;

 // ,    ""  ()
 UEPtr<NPulseGeneratorTransit> NormalInputGen;

 // ,    ""  ()
 UEPtr<NPulseGeneratorTransit> AltInputGen;

 // ,    
 bool is_conditional_stimulus_trained; //    

 /*
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
 */

 double start_iter_time;
 bool is_first_iter;

public: // 
// --------------------------
//   
// --------------------------
NPainReflexSimple(void);
virtual ~NPainReflexSimple(void);
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
//bool SetBigNeuronSize(const int &value);

///   
bool SetNeedToTrain(const bool &value);

///        ()
bool SetDelay(const double &value);

///   ()
bool SetSpikesFrequency(const double &value);

///       
//bool SetNumUnconditionalStimulDendrite(const int &value);

///       
bool SetNumConditionalStimulDendrite(const int &value);

///   
bool SetMaxDendriteLength(const int &value);

///       
//bool SetUnconditionalStimulPattern(const MDMatrix<double> &value);

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
virtual NPainReflexSimple* New(void);
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
