#include "NSdeSolver.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSdeSolver::NSdeSolver(void)
 : NumEquations("NumEquations", this, &NSdeSolver::SetNumEquations)
 , Coeffs("Coeffs", this)
 , InitialCondition("InitialCondition", this)
 , InputCorrTable("InputCorrTable", this)
 , Inputs("Inputs", this)
 , Outputs("Outputs", this)
{
 ChangeLookupPropertyType("TimeStep", ptPubParameter);
}

NSdeSolver::~NSdeSolver(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NSdeSolver::SetNumEquations(const int &value)
{
 Ready = false;
 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NSdeSolver* NSdeSolver::New(void)
{
 return new NSdeSolver;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NSdeSolver::ADefault(void)
{
 NumEquations = 1;
 Inputs->Assign(1,1, 0.0);
 Outputs->Assign(1,1, 0.0);
 Coeffs->Assign(1,1, 1.0);
 InitialCondition->Assign(1,1, 0.0);
 InputCorrTable->Assign(1,2, 1);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NSdeSolver::ABuild(void)
{
 Solver = ode::CreateOdeSolver(ode::Backend::kCpu);

 OdeCpuImpl.SetNumEquations(NumEquations);
 Outputs->Resize(NumEquations,1, 0.0);
 Coeffs->Resize(NumEquations,1, 0.0);
 InitialCondition->Resize(NumEquations,1, 0.0);
 InputCorrTable->Resize(NumEquations,2,1);

 return true;
}

// Сброс процесса счета.
bool NSdeSolver::AReset(void)
{
 for(int i=0; i<NumEquations; i++)
 {
    OdeCpuImpl.SetCoeffs(i, Coeffs(i,0));
    OdeCpuImpl.SetInitialCondition(i, InitialCondition(i,0));
    OdeCpuImpl.SetInputCorrTable(i,std::pair<int,int>(InputCorrTable(i,0),InputCorrTable(i,1)));
 }
 Solver->SetOde(OdeCpuImpl);
 Solver->ResetToInititalCondition();
 auto time_step = 1./TimeStep;
 Solver->SetTimeStep(time_step/10);
 return true;
}

// Выполняет расчет этого объекта
bool NSdeSolver::ACalculate(void)
{
 auto model_time = GetEnvironment()->GetTime().GetDoubleTime();
 auto time_step = 1./TimeStep;
 auto finish_model_time = model_time + time_step;
 OdeCpuImpl = Solver->GetOde();
 for(int i=0;i<NumEquations;i++)
 {
//  auto & data = (*Inputs).GetData();
  auto input_data = (*Inputs)(i,0);
  OdeCpuImpl.SetInputData(i, input_data);
 }
 Solver->SetOde(OdeCpuImpl);

 Solver->Solve(finish_model_time);
 OdeCpuImpl = Solver->GetOde();
 for(int i=0;i<NumEquations;i++)
  Outputs(i,0) = OdeCpuImpl.GetVariables()[i];
 return true;
}
// --------------------------

}
