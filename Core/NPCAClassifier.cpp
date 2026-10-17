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

#ifndef NPCACLASSIFIER_CPP
#define NPCACLASSIFIER_CPP

#include "NNeuronTrainer.h"
#include "NPCAClassifier.h"
#include "../../Rdk-BasicLib/Core/UMatrixSourceTimeSeries.h"
#include "../../Rdk-CvBasicLib/Core/UCRPrincipalComponentAnalysis.h"
#include "../../Nmsdk-PulseLib/Deploy/Include/Lib.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include <QString>

// Класс, для классификации паттерна данных
namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPCAClassifier::NPCAClassifier(void)
: StructureBuildMode("StructureBuildMode",this,&NPCAClassifier::SetStructureBuildMode),
  MatrixSourceTimeSeriesClassName("MatrixSourceTimeSeriesClassName",this,&NPCAClassifier::SetMatrixSourceTimeSeriesClassName),
  PCAClassName("PCAClassName",this,&NPCAClassifier::SetPCAClassName),
  SpikeClassifierClassName("SpikeClassifierClassName",this,&NPCAClassifier::SetSpikeClassifierClassName),
  FileName("FileName",this,&NPCAClassifier::SetFileName),
  OutComponents("OutComponents",this,&NPCAClassifier::SetOutComponents),
  IsCalibrativeDendrite("IsCalibrativeDendrite",this,&NPCAClassifier::SetIsCalibrativeDendrite),
  OutputFile("OutputFile",this,&NPCAClassifier::SetOutputFile),
  ColCount("ColCount",this, &NPCAClassifier::SetColCount),
  TimeWindowSize("TimeWindowSize",this, &NPCAClassifier::SetTimeWindowSize),
  TrainingPatternInx("TrainingPatternInx",this, &NPCAClassifier::SetTrainingPatternInx)
{
 IsFirstStep = true; //первая итерация
 IsLastStep = false; //последняя итерация
}

NPCAClassifier::~NPCAClassifier(void)
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
bool NPCAClassifier::SetStructureBuildMode(const int &value)
{
 if(value >0) // Пересборка структуры нужна только если StructureBuildMode не 0
  Ready=false;
 return true;
}

/// Имя класса, создающего блок матрицы
bool NPCAClassifier::SetMatrixSourceTimeSeriesClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// Имя класса, создающего блок для метода главных компонент
bool NPCAClassifier::SetPCAClassName(const std::string &value)
{
    Ready=false;
 return true;
}

/// Имя класса, создающего группу обученных нейронов для распознавания заданного паттерна импульсов
bool NPCAClassifier::SetSpikeClassifierClassName(const std::string &value)
{
    Ready=false;
 return true;
}

/// Имя файла с исходными данными
bool NPCAClassifier::SetFileName(const std::string &value)
{
 Ready=false;
 if(MatrixSourceTimeSeries)
 {
     MatrixSourceTimeSeries->FileName = value;
 }
 return true;
}

/// Число компоненент в PCA
bool NPCAClassifier::SetOutComponents(const int &value)
{
 Ready=false;
 if(PCA)
 {
     PCA->OutComponents = value;
 }
 return true;
}

/// Есть ли калибровочный дендрит
bool NPCAClassifier::SetIsCalibrativeDendrite(const bool &value)
{
    Ready=false;
 return true;
}

/// Имя выходного файла компонента
bool NPCAClassifier::SetOutputFile(const std::string &value)
{
    Ready=false;
 return true;
}

/// Размер паттерна данных
bool NPCAClassifier::SetColCount(const int &value)
{
    Ready=false;
    if(MatrixSourceTimeSeries)
    {
        MatrixSourceTimeSeries->ColCount = value;
    }
    return true;
}

/// Размер временного окна
bool NPCAClassifier::SetTimeWindowSize(const double &value)
{
 Ready=false;
 return true;
}

/// Индекс строки данных, которые используем в качестве обучающего набора
bool NPCAClassifier::SetTrainingPatternInx(const int &value)
{
 Ready=false;
 return true;
}

// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPCAClassifier* NPCAClassifier::New(void)
{
 return new NPCAClassifier;
}

UComponent* NPCAClassifier::NewStatic(void)
{
 return new NPCAClassifier;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPCAClassifier::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPCAClassifier::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Осуществляет сборку структуры в соответствии с выбранными именами компонентов
bool NPCAClassifier::BuildStructure(int structure_build_mode, const string &matrix_source_time_series_class_name,
                                    const string &pca_class_name, const string &spike_classifier_class_name,
                                    const string &file_name_class_name,
                                    const int &out_components_class_name, const bool &is_calibrative_dendrite,
                                    const string &output_file, const int &col_count, const double &time_window_size,
                                    const int &training_pattern_inx)
{
    if(StructureBuildMode == 1)
    {


        bool res = true;

        //Создаем блок матрицы
        MatrixSourceTimeSeries = AddMissingComponent<UMatrixSourceTimeSeries>(std::string("Source"), matrix_source_time_series_class_name);
        MatrixSourceTimeSeries->SetCoord(MVector<double,3>(4.3, 2.67, 0));
        MatrixSourceTimeSeries->FileName = FileName;
        MatrixSourceTimeSeries->ColCount = ColCount;

        //Создаем блок PCA
        PCA = AddMissingComponent<UCRPrincipalComponentAnalysis>(std::string("PCA"), pca_class_name);
        PCA->SetCoord(MVector<double,3>(12.3, 2.67, 0));
        PCA->OutComponents = OutComponents;

        //Создаем блок обученных нейронов для распознавания заданного паттерна импульсов
        SpikeClassifier = AddMissingComponent<NSpikeClassifier>(std::string("SpikeClassifier"), spike_classifier_class_name);
        SpikeClassifier->SetCoord(MVector<double,3>(20.3, 2.67, 0));
        if(IsCalibrativeDendrite) //при наличии калибровочного нейрона
                                  //число выходных параметров увеличивается на 1,
                                  //формируем число компонент
            {
            SpikeClassifier->NumInputDendrite = OutComponents+1;
            }
            else
            {
                SpikeClassifier->NumInputDendrite = OutComponents;
            }
        SpikeClassifier->SetActivity(false);
        SpikeClassifier->Reset();

        //Создаем связи между блоком матрицы и PCA
        res&=CreateLink("Source","CurrentLine","PCA","EncodingData");
        if(!res)
         return false;

        res&=CreateLink("Source","FullMatrix","PCA","TrainingData");
        if(!res)
         return false;
      }
     return true;
}


// Сброс процесса счета.
bool NPCAClassifier::AReset(void)
{
  counter = 0;
  IsFirstStep = true;
  IsLastStep = false;
  // A saved model can contain the post-run state (source/PCA off, classifier
  // on). Restore the preparation phase before child calculation begins.
  MatrixSourceTimeSeries->SetActivity(true);
  PCA->SetActivity(true);
  MatrixSourceTimeSeries->Init();
  PCA->Init();
  SpikeClassifier->DataFromFile = true;
  SpikeClassifier->SetActivity(false);
return true;
}

// Восстановление настроек по умолчанию и сброс процесса счета
bool NPCAClassifier::ADefault(void)
{
 StructureBuildMode=1;
 MatrixSourceTimeSeriesClassName="UMatrixSourceTimeSeries";
 PCAClassName="UCRPrincipalComponentAnalysis";
 SpikeClassifierClassName = "NSpikeClassifier";
 FileName="testing_input.txt";
 OutComponents=5;
 IsCalibrativeDendrite = true;
 OutputFile="input_data.txt";
 ColCount = 21;
 TimeWindowSize = 0.2;
 TrainingPatternInx = 0;
 return true;
}


// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPCAClassifier::ABuild(void)
{
 if(StructureBuildMode>0)
 {
  bool res=BuildStructure(StructureBuildMode, MatrixSourceTimeSeriesClassName, PCAClassName, SpikeClassifierClassName,
                          FileName, OutComponents, IsCalibrativeDendrite, OutputFile, ColCount,
                          TimeWindowSize, TrainingPatternInx);
  if(!res)
   return false;
 }
 SpikeClassifier->DataFromFile = true;
 SpikeClassifier->SetActivity(false);
 return true;
}

// Выполняет расчет этого объекта
bool NPCAClassifier::ACalculate(void)
{
    if(IsLastStep)
        return true;

    const int calibration_dendrite = IsCalibrativeDendrite ? 1 : 0;

    if(IsFirstStep)
    {
        // A restored project may keep these children inactive after an earlier
        // completed pass. Wake them before checking FullMatrix; the source may
        // need one calculation step to load it, so wait instead of treating
        // that initial empty state as a failed PCA input.
        MatrixSourceTimeSeries->SetActivity(true);
        PCA->SetActivity(true);
        MatrixSourceTimeSeries->Init();
        PCA->Init();

        const int row_count = MatrixSourceTimeSeries->FullMatrix.GetRows();
        if(row_count <= 0)
            return true;

        std::string file_dir;
        if(GetEnvironment())
            file_dir = GetEnvironment()->GetCurrentDataDir();

        fout.open(file_dir + OutputFile.GetData());

        results.Resize(row_count, OutComponents + calibration_dendrite);
        max_el.Resize(1, OutComponents, -1000000000.0);
        min_el.Resize(1, OutComponents, 1000000000.0);

        SpikeClassifier->SetActivity(false);
        SpikeClassifier->Reset();
        IsFirstStep = false;
    }

    const int row_count = MatrixSourceTimeSeries->FullMatrix.GetRows();
    const int source_line_index = MatrixSourceTimeSeries->CurrentLineIndex.GetData();
    const int current_row = source_line_index - 1;

    // CurrentLineIndex is one-based. Include row_count itself: it is the
    // final valid source row, not an end-of-stream marker.
    if(current_row < 0 || current_row >= row_count)
        return true;

    for(int component_idx = 0; component_idx < OutComponents; component_idx++)
    {
        const double result_value = PCA->PCAResult(0, component_idx);
        results(current_row, component_idx) = result_value;
        if(result_value > max_el(0, component_idx))
            max_el(0, component_idx) = result_value;
        if(result_value < min_el(0, component_idx))
            min_el(0, component_idx) = result_value;
    }

    if(IsCalibrativeDendrite)
        results(current_row, OutComponents) = TimeWindowSize;

    // Normalize and hand off only after the final source row has been copied.
    if(current_row + 1 < row_count)
        return true;

    for(int row_idx = 0; row_idx < row_count; row_idx++)
    {
        for(int component_idx = 0; component_idx < OutComponents; component_idx++)
        {
            const double range = max_el(0, component_idx) - min_el(0, component_idx);
            if(fabs(range) > 0.000001)
                results(row_idx, component_idx) =
                    (results(row_idx, component_idx) - min_el(0, component_idx)) * TimeWindowSize / range;
            else
                results(row_idx, component_idx) = 0.0;
        }
    }

    MDMatrix<double> training_pattern;
    training_pattern.Resize(1, OutComponents + calibration_dendrite);
    if(TrainingPatternInx < 0 || TrainingPatternInx >= row_count)
    {
        LogMessageEx(RDK_EX_ERROR, __FUNCTION__, "TrainingPatternInx is outside the PCA result matrix.");
        return false;
    }

    for(int row_idx = 0; row_idx < row_count; row_idx++)
    {
        std::string line;
        for(int component_idx = 0; component_idx < OutComponents + calibration_dendrite; component_idx++)
            line += std::to_string(results(row_idx, component_idx)) + "\t";
        fout << line << std::endl;

        if(TrainingPatternInx == row_idx)
            for(int component_idx = 0; component_idx < OutComponents + calibration_dendrite; component_idx++)
                training_pattern(0, component_idx) = results(row_idx, component_idx);
    }

    SpikeClassifier->TrainingPatterns = training_pattern;
    fout.close();

    SpikeClassifier->SetActivity(true);
    MatrixSourceTimeSeries->SetActivity(false);
    PCA->SetActivity(false);
    IsLastStep = true;
    return true;

// --------------------------
}
}
#endif
