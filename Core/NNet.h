// ===========================================================
// Version:        3.0.3
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

#ifndef NNET_H
#define NNET_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNet: public UNet
{
public: //  
typedef vector<vector<vector<vector<vector<UId> > > > > NNetStructure;
//   
RDK::UProperty<double, NNet, ptPubParameter> LinkDistance;

public: //  
//      
RDK::UProperty<UId,NNet, ptPubParameter> DefaultComponentClassId;

//  
// 0 -  
// 1 -  
// 2 -   
RDK::UProperty<int,NNet, ptPubParameter> NetType;

//  
RDK::UProperty<size_t,NNet, ptPubParameter> NumLayers;

//  
RDK::UProperty<size_t,NNet, ptPubParameter> LayerWidth;

//  
RDK::UProperty<size_t,NNet, ptPubParameter> LayerHeight;

//   
RDK::UProperty<size_t,NNet, ptPubParameter> ReceptorFieldWidth;

//   
RDK::UProperty<size_t,NNet, ptPubParameter> ReceptorFieldHeight;

//    
// 0 - 
// 1 -  -        'LinkDistance'
// 2 -  -     
// 3 -       'LinkDistance'
RDK::UProperty<size_t, NNet, ptPubParameter> LinksOrganizationMode;

protected: // 
//  
RDK::UProperty<NNetStructure,NNet,ptPubState> NetStructure;

public: // 
// --------------------------
//   
// --------------------------
NNet(void);
virtual ~NNet(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NNet* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
//           
RDK::UEPtr<UContainer> InsertComponent(void);
// --------------------------

// --------------------------
//   
// --------------------------
public:
//        
virtual bool Default(void);

//     
//   
//    Reset()   Ready  true
//    
virtual bool Build(void);

//      
virtual bool Reset(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//   
virtual bool BuildNetStructure(void);

//    'layer'
virtual bool BuildLayerStructure(size_t layer);

//      'layer',  
// 'i'  , 'j'  
virtual bool BuildReceptorField(size_t layer, size_t i, size_t j);

//   
virtual bool BuildLinksStructure(void);

//     'layer',  
// 'i'  , 'j'  
virtual bool BuildLinksStructure(size_t layer, size_t i, size_t j);

//    ,  LinksOrganizationMode
// 0 - 
virtual bool BuildLinksStructureNone(size_t layer, size_t i, size_t j);

//    ,  LinksOrganizationMode
// 1 -  -        'LinkDistance'
virtual bool BuildLinksStructureMultilayer(size_t layer, size_t i, size_t j);

//    ,  LinksOrganizationMode
// 2 -  -     
virtual bool BuildLinksStructureReceptor(size_t layer, size_t i, size_t j);

//    ,  LinksOrganizationMode
// 3 -       'LinkDistance'
virtual bool BuildLinksStructureFullRadius(size_t layer, size_t i, size_t j);
// --------------------------
};

}
#endif


