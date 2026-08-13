# NDatasetMatrix — датасет из матрицы

## RU

### Назначение

`NumSamples` / `NumFeatures` / `MaxSpikesPerFeature` ресайзят `MatrixData` до `(S*M)×F` и `MatrixClasses` до `1×S`.

Default: `1×1×1`, слот `(0,0)=0.0`.

### Несколько сэмплов

1. Задать `NumSamples=N` → матрица станет `(N*M)×F`.
2. Заполнить блоки строк `0..M-1`, `M..2M-1`, …
3. `Iteration` выбирает сэмпл; при `AdvanceSampleAfterBurst` после `Delay` идёт следующий.
4. `LoopSamples=false` — после последнего сэмпла воспроизведение останавливается; `CurrentClass` — метка активного сэмпла для линков.

### Favorites

`NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixData`, `MatrixClasses`, `Delay`, `Iteration`, `AdvanceSampleAfterBurst`, `LoopSamples`, `CurrentClass`.
