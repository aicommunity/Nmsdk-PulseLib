// ===========================================================
// Version:        3.0.2
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

#ifndef NPREDICTOR_CPP
#define NPREDICTOR_CPP

#include "NPredictor.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include <QString>


// Класс, создающий группу нейронов для моделирования условного рефлекса
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPredictor::NPredictor(void)
: StructureBuildMode("StructureBuildMode",this,&NPredictor::SetStructureBuildMode),
  InputMode("InputMode",this,&NPredictor::SetInputMode),
  InputMatrix("InputMatrix",this,&NPredictor::SetInputMatrix),
  InputCols("InputCols",this,&NPredictor::SetInputCols),
  InputRows("InputRows",this,&NPredictor::SetInputRows),
  MeasurementPeriod("MeasurementPeriod",this,&NPredictor::SetMeasurementPeriod),
  Input("Input",this),
  Output("Output",this)

{

}

NPredictor::~NPredictor(void)
{
}
// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool NPredictor::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Режим расчёта
/// 0 - ручной ввод матрицы входных значений
/// 1 - при подключении внешнего блока-источника
bool NPredictor::SetInputMode(const int &value)
{
 return true;
}


/// Матрица входных значений
bool NPredictor::SetInputMatrix(const MDMatrix<double> &value)
{    
    Ready=false;
    return true;
}


bool NPredictor::SetInputCols(const int &value)
{  
    if (value>=1)
    {
        Ready=false;
        InputMatrix.Resize(InputRows, value);
    }
 return true;
}


bool NPredictor::SetInputRows(const int &value)
{
    if (value>=1)
    {
        Ready=false;
        InputMatrix.Resize(value, InputCols);
        Output.Resize(value, 1);
    }
   return true;
}

///Интервал tau между измерениями состояния входа
/// (в режиме работы с источником сигнала)
bool NPredictor::SetMeasurementPeriod(const double &value)
{
 Ready=false;
 return true;
}


// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPredictor* NPredictor::New(void)
{
 return new NPredictor;
}

UComponent* NPredictor::NewStatic(void)
{
 return new NPredictor;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPredictor::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPredictor::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов


// Сброс процесса счета.
bool NPredictor::AReset(void)
{
    InputMatrix.Resize(InputRows, InputCols);

    return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPredictor::ADefault(void)
{
    InputMode = 0;

    InputRows = 1;
    InputCols = 5;
    InputMatrix.Resize(InputRows, InputCols);

    Input->Assign(1,1,0.0);
    measurement_period_start = 0;
    MeasurementPeriod = 0.01;
    between_measurements = false;
    k = (InputCols-1);

    return true;
}


// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPredictor::ABuild(void)
{
// if(StructureBuildMode>0)
// {

// }
 return true;
}


double NPredictor::Predict(MDMatrix<double> &input_pattern)
{
    int input_size = input_pattern.GetSize();
    int num_of_layers = input_size - 1;
    double output_sum = input_pattern[0];

    MDMatrix<double> prev_differences;
    prev_differences.Resize(input_size,1);
    prev_differences = input_pattern;

    for(int k = 0; k < num_of_layers; k++)
    {
        const int end = num_of_layers - k;
        MDMatrix<double> differences;
        differences.Resize(end,1);

        for (int j = 0; j<end; j++)
        {
           differences[j] = prev_differences[j+1] - prev_differences[j];
        }

        output_sum = output_sum - differences[0];
        prev_differences.Resize(end,1);
        prev_differences = differences;
    }
     return output_sum;
}


// Выполняет расчет этого объекта
bool NPredictor::ACalculate(void)
{
    input_pattern.Resize(1, InputCols);

    //При использовании матрицы входных значений
    if(InputMode == 0)
    {
        Output.Resize(InputRows, 1);

        for (int i = 0; i < InputRows; i++)
        {
            input_pattern = InputMatrix->GetRow(i);
            Output(i,0) = Predict(input_pattern);
        }
    }

    //При использовании внешнего источника
    else if(InputMode == 1)
    {
        Output.Resize(1, 1);
        double time_from_start = Environment->GetTime().GetDoubleTime() - measurement_period_start;

         //Определение переменных на первой итерации расчетов
        if (time_from_start <= 0)
        {
            measurement_period_start = Environment->GetTime().GetDoubleTime();
            between_measurements = true;
        }

        //По истечении периода tau между измерениями
        else if(time_from_start >= MeasurementPeriod)
        {
            //Начало нового периода
            if (between_measurements == false)
            {
                measurement_period_start = Environment->GetTime().GetDoubleTime();
                between_measurements = true;
            }
            //Запись значения в input_pattern
            else
            {
                between_measurements = false;

                input_pattern[k] = Input()(0,0);
                k--;

                //Набрано необходимое число значений в input_pattern - производим расчет
                if (k == -1)
                {
                    Output(0,0) = Predict(input_pattern);
                    for (int i = InputCols-2; i>=0; i--)
                    {
                        input_pattern[i+1] = input_pattern[i];

                    }
                    k = 0;
                }
            }
        }
  }



	return true;
}
// --------------------------


}
#endif
