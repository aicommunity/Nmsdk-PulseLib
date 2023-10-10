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
         return true;

        res&=CreateLink("Source","FullMatrix","PCA","TrainingData");
        if(!res)
         return true;
      }
     return true;
}


// Сброс процесса счета.
bool NPCAClassifier::AReset(void)
{
  counter = 0;
  IsFirstStep = true;
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
    if(!IsLastStep)
    {
        int cd = (IsCalibrativeDendrite)? 1 : 0; //проверка на наличие калибровочного дендрита

        if(IsFirstStep)
        {
            std::string file_dir;
            if(GetEnvironment())
            file_dir=GetEnvironment()->GetCurrentDataDir();

            // Открываем файл для записи данных
            std::string output_file = OutputFile;
            output_file = file_dir + output_file;
            fout.open(output_file);

            //Определяем размер выходной матрицы
            results.Resize(MatrixSourceTimeSeries->FullMatrix.GetRows(),OutComponents + cd);
            //Формируем матрицы максимумов и минимумов для каждого признака
            max_el.Resize(1, OutComponents,-1000000000.0);
            min_el.Resize(1, OutComponents,1000000000.0);

            SpikeClassifier->SetActivity(false);
            SpikeClassifier->Reset();

            IsFirstStep = false;
        }
        //Формируем матрицу с данными
        if(MatrixSourceTimeSeries->CurrentLineIndex < MatrixSourceTimeSeries->FullMatrix.GetRows())
        {
            for(int i = 0; i < OutComponents; i++) //пока строка не закончилась
            {
                results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) = PCA->PCAResult(0,i);
                //Ищем максимальный и минимальный элемент по строке
                if (results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) > max_el(0, i))
                {
                    max_el(0, i) = results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i);
                }
                if (results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) < min_el(0, i))
                {
                    min_el(0, i) = results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i);
                }
            }
            //Добавляем значение калибровочного дендрита при необходимости
            if(IsCalibrativeDendrite)
            {
                results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, OutComponents) = TimeWindowSize;
            }

            return true;
        }
        else
        {
            //Нормализум строку
            for(int i = 0; i < MatrixSourceTimeSeries->FullMatrix.GetRows(); i++)
            {
                for(int j = 0; j < OutComponents; j++)
                {
                    if(fabs(max_el(0,j)-min_el(0,j)) > 0.000001)
                    {
                        results(i,j) = (results(i,j)-min_el(0,j))*TimeWindowSize/(max_el(0,j)-min_el(0,j));
                    }
                     else
                        results(i,j) = 0;
                }
                if(IsCalibrativeDendrite)
                {
                    results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, OutComponents) = TimeWindowSize;
                }
            }

            string b = ""; //пустая строка
            MDMatrix<double> res; //матрица с данными для тренировочного набора
            res.Resize(1, OutComponents+cd); //размер матрицы
            for(int i = 0; i < MatrixSourceTimeSeries->FullMatrix.GetRows(); i++)
            {
                //формируем строку
                for(int j = 0; j < OutComponents + cd; j++)
                {
                    string q = std::to_string(results(i, j));
                    b = b+q+"\t";
                }
                //записываем строку в файл
                fout << b << std::endl;
                b="";
                if (TrainingPatternInx == i) //сравниваем индекс строки обучающего набора
                                             //с индексом текущей строки
                    for(int j = 0; j < OutComponents + cd; j++)
                        res(0,j) = results(i,j);
                SpikeClassifier->TrainingPatterns = res; //записываем строку в обучающий набор данных
            }
            fout.close(); //закрываем файл

            //Активным остается третий блок
            SpikeClassifier->SetActivity(true);
            MatrixSourceTimeSeries->SetActivity(false);
            PCA->SetActivity(false);

            IsLastStep = true;
            return true;
        }
    }
    else
        return true;

// --------------------------
}
}
#endif
