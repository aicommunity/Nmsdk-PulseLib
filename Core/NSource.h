// ===========================================================
// Version:        3.0.3
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

#ifndef NSOURCE_H
#define NSOURCE_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NSource: public UNet
{
public: // Общедоступные свойства
/// Длительность работы с момента включения (с)
/// 0 - постоянная работа
ULProperty<UTime,NSource, ptPubParameter> ActionPeriod;

public: // Входы и выходы
/// Выход источника сигнала
UPropertyOutputData<MDMatrix<double>, NSource, ptOutput | ptPubState> Output;

protected: // Временные переменные
ULProperty<UTime,NSource, ptPubState> ActionCounter;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSource(void);
virtual ~NSource(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Длительность работы с момента включения
bool SetActionPeriod(const RDK::UTime &value);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSource* New(void);
// --------------------------


// --------------------------
// Скрытые методы управления счетом
// --------------------------
protected:
// Восстановление настроек по умолчанию и сброс процесса счета
virtual bool ADefault(void);

// Сброс процесса счета.
virtual bool AReset(void);
// --------------------------
};

}
#endif



