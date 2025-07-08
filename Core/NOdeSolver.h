#ifndef NSDE_SOLVER_H
#define NSDE_SOLVER_H

#include "NNet.h"

#include "../../../Rdk/ThirdParty/ode-solver/include/OdeSolverBase.hpp"
#include "../../../Rdk/ThirdParty/ode-solver/include/OdeSolverFactory.hpp"


namespace NMSDK {

class RDK_LIB_TYPE NOdeSolver: public UNet
{
public: // Общедоступные свойства
/// Device
/// 0 - auto (Gpu if possible)
/// 1 - CPU
/// 2 - GPU
UProperty<int, NOdeSolver, ptPubParameter> DeviceMode;

/// Число уравнений
UProperty<int, NOdeSolver, ptPubParameter> NumEquations;

UProperty<MDMatrix<double>, NOdeSolver, ptPubParameter> Coeffs;

UProperty<MDMatrix<double>, NOdeSolver, ptPubParameter> InitialCondition;

UProperty<MDMatrix<int>, NOdeSolver, ptPubParameter> InputCorrTable;

UProperty<double, NOdeSolver, ptPubParameter> InternalTimeStep;

public: // Входы и выходы
UPropertyInputData<MDMatrix<double>, NOdeSolver, ptInput | ptPubState> Inputs;

UPropertyOutputData<MDMatrix<double>, NOdeSolver, ptOutput | ptPubState> Outputs;

protected:

//OdeGpu OdeGpuImpl;
ode::OdeCpu OdeCpuImpl;

std::unique_ptr<ode::OdeSolverBase> Solver;

//MDMatrix<double> InvCoeffs;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NOdeSolver(void);
virtual ~NOdeSolver(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetInternalTimeStep(const double &value);

bool SetDeviceMode(const int &value);

bool SetNumEquations(const int &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NOdeSolver* New(void);
// --------------------------

protected: // Setters
bool SetCoeffs(const MDMatrix<double> &value);

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}

#endif
