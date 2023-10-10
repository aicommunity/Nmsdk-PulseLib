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

#ifndef NMSDK_NCableModelH
#define NMSDK_NCableModelH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelCable: public NPulseChannelClassic
{
public: // Свойства
/// Потенциал покоя, В
ULProperty<double, NPulseChannelCable, ptPubParameter> EL;

/// Удельное сопротивление цитоплазмы, Ом*м
ULProperty<double, NPulseChannelCable, ptPubParameter> Ri;

/// Сопротивление мембраны, Ом
ULProperty<double, NPulseChannelCable, ptPubParameter> CableMembraneResistance;

/// Диаметр дендрита, м
ULProperty<double, NPulseChannelCable, ptPubParameter> D;

/// Удельное сопротивление мембраны, Ом*м^2
ULProperty<double, NPulseChannelCable, ptPubParameter> Rm;

/// Емкость мембраны, Ф
ULProperty<double, NPulseChannelCable, ptPubParameter> Cm;

/// Длина моделируемого отрезка нервного волокна, м
ULProperty<double, NPulseChannelCable, ptPubParameter> ModelMaxLength;

/// Длина моделируемого отрезка времени, сек
ULProperty<double, NPulseChannelCable, ptPubState> ModelMaxTime;

/// Шаг моделирования по длине дендрита, м
ULProperty<double, NPulseChannelCable, ptPubParameter> dx;

/// Шаг моделирования по времени, с
ULProperty<double, NPulseChannelCable, ptPubState> dt;

/// Режим расчета:
/// 0/false - Вычислять D на основе CableMembraneResistance,
/// 1/true - Вычислять CableMembraneResistance на основе D
ULProperty<bool, NPulseChannelCable, ptPubParameter> CalcMode;



protected: // Переменные состояния
/// Постоянная времени мембраны
ULProperty<double, NPulseChannelCable, ptPubState> TauM;

///// Потенциал клеточной мембраны
ULProperty<MDMatrix<double>, NPulseChannelCable, ptPubState> Vm;

///// Входное воздействие
ULProperty<MDMatrix<double>, NPulseChannelCable, ptPubState> InpV;

/////
MDMatrix<double> x;

/////
MDMatrix<double> t;

/////Число точек сетки по оси длины дендрита
int x_points_number;

/////Число точек сетки по оси времени
int t_points_number;

/////Момент подачи импульса, номер точки по стеке
int inpT;

/////Координата подачи импульса, номер точки по стеке
int inpX;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelCable(void);
virtual ~NPulseChannelCable(void);
// --------------------------

// --------------------------
// Методы упраления параметрами
// --------------------------
///Потенциал покоя
bool SetEL(const double &value);

///
bool SetRi(const double &value);

///
bool SetCableMembraneResistance(const double &value);

///
bool SetD(const double &value);

//
bool SetRm(const double &value);

//
bool SetTauM(const double &value);

//
bool SetCm(const double &value);

//
bool SetModelMaxLength(const double &value);

//
bool SetModelMaxTime(const double &value);

//
bool Setdx(const double &value);

//
bool Setdt(const double &value);

//
bool SetCalcMode(const bool &value);
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseChannelCable* New(void);
// --------------------------

// --------------------------
// Скрытые методы управления счетом 
// --------------------------
protected:

void FormingInput();


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



