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

#include "../../Kernel/NSupport.h"
#include "../SourceLib/NPulseGenerator.h"


namespace NMSDK {

class NPulseNeuron;

class NLTZone: public UANet
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
RDK::ULProperty<real,NPulseLTZone,ptPubState> NeuralPotential;

// ������������� �������� ����������
RDK::ULProperty<real,NPulseLTZone,ptPubState> PrePotential;

// ���� ������� ���������
RDK::ULProperty<int,NPulseLTZone,ptPubState> PulseCounter;

// ������� ������� �� �������� �������� �������
RDK::UCLProperty<list<real>,NPulseLTZone,ptPubState> AvgFrequencyCounter;

// ������� ������� ��������� ��������
RDK::ULProperty<bool,NPulseLTZone,ptPubState> PulseFlag;

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
RDK::ULProperty<real,NContinuesLTZone,ptPubState> NeuralPotential;

// ������������� �������� ����������
RDK::ULProperty<real,NContinuesLTZone,ptPubState> PrePotential;

// ���� ������� ���������
RDK::ULProperty<int,NContinuesLTZone,ptPubState> PulseCounter;

// ������� ������� �� �������� �������� �������
RDK::UCLProperty<list<real>,NContinuesLTZone,ptPubState> AvgFrequencyCounter;

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

class NPulseSimpleLTZone: public NPulseLTZone
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

class NContinuesSimpleLTZone: public NContinuesLTZone
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
bool SetTimeConstant(real value);

// ������������� ��������� ���������
bool SetPulseAmplitude(real value);
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
