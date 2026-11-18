#ifndef NMSDK_NPulseChannelIzhikevichH
#define NMSDK_NPulseChannelIzhikevichH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIzhikevich: public NPulseChannelClassic
{
public: // 
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> A;

UProperty<double, NPulseChannelIzhikevich, ptPubParameter> B;

/// The after-spike reset value of the membrane potential v
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> C;

/// The after-spike reset of the recovery variable u caused by slowhigh-threshold Na+ and K+ conductances.
UProperty<double, NPulseChannelIzhikevich, ptPubParameter> D;

public: //   

///
UProperty<MDMatrix<double>,NPulseChannelIzhikevich, ptOutput | ptPubState> OutputU;

protected: //  
UProperty<double, NPulseChannelIzhikevich, ptPubState> V;
UProperty<double, NPulseChannelIzhikevich, ptPubState> U;


public: // 
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseChannelIzhikevich(void);
virtual ~NPulseChannelIzhikevich(void);
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NPulseChannelIzhikevich* New(void);
// --------------------------        

// --------------------------
// Скрытые методы управления счетом
// --------------------------
public:
/// Принимает сигнал о генерации сигнала низкопороговой зоной
/// (однократно по началу импульса)
virtual void NeuronActivated(void);

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



