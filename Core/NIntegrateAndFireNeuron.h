#ifndef INTEGRATED_FIRE_NEURON_H
#define INTEGRATED_FIRE_NEURON_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuronCommon.h"

namespace NMSDK
{
    class NIntegrateAndFireNeuron: public RDK::UNet
    {
        // 
        protected:
        //  
		UProperty<double, NIntegrateAndFireNeuron, ptPubParameter> C;

		//   
		UProperty<double, NIntegrateAndFireNeuron, ptPubParameter> U0;

		//   
        UProperty<double, NIntegrateAndFireNeuron, ptPubParameter> Upr;

        protected:
        //  
        //   
        UProperty<MDMatrix<double>, NIntegrateAndFireNeuron, ptOutput | ptPubState> U;

        //  
        UProperty<MDMatrix<double>, NIntegrateAndFireNeuron, ptInput | ptPubState> I;

        // 
        public:
        // 
        NIntegrateAndFireNeuron(void);
        // 
        virtual ~NIntegrateAndFireNeuron(void);
        //         
        virtual NIntegrateAndFireNeuron* New(void);

        //    
        protected:
        //        
        virtual bool ADefault(void);

        //     
        //   
        //    Reset()   Ready  true
        //    
        virtual bool ABuild(void);

        //      
        virtual bool AReset(void);

        //    
        virtual bool ACalculate(void);

    };
}


#endif // NIntegrateAndFireNeuron

