# NAxoneChainAndDelay — узлы + междоузловые задержки

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAxoneChainAndDelay` — миелиноподобная схема:  
`Segment1 → Delay1 → Segment2 → … → SegmentN`.  
Узел = регенеративный `NAxoneSegment`, междоузлие = `NAxoneDelay` с `InternodeDelayTime`.  
`MembraneClassName` / `LTZoneClassName` / `PosGeneratorClassName` пробрасываются в сегменты.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAxoneChainAndDelay", ...)`.

При `NumNodes==1` — только `Segment1`.

### Свойства

| Свойство | Default | Описание |
|----------|---------|----------|
| `NumNodes` | 1 | Число узлов (≥1) |
| `InternodeDelayTime` | 0.001 | Задержка между узлами [с] |
| `MembraneClassName` | NPMembraneBio | Проброс в Segment* |
| `LTZoneClassName` | NPulseLTZoneThreshold | Проброс в Segment* |
| `PosGeneratorClassName` | NPNeuronPosCGenerator | Проброс в Segment* |

## Источники

[Literature-References.md](../Literature-References.md): **[25]**, **[26]**; SNM §2.6–2.7 (сальтаторная / бездекрементная передача).

---

## EN

Myelin-like axon: regenerative nodes (`NAxoneSegment`) and internode delays (`NAxoneDelay`).
