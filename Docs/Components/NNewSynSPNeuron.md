# NNewSynSPNeuron — новый мелкий син-нейрон

## RU

### Назначение

**Класс**: `NNewSynSPNeuron` — конфигурационный вариант нового мелкого импульсного нейрона с оптимизированными синапсами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynSPNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NNewSynSPNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NNewSynSPNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с мембраной, оптимизированной для синапсов. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов

**Использование:** Эксперименты с оптимизированными синапсами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynSPNeuron : configuration variant
    NNewSynSPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynSPNeuron {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### Свойства

`NNewSynSPNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 1`
- `MembraneClassName = "NPSynNeuronMembrane"`

### Методы

`NNewSynSPNeuron` использует все методы базового класса `NPulseNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NNewSPNeuron`](NNewSPNeuron.md) — новый мелкий нейрон
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewSynSPNeuron` — configuration variant of new small spiking neuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewSynSPNeuron", ...)`.  
**Instances**: `ClassName = "NNewSynSPNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NNewSynSPNeuron` is a configuration variant of the base class `NPulseNeuron` with membrane optimized for synapses. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

**Usage:** Experiments with optimized synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynSPNeuron : configuration variant
    NNewSynSPNeuron *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynSPNeuron {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NNewSynSPNeuron
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Membrane: CreateComponent()
    Neuron->>LTZone: CreateComponent()
    loop Each step
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: ACalculate()
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
    Building --> CreateMembrane: Create NPSynNeuronMembrane
    CreateMembrane --> CreateLTZone: Create LT-zone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Calculate membrane
    MembraneCalc --> LTZoneCalc: Calculate LT-zone
    LTZoneCalc --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcMembrane[Calculate NPSynNeuronMembrane]
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
    
    subgraph NNewSynSPNeuron["NNewSynSPNeuron Configuration"]
        Membrane[NPSynNeuronMembrane<br/>optimized for synapses]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses<br/>optimized]
        PreNeurons[Presynaptic neurons]
    end
    
    BaseNeuron -->|configured as| NNewSynSPNeuron
    NNewSynSPNeuron -->|creates| Membrane
    NNewSynSPNeuron -->|creates| LTZone
    PreNeurons -->|Input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|Output| NNewSynSPNeuron
```

### Properties

`NNewSynSPNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

### Methods

`NNewSynSPNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewSynSPNeuron` is used in experiments with optimized synapses:

- **Optimized synapses**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where synapse-optimized neurons are required)

**Typical parameter values:**
- **NumSomaMembraneParts**: 1 (one soma part for small neurons)
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane optimized for synapses)

**Features:**
- Synapse optimization: membrane is specifically optimized for efficient synapse processing
- Small neuron: uses one soma part for compact structure
- Efficient processing: optimized membrane provides better performance for synaptic inputs

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NNewSPNeuron`](NNewSPNeuron.md) — new small neuron
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [Architecture.md](../Architecture.md) — library architecture
