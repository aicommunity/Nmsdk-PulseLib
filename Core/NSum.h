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

#ifndef NSUM_H
#define NSUM_H

#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NSum: public UNet
{
public: // Общедоступные свойства
/// Режим суммации
/// 0 - складывает соответствующие значения всех векторов входов,
/// формируя один вектор выходов.
/// 1 - складывает все значения всех входов в один скаляр
ULProperty<int,NSum> Mode;

/// Коэффициент усиления
ULProperty<MDMatrix<double> ,NSum> Gain;

public: // Входы и выходы
/// Входные массивы данных
UPropertyInputCData<MDMatrix<double>, NSum, ptInput | ptPubState> Inputs;

/// Выходной массив данных
UPropertyOutputData<MDMatrix<double>, NSum, ptOutput | ptPubState> Output;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSum(void);
virtual ~NSum(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Усиление
bool SetGain(const MDMatrix<double> &value);

// Режим суммации
bool SetMode(const int &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSum* New(void);
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
