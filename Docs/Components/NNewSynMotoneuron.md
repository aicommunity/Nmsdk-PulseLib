# NNewSynMotoneuron — новый син-мотонейрон

## RU

### Назначение

**Класс**: `NNewSynMotoneuron` — конфигурационный вариант нового мотонейрона с оптимизированными синапсами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynMotoneuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNewSynMotoneuron"` в `Bin/Configs/*/Model_*.xml`.

`NNewSynMotoneuron` является конфигурационным вариантом базового класса `NPulseNeuron` для моделирования мотонейронов с оптимизированными синапсами. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 3` — три части сомы
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов

**Использование:** Моделирование двигательных систем с оптимизированными синапсами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynMotoneuron : configuration variant
    NNewSynMotoneuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynMotoneuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### Свойства

`NNewSynMotoneuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 3`
- `MembraneClassName = "NPSynNeuronMembrane"`

### Методы

`NNewSynMotoneuron` использует все методы базового класса `NPulseNeuron`.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NNewMotoneuron`](NNewMotoneuron.md) — новый мотонейрон
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewSynMotoneuron` — configuration variant of new motor neuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewSynMotoneuron", ...)`.  
**Instances**: `ClassName = "NNewSynMotoneuron"` in `Bin/Configs/*/Model_*.xml`.

`NNewSynMotoneuron` is a configuration variant of the base class `NPulseNeuron` for modeling motor neurons with optimized synapses. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

**Usage:** Modeling motor systems with optimized synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynMotoneuron : configuration variant
    NNewSynMotoneuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynMotoneuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NNewSynMotoneuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Muscle as NMuscle
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent() (3 soma parts)
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate() (3 parts)
        Neuron->>LTZone: ACalculate()
        Neuron-->>Muscle: Motor output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> Building: Build()
    Building --> CreateMembrane: Create NPSynNeuronMembrane (3 parts)
    CreateMembrane --> CreateLTZone: Create LT-zone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Calculate membrane (3 parts)
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> CheckThreshold: Check threshold
    CheckThreshold -->|Reached| MotorSpike: Generate motor spike
    CheckThreshold -->|Not reached| Ready: Step completed
    MotorSpike --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate NPSynNeuronMembrane<br/>3 soma parts]
    CalcMembrane --> CalcSynapses[Calculate optimized synapses]
    CalcSynapses --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateMotorSpike[Generate motor spike]
    CheckThreshold -->|No| NoSpike[No spike]
    GenerateMotorSpike --> SendToMuscle[Send to muscle]
    NoSpike --> SendToMuscle
    SendToMuscle --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewSynMotoneuron["NNewSynMotoneuron Configuration"]
        Membrane[NPSynNeuronMembrane<br/>3 soma parts<br/>optimized for synapses]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses<br/>optimized]
        PreNeurons[Presynaptic neurons]
        Muscle[NMuscle]
    end
    
    BaseNeuron -->|configured as| NNewSynMotoneuron
    NNewSynMotoneuron -->|creates| Membrane
    NNewSynMotoneuron -->|creates| LTZone
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|motor activity| Muscle
```

### Properties

`NNewSynMotoneuron` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

### Methods

`NNewSynMotoneuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewSynMotoneuron` is used in motor system experiments with optimized synapses:

- **Motor systems with optimized synapses**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where motor neurons with optimized synapses are required)

**Typical parameter values:**
- **NumSomaMembraneParts**: 3 (three soma parts)
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane optimized for synapses)

**Features:**
- Synapse optimization: membrane is specifically optimized for efficient synapse processing
- Motor neuron: converts neural signals into motor activity
- Large structure: uses three soma parts for complex motor control

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NNewMotoneuron`](NNewMotoneuron.md) — new motor neuron
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [Architecture.md](../Architecture.md) — library architecture
