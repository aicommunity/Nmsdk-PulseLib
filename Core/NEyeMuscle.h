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

#ifndef NEYEMUSCLE_H
#define NEYEMUSCLE_H

#include "../../Kernel/NBios.h"

namespace NMSDK {

//using namespace MySDK;

class NEyeMuscle: public NANet
{
public: // ������������� ��������
// ������������
RDK::UCLProperty<Real,NEyeMuscle> MulCoeffs;

RDK::ULProperty<real,NEyeMuscle> K;

// ���������� �������
RDK::UCLProperty<Real,NEyeMuscle> TC;

public: // ��������� ����������
Real P1,P2,P3,L;

// �����
Real Threshold;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NEyeMuscle(void);
virtual ~NEyeMuscle(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ������
bool SetAmplitude(real value);

// ������������� ������� ���������
bool SetFrequency(real value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NEyeMuscle* New(void);
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
// ���������� ����������
real MuscularReduction(size_t k,real in);

// ��������� �������
void ThresholdCount(size_t k);
// --------------------------
};

}
#endif

