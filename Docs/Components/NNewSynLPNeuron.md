# NNewSynLPNeuron — новый крупный син-нейрон

## RU

### Назначение

**Класс**: `NNewSynLPNeuron` — конфигурационный вариант нового крупного импульсного нейрона с оптимизированными синапсами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynLPNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNewSynLPNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NNewSynLPNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с мембраной, оптимизированной для синапсов. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 3` — три части сомы
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов

**Использование:** Эксперименты с оптимизированными синапсами для крупных нейронов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynLPNeuron : configuration variant
    NNewSynLPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynLPNeuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### Свойства

`NNewSynLPNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 3`
- `MembraneClassName = "NPSynNeuronMembrane"`

### Методы

`NNewSynLPNeuron` использует все методы базового класса `NPulseNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NNewLPNeuron`](NNewLPNeuron.md) — новый крупный нейрон
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewSynLPNeuron` — configuration variant of new large spiking neuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewSynLPNeuron", ...)`.  
**Instances**: `ClassName = "NNewSynLPNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NNewSynLPNeuron` is a configuration variant of the base class `NPulseNeuron` with membrane optimized for synapses. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

**Usage:** Experiments with optimized synapses for large neurons

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynLPNeuron : configuration variant
    NNewSynLPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynLPNeuron {
        +NumSomaMembraneParts : int = 3
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NNewSynLPNeuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent() (3 soma parts)
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate() (3 parts)
        Neuron->>LTZone: ACalculate()
        Neuron-->>Storage: Output
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
    LTZoneCalc --> Ready: Step completed
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
    CheckThreshold -->|Yes| GenerateSpike[Generate spike]
    CheckThreshold -->|No| UpdateOutput[Update Output]
    GenerateSpike --> UpdateOutput
    UpdateOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewSynLPNeuron["NNewSynLPNeuron Configuration"]
        Membrane[NPSynNeuronMembrane<br/>3 soma parts<br/>optimized for synapses]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses<br/>optimized]
        PreNeurons[Presynaptic neurons]
    end
    
    BaseNeuron -->|configured as| NNewSynLPNeuron
    NNewSynLPNeuron -->|creates| Membrane
    NNewSynLPNeuron -->|creates| LTZone
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|Output| NNewSynLPNeuron
```

### Properties

`NNewSynLPNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 3` — three soma parts
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

### Methods

`NNewSynLPNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewSynLPNeuron` is used in experiments with optimized synapses for large neurons:

- **Optimized synapses for large neurons**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where synapse-optimized large neurons are required)

**Typical parameter values:**
- **NumSomaMembraneParts**: 3 (three soma parts for large neurons)
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane optimized for synapses)

**Features:**
- Synapse optimization: membrane is specifically optimized for efficient synapse processing
- Large neuron: uses three soma parts for extended structure
- Efficient processing: optimized membrane provides better performance for synaptic inputs in large neurons

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NNewLPNeuron`](NNewLPNeuron.md) — new large neuron
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [Architecture.md](../Architecture.md) — library architecture
