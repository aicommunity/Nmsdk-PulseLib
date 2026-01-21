## NNeuronsLayerIaF — слой нейронов IaF

**Класс**: `NNeuronsLayerIaF` — слой integrate-and-fire нейронов.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronsLayerIaF", ...)`.  
**Storage**: `ClassName = "NNeuronsLayerIaF"`.

### Lifecycle
- **ADefault**: параметры слоя IaF.
- **ABuild**: создание и подключение нейронов IaF.
- **AReset**: сброс состояний.
- **ACalculate**: шаг обновления всех нейронов слоя.

### I/O
- Вход: сигналы/токи.
- Выход: спайки/активность слоя.

```mermaid
classDiagram
    UComponent <|-- NNeuronsLayerIaF
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronsLayerIaF
    In-->>L: signals
    L-->>In: spikes/activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronsLayerIaF]
    layer --> act[Spikes/Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronsLayerIaF
Name = LayerIaF1
```

---

## NNeuronsLayerIaF — IaF neuron layer (EN)

Layer of integrate-and-fire neurons.

```mermaid
classDiagram
    UComponent <|-- NNeuronsLayerIaF
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronsLayerIaF
    In-->>L: signals
    L-->>In: spikes/activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronsLayerIaF]
    layer --> act[Spikes/Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
