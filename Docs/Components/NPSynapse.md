# NPSynapse — импульсный синапс (алиас)

## RU

### Назначение

**Класс**: `NPSynapse` — алиас для класса `NPulseSynapse`.  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSynapse", ...)`.  
**Storage-инстансы**: `ClassName = "NPSynapse"` в `Bin/Configs/*/Model_*.xml`.

`NPSynapse` является алиасом (синонимом) для класса `NPulseSynapse`. При создании компонента с `ClassName = "NPSynapse"` фактически создается экземпляр класса `NPulseSynapse` с параметрами по умолчанию.

**Использование:** Упрощенное именование при конфигурации, обратная совместимость

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NPulseSynapse
    NPulseSynapse <|.. NPSynapse : alias
    class NPulseSynapse {
        +SecretionTC : double
        +DissociationTC : double
        +InhibitionCoeff : double
        +UsePulseSignal : bool
        +UsePresynapticInhibition : bool
    }
```

**Иерархия наследования:**
- `NPulseSynapseCommon` — общий импульсный синапс
- `NPulseSynapse` — импульсный синапс с моделью медиатора
- `NPSynapse` — алиас для `NPulseSynapse`

### Свойства

`NPSynapse` использует все свойства класса `NPulseSynapse` с параметрами по умолчанию.

**Параметры по умолчанию:**
- `SecretionTC = 0.001` (1 мс)
- `DissociationTC = 0.01` (10 мс)
- `Resistance = 1.0e9`
- `PulseAmplitude = 1.0`
- `UsePulseSignal = true`
- `UsePresynapticInhibition = false`

### Методы

`NPSynapse` использует все методы класса `NPulseSynapse`.

### Примеры использования

#### Пример 1: Создание синапса в коде C++

```cpp
// Создание синапса через алиас
auto synapse = storage->CreateComponent("NPSynapse");
synapse->SetName("PSynapse");

// Инициализация (использует параметры по умолчанию)
synapse->Default();

// Использование
synapse->Build();
for (int step = 0; step < 1000; step++) {
    synapse->Calculate();
}
```

#### Пример 2: Конфигурация XML

```xml
<Synapse1 Class="NPSynapse">
    <Parameters>
        <Type>1</Type>
        <PulseAmplitude>1.0</PulseAmplitude>
        <Resistance>1.0e9</Resistance>
        <Weight>1.0</Weight>
    </Parameters>
</Synapse1>
```

### Использование в конфигурациях

`NPSynapse` используется как упрощенное имя для `NPulseSynapse`:

- Упрощенное именование в конфигурациях
- Обратная совместимость со старыми конфигурациями
- Стандартные параметры по умолчанию

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseSynapse`](NPulseSynapse.md) — импульсный синапс с моделью медиатора
- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — общий импульсный синапс
- [`NPSynapseBio`](NPSynapseBio.md) — биоинспирированный вариант
- [`NPSynapseBio2`](NPSynapseBio2.md) — второй биоинспирированный вариант
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPSynapse` — alias for `NPulseSynapse` class.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPSynapse", ...)`.  
**Instances**: `ClassName = "NPSynapse"` in `Bin/Configs/*/Model_*.xml`.

`NPSynapse` is an alias (synonym) for the `NPulseSynapse` class. When creating a component with `ClassName = "NPSynapse"`, an instance of `NPulseSynapse` with default parameters is actually created.

**Usage:** Simplified naming in configurations, backward compatibility

### UML Class Diagram

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NPulseSynapse
    NPulseSynapse <|.. NPSynapse : alias
    class NPulseSynapse {
        +SecretionTC : double
        +DissociationTC : double
        +InhibitionCoeff : double
        +Resistance : double
        +UsePulseSignal : bool
        +UsePresynapticInhibition : bool
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Synapse as NPSynapse
    participant PreNeuron as PreNeuron
    participant Channel as NPulseSynChannel
    
    Storage->>Synapse: New() + Default()
    Storage->>Synapse: Build()
    Synapse->>Synapse: ABuild()
    loop Each step
        PreNeuron->>Synapse: Input pulse/signal
        Storage->>Synapse: Calculate()
        Synapse->>Synapse: Process input
        Synapse->>Synapse: Update mediator model
        Synapse->>Synapse: Calculate output
        Synapse-->>Channel: Output current
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
    Calculating --> ReceiveInput: Receive input from pre-neuron
    ReceiveInput --> UpdateMediator: Update mediator model
    UpdateMediator --> CalculateOutput: Calculate output current
    CalculateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInput[Receive input from pre-neuron]
    ReceiveInput --> CheckPulseSignal{UsePulseSignal?}
    CheckPulseSignal -->|Yes| ProcessPulse[Process pulse signal]
    CheckPulseSignal -->|No| ProcessSignal[Process continuous signal]
    ProcessPulse --> UpdateSecretion["Update mediator secretion<br/>SecretionTC"]
    ProcessSignal --> UpdateSecretion
    UpdateSecretion --> UpdateDissociation["Update mediator dissociation<br/>DissociationTC"]
    UpdateDissociation --> CalculateOutput["Calculate output<br/>Output = PreOutput / Resistance"]
    CalculateOutput --> ApplyWeight[Apply weight if needed]
    ApplyWeight --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseSynapse["NPulseSynapse Base"]
        BaseSynapse[NPulseSynapse]
    end
    
    subgraph NPSynapse["NPSynapse Alias"]
        MediatorModel["Mediator Model<br/>SecretionTC, DissociationTC"]
    end
    
    subgraph External["External Components"]
        PreNeuron[PreNeuron]
        Channel[NPulseSynChannel]
    end
    
    BaseSynapse -->|configured as| NPSynapse
    PreNeuron -->|Input| NPSynapse
    NPSynapse -->|Output current| Channel
    NPSynapse -->|uses| MediatorModel
```

### Properties

`NPSynapse` uses all properties of `NPulseSynapse` class with default parameters:

**Default parameters:**
- `SecretionTC = 0.001` (1 ms)
- `DissociationTC = 0.01` (10 ms)
- `Resistance = 1.0e9` (1 GΩ)
- `PulseAmplitude = 1.0`
- `UsePulseSignal = true`
- `UsePresynapticInhibition = false`

**Features:**
- Mediator dynamics: Uses secretion and dissociation time constants
- Pulse signal support: Can process pulse signals or continuous signals
- Presynaptic inhibition: Optional presynaptic inhibition support

### Methods

`NPSynapse` uses all methods of `NPulseSynapse` class.

### Usage in configurations

`NPSynapse` is used as a simplified name for `NPulseSynapse`:

- **Simplified naming**: Used in configurations for easier naming
- **Backward compatibility**: Maintains compatibility with older configurations
- **Standard defaults**: Uses standard default parameters

**Typical parameter values:**
- **SecretionTC**: 0.001 (1 ms time constant for mediator secretion)
- **DissociationTC**: 0.01 (10 ms time constant for mediator dissociation)
- **Resistance**: 1.0e9 (1 GΩ resistance for synapse output calculation)

**Features:**
- Mediator model: Uses mediator dynamics for synapse processing
- Pulse signal: Supports pulse signal processing
- Standard parameters: Uses standard default parameters

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseSynapse`](NPulseSynapse.md) — spiking synapse with neurotransmitter model
- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — common spiking synapse
- [`NPSynapseBio`](NPSynapseBio.md) — bio-inspired variant
- [`NPSynapseBio2`](NPSynapseBio2.md) — second bio-inspired variant
- [Architecture.md](../Architecture.md) — library architecture

