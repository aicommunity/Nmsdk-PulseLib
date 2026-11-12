# NDatasetBase — общая база датасетов

## RU

### Назначение

**Класс**: `NDatasetBase` — абстрактная база датасета спайков.  
**Регистрация**: не в Storage (листья `NDataset`, `NDatasetMatrix`).

### Layout `MatrixData`

```text
rows = NumSamples * MaxSpikesPerFeature   // блоки по M строк на сэмпл
cols = NumFeatures                        // колонка = фича

MatrixData(sample * M + spikeSlot, feature)
```

- Значение `≥ 0` — ISI (сек); `-1` — пропуск слота.
- Внутри сэмпла cumsum по строкам слотов **вниз по колонке** фичи.
- `MatrixClasses`: `1 × NumSamples`.
- Активный сэмпл: `Iteration` ∈ `[0, NumSamples)`.

### Воспроизведение

Единый старт сэмпла → one-shot на генераторах → пауза `Delay` → снова пачка.

- `AdvanceSampleAfterBurst=false` (default): повтор того же `Iteration`.
- `AdvanceSampleAfterBurst=true`: следующий `Iteration`; при `LoopSamples=true` — wrap на 0, при `LoopSamples=false` — `StateGeneration=0` и стоп.

### Свойства

**Parameters:** `PulseGeneratorClassName`, `Delay`, `Iteration`, `MaxSpikesPerFeature`, `AdvanceSampleAfterBurst`, `LoopSamples`.

**States:** `MatrixData`, `MatrixClasses`, `CurrentClass`, `NumFeatures`, `NumSamples`, `NumClasses`, `StateGeneration`, `TimeGeneration`, `OperatingTime`, `ResetDelay`.

### См. также

- [`NDataset`](NDataset.md)
- [`NDatasetMatrix`](NDatasetMatrix.md)
