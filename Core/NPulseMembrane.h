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

#ifndef NPULSE_MEMBRANE_H
#define NPULSE_MEMBRANE_H

#include "NPulseMembraneCommon.h"
#include "NPulseChannel.h"


namespace NMSDK {

class NPulseNeuron;

class RDK_LIB_TYPE NPulseMembrane: public NPulseMembraneCommon
{
public: // Основные свойства
/// Коэффициент обратной связи
ULProperty<double,NPulseMembrane, ptPubParameter> FeedbackGain;

/// Наличие механизма сброса
ULProperty<bool,NPulseMembrane, ptPubParameter> ResetAvailable;

/// Имя класса синапса
ULProperty<std::string, NPulseMembrane, ptPubParameter> SynapseClassName;

/// Имя класса ионного канала
ULProperty<std::string, NPulseMembrane, ptPubParameter> ExcChannelClassName;

/// Имя класса ионного канала
ULProperty<std::string, NPulseMembrane, ptPubParameter> InhChannelClassName;

/// Число возбуждающих синапсов
ULProperty<int, NPulseMembrane, ptPubParameter> NumExcitatorySynapses;

/// Число тормозных синапсов
ULProperty<int, NPulseMembrane, ptPubParameter> NumInhibitorySynapses;

public: // Входы и выходы
/// Сигнал обратной связи от низкопороговой зоны
UPropertyInputData<MDMatrix<double>, NPulseMembrane, ptInput | ptPubState> InputFeedbackSignal;


protected: // Временные переменные
// Ионные механизмы деполяризации
vector<NPulseChannelCommon*> ExcitatoryChannels;

// Ионные механизмы гиперполяризации
vector<NPulseChannelCommon*> InhibitoryChannels;

// Возбуждающие синапсы
vector<NPulseSynapseCommon*> ExcitatorySynapses;

// Тормозные синапсы
vector<NPulseSynapseCommon*> InhibitorySynapses;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembrane(void);
virtual ~NPulseMembrane(void);
// --------------------------

// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы деполяризации
size_t GetNumPosChannels(void) const;
NPulseChannelCommon* GetPosChannel(size_t i);

// Ионные механизмы гиперполяризации
size_t GetNumNegChannels(void) const;
NPulseChannelCommon* GetNegChannel(size_t i);

// Возбуждающие синапсы
size_t GetNumExcitatorySynapses(void) const;
NPulseSynapseCommon* GetExcitatorySynapses(size_t i);

// Тормозные синапсы
size_t GetNumInhibitorySynapses(void) const;
NPulseSynapseCommon* GetInhibitorySynapses(size_t i);

virtual bool UpdateChannelData(UEPtr<NPulseChannelCommon> channel, UEPtr<UIPointer> pointer=0);

virtual bool UpdateSynapseData(UEPtr<NPulseSynapseCommon> synapse, UEPtr<UIPointer> pointer=0);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Наличие механизма сброса
bool SetResetAvailable(const bool &value);

// Коэффициент обратной связи
bool SetFeedbackGain(const double &value);

bool SetSynapseClassName(const std::string &value);
bool SetExcChannelClassName(const std::string &value);
bool SetInhChannelClassName(const std::string &value);
bool SetNumExcitatorySynapses(const int &value);
bool SetNumInhibitorySynapses(const int &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseMembrane* New(void);

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
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
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
virtual bool ACalculate2(void);

// Устанавливает компоненты в требуемый порядок расчета
void UpdateComputationOrder(void);
// --------------------------
};

}
#endif

