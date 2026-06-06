# План дополнения описаний компонентов Nmsdk-PulseLib

## RU

Актуальная версия плана по доработке документации компонентов на основе текстовых источников: ВКР Демчевой, ВКР Зарубина. *(Пункт 2.2 по диссертации Бахшиева исключён из плана.)*

**Выполнено (улучшение документации, 2025):** в компонентах добавлены разделы **## Источники** (RU) со ссылкой на [Literature-References.md](Literature-References.md) и идентификаторами ([A], [B], [C], номера публикаций); в README, Usage-Examples и ключевых компонентах добавлены ссылки на `Bin/Configs/SpikeSamples/` (STDP, NM-Neurons, StructTrain, MC-Muscles и др.). План по CSNM (Демчева) и STDP (Зарубин) ниже оставлен как есть.

## 1. Кодировки

Рекомендовано конвертировать или вычитывать вручную фрагменты из файлов с повреждённой кодировкой (при необходимости) по мере работы с источниками.

---

## 2. ВКР Демчевой (кабельные компоненты)

Добавить в описания компонентов, связанных с кабельными моделями:

- Контекст **CSNM** (сегментная спайковая модель нейрона на основе кабельной теории), кабельное уравнение, параметры сегментов (200 мкм длина, 20 мкм диаметр).
- Компоненты: `NPulseNeuronCable`, `NPulseMembraneCable`, `NPulseChannelCable`, `NPulseLTZoneCable`, `NPulseNeuronPosCGeneratorCable`, `NSynapseCableMulti`.
- Отсылку к сравнению с IaF в соответствующие компоненты.

---

## 3. ВКР Зарубина (STDP, варианты обучения)

Добавить в описания соответствующих классов STDP-тренеров:

- Краткие описания и формулы для вариантов STDP: дискретное и интегральное изменение веса, P-STDP, S-STDP, T-STDP, M-STDP (по тексту ВКР Зарубина).

---

*Источники: локальный [Literature-References.md](Literature-References.md).*

---

## EN

Current plan for improving component documentation based on text sources: Demcheva's thesis, Zarubin's thesis. *(Section 2.2 on Bakhsheev's dissertation has been removed from the plan.)*

**Completed (documentation improvements, 2025):** components now include **## Источники** (RU) sections with a link to [Literature-References.md](Literature-References.md) and identifiers ([A], [B], [C], publication numbers); README, Usage-Examples, and key components include links to `Bin/Configs/SpikeSamples/` (STDP, NM-Neurons, StructTrain, MC-Muscles, etc.). The plan for CSNM (Demcheva) and STDP (Zarubin) below is unchanged.

## 1. Encodings

Recommended: convert or manually read fragments from files with corrupted encoding (as needed) while working with sources.

---

## 2. Demcheva's thesis (cable components)

Add to descriptions of components related to cable models:

- **CSNM** context (segmental spiking neuron model based on cable theory), cable equation, segment parameters (200 µm length, 20 µm diameter).
- Components: `NPulseNeuronCable`, `NPulseMembraneCable`, `NPulseChannelCable`, `NPulseLTZoneCable`, `NPulseNeuronPosCGeneratorCable`, `NSynapseCableMulti`.
- Reference comparison with IaF in the corresponding components.

---

## 3. Zarubin's thesis (STDP, training variants)

Add to descriptions of the corresponding STDP trainer classes:

- Brief descriptions and formulas for STDP variants: discrete and integral weight change, P-STDP, S-STDP, T-STDP, M-STDP (per Zarubin's thesis text).

---

*Sources: local [Literature-References.md](Literature-References.md).*
