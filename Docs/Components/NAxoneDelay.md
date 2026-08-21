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
| `DelayTime` | 0.001 | Задержка [с]; длина буфера = `int(DelayTime*TimeStep)` |

## Источники

[Literature-References.md](../Literature-References.md): **[14]**, **[25]**; SNM §2.6.

---

## EN

Phenomenological axon delay (default 1 ms). Not registered yet.
