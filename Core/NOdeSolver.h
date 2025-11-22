#ifndef NSDE_SOLVER_H
#define NSDE_SOLVER_H

#include "NNet.h"

#include "../../../Rdk/ThirdParty/ode-solver/include/OdeSolverBase.hpp"
#include "../../../Rdk/ThirdParty/ode-solver/include/OdeSolverFactory.hpp"


namespace NMSDK {

class RDK_LIB_TYPE NOdeSolver: public UNet
{
public: //  
/// Device
/// 0 - auto (Gpu if possible)
/// 1 - CPU
/// 2 - GPU
UProperty<int, NOdeSolver, ptPubParameter> DeviceMode;

///  
UProperty<int, NOdeSolver, ptPubParameter> NumEquations;

UProperty<MDMatrix<float>, NOdeSolver, ptPubParameter> Coeffs;

UProperty<MDMatrix<float>, NOdeSolver, ptPubParameter> InitialCondition;

UProperty<MDMatrix<int>, NOdeSolver, ptPubParameter> InputCorrTable;

UProperty<float, NOdeSolver, ptPubParameter> InternalTimeStep;

public: //   
UProperty<MDMatrix<double>, NOdeSolver, ptInput | ptPubState> Inputs;

UProperty<MDMatrix<double>, NOdeSolver, ptOutput | ptPubState> Outputs;

protected:

//OdeGpu OdeGpuImpl;
ode::OdeCpu OdeCpuImpl;

std::unique_ptr<ode::OdeSolverBase> Solver;

//MDMatrix<double> InvCoeffs;

public: // 
// --------------------------
//   
// --------------------------
NOdeSolver(void);
virtual ~NOdeSolver(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetInternalTimeStep(const float &value);

bool SetDeviceMode(const int &value);

bool SetNumEquations(const int &value);
// --------------------------

public:
// --------------------------
//    
// --------------------------
//         
virtual NOdeSolver* New(void);
// --------------------------

protected: // Setters
bool SetCoeffs(const MDMatrix<float> &value);

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
