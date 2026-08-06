# NDatasetManual — ручной датасет

## RU

### Назначение

**Класс**: `NDatasetManual` — датасет с ручным заданием размеров и матриц в параметрах GUI.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NDatasetManual", ...)` (`Default()` без `Build()`).  
**База**: [`NDatasetBase`](NDatasetBase.md).

В отличие от [`NDataset`](NDataset.md), файл не используется. В ctor/`ADefault` свойства `NumSamples`, `NumFeatures`, `MatrixData`, `MatrixClasses` переводятся в `ptPubParameter` через `ChangeLookupPropertyType`.

- **`NumSamples` / `NumFeatures`** — параметры: ресайзят `MatrixData` (samples×features) и `MatrixClasses` (1×samples).
- **`NumClasses`** — State, считается из уникальных меток при Build.
- По умолчанию **1×1**, чтобы Reset сразу создавал `Generator1`.

### Использование в GUI

1. Создать `NDatasetManual` (после Default уже 1 образец × 1 признак).
2. Задать `NumSamples` и `NumFeatures` — матрицы инициализируются нужным размером.
3. Заполнить значения `MatrixData` и метки `MatrixClasses`.
4. **Reset** (или Build) — появятся `Generator1..NumFeatures`.

При ошибке сборки (пустые/несогласованные матрицы) `ABuild` логирует и бросает исключение, чтобы `Ready` не залипал в `true`.

### Ключевые свойства / Favorites

ClDesc: `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

Favorites: `NumSamples`, `NumFeatures`, `MatrixData`, `MatrixClasses`, `SpikesFrequency`, `Delay`, `Tay`, `Iteration`, `PulseGeneratorClassName`, `NumClasses`.

### См. также

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md) — вариант из файла
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)

---

## EN

### Purpose

**Class**: `NDatasetManual` — manual dataset; `NumSamples`/`NumFeatures` resize matrices; fill values; Build creates one generator per feature.  
**Registration**: `UploadClass("NDatasetManual", ...)`.  
**Base**: [`NDatasetBase`](NDatasetBase.md).

Default size is 1×1. `NumClasses` remains a derived State.

### Favorites / ClDesc

See `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

### See Also

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md)
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)
