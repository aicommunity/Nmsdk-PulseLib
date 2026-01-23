## NContinuesSynLPNeuron — непрерывный LP нейрон (syn)

**Класс**: `NContinuesSynLPNeuron` — непрерывный LP-нейрон в син. семействе.  
**Регистрация**: `UploadClass("NContinuesSynLPNeuron", ...)` в `NPulseLibrary.cpp`.  
**Storage**: `ClassName = "NContinuesSynLPNeuron"`.

### Lifecycle
- **ADefault**: параметры LP.
- **ABuild**: подключение входов/синапсов.
- **AReset**: сброс.
- **ACalculate**: непрерывный LP-расчёт.

### I/O
- Вход: сигналы/токи.
- Выход: активность/спайк.

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynLPNeuron
```

Пояснение: диаграмма классов показывает место компонента в иерархии и ключевые связи.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynLPNeuron
    In-->>N: signals
    N-->>In: activity
```

Пояснение: диаграмма последовательности показывает типовой сценарий взаимодействия и порядок вызовов.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynLPNeuron]
    n --> act[Activity]
```

Пояснение: блок-схема показывает поток данных/сигналов (входы → компонент → выходы).

### Config snippet

```ini
[Component]
ClassName = NContinuesSynLPNeuron
Name = CSLP1
```

---

## NContinuesSynLPNeuron — continuous LP neuron (EN)

Continuous LP neuron variant (synaptic family).

```mermaid
classDiagram
    NNeuron <|-- NContinuesSynLPNeuron
```

Description: this class diagram shows the component position in the type hierarchy and key relations.

```mermaid
sequenceDiagram
    participant In as Inputs
    participant N as NContinuesSynLPNeuron
    In-->>N: signals
    N-->>In: activity
```

Description: this sequence diagram shows a typical runtime interaction and call order.

```mermaid
flowchart LR
    sig[Signals] --> n[NContinuesSynLPNeuron]
    n --> act[Activity]
```

Description: this flowchart shows the data/signal flow (inputs → component → outputs).

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ReceiveInputs: Receive input signals
    ReceiveInputs --> ProcessLP: Process LP calculation
    ProcessLP --> GenerateActivity: Generate activity
    GenerateActivity --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NNeuron["NNeuron Base"]
        BaseNeuron[NNeuron]
    end
    
    subgraph NContinuesSynLPNeuron["NContinuesSynLPNeuron"]
        LPProcessing[LP Processing<br/>Continuous]
    end
    
    subgraph External["External Components"]
        InputSignals[Input Signals]
        ClassicNeurons[Classic Neurons]
    end
    
    BaseNeuron -->|inherits| NContinuesSynLPNeuron
    InputSignals -->|signals| NContinuesSynLPNeuron
    NContinuesSynLPNeuron -->|activity| ClassicNeurons
    NContinuesSynLPNeuron -->|Output| External
```

### Properties

`NContinuesSynLPNeuron` uses all properties of base class `NNeuron`.

### Methods

`NContinuesSynLPNeuron` uses all methods of base class `NNeuron`:
- `ADefault()` → `bool` — sets LP parameters
- `ABuild()` → `bool` — connects inputs/synapses
- `AReset()` → `bool` — resets states
- `ACalculate()` → `bool` — performs continuous LP calculation

### Usage in configurations

`NContinuesSynLPNeuron` is used in continuous LP neuron experiments:

- **Continuous LP neurons**: Continuous LP neuron variant in synaptic family
- **Classic models**: Used in classic (non-spiking) neural network models

**Features:**
- Continuous processing: Performs continuous LP calculation
- Synaptic family: Part of synaptic neuron family
- Classic model: Optimized for classic (non-spiking) models
