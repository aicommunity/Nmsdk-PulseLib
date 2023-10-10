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

#ifndef NCLASSIFIER_H
#define NCLASSIFIER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// �����, ��������� �������������
class RDK_LIB_TYPE NClassifier: public UNet
{
public: // ���������
/// ����� ������ ��������� �������
/// 0 - �������������� ������ �� ������������
/// 1 - ������ ������ ��������, ���������� �������������� ��� ������������� ��������� ��������
/// ������� ����������� ������ � ������ �� �������� "����� n-��� ������� -> ��������� ���� m-��� �������" n != m
/// �� ���� ��������� ������ ������� �������. ������ ������������ ��� �������������.
/// ��������� ������������� �������� ���������� ������ (��� ����������) �� ��������� ��������
/// ����������� ������������� �������� �������(-�), �� �������� �������� �������� ������
ULProperty<int,NClassifier, ptPubParameter> StructureBuildMode;

/// ����� ������� �������� �������
/// 0 - ������ �� ������������
/// 1 - �������� ���� ����������� ����� ��������� ���� ������������ ��������� �������
/// 2 (�� ��������!!!) - �������� ���� ����������� ����� ��������� �� ��������� � ���������� �������� �� ��������
/// 3 - �������� ���� ����������� ����� ��������� ���� ������������� ������������ ������� ������� �������� �� ����
/// 4 - �������� ���� ����������� ����� ��������� �� ������� � ���������� �������� �� ��������
/// 5 - �������������� ������ ������ ������������ ������� ��� ������� ������������ ���������������� ��������

ULProperty<int,NClassifier, ptPubParameter> CalculateMode;

/// ��� ������, ���������� ��������� ���������
ULProperty<std::string, NClassifier, ptPubParameter> PulseGeneratorClassName;

/// ��� ������, ���������� ������� �������
ULProperty<std::string, NClassifier, ptPubParameter> NeuronTrainerClassName;

/// ��� ������ �������
ULProperty<std::string, NClassifier, ptPubParameter> NeuronClassName;

/// ��� ������ �������
ULProperty<std::string, NClassifier, ptPubParameter> SynapseClassName;

/// ������� ������������� ��������
ULProperty<bool, NClassifier, ptPubParameter> IsNeedToTrain;

/// ����� �������� ������ �������� ������������ ������ ������� (���)
ULProperty<double, NClassifier, ptPubParameter> Delay;

/// ������� ����������� (��)
ULProperty<double, NClassifier, ptPubParameter> SpikesFrequency;

/// ����� ������� ��������� ��� ������� �������
ULProperty<int, NClassifier, ptPubParameter> NumInputDendrite;

/// ������������ ����� �������� ��� ������� �������
ULProperty<int, NClassifier, ptPubParameter> MaxDendriteLength;

/// �������� ��� ����������� ���������. �������� ������������ �������� ������� �� �������
/// �������� (������ �������), ������������ �������, ������� ������ ������ � ���.
/// �������� ��� ������ ������� ������������� � ����� ������!!!
ULProperty<MDMatrix<double>, NClassifier, ptPubParameter> TrainingPatterns;

/// ������� ��� �������������. �������� ������������ �������� ������� �� �������
/// �������� (������ �������), ������������ �������, ������� ������ ������ � ���.
/// �������� �������� ��� ������������� ������������� � ����� �������!!!
ULProperty<MDMatrix<double>, NClassifier, ptPubParameter> InputPattern;

/// ����� �������������� ���� �������
ULProperty<double, NClassifier, ptPubParameter> LTZThreshold;

/// ������������� ����� �������������� ���� �������
ULProperty<double, NClassifier, ptPubParameter> FixedLTZThreshold;

/// ����� �������������� ���� ������� ��� ��������
ULProperty<double, NClassifier, ptPubParameter> TrainingLTZThreshold;

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
ULProperty<bool, NClassifier, ptPubParameter> UseFixedLTZThreshold;

/// ������� ������������� �������� ������� �� �������� ���������
ULProperty<bool,NClassifier, ptPubParameter> UseTransitSignal;

/// ����� ���������������� �������
ULProperty<int,NClassifier, ptPubParameter> NumClasses;

/// ����� �������� ������� ������ ��������� �������
ULProperty<int,NClassifier, ptPubParameter> SizeTrainingSet;

/// ������� ������������� ������ � ������ ������ �� �����
ULProperty<bool, NClassifier, ptPubParameter> DataFromFile;

protected:

 // ������ �������� ����� �������
 int OldNumClasses;
 // ������ �������� ����� �������
 int OldSizeTrainingSet;
 // ������ �������� ����� ������� ���������
 int OldNumInputDendrite;

 // ���������� ��� ������ � ������ ������ � ����
 // ------->
 // ������� ������ ��������
 bool IsFirstFileStep;
 // ���� � �������� �������
 ifstream fin;
 // ���� � ��������� �������
 ofstream fout;
 // ������� ������� ���������
 MDMatrix<double> inputs;
 // ������ ��������
 std::vector<int> outputs;
 // ���� ������ ��������
 bool is_first_iter;
 // ����� ������ ������� �������� (���)
 double start_iter_time;
 // <-------

 UEPtr<NPulseNeuron> LogicalOrNeuron;

 // ���������� ���������
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;
 // ��������� (�����������) �������
 //std::vector<UEPtr<NNeuronTrainer> > trainers;

 // ������ ��������� ��������
 std::vector<std::vector<UEPtr<NNeuronTrainer> > >groups_trainers;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NClassifier(void);
virtual ~NClassifier(void);
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool SetStructureBuildMode(const int &value);

/// ����� ������� �������
bool SetCalculateMode(const int &value);

/// ��� ������, ���������� ��������� ���������
bool SetPulseGeneratorClassName(const std::string &value);

/// ��� ������, ���������� ������� �������
bool SetNeuronTrainerClassName(const std::string &value);

/// ��� ������, ���������� ������
bool SetNeuronClassName(const std::string &value);

/// ��� ������, ���������� ������
bool SetSynapseClassName(const std::string &value);

/// ����� ��������
//bool SetNumNeurons(const int &value);

/// ������� ������������� ��������
bool SetNeedToTrain(const bool &value);

/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool SetDelay(const double &value);

/// ������� ����������� (��)
bool SetSpikesFrequency(const double &value);

/// ����� ������� ���������
bool SetNumInputDendrite(const int &value);

/// ������������ ����� ��������
bool SetMaxDendriteLength(const int &value);

/// �������� ��� �����������
bool SetTrainingPatterns(const MDMatrix<double> &value);

/// ������� ��� �������������.
bool SetInputPattern(const MDMatrix<double> &value);

/// ����� �������������� ���� �������
bool SetLTZThreshold(const double &value);

/// ������������� ����� �������������� ���� �������
bool SetFixedLTZThreshold(const double &value);

/// ����� �������������� ���� ������� ��� ��������
bool SetTrainingLTZThreshold(const double &value);

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
bool SetUseFixedLTZThreshold(const bool &value);

// ������������� ������������� �������� ������� �� �������� ���������
bool SetUseTransitSignal(const bool &value);

/// ����� ���������������� �������
bool SetNumClasses(const int &value);

/// ����� �������� ������� ������ ��������� �������
bool SetSizeTrainingSet(const int &value);

// ������������� ������������� ������ � ������ ������ �� �����
bool SetDataFromFile(const bool &value);

// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------

// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NClassifier* New(void);
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
// ������� ��� ������ � �������.
// ������������ ������ ������� ������ �� �����,
// ��������� ����������� � ������ ����������� � ����
bool TreatDataFromFile(void);

/// ������������ ������ ��������� � ������������ � ���������� ������� �����������
bool BuildStructure(void);

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
