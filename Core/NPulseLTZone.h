// ===========================================================
// Version:        3.0.3
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

#ifndef NPULSE_LTZONE_H
#define NPULSE_LTZONE_H

#include "../../Nmsdk-BasicLib/Core/NSupport.h"
#include "../../Nmsdk-SourceLib/Core/NPulseGenerator.h"
#include "NPulseLTZoneCommon.h"

namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseLTZone: public NPulseLTZoneCommon
{
public: // ������������� ��������
// ���������� �������
RDK::ULProperty<double,NPulseLTZone> TimeConstant;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ��������� ���������
RDK::ULProperty<double,NPulseLTZone,ptPubState> NeuralPotential;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLTZone(void);
virtual ~NPulseLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool SetTimeConstant(const double &value);

// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseLTZone* New(void);
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

/// ���������� true ���� ������� ��� ��������� �������� ���������
virtual bool CheckPulseOn(void);

/// ���������� true ���� ������� ��� ��������� ������� �� ���������
virtual bool CheckPulseOff(void);
// --------------------------
};

class RDK_LIB_TYPE NContinuesLTZone: public NLTZone
{
public: // ������������� ��������
// ���������� �������
RDK::ULProperty<double,NContinuesLTZone> TimeConstant;

// ��������� ���������
RDK::ULProperty<double,NContinuesLTZone> PulseAmplitude;

// ������������ ��������
RDK::ULProperty<double,NContinuesLTZone> PulseLength;

// �������� ������� ������ ������� ���������
RDK::ULProperty<double,NContinuesLTZone> AvgInterval;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ��������� ���������
RDK::ULProperty<double,NContinuesLTZone,ptPubState> NeuralPotential;

// ������������� �������� ����������
RDK::ULProperty<double,NContinuesLTZone,ptPubState> PrePotential;

// ���� ������� ���������
RDK::ULProperty<int,NContinuesLTZone,ptPubState> PulseCounter;

// ������� ������� �� �������� �������� �������
RDK::UCLProperty<list<double>,NContinuesLTZone,ptPubState> AvgFrequencyCounter;

// ������� ������� ��������� ��������
RDK::ULProperty<bool,NContinuesLTZone,ptPubState> PulseFlag;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesLTZone(void);
virtual ~NContinuesLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool SetTimeConstant(const double &value);

// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NContinuesLTZone* New(void);
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

class RDK_LIB_TYPE NPulseSimpleLTZone: public NPulseLTZone
{
public: // ������������� ��������

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
NPulseGenerator generator;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSimpleLTZone(void);
virtual ~NPulseSimpleLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSimpleLTZone* New(void);
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

class RDK_LIB_TYPE NContinuesSimpleLTZone: public NContinuesLTZone
{
public: // ������������� ��������

public: // ������

protected: // �������� ��������

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NContinuesSimpleLTZone(void);
virtual ~NContinuesSimpleLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� �������� ���������� �������
bool SetTimeConstant(const double &value);

// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NContinuesSimpleLTZone* New(void);
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
