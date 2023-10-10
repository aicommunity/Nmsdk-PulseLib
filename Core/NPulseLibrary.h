/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:            alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: NMSDK

File License:        BSD License
Project License:    GNU GPLv3 License
See file license.txt for more information
*********************************************************** */

#ifndef NPulseLibraryH
#define NPulseLibraryH

#include "NNet.h"
#include "NModel.h"
#include "NNeuron.h"
#include "NReceiver.h"
#include "NSource.h"

#include "NConstGenerator.h"
#include "NPulseGenerator.h"
#include "NPattern.h"
#include "NSinusGenerator.h"
#include "NFileGenerator.h"
#include "NPulseGeneratorDelay.h"
#include "NPulseGeneratorMulti.h"
#include "NPulseDelay.h"


#include "NMExtrapolator.h"
#include "NSum.h"

//#include "NPulseLifeNeuron.h"
//#include "NNeuronLife.h"
//#include "NLifeNet.h"
//#include "NPulseHebbLifeSynapse.h"

#include "NAfferentNeuron.h"
#include "NEyeMuscle.h"
#include "NMuscle.h"
#include "NPulseChannel.h"
#include "NPulseLTZone.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "NPulseSynapse.h"
#include "NPulseHebbSynapse.h"
#include "NPulseSynChannel.h"
#include "NPac.h"
#include "NReceptor.h"
#include "NPulseSynapseStdp.h"

#include "NPulseChannelIzhikevich.h"
#include "NPulseChannelIaF.h"
#include "NSynapseStdp.h"
#include "NIntegrateAndFireNeuron.h"
#include "NSynapseClassic.h"
#include "NSynapseClassicSlv.h"
#include "NSynapseTrainerStdp.h"
#include "NPulseChannelCable.h"

#include "NNeuronFreqGroup.h"
#include "NNeuronFreqGroupLayer.h"
#include "NNeuronLearner.h"
#include "NNeuronsLayer.h"
#include "NPulsePerseptron.h"
#include "NNeuronTrainer.h"
#include "NSpikeClassifier.h"
#include "NClassifier.h"
#include "NConditionedReflex.h"
#include "NAssociationFormer.h"
#include "NLogicalNot.h"
#include "NPredictor.h"
#include "NDataset.h"
#include "NStatePredictor.h"
#include "NPCAClassifier.h"
#include "NPainReflexSimple.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseLibrary: public ULibrary
{
public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseLibrary(void);
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
virtual void CreateClassSamples(UStorage *storage);
// --------------------------

// --------------------------
// Методы создания составных моделей
// --------------------------
// Метод формирования модели простого импульсного нейрона
UEPtr<NPulseNeuron> CreateNewSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzonemembraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length=1);

// Метод формирования модели простого импульсного нейрона с произвольной дендритной структурой
UEPtr<NPulseNeuron> CreateCustomNewSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzonemembraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, vector<int> &dendrite_length);

// Метод формирования модели простого импульсного нейрона
UEPtr<NPulseNeuron> CreateSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length=1);

// Метод формирования модели простого импульсного нейрона с произвольной дендритной структурой
UEPtr<NPulseNeuron> CreateCustomSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, std::vector<int> dendrite_length);

// Метод формирования модели простого импульсного нейрона с синапсами хебба
UEPtr<NPulseNeuron> CreateSimplePulseHebbNeuron(UStorage *storage, const string &neuronclass, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length=1);

// Метод формирования модели афферентного нейрона
UEPtr<NAfferentNeuron> CreateAfferentNeuron(UStorage *storage, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes);

// Метод формирования модели простого афферентного нейрона
UEPtr<NAfferentNeuron> CreateSimpleAfferentNeuron(UStorage *storage, const string &ltzone_class, double max_output);
// --------------------------
};                  

extern RDK_LIB_TYPE NPulseLibrary PulseLibrary;

}


#endif
