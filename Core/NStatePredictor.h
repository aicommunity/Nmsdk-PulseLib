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

// �����, ��������� ������ �������� ��� ������������� ��������� ��������
class RDK_LIB_TYPE NStatePredictor: public UNet
{
public: // ���������
/// ����� ������ ��������� �������
/// 0 - �������������� ������ �� ������������
/// 1 - ������ ������ �������� ��� ������������� ��������� ��������.
/// ������� ������������ � �������� 2 ������� ���������� +
/// ������������� ����, ����������� ������������� ��������� ��������.
/// ������ �����: ����������� ������������ -> ����������� �������
/// ������ �����: �������� ������������
/// �� ������ ����� ��������� ������� ������� 2-� �������, ������ ������ �������� ���������.
/// �� ������ ����� ��������� ������� "�" �������������� ����, �������
/// ������ ����������� ��������� �������� ��������� ��� ��������� ���� ������ ������������.
/// �� ������� ����� ���������� ����������� �������� �� ������� ������� �� ��� ���,
/// ���� ������� ������ �� �������� ������������ �������. ��������� ������� �������
/// �������� �������� ������������ ����� ������ ������� 2-��� ������ �� �������
/// "���" 1-��� ������.
ULProperty<int,NStatePredictor, ptPubParameter> StructureBuildMode;

///// ��� ������ �������
ULProperty<std::string, NStatePredictor, ptPubParameter> NeuronClassName;

///// ��� ������ �������
ULProperty<std::string, NStatePredictor, ptPubParameter> SynapseClassName;

/// ��� ������ ����������
ULProperty<std::string, NStatePredictor, ptPubParameter> PredictorClassName;

/// ��� ������ ���������� �������
ULProperty<std::string, NStatePredictor, ptPubParameter> NeuronLearnerClassName;

/// ��� ������ ����� ������� ��������
ULProperty<std::string, NStatePredictor, ptPubParameter> LogicalNotClassName;

/// ����� ����� ������� ��������
/// 0 - ���� ������� ������� �������� ������������ �������
/// 1 - ������������� �������� ��������� �������
ULProperty<int, NStatePredictor, ptPubParameter> InputMode;

/// ������� ������������� ��������
ULProperty<bool, NStatePredictor, ptPubParameter> IsNeedToTrain;

/// ������� ��� �����������. �������� ������������ �������� ������� �� �������
/// �������� (������ �������), ������������ �������, ������� ������ ������ � ���.
ULProperty<MDMatrix<double>, NStatePredictor, ptPubParameter> InputPattern;

///������� ������� �������� (������������ � ������ ������)
///������ �������� N �������� �������� �������� ������� � ������� ������� t, t-tau, ... t-(N-1)tau,
///��� tau - �������� ����� ����������� ��������� �����, N - ����� ���������
///������� �������� M ��������, ������������ ������� ������. M - ����������� �������
///�����! ��� �������� �������� ������ ������������ ������� [0; 0,2],
/// ��� ����� ��� ���������� ������ ���������� �������
ULProperty<MDMatrix<double>, NStatePredictor, ptPubParameter> InputMatrix;

////����� ������� �������� =
/// = ���������� �������� ������� ������� ��������
ULProperty<int, NStatePredictor, ptPubParameter> StatesNumber;

////����� ���������, ����������� ��������� =
/// = ���������� ����� ������� ������� ��������
ULProperty<int, NStatePredictor, ptPubParameter> FeaturesNumber;

///�������� tau ����� ����������� ��������� ����� (� ������ ������ � ������� ���������� �������)
ULProperty<double, NStatePredictor, ptPubParameter> MeasurementPeriod;

/// ������� ������ (������������ ������� ������� ��������, ������������ � ������ ������ � ������� ���������� �������)
UPropertyInputData<MDMatrix<double>,NStatePredictor, ptInput | ptPubState> Input;

/// ��������� ����������, ������������� ��������
UPropertyOutputData<MDMatrix<double>, NStatePredictor, ptOutput | ptPubState> Output;


protected:

//������� �������
//std::vector<UEPtr<NNeuronTrainer>> InputNeurons;

//���� ����������
UEPtr<NPredictor> Predictor;

//��������� �������� ������
//UEPtr<NNeuronLearner> OutputNeuron;
//UEPtr<NNeuronTrainer> OutputNeuron;

//���� ������� ��������
UEPtr<NLogicalNot> LogicalNot;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NStatePredictor(void);
virtual ~NStatePredictor(void);
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool SetStructureBuildMode(const int &value);

/// ��� ������, ���������� ������
bool SetSynapseClassName(const std::string &value);

/// ��� ������, ���������� ������
bool SetNeuronClassName(const std::string &value);

/// ��� ������, ���������� ���������
bool SetPredictorClassName(const std::string &value);

/// ��� ������, ���������� ��������� ������
bool SetNeuronLearnerClassName(const std::string &value);

/// ��� ������, ���������� ���� ������� ��������
bool SetLogicalNotClassName(const std::string &value);

/// ����� ������� �������
bool SetInputMode(const int &value);

/// ������� ������������� ��������
bool SetNeedToTrain(const bool &value);

/// ������� ��� �����������
bool SetInputPattern(const MDMatrix<double> &value);

/// ������� ������� ��������
bool SetInputMatrix(const MDMatrix<double> &value);

////����� ������� �������� =
/// = ���������� �������� ������� ������� ��������
bool SetStatesNumber(const int &value);

///����� ���������, ����������� ��������� =
/// = ���������� ����� ������� ������� ��������
bool SetFeaturesNumber(const int &value);

///�������� tau ����� ����������� ��������� �����
/// (� ������ ������ � ���������� �������)
bool SetMeasurementPeriod(const double &value);



// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------

// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NStatePredictor* New(void);
static UComponent* NewStatic(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
/// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool BuildStructure();

// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
