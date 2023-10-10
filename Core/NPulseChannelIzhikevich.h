#ifndef NMSDK_NPulseChannelIzhikevichH
#define NMSDK_NPulseChannelIzhikevichH

#include "NPulseChannelClassic.h"

namespace NMSDK {

class NPulseChannelIzhikevich: public NPulseChannelClassic
{
public: // Свойства
ULProperty<double, NPulseChannelIzhikevich, ptPubParameter> A;

ULProperty<double, NPulseChannelIzhikevich, ptPubParameter> B;

/// The after-spike reset value of the membrane potential v
ULProperty<double, NPulseChannelIzhikevich, ptPubParameter> C;

/// The after-spike reset of the recovery variable u caused by slowhigh-threshold Na+ and K+ conductances.
ULProperty<double, NPulseChannelIzhikevich, ptPubParameter> D;

public: // Входы и выходы

///
UPropertyOutputData<MDMatrix<double>,NPulseChannelIzhikevich, ptOutput | ptPubState> OutputU;

protected: // Переменные состояния
ULProperty<double, NPulseChannelIzhikevich, ptPubState> V;
ULProperty<double, NPulseChannelIzhikevich, ptPubState> U;


public: // Методы
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



