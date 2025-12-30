#ifndef NRESULT_DECODER_H
#define NRESULT_DECODER_H

#include "NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NResultDecoder: public RDK::UNet
{

//   

public: //   
///   
UProperty<std::vector<MDMatrix<double>>, NResultDecoder, ptInput | ptPubState> PerseptronInput;

/// 
UProperty<MDMatrix<double>, NResultDecoder, ptOutput | ptPubState> Result;

public: // 
// --------------------------
//   
// --------------------------
NResultDecoder(void);
virtual ~NResultDecoder(void);

public:
// --------------------------
//    
// --------------------------
//         
virtual NResultDecoder* New(void);

protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);


};

}

#endif
