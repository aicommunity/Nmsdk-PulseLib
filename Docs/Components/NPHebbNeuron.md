## NPHebbNeuron — Hebb нейрон

**Класс**: `NPHebbNeuron` — нейрон с Hebb-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPHebbNeuron", ...)`.  
**Storage**: `ClassName = "NPHebbNeuron"`.

### Lifecycle
- **ADefault**: параметры Hebb.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс весов.
- **ACalculate**: активация + Hebb-обновление весов.

### I/O
- Вход: сигналы/токи.
- Выход: активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NPHebbNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NPHebbNeuron
    In-->>N: signals
    N->>N: Hebb update
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NPHebbNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NPHebbNeuron
Name = HebbNeuron1
```

---

## NPHebbNeuron — Hebbian neuron (EN)

Neuron applying Hebbian plasticity to its weights.

```mermaid
classDiagram
    NNeuron <|-- NPHebbNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NPHebbNeuron
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NPHebbNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
