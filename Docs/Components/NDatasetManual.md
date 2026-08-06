# NDatasetManual — ручной датасет

## RU

### Назначение

**Класс**: `NDatasetManual` — датасет с ручным заданием размеров и матриц в параметрах GUI.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NDatasetManual", ...)` (`Default()` без `Build()`).  
**База**: [`NDatasetBase`](NDatasetBase.md).

В отличие от [`NDataset`](NDataset.md), файл не используется. В ctor/`ADefault` свойства `NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixData`, `MatrixClasses`, `MatrixSpikeDelays` переводятся в `ptPubParameter` через `ChangeLookupPropertyType`.

- **`NumSamples` / `NumFeatures` / `MaxSpikesPerFeature`** — параметры: ресайзят `MatrixData`, `MatrixClasses` и `MatrixSpikeDelays` (`samples × features×maxSpikes`, новые слоты = `-1`).
- **`MatrixSpikeDelays`** — ISI-слоты; sentinel `-1` = пропуск; layout как в [`NDatasetBase`](NDatasetBase.md).
- **`NumClasses`** — State, считается из уникальных меток при Build.
- По умолчанию **1×1×1** с одним слотом `0.0`, чтобы Reset создавал `Generator1` и давал один спайк в начале сэмпла.

### Использование в GUI

1. Создать `NDatasetManual` (после Default уже 1 образец × 1 признак × 1 слот).
2. Задать `NumSamples`, `NumFeatures`, `MaxSpikesPerFeature` — матрицы инициализируются нужным размером.
3. Заполнить `MatrixSpikeDelays` (и при необходимости legacy `MatrixData`) и метки `MatrixClasses`.
4. **Reset** (или Build) — появятся `Generator1..NumFeatures`; в spike-train режиме спайки идут по cumsum ISI.

При ошибке сборки (пустые/несогласованные матрицы) `ABuild` логирует и бросает исключение, чтобы `Ready` не залипал в `true`.

### Ключевые свойства / Favorites

ClDesc: `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

Favorites: `NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixSpikeDelays`, `MatrixData`, `MatrixClasses`, `SpikesFrequency`, `Delay`, `Tay`, `Iteration`, `PulseGeneratorClassName`, `NumClasses`.

### См. также

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md) — вариант из файла
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)

---

## EN

### Purpose

**Class**: `NDatasetManual` — manual dataset; dims resize matrices including wide `MatrixSpikeDelays`; Build creates one generator per feature.  
**Registration**: `UploadClass("NDatasetManual", ...)`.  
**Base**: [`NDatasetBase`](NDatasetBase.md).

Default size is 1×1×1 with spike slot `0.0`. `NumClasses` remains a derived State.

### Favorites / ClDesc

See `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

### See Also

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md)
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)
