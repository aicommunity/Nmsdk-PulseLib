// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NLIFE_NET_H
#define NLIFE_NET_H

#include "NNet.h"
#include "NNeuronLife.h"
#include "NPulseLifeNeuron.h"
#include "NPulseHebbSynapse.h"

namespace NMSDK {

// Тип ансамбля нейронов
typedef vector<NPulseLifeNeuron*> NEnsemble;

// Тип связи между ансамблями
struct RDK_LIB_TYPE NEnsembleLink
{
// Ансамбль-источник сигнала
NameT In;

// Ансамбль-приемник сигнала
NameT Out;

// Мощность связи
double Power;

// ------------------------
// Операторы
// ------------------------
friend bool operator < (const NEnsembleLink &link1, const NEnsembleLink &link2);
// ------------------------
};

class RDK_LIB_TYPE NLifeNet: public NNet
{
public: // Общедоступные свойства
/// Суммарная энергия сети
ULProperty<double,NLifeNet, ptPubParameter> AvailableEnergy;

/// Флаг разрешающий нейронам деление
ULProperty<bool,NLifeNet, ptPubParameter> CellDivisionFlag;

/// Запас энергии, по достижении которого клетка делится
ULProperty<double,NLifeNet, ptPubParameter> CellDivisionEnergy;

/// Порог принятия решения об объединении нейронов в ансамбль
ULProperty<double,NLifeNet, ptPubParameter> EnsembleThreshold;

/// Порог принятия решения о наличии связи между ансамблями
ULProperty<double,NLifeNet, ptPubParameter> EnsembleLinkThreshold;

/// Режим отсчета порога принятия решения
/// 0 - от нуля
/// 1 - от среднего Gs синапсов данного нейрона
/// 2 - от среднего Gs синапсов всех нейронов сети
ULProperty<int,NLifeNet, ptPubParameter> EnsembleThresholdMode;

/// Интервал обсчета состояний ансамблей (в секундах)
/// Если 0, то расчет ансамблей не производится
ULProperty<double,NLifeNet, ptPubParameter> EnsembleCalculationStep;

/// Метод деления энергии между нейронами
/// 0 - всем поровну
/// 1 - приоритет у генерирующих нейронов
ULProperty<int,NLifeNet, ptPubParameter> EnergyBalanceMode;

public: // Входы и выходы
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output1;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output2;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output3;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output4;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output5;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output6;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output7;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output8;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output9;
UPropertyOutputData<MDMatrix<double>,NLifeNet, ptOutput | ptPubState> Output10;

public: // Переменные состояния
/// Суммарное самочувствие нейронов сети
ULProperty<double,NLifeNet,ptPubState> SummaryFeeling;

/// Суммарное расход энергии нейронами сети
ULProperty<double,NLifeNet,ptPubState> SummaryEnergy;

/// Суммарный износ нейронов сети
ULProperty<double,NLifeNet,ptPubState> SummaryWearOut;

/// Суммарная частота активности нейронов сети
ULProperty<double,NLifeNet,ptPubState> SummaryFrequency;

/// Суммарная число живых нейронов сети
ULProperty<double,NLifeNet,ptPubState> SummaryLiveNeurons;

protected: // Данные
/// Список систем жизнеобеспечения отдельных нейронов
vector<NNeuronLife*> NeuronsLife;

/// Массив пар <мощность ансамбля,бета-ансамбль> нейронов
vector<pair<double, NEnsemble > > BetaEnsembles;

/// Массив пар <мощность ансамбля,ансамбль> нейронов
vector<pair<double, NEnsemble > > Ensembles;

/// Массив связей между ансамблями
/// <Мощность связи, связь>
vector<NEnsembleLink> EnsembleLinks;

protected: // Временные переменные
/// Список синапсов нейронов сети
map<NPulseLifeNeuron*,vector<NPulseHebbSynapse*> > SynapticMap;
typedef map<NPulseLifeNeuron*,vector<NPulseHebbSynapse*> >::iterator SynapticMapIterator;

int EnsembleCalculationCounter;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLifeNet(void);
virtual ~NLifeNet(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает число систем жизнеобеспечения нейронов
size_t GetNumNeuronsLife(void) const;

// Возвращает заданную систему жизнеобеспечения нейрона
NNeuronLife* GetNeuronLife(size_t i);

// Возвращает число бета-ансамблей нейронов
size_t GetNumBetaEnsembles(void) const;

// Возвращает заданный бета-ансамбль
const NEnsemble& GetBetaEnsemble(size_t i);

// Возвращает мощность заданного бета-ансамбля
double GetBetaEnsemblePower(size_t i) const;

// Возвращает число ансамблей нейронов
size_t GetNumEnsembles(void) const;

// Возвращает заданный ансамбль
const NEnsemble& GetEnsemble(size_t i);

// Возвращает мощность заданного ансамбля
double GetEnsemblePower(size_t i) const;

// Возвращает имя ансамбля по имени составляющих нейронов
NameT GetEnsembleName(size_t i) const;

// Возвращает абсолютное значение активности ансамбля
// (средняя частота всех нейронов ансамбля)
double GetEnsembleActivity(size_t i) const;

// Возвращает значение активности ансамбля
// относительно активности сети
double GetRelativeEnsembleActivity(size_t i) const;

// Возвращает число связей между ансамблями
size_t GetNumEnsembleLinks(void) const;

// Возвращает связь между ансамблями
const NEnsembleLink& GetEnsembleLink(size_t i) const;
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NLifeNet* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
virtual bool ABuild(void);

// Сброс процесса счета.
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate(void);

// Вычисляет ансамбли сети
bool CalcEnsembles(double threshold);

// Вычисляет связи между ансамблями сети
bool CalcEnsembleLinks(double threshold);
// --------------------------
};

}
#endif


