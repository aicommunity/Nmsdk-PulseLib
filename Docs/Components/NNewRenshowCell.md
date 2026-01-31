# NNewRenshowCell — новая клетка Реншоу

## RU

### Назначение

**Класс**: `NNewRenshowCell` — конфигурационный вариант новой клетки Реншоу (ингибирующий интернейрон) с улучшенной архитектурой.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NNewRenshowCell", ...)`.  
**Storage-инстансы**: `ClassName = "NNewRenshowCell"` в `Bin/Configs/*/Model_*.xml`.

`NNewRenshowCell` является конфигурационным вариантом базового класса `NPulseNeuron` для моделирования клеток Реншоу — ингибирующих интернейронов, которые обеспечивают реципрокное торможение мотонейронов. Создается из `NPulseNeuron` с настройками:
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NPNewNeuronMembrane"` — новая мембрана нейрона
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — новая LT-мембрана нейрона
- `LTZoneClassName = "NPLTZone"` — стандартная LT-зона

Клетки Реншоу получают возбуждающие входы от мотонейронов и обеспечивают ингибирующую обратную связь.

**Использование:** Моделирование реципрокного торможения, двигательных систем

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NNewRenshowCell : configuration variant
    NNewRenshowCell *-- NPNewNeuronMembrane : PulseMembrane
    NNewRenshowCell *-- NPLTZoneNeuronMembrane : LTMembrane
    class NNewRenshowCell {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPNewNeuronMembrane"
        +LTMembraneClassName : string = "NPLTZoneNeuronMembrane"
    }
```

### Свойства

`NNewRenshowCell` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `NumSomaMembraneParts = 1`
- `MembraneClassName = "NPNewNeuronMembrane"`
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"`
- `LTZoneClassName = "NPLTZone"`

### Методы

`NNewRenshowCell` использует все методы базового класса `NPulseNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NRenshowCell`](NRenshowCell.md) — базовая клетка Реншоу
- [`NNewMotoneuron`](NNewMotoneuron.md) — новый мотонейрон
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NNewRenshowCell` — configuration variant of new Renshaw cell (inhibitory interneuron) with improved architecture.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NNewRenshowCell", ...)`.  
**Instances**: `ClassName = "NNewRenshowCell"` in `Bin/Configs/*/Model_*.xml`.

`NNewRenshowCell` is a configuration variant of the base class `NPulseNeuron` for modeling Renshaw cells — inhibitory interneurons that provide reciprocal inhibition of motor neurons. Created from `NPulseNeuron` with settings:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPNewNeuronMembrane"` — new neuron membrane
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — new neuron LT-membrane
- `LTZoneClassName = "NPLTZone"` — standard LT-zone

Renshaw cells receive excitatory inputs from motor neurons and provide inhibitory feedback.

**Usage:** Modeling reciprocal inhibition, motor systems

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NNewRenshowCell : configuration variant
    NNewRenshowCell *-- NPNewNeuronMembrane : PulseMembrane
    NNewRenshowCell *-- NPLTZoneNeuronMembrane : LTMembrane
    NNewRenshowCell *-- NPLTZone : LTZone
    class NNewRenshowCell {
        +NumSomaMembraneParts : int = 1
        +MembraneClassName : string = "NPNewNeuronMembrane"
        +LTMembraneClassName : string = "NPLTZoneNeuronMembrane"
        +LTZoneClassName : string = "NPLTZone"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Cell as NNewRenshowCell
    participant Membrane as NPNewNeuronMembrane
    participant LTMembrane as NPLTZoneNeuronMembrane
    participant LTZone as NPLTZone
    participant Motoneuron as NMotoneuron
    
    Storage->>Cell: New() + Default()
    Storage->>Cell: Build()
    Cell->>Membrane: CreateComponent()
    Cell->>LTMembrane: CreateComponent()
    Cell->>LTZone: CreateComponent()
    loop Each step
        Motoneuron->>Cell: Excitatory input
        Storage->>Cell: Calculate()
        Cell->>Membrane: ACalculate()
        Cell->>LTMembrane: ACalculate()
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
    Building --> CreateMembrane: Create NPNewNeuronMembrane
    CreateMembrane --> CreateLTMembrane: Create NPLTZoneNeuronMembrane
    CreateLTMembrane --> CreateLTZone: Create NPLTZone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> MembraneCalc: Calculate new membrane
    MembraneCalc --> LTMembraneCalc: Calculate LT-membrane
    LTMembraneCalc --> LTZoneCalc: Calculate LT-zone
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
    Start([Start Calculate]) --> ReceiveInput[Receive excitatory input<br/>from motoneuron]
    ReceiveInput --> CalcMembrane[Calculate NPNewNeuronMembrane]
    CalcMembrane --> CalcLTMembrane[Calculate NPLTZoneNeuronMembrane]
    CalcLTMembrane --> CalcLTZone[Calculate NPLTZone]
    CalcLTZone --> CheckThreshold{Threshold reached?}
    CheckThreshold -->|Yes| GenerateInhibitorySpike[Generate inhibitory spike]
    CheckThreshold -->|No| NoSpike[No spike]
    GenerateInhibitorySpike --> SendFeedback[Send inhibitory feedback<br/>to motoneuron]
    NoSpike --> SendFeedback
    SendFeedback --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NNewRenshowCell["NNewRenshowCell Configuration"]
        Membrane[NPNewNeuronMembrane]
        LTMembrane[NPLTZoneNeuronMembrane]
        LTZone[NPLTZone]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses]
        Motoneuron[NMotoneuron]
    end
    
    BaseNeuron -->|configured as| NNewRenshowCell
    NNewRenshowCell -->|creates| Membrane
    NNewRenshowCell -->|creates| LTMembrane
    NNewRenshowCell -->|creates| LTZone
    Motoneuron -->|excitatory input| Synapses
    Synapses -->|current| Membrane
    Membrane -->|potential| LTMembrane
    LTMembrane -->|potential| LTZone
    LTZone -->|inhibitory feedback| Motoneuron
```

### Properties

`NNewRenshowCell` uses all properties of base class `NPulseNeuron` with parameters:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NPNewNeuronMembrane"` — new neuron membrane
- `LTMembraneClassName = "NPLTZoneNeuronMembrane"` — new neuron LT-membrane
- `LTZoneClassName = "NPLTZone"` — standard LT-zone

### Methods

`NNewRenshowCell` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NNewRenshowCell` is used in reciprocal inhibition experiments:

- **Reciprocal inhibition**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where Renshaw cells are required)

**Typical parameter values:**
- **NumSomaMembraneParts**: 1 (one soma part)
- **MembraneClassName**: "NPNewNeuronMembrane" (new membrane)
- **LTMembraneClassName**: "NPLTZoneNeuronMembrane" (new LT-membrane)
- **LTZoneClassName**: "NPLTZone" (standard LT-zone)

**Features:**
- Reciprocal inhibition: provides inhibitory feedback to motor neurons
- New architecture: uses improved membranes for more accurate modeling
- Interneuron: acts as inhibitory interneuron in motor circuits
- LT-membrane: separate membrane for LT-zone with optimized channels

### References

See [Literature-References.md](../Literature-References.md): **19**, **20**, **21**, **22**, **28**, **31**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NRenshowCell`](NRenshowCell.md) — base Renshaw cell
- [`NNewMotoneuron`](NNewMotoneuron.md) — new motor neuron
- [Architecture.md](../Architecture.md) — library architecture
