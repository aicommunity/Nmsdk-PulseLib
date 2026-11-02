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
- `-1` — канонический пропуск; существующий playback пропускает любое
  отрицательное значение. Не менять эту историческую интерпретацию без
  отдельной версии входного контракта.
- Внутри сэмпла cumsum по строкам слотов **вниз по колонке** фичи.
- `MatrixClasses`: `1 × NumSamples`.
- Активный сэмпл: `Iteration` ∈ `[0, NumSamples)`.

### Воспроизведение

Единый старт сэмпла → one-shot на генераторах → пауза `Delay` → снова пачка.

- `AdvanceSampleAfterBurst=false` (default): повтор того же `Iteration`.
- `AdvanceSampleAfterBurst=true`: следующий `Iteration`; при `LoopSamples=true` — wrap на 0, при `LoopSamples=false` — `StateGeneration=0` и стоп.
- `StateGeneration=0` выключает источник, `1` задаёт timed session,
  `2` воспроизводит непрерывные пачки. Defaults: режим2, `Delay=0`,
  `TimeGeneration=5s`, `Iteration=0`, `ResetDelay=true`, `LoopSamples=true`.
  Delay отсчитывается после завершения последнего pulse, не от его onset.

При единой частоте ancestry и generators управление выполняется в native
BeforeCalculate, дети рассчитываются штатным scheduler. Внутренние integer
microsecond anchors устраняют зависимость относительных времён от origin;
они не являются сохраняемым mid-probe checkpoint. На разных частотах source
сохраняет gated after-children compatibility path. Проверка фактических
callback counts не подтверждает multirate физику для нового learner.
Reset источника начинает новую пачку, а не сбрасывает весь learner.

### Свойства

**Parameters:** `PulseGeneratorClassName`, `Delay`, `Iteration`, `MaxSpikesPerFeature`, `AdvanceSampleAfterBurst`, `LoopSamples`.

**States:** `MatrixData`, `MatrixClasses`, `CurrentClass`, `CurrentClassMatrix`,
`NumFeatures`, `NumSamples`, `NumClasses`, `StateGeneration`, `TimeGeneration`,
`OperatingTime`, `ResetDelay`. `CurrentClass` и `CurrentClassMatrix` одновременно
являются outputs; второй имеет форму1×1. Эти типы относятся к базе и файловому
источнику; NDatasetMatrix продвигает ряд свойств в параметры через native
property registry, несмотря на базовые C++ декларации.

### См. также

- [`NDataset`](NDataset.md)
- [`NDatasetMatrix`](NDatasetMatrix.md)
