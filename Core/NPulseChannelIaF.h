#ifndef NMSDK_NPulseChannelIaFH
#define NMSDK_NPulseChannelIaFH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIaF: public NPulseChannelClassic
{
public: // Свойства
/// Емкость мембраны
ULProperty<double, NPulseChannelIaF, ptPubParameter> Cm;

/// Потенциал покоя
ULProperty<double, NPulseChannelIaF, ptPubParameter> EL;

/// Постоянная времени мембраны
ULProperty<double, NPulseChannelIaF, ptPubParameter> TauM;

/// Период рефрактерности
ULProperty<double, NPulseChannelIaF, ptPubParameter> TRef;

/// Потенциал обновления
ULProperty<double, NPulseChannelIaF, ptPubParameter> VReset;

/// Возбуждающий постоянный ток
ULProperty<double, NPulseChannelIaF, ptPubParameter> Ie;

/// Абсолютный минимум потенциала мембраны
ULProperty<double, NPulseChannelIaF, ptPubParameter> VMin;

/// Флаг разрешения сброса потенциала при генерации спайка
ULProperty<bool, NPulseChannelIaF, ptPubParameter> VResetEnable;

public: // Входы и выходы

protected: // Переменные состояния
/// Переменные состояния
/// Потенциал клеточной мембраны
ULProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Vm;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelIaF(void);
virtual ~NPulseChannelIaF(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseChannelIaF* New(void);
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



