## RU

## Отчёт по параметрам мембран (NPulseMembrane) для обучающих конфигов TestTrain

### 1. Введение

Этот документ дополняет `IonParamsHistory.md`, `SynapseParamsHistory.md` и `LTZoneUsageHistory.md` и фокусируется на **мембранных моделях**, которые используются в обучающих нейронах:

- `NNeuronLearner` и `NNeuronTrainer` в конфиге `TestTrain` (`Bin/Configs/Bakhshiev/TestTrain`).
- Внутренний нейрон: `NSPNeuronGen` (`NeuronClassName = "NSPNeuronGen"`).
- Мембрана: `NPMembraneBio` (биоинспирированная импульсная мембрана).

Цели отчёта:

- зафиксировать базовые параметры `NPulseMembrane` и их конфиг‑варианты из `NPulseLibrary.cpp`;
- проследить по git‑истории, изменялись ли численные параметры этих мембран;
- оценить, могли ли изменения мембран повлиять на наблюдаемое обучение (рост дендритов и числа синапсов).

---

## EN

## Membrane Parameter Report (NPulseMembrane) for TestTrain Training Configs

### 1. Introduction

This document supplements `IonParamsHistory.md`, `SynapseParamsHistory.md`, and `LTZoneUsageHistory.md` and focuses on **membrane models** used in training neurons:

- `NNeuronLearner` and `NNeuronTrainer` in the `TestTrain` config (`Bin/Configs/Bakhshiev/TestTrain`).
- Internal neuron: `NSPNeuronGen` (`NeuronClassName = "NSPNeuronGen"`).
- Membrane: `NPMembraneBio` (bio-inspired pulse membrane).

Report goals:

- record base `NPulseMembrane` parameters and config variants from `NPulseLibrary.cpp`;
- trace whether numeric membrane parameters changed in git history;
- assess whether membrane changes could affect observed training (dendrite length and synapse count growth).

---

### 2. Membrane Classes Used in TestTrain

#### 2.1. Learner/Trainer → NSPNeuronGen → NPMembraneBio Chain

- In `NNeuronLearner` and `NNeuronTrainer` (files `Core/NNeuronLearner.cpp`, `Core/NNeuronTrainer.cpp`) defaults set:
  - `NeuronClassName = "NSPNeuronGen"`.
- In `NSPNeuronGen` (see `Docs/Components/NSPNeuronGen.md`):
  - `MembraneClassName = "NPMembraneBio"`;
  - `LTZoneClassName = "NPulseLTZoneThreshold"`;
  - `LTMembraneClassName = ""`.
- In `Model_00.xml` for TestTrain:
  - neuron and all dendrites/soma explicitly use `Class="NPMembraneBio"`.

Thus the chains are:

- `NNeuronLearner` → `NSPNeuronGen` → `NPMembraneBio`,
- `NNeuronTrainer` → `NSPNeuronGen` → `NPMembraneBio`.

Other membrane configurations (`NPMembraneBio2`, LT membranes, etc.) are not used directly in TestTrain.

---

### 3. Base Membranes: NPulseMembraneCommon and NPulseMembrane

#### 3.1. NPulseMembraneCommon

Files: `Core/NPulseMembraneCommon.h`, `Core/NPulseMembraneCommon.cpp`.

Main responsibilities:

- store channel list (`Channels`) and synapses (`Synapses`);
- accumulate total potential `SumPotential`;
- track neuron activity (`IsNeuronActive`) via `Feedback` from LTZone.

Key implementation points (current version):

- In `ACalculate()` after `ACalculate2()`:
  - if `Feedback > 0` and neuron was inactive — set `IsNeuronActive = true` and call `NeuronActivated()` on all channels;
  - if `IsNeuronActive` and `Feedback <= 0` — reset `IsNeuronActive` to `false`;
  - then sum potentials of all channels:

```185:203:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseMembraneCommon.cpp
 if(IsNeuronActive && Feedback<=0)
  IsNeuronActive=false;
 SumPotential(0,0)=0;
 for(size_t i=0;i<Channels.size();i++)
  if(Channels[i])
   SumPotential(0,0)+=Channels[i]->Output(0,0);
 return true;
```

Git history:

- early commits (`700b7df`, `66d105a`) added base functionality and common classes;
- in `b8aad55` only reformatting of the summation block (see diff above), **numeric logic unchanged**;
- later commits changed code structure, not membrane numeric parameters.

Conclusion: `NPulseMembraneCommon` has no own numeric RC parameters (they are in channels), and values **did not change** in history.

#### 3.2. NPulseMembrane

Files: `Core/NPulseMembrane.h`, `Core/NPulseMembrane.cpp`.

Adds on top of `NPulseMembraneCommon`:

- parameters:
  - `FeedbackGain` — LTZone feedback gain;
  - `ResetAvailable` — reset mechanism availability flag;
  - `SynapseClassName` — synapse class name;
  - `ExcChannelClassName`, `InhChannelClassName` — excitatory and inhibitory channel class names;
  - `NumExcitatorySynapses`, `NumInhibitorySynapses` — number of synapses to create.

Default values (HEAD) in `NPulseMembrane::ADefault()`:

```308:318:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseMembrane.cpp
bool NPulseMembrane::ADefault(void)
{
 if(!NPulseMembraneCommon::ADefault())
  return false;
 FeedbackGain=2;
 ResetAvailable=true;
 SynapseClassName="NPSynapse";
 ExcChannelClassName="NPExcChannel";
 InhChannelClassName="NPInhChannel";
 NumExcitatorySynapses=1;
 NumInhibitorySynapses=1;

 return true;
}
```

Git history (`git log -- Core/NPulseMembrane.cpp`):

- numeric values `FeedbackGain=2`, `NumExcitatorySynapses=1`, `NumInhibitorySynapses=1` present from earliest versions and **unchanged**;
- historical changes affected:
  - bug fixes in part build/removal (`0330359`, `6943c32`, etc.);
  - logging and checks;
  - moving initialization to `ADefault` without changing numbers.

Thus base membrane `NPulseMembrane` has **stable numeric parameters** throughout available history.

---

### 4. Configuration Membranes from NPulseLibrary.cpp

File: `Core/NPulseLibrary.cpp`, function `NPulseLibrary::CreateClassSamples(UStorage *storage)`.

Fragment registering membranes (simplified):

```340:369:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
UEPtr<NPulseMembrane> membr;
cont=new NPulseMembrane;
cont->SetName("PMembrane");
cont->Default();
UploadClass("NPMembrane",cont);

// Регистрация NPNeuronMembrane как конфигурационного варианта NPMembrane
membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->Default();
UploadClass("NPNeuronMembrane",membr);

// Регистрация NPNewNeuronMembrane как конфигурационного варианта NPMembrane
membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->Default();
UploadClass("NPNewNeuronMembrane",membr);

membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->ExcChannelClassName="NPExcChannelBio";
membr->SynapseClassName="NPSynapseBio";
membr->InhChannelClassName="NPInhChannelBio";
UploadClass("NPMembraneBio",membr);

membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->ExcChannelClassName="NPExcChannelBio2";
membr->SynapseClassName="NPSynapseBio2";
membr->InhChannelClassName="NPInhChannelBio2";
membr->FeedbackGain = 0.02;
UploadClass("NPMembraneBio2",membr);

// Участки мембраны низкопороговой зоны и др. (NPLTZoneNeuronMembrane, NPSynNeuronMembrane, NCSynNeuronMembrane и т.д.)
...
```

#### 4.1. NPMembrane (base Storage membrane)

- **Storage name**: `"NPMembrane"`.
- **Base**: new `NPulseMembrane` with `Default()`.
- **Parameters** (all from `NPulseMembrane::ADefault()`):
  - `FeedbackGain = 2.0`;
  - `ResetAvailable = true`;
  - `SynapseClassName = "NPSynapse"`;
  - `ExcChannelClassName = "NPExcChannel"`;
  - `InhChannelClassName = "NPInhChannel"`;
  - `NumExcitatorySynapses = 1`;
  - `NumInhibitorySynapses = 1`.

History: from first `NPulseLibrary.cpp` versions these values are **not overridden** and unchanged throughout history.

#### 4.2. NPNeuronMembrane / NPNewNeuronMembrane

- **Storage names**: `"NPNeuronMembrane"`, `"NPNewNeuronMembrane"`.
- **Base**: copies of `"NPMembrane"`:
  - `TakeObject("NPMembrane")` and `Default()` without changing numbers.

Summary: numeric parameters match `NPMembrane`, used for backward compatibility/new neurons, but **no different RC parameters**.

#### 4.3. NPMembraneBio (used in TestTrain)

- **Storage name**: `"NPMembraneBio"`.
- **Base**: copy of `"NPMembrane"`.
- **Overrides in `NPulseLibrary.cpp`**:
  - `ExcChannelClassName = "NPExcChannelBio"`;
  - `InhChannelClassName = "NPInhChannelBio"`;
  - `SynapseClassName = "NPSynapseBio"`.
- **Inherited parameters**:
  - `FeedbackGain = 2.0` (unchanged);
  - `ResetAvailable = true`;
  - `NumExcitatorySynapses = 1`;
  - `NumInhibitorySynapses = 1`.

History (via `git log` and `git show b8aad55`):

- Class `NPMembraneBio` introduced in commit `b8aad55 (2021-03-04)` **with these same values**;
- in all subsequent commits the `NPMembraneBio` registration block did not change — neither `FeedbackGain` nor channel/synapse classes for this membrane were touched.

Thus **the main membrane used in TestTrain training configs has stable parameters since introduction**.

#### 4.4. NPMembraneBio2 (alternative bio membrane)

- **Storage name**: `"NPMembraneBio2"`.
- **Base**: copy of `"NPMembrane"`.
- **Overrides**:
  - `ExcChannelClassName = "NPExcChannelBio2"`;
  - `InhChannelClassName = "NPInhChannelBio2"`;
  - `SynapseClassName = "NPSynapseBio2"`;
  - `FeedbackGain = 0.02`.

This membrane targets alternative models (e.g. cable-compatible) and is **not used** in current TestTrain, but is important as an example:

- reduced `FeedbackGain` strongly weakens LTZone feedback on the membrane;
- different RC channel parameters (`NPExcChannelBio2`/`NPInhChannelBio2`) change temporal dynamics.

Git history: `NPMembraneBio2` block added later (commits `98d1edc`/`f44e78c`) and **unchanged** since.

#### 4.5. LT and Syn Membranes

Other registered membranes in `NPulseLibrary.cpp`:

- `NPLTZoneNeuronMembrane` — LT membrane segments, uses `NPLTExcChannel`/`NPLTInhChannel` (channels with `Capacity = 1e-8`, `RestingResistance = 1e6`).
- `NPLTZoneSynNeuronMembrane` — LT segments for synaptic channels (`NPLTSynExcChannel`/`NPLTSynInhChannel`). 
- `NPSynNeuronMembrane` — membrane with `NPSynExcChannel`/`NPSynInhChannel`.
- `NCSynNeuronMembrane` — continuous membrane with `NCSynExcChannel`/`NCSynInhChannel`.
- `NPNeuronHebbMembrane` — membrane with `SynapseClassName = "NPHebbSynapse"`.

In TestTrain config `NSPNeuronGen` and related neurons **do not use** these membranes directly; they matter for other scenarios (LT zones, Hebb training, cable model).
Per `NPulseLibrary.cpp` history their numeric parameters (channel RC characteristics and `FeedbackGain` for `NPMembraneBio2`) did not change after introduction.

---

### 5. Membrane Parameter Summary Table

| Storage class        | Base class       | FeedbackGain | ResetAvailable | ExcChannelClassName   | InhChannelClassName    | SynapseClassName  | Notes                          |
|----------------------|---------------------|--------------|----------------|------------------------|------------------------|-------------------|-------------------------------------|
| `NPMembrane`         | `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | base membrane                    |
| `NPNeuronMembrane`   | `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | copy of `NPMembrane`                  |
| `NPNewNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NPExcChannel`        | `NPInhChannel`        | `NPSynapse`       | copy of `NPMembrane`                  |
| `NPMembraneBio`      | `NPulseMembrane`    | 2.0          | true           | `NPExcChannelBio`     | `NPInhChannelBio`     | `NPSynapseBio`    | **used in TestTrain**        |
| `NPMembraneBio2`     | `NPulseMembrane`    | 0.02         | true           | `NPExcChannelBio2`    | `NPInhChannelBio2`    | `NPSynapseBio2`   | alternative bio membrane          |
| `NPLTZoneNeuronMembrane` | `NPulseMembrane`| 2.0 (inherited) | true        | `NPLTExcChannel`      | `NPLTInhChannel`      | `NPSynapse`       | LT membrane segments                  |
| `NPLTZoneSynNeuronMembrane` | `NPulseMembrane` | 2.0       | true           | `NPLTSynExcChannel`   | `NPLTSynInhChannel`   | `NPSynapse`       | LT segments with syn channels       |
| `NPSynNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NPSynExcChannel`     | `NPSynInhChannel`     | `NPSynapse`       | membrane with syn channels            |
| `NCSynNeuronMembrane`| `NPulseMembrane`    | 2.0          | true           | `NCSynExcChannel`     | `NCSynInhChannel`     | `NPSynapse`       | continuous syn membrane           |
| `NPNeuronHebbMembrane`| `NPulseMembrane`   | 2.0          | true           | `NPExcChannel`/`NPInhChannel` | same           | `NPHebbSynapse`   | membrane for Hebb neurons          |

All table values reflect **HEAD**; per `NPulseLibrary.cpp` history overrides for these membranes did not change after first registration.

---

### 6. Conclusions

1. **Membranes actually used in TestTrain training configs are `NPMembraneBio` (via `NSPNeuronGen`).**  
   Their parameters (`FeedbackGain = 2`, Bio channel and Bio synapse classes) were set in 2021 (`b8aad55`) and **remain unchanged**.

2. **Base logic of `NPulseMembraneCommon` and `NPulseMembrane` for potential summation and feedback is stable throughout history.**  
   Changes in these files were mainly formatting, structure build bug fixes, and compatibility, but **not numeric parameters**.

3. **Alternative membranes (`NPMembraneBio2`, LT and Syn membranes)** offer different RC parameters and `FeedbackGain`, but are not used in TestTrain.  
   They may be used later for experiments with different dynamics, but do not explain current Learner/Trainer behavior in the config under study.

4. **For regressions, observed training behavior (weak sensitivity to synapse count, dendrite length choice) cannot be explained by membrane parameter changes.**  
   `NPMembraneBio` and base membrane/channel parameters were stabilized long ago; the root cause is more likely in algorithm criteria (`ChangeDendriteStatus`, `SomaSynchronizePattern`, `SomaSynapseNormalization`) and current channel/synapse RC parameters covered in related reports.

5. **Practical conclusion:** when investigating structural growth algorithm behavior, focus on Learner/Trainer logic and channel/synapse parameters; membranes can be treated as a **fixed platform** whose parameters did not undergo dramatic historical changes.
