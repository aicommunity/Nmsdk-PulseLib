# NAperiodicLink — апериодическое (инерционное) звено

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAperiodicLink` — звено первого порядка `y += (x-y)/(Tau*TimeStep)`; **`Output = y + Bias`**.  
Обязательно на входе регенеративного `NAxoneSegment` (питает `ExcChannel` вместо константы −1).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAperiodicLink", ...)`.

### Свойства

| Свойство | Default | Описание |
|----------|---------|----------|
| `Tau` | 0.001 | Постоянная времени [с]; `Tau<=0` → фильтр pass-through |
| `Bias` | -1.0 | Смещение к фильтрованному состоянию перед `Output` |
| `Input` / `Output` | 1×1 | Сигналы (`Output = y + Bias`) |

## Источники

[Literature-References.md](../Literature-References.md): **[25]**, **[29]** (инерция ионного механизма как апериодическое звено).

---

## EN

First-order aperiodic link; `Output = filtered_y + Bias` (default Bias=-1) for axon ExcChannel drive.
