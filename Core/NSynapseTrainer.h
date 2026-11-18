#ifndef NSYNAPSE_TRAINER_H
#define NSYNAPSE_TRAINER_H

#include "NNet.h"


namespace NMSDK {

class RDK_LIB_TYPE NSynapseTrainer: public RDK::UNet
{
public: //   
/// Входной сигнал внешней постсинаптической активности
UProperty<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PostSynInput;

/// Входной сигнал внешней пресинаптической активности
UProperty<MDMatrix<double>, NSynapseTrainer, ptInput | ptPubState> PreSynInput;

/// Выходной сигнал влияния STDP
UProperty<MDMatrix<double>, NSynapseTrainer, ptOutput | ptPubState> WeightOutput;

public: // 
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
