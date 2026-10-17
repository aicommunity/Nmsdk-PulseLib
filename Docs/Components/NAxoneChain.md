# NAxoneChain — цепочка сегментов аксона

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAxoneChain` — структурная длина аксона = `NumSegments` детей `Segment1…N`.  
`MembraneClassName`, `LTZoneClassName`, `PosGeneratorClassName` пробрасываются в каждый сегмент.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAxoneChain", ...)`.

Связи: `CreateLink(Seg_i.Output → Seg_{i+1}.Input)`.  
API: `Elongate()`, `Shorten()`.

### Свойства

| Свойство | Default | Описание |
|----------|---------|----------|
| `NumSegments` | 1 | Число сегментов (≥1) |
| `MembraneClassName` | NPMembraneBio | Проброс в сегменты |
| `LTZoneClassName` | NPulseLTZoneThreshold | Проброс в сегменты |
| `PosGeneratorClassName` | NPNeuronPosCGenerator | Проброс в сегменты |

## Источники

[Literature-References.md](../Literature-References.md): **[26]**, **[11]**, **[4]**, **[A]**, **[C]** (структурная адаптация / цепочка участков).

---

## EN

Structural axon as a chain of regenerative `NAxoneSegment`; length = `NumSegments`.
