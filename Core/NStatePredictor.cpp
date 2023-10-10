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


// �����, ��������� ������ �������� ��� ������������� ��������� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NStatePredictor::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NStatePredictor::SetNeuronClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ���������
bool NStatePredictor::SetPredictorClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ��������� ������
bool NStatePredictor::SetNeuronLearnerClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ���� ������� ��������
bool NStatePredictor::SetLogicalNotClassName(const std::string &value)
{
 Ready=false;
 return true;
}

/// ��� ������, ���������� ������
bool NStatePredictor::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}


/// ����� �������
/// 0 - ������ ���� ������� ������� ��������
/// 1 - ��� ����������� �������� �����-���������
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

/// ������� ������������� ��������
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

// ������� ��� �����������
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


/// ������� ������� �������� (������������ � ������ ������)
/// ������ �������� N �������� �������� �������� ������� � ������� ������� t, t-tau, ... t-(N-1)tau,
/// ��� tau - �������� ����� ����������� ��������� �����, N - ����� ���������
/// ������� �������� M ��������, ������������ ������� ������. M - ����������� �������
/// �����! ��� �������� �������� ������ ������������ ������� [0; 0,2],
/// ��� ����� ��� ���������� ������ ���������� �������
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

///����� ������� �������� = ����� �������� ������� ������� ��������
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

///����� ���������, ����������� ��������� = ����� ����� ������� ������� ��������
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

///�������� tau ����� ����������� ��������� �����
/// (� ������ ������ � ���������� �������)
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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NStatePredictor::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NStatePredictor::ADelComponent(UEPtr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool NStatePredictor::BuildStructure()
{
    if(StructureBuildMode == 1)
    {
        //������� ���������
        Predictor = AddMissingComponent<NPredictor>(std::string("Predictor"), PredictorClassName);
        Predictor->SetCoord(MVector<double,3>(5, 2, 0));
        Predictor->InputCols = StatesNumber;
        Predictor->InputRows = FeaturesNumber;
        Predictor->InputMatrix.Resize(FeaturesNumber, StatesNumber);
        Predictor->MeasurementPeriod = MeasurementPeriod;
        Predictor->InputMode = InputMode;
        Predictor->Reset();

        //������� ���� ������� ��������
        LogicalNot = AddMissingComponent<NLogicalNot>(std::string("LogicalNot"), LogicalNotClassName);
        LogicalNot->SetCoord(MVector<double,3>(5, 5, 0));
        LogicalNot->InputPattern.Resize(FeaturesNumber, 1);
        LogicalNot->NeuronTrainer->NumInputDendrite = FeaturesNumber;
        LogicalNot->Reset();

//      //������� �������� ������
//      //OutputNeuron = AddMissingComponent<NNeuronLearner>(std::string("OutputNeuron"), NeuronLearnerClassName);
//      OutputNeuron = AddMissingComponent<NNeuronTrainer>(std::string("OutputNeuron"), NeuronLearnerClassName);
//      OutputNeuron->SetCoord(MVector<double,3>(22, 9.5, 0));
//      OutputNeuron->NumInputDendrite = FeaturesNumber;
//      OutputNeuron->InputPattern.Resize(FeaturesNumber, 1);
//      OutputNeuron->Reset();




     }
     return true;
}


// ����� �������� �����.
bool NStatePredictor::AReset(void)
{
     InputMatrix.Resize(FeaturesNumber, StatesNumber);
     //OutputNeuron->InputPattern.Resize(FeaturesNumber, 1);
     LogicalNot->InputPattern.Resize(FeaturesNumber, 1);

     return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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
    FeaturesNumber = 5; // = ����� ��������� ������� ��������� (InputRows � Predictor)
    StatesNumber = 4; // = ����� ��������� ������� �������� ������� ��������� (InputCols � Predictor)
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


// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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

// ��������� ������ ����� �������
bool NStatePredictor::ACalculate(void)
{
    IsNeedToTrain = LogicalNot->IsNeedToTrain;
    InputPattern =  LogicalNot->InputPattern;

   //�������� �������� � ������ ���������� �� ���� LogicalNot
   if (!IsNeedToTrain)
   {
       LogicalNot->InputPattern = Predictor->Output;
   }

   //�������� ������� � ������ LogicalNot � ������ �����
    Output = LogicalNot->OutputNeuron->LTZone->Output;

    return true;
}
// --------------------------


}
#endif
