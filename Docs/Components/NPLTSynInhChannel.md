# NPLTSynInhChannel — тормозной LT-синаптический канал

## RU

### Назначение

**Класс**: `NPLTSynInhChannel` — конфигурационный вариант LT-синаптического канала с типом тормозного канала.  
**Аббревиатуры**: `LT` — **L**ow **T**hreshold (низкопороговая зона); `Syn` — **Syn**apse (синапс); `Inh` — **Inh**ibitory (тормозной).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynInhChannel", ...)`.  
**Storage-инстансы**: `ClassName = "NPLTSynInhChannel"` в `Bin/Configs/*/Model_*.xml`.

`NPLTSynInhChannel` является конфигурационным вариантом базового класса `NPLTSynChannel` с предустановленным типом тормозного канала. Создается из `NPLTSynChannel` с настройкой:
- `Type = 1` — тормозной канал

Тормозной LT-синаптический канал уменьшает потенциал LT-зоны при наличии входных сигналов от синапсов, препятствуя генерации спайков.

**Использование:** Моделирование тормозных синаптических каналов в LT-зонах, эксперименты с тормозной LT-пластичностью

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPLTSynChannel : configuration variant
    NPLTSynChannel <|.. NPLTSynInhChannel : configuration variant
    class NPLTSynChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
    class NPLTSynInhChannel {
        +Type : double = 1
    }
```

**Иерархия наследования:**
- `NPulseChannelCommon` — общий импульсный канал
- `NPulseChannel` — базовый импульсный канал
- `NPulseSynChannel` — синаптический импульсный канал
- `NPLTSynChannel` — конфигурационный вариант для LT-зоны
- `NPLTSynInhChannel` — конфигурационный вариант для тормозного LT-синаптического канала

### Свойства

`NPLTSynInhChannel` использует все свойства базового класса `NPLTSynChannel` с предустановленным значением:

**Параметры:**
- `Type = 1` — тормозной канал
- `Capacity = 1e-8` (10 нФ) — увеличенная емкость для LT-зоны
- `RestingResistance = 1e6` (1 МОм) — сопротивление покоя для LT-зоны

**Остальные параметры по умолчанию:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 мс)
- `DissociationTC = 0.01` (10 мс)
- `SynapseResistance = 1.0e8` (100 МОм)
- `Resistance = 1.0e7` (10 МОм)
- `FBResistance = 1.0e8` (100 МОм)

### Методы

`NPLTSynInhChannel` использует все методы базового класса `NPLTSynChannel`.

### Примеры использования

#### Пример 1: Создание канала в коде C++

```cpp
// Создание тормозного LT-синаптического канала
auto channel = storage->CreateComponent("NPLTSynInhChannel");
channel->SetName("LTSynInhChannel");

// Инициализация (использует Type = 1, параметры для LT-зоны)
channel->Default();

// Использование
channel->Build();
```

#### Пример 2: Конфигурация XML

```xml
<Channel1 Class="NPLTSynInhChannel">
    <Parameters>
        <Type>1</Type>
        <Capacity>1e-8</Capacity>
        <Resistance>1.0e7</Resistance>
        <FBResistance>1.0e8</FBResistance>
        <RestingResistance>1e6</RestingResistance>
        <PulseAmplitude>1.0</PulseAmplitude>
        <SecretionTC>0.001</SecretionTC>
        <DissociationTC>0.01</DissociationTC>
        <SynapseResistance>1.0e8</SynapseResistance>
    </Parameters>
</Channel1>
```

### Использование в конфигурациях

`NPLTSynInhChannel` используется в экспериментах с тормозными синаптическими каналами в LT-зонах:

- Моделирование тормозных синаптических каналов в LT-зонах
- Эксперименты с тормозной LT-пластичностью
- Изучение подавления генерации спайков в LT-зонах

**Особенности:**
- Автоматически настроен как тормозной канал (`Type = 1`)
- Уменьшает потенциал LT-зоны при наличии входных сигналов от синапсов
- Комбинирует обработку синапсов с параметрами LT-зоны

## Источники

См. [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### См. также

- [`NPLTSynChannel`](NPLTSynChannel.md) — LT-синаптический канал (базовый класс)
- [`NPLTSynExcChannel`](NPLTSynExcChannel.md) — возбуждающий LT-синаптический канал
- [`NPulseSynChannel`](NPulseSynChannel.md) — синаптический импульсный канал
- [`NPLTChannel`](NPLTChannel.md) — LT-канал
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — общая импульсная LT-зона
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPLTSynInhChannel` — configuration variant of LT-synaptic channel with inhibitory channel type.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPLTSynInhChannel", ...)`.  
**Instances**: `ClassName = "NPLTSynInhChannel"` in `Bin/Configs/*/Model_*.xml`.

`NPLTSynInhChannel` is a configuration variant of the base class `NPLTSynChannel` with preset inhibitory channel type. Created from `NPLTSynChannel` with setting:
- `Type = 1` — inhibitory channel

Inhibitory LT-synaptic channel decreases LT-zone potential when input signals from synapses are present, preventing spike generation.

**Usage:** Modeling inhibitory synaptic channels in LT-zones, experiments with inhibitory LT plasticity

### UML Class Diagram

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|-- NPulseSynChannel
    NPulseSynChannel <|.. NPLTSynChannel : configuration variant
    NPLTSynChannel <|.. NPLTSynInhChannel : configuration variant
    class NPLTSynChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
    class NPLTSynInhChannel {
        +Type : double = 1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Channel as NPLTSynInhChannel
    participant Synapses as NPulseSynapse
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Channel: New() + Default()
    Storage->>Channel: Build()
    Channel->>Channel: Set Type = 1
    Channel->>Channel: Set Capacity = 1e-8
    Channel->>Channel: Set RestingResistance = 1e6
    Channel->>Channel: ABuild()
    Channel->>Synapses: Connect synapses
    loop Each step
        Synapses->>Channel: PreOutput signals
        Storage->>Channel: Calculate()
        Channel->>Channel: Process synapses
        Channel->>Channel: Apply Type = 1 (inhibitory)
        Channel->>Channel: Apply mediator dynamics
        Channel->>Channel: Calculate output current
        Channel->>LTZone: Output (decreases LT-zone potential)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Defaulted --> SetType: Set Type = 1
    SetType --> SetParams: Set LT-zone parameters
    SetParams --> Building: Build()
    Building --> Connecting: Connect synapses
    Connecting --> Built: Structure built
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> ProcessSynapses: Process synapse inputs
    ProcessSynapses --> ApplyInhibitory: Apply Type = 1 (inhibitory)
    ApplyInhibitory --> ApplyMediator: Apply mediator dynamics
    ApplyMediator --> CalculateOutput: Calculate output current
    CalculateOutput --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> ReceiveInputs[Receive PreOutput from synapses]
    ReceiveInputs --> AggregateInputs[Aggregate synapse inputs]
    AggregateInputs --> ApplyType["Apply Type = 1<br/>Inhibitory coefficient"]
    ApplyType --> ProcessMediator["Process mediator dynamics<br/>SecretionTC, DissociationTC"]
    ProcessMediator --> CalculateCurrent["Calculate output current<br/>Decreases LT-zone potential"]
    CalculateCurrent --> IntegrateOutput["Integrate with LT-zone parameters<br/>Capacity=1e-8, RestingResistance=1e6"]
    IntegrateOutput --> SetOutput[Set output]
    SetOutput --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPLTSynChannel["NPLTSynChannel Base"]
        BaseChannel[NPLTSynChannel]
    end
    
    subgraph NPLTSynInhChannel["NPLTSynInhChannel Configuration"]
        InhibitoryType["Inhibitory Type<br/>Type = 1"]
        LTParams["LT-zone Parameters<br/>Capacity=1e-8, RestingResistance=1e6"]
        SynapseProcessing["Synapse Processing<br/>Mediator dynamics"]
    end
    
    subgraph External["External Components"]
        Synapses["NPulseSynapse<br/>Synapses"]
        LTZone[NPulseLTZoneCommon]
    end
    
    BaseChannel -->|configured as| NPLTSynInhChannel
    NPLTSynInhChannel -->|uses| InhibitoryType
    NPLTSynInhChannel -->|uses| LTParams
    Synapses -->|PreOutput| NPLTSynInhChannel
    NPLTSynInhChannel -->|Output<br/>decreases LT-zone potential| LTZone
```

### Properties

`NPLTSynInhChannel` uses all properties of base class `NPLTSynChannel` with preset value:

**Parameters:**
- `Type = 1` — inhibitory channel
- `Capacity = 1e-8` (10 nF) — increased capacitance for LT-zone
- `RestingResistance = 1e6` (1 MΩ) — resting resistance for LT-zone

**Other default parameters:**
- `PulseAmplitude = 1.0`
- `SecretionTC = 0.001` (1 ms)
- `DissociationTC = 0.01` (10 ms)
- `SynapseResistance = 1.0e8` (100 MΩ)
- `Resistance = 1.0e7` (10 MΩ)
- `FBResistance = 1.0e8` (100 MΩ)

### Methods

`NPLTSynInhChannel` uses all methods of base class `NPLTSynChannel`.

### Usage in configurations

`NPLTSynInhChannel` is used in experiments with inhibitory synaptic channels in LT-zones:

- **Inhibitory LT-synaptic channels**: Modeling inhibitory channels in LT-zones
- **Inhibitory LT plasticity**: Experiments with inhibitory LT plasticity
- **Spike suppression**: Studying suppression of spike generation in LT-zones

**Features:**
- Automatically configured as inhibitory channel (`Type = 1`)
- Decreases LT-zone potential when input signals from synapses are present
- Combines synapse processing with LT-zone parameters

**Typical parameter values:**
- **Type**: 1 (inhibitory channel type)
- **Capacity**: 1e-8 (10 nF) — increased for LT-zone
- **RestingResistance**: 1e6 (1 MΩ) — decreased for LT-zone

### References

See [Literature-References.md](../Literature-References.md): **[A]**, **4**, **25**, **29**.

### See Also

- [`NPLTSynChannel`](NPLTSynChannel.md) — LT-synaptic channel (base class)
- [`NPLTSynExcChannel`](NPLTSynExcChannel.md) — excitatory LT-synaptic channel
- [`NPulseSynChannel`](NPulseSynChannel.md) — synaptic spiking channel
- [`NPLTChannel`](NPLTChannel.md) — LT-channel
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — common spiking LT-zone
- [Architecture.md](../Architecture.md) — library architecture

