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

// �����, ��� ������������� �������� ������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
 IsFirstStep = true; //������ ��������
 IsLastStep = false; //��������� ��������
}

NPCAClassifier::~NPCAClassifier(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NPCAClassifier::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������, ���������� ���� �������
bool NPCAClassifier::SetMatrixSourceTimeSeriesClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ���� ��� ������ ������� ���������
bool NPCAClassifier::SetPCAClassName(const std::string &value)
{
    Ready=false;
 return true;
}

/// ��� ������, ���������� ������ ��������� �������� ��� ������������� ��������� �������� ���������
bool NPCAClassifier::SetSpikeClassifierClassName(const std::string &value)
{
    Ready=false;
 return true;
}

/// ��� ����� � ��������� �������
bool NPCAClassifier::SetFileName(const std::string &value)
{
 Ready=false;
 if(MatrixSourceTimeSeries)
 {
     MatrixSourceTimeSeries->FileName = value;
 }
 return true;
}

/// ����� ����������� � PCA
bool NPCAClassifier::SetOutComponents(const int &value)
{
 Ready=false;
 if(PCA)
 {
     PCA->OutComponents = value;
 }
 return true;
}

/// ���� �� ������������� �������
bool NPCAClassifier::SetIsCalibrativeDendrite(const bool &value)
{
    Ready=false;
 return true;
}

/// ��� ��������� ����� ����������
bool NPCAClassifier::SetOutputFile(const std::string &value)
{
    Ready=false;
 return true;
}

/// ������ �������� ������
bool NPCAClassifier::SetColCount(const int &value)
{
    Ready=false;
    if(MatrixSourceTimeSeries)
    {
        MatrixSourceTimeSeries->ColCount = value;
    }
    return true;
}

/// ������ ���������� ����
bool NPCAClassifier::SetTimeWindowSize(const double &value)
{
 Ready=false;
 return true;
}

/// ������ ������ ������, ������� ���������� � �������� ���������� ������
bool NPCAClassifier::SetTrainingPatternInx(const int &value)
{
 Ready=false;
 return true;
}

// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPCAClassifier::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPCAClassifier::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
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

        //������� ���� �������
        MatrixSourceTimeSeries = AddMissingComponent<UMatrixSourceTimeSeries>(std::string("Source"), matrix_source_time_series_class_name);
        if(MatrixSourceTimeSeries)
        {
         MatrixSourceTimeSeries->SetCoord(MVector<double,3>(4.3, 2.67, 0));
         MatrixSourceTimeSeries->FileName = FileName;
         MatrixSourceTimeSeries->ColCount = ColCount;
        }

        //������� ���� PCA
        PCA = AddMissingComponent<UCRPrincipalComponentAnalysis>(std::string("PCA"), pca_class_name);
        if(PCA)
        {
         PCA->SetCoord(MVector<double,3>(12.3, 2.67, 0));
         PCA->OutComponents = OutComponents;
        }

        //������� ���� ��������� �������� ��� ������������� ��������� �������� ���������
        SpikeClassifier = AddMissingComponent<NSpikeClassifier>(std::string("SpikeClassifier"), spike_classifier_class_name);
        if(SpikeClassifier)
        {
         SpikeClassifier->SetCoord(MVector<double,3>(20.3, 2.67, 0));
        }
        if(IsCalibrativeDendrite) //��� ������� �������������� �������
                                  //����� �������� ���������� ������������� �� 1,
                                  //��������� ����� ���������
            {
            SpikeClassifier->NumInputDendrite = OutComponents+1;
            }
            else
            {
                SpikeClassifier->NumInputDendrite = OutComponents;
            }
        SpikeClassifier->SetActivity(false);
        SpikeClassifier->Reset();

        //������� ����� ����� ������ ������� � PCA
        res&=CreateLink("Source","CurrentLine","PCA","EncodingData");
        if(!res)
         return true;

        res&=CreateLink("Source","FullMatrix","PCA","TrainingData");
        if(!res)
         return true;
      }
     return true;
}


// ����� �������� �����.
bool NPCAClassifier::AReset(void)
{
  counter = 0;
  IsFirstStep = true;
return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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


// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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
if(SpikeClassifier)
{
 SpikeClassifier->DataFromFile = true;
 SpikeClassifier->SetActivity(false);
}
return true;
}

// ��������� ������ ����� �������
bool NPCAClassifier::ACalculate(void)
{
    if(!IsLastStep)
    {
        int cd = (IsCalibrativeDendrite)? 1 : 0; //�������� �� ������� �������������� ��������

        if(IsFirstStep)
        {
            std::string file_dir;
            if(GetEnvironment())
            file_dir=GetEnvironment()->GetCurrentDataDir();

            // ��������� ���� ��� ������ ������
            std::string output_file = OutputFile;
            output_file = file_dir + output_file;
            fout.open(output_file);

            //���������� ������ �������� �������
            results.Resize(MatrixSourceTimeSeries->FullMatrix.GetRows(),OutComponents + cd);
            //��������� ������� ���������� � ��������� ��� ������� ��������
            max_el.Resize(1, OutComponents,-1000000000.0);
            min_el.Resize(1, OutComponents,1000000000.0);

            SpikeClassifier->SetActivity(false);
            SpikeClassifier->Reset();

            IsFirstStep = false;
        }
        //��������� ������� � �������
        if(MatrixSourceTimeSeries->CurrentLineIndex < MatrixSourceTimeSeries->FullMatrix.GetRows())
        {
            for(int i = 0; i < OutComponents; i++) //���� ������ �� �����������
            {
                results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) = PCA->PCAResult(0,i);
                //���� ������������ � ����������� ������� �� ������
                if (results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) > max_el(0, i))
                {
                    max_el(0, i) = results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i);
                }
                if (results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i) < min_el(0, i))
                {
                    min_el(0, i) = results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, i);
                }
            }
            //��������� �������� �������������� �������� ��� �������������
            if(IsCalibrativeDendrite)
            {
                results(MatrixSourceTimeSeries->CurrentLineIndex.v - 1, OutComponents) = TimeWindowSize;
            }

            return true;
        }
        else
        {
            //���������� ������
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

            string b = ""; //������ ������
            MDMatrix<double> res; //������� � ������� ��� �������������� ������
            res.Resize(1, OutComponents+cd); //������ �������
            for(int i = 0; i < MatrixSourceTimeSeries->FullMatrix.GetRows(); i++)
            {
                //��������� ������
                for(int j = 0; j < OutComponents + cd; j++)
                {
                    string q = std::to_string(results(i, j));
                    b = b+q+"\t";
                }
                //���������� ������ � ����
                fout << b << std::endl;
                b="";
                if (TrainingPatternInx == i) //���������� ������ ������ ���������� ������
                                             //� �������� ������� ������
                    for(int j = 0; j < OutComponents + cd; j++)
                        res(0,j) = results(i,j);
                SpikeClassifier->TrainingPatterns = res; //���������� ������ � ��������� ����� ������
            }
            fout.close(); //��������� ����

            //�������� �������� ������ ����
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
