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

#include "../../Kernel/NBios.h"


namespace NMSDK {

class NPulseNeuron;

class NLTZone: public NANet
{
public: // ������������� ��������
// ����� �������
RDK::ULProperty<real,NLTZone> Threshold;

public: // ������
// ������ �������� �������� ������
NPulseNeuron* Neuron;

protected: // �������� ��������

protected: // ��������� ����������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NLTZone(void);
virtual ~NLTZone(void);
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ����� �������
bool SetThreshold(real value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);
// --------------------------
};


class NPulseLTZone: public NLTZone
{
public: // ������������� ��������
// ���������� �������
RDK::ULProperty<real,NPulseLTZone> TimeConstant;

// ��������� ���������
RDK::ULProperty<real,NPulseLTZone> PulseAmplitude;

// ������������ ��������
RDK::ULProperty<real,NPulseLTZone> PulseLength;

// �������� ������� ������ ������� ���������
RDK::ULProperty<real,NPulseLTZone> AvgInterval;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ��������� ���������
RDK::ULStateProperty<real,NPulseLTZone> NeuralPotential;

// ������������� �������� ����������
RDK::ULStateProperty<real,NPulseLTZone> PrePotential;

// ���� ������� ���������
RDK::ULStateProperty<int,NPulseLTZone> PulseCounter;

// ������� ������� �� �������� �������� �������
RDK::UCLStateProperty<list<real>,NPulseLTZone> AvgFrequencyCounter;

// ������� ������� ��������� ��������
RDK::ULStateProperty<bool,NPulseLTZone> PulseFlag;

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
bool SetTimeConstant(real value);

// ������������� ��������� ���������
bool SetPulseAmplitude(real value);
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
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;
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


class NContinuesLTZone: public NLTZone
{
public: // ������������� ��������
// ���������� �������
RDK::ULProperty<real,NContinuesLTZone> TimeConstant;

// ��������� ���������
RDK::ULProperty<real,NContinuesLTZone> PulseAmplitude;

// ������������ ��������
RDK::ULProperty<real,NContinuesLTZone> PulseLength;

// �������� ������� ������ ������� ���������
RDK::ULProperty<real,NContinuesLTZone> AvgInterval;

public: // ������

protected: // �������� ��������

protected: // ��������� ����������
// ��������� ���������
RDK::ULStateProperty<real,NContinuesLTZone> NeuralPotential;

// ������������� �������� ����������
RDK::ULStateProperty<real,NContinuesLTZone> PrePotential;

// ���� ������� ���������
RDK::ULStateProperty<int,NContinuesLTZone> PulseCounter;

// ������� ������� �� �������� �������� �������
RDK::UCLStateProperty<list<real>,NContinuesLTZone> AvgFrequencyCounter;

// ������� ������� ��������� ��������
RDK::ULStateProperty<bool,NContinuesLTZone> PulseFlag;

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
bool SetTimeConstant(real value);

// ������������� ��������� ���������
bool SetPulseAmplitude(real value);
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
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;
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
