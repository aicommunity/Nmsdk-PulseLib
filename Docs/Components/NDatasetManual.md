# NDatasetManual — ручной датасет

## RU

### Назначение

**Класс**: `NDatasetManual` — датасет с ручным заданием `MatrixData` / `MatrixClasses` в параметрах GUI.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NDatasetManual", ...)` (`Default()` без `Build()`).  
**База**: [`NDatasetBase`](NDatasetBase.md).

В отличие от [`NDataset`](NDataset.md), файл не используется. В ctor и `ADefault` вызывается:

```cpp
ChangeLookupPropertyType("MatrixData", ptPubParameter);
ChangeLookupPropertyType("MatrixClasses", ptPubParameter);
```

Размеры (`NumFeatures`, `NumSamples`, `NumGenerators`, `NumClasses`) и `MatrixDelay` остаются State и считаются в `ApplyFromMatrices()` при Build.

### Использование в GUI

1. Создать компонент `NDatasetManual`.
2. Задать `MatrixData` (строки = образцы, столбцы = признаки).
3. Задать `MatrixClasses` (1×NumSamples, число столбцов = число строк `MatrixData`).
4. Build — появятся `Generator1..N` и заполнятся States размеров.

### Ключевые свойства / Favorites

ClDesc: `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

Favorites: `MatrixData`, `MatrixClasses`, `SpikesFrequency`, `Delay`, `Tay`, `Iteration`, `PulseGeneratorClassName`, `NumFeatures`, `NumSamples`, `NumGenerators`, `NumClasses`.

### См. также

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md) — вариант из файла
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)

---

## EN

### Purpose

**Class**: `NDatasetManual` — dataset with editable `MatrixData`/`MatrixClasses` parameters.  
**Registration**: `UploadClass("NDatasetManual", ...)`.  
**Base**: [`NDatasetBase`](NDatasetBase.md).

Uses `ChangeLookupPropertyType` to promote matrices to parameters. Sizes remain derived States.

### Favorites / ClDesc

See `Bin/ClDesc/PulseLibrary/ru-RU/NDatasetManual.xml`.

### See Also

- [`NDatasetBase`](NDatasetBase.md)
- [`NDataset`](NDataset.md)
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)
