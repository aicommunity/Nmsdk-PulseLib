## NNeuronsLayer — слой нейронов (Nmsdk-PulseLib)

**Класс**: `NNeuronsLayer` — слой нейронов (базовая версия).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronsLayer", ...)`.  
**Storage**: `ClassName = "NNeuronsLayer"`.

### Lifecycle
- **ADefault**: параметры слоя (размер, тип нейронов).
- **ABuild**: создание/подключение нейронов.
- **AReset**: сброс состояний слоя.
- **ACalculate**: шаг обновления всех нейронов слоя.

### I/O
- Вход: входные сигналы слоя.
- Выход: активности/спайки слоя.

```mermaid
classDiagram
    UComponent <|-- NNeuronsLayer
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronsLayer
    In-->>L: signals
    L->>L: ACalculate()
    L-->>In: layer activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronsLayer]
    layer --> act[Layer activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronsLayer
Name = Layer1
```

---

## NNeuronsLayer — neurons layer (EN)

Neuron layer that updates all contained neurons.

```mermaid
classDiagram
    UComponent <|-- NNeuronsLayer
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant L as NNeuronsLayer
    In-->>L: signals
    L-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> layer[NNeuronsLayer]
    layer --> act[Layer activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
