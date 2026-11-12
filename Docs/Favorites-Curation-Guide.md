# Favorites curation (PulseLibrary)

## RU

Курация `<Favorites>` в `Bin/ClDesc/PulseLibrary/ru-RU/*.xml` по конвенции `Docs/ClDesc-Detailed-Methodology.md`.

### Правила

1. **Direct** — primary params/I/O: `Path` = `{CompName}:Prop` или `Prop`.
2. **Alias** — только полезные nested I/O (`Soma1.ExcSynapse1.Input`, `LTZone.Output`); не `Coord`/`Activity`/`Type`.
3. **Secondary** — `Activity`, `Coord`, `Name`, `Id`, `TimeStep`, duration/debug — только в Properties.
4. Leaf (synapse/channel/membrane/LTZone/generator) — обычно **только direct**, без aliases.
5. Композит-нейрон — directs на структуру/training + 2–4 I/O aliases; удалить autogen cap-20 шум.

### Типовые Favorites по семьям

| Семья | Direct (примеры) |
|-------|------------------|
| Synapse | Weight, Resistance, SecretionTC, DissociationTC, PulseAmplitude, UsePulseSignal, InhibitionCoeff, UsePresynapticInhibition |
| Channel | Capacity, Resistance, FBResistance, RestingResistance, UseAveragePotential, UseAverageSynapsis |
| Membrane | NumExcitatorySynapses, NumInhibitorySynapses, SynapseClassName, Exc/InhChannelClassName, FeedbackGain, UseAveragePotential, ResetAvailable |
| LTZone | Threshold, ThresholdOff, PulseAmplitude, PulseLength, TimeConstant, UseAveragePotential, UseLTZIntegtation, UseSpikeStabilizer |
| Generator | Frequency, Amplitude, PulseLength, Delay, FrequencyDeviation |
| Neuron | StructureBuildMode, NumSomaMembraneParts, NumDendriteMembranePartsVec, *ClassName, UseAverage*Potential |

### См. также

- Component docs в `Components/`
- SpikeSamples под `Bin/Configs/SpikeSamples/`

---

## EN

Curate PulseLibrary ClDesc Favorites: primary direct paths, rare nested I/O aliases, no Coord/Activity/Type noise. See RU table and root `Docs/ClDesc-Detailed-Methodology.md`.
