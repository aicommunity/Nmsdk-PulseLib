# NDataset — датасет из файла

## RU

### Назначение

**Класс**: `NDataset` — загрузка ISI-пачек из файла.  
**База**: [`NDatasetBase`](NDatasetBase.md).  
**Sibling**: [`NDatasetMatrix`](NDatasetMatrix.md).

### Формат файла

Разделитель `;`. Опциональная мета-строка `#NDataset;MaxSpikes=N` (без неё `MaxSpikes=1`).

Заголовок: число `;` = `NumFeatures * MaxSpikes`. Строки: `класс` + ISI-слоты (`-1` = пропуск) → **`MatrixData`**.

```text
#NDataset;MaxSpikes=4
class;f0s0;f0s1;f0s2;f0s3;f1s0;f1s1;f1s2;f1s3
0;0.01;0.02;-1;-1;0.05;0.01;0.03;-1
```

Узкий файл без мета (`MaxSpikes=1`) — частный случай того же формата.

### Параметры файла

`FileName`, `UseRelativePathFromConfig`, `UseRelativePathFromWorkDir`, `ReloadDataset` — как раньше (Config/Work/absolute).

### Favorites

ClDesc: `Bin/ClDesc/PulseLibrary/ru-RU/NDataset.xml` — путь, `MatrixData`, `MaxSpikesPerFeature`, `Delay`, `Iteration`.

---

## EN

File-backed leaf: loads wide ISI `MatrixData`, plays burst → Delay → repeat.
