#ifndef NMSDK_NPulseNeuronIzhikevichH
#define NMSDK_NPulseNeuronIzhikevichH

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuronCommon.h"

namespace NMSDK {

class NPulseNeuronIzhikevich: public NPulseNeuronCommon
{
public: // Свойства

public: // Переменные состояния

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseNeuronIzhikevich(void);
virtual ~NPulseNeuronIzhikevich(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseNeuronIzhikevich* New(void);
// --------------------------        

// --------------------------
// Методы управления счетом
// --------------------------
/// Создает структуру простого нейрона
virtual void CreateSimpleNeuron(void);
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
// --------------------------
};


}

#endif



