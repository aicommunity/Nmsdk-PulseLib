# NPSynNeuronMembrane — мембрана для syn-нейронов (импульсная)

## RU

### Назначение

**Класс**: `NPSynNeuronMembrane` — конфигурационный вариант импульсной мембраны для syn-нейронов (импульсных нейронов с синаптическими каналами).  
**Аббревиатура**: `Syn` — **Syn**apse (синапс).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSynNeuronMembrane", ...)`.  
**Storage-инстансы**: `ClassName = "NPSynNeuronMembrane"` в `Bin/Configs/*/Model_*.xml`.

`NPSynNeuronMembrane` является конфигурационным вариантом класса `NPulseMembrane` с параметрами для syn-нейронов. При создании компонента с `ClassName = "NPSynNeuronMembrane"` создается экземпляр `NPulseMembrane` с параметрами:
- `ExcChannelClassName = "NPSynExcChannel"` — возбуждающий синаптический канал
- `InhChannelClassName = "NPSynInhChannel"` — тормозной синаптический канал

**Использование:** Мембрана для syn-нейронов, импульсные нейроны с синаптическими каналами

### UML-диаграмма классов

```mermaid
classDiagram
    UNet <|-- NPulseMembraneCommon
    NPulseMembraneCommon <|-- NPulseMembrane
    NPulseMembrane <|.. NPSynNeuronMembrane : configuration variant
    NPulseMembrane *-- NPSynExcChannel : ExcChannelClassName
    NPulseMembrane *-- NPSynInhChannel : InhChannelClassName
    class NPulseMembrane {
        +FeedbackGain : double
        +SynapseClassName : string
        +ExcChannelClassName : string
        +InhChannelClassName : string
    }
    class NPSynNeuronMembrane {
        +ExcChannelClassName : string = "NPSynExcChannel"
        +InhChannelClassName : string = "NPSynInhChannel"
    }
```

**Иерархия наследования:**
- `UNet` — базовая сеть Rdk Framework
- `NPulseMembraneCommon` — общая импульсная мембрана
- `NPulseMembrane` — базовая импульсная мембрана
- `NPSynNeuronMembrane` — конфигурационный вариант для syn-нейронов

**Параметры конфигурации:**
- `ExcChannelClassName = "NPSynExcChannel"` — возбуждающий синаптический канал
- `InhChannelClassName = "NPSynInhChannel"` — тормозной синаптический канал

### Свойства

`NPSynNeuronMembrane` использует все свойства базового класса `NPulseMembrane` с параметрами:
- `ExcChannelClassName = "NPSynExcChannel"`
- `InhChannelClassName = "NPSynInhChannel"`

### Методы

`NPSynNeuronMembrane` использует все методы базового класса `NPulseMembrane`.

### Примеры использования

#### Пример 1: Создание мембраны в коде C++

```cpp
// Создание мембраны для syn-нейрона
auto membrane = storage->CreateComponent("NPSynNeuronMembrane");
membrane->SetName("PMembrane");

// Инициализация (использует параметры по умолчанию)
membrane->Default();

// Использование
membrane->Build();
```

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseMembrane`](NPulseMembrane.md) — базовая импульсная мембрана (базовый класс)
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — мембрана для непрерывных syn-нейронов
- [`NPSynExcChannel`](NPSynExcChannel.md) — возбуждающий синаптический канал
- [`NPSynInhChannel`](NPSynInhChannel.md) — тормозной синаптический канал
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPSynNeuronMembrane` — configuration variant of spiking membrane for syn-neurons (spiking neurons with synaptic channels).  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPSynNeuronMembrane", ...)`.  
**Instances**: `ClassName = "NPSynNeuronMembrane"` in `Bin/Configs/*/Model_*.xml`.

`NPSynNeuronMembrane` is a configuration variant of `NPulseMembrane` class with parameters for syn-neurons. When creating a component with `ClassName = "NPSynNeuronMembrane"`, an instance of `NPulseMembrane` is created with parameters:
- `ExcChannelClassName = "NPSynExcChannel"` — excitatory synaptic channel
- `InhChannelClassName = "NPSynInhChannel"` — inhibitory synaptic channel

**Usage:** Membrane for syn-neurons, spiking neurons with synaptic channels

### UML Class Diagram

```mermaid
classDiagram
    UNet <|-- NPulseMembraneCommon
    NPulseMembraneCommon <|-- NPulseMembrane
    NPulseMembrane <|.. NPSynNeuronMembrane : configuration variant
    NPulseMembrane *-- NPSynExcChannel : ExcChannelClassName
    NPulseMembrane *-- NPSynInhChannel : InhChannelClassName
    class NPulseMembrane {
        +FeedbackGain : double
        +SynapseClassName : string
        +ExcChannelClassName : string
        +InhChannelClassName : string
    }
    class NPSynNeuronMembrane {
        +ExcChannelClassName : string = "NPSynExcChannel"
        +InhChannelClassName : string = "NPSynInhChannel"
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Membrane as NPSynNeuronMembrane
    participant ExcChannel as NPSynExcChannel
    participant InhChannel as NPSynInhChannel
    participant Synapses as Synapses
    
    Storage->>Membrane: New() + Default()
    Storage->>Membrane: Build()
    Membrane->>Membrane: Set ExcChannelClassName = "NPSynExcChannel"
    Membrane->>Membrane: Set InhChannelClassName = "NPSynInhChannel"
    Membrane->>Membrane: ABuild()
    Membrane->>ExcChannel: Create excitatory channel
    Membrane->>InhChannel: Create inhibitory channel
    loop Each step
        Synapses->>ExcChannel: Input signals
        Synapses->>InhChannel: Input signals
        Storage->>Membrane: Calculate()
        Membrane->>ExcChannel: Calculate()
        Membrane->>InhChannel: Calculate()
        ExcChannel->>Membrane: Excitatory current
        InhChannel->>Membrane: Inhibitory current
        Membrane->>Membrane: Calculate membrane potential
        Membrane-->>Storage: Output potential
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetChannels: Set channel class names
    SetChannels --> Building: Build()
    Building --> CreateExcChannel: Create NPSynExcChannel
    CreateExcChannel --> CreateInhChannel: Create NPSynInhChannel
    CreateInhChannel --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessChannels: Process channels
    ProcessChannels --> CalculatePotential: Calculate membrane potential
    CalculatePotential --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ProcessExcChannel["Process excitatory channel<br/>NPSynExcChannel"]
    ProcessExcChannel --> ProcessInhChannel["Process inhibitory channel<br/>NPSynInhChannel"]
    ProcessInhChannel --> AggregateCurrents[Aggregate channel currents]
    AggregateCurrents --> CalculatePotential["Calculate membrane potential<br/>from channel currents"]
    CalculatePotential --> ApplyFeedback[Apply feedback gain if needed]
    ApplyFeedback --> SetOutput[Set output potential]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseMembrane["NPulseMembrane Base"]
        BaseMembrane[NPulseMembrane]
    end
    
    subgraph NPSynNeuronMembrane["NPSynNeuronMembrane Configuration"]
        ExcChannelConfig["ExcChannelClassName<br/>= #quot;NPSynExcChannel#quot;"]
        InhChannelConfig["InhChannelClassName<br/>= #quot;NPSynInhChannel#quot;"]
    end
    
    subgraph Channels["Channels"]
        ExcChannel["NPSynExcChannel<br/>Excitatory Channel"]
        InhChannel["NPSynInhChannel<br/>Inhibitory Channel"]
    end
    
    subgraph External["External Components"]
        Synapses[Synapses]
        Neuron[Neuron]
    end
    
    BaseMembrane -->|configured as| NPSynNeuronMembrane
    NPSynNeuronMembrane -->|creates| ExcChannel
    NPSynNeuronMembrane -->|creates| InhChannel
    Synapses -->|Input| ExcChannel
    Synapses -->|Input| InhChannel
    ExcChannel -->|Excitatory current| NPSynNeuronMembrane
    InhChannel -->|Inhibitory current| NPSynNeuronMembrane
    NPSynNeuronMembrane -->|Output potential| Neuron
```

### Properties

`NPSynNeuronMembrane` uses all properties of base class `NPulseMembrane` with parameters:
- `ExcChannelClassName = "NPSynExcChannel"` — excitatory synaptic channel
- `InhChannelClassName = "NPSynInhChannel"` — inhibitory synaptic channel

**Configuration parameters:**
- `ExcChannelClassName = "NPSynExcChannel"` — preset excitatory channel class name
- `InhChannelClassName = "NPSynInhChannel"` — preset inhibitory channel class name

### Methods

`NPSynNeuronMembrane` uses all methods of base class `NPulseMembrane`.

### Usage in configurations

`NPSynNeuronMembrane` is used in experiments with syn-neurons:

- **Syn-neurons**: Spiking neurons with synaptic channels
- **Spiking models**: Used in spiking neural network models

**Features:**
- Automatically configured with synaptic channels (`NPSynExcChannel` and `NPSynInhChannel`)
- Simplified configuration: Channel class names are preset
- Spiking model: Optimized for spiking neuron models

**Typical parameter values:**
- **ExcChannelClassName**: "NPSynExcChannel" (excitatory synaptic channel)
- **InhChannelClassName**: "NPSynInhChannel" (inhibitory synaptic channel)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseMembrane`](NPulseMembrane.md) — base spiking membrane (base class)
- [`NCSynNeuronMembrane`](NCSynNeuronMembrane.md) — membrane for continuous syn-neurons
- [`NPSynExcChannel`](NPSynExcChannel.md) — excitatory synaptic channel
- [`NPSynInhChannel`](NPSynInhChannel.md) — inhibitory synaptic channel
- [Architecture.md](../Architecture.md) — library architecture
