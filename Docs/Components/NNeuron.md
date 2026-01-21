## NNeuron — базовый нейрон

**Класс**: `NNeuron` — базовый класс нейрона (абстрактная/общая логика).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNeuron"` (обычно используется через наследников).

### Lifecycle
- **ADefault**: установка параметров нейрона по умолчанию.
- **ABuild**: подключение входных каналов/синапсов.
- **AReset**: сброс мембранного состояния/счётчиков.
- **ACalculate**: расчёт одного шага состояния/выхода.

### I/O
- Вход: токи/импульсы (через каналы/синапсы).
- Выход: активность/спайк/потенциал (в зависимости от реализации).

```mermaid
classDiagram
    UComponent <|-- NNeuron
```

Диаграмма подчёркивает базовую роль `NNeuron` как компонента нейронной динамики.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuron
    In-->>N: current/spikes
    N->>N: ACalculate()
    N-->>In: activity/spike
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    in[Current/spikes] --> n[NNeuron]
    n --> out[Activity/spike]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuron
Name = NeuronBase1
```

---

## NNeuron — base neuron (EN)

Base neuron component; typically used via derived neuron models.

```mermaid
classDiagram
    UComponent <|-- NNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNeuron
    In-->>N: current/spikes
    N-->>In: activity/spike
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    in[Current/spikes] --> n[NNeuron]
    n --> out[Activity/spike]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
