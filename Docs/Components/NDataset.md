# NDataset — датасет из файла

## RU

### Назначение

Загрузка ISI-пачек из файла в `MatrixData` layout `(S*M)×F`. База: [`NDatasetBase`](NDatasetBase.md).

### Формат файла

```text
#NDataset;MaxSpikes=3
feat0;feat1
0;0.01;0.05
;-1;0.02
;-1;-1
1;0.00;0.10
;0.10;-1
;-1;-1
```

- Мета `#NDataset;MaxSpikes=M` опциональна (без неё `M=1`).
- Header: число `;` = `NumFeatures`.
- На каждый сэмпл — блок из `M` строк: слот0 = `class;v0;...`, слоты 1..M-1 = `;v0;...` (пустое class-поле).

### Favorites

Путь к файлу, `MatrixData`, `MaxSpikesPerFeature`, `Delay`, `Iteration`, `AdvanceSampleAfterBurst`.
