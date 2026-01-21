## NLPLifeNeuron — LP Life нейрон

**Класс**: `NLPLifeNeuron` — LP-нейрон с расширением Life (метрики/жизненность).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NLPLifeNeuron", ...)`.  
**Storage**: `ClassName = "NLPLifeNeuron"`.

### Lifecycle
- **ADefault**: параметры LP/Life.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс состояния/метрик Life.
- **ACalculate**: шаг LP-модели + обновление метрик Life.

### I/O
- Вход: сигналы/токи.
- Выход: активность/метрики.

```mermaid
classDiagram
    NLPNeuron <|-- NLPLifeNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NLPLifeNeuron
    In-->>N: signals
    N-->>In: activity/metrics
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NLPLifeNeuron]
    n --> act[Activity/metrics]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NLPLifeNeuron
Name = LPLife1
```

---

## NLPLifeNeuron — LP Life neuron (EN)

LP neuron with Life metrics/behaviour.

```mermaid
classDiagram
    NLPNeuron <|-- NLPLifeNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NLPLifeNeuron
    In-->>N: signals
    N-->>In: activity/metrics
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NLPLifeNeuron]
    n --> act[Activity/metrics]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
