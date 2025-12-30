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

#ifndef NAFFERENT_NEURON_H
#define NAFFERENT_NEURON_H

#include "NPulseNeuron.h"


namespace NMSDK {

class RDK_LIB_TYPE NAfferentNeuron: public NPulseNeuronCommon
{
public: //  
///    
/// (  >1)         
/// 0 -    
/// 1 -      (  )
/// 2 -      (+ )
UProperty<int,NAfferentNeuron, ptPubParameter> StructureBuildMode;

///    
UProperty<std::string, NAfferentNeuron, ptPubParameter> MembraneClassName;

///    
UProperty<std::string, NAfferentNeuron, ptPubParameter> LTZoneClassName;

///   
UProperty<std::string, NAfferentNeuron, ptPubParameter> ReceptorClassName;

///         
UProperty<std::string, NAfferentNeuron, ptPubParameter> ExcGeneratorClassName;

///         
//UProperty<std::string, NAfferentNeuron, ptPubParameter> InhGeneratorClassName;

///     
UProperty<int, NAfferentNeuron, ptPubParameter> NumSomaMembraneParts;

///     
UProperty<double, NAfferentNeuron, ptPubParameter> MaxReceptorOutput;

public: //  
//RDK::UEPointer<NReceptor,NPulseNeuronCommon> Receptor;

public: // 
// --------------------------
//   
// --------------------------
NAfferentNeuron(void);
virtual ~NAfferentNeuron(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///    
bool SetMembraneClassName(const std::string &value);

///    
bool SetLTZoneClassName(const std::string &value);

///   
bool SetReceptorClassName(const std::string &value);

///         
bool SetExcGeneratorClassName(const std::string &value);

///         
//bool SetInhGeneratorClassName(const std::string &value);

///     
bool SetNumSomaMembraneParts(const int &value);

///     
bool SetMaxReceptorOutput(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NAfferentNeuron* New(void);
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
bool BuildClassicalStructure(const string &membraneclass, const string &ltzone_class,
					const string &receptorclass, const string &pos_gen_class,
					/*const string &neg_gen_class,*/ int num_membranes);

///    
bool BuildSimpleStructure(const string &ltzone_class,
					const string &receptorclass, double max_output);

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
