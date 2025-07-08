#include "NOdeSolver.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NOdeSolver::NOdeSolver(void)
 : DeviceMode("DeviceMode", this, &NOdeSolver::SetDeviceMode)
 , NumEquations("NumEquations", this, &NOdeSolver::SetNumEquations)
 , Coeffs("Coeffs", this, &NOdeSolver::SetCoeffs)
 , InternalTimeStep("InternalTimeStep", this, &NOdeSolver::SetInternalTimeStep)
 , InitialCondition("InitialCondition", this)
 , InputCorrTable("InputCorrTable", this)
 , Inputs("Inputs", this)
 , Outputs("Outputs", this)
{
 ChangeLookupPropertyType("TimeStep", ptPubParameter);
}

NOdeSolver::~NOdeSolver(void)
{
}
// --------------------------


// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool NOdeSolver::SetDeviceMode(const int &value)
{
 Ready = false;
 return true;
}

// Устанавливает амплитуду импульсов
bool NOdeSolver::SetNumEquations(const int &value)
{
 Ready = false;
 return true;
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NOdeSolver* NOdeSolver::New(void)
{
 return new NOdeSolver;
}
// --------------------------

bool NOdeSolver::SetInternalTimeStep(const double &value)
{
  if(value <= 0)
    return false;
  Ready=false;
  return true;
}

bool NOdeSolver::SetCoeffs(const MDMatrix<double> &value)
{
/* InvCoeffs.Resize(value.GetRows(), value.GetCols());
 for(int i=0;i<value.GetRows()*value.GetCols();i++)
 {
  if(fabs(value[i]) > 0)
    InvCoeffs[i] = 1.0/value[i];
  else
    InvCoeffs[i] = 0.0;
 }*/
 Ready = false;
 return true;
}

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NOdeSolver::ADefault(void)
{
 DeviceMode = 0;
 InternalTimeStep = 0.0005;
 NumEquations = 1;
 Inputs->Assign(1,1, 0.0);
 Outputs->Assign(1,1, 0.0);
 Coeffs->Assign(1,3, 1.0);
// InvCoeffs.Assign(1,3, 0.1);
 InitialCondition->Assign(1,1, 0.0);
 InputCorrTable->Assign(1,2, 1);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NOdeSolver::ABuild(void)
{
 if(Solver)
   Solver.reset();

 ode::Backend backend(ode::Backend::kAuto);
 switch(DeviceMode)
 {
 case 0:
    backend = ode::Backend::kAuto;
 break;

 case 1:
    backend = ode::Backend::kCpu;
 break;

 case 2:
    backend = ode::Backend::kGpu;
 break;
 }

 Solver = ode::CreateOdeSolver(backend);

 OdeCpuImpl.SetNumEquations(NumEquations);
 Outputs->Resize(NumEquations,1, 0.0);
 Coeffs->Resize(NumEquations,3, 1.0);
 //InvCoeffs.Resize(NumEquations,3, 0.1);
 InitialCondition->Resize(NumEquations,1, 0.0);
 InputCorrTable->Resize(NumEquations,2,1);
 Solver->SetTimeStep(InternalTimeStep);

 return true;
}

// Сброс процесса счета.
bool NOdeSolver::AReset(void)
{
 for(int i=0; i<NumEquations; i++)
 {
    OdeCpuImpl.SetCoeffs(i, Coeffs(i,0), Coeffs(i,1), Coeffs(i,2));
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
bool NOdeSolver::ACalculate(void)
{
 double model_time = GetEnvironment()->GetTime().GetDoubleTime();
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
