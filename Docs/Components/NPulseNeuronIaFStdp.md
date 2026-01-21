## NPulseNeuronIaFStdp — компонент PulseLib

**Класс**: `NPulseNeuronIaFStdp` — компонент PulseLib (см. реализацию в `NPulseLibrary.cpp`).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPulseNeuronIaFStdp", ...)`.  
**Storage**: `ClassName = "NPulseNeuronIaFStdp"` в `ClDesc`/`Configs`.

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
    UComponent <|-- NPulseNeuronIaFStdp
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPulseNeuronIaFStdp
    In-->>X: signals
    X->>X: ACalculate()
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NPulseNeuronIaFStdp]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet
```ini
[Component]
ClassName = NPulseNeuronIaFStdp
Name = NPulseNeuronIaFStdp1
```

---

## NPulseNeuronIaFStdp — component PulseLib (EN)

**Class**: `NPulseNeuronIaFStdp` — PulseLib component (see implementation in `NPulseLibrary.cpp`).

- **Registration**: `UploadClass("NPulseNeuronIaFStdp", ...)` in `NPulseLibrary.cpp`.  
- **Storage**: `ClassName = "NPulseNeuronIaFStdp"` in configs.

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
    UComponent <|-- NPulseNeuronIaFStdp
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NPulseNeuronIaFStdp
    In-->>X: signals
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NPulseNeuronIaFStdp]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).
