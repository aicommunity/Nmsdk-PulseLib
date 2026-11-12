# Статус документации Nmsdk-PulseLib

## RU

Краткий статус документации библиотеки Nmsdk-PulseLib и чеклист синхронизации с кодом.

---

## EN

## Audit Statistics (2026-06-06)

- Registered classes (`UploadClass`): **177** — see [Component-Gap-Report](../../../Docs/Audit/Component-Gap-Report.md)
- Per-class docs in `Docs/Components/`: see gap report (group docs cover families)

## Completed

### General Documentation
- **Literature-References.md** — literature reference ([A], [B], [C], publication numbers 1–31); link to full list in `Bin/Docs/Literature-References.md`
- **Component-Catalog.md** — component catalog with links to `Components/*.md` and config paths (`Bin/Configs/!OldConfigs/`, `Bin/Configs/User/`, `Bin/Configs/SpikeSamples/`)
- **API-Overview.md** — API description synced with Core headers (NIntegrateAndFireNeuron: C, U0, Upr, I, U; NSynapseStdp: XModCoeff, YModCoeff, APlus, AMinus, XTau, YTau, PsActivityInput, StdpInfluence and inherited Weight, Input, Output)
- **Usage-Examples.md** — usage examples; paths to `Bin/Configs/SpikeSamples/` added

### Components (Docs/Components)
- All component files have RU description and EN section with diagrams and text (no «[Same as RU section]» placeholders)
- Each component RU block ends with **## Sources** linking [Literature-References.md](Literature-References.md) with identifiers ([A], [B], [C], publication numbers); identifiers match **### References** (EN)
- **Component-Documentation-Template.md** — component doc template with required blocks including **## Sources** (RU) and **### References** (EN)

### Configuration Links
- **README.md** («Real usage examples») includes `Bin/Configs/SpikeSamples/`: STDP, NM-Neurons, StructTrain, MC-Muscles, Classifier, Memory, NM-AfferentNeurons
- **Usage-Examples.md** includes SpikeSamples examples (STDP-Simple-01, StructTrain, NM-Neurons, MC-Muscles, SpikeIrisClassifier)
- Key components «Usage» / «Usage in configurations» include `Bin/Configs/SpikeSamples/` paths: NPulseNeuronIzhikevich, NIntegrateAndFireNeuron, NSynapseStdp, NPulseGenerator, NEyeMuscle

### Navigation
- Key components may add «Component catalog: `../Component-Catalog.md`» at description start (relative to `Docs/Components/`, per template and MotionControlLib)

---

## Planned / Recommendations

- Continue using **Component-Documentation-Template.md** and **## Sources** / **### References** sections for new components
- When updating Literature-References, align identifiers with [Bin/Docs/Literature-References.md](../../../Bin/Docs/Literature-References.md)
- See [Docs-Enrichment-Plan.md](Docs-Enrichment-Plan.md) — enrichment plan (CSNM, STDP variants from Demcheva and Zarubin theses)

---

## Code Synchronization Checklist

When adding or changing a class in `Libraries/Nmsdk-PulseLib/Core/`, update:

1. **Component-Catalog.md** — when adding a new library component (registration in `NPulseLibrary.cpp`)
2. **Docs/Components/[ComponentName].md** — when changing properties/methods: property/method tables, class diagrams, code examples
3. **API-Overview.md** — when changing public API of key classes (NIntegrateAndFireNeuron, NSynapseStdp, NPulseNeuronIzhikevich, NPulseGenerator, NEyeMuscle, etc.)
4. **Usage-Examples.md** — for new typical scenarios or config path changes
5. In the component — **## Sources** (RU) and **### References** (EN) when using new publications from Literature-References.md

---

*Sources: PulseLib documentation improvement plan; [CONTRIBUTING-docs.md](CONTRIBUTING-docs.md).*
