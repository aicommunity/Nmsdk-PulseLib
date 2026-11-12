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

#ifndef NMUSCLE_H
#define NMUSCLE_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "NReceptor.h"

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NMuscle: public UNet
{
public: //  
/// 
UProperty<vector<double>,NMuscle, ptPubParameter> MulCoeffs;

UProperty<vector<double>,NMuscle, ptPubParameter> Param;

///  
UProperty<vector<double>,NMuscle, ptPubParameter> TC;

/// Масса
UProperty<double,NMuscle, ptPubParameter> Mass;

/// Порог
UProperty<double,NMuscle, ptPubParameter> Threshold;

/// Величина ускорения свободного падения
UProperty<double,NMuscle, ptPubParameter> G;

public: //   
///   ( )
UProperty<MDMatrix<double>, NMuscle, ptPubInput> Input;

/// Length input
UProperty<MDMatrix<double>, NMuscle, ptPubInput> LengthInput;

UProperty<MDMatrix<double>, NMuscle, ptOutput | ptPubState> Output;

public: //  
double y[15], yOld[15], mr[7], mrOld[7],vereteno1, vereteno2, goldgi;
double MaxActionPower;
bool FirstStep;


/*
// Указатель на вход по внешней силе
NReceptor *ExternalForce;

// Указатели на дополнительные выходы
// Активная сила
NItem *ActivePower;

// Длина
NItem *Length;

// Веретена
NItem *Spindle1, *Spindle2;

// Сухожильный орган Гольджи
NItem *Tendon;
             */
public: // 
// --------------------------
// --------------------------
// Конструкторы и деструкторы
// --------------------------
// --------------------------
NMuscle(void);
virtual ~NMuscle(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
//   
//bool SetAmplitude(double value);

//   
//bool SetFrequency(double value);
// --------------------------

public:
// --------------------------
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
// --------------------------
virtual NMuscle* New(void);
// --------------------------

// --------------------------
// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
// --------------------------
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// --------------------------
// Дополнительные скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// --------------------------
// --------------------------
// Дополнительные скрытые методы управления счетом
// --------------------------
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

// --------------------------
// --------------------------
// Дополнительные скрытые методы управления счетом
// --------------------------
// --------------------------
protected:
// Апериодическое звено
double AperiodicSection(double in, double inOld, double param1, double param2);

// Порог
double CompareThreshold(double);

// мускульное сокращение
double MuscularReduction(double);

// Инерционный элемент ОС мышцы
double MuscleFeedback(double x_new, double x_old, double y_old,
					  double k, double T1, double T2);
// --------------------------
};

}
#endif

