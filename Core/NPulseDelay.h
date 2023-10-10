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

#ifndef NPULSE_DELAY_H
#define NPULSE_DELAY_H

#include "NSource.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseDelay: public NSource
{
public: // Параметры

/// Входной сигнал
UPropertyInputData<MDMatrix<double>,NPulseDelay, ptInput | ptPubState> Input;
/// Время задержки по модельному времени
ULProperty<double,NPulseDelay, ptPubParameter> DelayTime;


protected: // Вычисляемые переменные

//Буфер для хранения задержанного сигнала
std::list<MDMatrix<double> > buffer;

//Расчетная длина буфера для задержанного сигнала
int desired_buffer_length;

public: // Входы и выходы


public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseDelay(void);
virtual ~NPulseDelay(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------

// Устанавливает длительность импульса
bool SetDelayTime(const double &value);


public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseDelay* New(void);
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
virtual bool ACalculate(void);
// --------------------------
};
}
#endif

