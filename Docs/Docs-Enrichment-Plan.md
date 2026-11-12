# План дополнения описаний компонентов Nmsdk-PulseLib

## RU

Актуальная версия плана по доработке документации компонентов на основе текстовых источников: ВКР Демчевой, ВКР Зарубина. *(Пункт 2.2 по диссертации Бахшиева исключён из плана.)*

**Выполнено (улучшение документации, 2025):** в компонентах добавлены разделы **## Источники** (RU) со ссылкой на [Literature-References.md](Literature-References.md) и идентификаторами ([A], [B], [C], номера публикаций); в README, Usage-Examples и ключевых компонентах добавлены ссылки на `Bin/Configs/SpikeSamples/` (STDP, NM-Neurons, StructTrain, MC-Muscles и др.).

**Track 3 (2026-07-27):** частичные formula stubs добавлены в [`NPulseNeuronCable.md`](Components/NPulseNeuronCable.md) (CSNM) и [`NSynapseTrainerStdpClassicDiscrete.md`](Components/NSynapseTrainerStdpClassicDiscrete.md) (STDP). Полное обогащение остальных файлов — **deferred enrichment backlog** ниже.

---

## 1. Кодировки

Рекомендовано конвертировать или вычитывать вручную фрагменты из файлов с повреждённой кодировкой (при необходимости) по мере работы с источниками.

---

## 2. ВКР Демчевой (кабельные компоненты) — deferred enrichment backlog

Статус: **отложено** (backlog). Контекст CSNM / кабельное уравнение / параметры сегментов (~200 мкм × ~20 мкм) — кратко затронут в `NPulseNeuronCable`; остальные файлы TODO.

Добавить в описания компонентов, связанных с кабельными моделями:

- Контекст **CSNM** (сегментная спайковая модель нейрона на основе кабельной теории), кабельное уравнение \(\lambda^2 \partial^2 V/\partial x^2 = \tau_m \partial V/\partial t + V\), параметры сегментов (200 мкм длина, 20 мкм диаметр).
- Отсылку к сравнению с IaF в соответствующие компоненты.
- Источники: [Literature-References.md](Literature-References.md) ([C], публ. 7), [`Bin/Docs/SpikeSamples/CSNM-Models.md`](../../../Bin/Docs/SpikeSamples/CSNM-Models.md).

**TODO (конкретный список файлов):**

| Файл | Статус |
|------|--------|
| [`Components/NPulseNeuronCable.md`](Components/NPulseNeuronCable.md) | ⚠ partial stub |
| [`Components/NPulseMembraneCable.md`](Components/NPulseMembraneCable.md) | TODO |
| [`Components/NPulseChannelCable.md`](Components/NPulseChannelCable.md) | TODO |
| [`Components/NPulseLTZoneCable.md`](Components/NPulseLTZoneCable.md) | TODO |
| [`Components/NPulseNeuronPosCGeneratorCable.md`](Components/NPulseNeuronPosCGeneratorCable.md) | TODO |
| [`Components/NSynapseCableMulti.md`](Components/NSynapseCableMulti.md) | TODO |
| [`Components/NSynapseCable.md`](Components/NSynapseCable.md) | TODO (связь с CSNM) |

---

## 3. ВКР Зарубина (STDP, варианты обучения) — deferred enrichment backlog

Статус: **отложено** (backlog). Краткие формулы LTP/LTD / отсылки к [B] — в `NSynapseTrainerStdpClassicDiscrete` и частично в других тренерах; полное выравнивание всех вариантов TODO.

Добавить в описания соответствующих классов STDP-тренеров:

- Краткие описания и формулы для вариантов STDP: дискретное и интегральное изменение веса, P-STDP, S-STDP, T-STDP, M-STDP (по тексту ВКР Зарубина).
- Источники: [Literature-References.md](Literature-References.md) ([B]).

**TODO (конкретный список файлов):**

| Файл | Вариант | Статус |
|------|---------|--------|
| [`Components/NSynapseTrainerStdp.md`](Components/NSynapseTrainerStdp.md) | база | TODO (инфра без формул вариантов) |
| [`Components/NSynapseTrainerStdpClassicDiscrete.md`](Components/NSynapseTrainerStdpClassicDiscrete.md) | дискретное | ⚠ partial stub |
| [`Components/NSynapseTrainerStdpClassicIntegrated.md`](Components/NSynapseTrainerStdpClassicIntegrated.md) | интегральное | TODO |
| [`Components/NSynapseTrainerStdpProbabilistic.md`](Components/NSynapseTrainerStdpProbabilistic.md) | P-STDP | TODO |
| [`Components/NSynapseTrainerStdpStable.md`](Components/NSynapseTrainerStdpStable.md) | S-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpTriplet.md`](Components/NSynapseTrainerStdpTriplet.md) | T-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpMirror.md`](Components/NSynapseTrainerStdpMirror.md) | M-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpTD.md`](Components/NSynapseTrainerStdpTD.md) | TD base | TODO |
| [`Components/NSynapseTrainerStdpWD.md`](Components/NSynapseTrainerStdpWD.md) | WD base | TODO |
| [`Components/NSynapseTrainerStdpLobov.md`](Components/NSynapseTrainerStdpLobov.md) | Lobov | TODO |

---

*Источники: локальный [Literature-References.md](Literature-References.md).*

---

## EN

Current plan for improving component documentation based on text sources: Demcheva's thesis, Zarubin's thesis. *(Section 2.2 on Bakhsheev's dissertation has been removed from the plan.)*

**Completed (documentation improvements, 2025):** components now include **## Sources** (RU) sections with a link to [Literature-References.md](Literature-References.md) and identifiers ([A], [B], [C], publication numbers); README, Usage-Examples, and key components include links to `Bin/Configs/SpikeSamples/` (STDP, NM-Neurons, StructTrain, MC-Muscles, etc.).

**Track 3 (2026-07-27):** partial formula stubs added to [`NPulseNeuronCable.md`](Components/NPulseNeuronCable.md) (CSNM) and [`NSynapseTrainerStdpClassicDiscrete.md`](Components/NSynapseTrainerStdpClassicDiscrete.md) (STDP). Full enrichment of remaining files is a **deferred enrichment backlog** below.

---

## 1. Encodings

Recommended: convert or manually read fragments from files with corrupted encoding (as needed) while working with sources.

---

## 2. Demcheva's thesis (cable components) — deferred enrichment backlog

Status: **deferred**. CSNM / cable equation / segment params (~200 µm × ~20 µm) briefly covered in `NPulseNeuronCable`; remaining files TODO.

Add to descriptions of components related to cable models:

- **CSNM** context, cable equation \(\lambda^2 \partial^2 V/\partial x^2 = \tau_m \partial V/\partial t + V\), segment parameters (200 µm length, 20 µm diameter).
- Reference comparison with IaF in the corresponding components.
- Sources: [Literature-References.md](Literature-References.md) ([C], pub. 7), [`Bin/Docs/SpikeSamples/CSNM-Models.md`](../../../Bin/Docs/SpikeSamples/CSNM-Models.md).

**TODO (concrete file list):**

| File | Status |
|------|--------|
| [`Components/NPulseNeuronCable.md`](Components/NPulseNeuronCable.md) | ⚠ partial stub |
| [`Components/NPulseMembraneCable.md`](Components/NPulseMembraneCable.md) | TODO |
| [`Components/NPulseChannelCable.md`](Components/NPulseChannelCable.md) | TODO |
| [`Components/NPulseLTZoneCable.md`](Components/NPulseLTZoneCable.md) | TODO |
| [`Components/NPulseNeuronPosCGeneratorCable.md`](Components/NPulseNeuronPosCGeneratorCable.md) | TODO |
| [`Components/NSynapseCableMulti.md`](Components/NSynapseCableMulti.md) | TODO |
| [`Components/NSynapseCable.md`](Components/NSynapseCable.md) | TODO (CSNM link) |

---

## 3. Zarubin's thesis (STDP, training variants) — deferred enrichment backlog

Status: **deferred**. Short LTP/LTD formulas / refs to [B] in `NSynapseTrainerStdpClassicDiscrete` and partially elsewhere; full alignment of all variants TODO.

Add to descriptions of the corresponding STDP trainer classes:

- Brief descriptions and formulas for STDP variants: discrete and integral weight change, P-STDP, S-STDP, T-STDP, M-STDP (per Zarubin's thesis text).
- Sources: [Literature-References.md](Literature-References.md) ([B]).

**TODO (concrete file list):**

| File | Variant | Status |
|------|---------|--------|
| [`Components/NSynapseTrainerStdp.md`](Components/NSynapseTrainerStdp.md) | base | TODO |
| [`Components/NSynapseTrainerStdpClassicDiscrete.md`](Components/NSynapseTrainerStdpClassicDiscrete.md) | discrete | ⚠ partial stub |
| [`Components/NSynapseTrainerStdpClassicIntegrated.md`](Components/NSynapseTrainerStdpClassicIntegrated.md) | integral | TODO |
| [`Components/NSynapseTrainerStdpProbabilistic.md`](Components/NSynapseTrainerStdpProbabilistic.md) | P-STDP | TODO |
| [`Components/NSynapseTrainerStdpStable.md`](Components/NSynapseTrainerStdpStable.md) | S-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpTriplet.md`](Components/NSynapseTrainerStdpTriplet.md) | T-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpMirror.md`](Components/NSynapseTrainerStdpMirror.md) | M-STDP | ⚠ partial |
| [`Components/NSynapseTrainerStdpTD.md`](Components/NSynapseTrainerStdpTD.md) | TD base | TODO |
| [`Components/NSynapseTrainerStdpWD.md`](Components/NSynapseTrainerStdpWD.md) | WD base | TODO |
| [`Components/NSynapseTrainerStdpLobov.md`](Components/NSynapseTrainerStdpLobov.md) | Lobov | TODO |

---

*Sources: local [Literature-References.md](Literature-References.md).*
