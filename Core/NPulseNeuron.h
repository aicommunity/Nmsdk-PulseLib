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

#ifndef NPULSE_NEURON_H
#define NPULSE_NEURON_H

#include "NPulseNeuronCommon.h"
#include "NPulseMembrane.h"
#include "NConstGenerator.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseNeuron: public NPulseNeuronCommon
{
public: // 
///    
/// (  >1)         
/// 0 -    
/// 1 -       
/// 2 -          

/// 3 -          
/// 4 -          
///   
/// 5 -         
/// 6 -         
///   
/// 7 -          
///    
/// 8 -          ,
///      
/// 9 -         ,
///    
/// 10 -         ,
///      
UProperty<int,NPulseNeuron, ptPubParameter> StructureBuildMode;

///    
UProperty<std::string, NPulseNeuron, ptPubParameter> MembraneClassName;

///        
UProperty<std::string, NPulseNeuron, ptPubParameter> LTMembraneClassName;

///    
UProperty<std::string, NPulseNeuron, ptPubParameter> LTZoneClassName;

///         
UProperty<std::string, NPulseNeuron, ptPubParameter> ExcGeneratorClassName;

///         
UProperty<std::string, NPulseNeuron, ptPubParameter> InhGeneratorClassName;

///     
UProperty<int, NPulseNeuron, ptPubParameter> NumSomaMembraneParts;

///     (  )
UProperty<int, NPulseNeuron, ptPubParameter> NumDendriteMembraneParts;

///     (  )
UProperty<std::vector<int>, NPulseNeuron, ptPubParameter> NumDendriteMembranePartsVec;

/// ,   
/// (    Size = 0)
///  !    !!!
UProperty<MDMatrix<double>, NPulseNeuron, ptPubParameter> TrainingPattern;

///     
/// (    Size = 0)
///  !    !!!
UProperty<MDMatrix<int>, NPulseNeuron, ptPubParameter> TrainingDendIndexes;

///       
/// (    Size = 0)
///  !    !!!
UProperty<MDMatrix<int>, NPulseNeuron, ptPubParameter> TrainingSynapsisNum;


protected:
NConstGenerator *PosGenerator,*NegGenerator;

vector<UEPtr<NPulseMembraneCommon> > Soma;

//       
int OldNumDendrites, OldNumSoma;

vector<int> OldNumDendritesVec;

int OldStructureBuildMode;

public: // 
// --------------------------
//   
// --------------------------
NPulseNeuron(void);
virtual ~NPulseNeuron(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///    
bool SetMembraneClassName(const std::string &value);

///        
bool SetLTMembraneClassName(const std::string &value);

///    
bool SetLTZoneClassName(const std::string &value);

///         
bool SetExcGeneratorClassName(const std::string &value);

///         
bool SetInhGeneratorClassName(const std::string &value);

///     
bool SetNumSomaMembraneParts(const int &value);

///     (  )
bool SetNumDendriteMembraneParts(const int &value);

///     (  )
bool SetNumDendriteMembranePartsVec(const std::vector<int> &value);

/// ,   
bool SetTrainingPattern(const MDMatrix<double> &value);

///     
bool SetTrainingDendIndexes(const MDMatrix<int> &value);

///       
bool SetTrainingSynapsisNum(const MDMatrix<int> &value);
// --------------------------

// --------------------------
//     
// --------------------------
//       
NConstGenerator* GetPosGenerator(void);

//       
NConstGenerator* GetNegGenerator(void);

///    
size_t GetNumMembranes(void) const;
NPulseMembrane* GetMembrane(size_t i);
// --------------------------

// --------------------------
//    
// --------------------------
//    ,      ,
//        
//     
NPulseMembraneCommon* ElongateDendrite(const string &name, bool feedback=false);

//    ,     
//    
//     
NPulseMembraneCommon* BranchDendrite(const string &name, bool feedback=false);

//    
//  full == true,      ,   
//        
bool EraseDendrite(const string &name);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPulseNeuron* New(void);
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
//       
//     
//   'true'   
//  'false'    
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
//    
// --------------------------
protected:
///         
/// dendrite_mode  2 :
/// 1 -      ( dendrite_length)
/// 2 -      ( dendrite_length_vec, dendrite_length
///      )
bool BuildStructure(const string &membraneclass, const string &ltzonemembraneclass,
					const string &ltzone_class, const string &pos_gen_class,
					const string &neg_gen_class, int num_soma_membranes, int dendrite_mode,
					int dendrite_length, const vector<int> &dendrite_length_vec, int num_stimulates, int num_arresting);

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

//      
void UpdateComputationOrder(void);
// --------------------------
public:
int GetNumOfConnectedSynToPosCh(NPulseMembrane* membr);
int GetNumOfConnectedSynToNegCh(NPulseMembrane* membr);
};

}
#endif
