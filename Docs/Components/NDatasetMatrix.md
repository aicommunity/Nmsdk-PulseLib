# NDatasetMatrix — датасет из матрицы

## RU

### Назначение

`NumSamples` / `NumFeatures` / `MaxSpikesPerFeature` ресайзят `MatrixData` до `(S*M)×F` и `MatrixClasses` до `1×S`.

Default: `1×1×1`, слот `(0,0)=0.0`.

Registry выставляет `NumFeatures`, `NumSamples`, `MatrixData`, `MatrixClasses`
публичными параметрами. Их базовые C++ свойства объявлены states, поэтому
нельзя выводить фактический save/load-контракт только из NDatasetBase.h.
`MaxSpikesPerFeature` и `AdvanceSampleAfterBurst` также остаются параметрами.
Настройку размеров выполнять перед заполнением payload; затем прочитать
эффективные размеры и матрицы. Повторная структурная настройка может менять
layout и не является прозрачным checkpoint сохранённой динамики.

### Несколько сэмплов

1. Задать `NumSamples=N` → матрица станет `(N*M)×F`.
2. Заполнить блоки строк `0..M-1`, `M..2M-1`, …
3. `Iteration` выбирает сэмпл; при `AdvanceSampleAfterBurst` после `Delay` идёт следующий.
4. `LoopSamples=false` — после последнего сэмпла воспроизведение останавливается; `CurrentClass` — метка активного сэмпла для линков.

### Favorites

`NumSamples`, `NumFeatures`, `MaxSpikesPerFeature`, `MatrixData`, `MatrixClasses`, `Delay`, `Iteration`, `AdvanceSampleAfterBurst`, `LoopSamples`, `CurrentClass`.

### Измеренное время и границы проверки

Нативный normal gate20 проверяет76 настоящих source/soma recordings при2000/
20000Hz, continuous/timed, gap0/5/30ms, shifted origins до1e9s, near-grid onset
и cutoff на первом onset. Сравниваются все отсчёты, ширина и повторение pulse.
ASan нового source ещё pending; это не проверка классификации либо всех
поддерживаемых learner profiles. Подробнее: [DatasetClockPlayback](../Analysis/DatasetClockPlayback.ru.md).
