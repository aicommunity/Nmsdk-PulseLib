#ifndef NSYNAPSE_TRAINER_H
#define NSYNAPSE_TRAINER_H

#include "../../Nmsdk-BasicLib/Core/NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NSynapseTrainer: public RDK::UNet
{
public: // Входы и выходы
/// Входной сигнал внешней постсинаптической активности
UPropertyInputData<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PostSynInput;

/// Входной сигнал внешней пресинаптической активности
UPropertyInputData<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PreSynInput;

/// Выходной сигнал влияния STDP
UPropertyOutputData<MDMatrix<double>, NSynapseTrainer, ptOutput | ptPubState> WeightOutput;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NSynapseTrainer(void);
virtual ~NSynapseTrainer(void);

public:
// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
virtual NSynapseTrainer* New(void);

public:
// Восстанавливает требуемые связи с тренером
virtual void RebuildInternalLinks(void);

};

}

#endif
