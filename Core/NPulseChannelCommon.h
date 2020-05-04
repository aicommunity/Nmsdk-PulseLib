// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2016.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_CHANNEL_COMMON_H
#define NPULSE_CHANNEL_COMMON_H

#include "NPulseSynapse.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseChannelCommon: public UNet
{
public: // Общедоступные свойства
/// Тип ионного механизма
/// <0 - накапливает отрицательный вклад в потенциал (или гиперполяризует мембрану)
/// >0 - накапливает положительный вклад в потенциал (или деполяризует мембрану)
ULProperty<double,NPulseChannelCommon, ptPubParameter> Type;

/// Флаг включения усреднения в выходных данных нейрона
ULProperty<bool, NPulseChannelCommon, ptPubParameter> UseAveragePotential;

/// Флаг включения усреднения выходов всех синапсов
ULProperty<bool, NPulseChannelCommon, ptPubParameter> UseAverageSynapsis;

public: // Входы и выходы
/// Входной сигнал от канала предыдущего участка мембраны
UPropertyInputCData<MDMatrix<double>, NPulseChannelCommon, ptInput | ptPubState> ChannelInputs;

/// Входной сигнал от синапсов
UPropertyInputCData<MDMatrix<double>, NPulseChannelCommon, ptInput | ptPubState> SynapticInputs;

/// Выходное влияние синапса на мембрану
UPropertyOutputData<MDMatrix<double>,NPulseChannelCommon, ptOutput | ptPubState> Output;


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelCommon(void);
virtual ~NPulseChannelCommon(void);
// --------------------------

// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
virtual int GetNumSynapses(void) const;
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Тип ионного механизма
bool SetType(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseChannelCommon* New(void);
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
// --------------------------
};

}
#endif
