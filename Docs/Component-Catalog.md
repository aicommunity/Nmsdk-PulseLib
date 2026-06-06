# Nmsdk-PulseLib — Component Catalog

## RU

Полный список регистраций (`Core/NPulseLibrary.cpp`) очень велик. Ниже — группировка с кратким назначением и типами сигналов. Для полного набора см. исходник `NPulseLibrary.cpp`.

## Базовые сети/структуры
- **NNet** — см. [`NNet`](Components/NNet.md)  
  *Использование:* Корневой компонент в некоторых конфигурационных проектах
- **NModel** — см. [`NModel`](Components/NModel.md)  
  *Использование:* Корневой компонент в большинстве проектов (`Bin/Configs/*/Model_*.xml`)
- **NLifeNet** — см. [`NLifeNet`](Components/NLifeNet.md)
- **NNeuronsLayer** — см. [`NNeuronsLayer`](Components/NNeuronsLayer.md)  
  *Использование:* Создание слоев нейронов для классификации (`Bin/Configs/!OldConfigs/SpikeClassifier/`)
- **NNeuronsLayerIaF** — см. [`NNeuronsLayerIaF`](Components/NNeuronsLayerIaF.md)
- **NNeuronFreqGroup** — см. [`NNeuronFreqGroup`](Components/NNeuronFreqGroup.md)
- **NNeuronFreqGroupLayer** — см. [`NNeuronFreqGroupLayer`](Components/NNeuronFreqGroupLayer.md)
- **NNeuronLife** — см. [`NNeuronLife`](Components/NNeuronLife.md)

## Нейроны
- Обзор: [`Neurons`](Components/Neurons.md)  
  *Типичные конфигурации:* `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`
- Детальные:
  - [`NPulseNeuronIzhikevich`](Components/NPulseNeuronIzhikevich.md) — *Часто используется в:* `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/!OldConfigs/STDP-Simple-01/`
  - [`NIntegrateAndFireNeuron`](Components/NIntegrateAndFireNeuron.md) — *Используется в:* экспериментах с IaF-моделью
  - [`NSPNeuronGen`](Components/NSPNeuronGen.md) — *Используется в когнитивной навигации:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NLPNeuron`](Components/NLPNeuron.md)
  - [`NNeuron`](Components/NNeuron.md)
  - [`NAfferentNeuron`](Components/NAfferentNeuron.md) — *Используется в:* `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`, `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControlSAfferent/`

## Синапсы / Тренеры / Каналы / Мембраны / LT-зоны
- Обзор:
  - [`Synapses & Trainers`](Components/SynapsesAndTrainers.md) — *Типичные конфигурации:* `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`
  - [`Channels, Membranes & LT zones`](Components/ChannelsMembranesLT.md) — *Типичные конфигурации:* `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`
- Детальные:
  - [`NSynapseStdp`](Components/NSynapseStdp.md) — *Часто используется в:* `Bin/Configs/!OldConfigs/STDP-Simple-01/`
  - [`NPSynapseBio`](Components/NPSynapseBio.md) — *Используется в когнитивной навигации:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NSynapseTrainerStdp`](Components/NSynapseTrainerStdp.md) — *Используется в:* экспериментах по обучению
  - [`NPExcChannelBio`](Components/NPExcChannelBio.md), [`NPInhChannelBio`](Components/NPInhChannelBio.md) — *Используются в:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NPMembraneBio`](Components/NPMembraneBio.md) — *Используется в:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NPulseLTZoneThreshold`](Components/NPulseLTZoneThreshold.md) — *Часто используется в:* `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NPulseChannelIaF`](Components/NPulseChannelIaF.md)
  - [`NPulseMembraneIzhikevich`](Components/NPulseMembraneIzhikevich.md)
  - [`NPulseLTZoneIzhikevich`](Components/NPulseLTZoneIzhikevich.md)
  - [`NCLTZone`](Components/NCLTZone.md)

## Генераторы / Источники / Задержки и IO
- Обзор: [`Generators, IO, Classifiers & Others`](Components/GeneratorsIOClassifiers.md)  
  *Типичные конфигурации:* `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/User/CognitiveNavigation/`
- Детальные:
  - [`NPulseGenerator`](Components/NPulseGenerator.md) — *Используется в:* `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NPulseGeneratorTransit`](Components/NPulseGeneratorTransit.md) — *Используется в когнитивной навигации:* `Bin/Configs/User/CognitiveNavigation/` (входные сигналы Forward, Back, Left, Right)
  - [`NPGenerator`](Components/NPGenerator.md) — *Используется в:* `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NCGenerator`](Components/NCGenerator.md)
  - [`NPDelay`](Components/NPDelay.md)
  - [`NSource`](Components/NSource.md)
  - [`NReceptor`](Components/NReceptor.md) — *Используется в:* `Bin/Configs/!OldConfigs/NReceptor/`

## Классификаторы / Рефлексы / Предсказатели / Эффекторы / Логика / Решатель
- Классификаторы:
  - [`NClassifier`](Components/NClassifier.md)
  - [`NSpikeClassifier`](Components/NSpikeClassifier.md) — *Используется в:* `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`
  - [`NPCAClassifier`](Components/NPCAClassifier.md) — *Используется в:* `Bin/Configs/!OldConfigs/PCATest/`
- Рефлексы/ассоциации:
  - [`NConditionedReflex`](Components/NConditionedReflex.md) — *Используется в:* `Bin/Configs/!OldConfigs/SpikeConditionalReflex/`
  - [`NAssociationFormer`](Components/NAssociationFormer.md) — *Используется в:* `Bin/Configs/!OldConfigs/SpikeAssociationPlus/`
- Эффекторы/логика/решатель:
  - [`NMuscle`](Components/NMuscle.md) — *Используется в:* `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`
  - [`NEyeMuscle`](Components/NEyeMuscle.md) — *Используется в:* `Bin/Configs/!OldConfigs/EyeRetina/`
  - [`NMotoneuron`](Components/NMotoneuron.md) — *Используется в:* `Bin/Configs/!OldConfigs/OldExperiments/Motoneuron/`
  - [`NLogicalNot`](Components/NLogicalNot.md)
  - [`NCPac`](Components/NCPac.md)
  - [`NOdeSolver`](Components/NOdeSolver.md)

## Полный список компонентов (UploadClass)
Ниже — полный список имён регистрации из `Core/NPulseLibrary.cpp` со ссылками на per-component файлы в `Docs/Components/`.

- **NAfferentNeuron** — [`NAfferentNeuron`](Components/NAfferentNeuron.md)
- **NAssociationFormer** — [`NAssociationFormer`](Components/NAssociationFormer.md)
- **NCGenerator** — [`NCGenerator`](Components/NCGenerator.md)
- **NCLTZone** — [`NCLTZone`](Components/NCLTZone.md)
- **NCNeuron** — [`NCNeuron`](Components/NCNeuron.md)
- **NCNeuronNegCGenerator** — [`NCNeuronNegCGenerator`](Components/NCNeuronNegCGenerator.md)
- **NCNeuronPosCGenerator** — [`NCNeuronPosCGenerator`](Components/NCNeuronPosCGenerator.md)
- **NCPac** — [`NCPac`](Components/NCPac.md)
- **NCSimpleLTZone** — [`NCSimpleLTZone`](Components/NCSimpleLTZone.md)
- **NCSynChannel** — [`NCSynChannel`](Components/NCSynChannel.md)
- **NCSynExcChannel** — [`NCSynExcChannel`](Components/NCSynExcChannel.md)
- **NCSynInhChannel** — [`NCSynInhChannel`](Components/NCSynInhChannel.md)
- **NCSynNeuronMembrane** — [`NCSynNeuronMembrane`](Components/NCSynNeuronMembrane.md)
- **NClassifier** — [`NClassifier`](Components/NClassifier.md)
- **NConditionedReflex** — [`NConditionedReflex`](Components/NConditionedReflex.md)
- **NContinuesSAfferentNeuron** — [`NContinuesSAfferentNeuron`](Components/NContinuesSAfferentNeuron.md)
- **NContinuesSimpleAfferentNeuron** — [`NContinuesSimpleAfferentNeuron`](Components/NContinuesSimpleAfferentNeuron.md)
- **NContinuesSynLPNeuron** — [`NContinuesSynLPNeuron`](Components/NContinuesSynLPNeuron.md)
- **NContinuesSynMotoneuron** — [`NContinuesSynMotoneuron`](Components/NContinuesSynMotoneuron.md)
- **NContinuesSynRenshowCell** — [`NContinuesSynRenshowCell`](Components/NContinuesSynRenshowCell.md)
- **NContinuesSynSPNeuron** — [`NContinuesSynSPNeuron`](Components/NContinuesSynSPNeuron.md)
- **NDataset** — [`NDataset`](Components/NDataset.md)
- **NEyeMuscle** — [`NEyeMuscle`](Components/NEyeMuscle.md)
- **NFileGenerator** — [`NFileGenerator`](Components/NFileGenerator.md)
- **NGenBioNeuron** — [`NGenBioNeuron`](Components/NGenBioNeuron.md)
- **NIntegrateAndFireNeuron** — [`NIntegrateAndFireNeuron`](Components/NIntegrateAndFireNeuron.md)
- **NLPHebbNeuron** — [`NLPHebbNeuron`](Components/NLPHebbNeuron.md)
- **NLPLifeHebbNeuron** — [`NLPLifeHebbNeuron`](Components/NLPLifeHebbNeuron.md)
- **NLPLifeNeuron** — [`NLPLifeNeuron`](Components/NLPLifeNeuron.md)
- **NLPNeuron** — [`NLPNeuron`](Components/NLPNeuron.md)
- **NLifeNet** — [`NLifeNet`](Components/NLifeNet.md)
- **NLogicalNot** — [`NLogicalNot`](Components/NLogicalNot.md)
- **NMExtrapolator** — [`NMExtrapolator`](Components/NMExtrapolator.md)
- **NModel** — [`NModel`](Components/NModel.md)
- **NMotoneuron** — [`NMotoneuron`](Components/NMotoneuron.md)
- **NMuscle** — [`NMuscle`](Components/NMuscle.md)
- **NNet** — [`NNet`](Components/NNet.md)
- **NNeuron** — [`NNeuron`](Components/NNeuron.md)
- **NNeuronFreqGroup** — [`NNeuronFreqGroup`](Components/NNeuronFreqGroup.md)
- **NNeuronFreqGroupLayer** — [`NNeuronFreqGroupLayer`](Components/NNeuronFreqGroupLayer.md)
- **NNeuronLearner** — [`NNeuronLearner`](Components/NNeuronLearner.md)
- **NNeuronLife** — [`NNeuronLife`](Components/NNeuronLife.md)
- **NNeuronTrainer** — [`NNeuronTrainer`](Components/NNeuronTrainer.md)
- **NNeuronsLayer** — [`NNeuronsLayer`](Components/NNeuronsLayer.md)
- **NNeuronsLayerIaF** — [`NNeuronsLayerIaF`](Components/NNeuronsLayerIaF.md)
- **NNewLPHebbNeuron** — [`NNewLPHebbNeuron`](Components/NNewLPHebbNeuron.md)
- **NNewLPNeuron** — [`NNewLPNeuron`](Components/NNewLPNeuron.md)
- **NNewMotoneuron** — [`NNewMotoneuron`](Components/NNewMotoneuron.md)
- **NNewRenshowCell** — [`NNewRenshowCell`](Components/NNewRenshowCell.md)
- **NNewSPHebbNeuron** — [`NNewSPHebbNeuron`](Components/NNewSPHebbNeuron.md)
- **NNewSPLifeNeuron** — [`NNewSPLifeNeuron`](Components/NNewSPLifeNeuron.md)
- **NNewSPNeuron** — [`NNewSPNeuron`](Components/NNewSPNeuron.md)
- **NNewSynLPNeuron** — [`NNewSynLPNeuron`](Components/NNewSynLPNeuron.md)
- **NNewSynMotoneuron** — [`NNewSynMotoneuron`](Components/NNewSynMotoneuron.md)
- **NNewSynRenshowCell** — [`NNewSynRenshowCell`](Components/NNewSynRenshowCell.md)
- **NNewSynSPNeuron** — [`NNewSynSPNeuron`](Components/NNewSynSPNeuron.md)
- **NNewSynTCNNeuron** — [`NNewSynTCNNeuron`](Components/NNewSynTCNNeuron.md)
- **NOdeSolver** — [`NOdeSolver`](Components/NOdeSolver.md)
- **NPCAClassifier** — [`NPCAClassifier`](Components/NPCAClassifier.md)
- **NPChannel** — [`NPChannel`](Components/NPChannel.md)
- **NPDelay** — [`NPDelay`](Components/NPDelay.md)
- **NPExcChannel** — [`NPExcChannel`](Components/NPExcChannel.md)
- **NPExcChannelBio** — [`NPExcChannelBio`](Components/NPExcChannelBio.md)
- **NPExcChannelBio2** — [`NPExcChannelBio2`](Components/NPExcChannelBio2.md)
- **NPGenerator** — [`NPGenerator`](Components/NPGenerator.md)
- **NPGeneratorDelay** — [`NPGeneratorDelay`](Components/NPGeneratorDelay.md)
- **NPHebbLifeSynapse** — [`NPHebbLifeSynapse`](Components/NPHebbLifeSynapse.md)
- **NPHebbNeuron** — [`NPHebbNeuron`](Components/NPHebbNeuron.md)
- **NPHebbSynapse** — [`NPHebbSynapse`](Components/NPHebbSynapse.md)
- **NPInhChannel** — [`NPInhChannel`](Components/NPInhChannel.md)
- **NPInhChannelBio** — [`NPInhChannelBio`](Components/NPInhChannelBio.md)
- **NPInhChannelBio2** — [`NPInhChannelBio2`](Components/NPInhChannelBio2.md)
- **NPLTChannel** — [`NPLTChannel`](Components/NPLTChannel.md)
- **NPLTExcChannel** — [`NPLTExcChannel`](Components/NPLTExcChannel.md)
- **NPLTInhChannel** — [`NPLTInhChannel`](Components/NPLTInhChannel.md)
- **NPLTSynChannel** — [`NPLTSynChannel`](Components/NPLTSynChannel.md)
- **NPLTSynExcChannel** — [`NPLTSynExcChannel`](Components/NPLTSynExcChannel.md)
- **NPLTSynInhChannel** — [`NPLTSynInhChannel`](Components/NPLTSynInhChannel.md)
- **NPLTZone** — [`NPLTZone`](Components/NPLTZone.md)
- **NPLTZoneNeuronMembrane** — [`NPLTZoneNeuronMembrane`](Components/NPLTZoneNeuronMembrane.md)
- **NPLTZoneSynNeuronMembrane** — [`NPLTZoneSynNeuronMembrane`](Components/NPLTZoneSynNeuronMembrane.md)
- **NPLifeNeuron** — [`NPLifeNeuron`](Components/NPLifeNeuron.md)
- **NPMembrane** — [`NPMembrane`](Components/NPMembrane.md)
- **NPMembraneBio** — [`NPMembraneBio`](Components/NPMembraneBio.md)
- **NPMembraneBio2** — [`NPMembraneBio2`](Components/NPMembraneBio2.md)
- **NPNeuron** — [`NPNeuron`](Components/NPNeuron.md)
- **NPNeuron1x4** — [`NPNeuron1x4`](Components/NPNeuron1x4.md)
- **NPNeuron4x1** — [`NPNeuron4x1`](Components/NPNeuron4x1.md)
- **NPNeuron4x4** — [`NPNeuron4x4`](Components/NPNeuron4x4.md)
- **NPNeuronHebbLifeMembrane** — [`NPNeuronHebbLifeMembrane`](Components/NPNeuronHebbLifeMembrane.md)
- **NPNeuronHebbMembrane** — [`NPNeuronHebbMembrane`](Components/NPNeuronHebbMembrane.md)
- **NPNeuronNegCGenerator** — [`NPNeuronNegCGenerator`](Components/NPNeuronNegCGenerator.md)
- **NPNeuronNegCGeneratorBio** — [`NPNeuronNegCGeneratorBio`](Components/NPNeuronNegCGeneratorBio.md)
- **NPNeuronPosCGenerator** — [`NPNeuronPosCGenerator`](Components/NPNeuronPosCGenerator.md)
- **NPNeuronPosCGeneratorBio** — [`NPNeuronPosCGeneratorBio`](Components/NPNeuronPosCGeneratorBio.md)
- **NPNeuronPosCGeneratorCable** — [`NPNeuronPosCGeneratorCable`](Components/NPNeuronPosCGeneratorCable.md)
- **NPSimpleLTZone** — [`NPSimpleLTZone`](Components/NPSimpleLTZone.md)
- **NPSynChannel** — [`NPSynChannel`](Components/NPSynChannel.md)
- **NPSynExcChannel** — [`NPSynExcChannel`](Components/NPSynExcChannel.md)
- **NPSynInhChannel** — [`NPSynInhChannel`](Components/NPSynInhChannel.md)
- **NPSynNeuronMembrane** — [`NPSynNeuronMembrane`](Components/NPSynNeuronMembrane.md)
- **NPSynapse** — [`NPSynapse`](Components/NPSynapse.md)
- **NPSynapseBio** — [`NPSynapseBio`](Components/NPSynapseBio.md)
- **NPSynapseBio2** — [`NPSynapseBio2`](Components/NPSynapseBio2.md)
- **NPSynapseStdp** — [`NPSynapseStdp`](Components/NPSynapseStdp.md)
- **NPac** — [`NPac`](Components/NPac.md)
- **NPainReflexSimple** — [`NPainReflexSimple`](Components/NPainReflexSimple.md)
- **NPattern** — [`NPattern`](Components/NPattern.md)
- **NPredictor** — [`NPredictor`](Components/NPredictor.md)
- **NPulseChannelCable** — [`NPulseChannelCable`](Components/NPulseChannelCable.md)
- **NPulseChannelCableMulti** — [`NPulseChannelCableMulti`](Components/NPulseChannelCableMulti.md)
- **NPulseChannelIaF** — [`NPulseChannelIaF`](Components/NPulseChannelIaF.md)
- **NPulseChannelIzhikevich** — [`NPulseChannelIzhikevich`](Components/NPulseChannelIzhikevich.md)
- **NPulseGeneratorDelay** — [`NPulseGeneratorDelay`](Components/NPulseGeneratorDelay.md)
- **NPulseGeneratorMulti** — [`NPulseGeneratorMulti`](Components/NPulseGeneratorMulti.md)
- **NPulseGeneratorTransit** — [`NPulseGeneratorTransit`](Components/NPulseGeneratorTransit.md)
- **NPulseGenerator** — [`NPulseGenerator`](Components/NPulseGenerator.md)
- **NPulseLTZoneCable** — [`NPulseLTZoneCable`](Components/NPulseLTZoneCable.md)
- **NPulseLTZoneIaF** — [`NPulseLTZoneIaF`](Components/NPulseLTZoneIaF.md)
- **NPulseLTZoneIzhikevich** — [`NPulseLTZoneIzhikevich`](Components/NPulseLTZoneIzhikevich.md)
- **NPulseLTZoneThreshold** — [`NPulseLTZoneThreshold`](Components/NPulseLTZoneThreshold.md)
- **NPulseLTZoneThresholdBio** — [`NPulseLTZoneThresholdBio`](Components/NPulseLTZoneThresholdBio.md)
- **NPulseLTZoneThresholdBio2** — [`NPulseLTZoneThresholdBio2`](Components/NPulseLTZoneThresholdBio2.md)
- **NPulseMembraneCable** — [`NPulseMembraneCable`](Components/NPulseMembraneCable.md)
- **NPulseMembraneCableMulti** — [`NPulseMembraneCableMulti`](Components/NPulseMembraneCableMulti.md)
- **NPulseMembraneIaF** — [`NPulseMembraneIaF`](Components/NPulseMembraneIaF.md)
- **NPulseMembraneIaFStdp** — [`NPulseMembraneIaFStdp`](Components/NPulseMembraneIaFStdp.md)
- **NPulseMembraneIzhikevich** — [`NPulseMembraneIzhikevich`](Components/NPulseMembraneIzhikevich.md)
- **NPulseNeuronCable** — [`NPulseNeuronCable`](Components/NPulseNeuronCable.md)
- **NPulseNeuronCableMulti** — [`NPulseNeuronCableMulti`](Components/NPulseNeuronCableMulti.md)
- **NPulseNeuronIaF** — [`NPulseNeuronIaF`](Components/NPulseNeuronIaF.md)
- **NPulseNeuronIaFStdp** — [`NPulseNeuronIaFStdp`](Components/NPulseNeuronIaFStdp.md)
- **NPulseNeuronIzhikevich** — [`NPulseNeuronIzhikevich`](Components/NPulseNeuronIzhikevich.md)
- **NPulsePerseptron** — [`NPulsePerseptron`](Components/NPulsePerseptron.md)
- **NPulsePerseptronIaF** — [`NPulsePerseptronIaF`](Components/NPulsePerseptronIaF.md)
- **NPulseSynapseStdp** — [`NPulseSynapseStdp`](Components/NPulseSynapseStdp.md)
- **NReceiver** — [`NReceiver`](Components/NReceiver.md)
- **NReceptor** — [`NReceptor`](Components/NReceptor.md)
- **NRenshowCell** — [`NRenshowCell`](Components/NRenshowCell.md)
- **NSAfferentNeuron** — [`NSAfferentNeuron`](Components/NSAfferentNeuron.md)
- **NSPDendriteLifeHebbNeuron** — [`NSPDendriteLifeHebbNeuron`](Components/NSPDendriteLifeHebbNeuron.md)
- **NSPHebbNeuron** — [`NSPHebbNeuron`](Components/NSPHebbNeuron.md)
- **NSPLifeHebbNeuron** — [`NSPLifeHebbNeuron`](Components/NSPLifeHebbNeuron.md)
- **NSPLifeNeuron** — [`NSPLifeNeuron`](Components/NSPLifeNeuron.md)
- **NSPMinNeuron** — [`NSPMinNeuron`](Components/NSPMinNeuron.md)
- **NSPNeuron** — [`NSPNeuron`](Components/NSPNeuron.md)
- **NSPNeuronBio** — [`NSPNeuronBio`](Components/NSPNeuronBio.md)
- **NSPNeuronBio2** — [`NSPNeuronBio2`](Components/NSPNeuronBio2.md)
- **NSPNeuronGen** — [`NSPNeuronGen`](Components/NSPNeuronGen.md)
- **NSimpleAfferentNeuron** — [`NSimpleAfferentNeuron`](Components/NSimpleAfferentNeuron.md)
- **NSinusGenerator** — [`NSinusGenerator`](Components/NSinusGenerator.md)
- **NSource** — [`NSource`](Components/NSource.md)
- **NSpikeClassifier** — [`NSpikeClassifier`](Components/NSpikeClassifier.md)
- **NStatePredictor** — [`NStatePredictor`](Components/NStatePredictor.md)
- **NSum** — [`NSum`](Components/NSum.md)
- **NSynLPNeuron** — [`NSynLPNeuron`](Components/NSynLPNeuron.md)
- **NSynMotoneuron** — [`NSynMotoneuron`](Components/NSynMotoneuron.md)
- **NSynRenshowCell** — [`NSynRenshowCell`](Components/NSynRenshowCell.md)
- **NSynSPNeuron** — [`NSynSPNeuron`](Components/NSynSPNeuron.md)
- **NSynTCNNeuron** — [`NSynTCNNeuron`](Components/NSynTCNNeuron.md)
- **NSynapseCable** — [`NSynapseCable`](Components/NSynapseCable.md)
- **NSynapseCableMulti** — [`NSynapseCableMulti`](Components/NSynapseCableMulti.md)
- **NSynapseClassic** — [`NSynapseClassic`](Components/NSynapseClassic.md)
- **NSynapseClassicSlv** — [`NSynapseClassicSlv`](Components/NSynapseClassicSlv.md)
- **NSynapseIaF** — [`NSynapseIaF`](Components/NSynapseIaF.md)
- **NSynapseStdp** — [`NSynapseStdp`](Components/NSynapseStdp.md)
- **NSynapseTrainerStdp** — [`NSynapseTrainerStdp`](Components/NSynapseTrainerStdp.md)
- **NSynapseTrainerStdpClassicDiscrete** — [`NSynapseTrainerStdpClassicDiscrete`](Components/NSynapseTrainerStdpClassicDiscrete.md)
- **NSynapseTrainerStdpClassicIntegrated** — [`NSynapseTrainerStdpClassicIntegrated`](Components/NSynapseTrainerStdpClassicIntegrated.md)
- **NSynapseTrainerStdpLobov** — [`NSynapseTrainerStdpLobov`](Components/NSynapseTrainerStdpLobov.md)
- **NSynapseTrainerStdpMirror** — [`NSynapseTrainerStdpMirror`](Components/NSynapseTrainerStdpMirror.md)
- **NSynapseTrainerStdpProbabilistic** — [`NSynapseTrainerStdpProbabilistic`](Components/NSynapseTrainerStdpProbabilistic.md)
- **NSynapseTrainerStdpStable** — [`NSynapseTrainerStdpStable`](Components/NSynapseTrainerStdpStable.md)
- **NSynapseTrainerStdpTD** — [`NSynapseTrainerStdpTD`](Components/NSynapseTrainerStdpTD.md)
- **NSynapseTrainerStdpTriplet** — [`NSynapseTrainerStdpTriplet`](Components/NSynapseTrainerStdpTriplet.md)
- **NSynapseTrainerStdpWD** — [`NSynapseTrainerStdpWD`](Components/NSynapseTrainerStdpWD.md)

**Приоритет описания** (по встречаемости в `Bin/Configs`): базовые нейроны/синапсы/каналы, LT-зоны, STDP-тренеры, классификаторы, генераторы, `NNet/NModel`, простые рефлексы/предсказатели.

### Использование компонентов в конфигурационных проектах

Компоненты библиотеки активно используются в конфигурационных проектах в `Bin/Configs`. Ниже указаны основные области применения для наиболее часто используемых компонентов:

- **Эксперименты с нейронами**: `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`
- **STDP-обучение**: `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`
- **Классификация**: `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`
- **Когнитивная навигация**: `Bin/Configs/User/CognitiveNavigation/`
- **Управление движением**: `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`

Подробнее о структуре конфигураций см. [Config-Overview.md](Config-Overview.md) и [Config-Templates.md](Config-Templates.md).

---

## EN

The full list of registrations (`Core/NPulseLibrary.cpp`) is very large. Below is a grouping with brief purpose and signal types. For the complete set, see the source file `NPulseLibrary.cpp`.

## Basic networks/structures

- **NNet** — see [`NNet`](Components/NNet.md)  
  *Usage:* Root component in some configuration projects
- **NModel** — see [`NModel`](Components/NModel.md)  
  *Usage:* Root component in most projects (`Bin/Configs/*/Model_*.xml`)
- **NLifeNet** — see [`NLifeNet`](Components/NLifeNet.md)
- **NNeuronsLayer** — see [`NNeuronsLayer`](Components/NNeuronsLayer.md)  
  *Usage:* Creating neuron layers for classification (`Bin/Configs/!OldConfigs/SpikeClassifier/`)
- **NNeuronsLayerIaF** — see [`NNeuronsLayerIaF`](Components/NNeuronsLayerIaF.md)
- **NNeuronFreqGroup** — see [`NNeuronFreqGroup`](Components/NNeuronFreqGroup.md)
- **NNeuronFreqGroupLayer** — see [`NNeuronFreqGroupLayer`](Components/NNeuronFreqGroupLayer.md)
- **NNeuronLife** — see [`NNeuronLife`](Components/NNeuronLife.md)

## Neurons

- Overview: [`Neurons`](Components/Neurons.md)  
  *Typical configurations:* `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`
- Details:
  - [`NPulseNeuronIzhikevich`](Components/NPulseNeuronIzhikevich.md) — *Frequently used in:* `Bin/Configs/!OldConfigs/OldExperiments/IzhikevichTest/`, `Bin/Configs/!OldConfigs/STDP-Simple-01/`
  - [`NIntegrateAndFireNeuron`](Components/NIntegrateAndFireNeuron.md) — *Used in:* IaF model experiments
  - [`NSPNeuronGen`](Components/NSPNeuronGen.md) — *Used in cognitive navigation:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NLPNeuron`](Components/NLPNeuron.md)
  - [`NNeuron`](Components/NNeuron.md)
  - [`NAfferentNeuron`](Components/NAfferentNeuron.md) — *Used in:* `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`, `Bin/Configs/!OldConfigs/OldExperiments/SimplestMotionControlSAfferent/`

## Synapses / Trainers / Channels / Membranes / LT zones

- Overview:
  - [`Synapses & Trainers`](Components/SynapsesAndTrainers.md) — *Typical configurations:* `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`
  - [`Channels, Membranes & LT zones`](Components/ChannelsMembranesLT.md) — *Typical configurations:* `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`
- Details:
  - [`NSynapseStdp`](Components/NSynapseStdp.md) — *Frequently used in:* `Bin/Configs/!OldConfigs/STDP-Simple-01/`
  - [`NPSynapseBio`](Components/NPSynapseBio.md) — *Used in cognitive navigation:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NSynapseTrainerStdp`](Components/NSynapseTrainerStdp.md) — *Used in:* training experiments
  - [`NPExcChannelBio`](Components/NPExcChannelBio.md), [`NPInhChannelBio`](Components/NPInhChannelBio.md) — *Used in:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NPMembraneBio`](Components/NPMembraneBio.md) — *Used in:* `Bin/Configs/User/CognitiveNavigation/`
  - [`NPulseLTZoneThreshold`](Components/NPulseLTZoneThreshold.md) — *Frequently used in:* `Bin/Configs/User/CognitiveNavigation/`, `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NPulseChannelIaF`](Components/NPulseChannelIaF.md)
  - [`NPulseMembraneIzhikevich`](Components/NPulseMembraneIzhikevich.md)
  - [`NPulseLTZoneIzhikevich`](Components/NPulseLTZoneIzhikevich.md)
  - [`NCLTZone`](Components/NCLTZone.md)

## Generators / Sources / Delays and IO

- Overview: [`Generators, IO, Classifiers & Others`](Components/GeneratorsIOClassifiers.md)  
  *Typical configurations:* `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/User/CognitiveNavigation/`
- Details:
  - [`NPulseGenerator`](Components/NPulseGenerator.md) — *Used in:* `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NPulseGeneratorTransit`](Components/NPulseGeneratorTransit.md) — *Used in cognitive navigation:* `Bin/Configs/User/CognitiveNavigation/` (input signals Forward, Back, Left, Right)
  - [`NPGenerator`](Components/NPGenerator.md) — *Used in:* `Bin/Configs/!OldConfigs/NM-Neurons/`
  - [`NCGenerator`](Components/NCGenerator.md)
  - [`NPDelay`](Components/NPDelay.md)
  - [`NSource`](Components/NSource.md)
  - [`NReceptor`](Components/NReceptor.md) — *Used in:* `Bin/Configs/!OldConfigs/NReceptor/`

## Classifiers / Reflexes / Predictors / Effectors / Logic / Solver

- Classifiers:
  - [`NClassifier`](Components/NClassifier.md)
  - [`NSpikeClassifier`](Components/NSpikeClassifier.md) — *Used in:* `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`
  - [`NPCAClassifier`](Components/NPCAClassifier.md) — *Used in:* `Bin/Configs/!OldConfigs/PCATest/`
- Reflexes/associations:
  - [`NConditionedReflex`](Components/NConditionedReflex.md) — *Used in:* `Bin/Configs/!OldConfigs/SpikeConditionalReflex/`
  - [`NAssociationFormer`](Components/NAssociationFormer.md) — *Used in:* `Bin/Configs/!OldConfigs/SpikeAssociationPlus/`
- Effectors/logic/solver:
  - [`NMuscle`](Components/NMuscle.md) — *Used in:* `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`
  - [`NEyeMuscle`](Components/NEyeMuscle.md) — *Used in:* `Bin/Configs/!OldConfigs/EyeRetina/`
  - [`NMotoneuron`](Components/NMotoneuron.md) — *Used in:* `Bin/Configs/!OldConfigs/OldExperiments/Motoneuron/`
  - [`NLogicalNot`](Components/NLogicalNot.md)
  - [`NCPac`](Components/NCPac.md)
  - [`NOdeSolver`](Components/NOdeSolver.md)

## Full component list (UploadClass)

Below is the full list of registration names from `Core/NPulseLibrary.cpp` with links to per-component files in `Docs/Components/`.

- **NAfferentNeuron** — [`NAfferentNeuron`](Components/NAfferentNeuron.md)
- **NAssociationFormer** — [`NAssociationFormer`](Components/NAssociationFormer.md)
- **NCGenerator** — [`NCGenerator`](Components/NCGenerator.md)
- **NCLTZone** — [`NCLTZone`](Components/NCLTZone.md)
- **NCNeuron** — [`NCNeuron`](Components/NCNeuron.md)
- **NCNeuronNegCGenerator** — [`NCNeuronNegCGenerator`](Components/NCNeuronNegCGenerator.md)
- **NCNeuronPosCGenerator** — [`NCNeuronPosCGenerator`](Components/NCNeuronPosCGenerator.md)
- **NCPac** — [`NCPac`](Components/NCPac.md)
- **NCSimpleLTZone** — [`NCSimpleLTZone`](Components/NCSimpleLTZone.md)
- **NCSynChannel** — [`NCSynChannel`](Components/NCSynChannel.md)
- **NCSynExcChannel** — [`NCSynExcChannel`](Components/NCSynExcChannel.md)
- **NCSynInhChannel** — [`NCSynInhChannel`](Components/NCSynInhChannel.md)
- **NCSynNeuronMembrane** — [`NCSynNeuronMembrane`](Components/NCSynNeuronMembrane.md)
- **NClassifier** — [`NClassifier`](Components/NClassifier.md)
- **NConditionedReflex** — [`NConditionedReflex`](Components/NConditionedReflex.md)
- **NContinuesSAfferentNeuron** — [`NContinuesSAfferentNeuron`](Components/NContinuesSAfferentNeuron.md)
- **NContinuesSimpleAfferentNeuron** — [`NContinuesSimpleAfferentNeuron`](Components/NContinuesSimpleAfferentNeuron.md)
- **NContinuesSynLPNeuron** — [`NContinuesSynLPNeuron`](Components/NContinuesSynLPNeuron.md)
- **NContinuesSynMotoneuron** — [`NContinuesSynMotoneuron`](Components/NContinuesSynMotoneuron.md)
- **NContinuesSynRenshowCell** — [`NContinuesSynRenshowCell`](Components/NContinuesSynRenshowCell.md)
- **NContinuesSynSPNeuron** — [`NContinuesSynSPNeuron`](Components/NContinuesSynSPNeuron.md)
- **NDataset** — [`NDataset`](Components/NDataset.md)
- **NEyeMuscle** — [`NEyeMuscle`](Components/NEyeMuscle.md)
- **NFileGenerator** — [`NFileGenerator`](Components/NFileGenerator.md)
- **NGenBioNeuron** — [`NGenBioNeuron`](Components/NGenBioNeuron.md)
- **NIntegrateAndFireNeuron** — [`NIntegrateAndFireNeuron`](Components/NIntegrateAndFireNeuron.md)
- **NLPHebbNeuron** — [`NLPHebbNeuron`](Components/NLPHebbNeuron.md)
- **NLPLifeHebbNeuron** — [`NLPLifeHebbNeuron`](Components/NLPLifeHebbNeuron.md)
- **NLPLifeNeuron** — [`NLPLifeNeuron`](Components/NLPLifeNeuron.md)
- **NLPNeuron** — [`NLPNeuron`](Components/NLPNeuron.md)
- **NLifeNet** — [`NLifeNet`](Components/NLifeNet.md)
- **NLogicalNot** — [`NLogicalNot`](Components/NLogicalNot.md)
- **NMExtrapolator** — [`NMExtrapolator`](Components/NMExtrapolator.md)
- **NModel** — [`NModel`](Components/NModel.md)
- **NMotoneuron** — [`NMotoneuron`](Components/NMotoneuron.md)
- **NMuscle** — [`NMuscle`](Components/NMuscle.md)
- **NNet** — [`NNet`](Components/NNet.md)
- **NNeuron** — [`NNeuron`](Components/NNeuron.md)
- **NNeuronFreqGroup** — [`NNeuronFreqGroup`](Components/NNeuronFreqGroup.md)
- **NNeuronFreqGroupLayer** — [`NNeuronFreqGroupLayer`](Components/NNeuronFreqGroupLayer.md)
- **NNeuronLearner** — [`NNeuronLearner`](Components/NNeuronLearner.md)
- **NNeuronLife** — [`NNeuronLife`](Components/NNeuronLife.md)
- **NNeuronTrainer** — [`NNeuronTrainer`](Components/NNeuronTrainer.md)
- **NNeuronsLayer** — [`NNeuronsLayer`](Components/NNeuronsLayer.md)
- **NNeuronsLayerIaF** — [`NNeuronsLayerIaF`](Components/NNeuronsLayerIaF.md)
- **NNewLPHebbNeuron** — [`NNewLPHebbNeuron`](Components/NNewLPHebbNeuron.md)
- **NNewLPNeuron** — [`NNewLPNeuron`](Components/NNewLPNeuron.md)
- **NNewMotoneuron** — [`NNewMotoneuron`](Components/NNewMotoneuron.md)
- **NNewRenshowCell** — [`NNewRenshowCell`](Components/NNewRenshowCell.md)
- **NNewSPHebbNeuron** — [`NNewSPHebbNeuron`](Components/NNewSPHebbNeuron.md)
- **NNewSPLifeNeuron** — [`NNewSPLifeNeuron`](Components/NNewSPLifeNeuron.md)
- **NNewSPNeuron** — [`NNewSPNeuron`](Components/NNewSPNeuron.md)
- **NNewSynLPNeuron** — [`NNewSynLPNeuron`](Components/NNewSynLPNeuron.md)
- **NNewSynMotoneuron** — [`NNewSynMotoneuron`](Components/NNewSynMotoneuron.md)
- **NNewSynRenshowCell** — [`NNewSynRenshowCell`](Components/NNewSynRenshowCell.md)
- **NNewSynSPNeuron** — [`NNewSynSPNeuron`](Components/NNewSynSPNeuron.md)
- **NNewSynTCNNeuron** — [`NNewSynTCNNeuron`](Components/NNewSynTCNNeuron.md)
- **NOdeSolver** — [`NOdeSolver`](Components/NOdeSolver.md)
- **NPCAClassifier** — [`NPCAClassifier`](Components/NPCAClassifier.md)
- **NPChannel** — [`NPChannel`](Components/NPChannel.md)
- **NPDelay** — [`NPDelay`](Components/NPDelay.md)
- **NPExcChannel** — [`NPExcChannel`](Components/NPExcChannel.md)
- **NPExcChannelBio** — [`NPExcChannelBio`](Components/NPExcChannelBio.md)
- **NPExcChannelBio2** — [`NPExcChannelBio2`](Components/NPExcChannelBio2.md)
- **NPGenerator** — [`NPGenerator`](Components/NPGenerator.md)
- **NPGeneratorDelay** — [`NPGeneratorDelay`](Components/NPGeneratorDelay.md)
- **NPHebbLifeSynapse** — [`NPHebbLifeSynapse`](Components/NPHebbLifeSynapse.md)
- **NPHebbNeuron** — [`NPHebbNeuron`](Components/NPHebbNeuron.md)
- **NPHebbSynapse** — [`NPHebbSynapse`](Components/NPHebbSynapse.md)
- **NPInhChannel** — [`NPInhChannel`](Components/NPInhChannel.md)
- **NPInhChannelBio** — [`NPInhChannelBio`](Components/NPInhChannelBio.md)
- **NPInhChannelBio2** — [`NPInhChannelBio2`](Components/NPInhChannelBio2.md)
- **NPLTChannel** — [`NPLTChannel`](Components/NPLTChannel.md)
- **NPLTExcChannel** — [`NPLTExcChannel`](Components/NPLTExcChannel.md)
- **NPLTInhChannel** — [`NPLTInhChannel`](Components/NPLTInhChannel.md)
- **NPLTSynChannel** — [`NPLTSynChannel`](Components/NPLTSynChannel.md)
- **NPLTSynExcChannel** — [`NPLTSynExcChannel`](Components/NPLTSynExcChannel.md)
- **NPLTSynInhChannel** — [`NPLTSynInhChannel`](Components/NPLTSynInhChannel.md)
- **NPLTZone** — [`NPLTZone`](Components/NPLTZone.md)
- **NPLTZoneNeuronMembrane** — [`NPLTZoneNeuronMembrane`](Components/NPLTZoneNeuronMembrane.md)
- **NPLTZoneSynNeuronMembrane** — [`NPLTZoneSynNeuronMembrane`](Components/NPLTZoneSynNeuronMembrane.md)
- **NPLifeNeuron** — [`NPLifeNeuron`](Components/NPLifeNeuron.md)
- **NPMembrane** — [`NPMembrane`](Components/NPMembrane.md)
- **NPMembraneBio** — [`NPMembraneBio`](Components/NPMembraneBio.md)
- **NPMembraneBio2** — [`NPMembraneBio2`](Components/NPMembraneBio2.md)
- **NPNeuron** — [`NPNeuron`](Components/NPNeuron.md)
- **NPNeuron1x4** — [`NPNeuron1x4`](Components/NPNeuron1x4.md)
- **NPNeuron4x1** — [`NPNeuron4x1`](Components/NPNeuron4x1.md)
- **NPNeuron4x4** — [`NPNeuron4x4`](Components/NPNeuron4x4.md)
- **NPNeuronHebbLifeMembrane** — [`NPNeuronHebbLifeMembrane`](Components/NPNeuronHebbLifeMembrane.md)
- **NPNeuronHebbMembrane** — [`NPNeuronHebbMembrane`](Components/NPNeuronHebbMembrane.md)
- **NPNeuronNegCGenerator** — [`NPNeuronNegCGenerator`](Components/NPNeuronNegCGenerator.md)
- **NPNeuronNegCGeneratorBio** — [`NPNeuronNegCGeneratorBio`](Components/NPNeuronNegCGeneratorBio.md)
- **NPNeuronPosCGenerator** — [`NPNeuronPosCGenerator`](Components/NPNeuronPosCGenerator.md)
- **NPNeuronPosCGeneratorBio** — [`NPNeuronPosCGeneratorBio`](Components/NPNeuronPosCGeneratorBio.md)
- **NPNeuronPosCGeneratorCable** — [`NPNeuronPosCGeneratorCable`](Components/NPNeuronPosCGeneratorCable.md)
- **NPSimpleLTZone** — [`NPSimpleLTZone`](Components/NPSimpleLTZone.md)
- **NPSynChannel** — [`NPSynChannel`](Components/NPSynChannel.md)
- **NPSynExcChannel** — [`NPSynExcChannel`](Components/NPSynExcChannel.md)
- **NPSynInhChannel** — [`NPSynInhChannel`](Components/NPSynInhChannel.md)
- **NPSynNeuronMembrane** — [`NPSynNeuronMembrane`](Components/NPSynNeuronMembrane.md)
- **NPSynapse** — [`NPSynapse`](Components/NPSynapse.md)
- **NPSynapseBio** — [`NPSynapseBio`](Components/NPSynapseBio.md)
- **NPSynapseBio2** — [`NPSynapseBio2`](Components/NPSynapseBio2.md)
- **NPSynapseStdp** — [`NPSynapseStdp`](Components/NPSynapseStdp.md)
- **NPac** — [`NPac`](Components/NPac.md)
- **NPainReflexSimple** — [`NPainReflexSimple`](Components/NPainReflexSimple.md)
- **NPattern** — [`NPattern`](Components/NPattern.md)
- **NPredictor** — [`NPredictor`](Components/NPredictor.md)
- **NPulseChannelCable** — [`NPulseChannelCable`](Components/NPulseChannelCable.md)
- **NPulseChannelCableMulti** — [`NPulseChannelCableMulti`](Components/NPulseChannelCableMulti.md)
- **NPulseChannelIaF** — [`NPulseChannelIaF`](Components/NPulseChannelIaF.md)
- **NPulseChannelIzhikevich** — [`NPulseChannelIzhikevich`](Components/NPulseChannelIzhikevich.md)
- **NPulseGeneratorDelay** — [`NPulseGeneratorDelay`](Components/NPulseGeneratorDelay.md)
- **NPulseGeneratorMulti** — [`NPulseGeneratorMulti`](Components/NPulseGeneratorMulti.md)
- **NPulseGeneratorTransit** — [`NPulseGeneratorTransit`](Components/NPulseGeneratorTransit.md)
- **NPulseGenerator** — [`NPulseGenerator`](Components/NPulseGenerator.md)
- **NPulseLTZoneCable** — [`NPulseLTZoneCable`](Components/NPulseLTZoneCable.md)
- **NPulseLTZoneIaF** — [`NPulseLTZoneIaF`](Components/NPulseLTZoneIaF.md)
- **NPulseLTZoneIzhikevich** — [`NPulseLTZoneIzhikevich`](Components/NPulseLTZoneIzhikevich.md)
- **NPulseLTZoneThreshold** — [`NPulseLTZoneThreshold`](Components/NPulseLTZoneThreshold.md)
- **NPulseLTZoneThresholdBio** — [`NPulseLTZoneThresholdBio`](Components/NPulseLTZoneThresholdBio.md)
- **NPulseLTZoneThresholdBio2** — [`NPulseLTZoneThresholdBio2`](Components/NPulseLTZoneThresholdBio2.md)
- **NPulseMembraneCable** — [`NPulseMembraneCable`](Components/NPulseMembraneCable.md)
- **NPulseMembraneCableMulti** — [`NPulseMembraneCableMulti`](Components/NPulseMembraneCableMulti.md)
- **NPulseMembraneIaF** — [`NPulseMembraneIaF`](Components/NPulseMembraneIaF.md)
- **NPulseMembraneIaFStdp** — [`NPulseMembraneIaFStdp`](Components/NPulseMembraneIaFStdp.md)
- **NPulseMembraneIzhikevich** — [`NPulseMembraneIzhikevich`](Components/NPulseMembraneIzhikevich.md)
- **NPulseNeuronCable** — [`NPulseNeuronCable`](Components/NPulseNeuronCable.md)
- **NPulseNeuronCableMulti** — [`NPulseNeuronCableMulti`](Components/NPulseNeuronCableMulti.md)
- **NPulseNeuronIaF** — [`NPulseNeuronIaF`](Components/NPulseNeuronIaF.md)
- **NPulseNeuronIaFStdp** — [`NPulseNeuronIaFStdp`](Components/NPulseNeuronIaFStdp.md)
- **NPulseNeuronIzhikevich** — [`NPulseNeuronIzhikevich`](Components/NPulseNeuronIzhikevich.md)
- **NPulsePerseptron** — [`NPulsePerseptron`](Components/NPulsePerseptron.md)
- **NPulsePerseptronIaF** — [`NPulsePerseptronIaF`](Components/NPulsePerseptronIaF.md)
- **NPulseSynapseStdp** — [`NPulseSynapseStdp`](Components/NPulseSynapseStdp.md)
- **NReceiver** — [`NReceiver`](Components/NReceiver.md)
- **NReceptor** — [`NReceptor`](Components/NReceptor.md)
- **NRenshowCell** — [`NRenshowCell`](Components/NRenshowCell.md)
- **NSAfferentNeuron** — [`NSAfferentNeuron`](Components/NSAfferentNeuron.md)
- **NSPDendriteLifeHebbNeuron** — [`NSPDendriteLifeHebbNeuron`](Components/NSPDendriteLifeHebbNeuron.md)
- **NSPHebbNeuron** — [`NSPHebbNeuron`](Components/NSPHebbNeuron.md)
- **NSPLifeHebbNeuron** — [`NSPLifeHebbNeuron`](Components/NSPLifeHebbNeuron.md)
- **NSPLifeNeuron** — [`NSPLifeNeuron`](Components/NSPLifeNeuron.md)
- **NSPMinNeuron** — [`NSPMinNeuron`](Components/NSPMinNeuron.md)
- **NSPNeuron** — [`NSPNeuron`](Components/NSPNeuron.md)
- **NSPNeuronBio** — [`NSPNeuronBio`](Components/NSPNeuronBio.md)
- **NSPNeuronBio2** — [`NSPNeuronBio2`](Components/NSPNeuronBio2.md)
- **NSPNeuronGen** — [`NSPNeuronGen`](Components/NSPNeuronGen.md)
- **NSimpleAfferentNeuron** — [`NSimpleAfferentNeuron`](Components/NSimpleAfferentNeuron.md)
- **NSinusGenerator** — [`NSinusGenerator`](Components/NSinusGenerator.md)
- **NSource** — [`NSource`](Components/NSource.md)
- **NSpikeClassifier** — [`NSpikeClassifier`](Components/NSpikeClassifier.md)
- **NStatePredictor** — [`NStatePredictor`](Components/NStatePredictor.md)
- **NSum** — [`NSum`](Components/NSum.md)
- **NSynLPNeuron** — [`NSynLPNeuron`](Components/NSynLPNeuron.md)
- **NSynMotoneuron** — [`NSynMotoneuron`](Components/NSynMotoneuron.md)
- **NSynRenshowCell** — [`NSynRenshowCell`](Components/NSynRenshowCell.md)
- **NSynSPNeuron** — [`NSynSPNeuron`](Components/NSynSPNeuron.md)
- **NSynTCNNeuron** — [`NSynTCNNeuron`](Components/NSynTCNNeuron.md)
- **NSynapseCable** — [`NSynapseCable`](Components/NSynapseCable.md)
- **NSynapseCableMulti** — [`NSynapseCableMulti`](Components/NSynapseCableMulti.md)
- **NSynapseClassic** — [`NSynapseClassic`](Components/NSynapseClassic.md)
- **NSynapseClassicSlv** — [`NSynapseClassicSlv`](Components/NSynapseClassicSlv.md)
- **NSynapseIaF** — [`NSynapseIaF`](Components/NSynapseIaF.md)
- **NSynapseStdp** — [`NSynapseStdp`](Components/NSynapseStdp.md)
- **NSynapseTrainerStdp** — [`NSynapseTrainerStdp`](Components/NSynapseTrainerStdp.md)
- **NSynapseTrainerStdpClassicDiscrete** — [`NSynapseTrainerStdpClassicDiscrete`](Components/NSynapseTrainerStdpClassicDiscrete.md)
- **NSynapseTrainerStdpClassicIntegrated** — [`NSynapseTrainerStdpClassicIntegrated`](Components/NSynapseTrainerStdpClassicIntegrated.md)
- **NSynapseTrainerStdpLobov** — [`NSynapseTrainerStdpLobov`](Components/NSynapseTrainerStdpLobov.md)
- **NSynapseTrainerStdpMirror** — [`NSynapseTrainerStdpMirror`](Components/NSynapseTrainerStdpMirror.md)
- **NSynapseTrainerStdpProbabilistic** — [`NSynapseTrainerStdpProbabilistic`](Components/NSynapseTrainerStdpProbabilistic.md)
- **NSynapseTrainerStdpStable** — [`NSynapseTrainerStdpStable`](Components/NSynapseTrainerStdpStable.md)
- **NSynapseTrainerStdpTD** — [`NSynapseTrainerStdpTD`](Components/NSynapseTrainerStdpTD.md)
- **NSynapseTrainerStdpTriplet** — [`NSynapseTrainerStdpTriplet`](Components/NSynapseTrainerStdpTriplet.md)
- **NSynapseTrainerStdpWD** — [`NSynapseTrainerStdpWD`](Components/NSynapseTrainerStdpWD.md)

**Description priority** (by frequency in `Bin/Configs`): basic neurons/synapses/channels, LT zones, STDP trainers, classifiers, generators, `NNet/NModel`, simple reflexes/predictors.

### Component usage in configuration projects

Library components are actively used in configuration projects in `Bin/Configs`. Below are the main application areas for the most frequently used components:

- **Neuron experiments**: `Bin/Configs/!OldConfigs/NM-Neurons/`, `Bin/Configs/!OldConfigs/NM-AfferentNeurons/`
- **STDP training**: `Bin/Configs/!OldConfigs/STDP-Simple-01/`, `Bin/Configs/!OldConfigs/SpikeAnsTrainer/`
- **Classification**: `Bin/Configs/!OldConfigs/SpikeClassifier/`, `Bin/Configs/!OldConfigs/SpikeANPA3/`
- **Cognitive navigation**: `Bin/Configs/User/CognitiveNavigation/`
- **Motion control**: `Bin/Configs/!OldConfigs/OldExperiments/MotionControl/`, `Bin/Configs/!OldConfigs/MC-Muscles/`

For more on configuration structure, see [Config-Overview.md](Config-Overview.md) and [Config-Templates.md](Config-Templates.md).
