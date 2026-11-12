## RU

## NSynTCNNeuron — компонент PulseLib

**Класс**: `NSynTCNNeuron` — компонент PulseLib (см. реализацию в `NPulseLibrary.cpp`).
**Аббревиатуры**: `Syn` — **Syn**apse (синапс); `TCN` — **T**emporal **C**onvolutional **N**etwork (временная сверточная сеть).
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSynTCNNeuron", ...)`.
**Storage**: `ClassName = "NSynTCNNeuron"` в `ClDesc`/`Configs`.

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
    UComponent <|-- NSynTCNNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant X as NSynTCNNeuron
    In-->>X: signals
    X->>X: ACalculate()
    X-->>In: outputs
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> x[NSynTCNNeuron]
    x --> out[Outputs]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet
```ini
[Component]
ClassName = NSynTCNNeuron
Name = NSynTCNNeuron1
```

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **25**, **29**.

---

## EN

## NSynTCNNeuron — component PulseLib (EN)

**Class**: `NSynTCNNeuron` — PulseLib component (see implementation in `NPulseLibrary.cpp`).

- **Registration**: `UploadClass("NSynTCNNeuron", ...)` in `NPulseLibrary.cpp`.
- **Storage**: `ClassName = "NSynTCNNeuron"` in configs.

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
    UNet <|-- UComponent
    UComponent <|-- NSynTCNNeuron
    class UComponent {
        +New() UComponent*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
    class NSynTCNNeuron {
        +New() NSynTCNNeuron*
        +ADefault() bool
        +ABuild() bool
        +AReset() bool
        +ACalculate() bool
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSynTCNNeuron
    participant Inputs as Input Signals
    participant Outputs as Output Signals

    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Neuron: ABuild()
    loop Each step
        Inputs->>Neuron: Input signals
        Storage->>Neuron: Calculate()
        Neuron->>Neuron: ACalculate()
        Neuron->>Neuron: Process signals
        Neuron->>Neuron: Update state
        Neuron-->>Outputs: Output signals
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessInputs: Process input signals
    ProcessInputs --> UpdateState: Update internal state
    UpdateState --> GenerateOutput: Generate output signals
    GenerateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInputs[Receive input signals]
    ReceiveInputs --> ProcessSignals[Process signals]
    ProcessSignals --> UpdateState[Update internal state]
    UpdateState --> CalculateOutput[Calculate output signals]
    CalculateOutput --> SetOutput[Set Output]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph UComponent["UComponent Base"]
        BaseComponent[UComponent]
    end

    subgraph NSynTCNNeuron["NSynTCNNeuron"]
        NeuronCore[Neuron Core]
    end

    subgraph External["External Components"]
        InputSignals[Input Signals]
        OutputTargets[Output Targets]
    end

    BaseComponent -->|inherits| NSynTCNNeuron
    InputSignals -->|Input| NSynTCNNeuron
    NSynTCNNeuron -->|Output| OutputTargets
```

### Properties

`NSynTCNNeuron` uses properties from base class `UComponent`:

**Inherited properties:**
- Standard component properties from `UComponent`

### Methods

`NSynTCNNeuron` uses all methods of base class `UComponent`:
- `ADefault()` → `bool` — set default parameters
- `ABuild()` → `bool` — build component structure
- `AReset()` → `bool` — reset component state
- `ACalculate()` → `bool` — perform calculation step

### Usage in configurations

`NSynTCNNeuron` is used in TCN (Thalamocortical Network) neuron experiments:

- **TCN networks**: `Bin/Configs/*/Model_*.xml` (where TCN neurons are required)
- **Thalamocortical modeling**: Experiments with thalamocortical network neurons

**Typical parameter values:**
- Standard default parameters from `UComponent`

**Features:**
- TCN neuron: Specialized neuron for thalamocortical networks
- Signal processing: Processes input signals and generates output signals
- Network integration: Integrates with TCN network structures

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **25**, **29**.
