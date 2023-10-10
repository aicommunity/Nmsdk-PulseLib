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

#ifndef NPCACLASSIFIER_H
#define NPCACLASSIFIER_H

//#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"
#include "../../Rdk-BasicLib/Core/UMatrixSourceTimeSeries.h"
#include "../../Rdk-CvBasicLib/Core/UCRPrincipalComponentAnalysis.h"
#include "NSpikeClassifier.h"
#include "../../../Rdk/Deploy/Include/rdk.h"
#include <iostream>
#include <fstream>
#include <cstring>

namespace NMSDK {

using namespace RDK;

// Класс, для классификации паттерна данных
class RDK_LIB_TYPE NPCAClassifier: public UNet
{
public: // Параметры
/// Режим сборки структуры нейрона
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка.
ULProperty<int,NPCAClassifier, ptPubParameter> StructureBuildMode;

/// Имя класса, создающего блок матрицы
ULProperty<std::string, NPCAClassifier, ptPubParameter> MatrixSourceTimeSeriesClassName;

/// Имя класса, создающего блок метода главных компонент
ULProperty<std::string, NPCAClassifier, ptPubParameter> PCAClassName;

/// Имя класса, создающего блок обученных нейронов для распознавания заданного паттерна импульсов
ULProperty<std::string, NPCAClassifier, ptPubParameter> SpikeClassifierClassName;

/// Имя файла с исходными данными
ULProperty<std::string, NPCAClassifier,  ptPubParameter> FileName;

/// Число выходных компонент
ULProperty<int, NPCAClassifier,  ptPubParameter> OutComponents;

/// Калибровочный дендрит
ULProperty<bool, NPCAClassifier,  ptPubParameter> IsCalibrativeDendrite;

/// Имя выходного файла компонента
ULProperty<std::string, NPCAClassifier,  ptPubParameter> OutputFile;

/// Размер паттерна данных
ULProperty<int, NPCAClassifier,  ptPubParameter> ColCount;

/// Значение временного окна
ULProperty<double, NPCAClassifier,  ptPubParameter> TimeWindowSize;

/// Индекс обучающего набора данных
ULProperty<int, NPCAClassifier,  ptPubParameter> TrainingPatternInx;

protected:

double counter;

//Матрица
UEPtr<UMatrixSourceTimeSeries> MatrixSourceTimeSeries;

//Метод главных компонентов
UEPtr<UCRPrincipalComponentAnalysis> PCA;

//Группа обученных нейронов для распознавания заданного паттерна импульсов
UEPtr<NSpikeClassifier> SpikeClassifier;

/// Переменные для чтение и записи данных в файл
// Флаг первой итерации расчёта
bool IsFirstStep;
// Флаг прерывания функций
bool IsLastStep;
// Файл с выходными данными
ofstream fout;
// Выходной файл из блока матрицы
MDMatrix<double> results;
// Матрица максимальных элементов в столбцах признаков
MDMatrix<double> max_el;
// Матрица минимальных элементов в столбцах признаков
MDMatrix<double> min_el;



public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPCAClassifier(void);
virtual ~NPCAClassifier(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса, создающего блок матрицы
bool SetMatrixSourceTimeSeriesClassName(const std::string &value);

/// Имя класса, создающего блок метода главных компонент
bool SetPCAClassName(const std::string &value);

/// Имя класса, создающего блок обученных нейронов для распознавания заданного паттерна импульсов
bool SetSpikeClassifierClassName(const std::string &value);

/// Имя класса, создающего имя файла
bool SetFileName(const std::string &value);

/// Имя класса, создающего набор данных после третьего блока
bool SetOutComponents(const int &value);

/// Определяет, есть ли калибровочный дендрит
bool SetIsCalibrativeDendrite(const bool &value);

/// Имя выходного файла компонента
bool SetOutputFile(const std::string &value);

/// Размер паттерна данных
bool SetColCount(const int &value);

/// Имя класса, содержащего значение временного окна
bool SetTimeWindowSize(const double &value);

/// Имя класса, создающего индекс строки данных, которые используем в качестве обучающего набора
bool SetTrainingPatternInx(const int &value);

// --------------------------

// --------------------------
// Методы доступа к временным переменным
// --------------------------

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPCAClassifier* New(void);
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
bool BuildStructure(int structure_build_mode, const string &matrix_source_time_series_class_name,
                    const string &pca_class_name, const string &spike_classifier_class_name,
                    const string &file_name_class_name,
                    const int &out_components_class_name, const bool &is_calibrative_dendrite,
                    const string &output_file, const int &col_count, const double &time_window_size,
                    const int &training_pattern_inx);

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
