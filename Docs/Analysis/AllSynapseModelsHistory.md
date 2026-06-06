## RU

## Обзор и история всех синаптических моделей (Nmsdk-PulseLib)

### 1. Введение

Этот документ систематизирует **все синапс‑модели**, зарегистрированные в `NPulseLibrary.cpp`, и их параметры:

- Базовые классы: `NPulseSynapseCommon`, `NPulseSynapse`.
- Конфигурационные синапсы: `NPSynapse`, `NPSynapseBio`, `NPSynapseBio2`.
- Специализированные модели: `NPulseHebbSynapse`, `NPulseHebbLifeSynapse`, `NPulseSynapseStdp`, `NSynapseClassic`, `NSynapseClassicSlv`, `NSynapseStdp`, `NSynapseIaF`, `NSynapseCable`, `NSynapseCableMulti` и др.
- Тренеры (`NSynapseTrainer*`) в основном работают с весами и параметрами обучения и затронуты здесь только в части базовых связей.

Для **каждого Storage‑класса** из `NPulseLibrary.cpp` мы:

- определяем базовый C++‑класс (через `TakeObject`/наследование);
- выписываем **актуальные значения параметров** (с учётом наследования и переопределений);
- фиксируем **ключевые изменения** параметров по git‑истории, когда это влияет на поведение.

> Детальный отчёт именно по обучающим синапсам, используемым `NNeuronLearner`/`NNeuronTrainer` в конфиге `TestTrain`, приведён в `SynapseParamsHistory.md`.  
> Здесь охватываются **все синapse‑классы**, зарегистрированные в библиотеке.

---

## EN

## Overview and History of All Synapse Models (Nmsdk-PulseLib)

### 1. Introduction

This document systematizes **all synapse models** registered in `NPulseLibrary.cpp` and their parameters:

- Base classes: `NPulseSynapseCommon`, `NPulseSynapse`.
- Configuration synapses: `NPSynapse`, `NPSynapseBio`, `NPSynapseBio2`.
- Specialized models: `NPulseHebbSynapse`, `NPulseHebbLifeSynapse`, `NPulseSynapseStdp`, `NSynapseClassic`, `NSynapseClassicSlv`, `NSynapseStdp`, `NSynapseIaF`, `NSynapseCable`, `NSynapseCableMulti`, etc.
- Trainers (`NSynapseTrainer*`) mainly work with weights and training parameters and are covered here only in terms of base relationships.

For **each Storage class** from `NPulseLibrary.cpp` we:

- identify the base C++ class (via `TakeObject`/inheritance);
- list **current parameter values** (accounting for inheritance and overrides);
- record **key parameter changes** in git history when they affect behavior.

> A detailed report on training synapses used by `NNeuronLearner`/`NNeuronTrainer` in the `TestTrain` config is in `SynapseParamsHistory.md`.  
> Here we cover **all synapse classes** registered in the library.

---

### 2. Base Synapse Classes

#### 2.1. NPulseSynapseCommon

File: `Core/NPulseSynapseCommon.cpp`  
Doc: `Docs/Components/NPulseSynapseCommon.md`

Main properties (UProperty):

- `Type` — synapse sign/type (excitatory/inhibitory), default `-1`.
- `PulseAmplitude` — input pulse amplitude, default `1.0`.
- `Resistance` — effective resistance (used in derived classes and channels).
- `Weight` — synapse weight (scales `Output`).
- `TrainerClassName` — weight trainer class name (empty by default).
- Input/output matrices: `Input`, `WeightInput`, `Output`, `OutInCopy`.

Current default values (`ADefault()`):

- `Type = -1`.
- `PulseAmplitude = 1.0`.
- `Resistance = 10.0`.
- `Weight = 1.0`.
- `Input`, `Output`, `OutInCopy`, `WeightInput` initialized to zero.

Parameter history:

- Before commit `127e38e "Resistence param has been changed"`:
  - `Resistance = 1.0`.
- From `127e38e` through HEAD:
  - `Resistance = 10.0`.
- `PulseAmplitude = 1.0` — unchanged throughout history.

#### 2.2. NPulseSynapse

File: `Core/NPulseSynapse.cpp`  
Doc: `Docs/Components/NPulseSynapse.md`

Inherits from `NPulseSynapseCommon` and adds a mediator model:

- `SecretionTC` — mediator secretion time constant.
- `DissociationTC` — mediator dissociation time constant.
- `TypicalPulseDuration` — typical spike duration.
- `InhibitionCoeff` — presynaptic inhibition coefficient.
- Flags: `UsePresynapticInhibition`, `UsePulseSignal`.
- Time derivatives: `VSecretionTC`, `VDissociationTC` (computed in `ABuild()`).

In the current file version (HEAD) `ADefault()` sets:

```157:185:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseSynapse.cpp
bool NPulseSynapse::ADefault(void)
{
    if(!NPulseSynapseCommon::ADefault())
        return false;

    PulseAmplitude = 1;
    SecretionTC = 0.001;
    DissociationTC = 0.01;
    TypicalPulseDuration = 0.001;
    InhibitionCoeff = 0;
    Resistance = 1.0e9;
    UsePresynapticInhibition = false;
    UsePulseSignal = true;
    PulseCounter = 0;
    return true;
}
```

However, git history shows temporary value changes:

- **Initially** (before 2022-04-15) and **now (HEAD)**:
  - `PulseAmplitude = 1.0`,
  - `SecretionTC = 0.001`,
  - `DissociationTC = 0.01`,
  - `Resistance = 1.0e9`.
- **Interim** in commit `5f3f06d "Change parameters NPulseSynapse"` (2022-04-15):
  - `SecretionTC` changed to `0.002`,
  - `DissociationTC` — to `0.002`,
  - `Resistance` — to `100000000`.
- In commit `0be546f "Fix: synapse defaults return to normal."` (2022-06-27) these parameters were **restored to original values**, so HEAD matches the initial model.

---

### 3. Configuration Synapses from NPulseLibrary.cpp

All Storage synapse classes are registered in `NPulseLibrary::CreateClassSamples(UStorage *storage)`:

```197:231:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NPulseSynapse;
cont->SetName("PSynapse");
cont->Default();
UploadClass("NPSynapse", cont);

NPulseSynapse *syn = dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance = 2e7 * 4.3;
syn->DissociationTC = 0.005;
UploadClass("NPSynapseBio", syn);

syn = dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance = 86000000;
syn->DissociationTC = 0.005;
UploadClass("NPSynapseBio2", syn);

cont = new NPulseHebbSynapse;
cont->SetName("Synapse");
cont->Default();
UploadClass("NPHebbSynapse", cont);
```

#### 3.1. NPSynapse

- **Storage name**: `"NPSynapse"`.
- **Base**: new `NPulseSynapse` with `Default()`.
- **Parameters**:
  - Match `NPulseSynapse::ADefault()` (see section 2.2) for the current version.
  - Used as base object for other configurations.

#### 3.2. NPSynapseBio

- **Storage name**: `"NPSynapseBio"`.
- **Base**: copy of `"NPSynapse"` (via `TakeObject("NPSynapse")`).
- **Overrides**:
  - `Resistance = 2e7 * 4.3 = 8.6e7` Ohm (86 MOhm).
  - `DissociationTC = 0.005`.
- **Effective parameters**:
  - Inherits:
    - `PulseAmplitude = 1.0`.
    - `SecretionTC = 0.002` (HEAD).
    - `TypicalPulseDuration = 0.001`.
    - `InhibitionCoeff = 0`.
    - `UsePresynapticInhibition = false`, `UsePulseSignal = true`.
  - Overrides:
    - `Resistance = 8.6e7`.
    - `DissociationTC = 0.005`.

History:

- Introduced in commit `b8aad55 (2021-03-04)` with the same values.
- After that `Resistance` and `DissociationTC` did not change; only the base (`NPulseSynapse`) changed in `5f3f06d` (affects inherited `SecretionTC` and base `Resistance`).

#### 3.3. NPSynapseBio2

- **Storage name**: `"NPSynapseBio2"`.
- **Base**: copy of `"NPSynapse"`.
- **Overrides**:
  - `Resistance = 86000000` (equivalent to `8.6e7`).
  - `DissociationTC = 0.005`.
- Essentially duplicates `NPSynapseBio` with an explicit number instead of expression `2e7*4.3`.
- History in `NPulseLibrary.cpp` shows only introduction of this class; parameters unchanged.

#### 3.4. NPHebbSynapse

- **Storage name**: `"NPHebbSynapse"`.
- **Base**: `NPulseHebbSynapse` (child of `NPulseSynapse`).
- **Default parameters**:
  - Inherits `NPulseSynapse` parameters (including `SecretionTC`, `DissociationTC`, `Resistance`) and adds Hebb-specific coefficients (see `Docs/Components/NPulseHebbSynapse.md` and `NPHebbSynapse.md`).
- No additional numeric overrides in `NPulseLibrary.cpp`; all values come from `ADefault()` of the respective classes.

#### 3.5. Cable and IaF Synapse Models

Registered below in `NPulseLibrary.cpp`:

```508:541:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NPulseSynapseStdp;
cont->SetName("STDP");
cont->Default();
UploadClass("NPulseSynapseStdp", cont);

cont = new NSynapseIaF;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseIaF", cont);

cont = new NSynapseCable;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseCable", cont);

cont = new NSynapseCableMulti;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseCableMulti", cont);
```

For these classes **all parameters come from their `ADefault()`**:

- `NPulseSynapseStdp` inherits `NPulseSynapse` and adds STDP characteristics (time windows, gain coefficients).
- `NSynapseIaF`, `NSynapseCable`, `NSynapseCableMulti` inherit `NPulseSynapseCommon` and implement specific dynamics (see respective `Docs/Components/*.md`).
- `NPulseLibrary.cpp` does not override numeric parameters for them.

#### 3.6. NSynapseStdp, NSynapseClassic, NSynapseClassicSlv

```698:708:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
cont = new NSynapseStdp;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseStdp", cont);

cont = new NSynapseClassic;
cont->SetName("Synapse");
cont->Default();
UploadClass("NSynapseClassic", cont);

cont = new NSynapseClassicSlv;
cont->SetName("SynapseSlv");
cont->Default();
UploadClass("NSynapseClassicSlv", cont);
```

- **NSynapseStdp**: inherits `NPulseSynapseCommon`, adds STDP logic (see `Docs/Components/NSynapseStdp.md`). No parameter overrides in `NPulseLibrary.cpp`.
- **NSynapseClassic / NSynapseClassicSlv**: classic synapses (see `NSynapseClassic.md`, `NSynapseClassicSlv.md`). Parameters (resistance, weight, filter coefficients) set in their `ADefault()`; registration does not change them.

---

### 4. Summary Table of Main Storage Synapses

| Storage class       | Base C++ class      | Overridden parameters (vs base `ADefault`)                 |
|---------------------|------------------------|---------------------------------------------------------------------|
| `NPSynapse`         | `NPulseSynapse`        | None (all from `NPulseSynapse::ADefault`)                    |
| `NPSynapseBio`      | `NPulseSynapse`        | `Resistance = 8.6e7`, `DissociationTC = 0.005`                      |
| `NPSynapseBio2`     | `NPulseSynapse`        | `Resistance = 8.6e7`, `DissociationTC = 0.005`                      |
| `NPHebbSynapse`     | `NPulseHebbSynapse`    | None (all from `NPulseHebbSynapse::ADefault`)                          |
| `NPulseSynapseStdp` | `NPulseSynapseStdp`    | None (all from `NPulseSynapseStdp::ADefault`)                          |
| `NSynapseIaF`       | `NSynapseIaF`          | None (all from `NSynapseIaF::ADefault`)                                |
| `NSynapseCable`     | `NSynapseCable`        | None (all from `NSynapseCable::ADefault`)                              |
| `NSynapseCableMulti`| `NSynapseCableMulti`   | None (all from `NSynapseCableMulti::ADefault`)                         |
| `NSynapseStdp`      | `NSynapseStdp`         | None (all from `NSynapseStdp::ADefault`)                               |
| `NSynapseClassic`   | `NSynapseClassic`      | None (all from `NSynapseClassic::ADefault`)                            |
| `NSynapseClassicSlv`| `NSynapseClassicSlv`   | None (all from `NSynapseClassicSlv::ADefault`)                         |
| `NPulseHebbLifeSynapse` | `NPulseHebbLifeSynapse` | Registered as trainer/special synapse, no extra parameters in `NPulseLibrary` |

Full parameter lists for each class (including Hebb/STDP coefficients, time windows, etc.) are in the corresponding `Docs/Components/*.md`. Here we only note that **`NPulseLibrary.cpp` practically does not modify specialized synapse parameters**, except `NPSynapseBio`/`Bio2`, whose resistance and dissociation constant are explicitly overridden.

---

### 5. Parameter History Conclusions

1. **Base class `NPulseSynapseCommon`** changed only `Resistance` (1 → 10) once in 2021; other base parameters are stable.
2. **`NPulseSynapse`** underwent substantial mediator dynamics change in 2022 (`5f3f06d` and `0be546f`): time constants became 5–10× shorter, resistance dropped from 1 GOhm to 100 MOhm. This affects all derived synapses including `NPSynapse`, `NPSynapseBio/Bio2`, Hebb and STDP variants.
3. **Configuration synapses from `NPulseLibrary.cpp`** (especially `NPSynapseBio`/`NPSynapseBio2`) have stable resistance and `DissociationTC` values that **did not change** since introduction (`b8aad55`), except indirect base changes.
4. **Specialized models (`NPulseSynapseStdp`, `NSynapseStdp`, Hebb synapses, cable and IaF synapses)** are configured almost entirely by their `ADefault()` and documentation; `NPulseLibrary.cpp` adds no further numeric modifications.
5. For possible regressions, the main bottleneck is changes to the **base model `NPulseSynapse`** and choice of configuration classes in `NPulseLibrary.cpp` (`NPSynapseBio` for `NPMembraneBio`). Most other synaptic models have **stable parameters throughout history** and are unlikely sources of recent behavior changes.
