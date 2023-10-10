// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NLIFE_NET_H
#define NLIFE_NET_H

#include "NNet.h"
#include "NNeuronLife.h"
#include "NPulseLifeNeuron.h"
#include "NPulseHebbSynapse.h"

namespace NMSDK {

// ��� �������� ��������
typedef vector<NPulseLifeNeuron*> NEnsemble;

// ��� ����� ����� ����������
struct RDK_LIB_TYPE NEnsembleLink
{
// ��������-�������� �������
NameT In;

// ��������-�������� �������
NameT Out;

// �������� �����
double Power;

// ------------------------
// ���������
// ------------------------
friend bool operator < (const NEnsembleLink &link1, const NEnsembleLink &link2);
// ------------------------
};

class RDK_LIB_TYPE NLifeNet: public NNet
{
public: // ������������� ��������
/// ��������� ������� ����
ULProperty<double,NLifeNet, ptPubParameter> AvailableEnergy;

/// ���� ����������� �������� �������
ULProperty<bool,NLifeNet, ptPubParameter> CellDivisionFlag;

/// ����� �������, �� ���������� �������� ������ �������
ULProperty<double,NLifeNet, ptPubParameter> CellDivisionEnergy;

/// ����� �������� ������� �� ����������� �������� � ��������
ULProperty<double,NLifeNet, ptPubParameter> EnsembleThreshold;

/// ����� �������� ������� � ������� ����� ����� ����������
ULProperty<double,NLifeNet, ptPubParameter> EnsembleLinkThreshold;

/// ����� ������� ������ �������� �������
/// 0 - �� ����
/// 1 - �� �������� Gs �������� ������� �������
/// 2 - �� �������� Gs �������� ���� �������� ����
ULProperty<int,NLifeNet, ptPubParameter> EnsembleThresholdMode;

/// �������� ������� ��������� ��������� (� ��������)
/// ���� 0, �� ������ ��������� �� ������������
ULProperty<double,NLifeNet, ptPubParameter> EnsembleCalculationStep;

/// ����� ������� ������� ����� ���������
/// 0 - ���� �������
/// 1 - ��������� � ������������ ��������
ULProperty<int,NLifeNet, ptPubParameter> EnergyBalanceMode;

public: // ����� � ������
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output4;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output6;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output7;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output8;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output9;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output10;

public: // ���������� ���������
/// ��������� ������������ �������� ����
ULProperty<double,NLifeNet,ptPubState> SummaryFeeling;

/// ��������� ������ ������� ��������� ����
ULProperty<double,NLifeNet,ptPubState> SummaryEnergy;

/// ��������� ����� �������� ����
ULProperty<double,NLifeNet,ptPubState> SummaryWearOut;

/// ��������� ������� ���������� �������� ����
ULProperty<double,NLifeNet,ptPubState> SummaryFrequency;

/// ��������� ����� ����� �������� ����
ULProperty<double,NLifeNet,ptPubState> SummaryLiveNeurons;

protected: // ������
/// ������ ������ ���������������� ��������� ��������
vector<NNeuronLife*> NeuronsLife;

/// ������ ��� <�������� ��������,����-��������> ��������
vector<pair<double, NEnsemble > > BetaEnsembles;

/// ������ ��� <�������� ��������,��������> ��������
vector<pair<double, NEnsemble > > Ensembles;

/// ������ ������ ����� ����������
/// <�������� �����, �����>
vector<NEnsembleLink> EnsembleLinks;

protected: // ��������� ����������
/// ������ �������� �������� ����
map<NPulseLifeNeuron*,vector<NPulseHebbSynapse*> > SynapticMap;
typedef map<NPulseLifeNeuron*,vector<NPulseHebbSynapse*> >::iterator SynapticMapIterator;

int EnsembleCalculationCounter;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NLifeNet(void);
virtual ~NLifeNet(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ����� ������ ���������������� ��������
size_t GetNumNeuronsLife(void) const;

// ���������� �������� ������� ���������������� �������
NNeuronLife* GetNeuronLife(size_t i);

// ���������� ����� ����-��������� ��������
size_t GetNumBetaEnsembles(void) const;

// ���������� �������� ����-��������
const NEnsemble& GetBetaEnsemble(size_t i);

// ���������� �������� ��������� ����-��������
double GetBetaEnsemblePower(size_t i) const;

// ���������� ����� ��������� ��������
size_t GetNumEnsembles(void) const;

// ���������� �������� ��������
const NEnsemble& GetEnsemble(size_t i);

// ���������� �������� ��������� ��������
double GetEnsemblePower(size_t i) const;

// ���������� ��� �������� �� ����� ������������ ��������
NameT GetEnsembleName(size_t i) const;

// ���������� ���������� �������� ���������� ��������
// (������� ������� ���� �������� ��������)
double GetEnsembleActivity(size_t i) const;

// ���������� �������� ���������� ��������
// ������������ ���������� ����
double GetRelativeEnsembleActivity(size_t i) const;

// ���������� ����� ������ ����� ����������
size_t GetNumEnsembleLinks(void) const;

// ���������� ����� ����� ����������
const NEnsembleLink& GetEnsembleLink(size_t i) const;
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NLifeNet* New(void);
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

// ��������� �������� ����
bool CalcEnsembles(double threshold);

// ��������� ����� ����� ���������� ����
bool CalcEnsembleLinks(double threshold);
// --------------------------
};

}
#endif


