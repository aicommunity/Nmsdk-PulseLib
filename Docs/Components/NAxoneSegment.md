# NAxoneSegment — регенеративный сегмент аксона

**Каталог:** [Component-Catalog.md](../Component-Catalog.md).

## RU

### Назначение

**Класс**: `NAxoneSegment` — минимальный регенеративный узел (как сома+LTZone нейрона).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NAxoneSegment", ...)` (после `NPMembraneBio` / `NPNeuronPosCGenerator`).

Дети (`BuildStructure` + `AddMissingComponent`):

1. `Inertial` (`NAperiodicLink`, `Bias=-1`) — вход сегмента
2. `PosGenerator` (`NPNeuronPosCGenerator`, Amplitude=+1)
3. `Soma` (`NPMembraneBio`)
4. `LTZone` (`NPulseLTZoneThreshold`, `Threshold=0.03`, `ThresholdOff=0`)

Линки:

- `Inertial.Output` → `Soma.ExcChannel.ChannelInputs` (вместо NegGenerator −1)
- `PosGenerator.Output` → `Soma.InhChannel.ChannelInputs`
- `Soma.Exc/InhChannel.Output` → `LTZone.Inputs`
- `LTZone.Output` → `Soma.InputFeedbackSignal`

Feed-in: `Calculate()` пишет `Input` → `Inertial.Input`. Feed-out: `ACalculate` копирует `LTZone.Output`.

### Свойства

| Свойство | Default | Описание |
|----------|---------|----------|
| `MembraneClassName` | NPMembraneBio | Класс сомы |
| `LTZoneClassName` | NPulseLTZoneThreshold | Класс LTZone |
| `PosGeneratorClassName` | NPNeuronPosCGenerator | Класс +1 генератора |

При `BuildStructure` / `ApplyDiagramLayout` для `NPulseLTZoneThreshold` выставляется `Threshold=0.03`: у минимального сегмента (Inertial→Soma, без синапсов) пик LTZ на первом шаге ~0.025, поэтому порог StructTrain `0.0115` даёт мгновенное срабатывание в такт генератору.

## Источники

[Literature-References.md](../Literature-References.md): **[26]**, **[29]**, **[C]**.

---

## EN

Regenerative axon segment: inertial link (Bias=-1) + soma + PosGenerator(+1) + LTZone feedback, same pattern as minimal `NPulseNeuron`.
