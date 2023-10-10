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

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> APlus;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> AMinus;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> WMin;

ULProperty<double,NSynapseTrainerStdp, ptPubParameter> WMax;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsTrainEnable;

public: // Переменные состояния
ULProperty<double,NSynapseTrainerStdp, ptPubState> TPre;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPost;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPreOld;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TPostOld;

ULProperty<double,NSynapseTrainerStdp, ptPubState> TDiff;

ULProperty<double,NSynapseTrainerStdp, ptPubState> XYDiff;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsInputPulseActive;

ULProperty<bool,NSynapseTrainerStdp, ptPubState> IsOutputPulseActive;

public: // Методы
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

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauX;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauY;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauMinus;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauPlus;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> XAvg;

ULProperty<double,NSynapseTrainerStdpTD, ptPubParameter> YAvg;

ULProperty<double,NSynapseTrainerStdpTD, ptPubState> WRange;

public: // Методы
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
public: // Методы
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
ULProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> XModCoeff;

ULProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> YModCoeff;

ULProperty<double,NSynapseTrainerStdpLobov,ptPubParameter> StartWeight;

public: // Методы
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
ULProperty<double,NSynapseTrainerStdpClassicDiscrete, ptPubState> Mu;

public: // Методы
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
ULProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuPlus;
ULProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuMinus;

public: // Методы
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

ULProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> APlus3;

ULProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> AMinus3;

public: // Методы
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
ULProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> APlus3;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> AMinus3;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> o1;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> o2;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> r1;

ULProperty<double,NSynapseTrainerStdpMirror, ptPubState> r2;

public: // Методы
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
public: // Методы
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
ULProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTP;

ULProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTD;

public: // Методы
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

