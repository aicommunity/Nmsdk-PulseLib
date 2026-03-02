## Отчёт по параметрам каналов, мембран и синапсов для NNeuronLearner (TestTrain)

### 1. Используемые классы и структура нейрона

- **Компонент обучения**: `NNeuronLearner`
  - Конфиг: `Bin/Configs/Bakhshiev/TestTrain/Model_00.xml`, `Parameters_00.xml`
  - Основные параметры (см. `NNeuronLearner::ADefault()`):
    - `NeuronClassName = "NSPNeuronGen"`
    - `SynapseClassName = "NPSynapseBio"`
    - `StructureBuildMode = 1`
    - `MaxDendriteLength = 100`
- **Класс нейрона**: `NSPNeuronGen`
  - В конфиге: `<Neuron Class="NSPNeuronGen">`
  - Работает с сегментами мембраны `NPMembraneBio` (см. ниже).
- **Мембрана (дендриты и сома)**: `NPMembraneBio`
  - В конфиге: `MembraneClassName = "NPMembraneBio"`, `<Dendrite*_1 Class="NPMembraneBio">`, `<Soma* Class="NPMembraneBio">`
  - Регистрация (конфиг‑вариант `NPulseMembrane`): `Core/NPulseLibrary.cpp`
    - `UploadClass("NPMembraneBio", membr);`
  - Документация: `Docs/Components/NPMembraneBio.md`
  - Внутренние компоненты:
    - `ExcChannelClassName = "NPExcChannelBio"`
    - `InhChannelClassName = "NPInhChannelBio"`
    - `SynapseClassName = "NPSynapseBio"`
- **Возбуждающий канал**: `NPExcChannelBio`
  - Конфиг‑вариант класса `NPulseChannel`.
  - Документация: `Docs/Components/NPExcChannelBio.md`.
- **Тормозной канал**: `NPInhChannelBio`
  - Конфиг‑вариант класса `NPulseChannel`.
  - Документация: `Docs/Components/NPInhChannelBio.md`.
- **Синапс**: `NPSynapseBio`
  - Конфиг‑вариант класса `NPulseSynapse` / `NPulseSynapseCommon`.
  - Документация: `Docs/Components/NPSynapseBio.md`.

Схематично (из доков, адаптировано к текущему кейсу):

```mermaid
flowchart TD
    NNeuronLearner --> NSPNeuronGen
    NSPNeuronGen --> NPMembraneBio
    NPMembraneBio --> NPExcChannelBio
    NPMembraneBio --> NPInhChannelBio
    NPMembraneBio --> NPSynapseBio
```

### 2. Значимые параметры и места их задания

#### 2.1. Базовый канал `NPulseChannel`

Файл: `Core/NPulseChannel.cpp`

- **Емкость мембраны**:
  - Свойство: `UProperty<double> Capacity;`
  - Значение по умолчанию (метод `NPulseChannel::ADefault()`):
    - `Capacity = 1.0e-9;` (1 нФ)
- **Сопротивление мембраны**:
  - Свойство: `UProperty<double> Resistance;`
  - Значение по умолчанию:
    - `Resistance = 1.0e7;` (10 МОм)
- **Сопротивление перезаряда (обратной связи)**:
  - Свойство: `UProperty<double> FBResistance;`
  - Значение по умолчанию:
    - `FBResistance = 1.0e8;` (100 МОм)
- **Сопротивление в состоянии покоя**:
  - Свойство: `UProperty<double> RestingResistance;`
  - Значение по умолчанию:
    - `RestingResistance = 1.0e7;` (10 МОм)
- **Тип канала**:
  - Свойство: `UProperty<double> Type;` (унаследовано от `NPulseChannelCommon`)
  - В `NPulseChannel::ADefault()`:
    - `Type = 0;` (нейтральный, затем переопределяется конфиг‑классами)
- **Временная константа канала**:
  - Свойство: `UProperty<double> TimeConstant;`
  - В `AReset()`:
    - `TimeConstant = Resistance * Capacity;`
  - В `ACalculate2()`:
    - Для случая малой обратной связи:
      \[
      T_i = \frac{Capacity}{G + 1 / resistance}, \quad
      \text{resistance} = \begin{cases}
      RestingResistance, & \text{вблизи покоя}\\
      Resistance, & \text{вне покоя}
      \end{cases}
      \]
    - Иначе:
      \[
      T_i = \frac{Capacity}{G + 1 / FBResistance}
      \]

#### 2.2. Конфиг‑варианты каналов `NPExcChannelBio` и `NPInhChannelBio`

Док‑файлы:
- `Docs/Components/NPExcChannelBio.md`
- `Docs/Components/NPInhChannelBio.md`

Параметры (конфиг‑обёртки над `NPulseChannel`):

- **NPExcChannelBio**:
  - `Type = -1` — возбуждающий канал.
  - `FBResistance = 1e7` (10 МОм) — сопротивление обратной связи.
  - Остальные параметры берутся из `NPulseChannel::ADefault()`:
    - `Capacity = 1.0e-9`
    - `Resistance = 1.0e7`
    - `RestingResistance = 1.0e7`
- **NPInhChannelBio**:
  - `Type = 1` — тормозной канал.
  - `FBResistance = 1e7` (10 МОм).
  - Остальные параметры по умолчанию из `NPulseChannel`.

#### 2.3. Базовая мембрана `NPulseMembrane` и конфиг `NPMembraneBio`

Файлы:
- `Core/NPulseMembrane.h`, `Core/NPulseMembrane.cpp`
- `Core/NPulseMembraneCommon.h`, `Core/NPulseMembraneCommon.cpp`
- Регистрация в библиотеке: `Core/NPulseLibrary.cpp`
- Док: `Docs/Components/NPMembraneBio.md`

**NPulseMembraneCommon**:
- Управляет списками каналов и синапсов, суммарным потенциалом `SumPotential`.
- Параметры RC‑модели задаются не здесь, а в каналах (`NPulseChannel`).

**NPulseMembrane**:
- Дополнительные свойства:
  - `FeedbackGain` — коэффициент обратной связи от LT‑зоны.
  - `ResetAvailable` — флаг механизма сброса.
  - `SynapseClassName`, `ExcChannelClassName`, `InhChannelClassName` — имена классов синапсов и каналов.
- Значения по умолчанию (`NPulseMembrane::ADefault()`):
  - `FeedbackGain = 2.0`
  - `ResetAvailable = true`
  - `SynapseClassName = "NPSynapse"`
  - `ExcChannelClassName = "NPExcChannel"`
  - `InhChannelClassName = "NPInhChannel"`

**NPMembraneBio** (регистрация в `NPulseLibrary.cpp`):

```cpp
membr = dynamic_pointer_cast<NPulseMembrane>(...->TakeObject("NPMembrane"));
membr->SetName("PMembrane");
membr->ExcChannelClassName = "NPExcChannelBio";
membr->SynapseClassName   = "NPSynapseBio";
membr->InhChannelClassName = "NPInhChannelBio";
UploadClass("NPMembraneBio", membr);
```

То есть:
- Наследует **все численные параметры мембраны и каналов** из `NPulseChannel::ADefault()` и `NPExcChannelBio`/`NPInhChannelBio`.
- Специального изменения `FeedbackGain` для `NPMembraneBio` **нет** (в отличие от `NPMembraneBio2`, где `FeedbackGain = 0.02`).

#### 2.4. Базовый синапс `NPulseSynapseCommon` и конфиг‑вариант `NPSynapseBio`

Файл: `Core/NPulseSynapseCommon.cpp`

- Свойства:
  - `PulseAmplitude` — амплитуда входного импульса.
  - `Resistance` — эффективное сопротивление синапса (влияет на ток через канал).
  - `Weight` — вес синапса.
  - `TrainerClassName` — класс тренера веса.
  - `PreOutput` — промежуточное состояние медиатора.
- Значения по умолчанию (`NPulseSynapseCommon::ADefault()`):
  - `Type = -1` — по умолчанию возбуждающий.
  - `PulseAmplitude = 1.0`
  - `Resistance = 10.0` (условные единицы для «обычных» синапсов).
  - `Weight = 1.0`

Конфиг‑вариант `NPSynapseBio` (док `Docs/Components/NPSynapseBio.md`):

- Явно задаёт биоинспирированные параметры:
  - `Resistance = 8.6e7` (86 МОм)
  - `DissociationTC = 0.005` (5 мс) — постоянная времени распада медиатора.
  - `SecretionTC = 0.001` (1 мс) — постоянная времени секреции медиатора.
- Алгоритм:
  - `PreOutput` интегрирует медиатор с учётом `SecretionTC` и `DissociationTC`.
  - Выходной ток: `Output = PreOutput / Resistance * Weight`.

Таким образом:
- Для **био‑синапсов** TestTrain конфигурации сопротивление значительно выше, чем у абстрактных «классических» синапсов, что сильно ограничивает ток на канал при фиксированном `PreOutput`.

> **Подробный отчёт именно по обучающим синапсам (NPulseSynapse / NPSynapseBio), включая их git‑историю и связь с NNeuronLearner/NNeuronTrainer в конфиге TestTrain, см. в отдельном документе `SynapseParamsHistory.md`.**

### 3. Изменения параметров в git‑истории (полная история)

Анализ выполнен по сабрепозиторию `Libraries/Nmsdk-PulseLib` (команды вида `git log`, `git show`, `git log -S` внутри этого репо) по **всей доступной истории**.

#### 3.1. Файлы ядра (C++)

**Core/NPulseChannel.cpp**

- Ключевые коммиты:
  - `700b7df (ранние версии) "Change NMSDK Structure"` — в `ADefault()` уже заданы:
    - `Capacity = 1.0e-9`, `Resistance = 1.0e7`, `FBResistance = 1.0e8`.
  - `389e5e2 (2016-06-24) "Motion control debug."`:
    - добавлен параметр `RestingResistance` и его значение по умолчанию:
      - `RestingResistance = 1.0e7`.
    - изменена формула в `ACalculate2()`, чтобы вблизи покоя использовать `RestingResistance`, а вне покоя — `Resistance`.
  - более поздние коммиты (`9d7339e`, `b6da90e`, `ac5b51a`, `df69224` и др.):
    - рефакторинг имён (`SumChannelInputs` → `SumChannelInput`),
    - переход с `.v` на `GetData()`,
    - мелкие оптимизации (временные переменные).
- **Численные значения**:
  - `Capacity = 1.0e-9`, `Resistance = 1.0e7`, `FBResistance = 1.0e8` заданы уже в самом раннем коммите и **не менялись** по всей истории.
  - `RestingResistance = 1.0e7` появился в 2016‑06‑24 (`389e5e2`) и с момента ввода **не менялся**.

**Core/NPulseSynapseCommon.cpp**

- Ключевые коммиты:
  - Ранние версии (`3b0b124` и последующие) — в `ADefault()`:
    - `PulseAmplitude = 1.0`
    - `Resistance = 1.0`
  - `127e38e (2021-03-12) "Resistence param has been changed"`:
    - изменение значения по умолчанию:
      - было: `Resistance = 1.0`
      - стало: `Resistance = 10.0`
  - `b6da90e (2025-11-22)` — упрощение иерархии свойств (без изменения чисел).
  - `ac5b51a (2026-01-02)` — перевод `Type.v` → `Type.SetDataDirect` (логика распределения по возбуждающим/тормозным синапсам, численные параметры без изменений).
- **Численные значения**:
  - `PulseAmplitude = 1.0` — стабильно по всей истории.
  - `Resistance`:
    - до 2021‑03‑12 (`127e38e`): `Resistance = 1.0`;
    - с 2021‑03‑12 и по сей день: `Resistance = 10.0`.

**Core/NPulseLibrary.cpp**

- Ключевые моменты:
  - `b8aad55 (2021-03-04) "Some bugs in neuron potential has been fixed."`:
    - впервые вводится конфигурационный класс `NPSynapseBio`:
      - `syn->Resistance = 2e7 * 4.3` (86 МОм);
      - `syn->DissociationTC = 0.005`.
    - регистрируются `NPExcChannelBio` / `NPInhChannelBio`:
      - для обоих задаётся `FBResistance = 1e7`;
    - добавляется `NPMembraneBio`:
      - `ExcChannelClassName = "NPExcChannelBio"`,
      - `InhChannelClassName = "NPInhChannelBio"`,
      - `SynapseClassName = "NPSynapseBio"`.
  - `5f3f06d (2022-04-15) "Change parameters NPulseSynapse"`:
    - правки в `Core/NPulseLibrary.cpp`, но строки с `Resistance=2e7*4.3`, `DissociationTC=0.005` для `NPSynapseBio` **не менялись** (изменения форматирования/отступов).
  - Более поздние коммиты (`9d7339e`, `b6da90e`, `9b44962`, `a73d592`, `ba0e0c9` и др.):
    - рефакторинг имён (`PLTZone` → `LTZone`, Pos/Neg → Inh/Exc),
    - добавление новых конфигураций (`*Bio2`, солверы и т.п.),
    - исправление предупреждений.
- Итог:
  - `NPSynapseBio` с параметрами `Resistance = 2e7 * 4.3`, `DissociationTC = 0.005` существует **с 2021‑03‑04** и далее **без изменений**.
  - `NPExcChannelBio` / `NPInhChannelBio` с `FBResistance = 1e7` также введены в `b8aad55` и их численные параметры не менялись.
  - Для `NPMembraneBio` строки с `ExcChannelClassName = "NPExcChannelBio"`, `SynapseClassName = "NPSynapseBio"`, `InhChannelClassName = "NPInhChannelBio"` стабильно присутствуют во всех последующих версиях; `FeedbackGain` берётся из `NPulseMembrane::ADefault()` (`2.0`) и отдельной настройки для `NPMembraneBio` не было.

#### 3.2. Документация (md‑файлы)

Файлы:
- `Docs/Components/NPMembraneBio.md`
- `Docs/Components/NPSynapseBio.md`
- `Docs/Components/NPExcChannelBio.md`
- `Docs/Components/NPInhChannelBio.md`

Коммиты (см. `git log --since=2024-01-01 --oneline`):
- `424eb3b Add autogenerated components docs`
- `5a37c86 Expand docs`
- `3b8f2a9 Fix docs`
- `a4cfc1b Autoupdate docs`

Характер изменений:
- Добавление/расширение UML‑диаграмм, описаний и примеров.
- Уточнение формулировок параметров.

Численные значения в доках:

- `NPSynapseBio.md`:
  - `Resistance = 8.6e7`
  - `DissociationTC = 0.005`
  - `SecretionTC = 0.001`
- `NPExcChannelBio.md`:
  - `Type = -1`
  - `FBResistance = 1e7`
- `NPInhChannelBio.md`:
  - `Type = 1`
  - `FBResistance = 1e7`
- `NPMembraneBio.md`:
  - `ExcChannelClassName = "NPExcChannelBio"`
  - `InhChannelClassName = "NPInhChannelBio"`
  - `SynapseClassName = "NPSynapseBio"`

По содержанию последних версий этих файлов численные значения **соответствуют исходным значениям** и не демонстрируют изменений в указанный период; изменения носили текстово‑документационный характер.

### 4. Сводная таблица параметров и истории изменений

#### 4.1. Каналы и мембрана

| Компонент            | Параметр           | Текущее значение          | Источник (код/док)                         | История изменений (полная)                                  |
|----------------------|--------------------|---------------------------|--------------------------------------------|-------------------------------------------------------------|
| `NPulseChannel`      | `Capacity`         | `1.0e-9`                  | `Core/NPulseChannel.cpp::ADefault`        | С самого первого коммита (`700b7df`), без изменений         |
| `NPulseChannel`      | `Resistance`       | `1.0e7`                   | `Core/NPulseChannel.cpp::ADefault`        | С самого первого коммита, без изменений                     |
| `NPulseChannel`      | `FBResistance`     | `1.0e8`                   | `Core/NPulseChannel.cpp::ADefault`        | С самого первого коммита, без изменений                     |
| `NPulseChannel`      | `RestingResistance`| `1.0e7`                   | `Core/NPulseChannel.cpp::ADefault`        | Введён в `389e5e2` (2016‑06‑24) и далее не менялся          |
| `NPulseChannel`      | `Type` (по умолч.) | `0`                       | `Core/NPulseChannel.cpp::ADefault`        | Всегда `0`, спец. значения задают конфиг‑варианты          |
| `NPExcChannelBio`    | `Type`             | `-1`                      | `Docs/Components/NPExcChannelBio.md`      | Введён в `b8aad55` (2021‑03‑04), без изменений              |
| `NPExcChannelBio`    | `FBResistance`     | `1e7`                     | `Docs/Components/NPExcChannelBio.md`      | Введён в `b8aad55` (2021‑03‑04), без изменений              |
| `NPInhChannelBio`    | `Type`             | `1`                       | `Docs/Components/NPInhChannelBio.md`      | Введён в `b8aad55` (2021‑03‑04), без изменений              |
| `NPInhChannelBio`    | `FBResistance`     | `1e7`                     | `Docs/Components/NPInhChannelBio.md`      | Введён в `b8aad55` (2021‑03‑04), без изменений              |
| `NPulseMembrane`     | `FeedbackGain`     | `2.0`                     | `Core/NPulseMembrane.cpp::ADefault`       | С момента введения свойства, без изменений                  |
| `NPMembraneBio`      | `ExcChannelClassName` | `"NPExcChannelBio"`    | `Core/NPulseLibrary.cpp`, `NPMembraneBio.md` | Введено в `b8aad55` вместе с классом, без изменений     |
| `NPMembraneBio`      | `InhChannelClassName` | `"NPInhChannelBio"`    | `Core/NPulseLibrary.cpp`, `NPMembraneBio.md` | Введено в `b8aad55`, без изменений                      |
| `NPMembraneBio`      | `SynapseClassName` | `"NPSynapseBio"`          | `Core/NPulseLibrary.cpp`, `NPMembraneBio.md` | Введено в `b8aad55`, без изменений                      |

#### 4.2. Синапсы

| Компонент             | Параметр        | Текущее значение | Источник                                  | История изменений (полная)                                      |
|-----------------------|-----------------|------------------|-------------------------------------------|------------------------------------------------------------------|
| `NPulseSynapseCommon` | `Resistance`    | `10.0`           | `Core/NPulseSynapseCommon.cpp::ADefault` | До `127e38e` (2021‑03‑12): `1.0`; с `127e38e`: `10.0`           |
| `NPulseSynapseCommon` | `PulseAmplitude`| `1.0`            | `Core/NPulseSynapseCommon.cpp::ADefault` | Всегда `1.0`                                                    |
| `NPSynapseBio`        | `Resistance`    | `8.6e7` (86 МОм) | `Docs/Components/NPSynapseBio.md`, `Core/NPulseLibrary.cpp` | Введён в `b8aad55` (2021‑03‑04) и далее не менялся        |
| `NPSynapseBio`        | `DissociationTC`| `0.005` (5 мс)   | `Docs/Components/NPSynapseBio.md`        | Введён в `b8aad55`, без изменений                               |
| `NPSynapseBio`        | `SecretionTC`   | `0.001` (1 мс)   | `Docs/Components/NPSynapseBio.md`        | Введён в `b8aad55`, без изменений                               |

### 5. Выводы по влиянию параметров на наблюдаемое обучение

1. **RC‑параметры каналов и мембран** (`Capacity`, `Resistance`, `FBResistance`, `RestingResistance`, `FeedbackGain`) **не менялись** в недавней истории (с 2024‑01‑01).  
   - Математика канала (`NPulseChannel::ACalculate2`) фиксирует временные константы \(\tau\) через эти значения; поведение, связанное с временной фильтрацией и насыщением потенциала, **стабильно во всех последних версиях**.
2. **Био‑синапсы `NPSynapseBio`** используют очень большое сопротивление (`8.6e7` против `10.0` у базового синапса), что:
   - резко снижает ток на канал при фиксированном `PreOutput`,
   - делает прирост тока при добавлении синапсов менее выраженным и более чувствительным к динамике медиатора, чем к числу синапсов.
3. В сочетании с критериями в `NNeuronLearner` / `NNeuronTrainer`:
   - **Изменение числа синапсов** зачастую не даёт достаточно заметной разницы в амплитуде/времени на соме, чтобы алгоритм «увидел» улучшение,  
   - в то время как изменение длины дендритов меняет только временной сдвиг, а не амплитуду — и при выбранных параметрах RC‑цепочек оптимальной признаётся минимальная длина.
4. Поскольку **численные значения параметров каналов и синапсов не изменялись в недавней истории**, наблюдаемое поведение (отсутствие устойчивого роста дендритов и неостанавливающийся рост синапсов) является **следствием самой текущей настройки RC‑параметров и критериев обучения**, а не недавних изменений в коде.

Практически это означает, что для изменения поведения обучения (например, чтобы рост синапсов оказывал более отчётливое влияние на амплитуду, а алгоритм мог стабилизироваться) потребуется **целенаправленная корректировка параметров**:
- уменьшение сопротивления синапсов (`Resistance` у `NPSynapseBio`),
- изменение `Capacity` и `Resistance` каналов (через новые конфиг‑варианты или настройки),
- тонкая настройка порогов и критериев в логике обучения (`ChangeSynapseStatus`, `SomaSynapseNormalization` и т.п.).

### 6. Регистрация конфигурационных классов в `NPulseLibrary` (Storage‑уровень)

В файле `Core/NPulseLibrary.cpp` конфигурационные варианты каналов, синапсов и мембран регистрируются как готовые экземпляры через `UploadClass(...)`. Это определяет, какие именно численные параметры получают компоненты при создании из `UStorage` по имени класса.

#### 6.1. Синапсы

Все синапсы строятся на базе `NPulseSynapse` (который наследует поведение от `NPulseSynapseCommon` с базовыми значениями из раздела 2.4).

| Storage‑класс     | Базовый объект            | Переопределённые параметры                                                  | Коммит появления / изменения             |
|-------------------|---------------------------|------------------------------------------------------------------------------|------------------------------------------|
| `NPSynapse`       | `new NPulseSynapse`       | `Default()` → базовые значения (`Resistance = 10.0`, `PulseAmplitude = 1.0`) | Базовый класс, изменения только в базовом коде |
| `NPSynapseBio`    | `TakeObject("NPSynapse")` | `Resistance = 2e7*4.3` (≈`8.6e7`), `DissociationTC = 0.005`                 | Введён в `b8aad55` (2021‑03‑04), без изменений |
| `NPSynapseBio2`   | `TakeObject("NPSynapse")` | `Resistance = 86000000`, `DissociationTC = 0.005` (тот же уровень, другая запись) | Введён в `98d1edc` (2023‑03‑25), без изменений |
| `NPHebbSynapse`   | `new NPulseHebbSynapse`   | Использует свои правила обучения; явных численных RC‑параметров в `NPulseLibrary` не задаётся | Ранее, численные параметры задаются в самом классе |

С точки зрения амплитуд:
- `NPSynapseBio` и `NPSynapseBio2` отличаются только способом записи `Resistance`, численно они эквивалентны (86 МОм).

#### 6.2. Каналы

Базовый канал `NPChannel` создаётся как `NPulseChannel` с параметрами из `ADefault()` (раздел 2.1). Все остальные Storage‑классы каналов создаются через `TakeObject(...)` с модификацией части параметров.

| Storage‑класс        | База (`TakeObject`) | Параметры относительно `NPulseChannel::ADefault()`                                                 | Коммит появления / изменения                  |
|----------------------|---------------------|-----------------------------------------------------------------------------------------------------|-----------------------------------------------|
| `NPChannel`          | `new NPulseChannel` | Базовые: `Capacity = 1e-9`, `Resistance = 1e7`, `FBResistance = 1e8`, `RestingResistance = 1e7`     | Базовый класс                                  |
| `NPNeuronChannel`    | `NPChannel`         | `Default()`; параметры идентичны `NPChannel`                                                        | Поздний рефакторинг (`517553e` и далее), без изменения чисел |
| `NPExcChannel`       | `NPChannel`         | `Type = -1` (возбуждающий); RC‑параметры как у `NPChannel`                                         | Введён в `73f3100` (2020‑04‑16), без изменений |
| `NPInhChannel`       | `NPChannel`         | `Type = 1` (тормозной); RC‑параметры как у `NPChannel`                                             | Введён в `73f3100`, без изменений             |
| `NPExcChannelBio`    | `NPChannel`         | `Type = -1`; `FBResistance = 1e7` (10 МОм); остальные параметры как у `NPChannel`                  | Введён в `b8aad55` (2021‑03‑04), без изменений |
| `NPExcChannelBio2`   | `NPChannel`         | `Type = -1`; `FBResistance = 3e6`; `Resistance = 1.6e7`; `RestingResistance = 3e6`; `Capacity = 2.5e-10` | Введён в `98d1edc` (2023‑03‑25), без изменений |
| `NPInhChannelBio`    | `NPChannel`         | `Type = 1`; `FBResistance = 1e7`; остальное как у `NPChannel`                                     | Введён в `b8aad55`, без изменений             |
| `NPInhChannelBio2`   | `NPChannel`         | `Type = 1`; `FBResistance = 3e6`; `Resistance = 1.6e7`; `RestingResistance = 3e6`; `Capacity = 2.5e-10` | Введён в `98d1edc`, без изменений             |
| `NPSynChannel`       | `new NPulseSynChannel` | Базовый синаптический канал (своё поведение); численные RC‑параметры задаются внутри класса       | Базовый класс                                  |
| `NCSynChannel`       | `new NContinuesSynChannel` | Непрерывный канал; RC‑параметры задаются внутри специализированного класса                        | Базовый класс                                  |
| `NPSynExcChannel`    | `NPSynChannel`      | `Type = -1`; остальные параметры как у `NPSynChannel`                                              | Введён в `73f3100`, без изменений             |
| `NPSynInhChannel`    | `NPSynChannel`      | `Type = 1`; остальные параметры как у `NPSynChannel`                                               | Введён в `73f3100`, без изменений             |
| `NCSynExcChannel`    | `NCSynChannel`      | `Type = -1`; остальные параметры как у `NCSynChannel`                                              | Введён в `73f3100`, без изменений             |
| `NCSynInhChannel`    | `NCSynChannel`      | `Type = 1`; остальные параметры как у `NCSynChannel`                                               | Введён в `73f3100`, без изменений             |
| `NPLTChannel`        | `NPChannel`         | `Capacity = 1e-8` (в 10 раз больше), `RestingResistance = 1e6` (в 10 раз меньше)                   | Введён в `73f3100`, без изменений             |
| `NPLTExcChannel`     | `NPLTChannel`       | `Type = -1`                                                                                         | Введён в `73f3100`, без изменений             |
| `NPLTInhChannel`     | `NPLTChannel`       | `Type = 1`                                                                                          | Введён в `73f3100`, без изменений (опечатка в старом diff уже исправлена) |
| `NPLTSynChannel`     | `NPSynChannel`      | `Capacity = 1e-8`, `RestingResistance = 1e6`                                                       | Введён в `73f3100`, без изменений             |
| `NPLTSynExcChannel`  | `NPLTSynChannel`    | `Type = -1`                                                                                         | Введён в `73f3100`, без изменений             |
| `NPLTSynInhChannel`  | `NPLTSynChannel`    | `Type = 1`                                                                                          | Введён в `73f3100`, без изменений             |

Для конфигурации TestTrain с `NNeuronLearner` используются только `NPExcChannelBio` / `NPInhChannelBio` (через `NPMembraneBio`) и не используются `*Bio2`, LT‑ и Syn‑варианты, однако понимание их параметров важно для сравнения альтернативных конфигураций.

#### 6.3. Мембраны

Все мембраны строятся на базе `NPulseMembrane` (с базовыми значениями из `NPulseMembrane::ADefault()`), после чего часть параметров переопределяется.

| Storage‑класс              | База (`TakeObject`) | Переопределённые параметры                                              | Коммит появления / изменения           |
|----------------------------|---------------------|--------------------------------------------------------------------------|----------------------------------------|
| `NPMembrane`              | `new NPulseMembrane`| `Default()`; использует `NPExcChannel` / `NPInhChannel`, `NPSynapse`     | Базовый класс                          |
| `NPNeuronMembrane`        | `NPMembrane`        | `Default()`; параметры идентичны `NPMembrane`                            | Поздний рефакторинг, без изменения чисел |
| `NPNewNeuronMembrane`     | `NPMembrane`        | `Default()`; параметры идентичны `NPMembrane`                            | Поздний рефакторинг, без изменения чисел |
| `NPMembraneBio`           | `NPMembrane`        | `ExcChannelClassName = "NPExcChannelBio"`, `InhChannelClassName = "NPInhChannelBio"`, `SynapseClassName = "NPSynapseBio"` | Введена в `b8aad55`, без изменений     |
| `NPMembraneBio2`          | `NPMembrane`        | `ExcChannelClassName = "NPExcChannelBio2"`, `InhChannelClassName = "NPInhChannelBio2"`, `SynapseClassName = "NPSynapseBio2"`, `FeedbackGain = 0.02` | Введена в `98d1edc`, без изменений     |
| `NPLTZoneNeuronMembrane`  | `NPMembrane`        | `SetName("LTMembrane")`, `ExcChannelClassName = "NPLTExcChannel"`, `InhChannelClassName = "NPLTInhChannel"` | Введена в `73f3100`, без изменений     |
| `NPLTZoneSynNeuronMembrane` | `NPMembrane`      | `ExcChannelClassName = "NPLTSynExcChannel"`, `InhChannelClassName = "NPLTSynInhChannel"` | Введена в `73f3100`, без изменений     |
| `NPSynNeuronMembrane`     | `NPMembrane`        | `ExcChannelClassName = "NPSynExcChannel"`, `InhChannelClassName = "NPSynInhChannel"` | Введена в `73f3100`, без изменений     |
| `NCSynNeuronMembrane`     | `NPMembrane`        | `ExcChannelClassName = "NCSynExcChannel"`, `InhChannelClassName = "NCSynInhChannel"` | Введена в `73f3100`, без изменений     |
| `NPNeuronHebbMembrane`    | `NPMembrane`        | `SynapseClassName = "NPHebbSynapse"`, дополнительная сборка Hebb‑синапсов | Введена до `61aaee9`, параметры RC мембраны как у `NPMembrane` |

Таким образом, `NPMembraneBio` и `NPMembraneBio2` отличаются не только набором используемых каналов и синапсов, но и (в случае `Bio2`) **усилением обратной связи** (`FeedbackGain = 0.02`) и более «быстрыми» RC‑цепочками в каналах (`Capacity` и `Resistance` через `NPExcChannelBio2` / `NPInhChannelBio2`).

Дополнительный анализ порогов LT‑зоны и их эволюции по git‑истории приведён в отдельном отчёте `LTZoneUsageHistory.md`, где рассматриваются:

- используемые в TestTrain LT‑зоны (`NPulseLTZoneThreshold` и её конфигурационные варианты);
- способ задания порогов через `NPulseLibrary.cpp` и свойства `LTZThreshold`/`TrainingLTZThreshold` у `NNeuronLearner`/`NNeuronTrainer`;
- подтверждение того, что в пределах истории сабмодуля Nmsdk‑PulseLib численные критерии LT‑зоны для `NSPNeuronGen` оставались стабильными.
