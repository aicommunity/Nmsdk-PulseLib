## NCNeuron — классический нейрон (Nmsdk-PulseLib)

**Класс**: `NCNeuron` — классический нейрон (непульсовый), вариант базовой модели.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCNeuron", ...)`.  
**Storage**: `ClassName = "NCNeuron"`.

### Lifecycle
- **ADefault**: параметры мембраны/активации.
- **ABuild**: подключение входов/выходов.
- **AReset**: сброс состояния.
- **ACalculate**: вычисление активации/выхода.

### I/O
- Вход: токи/сигналы.
- Выход: активация/потенциал (классический формат).

```mermaid
classDiagram
    NNeuron <|-- NCNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NCNeuron
    In-->>N: signals
    N->>N: ACalculate()
    N-->>In: activation
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    in[Signals] --> n[NCNeuron]
    n --> out[Activation]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NCNeuron
Name = CNeuron1
```

---

## NCNeuron — classic neuron (EN)

Non-spiking/classic neuron variant.

```mermaid
classDiagram
    NNeuron <|-- NCNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NCNeuron
    In-->>N: signals
    N-->>In: activation
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    in[Signals] --> n[NCNeuron]
    n --> out[Activation]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
