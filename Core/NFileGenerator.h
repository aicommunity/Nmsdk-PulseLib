// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NFILE_GENERATOR_H
#define NFILE_GENERATOR_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NFileGenerator: public NSource
{
public: //  
//    
UProperty<string,NFileGenerator, ptPubParameter> FileName;

public: //   

protected: // 
RDK::UIniFile<char> Ini;

// 2D      
map<double,MDMatrix<double> > Data;

public: //  
// ,     
map<double,MDMatrix<double> >::iterator CurrentIndex;

//     
double DataStartTime;


public: // 
// --------------------------
//   
// --------------------------
NFileGenerator(void);
virtual ~NFileGenerator(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//     
bool SetFileName(const string &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NFileGenerator* New(void);
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

