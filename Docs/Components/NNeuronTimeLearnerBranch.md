## RU

**Сверка 2026-09-22:** фаза 0 — совместная настройка длины/амплитуды; 1 не назначается; 2 Done; 3 legacy CalibrateLtz при PostTune OFF; 4 PostTune. Defaults: EnablePostTrainTuning=true, AutoScaleIterationGap=true. SearchSynthetic вызывает ScaleTipR для snapshot. Done допускает best-effort. [Текущий контракт и ограничения](../../../../Bin/Configs/SpikeSamples/StructTrain/AUDIT_2026-09-22.md).


## NNeuronTimeLearnerBranch — обучение паттерну на одном дендрите

**Класс**: `NNeuronTimeLearnerBranch` — контейнер (`UNet`) с внутренним `NPulseNeuron` (1 сома + 1 дендрит) и `NDatasetMatrix` (как у `NNeuronTimeLearner`: `NumFeatures=1`, `MaxSpikesPerFeature=N`, один `Generator1`, ISI N×1).
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTimeLearnerBranch", ...)`.
**База**: независимая копия [`NNeuronTimeLearner`](NNeuronTimeLearner.md) (не наследник).
**Алгоритм**: [`Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/ALGORITHM.md`](../../../../Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch/Train/ALGORITHM.md).

### Идея

`NumInputDendrite = N` — число **импульсов**, не дендритов. Физически: `Soma1` + цепь `Dendrite1_1 … Dendrite1_Lmax`.

`DendriteLength[k]` — **сегмент синапса импульса k** на `Dendrite1_{seg}` (`≥1`; на `Soma1` входов нет).

Обучение **в обратном порядке**: сначала якорь N−1, затем N−2 … 0. Нормализация амплитуды идёт вместе с синхронизацией текущего импульса. На время обучения к `Generator1` подключён **только синапс активного импульса** (`ApplyPulseGeneratorMute`); остальные отключены, чтобы весь burst шёл в одну точку кабеля (как один «виртуальный дендрит» в `NNeuronTimeLearner`).

### Ключевые параметры

Как у `NNeuronTimeLearner`, плюс семантика `DendriteLength` как сегментов импульсов. Cold: `[1,1,1,1]` при N=4.

Sample: `Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearnerBranch`.


### Уточнение 2026-09-23 (9a6cee0b)

Search после выбора Tips запускает отдельный free-run; при плохом landscape возвращает snapshot. При включённом mid Search Train оставляет silent-порог, Finalize запрещает второй inference-mid в этом процессе; следующий Test использует новый процесс/reset. Обычный finite inference failure также оставляет silent, но Complete не означает успешную калибровку. Остаются NaN/setup/fallback-metric проблемы и неподдержанный выбор LTZ в free-run. Same-Matrix mid (sample 0 — target) принят как текущая особенность A07. [Текущий контракт](../../../../Bin/Configs/SpikeSamples/StructTrain/AUDIT_2026-09-22.md).
