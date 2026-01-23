# NPLTExcChannel — возбуждающий LT-канал

## RU

### Назначение

**Класс**: `NPLTExcChannel` — конфигурационный вариант LT-канала с типом возбуждающего канала.  
**Аббревиатуры**: `LT` — **L**ow **T**hreshold (низкопороговая зона); `Exc` — **Exc**itatory (возбуждающий).  
**Регистрация**: `NPulseLibrary.cpp` → `UploadClass("NPLTExcChannel", ...)`.  
**Storage-инстансы**: `ClassName = "NPLTExcChannel"` в `Bin/Configs/*/Model_*.xml`.

`NPLTExcChannel` является конфигурационным вариантом базового класса `NPLTChannel` с предустановленным типом возбуждающего канала. Создается из `NPLTChannel` с настройкой:
- `Type = -1` — возбуждающий канал

Возбуждающий LT-канал увеличивает потенциал LT-зоны при наличии входных сигналов, способствуя генерации спайков.

**Использование:** Моделирование возбуждающих LT-каналов, эксперименты с возбуждающей LT-пластичностью

### UML-диаграмма классов

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|.. NPLTChannel : configuration variant
    NPLTChannel <|.. NPLTExcChannel : configuration variant
    NPLTExcChannel ..> NPulseLTZoneCommon : используется в
    class NPLTChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
    class NPLTExcChannel {
        +Type : double = -1
    }
```

**Иерархия наследования:**
- `NPulseChannelCommon` — общий импульсный канал
- `NPulseChannel` — базовый импульсный канал
- `NPLTChannel` — конфигурационный вариант для LT-зоны
- `NPLTExcChannel` — конфигурационный вариант для возбуждающего LT-канала

### Свойства

`NPLTExcChannel` использует все свойства базового класса `NPLTChannel` с предустановленным значением:

**Параметры:**
- `Type = -1` — возбуждающий канал
- `Capacity = 1e-8` (10 нФ) — увеличенная емкость для LT-зоны
- `RestingResistance = 1e6` (1 МОм) — сопротивление покоя для LT-зоны

**Остальные параметры по умолчанию:**
- `Resistance = 1.0e7` (10 МОм)
- `FBResistance = 1.0e8` (100 МОм)

### Методы

`NPLTExcChannel` использует все методы базового класса `NPLTChannel`.

### Примеры использования

#### Пример 1: Создание канала в коде C++

```cpp
// Создание возбуждающего LT-канала
auto channel = storage->CreateComponent("NPLTExcChannel");
channel->SetName("LTExcChannel");

// Инициализация (использует Type = -1, Capacity = 1e-8, RestingResistance = 1e6)
channel->Default();

// Использование
channel->Build();
```

#### Пример 2: Конфигурация XML

```xml
<Channel1 Class="NPLTExcChannel">
    <Parameters>
        <Type>-1</Type>
        <Capacity>1e-8</Capacity>
        <Resistance>1.0e7</Resistance>
        <FBResistance>1.0e8</FBResistance>
        <RestingResistance>1e6</RestingResistance>
    </Parameters>
</Channel1>
```

### Использование в конфигурациях

`NPLTExcChannel` используется в экспериментах с возбуждающими LT-каналами:

- Моделирование возбуждающих LT-каналов
- Эксперименты с возбуждающей LT-пластичностью
- Изучение генерации спайков в LT-зонах

**Особенности:**
- Автоматически настроен как возбуждающий канал (`Type = -1`)
- Увеличивает потенциал LT-зоны при наличии входных сигналов
- Использует параметры, оптимизированные для LT-зон

### См. также

- [`NPLTChannel`](NPLTChannel.md) — LT-канал (базовый класс)
- [`NPLTInhChannel`](NPLTInhChannel.md) — тормозной LT-канал
- [`NPLTSynChannel`](NPLTSynChannel.md) — LT-синаптический канал
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — общая импульсная LT-зона
- [`NPulseChannel`](NPulseChannel.md) — базовый импульсный канал
- [Architecture.md](../Architecture.md) — архитектура библиотеки

---

## EN

### Purpose

**Class**: `NPLTExcChannel` — configuration variant of LT-channel with excitatory channel type.  
**Registration**: `NPulseLibrary.cpp` → `UploadClass("NPLTExcChannel", ...)`.  
**Instances**: `ClassName = "NPLTExcChannel"` in `Bin/Configs/*/Model_*.xml`.

`NPLTExcChannel` is a configuration variant of the base class `NPLTChannel` with preset excitatory channel type. Created from `NPLTChannel` with setting:
- `Type = -1` — excitatory channel

Excitatory LT-channel increases LT-zone potential when input signals are present, promoting spike generation.

**Usage:** Modeling excitatory LT-channels, experiments with excitatory LT plasticity

### UML Class Diagram

```mermaid
classDiagram
    NPulseChannelCommon <|-- NPulseChannel
    NPulseChannel <|.. NPLTChannel : configuration variant
    NPLTChannel <|.. NPLTExcChannel : configuration variant
    NPLTExcChannel ..> NPulseLTZoneCommon : used in
    class NPLTChannel {
        +Capacity : double = 1e-8
        +RestingResistance : double = 1e6
    }
    class NPLTExcChannel {
        +Type : double = -1
    }
```

### UML Sequence Diagram

```mermaid
sequenceDiagram
    participant Storage
    participant Channel as NPLTExcChannel
    participant LTZone as NPulseLTZoneCommon
    
    Storage->>Channel: New() + Default()
    Note over Channel: Type = -1<br/>Capacity = 1e-8<br/>RestingResistance = 1e6
    Storage->>Channel: Build()
    loop Each step
        Storage->>Channel: Calculate()
        Channel->>Channel: ACalculate()
        Channel->>Channel: Calculate excitatory current
        Channel-->>LTZone: Output (increases potential)
    end
```

### UML State Diagram

```mermaid
stateDiagram-v2
    [*] --> Uninitialized: New()
    Uninitialized --> Defaulted: Default()
    Note over Defaulted: Type = -1<br/>Capacity = 1e-8<br/>RestingResistance = 1e6
    Defaulted --> Built: Build()
    Built --> Ready: Ready = true
    Ready --> Calculating: Calculate()
    Calculating --> CalcExcCurrent: Calculate excitatory current
    CalcExcCurrent --> Ready: Step completed
    Ready --> Resetting: Reset()
    Resetting --> Ready
```

### UML Activity Diagram

```mermaid
flowchart TD
    Start([Start Calculate]) --> AggregateInputs[Aggregate inputs]
    AggregateInputs --> CalcExcCurrent[Calculate excitatory current]
    Note over CalcExcCurrent: Type = -1<br/>Capacity = 1e-8<br/>RestingResistance = 1e6
    CalcExcCurrent --> IncreasePotential[Increase LT-zone potential]
    IncreasePotential --> End([End])
```

### UML Component Diagram

```mermaid
graph TB
    subgraph NPLTChannel["NPLTChannel Base"]
        BaseChannel[NPLTChannel]
    end
    
    subgraph NPLTExcChannel["NPLTExcChannel Configuration"]
        ExcType[Excitatory Type<br/>Type = -1]
        LTParams[LT-zone parameters]
    end
    
    subgraph External["External Components"]
        LTZone[NPulseLTZoneCommon]
    end
    
    BaseChannel -->|configured as| NPLTExcChannel
    NPLTExcChannel -->|uses| ExcType
    NPLTExcChannel -->|uses| LTParams
    NPLTExcChannel -->|Output<br/>increases potential| LTZone
```

### Properties

`NPLTExcChannel` uses all properties of base class `NPLTChannel` with preset value:

**Parameters:**
- `Type = -1` — excitatory channel
- `Capacity = 1e-8` (10 nF) — increased capacitance for LT-zone
- `RestingResistance = 1e6` (1 MΩ) — resting resistance for LT-zone

**Other default parameters:**
- `Resistance = 1.0e7` (10 MΩ)
- `FBResistance = 1.0e8` (100 MΩ)

### Methods

`NPLTExcChannel` uses all methods of base class `NPLTChannel`.

### Usage in configurations

`NPLTExcChannel` is used in experiments with excitatory LT-channels:

- **Excitatory LT-channels**: Modeling excitatory channels in LT-zones
- **Excitatory LT plasticity**: Experiments with excitatory LT plasticity
- **Spike generation**: Studying spike generation in LT-zones

**Features:**
- Automatically configured as excitatory channel (`Type = -1`)
- Increases LT-zone potential when input signals are present
- Uses parameters optimized for LT-zones

**Typical parameter values:**
- **Type**: -1 (excitatory channel type)
- **Capacity**: 1e-8 (10 nF) — increased for LT-zone
- **RestingResistance**: 1e6 (1 MΩ) — decreased for LT-zone

### See Also

- [`NPLTChannel`](NPLTChannel.md) — LT-channel (base class)
- [`NPLTInhChannel`](NPLTInhChannel.md) — inhibitory LT-channel
- [`NPLTSynChannel`](NPLTSynChannel.md) — LT-synaptic channel
- [`NPulseLTZoneCommon`](NPulseLTZoneCommon.md) — common spiking LT-zone
- [`NPulseChannel`](NPulseChannel.md) — base spiking channel
- [Architecture.md](../Architecture.md) — library architecture

