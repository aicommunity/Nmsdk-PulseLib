## RU

## LT‑зона в конфигурации TestTrain

### 1. Какие компоненты используются

- **Компоненты обучения**:
  - `NNeuronLearner` (Learner) — использует `NeuronClassName = "NSPNeuronGen"`.
  - `NNeuronTrainer` (Trainer) — в `ADefault()` и в конфиге `TestTrain` также использует `NeuronClassName = "NSPNeuronGen"`.
- **Конфиг TestTrain** (`Bin/Configs/Bakhshiev/TestTrain/Parameters_00.xml`):
  - Внутри `NeuronLearner` и `NeuronTrainer` вложен нейрон:
    - `<Neuron Class="NSPNeuronGen">`.
  - Для этого нейрона явно задано:
    - `<MembraneClassName>NPMembraneBio</MembraneClassName>`
    - `<LTZoneClassName>NPulseLTZoneThreshold</LTZoneClassName>`.
- **Регистрация `NSPNeuronGen` в `NPulseLibrary.cpp`**:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
// Создаем нейроны с упрощенным генератором спайков
n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
n->LTMembraneClassName="";
n->MembraneClassName="NPMembraneBio";
n->LTZoneClassName="NPulseLTZoneThreshold";
n->Build();
n->LTZone->Threshold=0.0117;
UploadClass("NSPNeuronGen",n);
```

**Вывод:** и Learner, и Trainer в конфиге `TestTrain` всегда работают с LT‑зоной класса `NPulseLTZoneThreshold` с исходным порогом `Threshold = 0.0117` (задаётся в `NPulseLibrary.cpp` при регистрации `NSPNeuronGen`).

### 2. Внутренняя реализация LT‑зоны

#### 2.1. Базовая иерархия

Ключевые классы (см. `Core/NPulseLTZone.h` и доки `Docs/Components/NPulseLTZoneCommon.md`, `NPulseLTZoneThreshold.md`):

```mermaid
flowchart TD
  unet[UNet] --> nltz[NLTZone]
  nltz --> ltzCommon[NPulseLTZoneCommon]
  ltzCommon --> ltzThr[NPulseLTZoneThreshold]
  ltzThr --> ltz[NPulseLTZone]
  ltzThr --> ltzBio[NPulseLTZoneThresholdBio]
  ltzThr --> ltzBio2[NPulseLTZoneThresholdBio2]
```

- `NLTZone`:
  - свойства: `Threshold`, `ThresholdOff`, `UseAveragePotential`, `Inputs`, `Output`, `Potential`.
- `NPulseLTZoneCommon`:
  - добавляет импульсные параметры: `NumChannelsInGroup`, `PulseAmplitude`, `PulseLength`, `AvgInterval`;
  - считает выходной потенциал/частоту и времена спайков (`OutputPotential`, `OutputFrequency`, `OutputPulseTimes`).
- `NPulseLTZoneThreshold`:
  - реализует пороговую логику:
    - `CheckPulseOn()` — `Potential >= Threshold`;
    - `CheckPulseOff()` — `Potential < ThresholdOff`;
  - вызывается из `ACalculate2()` после расчёта потенциала в `NPulseLTZoneCommon`.

Таким образом, **все решения о генерации спайка** в нейроне `NSPNeuronGen` принимаются на уровне `NPulseLTZoneThreshold` через сравнение текущего потенциала LT‑зоны с полями `Threshold` и `ThresholdOff`.

#### 2.2. Связь с порогами Learner/Trainer

И `NNeuronLearner`, и `NNeuronTrainer` имеют свойства:

- `LTZThreshold`, `FixedLTZThreshold`, `TrainingLTZThreshold`, `UseFixedLTZThreshold`.

Для Learner:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NNeuronLearner.cpp
bool NNeuronLearner::SetLTZThreshold(const double &value)
{
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>("Neuron",true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
 {
  UseFixedLTZThreshold = false;
 }
 return true;
}
```

Аналогичный код есть в `NNeuronTrainer::SetLTZThreshold`.  
При старте обучения оба компонента устанавливают LT‑зоне **учебный порог** (`TrainingLTZThreshold`), а по окончании — возвращают к **фиксированному** (`FixedLTZThreshold` ≈ 0.0117).

### 3. Регистрация LT‑зон и связанных классов в NPulseLibrary.cpp

Основные регистрации в `CreateClassSamples(UStorage *storage)`:

- **Базовые LT‑зоны**:
  - `UploadClass("NPulseLTZoneCommon", cont);`
  - `UploadClass("NPulseLTZoneThreshold", cont);`
  - `UploadClass("NPulseLTZone", cont);` (расширение с дополнительными параметрами `TimeConstant`, `UseLTZIntegtation`, `UseSpikeStabilizer`).
- **LT‑зоны для кабельной и моделей IaF**:
  - `UploadClass("NPulseLTZoneIzhikevich", cont);`
  - `UploadClass("NPulseLTZoneIaF", cont);`
  - `UploadClass("NPulseLTZoneCable", cont);`
- **Классические/простые LT‑зоны для непрерывных нейронов**:
  - `UploadClass("NPLTZone", cont);`
  - `UploadClass("NCLTZone", cont);`
  - `UploadClass("NPSimpleLTZone", cont);`
  - `UploadClass("NCSimpleLTZone", cont);`

Для **NSPNeuronGen** (и, следовательно, для TestTrain‑конфига) используется именно пара:

- LT‑зона: `LTZoneClassName = "NPulseLTZoneThreshold"`;
- мембрана: `MembraneClassName = "NPMembraneBio"`.

При регистрации `NSPNeuronGen` в `NPulseLibrary.cpp` порог LT‑зоны дополнительно фиксируется на **0.0117**:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
n->LTZoneClassName="NPulseLTZoneThreshold";
n->Build();
n->LTZone->Threshold=0.0117;
UploadClass("NSPNeuronGen",n);
```

Это число затем может временно переопределяться Learner/Trainer’ом через `LTZThreshold`/`TrainingLTZThreshold`, но **базовая «физиологичная» величина порога** именно так и задаётся.

### 4. Git‑история LT‑зоны и её параметров

#### 4.1. История `Core/NPulseLTZone.h` / `Core/NPulseLTZone.cpp`

По журналу:

- Ранние коммиты (`700b7df`, `2069614`, `10ade69` и далее) добавляли структуру LT‑зоны и базовые модели.
- Важные изменения:
  - `4a487cc` — «NPulseLTZone: Threshold sets to 0 by default.» — фиксирует поведение по умолчанию для `Threshold` в самой LT‑зоне (без учёта конфигурационных обёрток).
  - `8802d5d` — добавлен флаг `UseSpikeStabilizer` и связанная логика стабилизации длительности импульса (для `NPulseLTZone`).
  - `ebd855f` — добавлен параметр `ThresholdOff` в `NPulseLTZoneThreshold` (разделение порогов включения/выключения).
  - `7cf11a1` — изменено значение `OutputPotential` (возвращает чистый мембранный потенциал без спайков).
- Коммиты `b6da90e` и `ac5b51a` касаются системы свойств (`UProperty`), но **не меняют сами численные формулы в LT‑зоне**.

Внутри всей видимой истории сабмодуля Nmsdk‑PulseLib **не было изменений, радикально меняющих критерии `CheckPulseOn`/`CheckPulseOff`** — они всегда были завязаны на `Threshold`/`ThresholdOff` и потенциал, рассчитанный в `NPulseLTZoneCommon`.

#### 4.2. История `Core/NPulseLibrary.cpp` (фрагменты LT‑зон и NSPNeuronGen)

По `git log --oneline --reverse -- Core/NPulseLibrary.cpp`:

- Ранние коммиты (`61aaee9`, `6a2ccf8`, `6749781`, `3f18ba5` и др.) добавляли Hebb/STDP‑синапсы, непрерывные нейроны и т.п.
- Появление конфигурационных классов LT‑зоны и био‑нейронов:
  - `b8aad55` — исправления в потенциале нейрона, актуализация параметров.
  - `5f3f06d` — изменение базового сопротивления в `NPulseSynapseCommon` (косвенно влияет на амплитуды, но не на сами LT‑пороги).
  - `98d1edc` — добавлен `NSPNeuronBio2` для кабельной модели (другая конфигурация, не используемая в TestTrain‑конфиге Learner/Trainer).
  - `f44e78c` — миграция библиотек в `Nmsdk-PulseLib`, фиксация текущей организации `NPulseLibrary.cpp`.
- В районе `b6da90e` и `9b44962` происходил рефакторинг свойств и устранение предупреждений; параметры регистрации `NSPNeuronGen` (включая строку `n->LTZone->Threshold=0.0117;`) **остались неизменными**.

**Итог по истории:** с момента введения `NSPNeuronGen` с LT‑зоной `NPulseLTZoneThreshold` и порогом `0.0117` регистрируемые параметры LT‑зоны для этого нейрона **не менялись**. Все видимые изменения касались либо других конфигурационных классов (кабельные, био‑2, Izhikevich/IaF), либо внутренних оптимизаций/рефакторингов.

### 5. Выводы и связь с поведением обучения

1. **И Learner, и Trainer в TestTrain работают на одной и той же LT‑зоне `NPulseLTZoneThreshold` с базовым порогом 0.0117.**
   - Этот порог задаётся конфигурационным классом `NSPNeuronGen` в `NPulseLibrary.cpp` и при необходимости временно переопределяется через свойства `LTZThreshold`/`TrainingLTZThreshold`.
2. **Git‑история не показывает недавних изменений, которые могли бы радикально изменить пороговую механику LT‑зоны для этого нейрона.**
   - Основные формулы и критерии в `NPulseLTZoneThreshold` и связанной обвязке остаются прежними.
3. **Наблюдаемое поведение обучения (отсутствие устойчивого роста дендритов, бесконечный рост числа синапсов)**, судя по истории, связано не с регрессиями в LT‑зоне или её регистрации, а с:
   - выбранными критериями синхронизации/нормализации в `NNeuronLearner`/`NNeuronTrainer`;
   - текущим набором RC‑параметров каналов и синапсов (подробно разобраны в `IonParamsHistory.md`).
4. **Новый отчёт дополняет `IonParamsHistory.md` и `TrainingBehaviorConclusions.md`:**
   - первый концентрируется на численных значениях RC‑параметров и их истории;
   - второй — на алгоритмах роста структуры;
   - настоящий файл фиксирует «мост» между ними: **какая именно LT‑зона стоит в нейроне, какие пороги она использует и какова эволюция этих настроек по истории кода.**

---

## EN

## LT Zone in the TestTrain Configuration

### 1. Which Components Are Used

- **Training components**:
  - `NNeuronLearner` (Learner) — uses `NeuronClassName = "NSPNeuronGen"`.
  - `NNeuronTrainer` (Trainer) — in `ADefault()` and in the `TestTrain` config also uses `NeuronClassName = "NSPNeuronGen"`.
- **TestTrain config** (`Bin/Configs/Bakhshiev/TestTrain/Parameters_00.xml`):
  - Inside `NeuronLearner` and `NeuronTrainer` a neuron is nested:
    - `<Neuron Class="NSPNeuronGen">`.
  - For this neuron the following are explicitly set:
    - `<MembraneClassName>NPMembraneBio</MembraneClassName>`
    - `<LTZoneClassName>NPulseLTZoneThreshold</LTZoneClassName>`.
- **Registration of `NSPNeuronGen` in `NPulseLibrary.cpp`**:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
// Создаем нейроны с упрощенным генератором спайков
n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
n->LTMembraneClassName="";
n->MembraneClassName="NPMembraneBio";
n->LTZoneClassName="NPulseLTZoneThreshold";
n->Build();
n->LTZone->Threshold=0.0117;
UploadClass("NSPNeuronGen",n);
```

**Conclusion:** both Learner and Trainer in the `TestTrain` config always work with an LT zone of class `NPulseLTZoneThreshold` with the initial threshold `Threshold = 0.0117` (set in `NPulseLibrary.cpp` when registering `NSPNeuronGen`).

### 2. Internal Implementation of the LT Zone

#### 2.1. Base Hierarchy

Key classes (see `Core/NPulseLTZone.h` and docs `Docs/Components/NPulseLTZoneCommon.md`, `NPulseLTZoneThreshold.md`):

```mermaid
flowchart TD
  unet[UNet] --> nltz[NLTZone]
  nltz --> ltzCommon[NPulseLTZoneCommon]
  ltzCommon --> ltzThr[NPulseLTZoneThreshold]
  ltzThr --> ltz[NPulseLTZone]
  ltzThr --> ltzBio[NPulseLTZoneThresholdBio]
  ltzThr --> ltzBio2[NPulseLTZoneThresholdBio2]
```

- `NLTZone`:
  - properties: `Threshold`, `ThresholdOff`, `UseAveragePotential`, `Inputs`, `Output`, `Potential`.
- `NPulseLTZoneCommon`:
  - adds pulse parameters: `NumChannelsInGroup`, `PulseAmplitude`, `PulseLength`, `AvgInterval`;
  - computes output potential/frequency and spike times (`OutputPotential`, `OutputFrequency`, `OutputPulseTimes`).
- `NPulseLTZoneThreshold`:
  - implements threshold logic:
    - `CheckPulseOn()` — `Potential >= Threshold`;
    - `CheckPulseOff()` — `Potential < ThresholdOff`;
  - called from `ACalculate2()` after potential calculation in `NPulseLTZoneCommon`.

Thus, **all spike-generation decisions** in the `NSPNeuronGen` neuron are made at the `NPulseLTZoneThreshold` level by comparing the current LT zone potential with the `Threshold` and `ThresholdOff` fields.

#### 2.2. Connection to Learner/Trainer Thresholds

Both `NNeuronLearner` and `NNeuronTrainer` have properties:

- `LTZThreshold`, `FixedLTZThreshold`, `TrainingLTZThreshold`, `UseFixedLTZThreshold`.

For Learner:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NNeuronLearner.cpp
bool NNeuronLearner::SetLTZThreshold(const double &value)
{
 UEPtr<NPulseNeuron> n_in = GetComponentL<NPulseNeuron>("Neuron",true);
 if(!n_in)
  return true;

 UEPtr<NLTZone> ltzone = n_in->GetComponentL<NLTZone>("LTZone");
 if(!ltzone)
  return true;

 ltzone->Threshold = value;
 if(fabs(value - FixedLTZThreshold) > 0.000001)
 {
  UseFixedLTZThreshold = false;
 }
 return true;
}
```

Similar code exists in `NNeuronTrainer::SetLTZThreshold`.  
At the start of training both components set the LT zone to the **training threshold** (`TrainingLTZThreshold`), and at the end they restore the **fixed** one (`FixedLTZThreshold` ≈ 0.0117).

### 3. Registration of LT Zones and Related Classes in NPulseLibrary.cpp

Main registrations in `CreateClassSamples(UStorage *storage)`:

- **Base LT zones**:
  - `UploadClass("NPulseLTZoneCommon", cont);`
  - `UploadClass("NPulseLTZoneThreshold", cont);`
  - `UploadClass("NPulseLTZone", cont);` (extension with additional parameters `TimeConstant`, `UseLTZIntegtation`, `UseSpikeStabilizer`).
- **LT zones for cable and IaF models**:
  - `UploadClass("NPulseLTZoneIzhikevich", cont);`
  - `UploadClass("NPulseLTZoneIaF", cont);`
  - `UploadClass("NPulseLTZoneCable", cont);`
- **Classical/simple LT zones for continuous neurons**:
  - `UploadClass("NPLTZone", cont);`
  - `UploadClass("NCLTZone", cont);`
  - `UploadClass("NPSimpleLTZone", cont);`
  - `UploadClass("NCSimpleLTZone", cont);`

For **NSPNeuronGen** (and therefore for the TestTrain config) the following pair is used:

- LT zone: `LTZoneClassName = "NPulseLTZoneThreshold"`;
- membrane: `MembraneClassName = "NPMembraneBio"`.

When registering `NSPNeuronGen` in `NPulseLibrary.cpp` the LT zone threshold is additionally fixed at **0.0117**:

```12:18:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
n->LTZoneClassName="NPulseLTZoneThreshold";
n->Build();
n->LTZone->Threshold=0.0117;
UploadClass("NSPNeuronGen",n);
```

This value may then be temporarily overridden by Learner/Trainer via `LTZThreshold`/`TrainingLTZThreshold`, but the **baseline "physiological" threshold value** is set this way.

### 4. Git History of the LT Zone and Its Parameters

#### 4.1. History of `Core/NPulseLTZone.h` / `Core/NPulseLTZone.cpp`

From the log:

- Early commits (`700b7df`, `2069614`, `10ade69` and later) added the LT zone structure and base models.
- Important changes:
  - `4a487cc` — "NPulseLTZone: Threshold sets to 0 by default." — fixes the default behavior for `Threshold` in the LT zone itself (without configuration wrappers).
  - `8802d5d` — added the `UseSpikeStabilizer` flag and related pulse-duration stabilization logic (for `NPulseLTZone`).
  - `ebd855f` — added the `ThresholdOff` parameter in `NPulseLTZoneThreshold` (separation of on/off thresholds).
  - `7cf11a1` — changed the `OutputPotential` value (returns pure membrane potential without spikes).
- Commits `b6da90e` and `ac5b51a` concern the property system (`UProperty`) but **do not change the numerical formulas in the LT zone itself**.

Throughout the entire visible history of the Nmsdk-PulseLib submodule **there were no changes that radically altered the `CheckPulseOn`/`CheckPulseOff` criteria** — they have always been tied to `Threshold`/`ThresholdOff` and the potential computed in `NPulseLTZoneCommon`.

#### 4.2. History of `Core/NPulseLibrary.cpp` (LT Zone and NSPNeuronGen Fragments)

From `git log --oneline --reverse -- Core/NPulseLibrary.cpp`:

- Early commits (`61aaee9`, `6a2ccf8`, `6749781`, `3f18ba5`, etc.) added Hebb/STDP synapses, continuous neurons, and so on.
- Appearance of configuration classes for LT zones and bio-neurons:
  - `b8aad55` — fixes in neuron potential, parameter updates.
  - `5f3f06d` — change of base resistance in `NPulseSynapseCommon` (indirectly affects amplitudes, but not LT thresholds themselves).
  - `98d1edc` — added `NSPNeuronBio2` for the cable model (a different configuration, not used in the TestTrain Learner/Trainer config).
  - `f44e78c` — migration of libraries to `Nmsdk-PulseLib`, fixing the current organization of `NPulseLibrary.cpp`.
- Around `b6da90e` and `9b44962` property refactoring and warning cleanup took place; the registration parameters for `NSPNeuronGen` (including the line `n->LTZone->Threshold=0.0117;`) **remained unchanged**.

**History summary:** since the introduction of `NSPNeuronGen` with LT zone `NPulseLTZoneThreshold` and threshold `0.0117`, the registered LT zone parameters for this neuron **have not changed**. All visible changes concerned either other configuration classes (cable, bio-2, Izhikevich/IaF) or internal optimizations/refactorings.

### 5. Conclusions and Connection to Training Behavior

1. **Both Learner and Trainer in TestTrain operate on the same LT zone `NPulseLTZoneThreshold` with a base threshold of 0.0117.**
   - This threshold is set by the configuration class `NSPNeuronGen` in `NPulseLibrary.cpp` and, when needed, is temporarily overridden via the `LTZThreshold`/`TrainingLTZThreshold` properties.
2. **Git history does not show recent changes that could radically alter the threshold mechanics of the LT zone for this neuron.**
   - The main formulas and criteria in `NPulseLTZoneThreshold` and related wrappers remain the same.
3. **Observed training behavior (lack of stable dendrite growth, unbounded synapse count growth)**, judging by history, is not related to regressions in the LT zone or its registration, but rather to:
   - the chosen synchronization/normalization criteria in `NNeuronLearner`/`NNeuronTrainer`;
   - the current set of RC channel and synapse parameters (analyzed in detail in `IonParamsHistory.md`).
4. **This report complements `IonParamsHistory.md` and `TrainingBehaviorConclusions.md`:**
   - the first focuses on numerical RC parameter values and their history;
   - the second — on structure growth algorithms;
   - this file records the "bridge" between them: **which LT zone is in the neuron, which thresholds it uses, and how these settings evolved in the code history.**
