# NPSynapseBio2 — второй биоинспирированный импульсный синапс

## RU

### Назначение

**Класс**: `NPSynapseBio2` — конфигурационный вариант импульсного синапса с альтернативными биоинспирированными параметрами.  
**Аббревиатура**: `Bio` — **Bio**logical (биологическая модель).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSynapseBio2", ...)`.  
**Storage-инстансы**: `ClassName = "NPSynapseBio2"` в `Bin/Configs/*/Model_*.xml`.

`NPSynapseBio2` является конфигурационным вариантом базового класса `NPulseSynapse` с предустановленными альтернативными биоинспирированными параметрами. Создается из `NPulseSynapse` с настройками:
- `Resistance = 86000000` (86 МОм) — сопротивление синапса
- `DissociationTC = 0.005` (5 мс) — постоянная времени распада медиатора

Отличие от `NPSynapseBio` заключается в способе задания сопротивления (прямое значение вместо вычисления).

**Использование:** Моделирование биологически реалистичных синапсов с альтернативными параметрами

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NPulseSynapse
    NPulseSynapse <|.. NPSynapseBio2 : configuration variant
    class NPulseSynapse {
        +SecretionTC : double
        +DissociationTC : double
        +Resistance : double
    }
    class NPSynapseBio2 {
        +Resistance : double = 8.6e7
        +DissociationTC : double = 0.005
    }
```

**Иерархия наследования:**
- `NPulseSynapseCommon` — общий импульсный синапс
- `NPulseSynapse` — импульсный синапс с моделью медиатора
- `NPSynapseBio2` — конфигурационный вариант с альтернативными биоинспирированными параметрами

### Свойства

`NPSynapseBio2` использует все свойства базового класса `NPulseSynapse` с предустановленными биоинспирированными значениями:

**Параметры:**
- `Resistance = 86000000` (86 МОм) — сопротивление синапса
- `DissociationTC = 0.005` (5 мс) — постоянная времени распада медиатора

**Остальные параметры по умолчанию:**
- `SecretionTC = 0.001` (1 мс)
- `PulseAmplitude = 1.0`
- `UsePulseSignal = true`
- `UsePresynapticInhibition = false`
- `InhibitionCoeff = 0.0`

### Методы

`NPSynapseBio2` использует все методы базового класса `NPulseSynapse`.

### Примеры использования

#### Пример 1: Создание синапса в коде C++

```cpp
// Создание второго биоинспирированного синапса
auto synapse = storage->CreateComponent("NPSynapseBio2");
synapse->SetName("BioSynapse2");

// Инициализация (использует биоинспирированные параметры)
synapse->Default();

// Использование
synapse->Build();
for (int step = 0; step < 1000; step++) {
    synapse->Calculate();
}
```

#### Пример 2: Конфигурация XML

```xml
<Synapse1 Class="NPSynapseBio2">
    <Parameters>
        <Type>1</Type>
        <PulseAmplitude>1.0</PulseAmplitude>
        <Resistance>86000000</Resistance>
        <Weight>1.0</Weight>
        <SecretionTC>0.001</SecretionTC>
        <DissociationTC>0.005</DissociationTC>
    </Parameters>
</Synapse1>
```

### Использование в конфигурациях

`NPSynapseBio2` используется в экспериментах с альтернативными биологически реалистичными параметрами:

- Моделирование синаптической передачи с альтернативными параметрами
- Сравнение различных биоинспирированных вариантов
- Изучение влияния параметров на синаптическую передачу

**Особенности:**
- Использует биологически реалистичные значения сопротивления и постоянной времени распада
- Альтернативный вариант `NPSynapseBio` с теми же параметрами, но другим способом задания

## Источники

См. [Literature-References.md](../Literature-References.md): **26**, **29**, **30**.

### См. также

- [`NPulseSynapse`](NPulseSynapse.md) — импульсный синапс с моделью медиатора
- [`NPSynapse`](NPSynapse.md) — базовый импульсный синапс
- [`NPSynapseBio`](NPSynapseBio.md) — первый биоинспирированный вариант
- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — общий импульсный синапс
- [Architecture.md](../Architecture.md) — архитектура библиотеки
- [Scientific-Background.md](../Scientific-Background.md) — научный фон (синаптическая передача)

---

## EN

### Purpose

**Class**: `NPSynapseBio2` — configuration variant of spiking synapse with alternative bio-inspired parameters.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPSynapseBio2", ...)`.  
**Instances**: `ClassName = "NPSynapseBio2"` in `Bin/Configs/*/Model_*.xml`.

`NPSynapseBio2` is a configuration variant of the base class `NPulseSynapse` with preset alternative bio-inspired parameters. Created from `NPulseSynapse` with settings:
- `Resistance = 86000000` (86 MΩ) — synapse resistance
- `DissociationTC = 0.005` (5 ms) — neurotransmitter dissociation time constant

Difference from `NPSynapseBio` is the way resistance is set (direct value instead of calculation).

**Usage:** Modeling biologically realistic synapses with alternative parameters

### UML Class Diagram

```mermaid
classDiagram
    NPulseSynapseCommon <|-- NPulseSynapse
    NPulseSynapse <|.. NPSynapseBio2 : configuration variant
    class NPulseSynapse {
        +SecretionTC : double
        +DissociationTC : double
        +Resistance : double
        +InhibitionCoeff : double
    }
    class NPSynapseBio2 {
        +Resistance : double = 8.6e7
        +DissociationTC : double = 0.005
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Synapse as NPSynapseBio2
    participant PreNeuron as PreNeuron
    participant Channel as NPulseSynChannel
    
    Storage->>Synapse: New() + Default()
    Storage->>Synapse: Build()
    Synapse->>Synapse: Set Resistance = 8.6e7
    Synapse->>Synapse: Set DissociationTC = 0.005
    Synapse->>Synapse: ABuild()
    loop Each step
        PreNeuron->>Synapse: Input pulse/signal
        Storage->>Synapse: Calculate()
        Synapse->>Synapse: Process input
        Synapse->>Synapse: Update mediator model
        Note over Synapse: Resistance = 8.6e7<br/>DissociationTC = 0.005
        Synapse->>Synapse: Calculate output
        Synapse-->>Channel: Output current
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetParams: Set bio parameters
    SetParams --> Building: Build()
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
    ProcessPulse --> UpdateSecretion[Update mediator secretion<br/>SecretionTC]
    ProcessSignal --> UpdateSecretion
    UpdateSecretion --> UpdateDissociation[Update mediator dissociation<br/>DissociationTC = 0.005]
    UpdateDissociation --> CalculateOutput[Calculate output<br/>Output = PreOutput / Resistance<br/>Resistance = 8.6e7]
    CalculateOutput --> ApplyWeight[Apply weight if needed]
    ApplyWeight --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseSynapse["NPulseSynapse Base"]
        BaseSynapse[NPulseSynapse]
    end
    
    subgraph NPSynapseBio2["NPSynapseBio2 Configuration"]
        BioParams[Bio Parameters<br/>Resistance = 8.6e7<br/>DissociationTC = 0.005]
        MediatorModel[Mediator Model<br/>SecretionTC, DissociationTC]
    end
    
    subgraph External["External Components"]
        PreNeuron[PreNeuron]
        Channel[NPulseSynChannel]
    end
    
    BaseSynapse -->|configured as| NPSynapseBio2
    PreNeuron -->|Input| NPSynapseBio2
    NPSynapseBio2 -->|Output current| Channel
    NPSynapseBio2 -->|uses| BioParams
    NPSynapseBio2 -->|uses| MediatorModel
```

### Properties

`NPSynapseBio2` uses all properties of base class `NPulseSynapse` with preset bio-inspired values:

**Configuration parameters:**
- `Resistance = 86000000` (86 MΩ) — synapse resistance
- `DissociationTC = 0.005` (5 ms) — neurotransmitter dissociation time constant

**Other default parameters:**
- `SecretionTC = 0.001` (1 ms)
- `PulseAmplitude = 1.0`
- `UsePulseSignal = true`
- `UsePresynapticInhibition = false`
- `InhibitionCoeff = 0.0`

**Features:**
- Bio-inspired parameters: Uses biologically realistic values
- Alternative variant: Same parameters as `NPSynapseBio`, but with direct value assignment

### Methods

`NPSynapseBio2` uses all methods of base class `NPulseSynapse`.

### Usage in configurations

`NPSynapseBio2` is used in experiments with alternative bio-inspired parameters:

- **Bio-inspired synapses**: `Bin/Configs/*/Model_*.xml` (where alternative bio parameters are required)
- **Parameter comparison**: Comparing different bio-inspired variants

**Features:**
- Bio-inspired parameters: Uses biologically realistic resistance and dissociation time constant
- Alternative variant: Alternative to `NPSynapseBio` with same parameters but different assignment method
- Mediator dynamics: Uses mediator model for synapse processing

**Typical parameter values:**
- **Resistance**: 86000000 (86 MΩ for bio-inspired models)
- **DissociationTC**: 0.005 (5 ms time constant)
- **SecretionTC**: 0.001 (1 ms time constant)

### References

See [Literature-References.md](../Literature-References.md): **26**, **29**, **30**.

### See Also

- [`NPulseSynapse`](NPulseSynapse.md) — spiking synapse with neurotransmitter model
- [`NPSynapse`](NPSynapse.md) — base spiking synapse
- [`NPSynapseBio`](NPSynapseBio.md) — first bio-inspired variant
- [`NPulseSynapseCommon`](NPulseSynapseCommon.md) — common spiking synapse
- [Architecture.md](../Architecture.md) — library architecture

