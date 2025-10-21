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

#ifndef NPULSE_GENERATOR_H
#define NPULSE_GENERATOR_H

#include "NSource.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"
#include "../../../Rdk/Core/Engine/ModernContainers.h"
#include "../../../Rdk/Core/System/ModernChrono.h"
#include "UEPtr.h"
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <random>

namespace NMSDK {

class RDK_LIB_TYPE NPulseGenerator: public NSource
{
public: // ���������
/// ������� ��������� (��) (�������������� ��������)
ULProperty<double,NPulseGenerator, ptPubParameter> Frequency;

/// ������������ �������� (�)
ULProperty<double,NPulseGenerator, ptPubParameter> PulseLength;

/// ��������� ��������
ULProperty<double,NPulseGenerator, ptPubParameter> Amplitude;

/// �������� ������� ����������
ULProperty<double,NPulseGenerator, ptPubParameter> Delay;

/// ���������� �������
/// (������ ��������, �� �������� ����������� �����������!)
ULProperty<double,NPulseGenerator, ptPubParameter> FrequencyDeviation;

/// �������� ������� ������ ������� ���������
ULProperty<double,NPulseGenerator, ptPubParameter> AvgInterval;

public: // ����� � ������
/// ��������� (��� ����� ���������� �������� PulseOutput) (1)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPotential;

/// ������� (2)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputFrequency;

/// ������ �������� ������� ������ ��������� (3)
UPropertyOutputData<MDMatrix<double>,NPulseGenerator, ptOutput | ptPubState> OutputPulseTimes;

public: // ���������� ���������
/// ������� ��������
ULProperty<int,NPulseGenerator,ptPubState> PulseCounter;

/// ��������� �������� ������� �������
ULProperty<double,NPulseGenerator,ptPubState> RandomFrequency;

/// ������� ������� �� �������� �������� �������
UCLProperty<list<double>,NPulseGenerator,ptPubState> AvgFrequencyCounter;

//���� ��������� ������� � ���������� ������� (UsePatternOutput)
bool IsInPatternMode;

protected:

double OldFrequency;

// ������� �������� ----->
// ����� ���������� ������ Reset
double ResetTime;
// <-----

// �������� ������� ----->
//����� ������ ������ ���������� � ������ ���������� ������� (UsePatternOutput)
double PatternStartTime;

// <-----


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseGenerator(void);
virtual ~NPulseGenerator(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ������� ���������
bool SetFrequency(const double &value);

// ������������� ������������ ��������
bool SetPulseLength(const double &value);

// ������������� ��������� ���������
bool SetAmplitude(const double &value);

// ������������� �������� ������� ����������
bool SetDelay(const double &value);

// ������������� ��������� ���������� �������
// (������ ��������, �� �������� ����������� �����������!)
bool SetFrequencyDeviation(const double &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseGenerator* New(void);
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

class RDK_LIB_TYPE NPulseGeneratorTransit: public NPulseGenerator
{
public: // ���������
/// ������� ������������� �������� ������� �� �������� ���������
ULProperty<bool,NPulseGeneratorTransit, ptPubParameter> UseTransitSignal;

/// ���������� ������
UPropertyInputData<MDMatrix<double>,NPulseGeneratorTransit, ptInput | ptPubState> Input;

/// ������� ������������� ��������� ������� � ���������� �������� PatternFrequency �� �������� ��������
/// ��� �������:
/// 1. ��������� � ���������� �������� � ������� ��������� ������� PatternDuration
/// 2. ��������� � ���������� �������� �� ��������� ���������� �������� �������, ������������� ���������
ULProperty<bool,NPulseGeneratorTransit, ptPubParameter> UsePatternOutput;

/// ����������������� ������� ������ � ������ ���������� ������� (UsePatternOutput) (c)
ULProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternDuration;

/// ������� ��������� � ������ ���������� ������� (UsePatternOutput) (��)
ULProperty<double,NPulseGeneratorTransit, ptPubParameter> PatternFrequency;


protected:

/// �����, ��� �� ����� ����������� ��� �� �������, ��� � �� ���������� �����
/// �� ����� �������� ���������� ��������� ������, � ������� ������� �������� > 0
/// �� ��������� ����� �� ������ ������������� ��� ����� �������, ���� TheSamePulse == true
bool TheSamePulse = false;


public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseGeneratorTransit(void);
virtual ~NPulseGeneratorTransit(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ������������� �������� ������� �� �������� ���������
bool SetUseTransitSignal(const bool &value);
// --------------------------
// ������������� ������������� ���������� ������� ������� (�� ������������ ������ �������) �� �������� ��������
bool SetUsePatternOutput(const bool &value);
// --------------------------
// ������������� ����������������� ������� ������ � ������ ���������� ������� (UsePatternOutput)
bool SetPatternDuration(const double &value);
// --------------------------
// ������������� ������� ��������� � ������ ���������� ������� (UsePatternOutput)
bool SetPatternFrequency(const double &value);
// --------------------------

// Modern C++20 thread-safe pulse generation
// Thread-safe pulse generation
void GeneratePulseSafe(double frequency, double amplitude, double duration);
void SetFrequencySafe(double frequency);
void SetAmplitudeSafe(double amplitude);

// Modern random number generation
std::shared_ptr<std::mt19937> GetRandomEngine(void) const;
void SetRandomSeed(std::chrono::system_clock::time_point seed = std::chrono::system_clock::now());

// Modern smart pointer factory
static std::shared_ptr<NPulseGenerator> Create(double frequency, double amplitude);

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseGeneratorTransit* New(void);
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

