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

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
//#include "../BCL/NItem.h"
#include "NReceptor.h"

namespace NMSDK {

//using namespace MySDK;

class RDK_LIB_TYPE NMuscle: public UNet
{
public: // ������������� ��������
// ������������
RDK::UCLProperty<Real,NMuscle> MulCoeffs;

RDK::UCLProperty<Real,NMuscle> Param;

// ���������� �������
RDK::UCLProperty<Real,NMuscle> TC;

// �����
RDK::ULProperty<double,NMuscle> Mass;

// �����
RDK::ULProperty<double,NMuscle> Threshold;

// �������� ��������� ���������� �������
RDK::ULProperty<double,NMuscle> G;

public: // ��������� ����������
double y[15], yOld[15], mr[7], mrOld[7],vereteno1, vereteno2, goldgi;
double MaxActionPower;
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
//bool SetAmplitude(double value);

// ������������� ������� ���������
//bool SetFrequency(double value);
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
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
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
// --------------------------

// --------------------------
// �������������� ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �����
double AperiodicSection(double in, double inOld, double param1, double param2);

// �����
double CompareThreshold(double);

// ���������� ����������
double MuscularReduction(double);

// ����������� ������� �� �����
double MuscleFeedback(double x_new, double x_old, double y_old,
					  double k, double T1, double T2);
// --------------------------
};

}
#endif

