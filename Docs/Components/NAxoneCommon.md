# NAxoneCommon — базовый аксон

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAxoneCommon` — база иерархии аксонов: трансляция сигнала (как с выхода LTZone нейрона) дальше по сети.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAxoneCommon", ...)`.  
**База**: `NSource` (`Output` наследуется).

Порты `Input` / `Output` — `MDMatrix<double>`, по умолчанию 1×1. `ACalculate` по умолчанию: `Output = Input`.

### Свойства

| Свойство | Тип | Описание |
|----------|-----|----------|
| `Input` | MDMatrix\<double\> | Вход (ptInput \| ptPubState) |
| `Output` | MDMatrix\<double\> | Выход (из NSource) |

### См. также

- [`NAxoneDelay`](NAxoneDelay.md), [`NAxoneChain`](NAxoneChain.md)
- [Architecture.md](../Architecture.md) §8a

## Источники

[Literature-References.md](../Literature-References.md): **[25]**, **[26]**, **[29]**, **[A]**.

---

## EN

**Class**: `NAxoneCommon` — axon base class (`Input`/`Output` 1×1). Registered via `UploadClass("NAxoneCommon", ...)`.
