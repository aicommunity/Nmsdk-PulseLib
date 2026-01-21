## NNeuronTrainer — тренер нейронов

**Класс**: `NNeuronTrainer` — обучает нейроны по заданному правилу/данным.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronTrainer", ...)`.  
**Storage**: `ClassName = "NNeuronTrainer"`.

### Lifecycle
- **ADefault**: параметры обучения.
- **ABuild**: подключение целевых нейронов/данных.
- **AReset**: сброс состояния обучения.
- **ACalculate**: обновление нейронов по правилу.

### I/O
- Вход: данные/ошибки/активность нейронов.
- Выход: обновлённые веса/состояния (внутренне), метрики обучения.

```mermaid
classDiagram
    UComponent <|-- NNeuronTrainer
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant Data as Data
    participant T as NNeuronTrainer
    participant N as Neurons
    Data-->>T: samples/errors
    N-->>T: activity
    T->>T: ACalculate()
    T-->>N: updated weights
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    data[Training data] --> tr[NNeuronTrainer]
    act[Neurons activity] --> tr
    tr --> upd[Updated weights]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronTrainer
Name = NeuronTrainer1
```

---

## NNeuronTrainer — neuron trainer (EN)

Updates neurons using provided training data/errors.

```mermaid
classDiagram
    UComponent <|-- NNeuronTrainer
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant Data as Data
    participant T as NNeuronTrainer
    participant N as Neurons
    Data-->>T: samples/errors
    N-->>T: activity
    T-->>N: updated weights
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    data[Training data] --> tr[NNeuronTrainer]
    act[Neurons activity] --> tr
    tr --> upd[Updated weights]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
