# NDatasetManual — ручной датасет

## RU

### Назначение

**Класс**: `NDatasetManual` — размеры и `MatrixData` в параметрах GUI.  
**База**: [`NDatasetBase`](NDatasetBase.md).

В ctor/`ADefault` в Parameter: `NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixData`, `MatrixClasses`.

- Размерности ресайзят `MatrixData` (`samples × features×maxSpikes`, новые слоты `-1`) и `MatrixClasses` (`1×samples`).
- Default **1×1×1**, слот `0.0` (один спайк в старте сэмпла).
- На Reset матрицы синхронизируются с размерностями.

### Использование

1. Задать `NumSamples` / `NumFeatures` / `MaxSpikesPerFeature`.
2. Заполнить **`MatrixData`** (ISI) и `MatrixClasses`.
3. Reset — генераторы и цикл пачка→`Delay`→повтор.

### Favorites

`NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixData`, `MatrixClasses`, `Delay`, `Iteration`, `PulseGeneratorClassName`, `NumClasses`.

---

## EN

Manual leaf: edit wide ISI `MatrixData`; dims own matrix size; Reset syncs matrices to dims.
