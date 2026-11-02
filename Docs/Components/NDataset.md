# NDataset — датасет из файла

## RU

### Назначение

Загрузка ISI-пачек из файла в `MatrixData` layout `(S*M)×F`. База: [`NDatasetBase`](NDatasetBase.md).

### Формат файла

```text
#NDataset;MaxSpikes=3
class;feat0;feat1
0;0.01;0.05
;-1;0.02
;-1;-1
1;0.00;0.10
;0.10;-1
;-1;-1
```

- Мета `#NDataset;MaxSpikes=M` опциональна (без неё `M=1`).
- Header: число `;` = `NumFeatures`.
- Header включает колонку класса: `class;f0;f1` задаёт две фичи. Имена
  колонок позиционные, не сопоставляются по тексту.
- На каждый сэмпл — блок из `M` строк: слот0 = `class;v0;...`, слоты 1..M-1 = `;v0;...` (пустое class-поле).

### Кандидат строгой проверки входа

Числовые поля разбираются целиком через locale-independent from_chars:
class и MaxSpikes — int с проверкой диапазона; ISI — конечный double.
Принимаются десятичная точка, научная запись e/E, один знак, ASCII whitespace,
CRLF, UTF-8 BOM в начале файла и пустые строки между data rows. Поздние слоты
также допускают `v0;v1;...` без начального пустого поля класса.

Число полей должно точно соответствовать header. Пустые числовые значения,
мусор после числа, NaN/Inf, переполнение, лишний trailing delimiter и неизвестная
metadata отклоняются, а не превращаются в0. Отрицательные ISI сохраняют
прежнюю skip-семантику; канонический пропуск-1. MaxSpikes>=1, число строк кратно
MaxSpikes. До allocation проверяется вместимость int-indexed MDMatrix.

Missing file остаётся idle/retryable. Build сообщает распознанную ошибку через
Ready/operation diagnostics; его bool всегдаtrue. На старом parser22 native
cases воспроизвели15 ошибочных принятий; новый код ещё требует серверной
проверки. Это cold file validation scope, не подтверждение hot-reload lifecycle
или классификации; clock/reload исправление выполняется отдельно.

### Favorites

Путь к файлу, `MatrixData`, `MaxSpikesPerFeature`, `Delay`, `Iteration`, `AdvanceSampleAfterBurst`.
