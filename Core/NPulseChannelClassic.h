#ifndef NMSDK_NPulseChannelClassicH
#define NMSDK_NPulseChannelClassicH

//#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseChannelCommon.h"

namespace NMSDK {

class NPulseChannelClassic: public NPulseChannelCommon
{
public: // Параметры
/// Сопротивление сегмента (для вычисления входного тока по входному потенциалу)
ULProperty<double, NPulseChannelClassic, ptPubParameter> CompartmentR;

/// Сопротивление синапса (для вычисления синаптического тока по сопротивлению синапса)
ULProperty<double, NPulseChannelClassic, ptPubParameter> SynapticR;

protected: // Переменные состояния
ULProperty<double, NPulseChannelClassic, ptPubState> SynapticI;
ULProperty<double, NPulseChannelClassic, ptPubState> CompartmentI;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelClassic(void);
virtual ~NPulseChannelClassic(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseChannelClassic* New(void);
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
virtual bool ACalculate2(void);
// --------------------------
};


}

#endif



