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

#include "../../Nmsdk-BasicLib/Core/NSupport.h"

namespace NMSDK {

//using namespace MySDK;

class RDK_LIB_TYPE NEyeMuscle: public UDynamicMatNet
{
public: // ������������� ��������
// ������������
RDK::UPropertyRange<double,Real,NEyeMuscle,ptPubParameter> MulCoeffs;

RDK::UProperty<double,NEyeMuscle,ptPubParameter> K;

// ���������� �������
RDK::UPropertyRange<double,Real,NEyeMuscle,ptPubParameter> TC;

public: // ��������� ����������
Real P1,P2,P3,L;

// �����
Real Threshold;

// ��������
Real Speed;

// ���������
Real Acceleration;

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
bool SetAmplitude(double value);

// ������������� ������� ���������
bool SetFrequency(double value);
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
double MuscularReduction(size_t k,double in);

// ��������� �������
void ThresholdCount(size_t k);
// --------------------------
};

}
#endif

