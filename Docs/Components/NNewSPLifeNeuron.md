## NNewSPLifeNeuron — новая SP Life модель

**Класс**: `NNewSPLifeNeuron` — новая SP-нейрон с расширением Life.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSPLifeNeuron", ...)`.  
**Storage**: `ClassName = "NNewSPLifeNeuron"`.

### Lifecycle
- **ADefault**: параметры SP/Life.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния/метрик.
- **ACalculate**: SP-активация + обновление метрик Life.

### I/O
- Вход: сигналы/токи.
- Выход: активность/метрики.

```mermaid
classDiagram
    NNeuron <|-- NNewSPLifeNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSPLifeNeuron
    In-->>N: signals
    N-->>In: activity/metrics
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSPLifeNeuron]
    n --> act[Activity/metrics]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNewSPLifeNeuron
Name = NewSPLife1
```

---

## NNewSPLifeNeuron — new SP Life neuron (EN)

New SP neuron with Life metrics/behaviour.

```mermaid
classDiagram
    NNeuron <|-- NNewSPLifeNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NNewSPLifeNeuron
    In-->>N: signals
    N-->>In: activity/metrics
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NNewSPLifeNeuron]
    n --> act[Activity/metrics]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
