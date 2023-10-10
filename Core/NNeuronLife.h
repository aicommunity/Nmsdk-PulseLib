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

#ifndef NNEURON_LIFE_H
#define NNEURON_LIFE_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "NSum.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NNeuronLife: public UNet
{
public: // ������������� ��������
/// ����������� ����� �������
ULProperty<double,NNeuronLife,ptPubParameter> Threshold;

/// ����������� �������� �������, ������������ ��� ������� �-�� ���������
/// ��������� ��������
ULProperty<double,NNeuronLife,ptPubParameter> CriticalEnergy;

//���������� ��� ������� ������

/// ��������� �������� ������������� ��������� ������ �������
ULProperty<double,NNeuronLife,ptPubParameter> MaxPotentialGradient;

ULProperty<double,NNeuronLife,ptPubParameter> WearOutConstPositive;

ULProperty<double,NNeuronLife,ptPubParameter> WearOutConstNegative;

ULProperty<double,NNeuronLife,ptPubParameter> Kw;

/// ���������� ��� ������� ������������
ULProperty<double,NNeuronLife,ptPubParameter> Emax;

ULProperty<double,NNeuronLife,ptPubParameter> En;

ULProperty<double,NNeuronLife,ptPubParameter> Kq;

/// ���������� ��� ������� ������ �������
ULProperty<double,NNeuronLife,ptPubParameter> Ee0;
ULProperty<double,NNeuronLife,ptPubParameter> Es;
ULProperty<double,NNeuronLife,ptPubParameter> Eh0;
ULProperty<double,NNeuronLife,ptPubParameter> Econst;
ULProperty<double,NNeuronLife,ptPubParameter> Ecr;
ULProperty<double,NNeuronLife> WearOutcr;
ULProperty<double,NNeuronLife,ptPubParameter> EnergyWearOutCritical;
//RDK::ULProperty<double,NNeuronLife> E;    //?
//RDK::ULProperty<double,NNeuronLife> N;   //?
ULProperty<double,NNeuronLife,ptPubParameter> EyConst;
ULProperty<double,NNeuronLife,ptPubParameter> EyBonusPos;
ULProperty<double,NNeuronLife,ptPubParameter> EyBonusNeg;

/// ����������� ������ ����������� �������
ULProperty<double,NNeuronLife,ptPubParameter> EnergyComprehensibility;

/// ���������� ��� ������� ������
ULProperty<double,NNeuronLife,ptPubParameter> Kdp;
ULProperty<double,NNeuronLife,ptPubParameter> Pdmax;
ULProperty<double,NNeuronLife,ptPubParameter> Qd;
ULProperty<double,NNeuronLife,ptPubParameter> Khp0;
ULProperty<double,NNeuronLife,ptPubParameter> Khp1;

ULProperty<double,NNeuronLife,ptPubParameter> Qsummax;

// ��������� ����� � ������ �������
ULProperty<double,NNeuronLife,ptPubParameter> EnergyBonus;

public: // ������ � ������� �����������
// ����� �������
ULProperty<double,NNeuronLife,ptPubState> Energy;

public: // ����� � ������
///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input1;

///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input2;

///
UPropertyInputData<MDMatrix<double>,NNeuronLife, ptInput | ptPubState> Input3;

///
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> OutputThreshold;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output6;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output7;
UPropertyOutputData<MDMatrix<double>,NNeuronLife, ptOutput | ptPubState> Output8;

protected: // ��������� ����������
/// ����� �������
ULProperty<double,NNeuronLife,ptPubState> WearOut;

/// ����� �������
ULProperty<double,NNeuronLife,ptPubState> ThresholdLife;

ULProperty<double,NNeuronLife,ptPubState> ThresholdWearOut;

ULProperty<double,NNeuronLife,ptPubState> ThresholdFeel;

/// ������������
ULProperty<double,NNeuronLife,ptPubState> Feel;

ULProperty<double,NNeuronLife,ptPubState> Qsum;

ULProperty<double,NNeuronLife,ptPubState> Esum;

//RDK::ULProperty<double,NNeuronLife,ptPubState> UsumOld;

//RDK::ULProperty<double,NNeuronLife,ptPubState> UsumProizv;

ULProperty<double,NNeuronLife,ptPubState> EsumOld;

ULProperty<double,NNeuronLife,ptPubState> EsumProizv;

/// �������� ������� �� ���
ULProperty<double,NNeuronLife,ptPubState> Ea;

/// ������ ������� �� ��� �� ��������� ��������
ULProperty<double,NNeuronLife,ptPubState> Ey;

/// ������ ������� �� ��� �� ����������� ������ �������
ULProperty<double,NNeuronLife,ptPubState> Ee;

/// ������ ������� �� ��� �� ��������������� ������
ULProperty<double,NNeuronLife,ptPubState> Eh;

/// ����������� - �������� ������� �� ���
ULProperty<double,NNeuronLife,ptPubState> dEa;

/// ����������� - ������ ������� �� ��� �� ��������� ��������
ULProperty<double,NNeuronLife,ptPubState> dEy;

/// ����������� - ������ ������� �� ��� �� ����������� ������ �������
ULProperty<double,NNeuronLife,ptPubState> dEe;

/// ����������� - ������ ������� �� ��� �� ��������������� ������
ULProperty<double,NNeuronLife,ptPubState> dEh;

/// ����������� �������� ������ �������
ULProperty<double,NNeuronLife,ptPubState> dE;

ULProperty<double,NNeuronLife,ptPubState> Usum;

// ��������� ���������
//RDK::ULProperty<double,NNeuronLife,ptPubState> NeuralPotential;

// ������������� �������� ����������
//RDK::ULProperty<double,NNeuronLife,ptPubState> PrePotential;

// ���� ������� ���������
//RDK::ULProperty<int,NNeuronLife,ptPubState> PulseCounter;
public: // ��������� �� ���������� ����������
// ��������� ����� � ������ �������
UEPointer<NSum, NNeuronLife> ExternalEnergyBonus;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuronLife(void);
virtual ~NNeuronLife(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetThreshold(const double &value);

// ������������� ��������� �������� ������������� ��������� ������ �������
bool SetMaxPotentialGradient(const double &value);

bool SetEmax(const double &value);

bool SetEn(const double &value);

bool SetPdmax(const double &value);

bool SetQsummax(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NNeuronLife* New(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
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
// --------------------------

virtual bool ACalcWearOut(void);

virtual bool ACalcThresholdLife(void);

virtual bool ACalcEnergy(void);

virtual bool ACalcFeel(void);

double FeelDiff(double kq, double ediff, double en);

//virtual bool ACalcEnergy(void);
};

}
#endif
