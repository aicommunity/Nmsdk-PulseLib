#ifndef NMSDK_NSYNAPSECLASSIC_CPP
#define NMSDK_NSYNAPSECLASSIC_CPP

#include "NSynapseClassic.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseClassic::NSynapseClassic(void)
 : TauS("TauS",this),
   Current("Current",this)
{
}

NSynapseClassic::~NSynapseClassic(void)
{
}
// --------------------------    


// ---------------------
// Методы управления параметрами
// ---------------------
// ---------------------

// ---------------------
// Методы управления переменными состояния
// ---------------------
// ---------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NSynapseClassic* NSynapseClassic::New(void)
{
 return new NSynapseClassic;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом 
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NSynapseClassic::ADefault(void)
{
 if(!NPulseSynapseCommon::ADefault())
  return false;

 TauS=1e-3;
 Current=376e-12;
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NSynapseClassic::ABuild(void)
{
 if(!NPulseSynapseCommon::ABuild())
  return false;

 return true;
}

// Сброс процесса счета без потери настроек
bool NSynapseClassic::AReset(void)
{
 if(!NPulseSynapseCommon::AReset())
  return false;

 return true;
}

// Выполняет расчет этого объекта
bool NSynapseClassic::ACalculate2(void)
{
 return true;
}
// --------------------------

}
#endif



