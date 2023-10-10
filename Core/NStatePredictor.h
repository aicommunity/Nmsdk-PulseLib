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

#ifndef NSTATEPREDICTOR_H
#define NSTATEPREDICTOR_H

//#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"
//#include "NNeuronTrainer.h"

#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "NNeuronLearner.h"
#include "NNeuronTrainer.h"
#include "NPredictor.h"
#include "NLogicalNot.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// Класс, создающий группу нейронов для моделирования условного рефлекса
class RDK_LIB_TYPE NStatePredictor: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// 0 - автоматическая сборка не производится
/// 1 - Создаём группу нейронов для моделирования условного рефлекса.
/// Нейроны организованы в качестве 2 каналов информации +
/// промежуточный слой, реализующий возникновение условного рефлекса.
/// Первый канал: безусловный раздражитель -> безусловный рефлекс
/// Второй канал: условный раздражитель
/// На первом этапе обучаются входные нейроны 2-х каналов, каждый своему паттерну импульсов.
/// На втором этапе обучается элемент "И" промежуточного слоя, который
/// должен реагировать единичным выходным импульсом при активации двух входов одновременно.
/// На третьем этапе происходит наращивание синапсов на большом нейроне до тех пор,
/// пока большой нейрон не начинает генерировать сигналы. Генерация сигнала большим
/// нейроном вызывает формирование связи выхода нейрона 2-ого канала на элемент
/// "ИЛИ" 1-ого канала.
ULProperty<int,NStatePredictor, ptPubParameter> StructureBuildMode;

///// Имя класса нейрона
ULProperty<std::string, NStatePredictor, ptPubParameter> NeuronClassName;

///// Имя класса синапса
ULProperty<std::string, NStatePredictor, ptPubParameter> SynapseClassName;

/// Имя класса предиктора
ULProperty<std::string, NStatePredictor, ptPubParameter> PredictorClassName;

/// Имя класса обучаемого нейрона
ULProperty<std::string, NStatePredictor, ptPubParameter> NeuronLearnerClassName;

/// Имя класса блока боелвых ощущений
ULProperty<std::string, NStatePredictor, ptPubParameter> LogicalNotClassName;

/// Режим ввода входных значений
/// 0 - ввод матрицы входных значений производится вручную
/// 1 - использование внешнего источника сигнала
ULProperty<int, NStatePredictor, ptPubParameter> InputMode;

/// Признак необходимости обучения
ULProperty<bool, NStatePredictor, ptPubParameter> IsNeedToTrain;

/// Паттерн для запоминания. Содержит запаздывание входного сигнала на текущем
/// дендрите (индекс матрицы), относительно сигнала, который пришёл первым в сек.
ULProperty<MDMatrix<double>, NStatePredictor, ptPubParameter> InputPattern;

///Матрица входных значений (используется в ручном режиме)
///Строки содержат N значений элемента входного вектора в моменты времени t, t-tau, ... t-(N-1)tau,
///где tau - интервал между измерениями состояния входа, N - число измерений
///Столбцы содержат M значений, составляющих входной вектор. M - размерность вектора
///ВАЖНО! Все вводимые значения должны принадлежать отрезку [0; 0,2],
/// это важно для правильной работы обучаемого нейрона
ULProperty<MDMatrix<double>, NStatePredictor, ptPubParameter> InputMatrix;

////Число входных нейронов =
/// = количеству столбцов матрицы входных значений
ULProperty<int, NStatePredictor, ptPubParameter> StatesNumber;

////Число признаков, описывающих состояние =
/// = количеству строк матрицы входных значений
ULProperty<int, NStatePredictor, ptPubParameter> FeaturesNumber;

///Интервал tau между измерениями состояния входа (в режиме работы с внешним источником сигнала)
ULProperty<double, NStatePredictor, ptPubParameter> MeasurementPeriod;

/// Входной сигнал (альтернатива матрице входных значений, используется в режиме работы с внешним источником сигнала)
UPropertyInputData<MDMatrix<double>,NStatePredictor, ptInput | ptPubState> Input;

/// Результат вычислений, предсказанное значение
UPropertyOutputData<MDMatrix<double>, NStatePredictor, ptOutput | ptPubState> Output;


protected:

//Входные нейроны
//std::vector<UEPtr<NNeuronTrainer>> InputNeurons;

//Блок предиктора
UEPtr<NPredictor> Predictor;

//Обучаемый выходной нейрон
//UEPtr<NNeuronLearner> OutputNeuron;
//UEPtr<NNeuronTrainer> OutputNeuron;

//Блок болевых ощущений
UEPtr<NLogicalNot> LogicalNot;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NStatePredictor(void);
virtual ~NStatePredictor(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса, создающего синапс
bool SetSynapseClassName(const std::string &value);

/// Имя класса, создающего нейрон
bool SetNeuronClassName(const std::string &value);

/// Имя класса, создающего предиктор
bool SetPredictorClassName(const std::string &value);

/// Имя класса, создающего обучаемый нейрон
bool SetNeuronLearnerClassName(const std::string &value);

/// Имя класса, создающего блок боелвых ощущений
bool SetLogicalNotClassName(const std::string &value);

/// Режим расчёта нейрона
bool SetInputMode(const int &value);

/// Признак необходимости обучения
bool SetNeedToTrain(const bool &value);

/// Паттерн для запоминания
bool SetInputPattern(const MDMatrix<double> &value);

/// Матрица входных значений
bool SetInputMatrix(const MDMatrix<double> &value);

////Число входных нейронов =
/// = количеству столбцов матрицы входных значений
bool SetStatesNumber(const int &value);

///Число признаков, описывающих состояние =
/// = количеству строк матрицы входных значений
bool SetFeaturesNumber(const int &value);

///Интервал tau между измерениями состояния входа
/// (в режиме работы с источником сигнала)
bool SetMeasurementPeriod(const double &value);



// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NStatePredictor* New(void);
static UComponent* NewStatic(void);
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
/// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool BuildStructure();

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
