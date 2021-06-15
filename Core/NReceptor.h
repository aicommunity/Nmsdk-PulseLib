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

#ifndef NRECEPTOR_H
#define NRECEPTOR_H

#include "../../Nmsdk-BasicLib/Core/NSource.h"


namespace NMSDK {

class RDK_LIB_TYPE NReceptor: public NSource
{
public: // Общедоступные свойства
// Коэффициент усиления входного сигнала
RDK::ULProperty<double,NReceptor, ptPubParameter> Gain;

// Рабочий диапазон входного сигнала
RDK::ULProperty<double,NReceptor, ptPubParameter> MinInputRange, MaxInputRange;

// Рабочий диапазон выходного сигнала
RDK::ULProperty<double,NReceptor, ptPubParameter> MinOutputRange, MaxOutputRange;

// Коэффициент масштабирования для режима 2
RDK::ULProperty<double,NReceptor, ptPubParameter> ExpCoeff;

// Слагаемое для режима 1
RDK::ULProperty<double,NReceptor, ptPubParameter> SumCoeff;

// Постоянная времени забывания границ входного диапазона
RDK::ULProperty<double,NReceptor, ptPubParameter> InputAdaptationArrestingTC;


// Режим адаптации диапазона входного сигнала
// 0 - без адаптации
// 1 - постоянное расширение диапазона
// 2 - расширение с забыванием, с постояной времени InputAdaptationArrestingTC
RDK::ULProperty<int,NReceptor, ptPubParameter> InputAdaptationMode;

// Режим адаптации диапазона входного сигнала
// 0 - без адаптации
// 1 - экспоненциальное преобразование входного сигнала x
// в интервал [0;Gain] y=(SumCoeff-exp(-ExpCoeff*x))*Gain
// 2 - аналогично 1, но формула y=exp(-Kx)*Gain
// 3 - аналогично 1, ExpCoeff вычисляется автоматически
// 4 - адаптация по формуле y=Gain*(1.0+input);
RDK::ULProperty<int,NReceptor, ptPubParameter> OutputAdaptationMode;

protected: // Входы и выходы
/// Входной сигнал с нейрона
UPropertyInputData<MDMatrix<double>, NReceptor, ptInput | ptPubState> Input;

protected: // Временные переменные
double InputRange;
double OutputRange;



public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NReceptor(void);
virtual ~NReceptor(void);
// --------------------------

protected:
// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Коэффициент усиления входного сигнала
bool SetGain(const double &value);
// --------------------------

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NReceptor* New(void);
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

