#ifndef NMSDK_NPulseLTZoneIzhikevichH
#define NMSDK_NPulseLTZoneIzhikevichH

#include "../../Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"

namespace NMSDK {

class NPulseLTZoneIzhikevich: public NPulseLTZoneCommon
{
protected: // Свойства

protected: // Переменные состояния

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLTZoneIzhikevich(void);
virtual ~NPulseLTZoneIzhikevich(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseLTZoneIzhikevich* New(void);
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
virtual bool ACalculate(void);

/// Возвращает true если условие для генерации импульса выполнено
virtual bool CheckPulseOn(void);

/// Возвращает true если условие для генерации имульса не выполнено
virtual bool CheckPulseOff(void);
// --------------------------
};


}

#endif



