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

#ifndef NPULSE_SYNAPSE_H
#define NPULSE_SYNAPSE_H

#include "NPulseSynapseCommon.h"
#include "../../../Rdk/Core/Engine/ModernSmartPointers.h"
#include "../../../Rdk/Core/Engine/ModernContainers.h"
#include "../../../Rdk/Core/System/ModernChrono.h"
#include "UEPtr.h"
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>


namespace NMSDK {

class RDK_LIB_TYPE NPulseSynapse: public NPulseSynapseCommon
{
public: // ������������� ��������
/// ���������� ������� ��������� ���������
ULProperty<double,NPulseSynapse, ptPubParameter> SecretionTC;

/// ���������� ������� ������� ���������
ULProperty<double,NPulseSynapse, ptPubParameter> DissociationTC;

/// ����������� ����������������� ����������
ULProperty<double,NPulseSynapse, ptPubParameter> InhibitionCoeff;

/// ������� ������������ ��������, �
ULProperty<double,NPulseSynapse, ptPubParameter> TypicalPulseDuration;

/// ���� ������ � ������ ��������� ���������� �������� �� �������
ULProperty<bool,NPulseSynapse, ptPubParameter> UsePulseSignal;

/// ���� ��������� ����������������� ����������
ULProperty<bool,NPulseSynapse, ptPubParameter> UsePresynapticInhibition;

public: // ����� � ������

protected: // �������� ��������

protected: // ��������� ����������
// ���������� ������� ��������� � ������� ��������� � �������� ���� ��������������
double VSecretionTC,VDissociationTC;

// ���������� ������������ ����������� ����������
double OutputConstData;

/// ������� ����� �������� � ������� ������� �� ������ ������������ �������
int PulseCounter;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseSynapse(void);
virtual ~NPulseSynapse(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool SetPulseAmplitude(const double &value);

// ���������� ������� ��������� ���������
bool SetSecretionTC(const double &value);

// ���������� ������� ������� ���������
bool SetDissociationTC(const double &value);

/// ������� ������������ ��������, �
bool SetTypicalPulseDuration(const double &value);

// ����������� ����������������� ����������
bool SetInhibitionCoeff(const double &value);

/// ���� ������ � ������ ��������� ���������� �������� �� �������
bool SetUsePulseSignal(const bool &value);

// ������� ����� ��������� ����������������� ����������
bool SetUsePresynapticInhibition(const bool &value);

// ��� (������������� �������) �������
bool SetResistance(const double &value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseSynapse* New(void);
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
virtual bool ACalculate2(void);
// --------------------------

// --------------------------
// Modern C++20 methods
// --------------------------
// Move semantics
NPulseSynapse(const NPulseSynapse&) = default;
NPulseSynapse(NPulseSynapse&&) noexcept = default;
NPulseSynapse& operator=(const NPulseSynapse&) = default;
NPulseSynapse& operator=(NPulseSynapse&&) noexcept = default;

// Modern synapse operations
void OptimizeSynapsePerformance();
bool IsSynapseValid() const;
void ReserveSynapseMemory(size_t neurotransmitter_count);
TimePoint GetLastTransmissionTime() const;
void SetLastTransmissionTime(TimePoint time);

// Modern neurotransmitter operations
template<typename T>
void ReserveNeurotransmitters(std::vector<T>& neurotransmitters, size_t count);

template<typename T>
void MoveNeurotransmitters(std::vector<T>& target, std::vector<T>&& source);

// STDP (Spike-Timing Dependent Plasticity) operations
void UpdateSTDPWeights(TimePoint pre_spike_time, TimePoint post_spike_time);
double CalculateSTDPWeight(double time_difference) const;
// --------------------------
};

}
#endif
