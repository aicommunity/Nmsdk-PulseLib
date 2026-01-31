# NCSynExcChannel — возбуждающий непрерывный синаптический канал

## RU

### Назначение

**Класс**: `NCSynExcChannel` — конфигурационный вариант непрерывного синаптического канала с типом возбуждающего канала.  
**Префикс**: `NC` — **C**ontinuous (непрерывный, классический), компонент с непрерывными входами/выходами; `Syn` — **Syn**apse (синапс); `Exc` — **Exc**itatory (возбуждающий).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NCSynExcChannel", ...)`.  
**Storage-инстансы**: `ClassName = "NCSynExcChannel"` в `Bin/Configs/*/Model_*.xml`.

`NCSynExcChannel` является конфигурационным вариантом базового класса `NContinuesSynChannel` с предустановленным типом возбуждающего канала. Создается из `NContinuesSynChannel` с настройкой:
- `Type = -1` — возбуждающий канал

Возбуждающий канал увеличивает потенциал мембраны при наличии входных сигналов от синапсов.

**Использование:** Моделирование возбуждающих непрерывных синаптических каналов, обработка возбуждающих синапсов

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NContinuesSynChannel
    NContinuesSynChannel <|.. NCSynExcChannel : configuration variant
    class NContinuesSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Type : double
    }
    class NCSynExcChannel {
        +Type : double = -1
    }
```

**Иерархия наследования:**
- `NPulseChannelCommon` — общий импульсный канал
- `NPulseChannel` — базовый импульсный канал
- `NContinuesSynChannel` — непрерывный синаптический канал
- `NCSynExcChannel` — конфигурационный вариант для возбуждающего канала

### Свойства

`NCSynExcChannel` использует все свойства базового класса `NContinuesSynChannel` с предустановленным значением:

**Параметры:**
- `Type = -1` — возбуждающий канал

**Остальные параметры по умолчанию:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 мс)
- `DissociationTC = 0.01` (10 мс)
- `InhibitionCoeff = 1.0`
- `SynapseResistance = 1.0e8` (100 МОм)
- `Capacity = 1.0e-9` (1 нФ)
- `Resistance = 1.0e7` (10 МОм)
- `FBResistance = 1.0e8` (100 МОм)

### Методы

`NCSynExcChannel` использует все методы базового класса `NContinuesSynChannel`.

### Примеры использования

#### Пример 1: Создание канала в коде C++

```cpp
// Создание возбуждающего непрерывного синаптического канала
auto channel = storage->CreateComponent("NCSynExcChannel");
channel->SetName("CSynExcChannel");

// Инициализация (использует Type = -1)
channel->Default();

// Использование
channel->Build();
```

#### Пример 2: Конфигурация XML

```xml
<Channel1 Class="NCSynExcChannel">
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

`NCSynExcChannel` используется в экспериментах с возбуждающими непрерывными синаптическими каналами:

- Моделирование возбуждающих непрерывных синаптических каналов
- Обработка возбуждающих синапсов
- Эксперименты с возбуждающей синаптической передачей

**Особенности:**
- Автоматически настроен как возбуждающий канал (`Type = -1`)
- Увеличивает потенциал мембраны при наличии входных сигналов
- Использует упрощенную модель медиатора

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NContinuesSynChannel`](NContinuesSynChannel.md) — непрерывный синаптический канал (базовый класс)
- [`NCSynChannel`](NCSynChannel.md) — алиас для NContinuesSynChannel
- [`NCSynInhChannel`](NCSynInhChannel.md) — тормозной непрерывный синаптический канал
- [`NPulseSynChannel`](NPulseSynChannel.md) — импульсный синаптический канал
- [`NPulseChannel`](NPulseChannel.md) — базовый импульсный канал
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NCSynExcChannel` — configuration variant of continuous synaptic channel with excitatory channel type.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NCSynExcChannel", ...)`.  
**Instances**: `ClassName = "NCSynExcChannel"` in `Bin/Configs/*/Model_*.xml`.

`NCSynExcChannel` is a configuration variant of the base class `NContinuesSynChannel` with preset excitatory channel type. Created from `NContinuesSynChannel` with setting:
- `Type = -1` — excitatory channel

Excitatory channel increases membrane potential when input signals from synapses are present.

**Usage:** Modeling excitatory continuous synaptic channels, processing excitatory synapses

### UML Class Diagram

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NContinuesSynChannel
    NContinuesSynChannel <|.. NCSynExcChannel : configuration variant
    class NContinuesSynChannel {
        +PulseAmplitude : double
        +SecretionTC : double
        +DissociationTC : double
        +Type : double
    }
    class NCSynExcChannel {
        +Type : double = -1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Channel as NCSynExcChannel
    participant Synapses as NPulseSynapseCommon
    participant Membrane as Membrane
    
    Storage->>Channel: New() + Default()
    Storage->>Channel: Build()
    Channel->>Channel: Set Type = -1
    Channel->>Channel: ABuild()
    Channel->>Synapses: Connect synapses
    loop Each step
        Synapses->>Channel: PreOutput signals
        Storage->>Channel: Calculate()
        Channel->>Channel: Process synapses
        Channel->>Channel: Calculate syn_output = PreOutput / SynapseResistance
        Channel->>Channel: Apply Type = -1 (excitatory)
        Channel->>Channel: Apply mediator dynamics
        Channel->>Membrane: Output current (increases potential)
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
    ProcessSynapses --> CalculateOutput: Calculate syn_output
    CalculateOutput --> ApplyExcitatory: Apply Type = -1 (excitatory)
    ApplyExcitatory --> ApplyDynamics: Apply mediator dynamics
    ApplyDynamics --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInputs[Receive PreOutput from synapses]
    ReceiveInputs --> AggregateInputs[Aggregate synapse inputs]
    AggregateInputs --> CalculateOutput[Calculate syn_output<br/>syn_output = PreOutput / SynapseResistance]
    CalculateOutput --> ApplyType[Apply Type = -1<br/>Excitatory coefficient]
    ApplyType --> ApplyDynamics[Apply mediator dynamics<br/>SecretionTC, DissociationTC]
    ApplyDynamics --> SetOutput[Set output current<br/>Increases membrane potential]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NContinuesSynChannel["NContinuesSynChannel Base"]
        BaseChannel[NContinuesSynChannel]
    end
    
    subgraph NCSynExcChannel["NCSynExcChannel Configuration"]
        ExcitatoryType[Excitatory Type<br/>Type = -1]
        SynapseProcessing[Synapse Processing<br/>Simplified model]
    end
    
    subgraph External["External Components"]
        Synapses[NPulseSynapseCommon<br/>Synapses]
        Membrane[Membrane]
    end
    
    BaseChannel -->|configured as| NCSynExcChannel
    NCSynExcChannel -->|uses| ExcitatoryType
    Synapses -->|PreOutput| NCSynExcChannel
    NCSynExcChannel -->|Output current<br/>increases potential| Membrane
```

### Properties

`NCSynExcChannel` uses all properties of base class `NContinuesSynChannel` with preset value:

**Parameters:**
- `Type = -1` — excitatory channel

**Other default parameters:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 ms)
- `DissociationTC = 0.01` (10 ms)
- `InhibitionCoeff = 1.0`
- `SynapseResistance = 1.0e8` (100 MΩ)
- `Capacity = 1.0e-9` (1 nF)
- `Resistance = 1.0e7` (10 MΩ)
- `FBResistance = 1.0e8` (100 MΩ)

### Methods

`NCSynExcChannel` uses all methods of base class `NContinuesSynChannel`.

### Usage in configurations

`NCSynExcChannel` is used in experiments with excitatory continuous synaptic channels:

- **Excitatory channels**: Modeling excitatory continuous synaptic channels
- **Excitatory synapses**: Processing excitatory synapses
- **Excitatory transmission**: Experiments with excitatory synaptic transmission

**Features:**
- Automatically configured as excitatory channel (`Type = -1`)
- Increases membrane potential when input signals are present
- Uses simplified mediator model

**Typical parameter values:**
- **Type**: -1 (excitatory channel type)
- **SecretionTC**: 0.001 (1 ms time constant)
- **DissociationTC**: 0.01 (10 ms time constant)

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NContinuesSynChannel`](NContinuesSynChannel.md) — continuous synaptic channel (base class)
- [`NCSynChannel`](NCSynChannel.md) — alias for NContinuesSynChannel
- [`NCSynInhChannel`](NCSynInhChannel.md) — inhibitory continuous synaptic channel
- [`NPulseSynChannel`](NPulseSynChannel.md) — spiking synaptic channel
- [`NPulseChannel`](NPulseChannel.md) — base spiking channel
- [Architecture.md](../Architecture.md) — library architecture

