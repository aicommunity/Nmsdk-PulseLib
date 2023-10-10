#ifndef INTEGRATED_FIRE_NEURON_H
#define INTEGRATED_FIRE_NEURON_H

#include "../../../Rdk/Deploy/Include/rdk.h"
#include "../../Nmsdk-PulseLib/Core/NPulseNeuronCommon.h"

namespace NMSDK
{
    class NIntegrateAndFireNeuron: public RDK::UNet
    {
        // Свойства
        protected:
        // Емкость конденсатора
		ULProperty<double, NIntegrateAndFireNeuron, ptPubParameter> C;

		//Начальное напряжение на мембране
		ULProperty<double, NIntegrateAndFireNeuron, ptPubParameter> U0;

		//Пороговое напряжение на мембране
        ULProperty<double, NIntegrateAndFireNeuron, ptPubParameter> Upr;

        protected:
        // Переменные состояния
        // Напряжение на конденсаторе
        UPropertyOutputData<MDMatrix<double>, NIntegrateAndFireNeuron, ptOutput | ptPubState> U;

        // Стимулирующий ток
        UPropertyInputData<MDMatrix<double>, NIntegrateAndFireNeuron, ptInput | ptPubState> I;

        // Методы
        public:
        // Конструктор
        NIntegrateAndFireNeuron(void);
        // Деструктор
        virtual ~NIntegrateAndFireNeuron(void);
        // Выделяет память для новой чистой копии объекта этого класса
        virtual NIntegrateAndFireNeuron* New(void);

        // Скрытые методы управления счетом
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

    };
}


#endif // NIntegrateAndFireNeuron

