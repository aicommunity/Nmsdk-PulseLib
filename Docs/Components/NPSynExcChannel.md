# NPSynExcChannel — возбуждающий синаптический импульсный канал

## RU

### Назначение

**Класс**: `NPSynExcChannel` — конфигурационный вариант синаптического импульсного канала с типом возбуждающего канала.  
**Аббревиатуры**: `Syn` — **Syn**apse (синапс); `Exc` — **Exc**itatory (возбуждающий).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPSynExcChannel", ...)`.  
**Storage-инстансы**: `ClassName = "NPSynExcChannel"` в `Bin/Configs/*/Model_*.xml`.

`NPSynExcChannel` является конфигурационным вариантом базового класса `NPulseSynChannel` с предустановленным типом возбуждающего канала. Создается из `NPulseSynChannel` с настройкой:
- `Type = -1` — возбуждающий канал

Возбуждающий канал увеличивает потенциал мембраны при наличии входных сигналов от синапсов.

**Использование:** Моделирование возбуждающих синаптических каналов, обработка возбуждающих синапсов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPSynExcChannel : configuration variant
    class NPulseSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Type : double
    }
    class NPSynExcChannel {
        +Type : double = -1
    }
```

**Иерархия наследования:**
- `NPulseChannelCommon` — общий импульсный канал
- `NPulseChannel` — базовый импульсный канал
- `NPulseSynChannel` — синаптический импульсный канал
- `NPSynExcChannel` — конфигурационный вариант для возбуждающего канала

### Свойства

`NPSynExcChannel` использует все свойства базового класса `NPulseSynChannel` с предустановленным значением:

**Параметры:**
- `Type = -1` — возбуждающий канал

**Остальные параметры по умолчанию:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 мс)
- `DissociationTC = 0.01` (10 мс)
- `SynapseResistance = 1.0e8` (100 МОм)
- `Capacity = 1.0e-9` (1 нФ)
- `Resistance = 1.0e7` (10 МОм)
- `FBResistance = 1.0e8` (100 МОм)

**Особенности:**
- При сбросе (`AReset()`) устанавливается `Output = -1` (для возбуждающего канала)
- В расчете используется логика для возбуждающих каналов (`Type < 0`)

### Методы

`NPSynExcChannel` использует все методы базового класса `NPulseSynChannel`.

### Примеры использования

#### Пример 1: Создание канала в коде C++

```cpp
// Создание возбуждающего синаптического канала
auto channel = storage->CreateComponent("NPSynExcChannel");
channel->SetName("ExcSynChannel");

// Инициализация (использует Type = -1)
channel->Default();

// Использование
channel->Build();
```

#### Пример 2: Конфигурация XML

```xml
<Channel1 Class="NPSynExcChannel">
    <Parameters>
        <Type>-1</Type>
        <Capacity>1.0e-9</Capacity>
        <Resistance>1.0e7</Resistance>
        <FBResistance>1.0e8</FBResistance>
        <PulseAmplitude>1.0</PulseAmplitude>
        <SecretionTC>0.001</SecretionTC>
        <DissociationTC>0.01</DissociationTC>
        <SynapseResistance>1.0e8</SynapseResistance>
    </Parameters>
</Channel1>
```

### Использование в конфигурациях

`NPSynExcChannel` используется в экспериментах с возбуждающими синаптическими каналами:

- Моделирование возбуждающих синаптических каналов
- Обработка возбуждающих синапсов
- Эксперименты с возбуждающей синаптической передачей

**Особенности:**
- Автоматически настроен как возбуждающий канал (`Type = -1`)
- Увеличивает потенциал мембраны при наличии входных сигналов
- Используется в составе возбуждающих мембран

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPulseSynChannel`](NPulseSynChannel.md) — синаптический импульсный канал (базовый класс)
- [`NPSynChannel`](NPSynChannel.md) — алиас для NPulseSynChannel
- [`NPSynInhChannel`](NPSynInhChannel.md) — тормозной синаптический канал
- [`NPulseChannel`](NPulseChannel.md) — базовый импульсный канал
- [`NPulseSynapse`](NPulseSynapse.md) — импульсный синапс
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPSynExcChannel` — configuration variant of synaptic spiking channel with excitatory channel type.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPSynExcChannel", ...)`.  
**Instances**: `ClassName = "NPSynExcChannel"` in `Bin/Configs/*/Model_*.xml`.

`NPSynExcChannel` is a configuration variant of the base class `NPulseSynChannel` with preset excitatory channel type. Created from `NPulseSynChannel` with setting:
- `Type = -1` — excitatory channel

Excitatory channel increases membrane potential when input signals from synapses are present.

**Usage:** Modeling excitatory synaptic channels, processing excitatory synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPSynExcChannel : configuration variant
    class NPulseSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Type : double
    }
    class NPSynExcChannel {
        +Type : double = -1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Channel as NPSynExcChannel
    participant Synapses as NPulseSynapse
    participant Membrane as NPulseMembrane
    
    Storage->>Channel: New() + Default()
    Storage->>Channel: Build()
    Channel->>Channel: Set Type = -1
    Channel->>Channel: ABuild()
    Channel->>Synapses: Connect synapses
    loop Each step
        Synapses->>Channel: PreOutput signals
        Storage->>Channel: Calculate()
        Channel->>Channel: Process synapses
        Channel->>Channel: Apply Type = -1 (excitatory)
        Channel->>Channel: Apply mediator dynamics
        Channel->>Channel: Calculate output current
        Channel->>Membrane: Output (increases potential)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetType: Set Type = -1
    SetType --> Building: Build()
    Building --> Connecting: Connect synapses
    Connecting --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessSynapses: Process synapse inputs
    ProcessSynapses --> ApplyExcitatory: Apply Type = -1 (excitatory)
    ApplyExcitatory --> ApplyMediator: Apply mediator dynamics
    ApplyMediator --> CalculateOutput: Calculate output current
    CalculateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> SetOutputNeg: Set Output = -1
    SetOutputNeg --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInputs[Receive PreOutput from synapses]
    ReceiveInputs --> AggregateInputs[Aggregate synapse inputs]
    AggregateInputs --> ApplyType["Apply Type = -1<br/>Excitatory coefficient"]
    ApplyType --> ProcessMediator["Process mediator dynamics<br/>SecretionTC, DissociationTC"]
    ProcessMediator --> CalculateCurrent["Calculate output current<br/>Increases membrane potential"]
    CalculateCurrent --> SetOutput[Set output]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPulseSynChannel["NPulseSynChannel Base"]
        BaseChannel[NPulseSynChannel]
    end
    
    subgraph NPSynExcChannel["NPSynExcChannel Configuration"]
        ExcitatoryType["Excitatory Type<br/>Type = -1"]
        SynapseProcessing["Synapse Processing<br/>Mediator dynamics"]
    end
    
    subgraph External["External Components"]
        Synapses["NPulseSynapse<br/>Synapses"]
        Membrane[NPulseMembrane]
    end
    
    BaseChannel -->|configured as| NPSynExcChannel
    NPSynExcChannel -->|uses| ExcitatoryType
    Synapses -->|PreOutput| NPSynExcChannel
    NPSynExcChannel -->|Output<br/>increases potential| Membrane
```

### Properties

`NPSynExcChannel` uses all properties of base class `NPulseSynChannel` with preset value:

**Parameters:**
- `Type = -1` — excitatory channel

**Other default parameters:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 ms)
- `DissociationTC = 0.01` (10 ms)
- `SynapseResistance = 1.0e8` (100 MΩ)
- `Capacity = 1.0e-9` (1 nF)
- `Resistance = 1.0e7` (10 MΩ)
- `FBResistance = 1.0e8` (100 MΩ)

**Features:**
- On reset (`AReset()`), `Output = -1` is set (for excitatory channel)
- Uses logic for excitatory channels (`Type < 0`)

### Methods

`NPSynExcChannel` uses all methods of base class `NPulseSynChannel`.

### Usage in configurations

`NPSynExcChannel` is used in experiments with excitatory synaptic channels:

- **Excitatory channels**: Modeling excitatory synaptic channels
- **Excitatory synapses**: Processing excitatory synapses
- **Excitatory transmission**: Experiments with excitatory synaptic transmission

**Features:**
- Automatically configured as excitatory channel (`Type = -1`)
- Increases membrane potential when input signals are present
- Used in excitatory membranes

**Typical parameter values:**
- **Type**: -1 (excitatory channel type)
- **SecretionTC**: 0.001 (1 ms time constant)
- **DissociationTC**: 0.01 (10 ms time constant)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPulseSynChannel`](NPulseSynChannel.md) — synaptic spiking channel (base class)
- [`NPSynChannel`](NPSynChannel.md) — alias for NPulseSynChannel
- [`NPSynInhChannel`](NPSynInhChannel.md) — inhibitory synaptic channel
- [`NPulseChannel`](NPulseChannel.md) — base spiking channel
- [`NPulseSynapse`](NPulseSynapse.md) — spiking synapse
- [Architecture.md](../Architecture.md) — library architecture

