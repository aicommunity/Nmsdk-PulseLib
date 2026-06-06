## NNeuronFreqGroup — группа частотных нейронов

**Класс**: `NNeuronFreqGroup` — группирует нейроны по частотным признакам.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronFreqGroup", ...)`.
**Storage**: `ClassName = "NNeuronFreqGroup"`.

### Lifecycle
- **ADefault**: параметры группировки/частот.
- **ABuild**: создание/подключение нейронов группы.
- **AReset**: сброс состояний.
- **ACalculate**: обновление частотной активности группы.

### I/O
- Вход: сигналы/стимулы.
- Выход: частотные признаки/активности.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroup
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant G as NNeuronFreqGroup
    In-->>G: signals
    G-->>In: freq activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> g[NNeuronFreqGroup]
    g --> freq[Freq activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingNeurons: Создание афферентных нейронов
    CreatingNeurons --> ConfiguringFreqs: Настройка частотных диапазонов
    ConfiguringFreqs --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessInputs: Обработка входных сигналов
    ProcessInputs --> CalcFreqActivity: Вычисление частотной активности
    CalcFreqActivity --> AggregateActivity: Агрегация активности группы
    AggregateActivity --> Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Building** — выполняется сборка структуры
- **CreatingNeurons** — создание афферентных нейронов
- **ConfiguringFreqs** — настройка частотных диапазонов
- **Built** — структура группы построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет группы
- **ProcessInputs** — обработка входных сигналов
- **CalcFreqActivity** — вычисление частотной активности
- **AggregateActivity** — агрегация активности группы
- **Resetting** — выполняется сброс состояний

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph UComponent["UComponent Base"]
        BaseComponent[UComponent]
    end

    subgraph NNeuronFreqGroup["NNeuronFreqGroup"]
        FreqGroup[Группа частотных нейронов]
        AffNeurons[Афферентные нейроны]
    end

    subgraph External["Внешние компоненты"]
        InputSource[Источник входных сигналов]
        OutputTarget[Целевой компонент]
    end

    BaseComponent -->|наследуется| NNeuronFreqGroup
    NNeuronFreqGroup -->|создает| AffNeurons
    NNeuronFreqGroup -->|управляет| FreqGroup
    InputSource -->|сигналы| NNeuronFreqGroup
    NNeuronFreqGroup -->|частотная активность| OutputTarget
```

**Зависимости:**
- **Базовый класс**: `UComponent`
- **Внутренние компоненты**: афферентные нейроны (создаются автоматически)
- **Внешние компоненты**: источник входных сигналов (источник данных), целевой компонент (получатель частотной активности)

### Config snippet

```ini
[Component]
ClassName = NNeuronFreqGroup
Name = FreqGroup1
```

---

## NNeuronFreqGroup — neuron frequency group (EN)

Groups neurons by frequency features and outputs frequency activity.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroup
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant G as NNeuronFreqGroup
    In-->>G: signals
    G-->>In: freq activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> g[NNeuronFreqGroup]
    g --> freq[Freq activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingNeurons: Create afferent neurons
    CreatingNeurons --> ConfiguringFreqs: Configure frequency ranges
    ConfiguringFreqs --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessInputs: Process inputs
    ProcessInputs --> CalcFreqActivity: Calculate frequency activity
    CalcFreqActivity --> AggregateActivity: Aggregate activity
    AggregateActivity --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckMode{StructureBuildMode > 0?}
    CheckMode -->|Yes| BuildStructure[BuildStructure]
    CheckMode -->|No| ProcessInputs[Process inputs]
    BuildStructure --> CreateNeurons[Create afferent neurons]
    CreateNeurons --> ConfigFreqs[Configure frequency ranges]
    ConfigFreqs --> ProcessInputs
    ProcessInputs --> CalcFreqActivity[Calculate frequency activity]
    CalcFreqActivity --> AggregateActivity[Aggregate activity]
    AggregateActivity --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end

    subgraph NNeuronFreqGroup["NNeuronFreqGroup"]
        FreqGroup[Frequency neuron group]
        AffNeurons["Afferent Neurons<br/>NumAffNeurons"]
    end

    subgraph External["External Components"]
        InputSource[Input source]
        OutputTarget[Output target]
    end

    BaseNet -->|inherits| NNeuronFreqGroup
    NNeuronFreqGroup -->|creates| AffNeurons
    NNeuronFreqGroup -->|manages| FreqGroup
    InputSource -->|signals| NNeuronFreqGroup
    NNeuronFreqGroup -->|frequency activity| OutputTarget
```

## Источники

См. [Literature-References.md](../Literature-References.md): **neuromodeler.ru**, **15**, **[A]**.

### Properties

- `StructureBuildMode` — режим пересборки структуры (0 — не пересобирать, 1 — пересобрать)
- `AffNeuronClassName` — имя класса афферентных нейронов
- `NumAffNeurons` — количество афферентных нейронов
- `MinInputFreq` — минимальная входная частота
- `MaxInputFreq` — максимальная входная частота

### Methods

- `SetStructureBuildMode(value)` — установка режима пересборки структуры
- `SetAffNeuronClassName(value)` — установка имени класса афферентных нейронов
- `SetNumAffNeurons(value)` — установка количества афферентных нейронов
- `SetMinInputFreq(value)` — установка минимальной входной частоты
- `SetMaxInputFreq(value)` — установка максимальной входной частоты
- `ADefault()` — установка параметров по умолчанию
- `ABuild()` — сборка структуры группы
- `ACalculate()` — выполнение шага расчета частотной активности
- `BuildStructure()` — построение структуры афферентных нейронов

### Usage in configurations

`NNeuronFreqGroup` is used for grouping neurons by frequency features:

- **Frequency grouping**: `Bin/Configs/*/Model_*.xml` (where frequency-based neuron grouping is required)
- **Frequency analysis**: experiments with frequency-based signal analysis
- **Afferent processing**: processing afferent signals with frequency characteristics

**Features:**
- Automatic structure building: creates afferent neurons for frequency analysis
- Frequency ranges: configures frequency ranges for neurons
- Activity aggregation: aggregates frequency activity from neurons

**Typical parameter values:**
- **AffNeuronClassName**: "NAfferentNeuron" (afferent neuron)
- **NumAffNeurons**: 5-20 (number of afferent neurons)
- **MinInputFreq**: 1-10 (minimum input frequency)
- **MaxInputFreq**: 10-100 (maximum input frequency)

### References

See [Literature-References.md](../Literature-References.md): **neuromodeler.ru**, **15**, **[A]**.
