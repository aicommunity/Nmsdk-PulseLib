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
| `Tau` | 0.001 | Постоянная времени [с]; `Tau=0` → pass-through; setter отклоняет отрицательные значения |
| `Bias` | -1.0 | Смещение к фильтрованному состоянию перед `Output` |
| `Input` / `Output` | 1×1 | Сигналы (`Output = y + Bias`) |

## Источники

[Literature-References.md](../Literature-References.md): **[25]**, **[29]** (инерция ионного механизма как апериодическое звено).

---

## EN

First-order explicit Euler update; `Output = filtered_y + Bias` (default Bias=-1). TimeStep is a frequency; alpha=1/(Tau*TimeStep). Stability for Tau>0 requires 0<alpha<2, monotone decay requires alpha<=1. The implementation does not check these conditions. For example, Tau=0.001 at 250 steps/s gives alpha=4 and an unstable response. Tau=0 bypasses filtering; the property setter rejects negative values.


**Уточнение владельца, 2026-09-23:** предполагается достаточно малый шаг численного интегрирования. Ограничение устойчивости A12 принято как особенность схемы и отложено; изменение интегратора/guard сейчас не требуется. Указанный пример alpha=4 показывает поведение вне этого предположения.

**Owner clarification, 2026-09-23:** a sufficiently small integration step is assumed. A12 is a deferred numerical limitation, not an active repair requirement; no integrator/guard change is currently planned.
