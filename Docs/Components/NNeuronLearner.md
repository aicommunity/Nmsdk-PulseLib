## NNeuronLearner — обучающийся нейрон

**Класс**: `NNeuronLearner` — нейрон с функциями самообучения.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronLearner", ...)`.  
**Storage**: `ClassName = "NNeuronLearner"`.

### Lifecycle
- **ADefault**: параметры обучения.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния/весов.
- **ACalculate**: шаг расчёта + обновление по правилу обучения.

### I/O
- Вход: сигналы/ошибка (при наличии).
- Выход: активность/обновлённые веса (внутренне).

```mermaid
classDiagram
    NNeuron <|-- NNeuronLearner
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuronLearner
    In-->>N: signals
    N->>N: learn + ACalculate()
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNeuronLearner]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronLearner
Name = Learner1
```

---

## NNeuronLearner — learning neuron (EN)

Self-learning neuron applying its learning rule during calculation.

```mermaid
classDiagram
    NNeuron <|-- NNeuronLearner
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuronLearner
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNeuronLearner]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
