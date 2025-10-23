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


// �����, ��������� ������ �������� ��� ������������� ��������� ��������
namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
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
// ������ ������� � ��������� ����������
// --------------------------
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool NPredictor::SetStructureBuildMode(const int &value)
{
 if(value >0) // ���������� ��������� ����� ������ ���� StructureBuildMode �� 0
  Ready=false;
 return true;
}

/// ����� �������
/// 0 - ������ ���� ������� ������� ��������
/// 1 - ��� ����������� �������� �����-���������
bool NPredictor::SetInputMode(const int &value)
{
 return true;
}


/// ������� ������� ��������
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

///�������� tau ����� ����������� ��������� �����
/// (� ������ ������ � ���������� �������)
bool NPredictor::SetMeasurementPeriod(const double &value)
{
 Ready=false;
 return true;
}


// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
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
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPredictor::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPredictor::ADelComponent(std::shared_ptr<UContainer> comp)
{

 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ������������ ������ ��������� � ������������ � ���������� ������� �����������


// ����� �������� �����.
bool NPredictor::AReset(void)
{
    InputMatrix.Resize(InputRows, InputCols);

    return true;
}

// �������������� �������� �� ��������� � ����� �������� �����
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


// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
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


// ��������� ������ ����� �������
bool NPredictor::ACalculate(void)
{
    input_pattern.Resize(1, InputCols);

    //��� ������������� ������� ������� ��������
    if(InputMode == 0)
    {
        Output.Resize(InputRows, 1);

        for (int i = 0; i < InputRows; i++)
        {
            input_pattern = InputMatrix->GetRow(i);
            Output(i,0) = Predict(input_pattern);
        }
    }

    //��� ������������� �������� ���������
    else if(InputMode == 1)
    {
        Output.Resize(1, 1);
        double time_from_start = Environment.lock()->GetTime().GetDoubleTime() - measurement_period_start;

         //����������� ���������� �� ������ �������� ��������
        if (time_from_start <= 0)
        {
            measurement_period_start = Environment.lock()->GetTime().GetDoubleTime();
            between_measurements = true;
        }

        //�� ��������� ������� tau ����� �����������
        else if(time_from_start >= MeasurementPeriod)
        {
            //������ ������ �������
            if (between_measurements == false)
            {
                measurement_period_start = Environment.lock()->GetTime().GetDoubleTime();
                between_measurements = true;
            }
            //������ �������� � input_pattern
            else
            {
                between_measurements = false;

                input_pattern[k] = Input()(0,0);
                k--;

                //������� ����������� ����� �������� � input_pattern - ���������� ������
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
