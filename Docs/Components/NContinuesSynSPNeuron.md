# NContinuesSynSPNeuron — непрерывный син-SP-нейрон

## RU

### Назначение

**Класс**: `NContinuesSynSPNeuron` — конфигурационный вариант мелкого непрерывного импульсного нейрона с оптимизированными синапсами.  
**Префикс**: `NContinues` — **Continues** (Continuous, непрерывный вариант компонента); **Аббревиатуры**: `Syn` — **Syn**apse (синапс); `SP` — **S**pike **P**attern (паттерн спайков).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSynSPNeuron", ...)`.  
**Storage-инстансы**: `ClassName = "NContinuesSynSPNeuron"` в `Bin/Configs/*/Model_*.xml`.

`NContinuesSynSPNeuron` является конфигурационным вариантом базового класса `NPulseNeuron` с непрерывной обработкой и оптимизированными синапсами. Создается из `NCNeuron` с настройками:
- `NumSomaMembraneParts = 1` — одна часть сомы
- `MembraneClassName = "NCSynNeuronMembrane"` — классическая мембрана, оптимизированная для синапсов
- `LTZoneClassName = "NCLTZone"` — классическая LT-зона

Непрерывные нейроны используют классическую модель с непрерывным выходом вместо импульсного.

**Использование:** Эксперименты с непрерывными моделями и оптимизированными синапсами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseNeuron <|.. NContinuesSynSPNeuron : configuration variant
    NContinuesSynSPNeuron *-- NCSynNeuronMembrane : PulseMembrane
    NContinuesSynSPNeuron *-- NCLTZone : LTZone
    class NContinuesSynSPNeuron {
        +MembraneClassName : string = "NCSynNeuronMembrane"
        +LTZoneClassName : string = "NCLTZone"
        +NumSomaMembraneParts : int = 1
    }
```

### Свойства

`NContinuesSynSPNeuron` использует все свойства базового класса `NPulseNeuron` с параметрами:
- `MembraneClassName = "NCSynNeuronMembrane"`
- `LTZoneClassName = "NCLTZone"`
- `NumSomaMembraneParts = 1`

### Методы

`NContinuesSynSPNeuron` использует все методы базового класса `NPulseNeuron`.

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseNeuron`](NPulseNeuron.md) — импульсный нейрон
- [`NSynSPNeuron`](NSynSPNeuron.md) — син-SP-нейрон
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — классическая мембрана, оптимизированная для синапсов
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NContinuesSynSPNeuron` — configuration variant of small continuous spiking neuron with optimized synapses.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NContinuesSynSPNeuron", ...)`.  
**Instances**: `ClassName = "NContinuesSynSPNeuron"` in `Bin/Configs/*/Model_*.xml`.

`NContinuesSynSPNeuron` is a configuration variant of the base class `NPulseNeuron` with continuous processing and optimized synapses. Created from `NCNeuron` with settings:
- `NumSomaMembraneParts = 1` — one soma part
- `MembraneClassName = "NCSynNeuronMembrane"` — classic membrane optimized for synapses
- `LTZoneClassName = "NCLTZone"` — classic LT-zone

Continuous neurons use classic model with continuous output instead of spiking.

**Usage:** Experiments with continuous models and optimized synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseNeuron <|.. NContinuesSynSPNeuron : configuration variant
    NContinuesSynSPNeuron *-- NCSynNeuronMembrane : PulseMembrane
    NContinuesSynSPNeuron *-- NCLTZone : LTZone
    class NContinuesSynSPNeuron {
        +MembraneClassName : string = "NCSynNeuronMembrane"
        +LTZoneClassName : string = "NCLTZone"
        +NumSomaMembraneParts : int = 1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Neuron as NContinuesSynSPNeuron
    participant Membrane as NCSynNeuronMembrane
    participant LTZone as NCLTZone
    participant Inputs as Input Signals
    
    Storage->>Neuron: New() + Default()
    Storage->>Neuron: Build()
    Neuron->>Neuron: Set MembraneClassName = "NCSynNeuronMembrane"
    Neuron->>Neuron: Set LTZoneClassName = "NCLTZone"
    Neuron->>Membrane: Create membrane
    Neuron->>LTZone: Create LT-zone
    loop Each step
        Inputs->>Membrane: Input signals
        Storage->>Neuron: Calculate()
        Neuron->>Membrane: Calculate()
        Membrane->>LTZone: Membrane potential
        LTZone->>Neuron: Continuous output
        Neuron-->>Inputs: Output
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetConfig: Set configuration parameters
    SetConfig --> Building: Build()
    Building --> CreateMembrane: Create NCSynNeuronMembrane
    CreateMembrane --> CreateLTZone: Create NCLTZone
    CreateLTZone --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessMembrane: Process membrane
    ProcessMembrane --> ProcessLTZone: Process LT-zone
    ProcessLTZone --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ProcessMembrane[Process NCSynNeuronMembrane<br/>continuous processing]
    ProcessMembrane --> CalculatePotential[Calculate membrane potential]
    CalculatePotential --> ProcessLTZone[Process NCLTZone<br/>classic LT-zone]
    ProcessLTZone --> CheckThreshold[Check threshold]
    CheckThreshold --> GenerateOutput[Generate continuous output]
    GenerateOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseNeuron["NPulseNeuron Base"]
        BaseNeuron[NPulseNeuron]
    end
    
    subgraph NContinuesSynSPNeuron["NContinuesSynSPNeuron Configuration"]
        Membrane[NCSynNeuronMembrane<br/>Classic Membrane]
        LTZone[NCLTZone<br/>Classic LT-Zone]
    end
    
    subgraph External["External Components"]
        InputSignals[Input Signals]
        ClassicNeurons[Classic Neurons]
    end
    
    BaseNeuron -->|configured as| NContinuesSynSPNeuron
    NContinuesSynSPNeuron -->|creates| Membrane
    NContinuesSynSPNeuron -->|creates| LTZone
    InputSignals -->|signals| Membrane
    Membrane -->|potential| LTZone
    LTZone -->|continuous output| NContinuesSynSPNeuron
    NContinuesSynSPNeuron -->|Output| ClassicNeurons
```

### Properties

`NContinuesSynSPNeuron` uses all properties of base class `NPulseNeuron` with parameters:
- `MembraneClassName = "NCSynNeuronMembrane"` — classic membrane optimized for synapses
- `LTZoneClassName = "NCLTZone"` — classic LT-zone
- `NumSomaMembraneParts = 1` — one soma part

### Methods

`NContinuesSynSPNeuron` uses all methods of base class `NPulseNeuron`.

### Usage in configurations

`NContinuesSynSPNeuron` is used in experiments with continuous models and optimized synapses:

- **Continuous models**: Experiments with continuous (non-spiking) neuron models
- **Optimized synapses**: Uses classic membrane and LT-zone optimized for synapses

**Features:**
- Continuous output: Uses classic model with continuous output instead of spiking
- Optimized synapses: Membrane and LT-zone are optimized for synaptic processing
- Single soma: Uses one soma membrane part

**Typical parameter values:**
- **MembraneClassName**: "NCSynNeuronMembrane" (classic membrane optimized for synapses)
- **LTZoneClassName**: "NCLTZone" (classic LT-zone)
- **NumSomaMembraneParts**: 1 (one soma part)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseNeuron`](NPulseNeuron.md) — spiking neuron
- [`NSynSPNeuron`](NSynSPNeuron.md) — synaptic SP-neuron
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — classic membrane optimized for synapses
- [Architecture.md](../Architecture.md) — library architecture
