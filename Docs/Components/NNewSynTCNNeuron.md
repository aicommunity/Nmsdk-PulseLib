## NNewSynTCNNeuron — новая син. TCN модель

**Класс**: `NNewSynTCNNeuron` — новая TCN-нейрон в син. семействе.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynTCNNeuron", ...)`.  
**Storage**: `ClassName = "NNewSynTCNNeuron"`.

### Lifecycle
- **ADefault**: параметры TCN.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: TCN-активация.

### I/O
- Вход: сигналы/токи.
- Выход: активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NNewSynTCNNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynTCNNeuron
    In-->>N: signals
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynTCNNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewSynTCNNeuron
Name = NewSynTCN1
```

---

## NNewSynTCNNeuron — new synaptic TCN neuron (EN)

New TCN neuron variant in synaptic family.

```mermaid
classDiagram
    NNeuron <|-- NNewSynTCNNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSynTCNNeuron
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSynTCNNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
