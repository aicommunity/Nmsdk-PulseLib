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
 InputCorrTable->Assign(1,3, 0);
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NSdeSolver::ABuild(void)
{
 OdeCpuImpl.SetNumEquations(NumEquations);
 OdeGpuImpl.SetNumEquations(NumEquations);
 Outputs->Resize(NumEquations,1);
 Coeffs->Resize(NumEquations,1);
 InitialCondition->Resize(NumEquations,1);
 InputCorrTable->Resize(NumEquations,3);
 // Inputs->Resize(NumEquations,1);

 return true;
}

// Сброс процесса счета.
bool NSdeSolver::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool NSdeSolver::ACalculate(void)
{
 return true;
}
// --------------------------

}
