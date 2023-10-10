#ifndef NMSDK_NPulseNeuronIzhikevichCPP
#define NMSDK_NPulseNeuronIzhikevichCPP

#include "NPulseNeuronIzhikevich.h"
#include "NPulseChannelIzhikevich.h"
#include "NPulseMembraneIzhikevich.h"
#include "NPulseLTZoneIzhikevich.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseNeuronIzhikevich::NPulseNeuronIzhikevich(void)
{
}

NPulseNeuronIzhikevich::~NPulseNeuronIzhikevich(void)
{
}
// --------------------------    

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseNeuronIzhikevich* NPulseNeuronIzhikevich::New(void)
{
 return new NPulseNeuronIzhikevich;
}
// --------------------------

// --------------------------
// Методы управления счетом
// --------------------------
/// Создает структуру простого нейрона
void NPulseNeuronIzhikevich::CreateSimpleNeuron(void)
{
 AddMissingComponent<NPulseMembraneIzhikevich>("PulseMembrane", "NPulseMembraneIzhikevich");
 AddMissingComponent<NPulseLTZoneIzhikevich>("LTZone", "NPulseLTZoneIzhikevich");

 bool res=CreateLink("LTZone","Output","PulseMembrane","InputFeedbackSignal");
 res&=CreateLink("PulseMembrane.PosChannel","Output","LTZone","InputChannels");
 res&=CreateLink("PulseMembrane.PosChannel","Output","LTZone","InputChannels");
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseNeuronIzhikevich::ADefault(void)
{
 if(!NPulseNeuronCommon::ADefault())
  return false;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseNeuronIzhikevich::ABuild(void)
{
 if(!NPulseNeuronCommon::ABuild())
  return false;

 CreateSimpleNeuron();
 return true;
}

// Сброс процесса счета без потери настроек
bool NPulseNeuronIzhikevich::AReset(void)
{
 if(!NPulseNeuronCommon::AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool NPulseNeuronIzhikevich::ACalculate(void)
{
 if(!NPulseNeuronCommon::ACalculate())
  return false;

 return true;
}
// --------------------------

}
#endif



