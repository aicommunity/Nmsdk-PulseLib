## RU

## NNeuronTimeLearner — обучение временному паттерну

**Класс**: `NNeuronTimeLearner` — контейнер (`UNet`) с внутренним `NPulseNeuron` и дочерним `NDatasetMatrix`.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTimeLearner", ...)`.
**База кода**: копия/адаптация [`NNeuronLearner`](NNeuronLearner.md).

### Идея

Один генератор датасета подаёт **последовательность из N импульсов** одновременно на дистальные возбуждающие синапсы **всех** дендритов (fan-out), как в конфиге `TimePatternDetectionTest01` (`DatasetMatrix.Generator1` → все дендриты).

- Импульс `k` влияет на рост только дендрита `k` (пик измеряется на `Soma{k}`).
- **Последний** дендрит — опора синхронизации (действующий и калибровочный): его длина не наращивается, к времени его пика подстраиваются дендриты `1..N-1`.
- После синхронизации (с `SyncTolerance`) — нормализация числа синапсов как в `NNeuronLearner`, затем `FixedLTZThreshold`.

### Ключевые параметры

| Свойство | Смысл |
|----------|--------|
| `NumInputDendrite` | Число дендритов = `MaxSpikesPerFeature` датасета |
| `InputPattern` | ISI-матрица `N×1` → `DatasetMatrix.MatrixData` |
| `Delay` | Пауза датасета между пачками |
| `IterationGap` (`t`) | Мин. время после первого импульса прошлой итерации до старта следующей |
| `SyncTolerance` | Допуск одновременности пиков (сек) |
| `TrainingPhase` | 0 Sync / 1 Normalize / 2 Done |
| `TrainingLTZThreshold` / `FixedLTZThreshold` | Порог при обучении / после |
| `AutoCalibrateFixedLTZThreshold` | После Done: выставить `FixedLTZThreshold` по min/max LTZ последней synced-итерации |
| `CalibrateLTZThresholdMode` | 0 = gap_fraction (`min + f*(max-min)`), 1 = peak_fraction (`max*f`) |
| `CalibrateLTZThresholdFraction` | Доля (default 0.85) |
| `CalibratedFixedLTZThreshold` | Результат калибровки (state) |

### Lifecycle

- **ADefault** — defaults (`IterationGap=0.5`, `SyncTolerance=1e-6`, высокий `TrainingLTZThreshold`).
- **ABuild** → `BuildStructure`: `Neuron` + `DatasetMatrix`, fan-out `Generator1` на все дистальные `ExcSynapse`.
- **ACalculate** → амплитуды; при `IsNeedToTrain` — `Training()`.

### Config snippet

```ini
[Component]
ClassName = NNeuronTimeLearner
Name = NeuronTimeLearner1
NumInputDendrite = 4
IterationGap = 0.5
SyncTolerance = 0.000001
```

Внутренний `DatasetMatrix`: `NumFeatures=1`, `MaxSpikesPerFeature=NumInputDendrite`, `StateGeneration=2`.

### Связанные документы

- [`NNeuronLearner`](NNeuronLearner.md)
- [`NDatasetMatrix`](NDatasetMatrix.md)
- Эталон wiring: `Bin/Configs/Users/user/TimePatternDetectionTest01`
