# NNewSynRenshowCell — новая син-клетка Реншоу

## RU

### Назначение

**Класс**: `NNewSynRenshowCell` — конфигурационный вариант новой клетки Реншоу с оптимизированными синапсами.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewSynRenshowCell", ...)`.  
**Storage-инстансы**: `ClassName = "NNewSynRenshowCell"` в `Bin/Configs/*/Model_*.xml`.

`NNewSynRenshowCell` является конфигурационным вариантом базового класса `NPulseNeuron` для моделирования клеток Реншоу с оптимизированными синапсами. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NPSynNeuronMembrane"` — мембрана, оптимизированная для синапсов

**Использование:** Моделирование реципрокного торможения с оптимизированными синапсами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynRenshowCell : configuration variant
    NNewSynRenshowCell *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynRenshowCell {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### Свойства

`NNewSynRenshowCell` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 1`
- `MembraneClassName = "NPSynNeuronMembrane"`

### Методы

`NNewSynRenshowCell` использует все методы базового класса `NPulseNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NNewRenshowCell`](NNewRenshowCell.md) — новая клетка Реншоу
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — мембрана, оптимизированная для синапсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewSynRenshowCell` — configuration variant of new Renshaw cell with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewSynRenshowCell", ...)`.  
**Instances**: `ClassName = "NNewSynRenshowCell"` in `Bin/Configs/*/Model_*.xml`.

`NNewSynRenshowCell` is a configuration variant of the base class `NPulseNeuron` for modeling Renshaw cells with optimized synapses. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

**Usage:** Modeling reciprocal inhibition with optimized synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewSynRenshowCell : configuration variant
    NNewSynRenshowCell *-- NPSynNeuronMembrane : PulseMembrane
    class NNewSynRenshowCell {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPSynNeuronMembrane"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Cell as NNewSynRenshowCell
    participant Membrane as NPSynNeuronMembrane
    participant LTZone as NPulseLTZoneCommon
    participant Motoneuron as NMotoneuron
    
    Storage->>Cell: New() + Default()
    Storage->>Cell: Build()
    Cell->>Membrane: CreateComponent()
    Cell->>LTZone: CreateComponent()
    loop Each step
        Motoneuron->>Cell: Excitatory input
        Storage->>Cell: Calculate()
        Cell->>Membrane: ACalculate()
        Cell->>LTZone: ACalculate()
        Cell-->>Motoneuron: Inhibitory feedback
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
    LTZoneCalc --> CheckThreshold: Check threshold
    CheckThreshold -->|Reached| InhibitorySpike: Generate inhibitory spike
    CheckThreshold -->|Not reached| Ready: Step completed
    InhibitorySpike --> Ready
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInput["Receive excitatory input<br/>from motoneuron"]
    ReceiveInput --> CalcMembrane[Calculate NPSynNeuronMembrane]
    CalcMembrane --> CalcSynapses[Calculate optimized synapses]
    CalcSynapses --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateInhibitorySpike[Generate inhibitory spike]
    CheckThreshold -->|No| NoSpike[No spike]
    GenerateInhibitorySpike --> SendFeedback["Send inhibitory feedback<br/>to motoneuron"]
    NoSpike --> SendFeedback
    SendFeedback --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewSynRenshowCell["NNewSynRenshowCell Configuration"]
        Membrane["NPSynNeuronMembrane<br/>optimized for synapses"]
        LTZone[NPulseLTZoneCommon]
    end
    
    subgraph External["External Components"]
        Synapses["Synapses<br/>optimized"]
        Motoneuron[NMotoneuron]
    end
    
    BaseNeuron -->|configured as| NNewSynRenshowCell
    NNewSynRenshowCell -->|creates| Membrane
    NNewSynRenshowCell -->|creates| LTZone
    Motoneuron -->|excitatory input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|inhibitory feedback| Motoneuron
```

### Properties

`NNewSynRenshowCell` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPSynNeuronMembrane"` — membrane optimized for synapses

### Methods

`NNewSynRenshowCell` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewSynRenshowCell` is used in reciprocal inhibition experiments:

- **Reciprocal inhibition**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where Renshaw cells with optimized synapses are required)

**Typical parameter values:**
- **NumSomaMembraneParts**: 1 (one soma part)
- **MembraneClassName**: "NPSynNeuronMembrane" (membrane optimized for synapses)

**Features:**
- Reciprocal inhibition: provides inhibitory feedback to motor neurons
- Synapse optimization: membrane optimized for efficient synapse processing
- Interneuron: acts as inhibitory interneuron in motor circuits

### References

See [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NNewRenshowCell`](NNewRenshowCell.md) — new Renshaw cell
- [`NPSynNeuronMembrane`](NPSynNeuronMembrane.md) — membrane optimized for synapses
- [Architecture.md](../Architecture.md) — library architecture
