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

#ifndef NSPIKECLASSIFIER_H
#define NSPIKECLASSIFIER_H

#include "NPulseGenerator.h"
#include "NPulseNeuron.h"
#include "NNeuronTrainer.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// �����, ��������� ������ ��������� �������� ��� ������������� ��������� �������� ���������
class RDK_LIB_TYPE NSpikeClassifier: public UNet
{
public: // ���������
/// ����� ������ ��������� �������
/// 0 - �������������� ������ �� ������������
/// 1 - ������ ������ ��������, ���������� �������������� ��� ������������� ��������� ��������
/// ������� ����������� ������ � ������ �� �������� "����� n-��� ������� -> ��������� ���� m-��� �������" n != m
/// �� ���� ��������� ������ ������� �������. ������ ������������ ��� �������������.
/// ��������� ������������� �������� ���������� ������ (��� ����������) �� ��������� ��������
/// ����������� ������������� �������� �������(-�), �� �������� �������� �������� ������
ULProperty<int,NSpikeClassifier, ptPubParameter> StructureBuildMode;

/// ��� ������, ���������� ��������� ���������
ULProperty<std::string, NSpikeClassifier, ptPubParameter> PulseGeneratorClassName;

/// ��� ������, ���������� ������� �������
ULProperty<std::string, NSpikeClassifier, ptPubParameter> NeuronTrainerClassName;

/// ��� ������ �������
ULProperty<std::string, NSpikeClassifier, ptPubParameter> NeuronClassName;

/// ��� ������ �������
ULProperty<std::string, NSpikeClassifier, ptPubParameter> SynapseClassName;

/// ����� ��������
ULProperty<int, NSpikeClassifier, ptPubParameter> NumNeurons;

/// ������� ������������� ��������
ULProperty<bool, NSpikeClassifier, ptPubParameter> IsNeedToTrain;

/// ����� �������� ������ �������� ������������ ������ ������� (���)
ULProperty<double, NSpikeClassifier, ptPubParameter> Delay;

/// ������� ����������� (��)
ULProperty<double, NSpikeClassifier, ptPubParameter> SpikesFrequency;

/// ����� ������� ��������� ��� ������� �������
ULProperty<int, NSpikeClassifier, ptPubParameter> NumInputDendrite;

/// ������������ ����� �������� ��� ������� �������
ULProperty<int, NSpikeClassifier, ptPubParameter> MaxDendriteLength;

/// �������� ��� ����������� ���������. �������� ������������ �������� ������� �� �������
/// �������� (������ �������), ������������ �������, ������� ������ ������ � ���.
/// �������� ��� ������ ������� ������������� � ����� ������!!!
ULProperty<MDMatrix<double>, NSpikeClassifier, ptPubParameter> TrainingPatterns;

/// ������� ��� �������������. �������� ������������ �������� ������� �� �������
/// �������� (������ �������), ������������ �������, ������� ������ ������ � ���.
/// �������� �������� ��� ������������� ������������� � ����� �������!!!
ULProperty<MDMatrix<double>, NSpikeClassifier, ptPubParameter> InputPattern;

/// ����� �������������� ���� �������
ULProperty<double, NSpikeClassifier, ptPubParameter> LTZThreshold;

/// ������������� ����� �������������� ���� �������
ULProperty<double, NSpikeClassifier, ptPubParameter> FixedLTZThreshold;

/// ����� �������������� ���� ������� ��� ��������
ULProperty<double, NSpikeClassifier, ptPubParameter> TrainingLTZThreshold;

/// ������� ������������� ������������� �������������� ������ �������������� ���� �������
ULProperty<bool, NSpikeClassifier, ptPubParameter> UseFixedLTZThreshold;

/// ������� ������������� �������� ������� �� �������� ���������
ULProperty<bool,NSpikeClassifier, ptPubParameter> UseTransitSignal;

/// ������� ������������� ������ � ������ ������ �� �����
ULProperty<bool, NSpikeClassifier, ptPubParameter> DataFromFile;


protected:

 // ������ �������� ����� ��������
 int OldNumNeurons;
 // ������ �������� ����� ������� ���������
 int OldNumInputDendrite;

 // ���������� ���������
 std::vector<UEPtr<NPulseGeneratorTransit> > generators;
 // ��������� (�����������) �������
 std::vector<UEPtr<NNeuronTrainer> > trainers;

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


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSpikeClassifier(void);
virtual ~NSpikeClassifier(void);
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool SetStructureBuildMode(const int &value);

/// ��� ������, ���������� ��������� ���������
bool SetPulseGeneratorClassName(const std::string &value);

/// ��� ������, ���������� ������� �������
bool SetNeuronTrainerClassName(const std::string &value);

/// ��� ������, ���������� ������
bool SetNeuronClassName(const std::string &value);

/// ��� ������, ���������� ������
bool SetSynapseClassName(const std::string &value);

/// ����� ��������
bool SetNumNeurons(const int &value);

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
virtual NSpikeClassifier* New(void);
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
bool BuildStructure(void);

// ������� ��� ������ � �������.
// ������������ ������ ������� ������ �� �����,
// ��������� ����������� � ������ ����������� � ����
bool TreatDataFromFile(void);

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

// ������������� ���������� � ��������� ������� �������
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif
