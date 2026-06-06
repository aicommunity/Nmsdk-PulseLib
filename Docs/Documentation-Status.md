# Статус документации Nmsdk-PulseLib

Краткий статус документации библиотеки Nmsdk-PulseLib и чеклист синхронизации с кодом.

---

## Статистика аудита (2026-06-06)

- Зарегистрированных классов (`UploadClass`): **177** — см. [Component-Gap-Report](../../../Docs/Audit/Component-Gap-Report.md)
- Per-class docs в `Docs/Components/`: см. отчёт gap (групповые docs покрывают семейства)

## Выполнено

### Общая документация
- **Literature-References.md** — справочник источников ([A], [B], [C], номера публикаций 1–31); ссылка на полный список в `Bin/Docs/Literature-References.md`
- **Component-Catalog.md** — каталог компонентов со ссылками на `Components/*.md` и путями к конфигам (`Bin/Configs/!OldConfigs/`, `Bin/Configs/User/`, `Bin/Configs/SpikeSamples/`)
- **API-Overview.md** — описание API синхронизировано с реальными заголовками Core (NIntegrateAndFireNeuron: C, U0, Upr, I, U; NSynapseStdp: XModCoeff, YModCoeff, APlus, AMinus, XTau, YTau, PsActivityInput, StdpInfluence и наследованные Weight, Input, Output)
- **Usage-Examples.md** — примеры использования; добавлены пути к `Bin/Configs/SpikeSamples/`

### Компоненты (Docs/Components)
- Во всех файлах компонентов есть RU-описание и EN-секция с диаграммами и текстом (без заглушек «[Same as RU section]»)
- В конце RU-блока каждого компонента добавлена секция **## Источники** со ссылкой на [Literature-References.md](Literature-References.md) и идентификаторами ([A], [B], [C], номера публикаций); идентификаторы взяты из блока **### References** (EN)
- **Component-Documentation-Template.md** — шаблон документа компонента с обязательными блоками, включая **## Источники** (RU) и **### References** (EN)

### Ссылки на конфигурации
- В **README.md** (раздел «Реальные примеры использования») добавлены пункты по `Bin/Configs/SpikeSamples/`: STDP, NM-Neurons, StructTrain, MC-Muscles, Classifier, Memory, NM-AfferentNeurons
- В **Usage-Examples.md** добавлены примеры из SpikeSamples (STDP-Simple-01, StructTrain, NM-Neurons, MC-Muscles, SpikeIrisClassifier)
- В ключевых компонентах в блоке «Использование» / «Использование в конфигурациях» добавлены пути к `Bin/Configs/SpikeSamples/`: NPulseNeuronIzhikevich, NIntegrateAndFireNeuron, NSynapseStdp, NPulseGenerator, NEyeMuscle

### Навигация
- В ключевых компонентах в начале описания можно добавить строку «Каталог компонентов: `../Component-Catalog.md`» (относительно `Docs/Components/`, по образцу шаблона и MotionControlLib)

---

## Планируется / рекомендации

- Продолжать при добавлении новых компонентов использовать **Component-Documentation-Template.md** и секции **## Источники** / **### References**
- При обновлении Literature-References согласовывать идентификаторы с [Bin/Docs/Literature-References.md](../../../Bin/Docs/Literature-References.md)
- См. [Docs-Enrichment-Plan.md](Docs-Enrichment-Plan.md) — план дополнения описаний (CSNM, STDP-варианты по ВКР Демчевой и Зарубина)

---

## Чеклист синхронизации с кодом

При добавлении или изменении класса в `Libraries/Nmsdk-PulseLib/Core/` рекомендуется обновить:

1. **Component-Catalog.md** — при добавлении нового компонента в библиотеку (регистрация в `NPulseLibrary.cpp`)
2. **Docs/Components/[ComponentName].md** — при изменении свойств/методов: таблицы свойств и методов, диаграммы классов, примеры кода
3. **API-Overview.md** — при изменении публичного API ключевых классов (NIntegrateAndFireNeuron, NSynapseStdp, NPulseNeuronIzhikevich, NPulseGenerator, NEyeMuscle и др.)
4. **Usage-Examples.md** — при появлении новых типовых сценариев или изменении путей к конфигам
5. В соответствующем компоненте — блок **## Источники** (RU) и **### References** (EN) при использовании новых публикаций из Literature-References.md

---

*Источники: план улучшения документации PulseLib; [CONTRIBUTING-docs.md](CONTRIBUTING-docs.md).*
