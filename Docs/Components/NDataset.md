# NDataset — датасет из файла

## RU

### Назначение

**Класс**: `NDataset` — загрузка датасета из файла и генерация спайковых паттернов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NDataset", ...)` (только `Default()`, без `Build()`).  
**База**: [`NDatasetBase`](NDatasetBase.md).  
**Sibling**: [`NDatasetManual`](NDatasetManual.md) — ручной ввод матриц.

`NDataset` читает файл с метками классов и признаками (или ISI-слотами), заполняет матрицы, затем база выставляет размеры и создаёт по одному `NPulseGeneratorTransit` на признак.

### UML

```mermaid
classDiagram
    UNet <|-- NDatasetBase
    NDatasetBase <|-- NDataset
    NDatasetBase <|-- NDatasetManual
    NDataset *-- NPulseGeneratorTransit : Generators
```

### Свойства

#### Параметры файла (`ptPubParameter`)

| Свойство | Описание |
|----------|----------|
| `FileName` | Путь к файлу (абсолютный или относительный к Config/Work) |
| `UseRelativePathFromConfig` | База `GetCurrentDataDir()` (по умолчанию `true`) |
| `UseRelativePathFromWorkDir` | База `GetSystemDir()`; взаимоисключает Config |
| `ReloadDataset` | Перечитать файл на Build/Calculate; после успеха сбрасывается |
| `MaxSpikesPerFeature` | Перезаписывается из заголовка файла v2 |

Если оба флага относительных путей выключены, `FileName` используется как есть. Логика как у `UMatrixSourceDataFile::CalcActualSourceFilePath`.

Общие параметры генерации и States — см. [`NDatasetBase`](NDatasetBase.md). Матрицы и размеры у file-класса остаются **State** (из файла), кроме `MaxSpikesPerFeature` (Parameter, обновляется из файла).

### Формат файла

Разделитель `;`.

#### v1 (legacy)

Первая строка — заголовок (число `;` = `NumFeatures`). Далее: `класс;признак1;признак2;...`.

```text
class;feat0;feat1;feat2
0;0.1;0.2;0.3
1;0.4;0.5;0.6
```

Заполняется `MatrixData` / `MatrixClasses`; `MaxSpikesPerFeature=1`, `MatrixSpikeDelays` пуста → legacy path.

#### v2 (мультиспайки)

Мета-строка `#NDataset;MaxSpikes=N`, затем заголовок с `NumFeatures*N` колонками, строки данных с ISI и sentinel `-1`:

```text
#NDataset;MaxSpikes=4
class;f0s0;f0s1;f0s2;f0s3;f1s0;f1s1;f1s2;f1s3
0;0.01;0.02;-1;-1;0.05;0.01;0.03;-1
1;0.00;0.10;0.10;-1;-1;-1;-1;-1
```

Число `;` в заголовке должно делиться на `MaxSpikes`. Заполняется `MatrixSpikeDelays`.

### Методы

- `PrepareDataset()` → `TreatDataFromFile()` — загрузка матриц (v1/v2).
- `ACalculate()` — при `ReloadDataset` перечитывает файл, затем `NDatasetBase::ACalculate()`.

### Ключевые свойства / Favorites

ClDesc: `Bin/ClDesc/PulseLibrary/ru-RU/NDataset.xml`.

Favorites: `FileName`, `ReloadDataset`, path-флаги, `SpikesFrequency`, `Delay`, `Tay`, `Iteration`, `MaxSpikesPerFeature`, `MatrixSpikeDelays`, размеры-state.

### См. также

- [`NDatasetBase`](NDatasetBase.md)
- [`NDatasetManual`](NDatasetManual.md)
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)
- [`NClassifier`](NClassifier.md)
- [Architecture.md](../Architecture.md)

### Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**.

---

## EN

### Purpose

**Class**: `NDataset` — file-backed spike dataset (leaf of `NDatasetBase`).  
**Registration**: `UploadClass("NDataset", ...)` without `Build()` at upload.  
**Sibling**: [`NDatasetManual`](NDatasetManual.md) for manual matrices.

Supports legacy CSV and v2 `#NDataset;MaxSpikes=N` wide ISI matrix.

### Path resolution

Same rules as `UMatrixSourceDataFile::CalcActualSourceFilePath` (absolute / Config / Work).

### Favorites / ClDesc

See `Bin/ClDesc/PulseLibrary/ru-RU/NDataset.xml`.

### See Also

- [`NDatasetBase`](NDatasetBase.md)
- [`NDatasetManual`](NDatasetManual.md)
- [`NPulseGeneratorTransit`](NPulseGeneratorTransit.md)
