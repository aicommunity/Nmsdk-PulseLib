#ifndef NSYNAPSE_TRAINER_H
#define NSYNAPSE_TRAINER_H

#include "NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NSynapseTrainer: public RDK::UNet
{
public: //   
///     
UProperty<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PostSynInput;

///     
UProperty<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PreSynInput;

///    STDP
UProperty<MDMatrix<double>, NSynapseTrainer, ptOutput | ptPubState> WeightOutput;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainer(void);
virtual ~NSynapseTrainer(void);

public:
// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainer* New(void);

public:
//     
virtual void RebuildInternalLinks(void);

};

}

#endif
