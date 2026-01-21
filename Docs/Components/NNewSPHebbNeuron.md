## NNewSPHebbNeuron — новая SP Hebb модель

**Класс**: `NNewSPHebbNeuron` — новая реализация SP-нейрона с Hebb-пластичностью.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSPHebbNeuron", ...)`.  
**Storage**: `ClassName = "NNewSPHebbNeuron"`.

### Lifecycle
- **ADefault**: параметры SP + Hebb.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: SP-активация + Hebb-обновление.

### I/O
- Вход: сигналы/токи.
- Выход: активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NNewSPHebbNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSPHebbNeuron
    In-->>N: signals
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSPHebbNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewSPHebbNeuron
Name = NewSPHebb1
```

---

## NNewSPHebbNeuron — new SP Hebbian neuron (EN)

New SP neuron with Hebbian plasticity.

```mermaid
classDiagram
    NNeuron <|-- NNewSPHebbNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSPHebbNeuron
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSPHebbNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
