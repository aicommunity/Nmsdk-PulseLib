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

#ifndef NNEURONFREQGROUP_H
#define NNEURONFREQGROUP_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// �����, ��������� ������ ����������� ��������, �������������� � ������ ���������� ������
class RDK_LIB_TYPE NNeuronFreqGroup: public UNet
{
public: // ���������
/// ����� ������ ��������� �������
/// (� ������� >1) �������������� ������������� ����� ������� �������� �������� � �������������� ����
/// 0 - �������������� ������ �� ������������
/// 1 - �������������� ������ ������������ � ����������� ���������� ��������� ������� ������
ULProperty<int,NNeuronFreqGroup, ptPubParameter> StructureBuildMode;

/// ��� ������ ������������ �������
ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> AffNeuronClassName;

/// ����� ����������� ��������
ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumAffNeurons;

/// �������� ������� ������
/// ����������� ������� �������
ULProperty<int, NNeuronFreqGroup, ptPubParameter> MinInputFreq;
/// ������������ ������� �������
ULProperty<int, NNeuronFreqGroup, ptPubParameter> MaxInputFreq;

/// ��� ������ ����������� ������� �������� ��� ������������ ����
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> LTMembraneClassName;

/// ��� ������ ������������ ����
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> LTZoneClassName;

/// ��� ������ ��������� ��������� ������� ��� ������������� ������� ���������
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> ExcGeneratorClassName;

/// ��� ������ ��������� ��������� ������� ��� ���������� ������� ���������
//ULProperty<std::string, NNeuronFreqGroup, ptPubParameter> InhGeneratorClassName;

/// ����� �������� �������� ���� �������
//ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumSomaMembraneParts;

/// ����� �������� �������� ��������� (�������� ������� ����)
//ULProperty<int, NNeuronFreqGroup, ptPubParameter> NumDendriteMembraneParts;

protected:
//NConstGenerator *PosGenerator,*NegGenerator;

// ������ �������� ���������� �������� ���� � ���������
int OldNumAffNeurons;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuronFreqGroup(void);
virtual ~NNeuronFreqGroup(void);
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------
/// ����� ������ ��������� �������
bool SetStructureBuildMode(const int &value);

/// ��� ������ ������� ��������
bool SetAffNeuronClassName(const std::string &value);

/// ����� ����������� ��������
bool SetNumAffNeurons(const int &value);

/// ����������� ������� �������
bool SetMinInputFreq(const int &value);

/// ������������ ������� �������
bool SetMaxInputFreq(const int &value);

/// ��� ������ ����������� ������� �������� ��� ������������ ����
//bool SetLTMembraneClassName(const std::string &value);

/// ��� ������ ������������ ����
//bool SetLTZoneClassName(const std::string &value);

/// ��� ������ ��������� ��������� ������� ��� ������������� ������� ���������
//bool SetExcGeneratorClassName(const std::string &value);

/// ��� ������ ��������� ��������� ������� ��� ���������� ������� ���������
//bool SetInhGeneratorClassName(const std::string &value);

/// ����� �������� �������� ���� �������
//bool SetNumSomaMembraneParts(const int &value);

/// ����� �������� �������� ��������� (�������� ������� ����)
//bool SetNumDendriteMembraneParts(const int &value);
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ ��������� ������������� ����������
//NConstGenerator* GetPosGenerator(void);

// ���������� ��������� �� ������ ��������� ���������� ����������
//NConstGenerator* GetNegGenerator(void);

/// ������ � �������� ��������
//size_t GetNumMembranes(void) const;
//NPulseMembrane* GetMembrane(size_t i);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NNeuronFreqGroup* New(void);
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
bool BuildStructure(int structure_build_mode, const string &aff_neuron_class_name,
					int old_num_aff_neurons, int num_aff_neurons, int min_inp_freq, int max_inp_freq);

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
