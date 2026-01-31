# NSPDendriteLifeHebbNeuron — мелкий живой импульсный нейрон с синапсами Хебба и дендритом

## RU

### Назначение

**Класс**: `NSPDendriteLifeHebbNeuron` — конфигурационный вариант мелкого живого импульсного нейрона с синапсами Хебба и дендритной структурой.  
**Аббревиатура**: `Hebb` — **Hebb**ian (геббовская пластичность, обучение по правилу Хебба).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NSPDendriteLifeHebbNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NSPDendriteLifeHebbNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NSPDendriteLifeHebbNeuron` является конфигурационным вариантом базового класса `NPulseLifeNeuron` с мембраной, поддерживающей синапсы Хебба, и дендритной структурой. Создается из `NPulseLifeNeuron` с настройками:
- `NumDendriteMembraneParts = 3` — три части дендритной мембраны
- `LTMembraneClassName = ""` — без LT-мембраны
- `MembraneClassName = "NPNeuronHebbMembrane"` — мембрана с поддержкой синапсов Хебба

Отличие от `NSPLifeHebbNeuron` заключается в наличии дендритной структуры для более сложного пространственного моделирования.

**Использование:** Моделирование живых нейронов с дендритной структурой и обучением Хебба

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|-- NPulseLifeNeuron
    NPulseLifeNeuron <|.. NSPDendriteLifeHebbNeuron : configuration variant
    NSPDendriteLifeHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NSPDendriteLifeHebbNeuron *-- NPNeuronHebbMembrane[] : DendriteMembranes
    NSPDendriteLifeHebbNeuron *-- NNeuronLife : NeuronLife
    class NPulseLifeNeuron {
        +NumDendriteMembraneParts : int
        +SummaryPosGd : double
        +GetNeuronLife() NNeuronLife*
    }
    class NSPDendriteLifeHebbNeuron {
        +NumDendriteMembraneParts : int = 3
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = ""
    }
```

**Иерархия наследования:**
- `NPulseNeuron` — импульсный нейрон
- `NPulseLifeNeuron` — живой импульсный нейрон
- `NSPDendriteLifeHebbNeuron` — конфигурационный вариант с дендритами и синапсами Хебба

### Свойства

`NSPDendriteLifeHebbNeuron` использует все свойства базового класса `NPulseLifeNeuron` с параметрами:
- `NumDendriteMembraneParts = 3`
- `MembraneClassName = "NPNeuronHebbMembrane"`
- `LTMembraneClassName = ""`

### Методы

`NSPDendriteLifeHebbNeuron` использует все методы базового класса `NPulseLifeNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseLifeNeuron`](NPulseLifeNeuron.md) — живой импульсный нейрон
- [`NSPLifeHebbNeuron`](NSPLifeHebbNeuron.md) — мелкий живой нейрон с синапсами Хебба
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — синапс Хебба
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NSPDendriteLifeHebbNeuron` — configuration variant of small living spiking neuron with Hebbian synapses and dendritic structure.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NSPDendriteLifeHebbNeuron", ...)`.  
**Instances**: `ClassName = "NSPDendriteLifeHebbNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NSPDendriteLifeHebbNeuron` is a configuration variant of the base class `NPulseLifeNeuron` with membrane supporting Hebbian synapses and dendritic structure. Created from `NPulseLifeNeuron` with settings:
- `NumDendriteMembraneParts = 3` — three dendritic membrane parts
- `LTMembraneClassName = ""` — without LT-membrane
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support

Difference from `NSPLifeHebbNeuron` is the presence of dendritic structure for more complex spatial modeling.

**Usage:** Modeling living neurons with dendritic structure and Hebbian learning

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NPulseNeuron
    NPulseNeuron <|-- NPulseLifeNeuron
    NPulseLifeNeuron <|.. NSPDendriteLifeHebbNeuron : configuration variant
    NSPDendriteLifeHebbNeuron *-- NPNeuronHebbMembrane : PulseMembrane
    NSPDendriteLifeHebbNeuron *-- NPNeuronHebbMembrane[] : DendriteMembranes
    NSPDendriteLifeHebbNeuron *-- NNeuronLife : NeuronLife
    class NPulseLifeNeuron {
        +NumDendriteMembraneParts : int
        +SummaryPosGd : double
        +GetNeuronLife() NNeuronLife*
    }
    class NSPDendriteLifeHebbNeuron {
        +NumDendriteMembraneParts : int = 3
        +MembraneClassName : string = "NPNeuronHebbMembrane"
        +LTMembraneClassName : string = ""
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NSPDendriteLifeHebbNeuron
    participant SomaMembrane as NPNeuronHebbMembrane
    participant DendriteMembranes as NPNeuronHebbMembrane[]
    participant LTZone as NPulseLTZoneCommon
    participant NeuronLife as NNeuronLife
    participant Synapses as NPulseHebbSynapse
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Neuron: Set NumDendriteMembraneParts = 3
    Neuron->>Neuron: Set MembraneClassName = "NPNeuronHebbMembrane"
    Neuron->>SomaMembrane: CreateComponent()
    Neuron->>DendriteMembranes: CreateComponent() (3 parts)
    Neuron->>LTZone: CreateComponent()
    Neuron->>NeuronLife: CreateComponent()
    loop Each step
        Synapses->>SomaMembrane: Input signals
        Synapses->>DendriteMembranes: Input signals
        Storage->>Neuron: Calculate()
        Neuron->>SomaMembrane: ACalculate()
        Neuron->>DendriteMembranes: ACalculate()
        SomaMembrane->>LTZone: Soma potential
        DendriteMembranes->>LTZone: Dendrite potentials
        LTZone->>NeuronLife: Output
        NeuronLife->>Neuron: Life metrics
        Neuron-->>Storage: Output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetParams: Set NumDendriteMembraneParts = 3
    SetParams --> Building: Build()
    Building --> CreateSomaMembrane: Create soma membrane
    CreateSomaMembrane --> CreateDendriteMembranes: Create dendrite membranes (3 parts)
    CreateDendriteMembranes --> CreateLTZone: Create LT-zone
    CreateLTZone --> CreateNeuronLife: Create NeuronLife
    CreateNeuronLife --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CalcSomaMembrane: Calculate soma membrane
    CalcSomaMembrane --> CalcDendriteMembranes: Calculate dendrite membranes
    CalcDendriteMembranes --> CalcLTZone: Calculate LT-zone
    CalcLTZone --> UpdateNeuronLife: Update NeuronLife
    UpdateNeuronLife --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> CalcSomaMembrane[Calculate soma membrane<br/>NPNeuronHebbMembrane]
    CalcSomaMembrane --> CalcDendriteMembranes[Calculate dendrite membranes<br/>3 parts]
    CalcDendriteMembranes --> AggregatePotentials[Aggregate potentials<br/>from soma and dendrites]
    AggregatePotentials --> CalcLTZone[Calculate LT-zone]
    CalcLTZone --> UpdateNeuronLife[Update NeuronLife<br/>life metrics]
    UpdateNeuronLife --> CheckLifeMetrics{Life metrics OK?}
    CheckLifeMetrics -->|Yes| UpdateOutput[Update Output]
    CheckLifeMetrics -->|No| AdjustActivity[Adjust activity]
    AdjustActivity --> UpdateOutput
    UpdateOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseLifeNeuron["NPulseLifeNeuron Base"]
        BaseNeuron[NPulseLifeNeuron]
    end
    
    subgraph NSPDendriteLifeHebbNeuron["NSPDendriteLifeHebbNeuron Configuration"]
        SomaMembrane[NPNeuronHebbMembrane<br/>Soma Membrane]
        DendriteMembrane1[NPNeuronHebbMembrane<br/>Dendrite 1]
        DendriteMembrane2[NPNeuronHebbMembrane<br/>Dendrite 2]
        DendriteMembrane3[NPNeuronHebbMembrane<br/>Dendrite 3]
        LTZone[NPulseLTZoneCommon<br/>LTZone]
        NeuronLife[NNeuronLife<br/>NeuronLife]
        Synapses[NPulseHebbSynapse<br/>Synapses]
    end
    
    subgraph External["External Components"]
        PreNeurons[Presynaptic Neurons]
        MotivationSource[Motivation Source]
        EnergySource[Energy Source]
    end
    
    BaseNeuron -->|configured as| NSPDendriteLifeHebbNeuron
    NSPDendriteLifeHebbNeuron -->|creates| SomaMembrane
    NSPDendriteLifeHebbNeuron -->|creates| DendriteMembrane1
    NSPDendriteLifeHebbNeuron -->|creates| DendriteMembrane2
    NSPDendriteLifeHebbNeuron -->|creates| DendriteMembrane3
    NSPDendriteLifeHebbNeuron -->|creates| LTZone
    NSPDendriteLifeHebbNeuron -->|creates| NeuronLife
    PreNeurons -->|Input| Synapses
    MotivationSource -->|InputMotivation| Synapses
    LTZone -->|InputLTZoneFeedbackSignal| Synapses
    Synapses -->|Output| SomaMembrane
    Synapses -->|Output| DendriteMembrane1
    Synapses -->|Output| DendriteMembrane2
    Synapses -->|Output| DendriteMembrane3
    SomaMembrane -->|potential| LTZone
    DendriteMembrane1 -->|potential| LTZone
    DendriteMembrane2 -->|potential| LTZone
    DendriteMembrane3 -->|potential| LTZone
    LTZone -->|Output| NeuronLife
    EnergySource -->|energy| NeuronLife
    NeuronLife -->|metrics| NSPDendriteLifeHebbNeuron
```

### Properties

`NSPDendriteLifeHebbNeuron` uses all properties of base class `NPulseLifeNeuron` with parameters:
- `NumDendriteMembraneParts = 3` — three dendritic membrane parts
- `MembraneClassName = "NPNeuronHebbMembrane"` — membrane with Hebbian synapse support
- `LTMembraneClassName = ""` — without LT-membrane

**Inherited properties from NPulseLifeNeuron:**
- `SummaryPosGd`, `SummaryPosGs`, `SummaryPosG` (double) — summary positive weights
- `SummaryNegGd`, `SummaryNegGs`, `SummaryNegG` (double) — summary negative weights
- `OutputSummaryPosGd`, `OutputSummaryPosGs`, `OutputSummaryPosG` (MDMatrix<double>) — output signals of positive weights
- `OutputSummaryNegGd`, `OutputSummaryNegGs`, `OutputSummaryNegG` (MDMatrix<double>) — output signals of negative weights
- All normalized outputs (`*Norm`)

**Life support parameters (in NeuronLife):**
- `Energy` (double) — current neuron energy
- `Threshold` (double) — life threshold
- `CriticalEnergy` (double) — critical energy level
- `WearOut` (double) — neuron wear out
- `Feel` (double) — neuron feel

**Hebb synapse parameters (in NPulseHebbSynapse):**
- `Min`, `Mout`, `Md` (double) — forgetting constants
- `Kin`, `Kout` (double) — activity coefficients

### Methods

`NSPDendriteLifeHebbNeuron` uses all methods of base class `NPulseLifeNeuron`:
- `GetNeuronLife()` → `NNeuronLife*` — get life support model

### Usage in configurations

`NSPDendriteLifeHebbNeuron` is used in experiments with living neurons, dendritic structure, and Hebbian learning:

- **Dendritic + Life + Hebb**: `Bin/Configs/!OldConfigs/*/Model_*.xml` (where combination of dendritic structure, life support, and Hebbian learning is used)
- **Spatial modeling**: Experiments with spatial neuron modeling using dendritic compartments
- **Complex neurons**: Modeling neurons with complex morphology and learning

**Typical parameter values:**
- **NumDendriteMembraneParts**: 3 (three dendritic membrane parts)
- **MembraneClassName**: "NPNeuronHebbMembrane" (membrane with Hebb support)
- **LTMembraneClassName**: "" (without LT-membrane)

**Features:**
- Dendritic structure: Includes three dendritic compartments for spatial modeling
- Life support: Automatically creates and links NeuronLife model
- Hebbian learning: Uses Hebb synapses for synaptic plasticity
- Combined functionality: Combines dendritic structure, life support, and Hebbian learning

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseLifeNeuron`](NPulseLifeNeuron.md) — living spiking neuron
- [`NSPLifeHebbNeuron`](NSPLifeHebbNeuron.md) — small living neuron with Hebbian synapses
- [`NPulseHebbSynapse`](NPulseHebbSynapse.md) — Hebbian synapse
- [Architecture.md](../Architecture.md) — library architecture
