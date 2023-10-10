#ifndef NMSDK_NSYNAPSECLASSIC_H
#define NMSDK_NSYNAPSECLASSIC_H

#include "../../Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"

namespace NMSDK {

class NSynapseClassic: public NPulseSynapseCommon
{
public: // Параметры
/// Постоянная времени модели
ULProperty<double, NSynapseClassic, ptPubParameter> TauS;

/// Амплитудное значение тока
ULProperty<double, NSynapseClassic, ptPubParameter> Current;

public: // Входы и выходы

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseClassic(void);
virtual ~NSynapseClassic(void);
// --------------------------

// ---------------------
// Методы управления параметрами
// ---------------------
// ---------------------

// ---------------------
// Методы управления переменными состояния
// ---------------------
// ---------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseClassic* New(void);
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

// Сброс процесса счета без потери настроек
virtual bool AReset(void);

// Выполняет расчет этого объекта
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



