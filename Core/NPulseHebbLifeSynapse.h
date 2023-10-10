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

#ifndef NPULSE_HEBB_LIFE_SYNAPSE_H
#define NPULSE_HEBB_LIFE_SYNAPSE_H

#include "../../Nmsdk-PulseLib/Core/NPulseHebbSynapse.h"


namespace NMSDK {

class RDK_LIB_TYPE NPulseHebbLifeSynapse: public NPulseHebbSynapse
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseHebbLifeSynapse(void);
virtual ~NPulseHebbLifeSynapse(void);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseHebbLifeSynapse* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Подключает синапс хебба к модели жизнеобеспечения нейрона
bool InstallLifeConnection(void);

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
// --------------------------
};

}
#endif
