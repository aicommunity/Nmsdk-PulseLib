## NNeuronLife — нейрон с метриками «жизненности»

**Класс**: `NNeuronLife` — нейрон с расширением Life (учёт метрик/долговечности).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronLife", ...)`.  
**Storage**: `ClassName = "NNeuronLife"`.

### Lifecycle
- **ADefault**: параметры Life.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния/метрик.
- **ACalculate**: обновление активации + метрик Life.

### I/O
- Вход: сигналы/токи.
- Выход: активность + метрики Life.

```mermaid
classDiagram
    NNeuron <|-- NNeuronLife
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuronLife
    In-->>N: signals
    N-->>In: activity/metrics
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNeuronLife]
    n --> out[Activity/metrics]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronLife
Name = NeuronLife1
```

---

## NNeuronLife — life-enabled neuron (EN)

Neuron with Life metrics/behaviour.

```mermaid
classDiagram
    NNeuron <|-- NNeuronLife
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuronLife
    In-->>N: signals
    N-->>In: activity/metrics
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNeuronLife]
    n --> out[Activity/metrics]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
