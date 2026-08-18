## RU

## NNeuronTimeLearnerBranch — обучение паттерну на одном дендрите

**Класс**: `NNeuronTimeLearnerBranch` — контейнер (`UNet`) с внутренним `NPulseNeuron` (1 сома + 1 дендрит) и `NDatasetMatrix` (как у `NNeuronTimeLearner`: `NumFeatures=1`, `MaxSpikesPerFeature=N`, один `Generator1`, ISI N×1).
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTimeLearnerBranch", ...)`.
**База**: независимая копия [`NNeuronTimeLearner`](NNeuronTimeLearner.md) (не наследник).
**Алгоритм**: [`Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/ALGORITHM.md`](../../../../Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/ALGORITHM.md).

### Идея

`NumInputDendrite = N` — число **импульсов**, не дендритов. Физически: `Soma1` + цепь `Dendrite1_1 … Dendrite1_Lmax`.

`DendriteLength[k]` — **сегмент синапса импульса k** на `Dendrite1_{seg}` (`≥1`; на `Soma1` входов нет).

Обучение **в обратном порядке**: сначала якорь N−1, затем N−2 … 0. Нормализация амплитуды идёт вместе с синхронизацией текущего импульса. На время обучения к `Generator1` подключён **только синапс активного импульса** (`ApplyPulseGeneratorMute`); остальные отключены, чтобы весь burst шёл в одну точку кабеля (как один «виртуальный дендрит» в `NNeuronTimeLearner`).

### Ключевые параметры

Как у `NNeuronTimeLearner`, плюс семантика `DendriteLength` как сегментов импульсов. Cold: `[1,1,1,1]` при N=4.

Sample: `Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch`.
