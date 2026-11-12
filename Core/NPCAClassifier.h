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

#ifndef NPCACLASSIFIER_H
#define NPCACLASSIFIER_H

//#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"
#include "../../Rdk-BasicLib/Core/UMatrixSourceTimeSeries.h"
#include "../../Rdk-CvBasicLib/Core/UCRPrincipalComponentAnalysis.h"
#include "NSpikeClassifier.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// ,    
class RDK_LIB_TYPE NPCAClassifier: public UNet
{
public: // 
///    
/// 0 -    
/// 1 -  .
UProperty<int,NPCAClassifier, ptPubParameter> StructureBuildMode;

///  ,   
UProperty<std::string, NPCAClassifier, ptPubParameter> MatrixSourceTimeSeriesClassName;

///  ,     
UProperty<std::string, NPCAClassifier, ptPubParameter> PCAClassName;

///  ,         
UProperty<std::string, NPCAClassifier, ptPubParameter> SpikeClassifierClassName;

///     
UProperty<std::string, NPCAClassifier,  ptPubParameter> FileName;

///   
UProperty<int, NPCAClassifier,  ptPubParameter> OutComponents;

///  
UProperty<bool, NPCAClassifier,  ptPubParameter> IsCalibrativeDendrite;

///    
UProperty<std::string, NPCAClassifier,  ptPubParameter> OutputFile;

///   
UProperty<int, NPCAClassifier,  ptPubParameter> ColCount;

///   
UProperty<double, NPCAClassifier,  ptPubParameter> TimeWindowSize;

///    
UProperty<int, NPCAClassifier,  ptPubParameter> TrainingPatternInx;

protected:

double counter;

//
UEPtr<UMatrixSourceTimeSeries> MatrixSourceTimeSeries;

//  
UEPtr<UCRPrincipalComponentAnalysis> PCA;

//       
UEPtr<NSpikeClassifier> SpikeClassifier;

///        
//    
bool IsFirstStep;
//   
bool IsLastStep;
//    
ofstream fout;
//     
MDMatrix<double> results;
//      
MDMatrix<double> max_el;
//      
MDMatrix<double> min_el;



public: // 
// --------------------------
//   
// --------------------------
NPCAClassifier(void);
virtual ~NPCAClassifier(void);
// --------------------------

// --------------------------
//   
// --------------------------
///    
bool SetStructureBuildMode(const int &value);

///  ,   
bool SetMatrixSourceTimeSeriesClassName(const std::string &value);

///  ,     
bool SetPCAClassName(const std::string &value);

///  ,         
bool SetSpikeClassifierClassName(const std::string &value);

///  ,   
bool SetFileName(const std::string &value);

///  ,      
bool SetOutComponents(const int &value);

/// ,    
bool SetIsCalibrativeDendrite(const bool &value);

///    
bool SetOutputFile(const std::string &value);

///   
bool SetColCount(const int &value);

///  ,    
bool SetTimeWindowSize(const double &value);

///  ,    ,      
bool SetTrainingPatternInx(const int &value);

// --------------------------

// --------------------------
//     
// --------------------------

// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NPCAClassifier* New(void);
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
bool BuildStructure(int structure_build_mode, const string &matrix_source_time_series_class_name,
                    const string &pca_class_name, const string &spike_classifier_class_name,
                    const string &file_name_class_name,
                    const int &out_components_class_name, const bool &is_calibrative_dendrite,
                    const string &output_file, const int &col_count, const double &time_window_size,
                    const int &training_pattern_inx);

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
