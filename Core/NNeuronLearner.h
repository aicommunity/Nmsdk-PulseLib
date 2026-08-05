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

#ifndef NNEURONLEARNER_H
#define NNEURONLEARNER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNeuronLearner: public UNet
{
public:
 ///    
 /// 0 -    
 /// 1 -   ,            
 UProperty<int, NNeuronLearner, ptPubParameter> StructureBuildMode;

 ///    
 UProperty<std::string, NNeuronLearner, ptPubParameter> PulseGeneratorClassName;

 ///   
 UProperty<std::string, NNeuronLearner, ptPubParameter> NeuronClassName;

 ///   
 UProperty<std::string, NNeuronLearner, ptPubParameter> SynapseClassName;

 ///   (, ) :
 ///    IsNeedToTrain  ,  - 
 /// 0 -   NeuronTrainer:  ,    ,
 ///    IsNeedToTrain,   
 /// 1 -  IsNeedToTrain   ,     
 UProperty<int, NNeuronLearner, ptPubParameter> CalculateMode;

 ///     
 UProperty<bool, NNeuronLearner, ptPubParameter> IsNeedToTrain;

 ///        ()
 UProperty<double, NNeuronLearner, ptPubParameter> Delay;

 ///   ()
 UProperty<double, NNeuronLearner, ptPubParameter> SpikesFrequency;

 ///   
 UProperty<int, NNeuronLearner, ptPubParameter> NumInputDendrite;

 ///   
 UProperty<int, NNeuronLearner, ptPubParameter> MaxDendriteLength;

 ///   ,   TTFS -       
 UProperty<MDMatrix<double>, NNeuronLearner, ptPubParameter> InputPattern;

 ///  ,        
 UProperty<MDMatrix<double>, NNeuronLearner, ptPubParameter> AdditionalInputPattern;

 ///  ,      
 MDMatrix<double> PrevInputPattern;

 ///     ,      
 /// 0 - DendriticSumPotential -       
 /// i = 1..N -        i- 
 UProperty<MDMatrix<double>,NNeuronLearner,ptOutput | ptPubState> DendriteNeuronAmplitude;

 ///     ,     
 /// 0 -        
 /// i = 1..N -      i-  
 UProperty<MDMatrix<double>,NNeuronLearner,ptOutput | ptPubState> SomaNeuronAmplitude;

 ///     
 UProperty<double, NNeuronLearner, ptPubParameter> LTZThreshold;

 ///   (  )    
 UProperty<double, NNeuronLearner, ptPubParameter> FixedLTZThreshold;

 ///     ,    
 UProperty<double, NNeuronLearner, ptPubParameter> TrainingLTZThreshold;

 ///        
 UProperty<bool, NNeuronLearner, ptPubParameter> UseFixedLTZThreshold;

 ///  
 UProperty<MDMatrix<double>, NNeuronLearner, ptOutput | ptPubState> Output;

 ///       
 UProperty<double, NNeuronLearner, ptPubParameter> SynapseResistanceStep;

 ///     
 UProperty<std::vector<int>, NNeuronLearner, ptPubParameter> DendriteLength;

 ///     ,    .
 ///  ,       ,
 ///    
 std::vector<int> OldDendriteLength;

 ///        ,      ,
 ///                 
 UProperty<std::vector<double>, NNeuronLearner, ptPubParameter> InitialSomaPotential;

 ///      
 UProperty<std::vector<int>, NNeuronLearner, ptPubParameter> NumSynapse;

 ///  
 /// 2 - LearningSecondPattern() -  . 
 UProperty<int, NNeuronLearner, ptPubParameter> ExperimentNum;
 
 ///   
 UProperty<bool, NNeuronLearner, ptPubParameter> ExperimentMode;

 ///   DEBUG-  :      
 ///   ,       
 UProperty<bool, NNeuronLearner, ptPubParameter> EnableDebug;

protected:
 ///     
 int OldNumInputDendrite;

 ///  
 std::vector<UEPtr<NPulseGeneratorTransit> > Generators;

 ///  
 UEPtr <NPulseNeuron> Neuron;

 ///     (  ) ()
 double StartIterTime;


 //     

 ///    
 ifstream Fin;

 ///    
 ofstream Fout;

 ///    
 MDMatrix<double> Inputs;

 ///  
 std::vector<int> Outputs;

 ///      
 bool IsWritten;

 ///      
 bool IsFirstFileStep;

 ///  .  - ,   
 ///     
 int CountIteration;


 //     

 ///         
 ///         
 ///    
 bool IsFirstBeat;

 ///   
 double IterLength;

 ///       ,
 ///     
 std::vector<double> MaxIterSomaAmp;

 ///         
 std::vector<double> TimeOfMaxIterSomaAmp;

 ///      -   
 ///           
 std::vector<double> Dissynchronization;

 ///          
 ///          
 std::vector<double> AmpDifference;

 ///   -   ,      :
 /// 0  -   ;
 /// 1  -      ;
 /// -1 -      
 std::vector<int> DendStatus;

 ///   -   ,    .   :
 /// 0  -   ;
 /// 1  -        ;
 /// -1 -        
 std::vector<int> SynapseStatus;


 //   

 ///   .   -   - ,
 ///        
 int EpochCur;

 /// ,    
 bool CanChangeDendLength;

 ///           
 double SumMaxIterSomaAmp;

 ///           
 ///    
 double PrevSumMaxIterSomaAmp;


public: // 
 // --------------------------
 //   
 // --------------------------

 NNeuronLearner(void);
 virtual ~NNeuronLearner(void);

 // --------------------------



 // --------------------------
 //   
 // --------------------------

 ///     
 bool SetStructureBuildMode(const int &value);

 ///     
 bool SetPulseGeneratorClassName(const std::string &value);

 ///    
 bool SetNeuronClassName(const std::string &value);

 ///    
 bool SetSynapseClassName(const std::string &value);

 ///    (, ) :
 ///    IsNeedToTrain  ,  - 
 /// 0 -   NeuronTrainer:  ,    ,
 ///    IsNeedToTrain,   
 /// 1 -  IsNeedToTrain   ,     
 bool SetCalculateMode(const int &value);

 ///  (  )     
 bool SetIsNeedToTrain(const bool &value);

 ///         ()
 bool SetDelay(const double &value);

 ///    ()
 bool SetSpikesFrequency(const double &value);

 ///    
 bool SetNumInputDendrite(const int &value);

 ///      (=    )
 bool SetMaxDendriteLength(const int &value);

 ///    ,   TTFS (      )
 bool SetInputPattern(const MDMatrix<double> &value);

 ///   ,        
 bool SetAdditionalInputPattern(const MDMatrix<double> &value);

 ///  ,      
 bool SetPrevInputPattern(const MDMatrix<double> &value);

 ///      
 bool SetLTZThreshold(const double &value);

 ///      ,    
 bool SetTrainingLTZThreshold(const double &value);

 ///    (  )    
 bool SetFixedLTZThreshold(const double &value);

 ///         
 bool SetUseFixedLTZThreshold(const bool &value);

 ///         
 bool SetSynapseResistanceStep(const double &value);

 ///    
 bool SetExperimentMode(const bool &value);

 ///      (   )
 bool SetDendriteLength(const std::vector<int> &value);

 ///        ,
 ///      ,    
 ///             
 bool SetInitialSomaPotential(const std::vector<double> &value);

 ///        
 bool SetNumSynapse(const std::vector<int> &value);

 ///   
 /// 1 - PatternRecognition()
 /// 2 - LearningSecondPattern()
 bool SetExperimentNum(const int &value);

 ///   DEBUG-  
 bool SetEnableDebug(const bool &value);

 // --------------------------



 // --------------------------
 //    
 // --------------------------

 ///         
 virtual NNeuronLearner* New(void);
 static UComponent* NewStatic(void);

 // --------------------------



protected:
 // --------------------------
 //    
 // --------------------------

 ///    
 ///       
 //      comp      
 virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

 ///    
 ///       
 //      comp    
 virtual bool ADelComponent(UEPtr<UContainer> comp);

 // --------------------------



protected:
 // --------------------------
 //    
 // --------------------------

 ///         
 bool BuildStructure();

 ///        
 virtual bool ADefault(void);

 ///        
 //    Reset()   Ready  true
 //    
 virtual bool ABuild(void);

 /// Sources (generators) before Neuron — same calc contract as NNeuronTrainer.
 void UpdateComputationOrder(void) override;

 ///   
 virtual bool AReset(void);

 ///     
 /// prev_input_pattern -      
 /// input_pattern -      
 /// e -    ,      
 bool CompareInputPatterns(MDMatrix<double> prev_input_pattern, MDMatrix<double> input_pattern, double e);

 ///    ,   ,     
 bool ZeroingTrainingPattern(void);

 ///    (   ) num
 bool ChangeDendriteLength(int num);

 ///         num
 bool ChangeSynapseNumber(int num);

 ///       .    ,
 ///         
 ///    
 bool MeasureMaxPotentialAndTime(void);

 ///         .
 ///     
 bool ChangeDendriteStatus(int num);

 ///         .
 ///     
 bool ChangeSynapseStatus(int num);

 /// ,       
 bool PatternRecognition(void);

 /// ,     
 ///        .   -   .
 ///      :           
 ///  ,   4-      ,     
 ///       
 bool LearningAdditionalPattern_1_4(MDMatrix<double> second_pattern);

 ///   :
 ///    ,      
 bool IncrementalLearning(MDMatrix<double> InitialPattern, MDMatrix<double> second_pattern);

 ///      ,  ..    
 /// 1  - PatternRecognition() -    
 /// 2  - LearningAdditionalPattern_1_4() -   
 bool Experiment(void);

 ///   
 bool EndOfLearning(void);

 ///    NeuronLearner -  
 bool Training(void);

 ///   NeuronLearner
 virtual bool ACalculate(void);

 // --------------------------
};
}

#endif // NNEURONLEARNER_H
