## NSynapseTrainerStdpClassicIntegrated — компонент PulseLib

**Класс**: `NSynapseTrainerStdpClassicIntegrated` — компонент PulseLib (см. реализацию в `NPulseLibrary.cpp`).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynapseTrainerStdpClassicIntegrated", ...)`.  
**Storage**: `ClassName = "NSynapseTrainerStdpClassicIntegrated"` в `ClDesc`/`Configs`.

### Lifecycle
- **ADefault**: установка параметров по умолчанию.
- **ABuild**: подключение входов/выходов, подготовка внутренних структур.
- **AReset**: сброс внутренних состояний/счётчиков.
- **ACalculate**: выполнение шага расчёта (интеграция/передача/обновление).

### I/O (UProperty)
- **Входы**: сигналы/токи/спайки или данные (зависят от роли компонента).
- **Выходы**: потенциалы/спайки/активности или преобразованные данные.

```mermaid
classDiagram
    UComponent <|-- NSynapseTrainerStdpClassicIntegrated
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NSynapseTrainerStdpClassicIntegrated
    In-->>X: signals
    X->>X: ACalculate()
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NSynapseTrainerStdpClassicIntegrated]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet
```ini
[Component]
ClassName = NSynapseTrainerStdpClassicIntegrated
Name = NSynapseTrainerStdpClassicIntegrated1
```

---

## NSynapseTrainerStdpClassicIntegrated — component PulseLib (EN)

**Class**: `NSynapseTrainerStdpClassicIntegrated` — PulseLib component (see implementation in `NPulseLibrary.cpp`).

- **Registration**: `UploadClass("NSynapseTrainerStdpClassicIntegrated", ...)` in `NPulseLibrary.cpp`.  
- **Storage**: `ClassName = "NSynapseTrainerStdpClassicIntegrated"` in configs.

### Lifecycle
- **ADefault**: set default parameters.
- **ABuild**: wire inputs/outputs and internal state.
- **AReset**: reset internal state/counters.
- **ACalculate**: perform one calculation step.

### I/O (UProperty)
- **Inputs**: signals/currents/spikes or data (depends on role).
- **Outputs**: potentials/spikes/activities or transformed data.

```mermaid
classDiagram
    UComponent <|-- NSynapseTrainerStdpClassicIntegrated
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NSynapseTrainerStdpClassicIntegrated
    In-->>X: signals
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NSynapseTrainerStdpClassicIntegrated]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).
