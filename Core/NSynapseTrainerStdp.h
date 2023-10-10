#ifndef NSYNAPSE_TRAINER_STDP_H
#define NSYNAPSE_TRAINER_STDP_H

#include "../../Nmsdk-PulseLib/Core/NSynapseTrainer.h"

/// NSynapseTrainerStdp
/// NSynapseTrainerStdpTD
/// NSynapseTrainerStdpWD
/// NSynapseTrainerStdpLobov
/// NSynapseTrainerStdpClassicDiscrete
/// NSynapseTrainerStdpClassicIntegrated
/// NSynapseTrainerStdpTriplet
/// NSynapseTrainerStdpMirror
/// NSynapseTrainerStdpProbabilistic
/// NSynapseTrainerStdpStable



namespace NMSDK {

///������������ ����� STDP
class NSynapseTrainerStdp: public NSynapseTrainer
{
public:

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> APlus;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> AMinus;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> WMin;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> WMax;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsTrainEnable;

public: // ���������� ���������
ULProperty<double,NSynapseTrainerStdp, ptPubState> TPre;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPost;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPreOld;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPostOld;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TDiff;

ULProperty<double,NSynapseTrainerStdp, ptPubState> XYDiff;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsInputPulseActive;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsOutputPulseActive;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdp(void);
virtual ~NSynapseTrainerStdp(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdp* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetWMin(const double &value);

bool SetWMax(const double &value);
// --------------------------

bool WriteIntoFile(double deltaT, double deltaW);

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

///STDP, ��������� �� ������� ��������
class NSynapseTrainerStdpTD: public NSynapseTrainerStdp
{
public:

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauX;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauY;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauMinus;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauPlus;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> XAvg;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> YAvg;

ULProperty<double,NSynapseTrainerStdpTD, ptPubState> WRange;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpTD(void);
virtual ~NSynapseTrainerStdpTD(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpTD* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetTauX(const double &value);

bool SetTauY(const double &value);

bool SetTauMinus(const double &value);

bool SetTauPlus(const double &value);

bool SetXAvg(const double &value);

bool SetYAvg(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

///STDP, ��������� �� ���� ��������
class NSynapseTrainerStdpWD: public NSynapseTrainerStdp
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpWD(void);
virtual ~NSynapseTrainerStdpWD(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpWD* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

/// STDP - ����������� (�� ��������, �� ������)
class NSynapseTrainerStdpLobov: public NSynapseTrainerStdpTD
{
public:
ULProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> XModCoeff;

ULProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> YModCoeff;

ULProperty<double,NSynapseTrainerStdpLobov,ptPubParameter> StartWeight;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpLobov(void);
virtual ~NSynapseTrainerStdpLobov(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpLobov* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetStartWeight(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


///STDP - ������������, ������� ����������
class NSynapseTrainerStdpClassicDiscrete: public NSynapseTrainerStdpTD
{
public:
ULProperty<double,NSynapseTrainerStdpClassicDiscrete, ptPubState> Mu;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpClassicDiscrete(void);
virtual ~NSynapseTrainerStdpClassicDiscrete(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpClassicDiscrete* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

///STDP - ������������, ��������������
class NSynapseTrainerStdpClassicIntegrated: public NSynapseTrainerStdpTD
{
public:
ULProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuPlus;
ULProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuMinus;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpClassicIntegrated(void);
virtual ~NSynapseTrainerStdpClassicIntegrated(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpClassicIntegrated* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


///STDP Triplet
class NSynapseTrainerStdpTriplet: public NSynapseTrainerStdpTD
{
public:

ULProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> APlus3;

ULProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> AMinus3;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpTriplet(void);
virtual ~NSynapseTrainerStdpTriplet(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpTriplet* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


///STDP Mirror
class NSynapseTrainerStdpMirror: public NSynapseTrainerStdpTD
{
public:
ULProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> APlus3;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> AMinus3;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> o1;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> o2;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> r1;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> r2;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpMirror(void);
virtual ~NSynapseTrainerStdpMirror(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpMirror* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};


///������������� STDP
class NSynapseTrainerStdpProbabilistic: public NSynapseTrainerStdpWD
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpProbabilistic(void);
virtual ~NSynapseTrainerStdpProbabilistic(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpProbabilistic* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

///���������� STDP
class NSynapseTrainerStdpStable: public NSynapseTrainerStdpWD
{
public:
ULProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTP;

ULProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTD;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpStable(void);
virtual ~NSynapseTrainerStdpStable(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NSynapseTrainerStdpStable* New(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool SetTauLTP(const double &value);

bool SetTauLTD(const double &value);
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
virtual bool ABuild(void);

// ����� �������� ����� ��� ������ ��������
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}

#endif

