#ifndef NMSDK_NPulseChannelClassicH
#define NMSDK_NPulseChannelClassicH

//#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseChannelCommon.h"

namespace NMSDK {

class NPulseChannelClassic: public NPulseChannelCommon
{
public: // 
/// Коэффициент пересчета суммарного входного влияния каналов (в ток)
UProperty<double, NPulseChannelClassic, ptPubParameter> ChannelInputCoeff;

/// Коэффициент пересчета суммарного входного влияния синапсов (в синаптический ток)
UProperty<double, NPulseChannelClassic, ptPubParameter> SynapticInputCoeff;

public: // 
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



