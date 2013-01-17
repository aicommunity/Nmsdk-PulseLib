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

#ifndef NRECEPTOR_H
#define NRECEPTOR_H

#include "../BCL/NSource.h"


namespace NMSDK {

class NReceptor: public NSource
{
public: // ������������� ��������
// ����������� �������� �������� �������
RDK::ULProperty<real,NReceptor> Gain;

// ������� �������� �������� �������
RDK::ULProperty<real,NReceptor> MinInputRange, MaxInputRange;

// ������� �������� ��������� �������
RDK::ULProperty<real,NReceptor> MinOutputRange, MaxOutputRange;

// ����������� ��������������� ��� ������ 2
RDK::ULProperty<real,NReceptor> ExpCoeff;

// ��������� ��� ������ 1
RDK::ULProperty<real,NReceptor> SumCoeff;

// ���������� ������� ��������� ������ �������� ���������
RDK::ULProperty<real,NReceptor> InputAdaptationArrestingTC;


// ����� ��������� ��������� �������� �������
// 0 - ��� ���������
// 1 - ���������� ���������� ���������
// 2 - ���������� � ����������, � ��������� ������� InputAdaptationArrestingTC
RDK::ULProperty<int,NReceptor> InputAdaptationMode;

// ����� ��������� ��������� �������� �������
// 0 - ��� ���������
// 1 - ���������������� �������������� �������� ������� x
// � �������� [0;Gain] y=(SumCoeff-exp(-ExpCoeff*x))*Gain
// 2 - ���������� 1, �� ������� y=exp(-Kx)*Gain
// 3 - ���������� 1, ExpCoeff ����������� �������������
// 4 - ��������� �� ������� y=Gain*(1.0+input);
RDK::ULProperty<int,NReceptor> OutputAdaptationMode;



protected: // �������� ��������

protected: // ��������� ����������
real InputRange;
real OutputRange;



public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NReceptor(void);
virtual ~NReceptor(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ����������� �������� �������� �������
bool SetGain(real value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NReceptor* New(void);
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

