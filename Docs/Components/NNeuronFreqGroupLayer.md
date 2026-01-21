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

### Config snippet

```ini
[Component]
ClassName = NNeuronFreqGroupLayer
Name = FreqLayer1
```

---

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
