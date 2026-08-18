## RU

## NNeuronTimeLearner — обучение временному паттерну

**Класс**: `NNeuronTimeLearner` — контейнер (`UNet`) с внутренним `NPulseNeuron` и дочерним `NDatasetMatrix`.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTimeLearner", ...)`.
**База кода**: копия/адаптация [`NNeuronLearner`](NNeuronLearner.md).
**Алгоритм (подробно)**: [`Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearner/ALGORITHM.md`](../../../../Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearner/ALGORITHM.md).
**Вариант с одним дендритом**: [`NNeuronTimeLearnerBranch`](NNeuronTimeLearnerBranch.md).

### Идея

Один генератор датасета подаёт **последовательность из N импульсов** одновременно на дистальные возбуждающие синапсы **всех** дендритов (fan-out): `DatasetMatrix.Generator1` → tip `ExcSynapse` каждого дендрита.

- Импульс `k` влияет на рост только дендрита `k` (пик измеряется на `Soma{k+1}`).
- **Последний** дендрит (`N−1`) — опора синхронизации: его длина не наращивается, к времени его пика подстраиваются дендриты `0..N-2`.
- **Joint train** (фаза 0): в одной пачке — подбор длины активного дендрита (round-robin) плюс нормализация амплитуд (parametric R по умолчанию). Фаза 1 в коде **не выставляется**. Done (`TrainingPhase=2`) при `AllDendritesSynced() ∧ AllSynapsesNormalized()`.

### Ключевые параметры

| Свойство | Смысл |
|----------|--------|
| `NumInputDendrite` | Число дендритов = `MaxSpikesPerFeature` датасета |
| `InputPattern` | ISI-матрица `N×1` → `DatasetMatrix.MatrixData` |
| `Delay` | Пауза датасета между пачками |
| `IterationGap` (`t`) | Мин. время после первого импульса прошлой итерации до старта следующей |
| `SyncTolerance` | Допуск одновременности пиков (сек); default **0.02** |
| `PeakMeasureMargin` | Полуокно поиска пика вокруг `Expected[i]+cable delay` (default 0.06) |
| `DelayAgreeMarginMin` | Пол agree в `DelayUseOf`: `max(SyncTolerance, DelayAgreeMarginMin)` (default 0.03) |
| `TrainingPhase` | **0** joint train (длина+амплитуда); **1** legacy unused; **2** Done |
| `NormalizationMode` | **0** structural (±`NumSynapse`); **1** parametric tip R (default) |
| `TrainingLTZThreshold` / `FixedLTZThreshold` | Порог при обучении / после |
| `AutoCalibrateFixedLTZThreshold` | После Done: выставить `FixedLTZThreshold` по min/max LTZ последней synced-итерации |
| `CalibrateLTZThresholdMode` | 0 = gap_fraction (`min + f*(max-min)`), 1 = peak_fraction (`max*f`) |
| `CalibrateLTZThresholdFraction` | Доля (default 0.85) |
| `CalibratedFixedLTZThreshold` | Результат калибровки (state) |

### Критерий длины (coincidence)

```
needed[k] = PrevPeakRel[ref] - Expected[k]
delay_len = (L-1)·EstDelayPerSeg
delay_use = DelayFromPulse  если согласован с delay_len / peak_model (±agree_margin)
          иначе delay_len
dt = needed - delay_use
```

Пока нет valid peak опоры (`HasPrevPeakSnapshot` только при `PrevPeakValid[N−1]`) — **роста нет** (bootstrap).

### Lifecycle

- **ADefault** — defaults (`IterationGap=0.5`, `SyncTolerance=0.02`, `NormalizationMode=1`, высокий `TrainingLTZThreshold=100`). `SyncTolerance=1e-6` не ловит дискретные пики при `DefaultTimeStep≈2e-3`.
- **ABuild** → `BuildStructure`: `Neuron` (`NumSomaMembraneParts=N`, независимые длины) + `DatasetMatrix`, fan-out `Generator1` на все дистальные `ExcSynapse`.
- **ACalculate** → амплитуды; при `IsNeedToTrain` — `Training()`. Done проверяется **между** пачками.

### Config snippet

```ini
[Component]
ClassName = NNeuronTimeLearner
Name = NeuronTimeLearner1
NumInputDendrite = 4
IterationGap = 1.5
SyncTolerance = 0.02
NormalizationMode = 1
```

Внутренний `DatasetMatrix`: `NumFeatures=1`, `MaxSpikesPerFeature=NumInputDendrite`, `StateGeneration=2`.

Sample: `Bin/Configs/SpikeSamples/StructTrain/TimeNeuronTimeLearner`.

### Связанные документы

- [`NNeuronTimeLearnerBranch`](NNeuronTimeLearnerBranch.md)
- [`NNeuronLearner`](NNeuronLearner.md)
- [`NDatasetMatrix`](NDatasetMatrix.md)
- Эталон wiring: `Bin/Configs/Users/user/TimePatternDetectionTest01`
