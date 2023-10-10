#ifndef INTEGRATED_FIRE_NEURON_CPP
#define INTEGRATED_FIRE_NEURON_CPP

#include "NIntegrateAndFireNeuron.h"


namespace NMSDK
{
    //Конструкторы
    NIntegrateAndFireNeuron::NIntegrateAndFireNeuron(void)
     : C("C", this),
       U0("U0", this),
       Upr("Upr", this),
       U("U", this),
       I("I", this)

    {
    }

    // Деструкторы
    NIntegrateAndFireNeuron::~NIntegrateAndFireNeuron(void)
    {
    }

    // Выделяет память для новой чистой копии объекта этого класса
    NIntegrateAndFireNeuron* NIntegrateAndFireNeuron::New(void)
    {
        return new NIntegrateAndFireNeuron;
    }

    // Восстановление настроек по умолчанию и сброс процесса счета
    bool NIntegrateAndFireNeuron::ADefault(void)
    {
        C = 10e-10;
        Upr=-30e-3;
        U0=-60e-3;
        return true;
    }

    // Обеспечивает сборку внутренней структуры объекта
    // после настройки параметров
    // Автоматически вызывает метод Reset() и выставляет Ready в true
    // в случае успешной сборки
    bool NIntegrateAndFireNeuron::ABuild(void)
    {
     I->Assign(1,1,0.0);
     U->Assign(1,1,0.0);
        return true;
    }

    // Сброс процесса счета без потери настроек
    bool NIntegrateAndFireNeuron::AReset(void)
    {
        (*U)(0,0)=U0;
        (*I)(0,0)=0;
        return true;
    }

    // Выполняет расчет этого объекта
    bool NIntegrateAndFireNeuron::ACalculate(void)
    {
     MDMatrix<double>& refI=*I;
     MDMatrix<double>& refU=*U;
        //шаг
        double step=1./TimeStep;
        refU(0,0)=refI(0,0)*step/C+refU(0,0);
        if(refU(0,0)>Upr)
            refU(0,0)=U0;
        return true;
    }

}


#endif // NIntegrateAndFireNeuron
