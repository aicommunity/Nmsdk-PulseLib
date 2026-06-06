## RU

## NNeuronFreqGroupLayer — слой частотных групп

**Класс**: `NNeuronFreqGroupLayer` — слой, состоящий из нескольких `NNeuronFreqGroup`.
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronFreqGroupLayer", ...)`.
**Storage**: `ClassName = "NNeuronFreqGroupLayer"`.

### Lifecycle
- **ADefault**: параметры групп/слоя.
- **ABuild**: создание и связывание групп.
- **AReset**: сброс состояний групп.
- **ACalculate**: обновление всех групп слоя.

### I/O
- Вход: сигналы/стимулы.
- Выход: совокупная частотная активность по группам.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroupLayer
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronFreqGroupLayer
    In-->>L: signals
    L-->>In: freq groups activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronFreqGroupLayer]
    layer --> act[Groups activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### UML-диаграмма состояний

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingGroups: Создание частотных групп
    CreatingGroups --> LinkingGroups: Связывание групп
    LinkingGroups --> Built: Структура создана
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> LoopGroups: Цикл по группам
    LoopGroups --> CalcGroup: Расчет группы
    CalcGroup --> AggregateGroups: Агрегация активности групп
    AggregateGroups --> CheckMoreGroups: Есть еще группы?
    CheckMoreGroups -->|Да| LoopGroups
    CheckMoreGroups -->|Нет| Ready: Шаг завершен
    Ready --> Resetting: Reset()
    Resetting --> Ready: Состояния сброшены
```

**Состояния:**
- **Uninitialized** — создан, но не инициализирован
- **Defaulted** — параметры установлены по умолчанию
- **Building** — выполняется сборка структуры
- **CreatingGroups** — создание частотных групп
- **LinkingGroups** — связывание групп
- **Built** — структура слоя построена
- **Ready** — готов к выполнению расчетов
- **Calculating** — выполняется расчет слоя
- **LoopGroups** — цикл по группам
- **CalcGroup** — расчет группы
- **AggregateGroups** — агрегация активности групп
- **CheckMoreGroups** — проверка наличия еще групп
- **Resetting** — выполняется сброс состояний

### UML-диаграмма компонентов

```mermaid
graph TB
    subgraph UComponent["UComponent Base"]
        BaseComponent[UComponent]
    end

    subgraph NNeuronFreqGroupLayer["NNeuronFreqGroupLayer"]
        Layer[Слой частотных групп]
        FreqGroups["Частотные группы<br/>NNeuronFreqGroup"]
    end

    subgraph External["Внешние компоненты"]
        InputSource[Источник входных сигналов]
        OutputTarget[Целевой компонент]
    end

    BaseComponent -->|наследуется| NNeuronFreqGroupLayer
    NNeuronFreqGroupLayer -->|создает| FreqGroups
    NNeuronFreqGroupLayer -->|управляет| Layer
    InputSource -->|сигналы| NNeuronFreqGroupLayer
    NNeuronFreqGroupLayer -->|совокупная активность| OutputTarget
```

**Зависимости:**
- **Базовый класс**: `UComponent`
- **Внутренние компоненты**: частотные группы (`NNeuronFreqGroup`, создаются автоматически)
- **Внешние компоненты**: источник входных сигналов (источник данных), целевой компонент (получатель совокупной активности)

### Config snippet

```ini
[Component]
ClassName = NNeuronFreqGroupLayer
Name = FreqLayer1
```

## Источники

См. [Literature-References.md](../Literature-References.md): **neuromodeler.ru**, **15**, **[A]**.

---

## EN

## NNeuronFreqGroupLayer — frequency group layer (EN)

Layer of frequency groups aggregating activity across groups.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroupLayer
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronFreqGroupLayer
    In-->>L: signals
    L-->>In: groups activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronFreqGroupLayer]
    layer --> act[Groups activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreatingGroups: Create frequency groups
    CreatingGroups --> LinkingGroups: Link groups
    LinkingGroups --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> LoopGroups: Loop through groups
    LoopGroups --> CalcGroup: Calculate group
    CalcGroup --> AggregateGroups: Aggregate activity
    AggregateGroups --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CheckMode{StructureBuildMode > 0?}
    CheckMode -->|Yes| BuildStructure[BuildStructure]
    CheckMode -->|No| LoopGroups[Loop through groups]
    BuildStructure --> CreateGroups[Create frequency groups]
    CreateGroups --> LinkGroups[Link groups]
    LinkGroups --> LoopGroups
    LoopGroups --> CalcGroup[Calculate group]
    CalcGroup --> AggregateActivity[Aggregate activity]
    AggregateActivity --> CheckMore{More groups?}
    CheckMore -->|Yes| LoopGroups
    CheckMore -->|No| End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UNet["UNet Base"]
        BaseNet[UNet]
    end

    subgraph NNeuronFreqGroupLayer["NNeuronFreqGroupLayer"]
        Layer[Frequency group layer]
        FreqGroups["NNeuronFreqGroup<br/>Groups grid<br/>Height x Width"]
    end

    subgraph External["External Components"]
        InputSource[Input source]
        OutputTarget[Output target]
    end

    BaseNet -->|inherits| NNeuronFreqGroupLayer
    NNeuronFreqGroupLayer -->|creates| FreqGroups
    NNeuronFreqGroupLayer -->|manages| Layer
    InputSource -->|signals| NNeuronFreqGroupLayer
    NNeuronFreqGroupLayer -->|aggregated activity| OutputTarget
```

### Properties

- `StructureBuildMode` — structure rebuild mode (0 — do not rebuild, 1 — rebuild)
- `AffNeuronGroupClassName` — afferent neuron group class name (default "NNeuronFreqGroup")
- `AffNeuronsGroupHeight` — group layer height (row count)
- `AffNeuronsGroupWidth` — group layer width (column count)
- `NumAffNeuronsInGroup` — number of afferent neurons in group

### Methods

- `SetStructureBuildMode(value)` — setting structure rebuild mode
- `SetAffNeuronGroupClassName(value)` — setting group class name
- `SetAffNeuronsGroupHeight(value)` — setting group layer height
- `SetAffNeuronsGroupWidth(value)` — setting group layer width
- `SetNumAffNeuronsInGroup(value)` — setting number of neurons in group
- `ADefault()` — setting default parameters
- `ABuild()` — building group layer structure
- `ACalculate()` — calculation step for all groups
- `BuildStructure()` — building frequency group grid

### Usage in configurations

`NNeuronFreqGroupLayer` is used for creating layers of frequency groups:

- **Frequency group layers**: `Bin/Configs/*/Model_*.xml` (where frequency group layers are required)
- **Multi-layer frequency analysis**: experiments with multi-layer frequency-based analysis
- **Frequency processing**: processing signals with frequency characteristics in layers

**Features:**
- Automatic structure building: creates frequency groups in a grid (Height × Width)
- Flexible configuration: supports various group types
- Activity aggregation: aggregates activity from all groups in the layer

**Typical parameter values:**
- **AffNeuronGroupClassName**: "NNeuronFreqGroup" (frequency neuron group)
- **AffNeuronsGroupHeight**: 1-5 (number of rows)
- **AffNeuronsGroupWidth**: 1-10 (number of columns)
- **NumAffNeuronsInGroup**: 5-20 (number of afferent neurons per group)

### References

See [Literature-References.md](../Literature-References.md): **neuromodeler.ru**, **15**, **[A]**.
