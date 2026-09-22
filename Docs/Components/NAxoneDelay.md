# NAxoneDelay — феноменологическая задержка аксона

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAxoneDelay` — задержка `Input` на `DelayTime` секунд (буфер как у `NPulseDelay`).  
**Не путать** с `NPDelay` / `NPulseDelay`.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAxoneDelay", ...)`.

Default `DelayTime = 0.001` (≈ длительность спайка). Pass-through только при `desired_buffer_length==0` или `DelayTime<=0` (порог 0.001 **не** используется, чтобы 1 мс реально задерживал).

### Свойства

| Свойство | Default | Описание |
|----------|---------|----------|
| `DelayTime` | 0.001 | Задержка [с]; целое N = `int(DelayTime*TimeStep)`; фактическая задержка при N>0 сейчас (N+1) тактов |

## Источники

[Literature-References.md](../Literature-References.md): **[14]**, **[25]**; SNM §2.6.

---

## EN

Registered as `NAxoneDelay` in `NPulseLibrary.cpp`. The current buffer loop delays by N+1 ticks when N=int(DelayTime*TimeStep)>0; TimeStep is a frequency in steps/s. At 2000 steps/s, DelayTime=0.001 produces 1.5 ms, not 1 ms. See the [audit](../../../../Docs/Audit/TimeLearner-2026-09-22/README.md).
