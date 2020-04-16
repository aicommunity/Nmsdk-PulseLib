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

#ifndef NAFFERENT_NEURON_H
#define NAFFERENT_NEURON_H

#include "NPulseNeuron.h"


namespace NMSDK {

class RDK_LIB_TYPE NAfferentNeuron: public NPulseNeuronCommon
{
public: // Основные свойства
/// Режим сборки структуры нейрона
/// (В режимах >1) осуществляется автомтический выбор классов участков мембраны и низкопороговой зоны
/// 0 - автоматическая сборка не производится
/// 1 - автоматическая сборка классического афферентного нейрона (с участком мембраны)
/// 2 - автоматическая сборка упрощенного афферентного нейрона (рецептор+низкопороговая зона)
ULProperty<int,NAfferentNeuron, ptPubParameter> StructureBuildMode;

/// Имя класса участка мембраны
ULProperty<std::string, NAfferentNeuron, ptPubParameter> MembraneClassName;

/// Имя класса генераторной зоны
ULProperty<std::string, NAfferentNeuron, ptPubParameter> LTZoneClassName;

/// Имя класса рецептора
ULProperty<std::string, NAfferentNeuron, ptPubParameter> ReceptorClassName;

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
//ULProperty<std::string, NAfferentNeuron, ptPubParameter> ExcGeneratorClassName;

/// Имя класса источника задающего сигнала для тормозного ионного механизма
ULProperty<std::string, NAfferentNeuron, ptPubParameter> InhGeneratorClassName;

/// Число участков мембраны тела нейрона
ULProperty<int, NAfferentNeuron, ptPubParameter> NumSomaMembraneParts;

/// Максимальная величина входного значения рецептора
ULProperty<double, NAfferentNeuron, ptPubParameter> MaxReceptorOutput;

public: // Временные переменные
//RDK::UEPointer<NReceptor,NPulseNeuronCommon> Receptor;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NAfferentNeuron(void);
virtual ~NAfferentNeuron(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
/// Режим сборки структуры нейрона
bool SetStructureBuildMode(const int &value);

/// Имя класса участка мембраны
bool SetMembraneClassName(const std::string &value);

/// Имя класса генераторной зоны
bool SetLTZoneClassName(const std::string &value);

/// Имя класса рецептора
bool SetReceptorClassName(const std::string &value);

/// Имя класса источника задающего сигнала для возбуждающего ионного механизма
//bool SetExcGeneratorClassName(const std::string &value);

/// Имя класса источника задающего сигнала для тормозного ионного механизма
bool SetInhGeneratorClassName(const std::string &value);

/// Число участков мембраны тела нейрона
bool SetNumSomaMembraneParts(const int &value);

/// Максимальная величина входного значения рецептора
bool SetMaxReceptorOutput(const double &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NAfferentNeuron* New(void);
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
/// Осуществляет сборку классической структуры
bool BuildClassicalStructure(const string &membraneclass, const string &ltzone_class,
					const string &receptorclass, const string &pos_gen_class,
					/*const string &neg_gen_class,*/ int num_membranes);

/// Осуществляет сборку упрощенной структуры
bool BuildSimpleStructure(const string &ltzone_class,
					const string &receptorclass, double max_output);

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
