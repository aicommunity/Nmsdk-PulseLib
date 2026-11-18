#ifndef NMSDK_NPulseChannelIaFH
#define NMSDK_NPulseChannelIaFH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIaF: public NPulseChannelClassic
{
public: // 
/// Емкость мембраны
UProperty<double, NPulseChannelIaF, ptPubParameter> Cm;

/// Потенциал покоя
UProperty<double, NPulseChannelIaF, ptPubParameter> EL;

/// Постоянная времени мембраны
UProperty<double, NPulseChannelIaF, ptPubParameter> TauM;

/// Период рефрактерности
UProperty<double, NPulseChannelIaF, ptPubParameter> TRef;

/// Потенциал обновления
UProperty<double, NPulseChannelIaF, ptPubParameter> VReset;

/// Абсолютный минимум потенциала мембраны
UProperty<double, NPulseChannelIaF, ptPubParameter> VMin;

/// Флаг разрешения сброса потенциала при генерации спайка
UProperty<bool, NPulseChannelIaF, ptPubParameter> VResetEnable;

public: //   

protected: //  
/// Переменные состояния
/// Потенциал клеточной мембраны
UProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Vm;

/// Сопротивление мембраны
UProperty<MDMatrix<double>, NPulseChannelIaF, ptPubState> Rm;

public: // 
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



