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

#include "../../Nmsdk-BasicLib/Core/NSource.h"


namespace NMSDK {

class RDK_LIB_TYPE NReceptor: public NSource
{
public: // ������������� ��������
// ����������� �������� �������� �������
RDK::ULProperty<double,NReceptor, ptPubParameter> Gain;

// ������� �������� �������� �������
RDK::ULProperty<double,NReceptor, ptPubParameter> MinInputRange, MaxInputRange;

// ������� �������� ��������� �������
RDK::ULProperty<double,NReceptor, ptPubParameter> MinOutputRange, MaxOutputRange;

// ����������� ��������������� ��� ������ 2
RDK::ULProperty<double,NReceptor, ptPubParameter> ExpCoeff;

// ��������� ��� ������ 1
RDK::ULProperty<double,NReceptor, ptPubParameter> SumCoeff;

// ���������� ������� ��������� ������ �������� ���������
RDK::ULProperty<double,NReceptor, ptPubParameter> InputAdaptationArrestingTC;


// ����� ��������� ��������� �������� �������
// 0 - ��� ���������
// 1 - ���������� ���������� ���������
// 2 - ���������� � ����������, � ��������� ������� InputAdaptationArrestingTC
RDK::ULProperty<int,NReceptor, ptPubParameter> InputAdaptationMode;

// ����� ��������� ��������� �������� �������
// 0 - ��� ���������
// 1 - ���������������� �������������� �������� ������� x
// � �������� [0;Gain] y=(SumCoeff-exp(-ExpCoeff*x))*Gain
// 2 - ���������� 1, �� ������� y=exp(-Kx)*Gain
// 3 - ���������� 1, ExpCoeff ����������� �������������
// 4 - ��������� �� ������� y=Gain*(1.0+input);
RDK::ULProperty<int,NReceptor, ptPubParameter> OutputAdaptationMode;

protected: // ����� � ������
/// ������� ������ � �������
UPropertyInputData<MDMatrix<double>, NReceptor, ptInput | ptPubState> Input;

protected: // ��������� ����������
double InputRange;
double OutputRange;



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
bool SetGain(const double &value);
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

