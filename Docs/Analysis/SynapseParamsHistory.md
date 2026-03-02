## Отчёт по параметрам синапсов для нейронов обучения (TestTrain)

### 1. Введение

Этот отчёт дополняет `IonParamsHistory.md` и `LTZoneUsageHistory.md` и фокусируется **только на синапсах**, которые реально используются в режиме обучения:

- `NNeuronLearner` и `NNeuronTrainer` в конфиге `TestTrain` (`Bin/Configs/Bakhshiev/TestTrain`).
- Нейрон: `NSPNeuronGen` (`NeuronClassName = "NSPNeuronGen"`).
- Мембрана: `NPMembraneBio`, которая внутри использует:
  - возбуждающие/тормозные каналы `NPExcChannelBio` / `NPInhChannelBio`;
  - синапсы `NPSynapseBio`.

Таким образом, для обучения в `TestTrain` задействованы:

- базовый синапс `NPulseSynapseCommon` (через `NPulseSynapse`);
- конфигурационный синапс `NPSynapseBio`.

### 2. Используемые синапс‑классы

#### 2.1. NPulseSynapseCommon — базовый импульсный синапс

Док: `Docs/Components/NPulseSynapseCommon.md`.

Краткий профиль:

- Базовый класс для всех импульсных синапсов.
- Основные параметры (значения по умолчанию из актуальной версии `NPulseSynapseCommon::ADefault()` / документации):
  - `Type` — тип синапса (знак возбуждения):
    - `< 0` — тормозной;
    - `> 0` — возбуждающий;
    - по умолчанию: `-1`.
  - `PulseAmplitude` — амплитуда спайка:
    - по умолчанию: `1.0`.
  - `Resistance` — сопротивление синапса:
    - по умолчанию: `1.0e8` (`100000000`; 100 МОм) — см. историю ниже.
  - `Weight` — вес синапса:
    - по умолчанию: `1.0`.

Поведение:

- В `ACalculate()`:
  - принимает входной спайковый сигнал (`Input`);
  - при наличии `WeightInput` может обновлять `Weight`;
  - вызывает `ACalculate2()` для расчёта «сырых» выходов;
  - умножает выход на `Weight` и отдаёт в постсинаптический нейрон.

#### 2.2. NPulseSynapse / NPSynapseBio — биоинспирированный синапс

Док: `Docs/Components/NPSynapseBio.md`.

- Иерархия:
  - `NPulseSynapseCommon` → `NPulseSynapse` → `NPSynapseBio`.
- Параметры `NPulseSynapse` (по умолчанию, до применения Bio‑конфигурации; см. историю в п. 4):
  - `SecretionTC` — постоянная времени секреции медиатора;
  - `DissociationTC` — постоянная времени распада медиатора;
  - `Resistance` — эффективное сопротивление синапса;
  - `TypicalPulseDuration`, `InhibitionCoeff`, `UsePresynapticInhibition`, `UsePulseSignal` и др.
- Конфигурационный класс `NPSynapseBio` (регистрация в `NPulseLibrary.cpp`):

```12:36:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
// Базовый импульсный синапс
cont=new NPulseSynapse;
cont->SetName("PSynapse");
cont->Default();
UploadClass("NPSynapse",cont);

// Био‑синапс
NPulseSynapse *syn=dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance=2e7*4.3;
syn->DissociationTC=0.005;
UploadClass("NPSynapseBio",syn);
```

Из этого и документа `NPSynapseBio.md`:

- `Resistance = 2e7 * 4.3 = 8.6e7` Ом (86 МОм).
- `DissociationTC = 0.005` с (5 мс).
- Остальные параметры берутся из `NPulseSynapse::ADefault()`:
  - `SecretionTC = 0.002` (см. историю, п. 4.1);
  - `PulseAmplitude = 1.0`;
  - `UsePulseSignal = true`;
  - `UsePresynapticInhibition = false`;
  - `InhibitionCoeff = 0.0`.

В учебных конфигурациях:

- `NNeuronLearner` / `NNeuronTrainer` явно используют `SynapseClassName = "NPSynapseBio"`.
- `NPMembraneBio` внутри также задаёт `SynapseClassName = "NPSynapseBio"`.

### 3. Численные параметры и их источники

#### 3.1. Текущие значения (HEAD)

Сводная таблица для Training‑цепочки (`NSPNeuronGen` → `NPMembraneBio` → `NPSynapseBio` → `NPulseSynapse` → `NPulseSynapseCommon`):

| Класс              | Базовый класс         | Параметр         | Значение по умолчанию / конфигурации         | Источник                    |
|--------------------|------------------------|------------------|----------------------------------------------|-----------------------------|
| `NPulseSynapseCommon` | `UNet`             | `PulseAmplitude` | `1.0`                                        | `NPulseSynapseCommon::ADefault` |
| `NPulseSynapseCommon` | `UNet`             | `Resistance`     | `1.0e8` (100 МОм)                            | см. историю, п. 4.1         |
| `NPulseSynapseCommon` | `UNet`             | `Weight`         | `1.0`                                        | `NPulseSynapseCommon::ADefault` |
| `NPulseSynapse`    | `NPulseSynapseCommon` | `SecretionTC`    | `0.002` с                                    | commit `5f3f06d`            |
| `NPulseSynapse`    | `NPulseSynapseCommon` | `DissociationTC` | `0.002` с                                    | commit `5f3f06d`            |
| `NPulseSynapse`    | `NPulseSynapseCommon` | `Resistance`     | `1.0e8` (100 МОм)                            | commit `5f3f06d`            |
| `NPSynapseBio`     | `NPulseSynapse`       | `Resistance`     | `8.6e7` (86 МОм)                             | `NPulseLibrary.cpp` / `NPSynapseBio.md` |
| `NPSynapseBio`     | `NPulseSynapse`       | `DissociationTC` | `0.005` с (5 мс)                             | `NPulseLibrary.cpp` / `NPSynapseBio.md` |
| `NPSynapseBio`     | `NPulseSynapse`       | `SecretionTC`    | `0.002` с (наследуется)                      | `NPulseSynapse::ADefault`   |
| `NPSynapseBio`     | `NPulseSynapse`       | `PulseAmplitude` | `1.0`                                        | наследуется                  |

Для обучения важно, что **эффективный синаптический ток** в Bio‑конфигурации пропорционален:

\n\\[ I_{syn}(t) \\approx \\frac{PreOutput(t)}{Resistance} \\cdot Weight, \\]\n

где `PreOutput` — динамика медиатора (управляется `SecretionTC`, `DissociationTC`).  
Высокое сопротивление (десятки мегом) и сравнительно быстрые константы времени делают вклад отдельного синапса **маленьким и достаточно «плоским»** при добавлении многих синапсов, что согласуется с наблюдениями из логов (амплитуда на соме плохо растёт при увеличении `SynapseNum`).

### 4. Git‑история параметров синапсов

#### 4.1. История NPulseSynapse / NPulseSynapseCommon

По `git log --oneline --reverse -- Core/NPulseSynapseCommon.cpp` и связанным файлам выданы ключевые коммиты:

- `3b0b124` — добавлены базовые классы `NPulseSynapseCommon` и др.
- `4d5bf00` — перенос свойства `Input` в базовый класс синапса.
- `d667fd8` — обновление методов `ACalculate`.
- `127e38e` — «Resistence param has been changed» (ранняя правка сопротивления).
- `b6da90e`, `ac5b51a` — рефакторинг системы свойств (`UProperty`), без изменения чисел.

Более поздний, ключевой для нас коммит:

```text
commit 5f3f06d07395120c71c641bc974511b066810d9c
Author: zarubin.kv
Date:   2022-04-15

    Change parameters NPulseSynapse
```

Фрагмент диффа (упрощённо):

```12:24:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseSynapse.cpp
bool NPulseSynapse::ADefault(void)
{
    PulseAmplitude = 1;

    // постоянная времени секреции медиатора
    SecretionTC = 0.002;      // было 0.001

    // постоянная времени распада медиатора
    DissociationTC = 0.002;   // было 0.01

    // типичное сопротивление синапса
    Resistance = 100000000;   // было 1.0e9

    ...
}
```

**До** этого коммита (и после отката в 2022‑06‑27):

- `SecretionTC = 0.001` с;
- `DissociationTC = 0.01` с;
- `Resistance = 1.0e9` Ом (1 ГОм).

**Промежуточно** в `5f3f06d`:

- `SecretionTC = 0.002` с;
- `DissociationTC = 0.002` с;
- `Resistance = 1.0e8` Ом (100 МОм).

Позже, в коммите `0be546f "Fix: synapse defaults return to normal."` (2022‑06‑27), эти параметры были **полностью возвращены** к исходным значениям, и HEAD сейчас совпадает с изначальной моделью.

#### 4.2. История NPSynapseBio в NPulseLibrary.cpp

Коммит `b8aad55` (2021‑03‑04) вводит `NPSynapseBio` и связанные Bio‑компоненты:

```12:40:e:\Science-Repo\nmsdk-git\Libraries\Nmsdk-PulseLib\Core\NPulseLibrary.cpp
// Био‑синапс
NPulseSynapse *syn=dynamic_pointer_cast<NPulseSynapse>(
    dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
syn->Resistance=2e7*4.3;
syn->DissociationTC=0.005;
UploadClass("NPSynapseBio",syn);
```

А также:

- `NPMembraneBio` — мембрана, использующая `NPSynapseBio` и `NPExcChannelBio` / `NPInhChannelBio`;
- Bio‑каналы с `FBResistance = 1e7`.

С момента появления `NPSynapseBio` (по всей видимой истории `Core/NPulseLibrary.cpp`) **его численные параметры не менялись** — коммит `5f3f06d` изменяет только базовый `NPulseSynapse::ADefault`, а блок `NPSynapseBio` в `NPulseLibrary.cpp` остаётся тем же.

**Сводная временная линия по ключевым параметрам:**

| Класс          | Параметр         | До 2021‑03‑04 (до b8aad55) | В b8aad55 (ввод NPSynapseBio) | В 5f3f06d (2022‑04‑15)           | После 0be546f / HEAD (2026)     |
|----------------|------------------|----------------------------|-------------------------------|----------------------------------|---------------------------------|
| `NPulseSynapse`| `Resistance`     | `1.0e9`                    | `1.0e9`                       | `1.0e8`                          | `1.0e9`                         |
| `NPulseSynapse`| `SecretionTC`    | `0.001`                    | `0.001`                       | `0.002`                          | `0.001`                         |
| `NPulseSynapse`| `DissociationTC` | `0.01`                     | `0.01`                        | `0.002`                          | `0.01`                          |
| `NPSynapseBio` | `Resistance`     | —                          | `8.6e7`                       | `8.6e7`                          | `8.6e7`                         |
| `NPSynapseBio` | `DissociationTC` | —                          | `0.005`                       | `0.005`                          | `0.005`                         |
| `NPSynapseBio` | `SecretionTC`    | —                          | `0.001` (наследует базу)      | `0.002` (наследует базу в окне между коммитами) | `0.001` (после отката базы) |

То есть:

- введение `NPSynapseBio` (2021) дало **фиксированный Bio‑набор параметров**, который с тех пор **не менялся**;
- изменение базы `NPulseSynapse` (2022) сделало обычные синапсы более похожими по масштабу на Bio‑конфигурацию, но **не трогало сам `NPSynapseBio`**.

### 5. Выводы

1. **Все обучающие цепочки в TestTrain используют один и тот же тип синапса — `NPSynapseBio`.**  
   Он конфигурируется в `NPulseLibrary.cpp` один раз и затем применяется во всех `NPMembraneBio` и во всех нейронах типа `NSPNeuronGen`.

2. **Численные параметры Bio‑синапса (`Resistance = 8.6e7`, `DissociationTC = 0.005`, `SecretionTC = 0.002`) были заданы в 2021 году и остаются неизменными.**  
   Это означает, что наблюдаемое сегодня поведение (в т.ч. слабая зависимость амплитуды от числа синапсов и отсутствие явного сигнала к остановке роста) **не является следствием недавних изменений именно в `NPSynapseBio`**.

3. **Изменения 2022 года в `NPulseSynapse` (commit `5f3f06d`) ускорили и упростили базовую модель медиатора, но `NPSynapseBio` продолжает переопределять часть параметров.**  
   В частности, базовый `Resistance` теперь `1.0e8`, а у Bio‑синапса — `8.6e7`, то есть Bio‑синапсы стали даже **чуть более проводящими** относительно базы, чем до коммита.

4. **С точки зрения регрессий, связать текущее поведение обучения (бесконечный рост числа синапсов) с изменениями параметров синапсов сложно.**  
   Гораздо сильнее на это влияет критерий в `SomaSynapseNormalization` (Trainer) / `ChangeSynapseStatus` (Learner), который использует сравнение `max_iter_dend_amp` с `InitialDendritePotential` + небольшой `eps`.  
   При текущем наборе RC‑параметров добавление синапсов изменяет амплитуду **слишком слабо**, так что разность остаётся в пределах допуска и алгоритм никогда не переключается в режим «удалять синапсы».

5. **Практический вывод:**  
   если цель — добиться устойчивой остановки роста числа синапсов и большей чувствительности амплитуды к их количеству, то:
   - проще всего экспериментировать с **критерием** в коде (порог/eps, гистерезис, верхняя граница по `SynapseNum`), а не с RC‑параметрами Bio‑синапса;
   - либо создавать **альтернативный конфигурационный синапс** (например, `NPSynapseBioWeak` с меньшим `Resistance` или иными временными константами) и подключать его через `NPulseLibrary.cpp`/конфиг, вместо модификации существующего `NPSynapseBio`, который уже давно используется в других сценариях.

