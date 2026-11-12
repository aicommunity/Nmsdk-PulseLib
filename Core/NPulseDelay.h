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

#ifndef NPULSE_DELAY_H
#define NPULSE_DELAY_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseDelay: public NSource
{
public: // 

///  
UProperty<MDMatrix<double>,NPulseDelay, ptInput | ptPubState> Input;
///     
UProperty<double,NPulseDelay, ptPubParameter> DelayTime;


protected: //  

//    
std::list<MDMatrix<double> > buffer;

//     
int desired_buffer_length;

public: //   


public: // 
// --------------------------
//   
// --------------------------
NPulseDelay(void);
virtual ~NPulseDelay(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------

//   
bool SetDelayTime(const double &value);


public:
// --------------------------
//    
// --------------------------
//         
virtual NPulseDelay* New(void);
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

