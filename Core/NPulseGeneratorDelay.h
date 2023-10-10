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

#ifndef NPULSE_GENERATOR_DELAY_H
#define NPULSE_GENERATOR_DELAY_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseGeneratorDelay: public NSource
{
public: // ���������
/// ������������ �������� (�)
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> PulseLength;

/// ��������� ��������
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> Amplitude;

/// ������� �������� ������ (�� 1 �� 4)
ULProperty<int,NPulseGeneratorDelay, ptPubParameter> ParameterData;

/// ����������� �������� �������
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> MinSample;

/// ������������ �������� �������
ULProperty<double,NPulseGeneratorDelay, ptPubParameter> MaxSample;

protected: // ����������� ����������

/// �������� ������� ����������
double Delay;

public: // ����� � ������
/// ���� ������������� �����
ULProperty<double,NPulseGeneratorDelay, ptPubInput> Number;

/// ������ ������� ��������
ULProperty<int,NPulseGeneratorDelay,ptPubInput> FlagStart;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseGeneratorDelay(void);
virtual ~NPulseGeneratorDelay(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------

// ������������� ������������ ��������
bool SetPulseLength(const double &value);

// ������������� ��������� ���������
bool SetAmplitude(const double &value);

// ������������� ��������� ���������
bool SetParameterData(const int &value);

// ������������� ����������� �������� � ������
bool SetMinSample(const double &value);

// ������������� ������������ �������� � ������
bool SetMaxSample(const double &value);

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseGeneratorDelay* New(void);
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
};
}
#endif

