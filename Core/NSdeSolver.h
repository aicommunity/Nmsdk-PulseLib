#ifndef NSDE_SOLVER_H
#define NSDE_SOLVER_H

#include "NNet.h"

#include "../../../Rdk/ThirdParty/sde-solver/include/OdeSolverBase.hpp"
#include "../../../Rdk/ThirdParty/sde-solver/include/OdeSolverFactory.hpp"


namespace NMSDK {

class RDK_LIB_TYPE NSdeSolver: public UNet
{
public: // Общедоступные свойства
/// Число уравнений
UProperty<int, NSdeSolver, ptPubParameter> NumEquations;

UProperty<MDMatrix<double>, NSdeSolver, ptPubParameter> Coeffs;

UProperty<MDMatrix<double>, NSdeSolver, ptPubParameter> InitialCondition;

UProperty<MDMatrix<int>, NSdeSolver, ptPubParameter> InputCorrTable;

public: // Входы и выходы
UPropertyInputData<MDMatrix<double>, NSdeSolver, ptInput | ptPubState> Inputs;

UPropertyOutputData<MDMatrix<double>, NSdeSolver, ptOutput | ptPubState> Outputs;

protected:

//OdeGpu OdeGpuImpl;
ode::OdeCpu OdeCpuImpl;

std::unique_ptr<ode::OdeSolverBase> Solver;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSdeSolver(void);
virtual ~NSdeSolver(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool SetNumEquations(const int &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSdeSolver* New(void);
// --------------------------

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
