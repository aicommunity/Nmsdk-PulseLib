#ifndef NMSDK_NCONVOLUTIONNET_CPP
#define NMSDK_NCONVOLUTIONNET_CPP

#include "NConvolutionNet.h"

namespace NMSDK {

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NConvolutionNet::NConvolutionNet(void)
{
}

NConvolutionNet::~NConvolutionNet(void)
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
NConvolutionNet* NConvolutionNet::New(void)
{
 return new NConvolutionNet;
}
// --------------------------


// --------------------------
// Скрытые методы управления счетом 
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NConvolutionNet::ADefault(void)
{           
 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NConvolutionNet::ABuild(void)
{
 return true;
}

// Сброс процесса счета без потери настроек
bool NConvolutionNet::AReset(void)
{
 return true;
}

// Выполняет расчет этого объекта
bool NConvolutionNet::ACalculate(void)
{
 return true;
}
// --------------------------

}
#endif



