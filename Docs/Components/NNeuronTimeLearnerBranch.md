## RU

## NNeuronTimeLearnerBranch — обучение паттерну на одном дендрите

**Класс**: `NNeuronTimeLearnerBranch` — контейнер (`UNet`) с внутренним `NPulseNeuron` (1 сома + 1 дендрит) и `NDatasetMatrix` (`NumFeatures=N` генераторов).
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTimeLearnerBranch", ...)`.
**База**: независимая копия [`NNeuronTimeLearner`](NNeuronTimeLearner.md) (не наследник).
**Алгоритм**: [`Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/ALGORITHM.md`](../../../../Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/ALGORITHM.md).

### Идея

`NumInputDendrite = N` — число **импульсов**, не дендритов. Физически: `Soma1` + цепь `Dendrite1_1 … Dendrite1_Lmax`.

`DendriteLength[k]` — **позиция синапса импульса k**: `0` = якорь на `Soma1` (последний импульс, не растёт); `≥1` = сегмент `Dendrite1_{seg}`.

Обучение **в обратном порядке**: сначала якорь N−1, затем N−2 … 0. Нормализация амплитуды идёт вместе с синхронизацией текущего импульса. Необученные ранние импульсы глушатся (mute), чтобы не забивать общую сому.

### Ключевые параметры

Как у `NNeuronTimeLearner`, плюс семантика `DendriteLength` как позиций импульсов. Cold: `[1,1,1,0]` при N=4.

Sample: `Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch`.
