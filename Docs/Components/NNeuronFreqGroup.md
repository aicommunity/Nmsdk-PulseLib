## NNeuronFreqGroup — группа частотных нейронов

**Класс**: `NNeuronFreqGroup` — группирует нейроны по частотным признакам.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNeuronFreqGroup", ...)`.  
**Storage**: `ClassName = "NNeuronFreqGroup"`.

### Lifecycle
- **ADefault**: параметры группировки/частот.
- **ABuild**: создание/подключение нейронов группы.
- **AReset**: сброс состояний.
- **ACalculate**: обновление частотной активности группы.

### I/O
- Вход: сигналы/стимулы.
- Выход: частотные признаки/активности.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroup
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant G as NNeuronFreqGroup
    In-->>G: signals
    G-->>In: freq activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> g[NNeuronFreqGroup]
    g --> freq[Freq activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NNeuronFreqGroup
Name = FreqGroup1
```

---

## NNeuronFreqGroup — neuron frequency group (EN)

Groups neurons by frequency features and outputs frequency activity.

```mermaid
classDiagram
    UComponent <|-- NNeuronFreqGroup
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant G as NNeuronFreqGroup
    In-->>G: signals
    G-->>In: freq activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> g[NNeuronFreqGroup]
    g --> freq[Freq activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).
