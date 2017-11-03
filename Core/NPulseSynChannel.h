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

#ifndef NPULSE_SYN_CHANNEL_H
#define NPULSE_SYN_CHANNEL_H

#include "NPulseChannel.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseSynChannel: public NPulseChannel
{
public: // Общедоступные свойства
// Параметры синапсов
// Амплитуда входных импульсов
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> PulseAmplitude;

// Постоянная времени выделения медиатора
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> SecretionTC;

// Постоянная времени распада медиатора
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> DissociationTC;

// Коэффициент пресинаптического торможения
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> InhibitionCoeff;

// Вес (эффективность синапса) синапса
RDK::UProperty<double,NPulseSynChannel,ptPubParameter> SynapseResistance;

public: // Данные
// Промежуточное значение эффективности синапса
RDK::UProperty<vector<double>,NPulseSynChannel,ptPubState> PreOutput;

// Постоянные времени выделения и распада медиатора в единицах шага интегрирования
double VSecretionTC,VDissociationTC;

// Постоянная составляющая результатов вычислений
double OutputConstData;

//Значение выхода синапса
double syn_output;

protected: // Основные свойства

protected: // Временные переменные
/// Массив признаков является ли вход синапсом или нет
std::vector<bool> SynapseInputFlagsList;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynChannel(void);
virtual ~NPulseSynChannel(void);
// --------------------------

// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
size_t GetNumSynapses(void) const;

// Возвращает синапс по индексу
NPulseSynapse* GetSynapse(size_t i);
// --------------------------
double GetSynOutput(void);
bool ResetOut(void);
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);

// Постоянная времени выделения медиатора
bool SetSecretionTC(const double &value);

// Постоянная времени распада медиатора
bool SetDissociationTC(const double &value);

// Коэффициент пресинаптического торможения
bool SetInhibitionCoeff(const double &value);

// Вес (эффективность синапса) синапса
bool SetSynapseResistance(const double &value);
// --------------------------

// --------------------------
// Методы управления объектом
// --------------------------
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseSynChannel* New(void);
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
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
//virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
//virtual bool ADelComponent(UEPtr<UContainer> comp);
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
// --------------------------
};

class RDK_LIB_TYPE NContinuesSynChannel: public NPulseChannel
{
public: // Общедоступные свойства
// Параметры синапсов
// Амплитуда входных импульсов
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> PulseAmplitude;

// Постоянная времени выделения медиатора
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> SecretionTC;

// Постоянная времени распада медиатора
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> DissociationTC;

// Коэффициент пресинаптического торможения
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> InhibitionCoeff;

// Вес (эффективность синапса) синапса
RDK::UProperty<double,NContinuesSynChannel,ptPubParameter> SynapseResistance;

public: // Данные
// Промежуточное значение эффективности синапса
RDK::UProperty<vector<double>,NContinuesSynChannel,ptPubState> PreOutput;

// Постоянные времени выделения и распада медиатора в единицах шага интегрирования
double VSecretionTC,VDissociationTC;

// Постоянная составляющая результатов вычислений
double OutputConstData;


protected: // Основные свойства

protected: // Временные переменные
/// Массив признаков является ли вход синапсом или нет
std::vector<bool> SynapseInputFlagsList;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesSynChannel(void);
virtual ~NContinuesSynChannel(void);
// --------------------------

// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
size_t GetNumSynapses(void) const;

// Возвращает синапс по индексу
NPulseSynapse* GetSynapse(size_t i);
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool SetPulseAmplitude(const double &value);

// Постоянная времени выделения медиатора
bool SetSecretionTC(const double &value);

// Постоянная времени распада медиатора
bool SetDissociationTC(const double &value);

// Коэффициент пресинаптического торможения
bool SetInhibitionCoeff(const double &value);

// Вес (эффективность синапса) синапса
bool SetSynapseResistance(const double &value);
// --------------------------

// --------------------------
// Методы управления объектом
// --------------------------
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NContinuesSynChannel* New(void);
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
// Скрытые методы управления компонентами
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
//virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
//virtual bool ADelComponent(UEPtr<UContainer> comp);
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
// --------------------------
};

}
#endif
