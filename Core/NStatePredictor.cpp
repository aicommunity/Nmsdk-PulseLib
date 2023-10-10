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

#ifndef NSTATEPREDICTOR_CPP
#define NSTATEPREDICTOR_CPP

#include "NStatePredictor.h"
#include "NNeuronLearner.h"
#include "NNeuronTrainer.h"
#include "NLogicalNot.h"
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
NStatePredictor::NStatePredictor(void)
: StructureBuildMode("StructureBuildMode",this,&NStatePredictor::SetStructureBuildMode),
  NeuronClassName("NeuronClassName",this,&NStatePredictor::SetNeuronClassName),
  SynapseClassName("SynapseClassName",this,&NStatePredictor::SetSynapseClassName),
  PredictorClassName("PredictorClassName",this,&NStatePredictor::SetPredictorClassName),
  NeuronLearnerClassName("NeuronLearnerClassName",this,&NStatePredictor::SetNeuronLearnerClassName),
  LogicalNotClassName("LogicalNotClassName",this,&NStatePredictor::SetLogicalNotClassName),
  InputMode("InputMode",this,&NStatePredictor::SetInputMode),
  IsNeedToTrain("IsNeedToTrain",this,&NStatePredictor::SetNeedToTrain),
  InputPattern("InputPattern",this,&NStatePredictor::SetInputPattern),
  InputMatrix("InputMatrix",this,&NStatePredictor::SetInputMatrix),
  StatesNumber("StatesNumber",this,&NStatePredictor::SetStatesNumber),
  FeaturesNumber("FeaturesNumber",this,&NStatePredictor::SetFeaturesNumber),
  MeasurementPeriod("MeasurementPeriod",this,&NStatePredictor::SetMeasurementPeriod),
  Input("Input",this),
  Output("Output",this)

{

}

NStatePredictor::~NStatePredictor(void)
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
bool NStatePredictor::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего нейрон
bool NStatePredictor::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего предиктор
bool NStatePredictor::SetPredictorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего обучаемый нейрон
bool NStatePredictor::SetNeuronLearnerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего блок болевых ощущений
bool NStatePredictor::SetLogicalNotClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего синапс
bool NStatePredictor::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// Режим расчёта
/// 0 - ручной ввод матрицы входных значений
/// 1 - при подключении внешнего блока-источника
bool NStatePredictor::SetInputMode(const int &value)
{
    Ready=false;
    if(Predictor)
    {
        Predictor->InputMode = value;
        Predictor->Reset();
    }
    return true;
}

/// Признак необходимости обучения
bool NStatePredictor::SetNeedToTrain(const bool &value)
{
    if(LogicalNot)
    {
        if(LogicalNot->IsNeedToTrain)
        {
            Ready=false;
        }
        LogicalNot->IsNeedToTrain = value;
    }
    return true;
}

// Паттерн для запоминания
bool NStatePredictor::SetInputPattern(const MDMatrix<double> &value)
{
    if(LogicalNot)
    {
        if(LogicalNot->IsNeedToTrain)
        {
            Ready=false;
        }
        LogicalNot->InputPattern = value;
    }
  return true;
}


/// Матрица входных значений (используется в ручном режиме)
/// Строки содержат N значений элемента входного вектора в моменты времени t, t-tau, ... t-(N-1)tau,
/// где tau - интервал между измерениями состояния входа, N - число измерений
/// Столбцы содержат M значений, составляющих входной вектор. M - размерность вектора
/// ВАЖНО! Все вводимые значения должны принадлежать отрезку [0; 0,2],
/// это важно для правильной работы обучаемого нейрона
bool NStatePredictor::SetInputMatrix(const MDMatrix<double> &value)
{
    Ready=false;
    if(Predictor)
    {
      Predictor->InputMatrix.Resize(FeaturesNumber, StatesNumber);
      Predictor->InputMatrix = value;
      Predictor->Reset();
    }
    return true;
}

///Число входных нейронов = числу столбцов матрицы входных значений
bool NStatePredictor::SetStatesNumber(const int &value)
{
    Ready=false;
    if (Predictor)
    {
        Predictor->InputCols = value;
        Predictor->InputMatrix.Resize(FeaturesNumber, Predictor->InputCols);
        InputMatrix.Resize(FeaturesNumber, Predictor->InputCols);
    }
 return true;
}

///Число признаков, описывающих состояние = числу строк матрицы входных значений
bool NStatePredictor::SetFeaturesNumber(const int &value)
{
    Ready=false;
    InputPattern.Resize(value,1);

    if (Predictor)
    {
        Predictor->InputRows = value;
        Predictor->InputMatrix.Resize(Predictor->InputRows, StatesNumber);
        InputMatrix.Resize(Predictor->InputRows, StatesNumber);
    }

    if (LogicalNot)
    {
        LogicalNot->NumInputDendrite = value;
        LogicalNot->Reset();
        LogicalNot->InputPattern.Resize(value, 1);

    }

   return true;
}

///Интервал tau между измерениями состояния входа
/// (в режиме работы с источником сигнала)
bool NStatePredictor::SetMeasurementPeriod(const double &value)
{
    Ready=false;
    if (Predictor)
    {
        Predictor->MeasurementPeriod = value;
    }
 return true;
}



// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NStatePredictor* NStatePredictor::New(void)
{
 return new NStatePredictor;
}

UComponent* NStatePredictor::NewStatic(void)
{
 return new NStatePredictor;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NStatePredictor::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NStatePredictor::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NStatePredictor::BuildStructure()
{
    if(StructureBuildMode == 1)
    {
        //Создаем предиктор
        Predictor = AddMissingComponent<NPredictor>(std::string("Predictor"), PredictorClassName);
        Predictor->SetCoord(MVector<double,3>(5, 2, 0));
        Predictor->InputCols = StatesNumber;
        Predictor->InputRows = FeaturesNumber;
        Predictor->InputMatrix.Resize(FeaturesNumber, StatesNumber);
        Predictor->MeasurementPeriod = MeasurementPeriod;
        Predictor->InputMode = InputMode;
        Predictor->Reset();

        //Создаем блок болевых ощущений
        LogicalNot = AddMissingComponent<NLogicalNot>(std::string("LogicalNot"), LogicalNotClassName);
        LogicalNot->SetCoord(MVector<double,3>(5, 5, 0));
        LogicalNot->InputPattern.Resize(FeaturesNumber, 1);
        LogicalNot->NeuronTrainer->NumInputDendrite = FeaturesNumber;
        LogicalNot->Reset();

//      //Создаем выходной нейрон
//      //OutputNeuron = AddMissingComponent<NNeuronLearner>(std::string("OutputNeuron"), NeuronLearnerClassName);
//      OutputNeuron = AddMissingComponent<NNeuronTrainer>(std::string("OutputNeuron"), NeuronLearnerClassName);
//      OutputNeuron->SetCoord(MVector<double,3>(22, 9.5, 0));
//      OutputNeuron->NumInputDendrite = FeaturesNumber;
//      OutputNeuron->InputPattern.Resize(FeaturesNumber, 1);
//      OutputNeuron->Reset();




     }
     return true;
}


// Сброс процесса счета.
bool NStatePredictor::AReset(void)
{
     InputMatrix.Resize(FeaturesNumber, StatesNumber);
     //OutputNeuron->InputPattern.Resize(FeaturesNumber, 1);
     LogicalNot->InputPattern.Resize(FeaturesNumber, 1);

     return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NStatePredictor::ADefault(void)
{
    StructureBuildMode=1;

    NeuronClassName="NNeuronTrainer";
    SynapseClassName="NPSynapseBio";
    PredictorClassName="NPredictor";
    NeuronLearnerClassName="NNeuronTrainer";
    //NeuronLearnerClassName="NNeuronLearner";
    LogicalNotClassName="NLogicalNot";


    InputMode = 0;
    FeaturesNumber = 5; // = числу элементов вектора состояний (InputRows в Predictor)
    StatesNumber = 4; // = числу измерений каждого элемента вектора состояний (InputCols в Predictor)
    InputMatrix.Resize(FeaturesNumber, StatesNumber);
    Input->Assign(1,1,0.0);

    IsNeedToTrain = true;
    InputPattern.Resize(FeaturesNumber,1);

    if (LogicalNot)
    {
         LogicalNot->NumInputDendrite = FeaturesNumber;
         LogicalNot->NeuronTrainer->NumInputDendrite = FeaturesNumber;
    }

    return true;
}


// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NStatePredictor::ABuild(void)
{
    if(StructureBuildMode>0)
    {
        bool res=BuildStructure();
        if(!res)
         return false;
    }

 return true;
}

// Выполняет расчет этого объекта
bool NStatePredictor::ACalculate(void)
{
    IsNeedToTrain = LogicalNot->IsNeedToTrain;
    InputPattern =  LogicalNot->InputPattern;

   //Передача значений с выхода предиктора на вход LogicalNot
   if (!IsNeedToTrain)
   {
       LogicalNot->InputPattern = Predictor->Output;
   }

   //Передача значенй с выхода LogicalNot к выходу блока
    Output = LogicalNot->OutputNeuron->LTZone->Output;

    return true;
}
// --------------------------


}
#endif
