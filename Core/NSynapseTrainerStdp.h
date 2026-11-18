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

///Родительский класс STDP
class NSynapseTrainerStdp: public NSynapseTrainer
{
public:

UProperty<double,NSynapseTrainerStdp, ptPubParameter> APlus;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> AMinus;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> WMin;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> WMax;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsTrainEnable;

public: //  
UProperty<double,NSynapseTrainerStdp, ptPubState> TPre;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPost;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPreOld;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPostOld;

UProperty<double,NSynapseTrainerStdp, ptPubState> TDiff;

UProperty<double,NSynapseTrainerStdp, ptPubState> XYDiff;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsInputPulseActive;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsOutputPulseActive;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdp(void);
virtual ~NSynapseTrainerStdp(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdp* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetWMin(const double &value);

bool SetWMax(const double &value);
// --------------------------

bool WriteIntoFile(double deltaT, double deltaW);

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

///STDP, зависящий от времени напрямую
class NSynapseTrainerStdpTD: public NSynapseTrainerStdp
{
public:

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauX;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauY;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauMinus;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauPlus;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> XAvg;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> YAvg;

UProperty<double,NSynapseTrainerStdpTD, ptPubState> WRange;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpTD(void);
virtual ~NSynapseTrainerStdpTD(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpTD* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetTauX(const double &value);

bool SetTauY(const double &value);

bool SetTauMinus(const double &value);

bool SetTauPlus(const double &value);

bool SetXAvg(const double &value);

bool SetYAvg(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

///STDP, зависящий от веса напрямую
class NSynapseTrainerStdpWD: public NSynapseTrainerStdp
{
public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpWD(void);
virtual ~NSynapseTrainerStdpWD(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpWD* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

/// STDP - изначальный (не работает, по Лобову)
class NSynapseTrainerStdpLobov: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> XModCoeff;

UProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> YModCoeff;

UProperty<double,NSynapseTrainerStdpLobov,ptPubParameter> StartWeight;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpLobov(void);
virtual ~NSynapseTrainerStdpLobov(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpLobov* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetStartWeight(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};


///STDP - классический, лобовая реализация
class NSynapseTrainerStdpClassicDiscrete: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpClassicDiscrete, ptPubState> Mu;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpClassicDiscrete(void);
virtual ~NSynapseTrainerStdpClassicDiscrete(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpClassicDiscrete* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

///STDP - классический, интегрирование
class NSynapseTrainerStdpClassicIntegrated: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuPlus;
UProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuMinus;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpClassicIntegrated(void);
virtual ~NSynapseTrainerStdpClassicIntegrated(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpClassicIntegrated* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};


///STDP Triplet
class NSynapseTrainerStdpTriplet: public NSynapseTrainerStdpTD
{
public:

UProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> APlus3;

UProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> AMinus3;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpTriplet(void);
virtual ~NSynapseTrainerStdpTriplet(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpTriplet* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};


///STDP Mirror
class NSynapseTrainerStdpMirror: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> APlus3;

UProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> AMinus3;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> o1;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> o2;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> r1;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> r2;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpMirror(void);
virtual ~NSynapseTrainerStdpMirror(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpMirror* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};


///Вероятностный STDP
class NSynapseTrainerStdpProbabilistic: public NSynapseTrainerStdpWD
{
public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpProbabilistic(void);
virtual ~NSynapseTrainerStdpProbabilistic(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpProbabilistic* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

///Стабильный STDP
class NSynapseTrainerStdpStable: public NSynapseTrainerStdpWD
{
public:
UProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTP;

UProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTD;

public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainerStdpStable(void);
virtual ~NSynapseTrainerStdpStable(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainerStdpStable* New(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
bool SetTauLTP(const double &value);

bool SetTauLTD(const double &value);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
virtual bool ABuild(void);

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);
// --------------------------
};

}

#endif

