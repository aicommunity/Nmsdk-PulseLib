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

#ifndef NMUSCLE_H
#define NMUSCLE_H

#include "../../Kernel/NBios.h"
#include "../BCL/NItem.h"
#include "../ReceiverLib/NReceptor.h"

namespace NMSDK {

//using namespace MySDK;

class NMuscle: public NANet
{
public: // ������������� ��������
// ������������
RDK::UCLProperty<Real,NMuscle> MulCoeffs;

RDK::UCLProperty<Real,NMuscle> Param;

// ���������� �������
RDK::UCLProperty<Real,NMuscle> TC;

// �����
RDK::ULProperty<real,NMuscle> Mass;

// �����
RDK::ULProperty<real,NMuscle> Threshold;

// �������� ��������� ���������� �������
RDK::ULProperty<real,NMuscle> G;

public: // ��������� ����������
real y[15], yOld[15], mr[7], mrOld[7],vereteno1, vereteno2, goldgi;
real MaxActionPower;
bool FirstStep;

// ��������� �� ���� �� �����
NReceptor *LengthInput;
/*
// ��������� �� ���� �� ������� ����
NReceptor *ExternalForce;

// ��������� �� �������������� ������
// �������� ����
NItem *ActivePower;

// �����
NItem *Length;

// ��������
NItem *Spindle1, *Spindle2;

// ����������� ����� �������
NItem *Tendon;
             */
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NMuscle(void);
virtual ~NMuscle(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ������
//bool SetAmplitude(real value);

// ������������� ������� ���������
//bool SetFrequency(real value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NMuscle* New(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<NAContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UAContainer> comp);
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

// --------------------------
// �������������� ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �����
real AperiodicSection(real in, real inOld, real param1, real param2);

// �����
real CompareThreshold(real);

// ���������� ����������
real MuscularReduction(real);

// ����������� ������� �� �����
real MuscleFeedback(real x_new, real x_old, real y_old,
					  real k, real T1, real T2);
// --------------------------
};

}
#endif

